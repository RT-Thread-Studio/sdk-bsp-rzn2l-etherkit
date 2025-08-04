/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_IRQ_H
#define BSP_IRQ_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

#if defined(BSP_CFG_CORE_CR52)
 #include "cr/bsp_irq_core.h"
#elif defined(BSP_CFG_CORE_CA55)
 #include "ca/bsp_irq_core.h"
#endif

#include "fsp_features.h"

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define BSP_IRQ_GPT_EVENT_NUM_BASE                  ((IRQn_Type) ELC_EVENT_GPT00_0_INT0) // IRQ number of GPT_INT00_0.
#define BSP_IRQ_GPT_INT_NUM_MAX                     (5U)                                 // Total size from GPT_INT0 to GPT_INT4.
#define BSP_IRQ_GPT_REGISTER_DIVISION               (2U)                                 // Divide GPT channel number.
#define BSP_IRQ_GPT_IY_SHIFT_NUM                    (16U)                                // Upper bits shift of GPT_INTSELn.
#define BSP_IRQ_GPT_IX_SHIFT_NUM                    (0U)                                 // Low bits shift of GPT_INTSELn.

#define BSP_IRQ_GPT_SELECTED_INTERVAL               (4U)                                 // Bit interval of GPT_INTSELn.
#define BSP_IRQ_GPT_SELECTED_EVENT_MASK             (0xFU)                               // Bits mask of GPT_INTSELn.

#define BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP    (32)                                 // 32bit state bit per GPT_INTSTATn and so on.
#define BSP_IRQ_GPT_COMBINED_GROUP_NUM              (28)                                 // NS_GPT_INTSTAT0 to 25 and S_GPT_INTSTAT0 to 1.
#define BSP_IRQ_GPT_COMBINED_EVENT_MASK             (0x1U)                               // Bit mask of GPT_INTMSKn
#define BSP_IRQ_GPT_COMBINED_EVENT_ENUM_MASK        (0xFU)                               // Bit mask of bsp_irq_gpt_combined_event_t

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

#ifndef BSP_OVERRIDE_BSP_IRQ_GPT_SELECTED_EVENT_T

/** GPT selected interrupt event */
typedef enum e_bsp_irq_gpt_selected_event
{
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_CCMPA = 0x0, ///< GPT_CCMPA event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_CCMPB = 0x1, ///< GPT_CCMPB event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_CMPC  = 0x2, ///< GPT_CMPC event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_CMPD  = 0x3, ///< GPT_CMPD event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_CMPE  = 0x4, ///< GPT_CMPE event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_CMPF  = 0x5, ///< GPT_CMPF event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_OVF   = 0x6, ///< GPT_OVF event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_UDF   = 0x7, ///< GPT_UDF event source
    BSP_IRQ_GPT_SELECTED_EVENT_GPT_DTE   = 0x8, ///< GPT_DTE event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA0     = 0x9, ///< MTU3 TGIA0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB0     = 0xA, ///< MTU3 TGIB0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIC0     = 0xB, ///< MTU3 TGIC0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGID0     = 0xC, ///< MTU3 TGID0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV0     = 0xD, ///< MTU3 TCIV0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIE0     = 0x9, ///< MTU3 TGIE0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIF0     = 0xA, ///< MTU3 TGIF0 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA1     = 0xB, ///< MTU3 TGIA1 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB1     = 0xC, ///< MTU3 TGIB1 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV1     = 0xD, ///< MTU3 TCIV1 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIU1     = 0x9, ///< MTU3 TCIU1 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA2     = 0xA, ///< MTU3 TGIA2 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB2     = 0xB, ///< MTU3 TGIB2 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV2     = 0xC, ///< MTU3 TCIV2 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIU2     = 0xD, ///< MTU3 TCIU2 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA3     = 0x9, ///< MTU3 TGIA3 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB3     = 0xA, ///< MTU3 TGIB3 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIC3     = 0xB, ///< MTU3 TGIC3 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGID3     = 0xC, ///< MTU3 TGID3 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV3     = 0xD, ///< MTU3 TCIV3 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA4     = 0x9, ///< MTU3 TGIA4 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB4     = 0xA, ///< MTU3 TGIB4 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIC4     = 0xB, ///< MTU3 TGIC4 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGID4     = 0xC, ///< MTU3 TGID4 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV4     = 0xD, ///< MTU3 TCIV4 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIU5     = 0x9, ///< MTU3 TCIU5 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV5     = 0xA, ///< MTU3 TCIV5 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIW5     = 0xB, ///< MTU3 TGIW5 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA6     = 0x9, ///< MTU3 TGIA6 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB6     = 0xA, ///< MTU3 TGIB6 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIC6     = 0xB, ///< MTU3 TGIC6 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGID6     = 0xC, ///< MTU3 TGID6 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV6     = 0xD, ///< MTU3 TCIV6 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA7     = 0x9, ///< MTU3 TGIA7 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB7     = 0xA, ///< MTU3 TGIB7 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIC7     = 0xB, ///< MTU3 TGIC7 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGID7     = 0xC, ///< MTU3 TGID7 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV7     = 0xD, ///< MTU3 TCIV7 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIA8     = 0x9, ///< MTU3 TGIA8 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIB8     = 0xA, ///< MTU3 TGIB8 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGIC8     = 0xB, ///< MTU3 TGIC8 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TGID8     = 0xC, ///< MTU3 TGID8 event source
    BSP_IRQ_GPT_SELECTED_EVENT_TCIV8     = 0xD, ///< MTU3 TCIV8 event source
} bsp_irq_gpt_selected_event_t;

