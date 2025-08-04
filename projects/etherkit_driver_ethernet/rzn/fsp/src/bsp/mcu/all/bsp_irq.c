/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

/* Pointer to control structure table. */
void ** gp_bsp_gpt_combined_ctrl_table[BSP_IRQ_GPT_COMBINED_GROUP_NUM] =
{
    NULL,                              /* Pointer to Group 0 control structure table.  (Use NS_GPT_INTSTAT0) */
    NULL,                              /* Pointer to Group 1 control structure table.  (Use NS_GPT_INTSTAT1) */
    NULL,                              /* Pointer to Group 2 control structure table.  (Use NS_GPT_INTSTAT2) */
    NULL,                              /* Pointer to Group 3 control structure table.  (Use NS_GPT_INTSTAT3) */
    NULL,                              /* Pointer to Group 4 control structure table.  (Use NS_GPT_INTSTAT4) */
    NULL,                              /* Pointer to Group 5 control structure table.  (Use NS_GPT_INTSTAT5) */
    NULL,                              /* Pointer to Group 6 control structure table.  (Use NS_GPT_INTSTAT6) */
    NULL,                              /* Pointer to Group 7 control structure table.  (Use NS_GPT_INTSTAT7) */
    NULL,                              /* Pointer to Group 8 control structure table.  (Use NS_GPT_INTSTAT8) */
    NULL,                              /* Pointer to Group 9 control structure table.  (Use NS_GPT_INTSTAT9) */
    NULL,                              /* Pointer to Group 10 control structure table. (Use NS_GPT_INTSTAT10) */
    NULL,                              /* Pointer to Group 11 control structure table. (Use NS_GPT_INTSTAT11) */
    NULL,                              /* Pointer to Group 12 control structure table. (Use NS_GPT_INTSTAT12) */
    NULL,                              /* Pointer to Group 13 control structure table. (Use NS_GPT_INTSTAT13) */
    NULL,                              /* Pointer to Group 14 control structure table. (Use NS_GPT_INTSTAT14) */
    NULL,                              /* Pointer to Group 15 control structure table. (Use NS_GPT_INTSTAT15) */
    NULL,                              /* Pointer to Group 16 control structure table. (Use NS_GPT_INTSTAT16) */
    NULL,                              /* Pointer to Group 17 control structure table. (Use NS_GPT_INTSTAT17) */
    NULL,                              /* Pointer to Group 18 control structure table. (Use NS_GPT_INTSTAT18) */
    NULL,                              /* Pointer to Group 19 control structure table. (Use NS_GPT_INTSTAT19) */
    NULL,                              /* Pointer to Group 20 control structure table. (Use NS_GPT_INTSTAT20) */
    NULL,                              /* Pointer to Group 21 control structure table. (Use NS_GPT_INTSTAT21) */
    NULL,                              /* Pointer to Group 22 control structure table. (Use NS_GPT_INTSTAT22) */
    NULL,                              /* Pointer to Group 23 control structure table. (Use NS_GPT_INTSTAT23) */
    NULL,                              /* Pointer to Group 24 control structure table. (Use NS_GPT_INTSTAT24) */
    NULL,                              /* Pointer to Group 25 control structure table. (Use NS_GPT_INTSTAT25) */
    NULL,                              /* Pointer to Group 26 control structure table. (Use S_GPT_INTSTAT0) */
    NULL,                              /* Pointer to Group 27 control structure table. (Use S_GPT_INTSTAT1) */
};

