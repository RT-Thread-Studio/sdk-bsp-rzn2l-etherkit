/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"

/** ELC event definitions. */

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define BSP_IRQ_UINT32_MAX                       (0xFFFFFFFFU)

#define BSP_PRV_CLEAR_REG_MAX                    (13U)
#define BSP_PRV_ALL_BIT_CLEAR                    (0xFFFFFFFFU)

#define BSP_PRV_ID_MASK                          (0x000003FFU)
#define BSP_PRV_INTERRUPTABLE_NUM                (32U)

#define BSP_PRV_ICFGR_EVENT_TYPE_SETTING_MASK    (0x3U)
#define BSP_PRV_ICFGR_EDGE_DETECTION_TYPE        (0x2U)
#define BSP_PRV_ICFGR_REG_FIELD_NUM              (16U)
#define BSP_PRV_ICFGR_FIELD_WIDTH_BITS           (2U)

#define BSP_PRV_CR52_INTSEL_INIT                 (0x03FF03FFUL)

#define R_BSP_GIC_INTID_1023                     (1023)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/
#if VECTOR_DATA_IRQ_COUNT > 0
extern fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES];
#endif
extern fsp_vector_t g_sgi_ppi_vector_table[BSP_CORTEX_VECTOR_TABLE_ENTRIES];

#if (0 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)
extern const uint32_t BSP_GICD_ICFGR_INIT[BSP_NON_SELECTABLE_ICFGR_MAX];

#elif (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)
extern const uint32_t BSP_GICD_ICFGR_INIT[BSP_EVENT_ARRAY_NUM];

#endif

/* This table is used to store the context in the ISR. */
void * gp_renesas_isr_context[BSP_ICU_VECTOR_MAX_ENTRIES + BSP_CORTEX_VECTOR_TABLE_ENTRIES];

/* GIC current interrupt ID and variable. */
IRQn_Type g_current_interrupt_num[BSP_PRV_INTERRUPTABLE_NUM];
uint8_t   g_current_interrupt_pointer = 0;

const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_WEAK_REFERENCE =
{
    (bsp_interrupt_event_t) 0
};

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
#if (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)

/*******************************************************************************************************************//**
 * Set the events to CR52_SEL. Events can be set through the configurator.
 *
 **********************************************************************************************************************/
static void bsp_prv_cr52_sel_cfg (void)
{
    for (uint32_t intsel_reg_num = 0;
         intsel_reg_num < BSP_FEATURE_BSP_SELECTABLE_INTERRUPT_EVENT_NUM / 2U;
         intsel_reg_num++)
    {
        /* Get interrupt number of SELECT0_IRQn */
        uint32_t select0_irqx = (2U * intsel_reg_num) + BSP_FEATURE_BSP_SELECTABLE_INTERRUPT_START;
        uint32_t select0_irqy = (2U * intsel_reg_num + 1U) + BSP_FEATURE_BSP_SELECTABLE_INTERRUPT_START;

        /* Get user-setting event number from vector_data.c */
        uint32_t intselx = g_interrupt_event_link_select[select0_irqx];
        uint32_t intsely = g_interrupt_event_link_select[select0_irqy];

        uint32_t intseln = BSP_PRV_CR52_INTSEL_INIT;

        /* 0 in g_interrupt_event_link_select indicates that either INTCPU0 event is registered
         * or no event is registered. INTCPU0 is not a target of selected interrupt
         * and does not appear as a value after 384 in the array.
         * Therefore, 0 means that user registered no event in SELECT0_IRQn with FSP configurator. */
        if (intselx != 0U)             // Set INTSELX of CR520_INTSELn register
        {
            intseln &= (intselx << R_ICU_NS_CR520_INTSEL_INTSELX_Pos) | R_ICU_NS_CR520_INTSEL_INTSELY_Msk;
        }

        if (intsely != 0U)             // Set INTSELY of CR520_INTSELn register
        {
            intseln &= (intsely << R_ICU_NS_CR520_INTSEL_INTSELY_Pos) | R_ICU_NS_CR520_INTSEL_INTSELX_Msk;
        }

 #if (0 == BSP_CFG_CORE_CR52)
        R_ICU_NS->CR520_INTSEL[intsel_reg_num] = intseln;
 #elif (1 == BSP_CFG_CORE_CR52)
        R_ICU_NS->CR521_INTSEL[intsel_reg_num] = intseln;
 #endif
    }
}

#endif

#if (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)

/*******************************************************************************************************************//**
 * Identifies the event interrupt type of selected interrupts to be set in GICD_ICFGR.
 *
 * @param[in]  gicd_reg_num    Register number of GICD_ICFGR.
 **********************************************************************************************************************/