#endif

#ifndef BSP_OVERRIDE_BSP_IRQ_GPT_COMBINED_EVENT_T

/** GPT combined interrupt event */
typedef enum e_bsp_irq_gpt_combined_event
{
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_CCMPA = 0x0, ///< GPT_CCMPA event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_CCMPB = 0x1, ///< GPT_CCMPB event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_CMPC  = 0x2, ///< GPT_CMPC event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_CMPD  = 0x3, ///< GPT_CMPD event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_CMPE  = 0x4, ///< GPT_CMPE event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_CMPF  = 0x5, ///< GPT_CMPF event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_OVF   = 0x6, ///< GPT_OVF event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_UDF   = 0x7, ///< GPT_UDF event source
    BSP_IRQ_GPT_COMBINED_EVENT_GPT_DTE   = 0x8, ///< GPT_DTE event source
    BSP_IRQ_GPT_COMBINED_EVENT_MTU3_0    = 0x9, ///< MTU3 event source 0
    BSP_IRQ_GPT_COMBINED_EVENT_MTU3_1    = 0xA, ///< MTU3 event source 1
    BSP_IRQ_GPT_COMBINED_EVENT_MTU3_2    = 0xB, ///< MTU3 event source 2
    BSP_IRQ_GPT_COMBINED_EVENT_MTU3_3    = 0xC, ///< MTU3 event source 3
    BSP_IRQ_GPT_COMBINED_EVENT_MTU3_4    = 0xD, ///< MTU3 event source 4
    BSP_IRQ_GPT_COMBINED_EVENT_RESERVED1 = 0xE, ///< Reserved bit
    BSP_IRQ_GPT_COMBINED_EVENT_RESERVED2 = 0xF, ///< Reserved bit
} bsp_irq_gpt_combined_event_t;

#endif

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern void ** gp_bsp_gpt_combined_ctrl_table[BSP_IRQ_GPT_COMBINED_GROUP_NUM];
extern fsp_vector_t * gp_bsp_gpt_combined_isr_table[BSP_IRQ_GPT_COMBINED_GROUP_NUM];

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)
uint32_t bsp_prv_irq_gpt_channel_get(IRQn_Type irq);
uint32_t bsp_prv_irq_gpt_reg_num_get(uint32_t channel);
uint32_t bsp_prv_irq_gpt_selected_shift_num_get(IRQn_Type irq, uint32_t channel);
uint32_t bsp_prv_irq_gpt_combined_shift_num_get(uint32_t channel, bsp_irq_gpt_combined_event_t event_source);
void   * bsp_prv_irq_gpt_combined_ctrl_table_allocate(uint32_t groupIndex);
void   * bsp_prv_irq_gpt_combined_isr_table_allocate(uint32_t groupIndex);
bool     bsp_prv_irq_gpt_combined_table_status_get(uintptr_t * p_table, uint32_t size);
uint32_t bsp_prv_irq_gpt_combined_table_num_get(uint32_t channel);
void     bsp_irq_gpt_combined_interrupt_handler(void);

#endif

/***********************************************************************************************************************
 * Inline Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Sets the ISR context associated with the requested IRQ.
 *
 * @param[in]  irq            IRQ number (parameter checking must ensure the IRQ number is valid before calling this
 *                            function.
 * @param[in]  p_context      ISR context for IRQ.
 **********************************************************************************************************************/