/* Pointer to interrupt handler table. */
fsp_vector_t * gp_bsp_gpt_combined_isr_table[BSP_IRQ_GPT_COMBINED_GROUP_NUM] =
{
    NULL,                              /* Pointer to Group 0 interrupt handler table.  (Use NS_GPT_INTSTAT0) */
    NULL,                              /* Pointer to Group 1 interrupt handler table.  (Use NS_GPT_INTSTAT1) */
    NULL,                              /* Pointer to Group 2 interrupt handler table.  (Use NS_GPT_INTSTAT2) */
    NULL,                              /* Pointer to Group 3 interrupt handler table.  (Use NS_GPT_INTSTAT3) */
    NULL,                              /* Pointer to Group 4 interrupt handler table.  (Use NS_GPT_INTSTAT4) */
    NULL,                              /* Pointer to Group 5 interrupt handler table.  (Use NS_GPT_INTSTAT5) */
    NULL,                              /* Pointer to Group 6 interrupt handler table.  (Use NS_GPT_INTSTAT6) */
    NULL,                              /* Pointer to Group 7 interrupt handler table.  (Use NS_GPT_INTSTAT7) */
    NULL,                              /* Pointer to Group 8 interrupt handler table.  (Use NS_GPT_INTSTAT8) */
    NULL,                              /* Pointer to Group 9 interrupt handler table.  (Use NS_GPT_INTSTAT9) */
    NULL,                              /* Pointer to Group 10 interrupt handler table. (Use NS_GPT_INTSTAT10) */
    NULL,                              /* Pointer to Group 11 interrupt handler table. (Use NS_GPT_INTSTAT11) */
    NULL,                              /* Pointer to Group 12 interrupt handler table. (Use NS_GPT_INTSTAT12) */
    NULL,                              /* Pointer to Group 13 interrupt handler table. (Use NS_GPT_INTSTAT13) */
    NULL,                              /* Pointer to Group 14 interrupt handler table. (Use NS_GPT_INTSTAT14) */
    NULL,                              /* Pointer to Group 15 interrupt handler table. (Use NS_GPT_INTSTAT15) */
    NULL,                              /* Pointer to Group 16 interrupt handler table. (Use NS_GPT_INTSTAT16) */
    NULL,                              /* Pointer to Group 17 interrupt handler table. (Use NS_GPT_INTSTAT17) */
    NULL,                              /* Pointer to Group 18 interrupt handler table. (Use NS_GPT_INTSTAT18) */
    NULL,                              /* Pointer to Group 19 interrupt handler table. (Use NS_GPT_INTSTAT19) */
    NULL,                              /* Pointer to Group 20 interrupt handler table. (Use NS_GPT_INTSTAT20) */
    NULL,                              /* Pointer to Group 21 interrupt handler table. (Use NS_GPT_INTSTAT21) */
    NULL,                              /* Pointer to Group 22 interrupt handler table. (Use NS_GPT_INTSTAT22) */
    NULL,                              /* Pointer to Group 23 interrupt handler table. (Use NS_GPT_INTSTAT23) */
    NULL,                              /* Pointer to Group 24 interrupt handler table. (Use NS_GPT_INTSTAT24) */
    NULL,                              /* Pointer to Group 25 interrupt handler table. (Use NS_GPT_INTSTAT25) */
    NULL,                              /* Pointer to Group 26 control structure table. (Use S_GPT_INTSTAT0) */
    NULL,                              /* Pointer to Group 27 control structure table. (Use S_GPT_INTSTAT1) */
};

#endif

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initialize interrupt controller.
 *
 * @retval       None
 **********************************************************************************************************************/
void bsp_irq_cfg (void)
{
    bsp_irq_core_cfg();
}

#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

/*******************************************************************************************************************//**
 * Get GPT_INT channel number.
 *
 * @param[in]   irq     IRQ number
 *
 * @return      GPT_INT channel number.
 **********************************************************************************************************************/
uint32_t bsp_prv_irq_gpt_channel_get (IRQn_Type irq)
{
    return ((uint32_t) irq - BSP_IRQ_GPT_EVENT_NUM_BASE) / BSP_IRQ_GPT_INT_NUM_MAX;
}

/*******************************************************************************************************************//**
 * Get GPT_INT register number.
 *
 * @param[in]   channel     GPT_INT channel number
 *
 * @return      GPT_INT register number.
 **********************************************************************************************************************/
uint32_t bsp_prv_irq_gpt_reg_num_get (uint32_t channel)
{
    return (channel % BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL) / BSP_IRQ_GPT_REGISTER_DIVISION;
}

/*******************************************************************************************************************//**
 * Get bit shift value of the GPT_INTSELn register.
 *
 * @param[in]   irq         IRQ number
 * @param[in]   channel     GPT_INT channel number
 *
 * @return      Bit shift value.
 **********************************************************************************************************************/
uint32_t bsp_prv_irq_gpt_selected_shift_num_get (IRQn_Type irq, uint32_t channel)
{
    uint32_t irqdiff = (uint32_t) irq - BSP_IRQ_GPT_EVENT_NUM_BASE;

    return channel % BSP_IRQ_GPT_REGISTER_DIVISION ?
           BSP_IRQ_GPT_SELECTED_INTERVAL * (irqdiff % BSP_IRQ_GPT_INT_NUM_MAX) + BSP_IRQ_GPT_IY_SHIFT_NUM :
           BSP_IRQ_GPT_SELECTED_INTERVAL * (irqdiff % BSP_IRQ_GPT_INT_NUM_MAX) + BSP_IRQ_GPT_IX_SHIFT_NUM;
}

/*******************************************************************************************************************//**
 * Get GPT_INT table number.
 *
 * @param[in]   channel     GPT_INT channel number
 *
 * @return      GPT_INT table number.
 **********************************************************************************************************************/
uint32_t bsp_prv_irq_gpt_combined_table_num_get (uint32_t channel)
{
    return (channel % BSP_FEATURE_GPT_CHANNEL) / BSP_IRQ_GPT_REGISTER_DIVISION;
}