static uint32_t bsp_prv_gicd_icfgr_set (uint32_t gicd_reg_num)
{
    uint32_t gicd_reg_icfgr = 0;

    /* GICD_ICFGR sets 16 events per register */
    for (uint32_t field_num = 0; field_num < BSP_PRV_ICFGR_REG_FIELD_NUM; field_num++)
    {
        bsp_interrupt_event_t event =
            g_interrupt_event_link_select[gicd_reg_num * BSP_PRV_ICFGR_REG_FIELD_NUM + field_num];

        /* Identify the location in BSP_GICD_ICFGR_INIT where type of the event is stored */
        uint32_t event_type_array_num   = event / BSP_PRV_ICFGR_REG_FIELD_NUM;
        uint32_t event_type_array_shift = (event % BSP_PRV_ICFGR_REG_FIELD_NUM) * BSP_PRV_ICFGR_FIELD_WIDTH_BITS;

        /* 0 in g_interrupt_event_link_select indicates that either INTCPU0 event is registered
         * or no event is registered. INTCPU0 is not a target of selected interrupt
         * and does not appear as a value after 384 in the array.
         * Therefore, 0 means that user registered no event in SELECT0_IRQn with FSP configurator. */
        if (event != 0)
        {
            gicd_reg_icfgr |=
                ((BSP_GICD_ICFGR_INIT[event_type_array_num] >> event_type_array_shift) &
                 BSP_PRV_ICFGR_EVENT_TYPE_SETTING_MASK) <<
                    field_num * BSP_PRV_ICFGR_FIELD_WIDTH_BITS;
        }
        else
        {
            /* SELECT0_IRQn is level/edge type. Therefore, interrupt type is initialized by edge. */
            gicd_reg_icfgr |= (BSP_PRV_ICFGR_EDGE_DETECTION_TYPE & BSP_PRV_ICFGR_EVENT_TYPE_SETTING_MASK) <<
                              field_num * BSP_PRV_ICFGR_FIELD_WIDTH_BITS;
        }
    }

    return gicd_reg_icfgr;
}

#endif

/*******************************************************************************************************************//**
 *        Using the vector table information section that has been built by the linker and placed into ROM in the
 * .vector_info. section, this function will initialize the ICU so that configured ELC events will trigger interrupts
 * in the NVIC.
 *
 **********************************************************************************************************************/
void bsp_irq_core_cfg (void)
{
    uint32_t                   gicd_reg_num;
    GICD_Type                * GICD;
    GICR_CONTROL_TARGET_Type * GICR_TARGET0_IFREG;

    GICD               = BSP_PRV_GICD_ADDRESS;
    GICR_TARGET0_IFREG = BSP_PRV_GICR_TARGET0_IFREG_ADDRESS;

#if (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)

    /* Set events to CR520_INTSELn */
    bsp_prv_cr52_sel_cfg();
#endif

    /* Enable Group1 interrupts from the GIC Distributor to the GIC CPU interface. */
    GICD->GICD_CTLR |= 0x00000002UL;

    /* Release Processor Sleep state of the target. */
    GICR_TARGET0_IFREG->GICR_WAKER = 0x00000000UL;

    /* Initialize GICD_ICFGR register for the edge-triggered interrupt. */
    for (gicd_reg_num = 0; gicd_reg_num < BSP_NON_SELECTABLE_ICFGR_MAX; gicd_reg_num++)
    {
        GICD->GICD_ICFGR[gicd_reg_num] = BSP_GICD_ICFGR_INIT[gicd_reg_num];
    }

#if (1 == BSP_FEATURE_BSP_IRQ_CR52_SEL_SUPPORTED)

    /* Initialize GICD_ICFGR register of SELECT0_IRQn interrupt. */
    for (gicd_reg_num = BSP_NON_SELECTABLE_ICFGR_MAX; gicd_reg_num < BSP_ICFGR_MAX; gicd_reg_num++)
    {
        GICD->GICD_ICFGR[gicd_reg_num] = bsp_prv_gicd_icfgr_set(gicd_reg_num);
    }
#endif

    /* Clear the Pending and Active bit for the all interrupts. */
    for (gicd_reg_num = 0; gicd_reg_num < BSP_PRV_CLEAR_REG_MAX; gicd_reg_num++)
    {
        GICD->GICD_ICPENDR[gicd_reg_num]   = BSP_PRV_ALL_BIT_CLEAR;
        GICD->GICD_ICACTIVER[gicd_reg_num] = BSP_PRV_ALL_BIT_CLEAR;
    }

    __asm volatile ("cpsie i    \n"    /* Enable IRQ Interrupts */
                    "cpsie f    \n"    /* Enable FIQ Interrupts */
                    "cpsie a    \n"    /* Enable SError Interrupts */
                    "isb");            /* Ensuring Context-changing */
}

/*******************************************************************************************************************//**
 * This function is called first when an interrupt is generated and branches to each interrupt isr function.
 *
 * @param[in]  id    GIC INTID used to identify the interrupt.
 **********************************************************************************************************************/
void bsp_common_interrupt_handler (uint32_t id)
{
    uint16_t  gic_intid;
    IRQn_Type irq;

    /* Get interrupt ID (GIC INTID). */
    gic_intid = (uint16_t) (id & BSP_PRV_ID_MASK);

    irq = (IRQn_Type) (gic_intid - BSP_CORTEX_VECTOR_TABLE_ENTRIES);

    FSP_ASSERT_NOT_RETURN_VALUE(R_BSP_GIC_INTID_1023 != gic_intid);

    /* Remain the interrupt number */
    g_current_interrupt_num[g_current_interrupt_pointer++] = irq;
    __asm volatile ("dmb");

    BSP_CFG_TFU_SAVE_ENABLE;
    BSP_CFG_MULTIPLEX_INTERRUPT_ENABLE;

#if VECTOR_DATA_IRQ_COUNT > 0
    if (BSP_CORTEX_VECTOR_TABLE_ENTRIES <= gic_intid)
    {
        /* Branch to an interrupt handler. */
        g_vector_table[irq]();
    }
    else
#endif
    {
        /* Branch to an interrupt handler. */
        g_sgi_ppi_vector_table[gic_intid]();
    }

    BSP_CFG_MULTIPLEX_INTERRUPT_DISABLE;
    BSP_CFG_TFU_RESTORE_ENABLE;

    g_current_interrupt_pointer--;
}