__STATIC_INLINE void R_FSP_IsrContextSet (IRQn_Type const irq, void * p_context)
{
    /* This provides access to the ISR context array defined in bsp_irq.c. This is an inline function instead of
     * being part of bsp_irq.c for performance considerations because it is used in interrupt service routines. */
    r_fsp_irq_context_set(irq, p_context);
}

/*******************************************************************************************************************//**
 * Clear the GIC pending interrupt.
 *
 * @param[in] irq            Interrupt for which to clear the Pending bit. Note that the enums listed for IRQn_Type are
 *                           only those for the Cortex Processor Exceptions Numbers.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqClearPending (IRQn_Type irq)
{
    r_bsp_irq_clear_pending(irq);
}

/*******************************************************************************************************************//**
 * Get the GIC pending interrupt.
 *
 * @param[in] irq            Interrupt that gets a pending bit.. Note that the enums listed for IRQn_Type are
 *                           only those for the Cortex Processor Exceptions Numbers.
 *
 * @return  Value indicating the status of the level interrupt.
 **********************************************************************************************************************/
__STATIC_INLINE uint32_t R_BSP_IrqPendingGet (IRQn_Type irq)
{
    return r_bsp_irq_pending_get(irq);
}

/*******************************************************************************************************************//**
 * Sets the interrupt priority and context.
 *
 * @param[in] irq            The IRQ number to configure.
 * @param[in] priority       GIC priority of the interrupt
 * @param[in] p_context      The interrupt context is a pointer to data required in the ISR.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqCfg (IRQn_Type const irq, uint32_t priority, void * p_context)
{
    r_bsp_irq_cfg(irq, priority);

    /* Store the context. The context is recovered in the ISR. */
    R_FSP_IsrContextSet(irq, p_context);
}

/*******************************************************************************************************************//**
 * Enable the IRQ in the GIC (Without clearing the pending bit).
 *
 * @param[in] irq            The IRQ number to enable. Note that the enums listed for IRQn_Type are only those for the
 *                           Cortex Processor Exceptions Numbers.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqEnableNoClear (IRQn_Type const irq)
{
    r_bsp_irq_enable_no_clear(irq);
}

/*******************************************************************************************************************//**
 * Enable the IRQ in the GIC (With clearing the pending bit).
 *
 * @param[in] irq            The IRQ number to enable. Note that the enums listed for IRQn_Type are only those for the
 *                           Cortex Processor Exceptions Numbers.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqEnable (IRQn_Type const irq)
{
    /* Clear pending interrupts in the GIC. */
    R_BSP_IrqClearPending(irq);

    /* Enable the interrupt in the GIC. */
    R_BSP_IrqEnableNoClear(irq);
}

/*******************************************************************************************************************//**
 * Disables interrupts in the GIC.
 *
 * @param[in] irq            The IRQ number to disable in the GIC. Note that the enums listed for IRQn_Type are
 *                           only those for the Cortex Processor Exceptions Numbers.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqDisable (IRQn_Type const irq)
{
    r_bsp_irq_disable(irq);
}

/*******************************************************************************************************************//**
 * Sets the interrupt priority and context, clears pending interrupts, then enables the interrupt.
 *
 * @param[in] irq            Interrupt number.
 * @param[in] priority       GIC priority of the interrupt
 * @param[in] p_context      The interrupt context is a pointer to data required in the ISR.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqCfgEnable (IRQn_Type const irq, uint32_t priority, void * p_context)
{
    R_BSP_IrqCfg(irq, priority, p_context);
    R_BSP_IrqEnable(irq);
}

/*******************************************************************************************************************//**
 * @brief      Finds the ISR context associated with the requested IRQ.
 *
 * @param[in]  irq            IRQ number (parameter checking must ensure the IRQ number is valid before calling this
 *                            function.
 * @return  ISR context for IRQ.
 **********************************************************************************************************************/
__STATIC_INLINE void * R_FSP_IsrContextGet (IRQn_Type const irq)
{

    /* This provides access to the ISR context array defined in bsp_irq.c. This is an inline function instead of
     * being part of bsp_irq.c for performance considerations because it is used in interrupt service routines. */
    return gp_renesas_isr_context[irq + BSP_VECTOR_NUM_OFFSET];
}