/*******************************************************************************************************************//**
 * Allocate memory for 'groupIndex'th control structure table.
 *
 * @param[in]   groupIndex         Register number
 *
 * @return      Allocated memory address
 **********************************************************************************************************************/
void * bsp_prv_irq_gpt_combined_ctrl_table_allocate (uint32_t groupIndex)
{
    /* Allocate memory for 'groupIndex'th control structure table. */
    gp_bsp_gpt_combined_ctrl_table[groupIndex] =
        bsp_prv_malloc(BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP * sizeof(uintptr_t));
    if (NULL != gp_bsp_gpt_combined_ctrl_table[groupIndex])
    {
        /* Set initial value at malloc area. */
        for (uint32_t elementIndex = 0U; elementIndex < BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP; elementIndex++)
        {
            gp_bsp_gpt_combined_ctrl_table[groupIndex][elementIndex] = NULL;
        }
    }

    return gp_bsp_gpt_combined_ctrl_table[groupIndex];
}

/*******************************************************************************************************************//**
 * Allocate memory for 'groupIndex'th interrupt handler table.
 *
 * @param[in]   groupIndex         Register number
 *
 * @return      Allocated memory address
 **********************************************************************************************************************/
void * bsp_prv_irq_gpt_combined_isr_table_allocate (uint32_t groupIndex)
{
    /* Allocate memory for 'groupIndex'th structure table. */
    gp_bsp_gpt_combined_isr_table[groupIndex] =
        bsp_prv_malloc(BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP * sizeof(uintptr_t));
    if (NULL != gp_bsp_gpt_combined_isr_table[groupIndex])
    {
        /* Set initial value at malloc area. */
        for (uint32_t elementIndex = 0U; elementIndex < BSP_IRQ_GPT_COMBINED_EVENT_NUM_PER_GROUP; elementIndex++)
        {
            gp_bsp_gpt_combined_isr_table[groupIndex][elementIndex] = NULL;
        }
    }

    return gp_bsp_gpt_combined_isr_table[groupIndex];
}

/*******************************************************************************************************************//**
 * Get bit shift value of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn registers.
 *
 * @param[in]   channel       GPT_INT channel number
 * @param[in]   event_source  Define the bit shift values ​​of GPT_INTMSKn, GPT_INTCLRn and GPT_INTSTATn as an enum.
 *
 * @return      Bit shift value.
 **********************************************************************************************************************/
uint32_t bsp_prv_irq_gpt_combined_shift_num_get (uint32_t channel, bsp_irq_gpt_combined_event_t event_source)
{
    return channel % BSP_IRQ_GPT_REGISTER_DIVISION ? (uint32_t) event_source + BSP_IRQ_GPT_IY_SHIFT_NUM :
           (uint32_t) event_source + BSP_IRQ_GPT_IX_SHIFT_NUM;
}

/*******************************************************************************************************************//**
 * Called when a GPT combined interrupt occurs, clears the GPT/MTU3 interrupt and executes the ISR function.
 **********************************************************************************************************************/
void bsp_irq_gpt_combined_interrupt_handler (void)
{
    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();

    uint32_t gpt_intstat = R_BSP_IrqGptCombinedStatusRead(irq);

    /* Since the GPT/MTU interrupt status is divided into upper 16 bits and lower 16 bits,
     * the bit positions are adjusted for the upper bits. */
    if (bsp_prv_irq_gpt_channel_get(irq) % BSP_IRQ_GPT_REGISTER_DIVISION)
    {
        gpt_intstat >>= BSP_IRQ_GPT_IY_SHIFT_NUM;
    }

    bsp_irq_gpt_combined_event_t event_source;
    uint32_t next_event  = 0;
    uint32_t event_shift = 0;
    void   * p_context;
    void     (* p_interrupt_handler)(void);

    while (gpt_intstat)
    {
        /* Scan and search for GPT and MTU3 interrupts factors one by one. */
        next_event    = __CLZ(__RBIT(gpt_intstat));
        gpt_intstat >>= next_event;
        event_shift  += next_event;
        event_source  = (bsp_irq_gpt_combined_event_t) (event_shift & BSP_IRQ_GPT_COMBINED_EVENT_ENUM_MASK);

        /* Obtains the context and interrupt handler from the table and executes interrupt processing. */
        R_BSP_IrqGptCombinedStatusClear(irq, event_source);
        p_context = R_BSP_IrqGptCombinedCtrlGet(irq, event_source);
        R_FSP_IsrContextSet(irq, p_context);
        p_interrupt_handler = R_BSP_IrqGptCombinedIsrGet(irq, event_source);
        p_interrupt_handler();

        /* Clear the scanned flags one by one. */
        gpt_intstat &= (uint32_t) ~(1UL);
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;
}

#endif