/*******************************************************************************************************************//**
 * Sets the interrupt detect type.
 *
 * @param[in] irq             The IRQ number to configure.
 * @param[in] detect_type     GIC detect type of the interrupt (0 : active-HIGH level, 1 : rising edge-triggerd).
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqDetectTypeSet (IRQn_Type const irq, uint32_t detect_type)
{
    r_bsp_irq_detect_type_set(irq, detect_type);
}

/*******************************************************************************************************************//**
 * Sets the interrupt Group.
 *
 * @param[in] irq               The IRQ number to configure.
 * @param[in] interrupt_group   GIC interrupt group number ( 0 : FIQ, 1 : IRQ ).
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGroupSet (IRQn_Type const irq, uint32_t interrupt_group)
{
    r_bsp_irq_group_set(irq, interrupt_group);
}

/*******************************************************************************************************************//**
 * Sets the interrupt mask level.
 *
 * @param[in] mask_level          The interrupt mask level
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqMaskLevelSet (uint32_t mask_level)
{
    FSP_CRITICAL_SECTION_SET_STATE(mask_level << BSP_FEATURE_BSP_IRQ_PRIORITY_POS_BIT);
}

/*******************************************************************************************************************//**
 * Gets the interrupt mask level.
 *
 * @return  Value indicating the interrupt mask level.
 **********************************************************************************************************************/
__STATIC_INLINE uint32_t R_BSP_IrqMaskLevelGet (void)
{
    return (uint32_t) ((FSP_CRITICAL_SECTION_GET_CURRENT_STATE() >> BSP_FEATURE_BSP_IRQ_PRIORITY_POS_BIT) &
                       0x0000001FUL);
}

#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

/*******************************************************************************************************************//**
 * Manipulate GPT_INTSELn and specify the GPT/MTU3 interrupt source as GPT_INT0-3.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the INTSEL bit value of GPT_INTSELn as an enum
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGptSelectedSet (IRQn_Type irq, bsp_irq_gpt_selected_event_t event_source)
{
    if (irq >= BSP_IRQ_GPT_EVENT_NUM_BASE)
    {
        uint32_t channel   = bsp_prv_irq_gpt_channel_get(irq);
        uint32_t reg_num   = bsp_prv_irq_gpt_reg_num_get(channel);
        uint32_t shift_num = bsp_prv_irq_gpt_selected_shift_num_get(irq, channel);

        if (channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
        {
            R_ICU_NS->NS_GPT_INTSEL[reg_num] &= ~(BSP_IRQ_GPT_SELECTED_EVENT_MASK << shift_num);
            R_ICU_NS->NS_GPT_INTSEL[reg_num] |= (uint32_t) event_source << shift_num;
        }
        else
        {
            R_ICU_S->S_GPT_INTSEL[reg_num] &= ~(BSP_IRQ_GPT_SELECTED_EVENT_MASK << shift_num);
            R_ICU_S->S_GPT_INTSEL[reg_num] |= (uint32_t) event_source << shift_num;
        }
    }
}

/*******************************************************************************************************************//**
 * Manipulate GPT_INTSELn to clear the GPT/MTU3 interrupt cause setting to 0.
 *
 * @param[in]   irq                 IRQ number
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGptSelectedClear (IRQn_Type irq)
{
    if (irq >= BSP_IRQ_GPT_EVENT_NUM_BASE)
    {
        uint32_t channel   = bsp_prv_irq_gpt_channel_get(irq);
        uint32_t reg_num   = bsp_prv_irq_gpt_reg_num_get(channel);
        uint32_t shift_num = bsp_prv_irq_gpt_selected_shift_num_get(irq, channel);

        if (channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
        {
            R_ICU_NS->NS_GPT_INTSEL[reg_num] &= ~(BSP_IRQ_GPT_SELECTED_EVENT_MASK << shift_num);
        }
        else
        {
            R_ICU_S->S_GPT_INTSEL[reg_num] &= ~(BSP_IRQ_GPT_SELECTED_EVENT_MASK << shift_num);
        }
    }
}

/*******************************************************************************************************************//**
 * Registers pointer addresses for control structure and interrupt handler.
 * If there is no management table, memory is allocated.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 * @param[in]   p_context           Control structure
 * @param[in]   p_interrupt_handler Interrupt handler
 *
 * @note        Function uses the malloc function to create management tables for "control structures" and
 *              "interrupt handlers". The management table is created in units of 1 group, with 2 channels of
 *              GPT/MTU3 as 1 group.
 *              The following memory is consumed for each group. The consumption can be calculated by
 *              "address size * number of elements * number of tables".
 *                  256 bytes for CR52 (=4*32*2)
 *                  512 bytes for CA55 (=8*32*2)
 *
 * @retval      FSP_SUCCESS                 Successful.
 * @retval      FSP_ERR_NOT_INITIALIZED     Memory allocation failed.
 **********************************************************************************************************************/
__STATIC_INLINE fsp_err_t R_BSP_IrqGptCombinedTableSet (IRQn_Type                    irq,
                                                        bsp_irq_gpt_combined_event_t event_source,
                                                        void                       * p_context,
                                                        void (                     * p_interrupt_handler)(void))
{
    uint32_t channel      = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t groupIndex   = bsp_prv_irq_gpt_combined_table_num_get(channel);
    uint32_t elementIndex = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);
    void   * p_table;

    /* If not exist 'groupIndex'th control structure table,
     * allocate memory for 'groupIndex'th control structure table. */
    if (NULL == gp_bsp_gpt_combined_ctrl_table[groupIndex])
    {
        p_table = bsp_prv_irq_gpt_combined_ctrl_table_allocate(groupIndex);
        FSP_ERROR_RETURN(NULL != p_table, FSP_ERR_NOT_INITIALIZED);
    }

    /* If not exist 'groupIndex'th interrupt handler table,
     * allocate memory for 'groupIndex'th interrupt handler table. */
    if (NULL == gp_bsp_gpt_combined_isr_table[groupIndex])
    {
        p_table = bsp_prv_irq_gpt_combined_isr_table_allocate(groupIndex);
        FSP_ERROR_RETURN(NULL != p_table, FSP_ERR_NOT_INITIALIZED);
    }

    /* Registers pointer addresses for control structure and interrupt handler. */
    gp_bsp_gpt_combined_ctrl_table[groupIndex][elementIndex] = p_context;
    gp_bsp_gpt_combined_isr_table[groupIndex][elementIndex]  = p_interrupt_handler;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Get pointer addresses for control structure.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 *
 * @return      Pointer addresses for control structure.
 **********************************************************************************************************************/
__STATIC_INLINE void * R_BSP_IrqGptCombinedCtrlGet (IRQn_Type irq, bsp_irq_gpt_combined_event_t event_source)
{
    uint32_t channel      = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t groupIndex   = bsp_prv_irq_gpt_combined_table_num_get(channel);
    uint32_t elementIndex = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);

    return gp_bsp_gpt_combined_ctrl_table[groupIndex][elementIndex];
}

/*******************************************************************************************************************//**
 * Get pointer addresses for interrupt handler.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 *
 * @return      Pointer addresses for interrupt handler.
 **********************************************************************************************************************/
__STATIC_INLINE fsp_vector_t R_BSP_IrqGptCombinedIsrGet (IRQn_Type irq, bsp_irq_gpt_combined_event_t event_source)
{
    uint32_t channel      = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t groupIndex   = bsp_prv_irq_gpt_combined_table_num_get(channel);
    uint32_t elementIndex = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);

    return gp_bsp_gpt_combined_isr_table[groupIndex][elementIndex];
}

/*******************************************************************************************************************//**
 * Releases pointer addresses for control structure and interrupt handler.
 * If there are no more values to manage in the management table, memory is free.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGptCombinedTableClear (IRQn_Type irq, bsp_irq_gpt_combined_event_t event_source)
{
    uint32_t channel      = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t groupIndex   = bsp_prv_irq_gpt_combined_table_num_get(channel);
    uint32_t elementIndex = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);

    gp_bsp_gpt_combined_ctrl_table[groupIndex][elementIndex] = NULL;
    gp_bsp_gpt_combined_isr_table[groupIndex][elementIndex]  = NULL;

    uint32_t cnt;
    for (cnt = 0; cnt < BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP; cnt++)
    {
        if (NULL != gp_bsp_gpt_combined_ctrl_table[groupIndex][cnt])
        {
            break;
        }
    }

    if (BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP == cnt)
    {
        bsp_prv_free(gp_bsp_gpt_combined_ctrl_table[groupIndex]);
        gp_bsp_gpt_combined_ctrl_table[groupIndex] = NULL;
    }

    for (cnt = 0; cnt < BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP; cnt++)
    {
        if (NULL != gp_bsp_gpt_combined_isr_table[groupIndex][cnt])
        {
            break;
        }
    }

    if (BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP == cnt)
    {
        bsp_prv_free(gp_bsp_gpt_combined_ctrl_table[groupIndex]);
        gp_bsp_gpt_combined_isr_table[groupIndex] = NULL;
    }
}

/*******************************************************************************************************************//**
 * Manipulate GPT_INTMSKn to mask GPT/MTU3 interrupts.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGptCombinedMaskSet (IRQn_Type irq, bsp_irq_gpt_combined_event_t event_source)
{
    uint32_t channel   = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t reg_num   = bsp_prv_irq_gpt_reg_num_get(channel);
    uint32_t shift_num = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);

    if (channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
    {
        R_ICU_NS->NS_GPT_INTMSK[reg_num] |= BSP_IRQ_GPT_COMBINED_EVENT_MASK << shift_num;
    }
    else
    {
        R_ICU_S->S_GPT_INTMSK[reg_num] |= BSP_IRQ_GPT_COMBINED_EVENT_MASK << shift_num;
    }
}

/*******************************************************************************************************************//**
 * Manipulate GPT_INTMSKn to unmask the GPT/MTU3 interrupt.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGptCombinedMaskClear (IRQn_Type irq, bsp_irq_gpt_combined_event_t event_source)
{
    uint32_t channel   = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t reg_num   = bsp_prv_irq_gpt_reg_num_get(channel);
    uint32_t shift_num = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);

    if (channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
    {
        R_ICU_NS->NS_GPT_INTMSK[reg_num] &= ~(BSP_IRQ_GPT_COMBINED_EVENT_MASK << shift_num);
    }
    else
    {
        R_ICU_S->S_GPT_INTMSK[reg_num] &= ~(BSP_IRQ_GPT_COMBINED_EVENT_MASK << shift_num);
    }
}

/*******************************************************************************************************************//**
 * Read GPT_INTSTATn and return the status of the GPT/MTU3 interrupt.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 *
 * @return      GPT_INTSTATn value.
 **********************************************************************************************************************/
__STATIC_INLINE uint32_t R_BSP_IrqGptCombinedStatusRead (IRQn_Type irq)
{
    uint32_t channel = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t reg_num = bsp_prv_irq_gpt_reg_num_get(channel);
    uint32_t intmsk  = 0;
    uint32_t intstat = 0;

    if (channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
    {
        intmsk  = ~(R_ICU_NS->NS_GPT_INTMSK[reg_num]);
        intstat = R_ICU_NS->NS_GPT_INTSTAT[reg_num] & intmsk;
    }
    else
    {
        intmsk  = ~(R_ICU_S->S_GPT_INTMSK[reg_num]);
        intstat = R_ICU_S->S_GPT_INTSTAT[reg_num] & intmsk;
    }

    return intstat;
}

/*******************************************************************************************************************//**
 * Write to GPT_INTCLRn to clear the GPT/MTU3 event occurrence state.
 *
 * @param[in]   irq                 IRQ number
 * @param[in]   event_source        Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 **********************************************************************************************************************/
__STATIC_INLINE void R_BSP_IrqGptCombinedStatusClear (IRQn_Type irq, bsp_irq_gpt_combined_event_t event_source)
{
    uint32_t channel   = bsp_prv_irq_gpt_channel_get(irq);
    uint32_t reg_num   = bsp_prv_irq_gpt_reg_num_get(channel);
    uint32_t shift_num = bsp_prv_irq_gpt_combined_shift_num_get(channel, event_source);

    if (channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
    {
        R_ICU_NS->NS_GPT_INTCLR[reg_num] |= (BSP_IRQ_GPT_COMBINED_EVENT_MASK << shift_num);
    }
    else
    {
        R_ICU_S->S_GPT_INTCLR[reg_num] |= (BSP_IRQ_GPT_COMBINED_EVENT_MASK << shift_num);
    }
}

#endif

/** @} (end addtogroup BSP_MCU) */

/*******************************************************************************************************************//**
 * @internal
 * @addtogroup BSP_MCU_PRV Internal BSP Documentation
 * @ingroup RENESAS_INTERNAL
 * @{
 **********************************************************************************************************************/

/* Public functions defined in bsp.h */
void bsp_irq_cfg(void);                // Used internally by BSP

/** @} (end addtogroup BSP_MCU_PRV) */

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
