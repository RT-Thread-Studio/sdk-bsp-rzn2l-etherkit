/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics Corporation and/or its affiliates and may only
 * be used with products of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.
 * Renesas products are sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for
 * the selection and use of Renesas products and Renesas assumes no liability.  No license, express or implied, to any
 * intellectual property right is granted by Renesas.  This software is protected under all applicable laws, including
 * copyright laws. Renesas reserves the right to change or discontinue this software and/or this documentation.
 * THE SOFTWARE AND DOCUMENTATION IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND
 * TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY,
 * INCLUDING WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE
 * SOFTWARE OR DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR
 * DOCUMENTATION (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER,
 * INCLUDING, WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY
 * LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_icu.h"
#include "r_icu_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/** "ICU" in ASCII, used to determine if channel is open. */
#define ICU_OPEN                      (0x00494355U)

#define ICU_SAFETY_REGISTER_OFFSET    (14)
#define ICU_PORTNF_CLKSEL_MASK        (0x03U)
#define ICU_PORTNF_FLTSEL_MASK        (0x01U)
#define ICU_PORTNF_MD_MASK            (0x03U)
#define ICU_NS_PORTNF_OFFSET(channel)    (channel * 2)
#define ICU_S_PORTNF_OFFSET(channel)     ((channel - ICU_SAFETY_REGISTER_OFFSET) * 2)

#define ICU_PORTNF_MD_LEVEL_LOW       (0x00)
#define ICU_PORTNF_MD_FALLING_EDGE    (0x01)
#define ICU_PORTNF_MD_RISING_EDGE     (0x02)
#define ICU_PORTNF_MD_BOTH_EDGE       (0x03)

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
void r_icu_isr(void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/* Convert api defined value to PORT_NF_MD bit setting value. */
static const uint8_t g_icu_detect_mode[] =
{
    [EXTERNAL_IRQ_TRIG_FALLING]   = ICU_PORTNF_MD_FALLING_EDGE,
    [EXTERNAL_IRQ_TRIG_RISING]    = ICU_PORTNF_MD_RISING_EDGE,
    [EXTERNAL_IRQ_TRIG_BOTH_EDGE] = ICU_PORTNF_MD_BOTH_EDGE,
    [EXTERNAL_IRQ_TRIG_LEVEL_LOW] = ICU_PORTNF_MD_LEVEL_LOW,
};

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/* ICU implementation of External IRQ API. */
const external_irq_api_t g_external_irq_on_icu =
{
    .open        = R_ICU_ExternalIrqOpen,
    .enable      = R_ICU_ExternalIrqEnable,
    .disable     = R_ICU_ExternalIrqDisable,
    .callbackSet = R_ICU_ExternalIrqCallbackSet,
    .close       = R_ICU_ExternalIrqClose,
};

/*******************************************************************************************************************//**
 * @addtogroup ICU
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Configure an IRQ input pin for use with the external interrupt interface.  Implements @ref external_irq_api_t::open.
 *
 * The Open function is responsible for preparing an external IRQ pin for operation.
 *
 * @retval FSP_SUCCESS                    Open successful.
 * @retval FSP_ERR_ASSERTION              One of the following is invalid:
 *                                          - p_instance_ctrl or p_cfg is NULL
 * @retval FSP_ERR_ALREADY_OPEN           The channel specified has already been opened. No configurations were changed.
 *                                        Call the associated Close function to reconfigure the channel.
 * @retval FSP_ERR_IP_CHANNEL_NOT_PRESENT The channel requested in p_cfg is not available on the device selected in
 *                                        r_bsp_cfg.h.
 * @retval FSP_ERR_INVALID_ARGUMENT       p_cfg->p_callback is not NULL, but ISR is not enabled. ISR must be enabled to
 *                                        use callback function.
 *
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel.
 **********************************************************************************************************************/
fsp_err_t R_ICU_ExternalIrqOpen (external_irq_ctrl_t * const p_ctrl, external_irq_cfg_t const * const p_cfg)
{
    icu_instance_ctrl_t * p_instance_ctrl = (icu_instance_ctrl_t *) p_ctrl;

#if ICU_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ICU_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ERROR_RETURN(0 != ((1U << p_cfg->channel) & BSP_FEATURE_ICU_IRQ_CHANNELS_MASK), FSP_ERR_IP_CHANNEL_NOT_PRESENT);

    /* Callback must be used with a valid interrupt priority otherwise it will never be called. */
    if (p_cfg->p_callback)
    {
        FSP_ERROR_RETURN(BSP_IRQ_DISABLED != p_cfg->ipl, FSP_ERR_INVALID_ARGUMENT);
    }
#endif

    p_instance_ctrl->irq     = p_cfg->irq;
    p_instance_ctrl->channel = p_cfg->channel;

    /* Initialize control block. */
    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;

    if (p_instance_ctrl->channel < ICU_SAFETY_REGISTER_OFFSET)
    {
        /* Set the digital filter divider. */
        uint32_t clksel = R_ICU_NS->NS_PORTNF_CLKSEL;
        clksel &= ~(ICU_PORTNF_CLKSEL_MASK << ICU_NS_PORTNF_OFFSET(p_cfg->channel));
        clksel |= (uint32_t) (p_cfg->clock_source_div << ICU_NS_PORTNF_OFFSET(p_cfg->channel));

        /* Enable/Disable digital filter. */
        uint32_t fltsel = R_ICU_NS->NS_PORTNF_FLTSEL;
        fltsel &= ~(ICU_PORTNF_FLTSEL_MASK << p_cfg->channel);
        fltsel |= (uint32_t) (((true == p_cfg->filter_enable) ? 1U : 0U) << p_cfg->channel);

        /* Set the IRQ trigger. */
        uint32_t md = R_ICU_NS->NS_PORTNF_MD;
        md &= ~(ICU_PORTNF_MD_MASK << ICU_NS_PORTNF_OFFSET(p_cfg->channel));
        md |= (uint32_t) (g_icu_detect_mode[p_cfg->trigger] << (ICU_NS_PORTNF_OFFSET(p_cfg->channel)));

        /* Write NS_PORTNF_CLKSEL. */
        R_ICU_NS->NS_PORTNF_CLKSEL = clksel;

        /* Write NS_PORTNF_MD. */
        R_ICU_NS->NS_PORTNF_MD = md;

        /* Write NS_PORTNF_FLTSEL. */
        R_ICU_NS->NS_PORTNF_FLTSEL = fltsel;
    }
    else
    {
        /* Set the digital filter divider. */
        uint32_t clksel = R_ICU->S_PORTNF_CLKSEL;
        clksel &= ~(ICU_PORTNF_CLKSEL_MASK << ICU_S_PORTNF_OFFSET(p_cfg->channel));
        clksel |= (uint32_t) (p_cfg->clock_source_div << ICU_S_PORTNF_OFFSET(p_cfg->channel));

        /* Enable/Disable digital filter. */
        uint32_t fltsel = R_ICU->S_PORTNF_FLTSEL;
        fltsel &= ~(ICU_PORTNF_FLTSEL_MASK << (p_cfg->channel - ICU_SAFETY_REGISTER_OFFSET));
        fltsel |=
            (uint32_t) (((true == p_cfg->filter_enable) ? 1U : 0U) << (p_cfg->channel - ICU_SAFETY_REGISTER_OFFSET));

        /* Set the IRQ trigger. */
        uint32_t md = R_ICU->S_PORTNF_MD;
        md &= ~(ICU_PORTNF_MD_MASK << ICU_S_PORTNF_OFFSET(p_cfg->channel));
        md |= (uint32_t) (g_icu_detect_mode[p_cfg->trigger] << (ICU_S_PORTNF_OFFSET(p_cfg->channel)));

        /* Write S_PORTNF_CLKSEL. */
        R_ICU->S_PORTNF_CLKSEL = clksel;

        /* Write S_PORTNF_MD. */
        R_ICU->S_PORTNF_MD = md;

        /* Write S_PORTNF_FLTSEL. */
        R_ICU->S_PORTNF_FLTSEL = fltsel;
    }

    /* NOTE: User can have the driver opened when the IRQ is not in the vector table. This is for use cases
     * where the external IRQ driver is used to generate ELC events only (without CPU interrupts).
     * In such cases we will not set the IRQ priority but will continue with the processing.
     */
    if (p_instance_ctrl->irq >= 0)
    {
        /* The detection type on the ICU peripheral and the GIC must be match. */
        uint32_t type = (EXTERNAL_IRQ_TRIG_LEVEL_LOW == p_cfg->trigger) ? 0U : 1U;
        R_BSP_IrqDetectTypeSet(p_instance_ctrl->irq, type);

        R_BSP_IrqCfg(p_instance_ctrl->irq, p_cfg->ipl, p_instance_ctrl);
    }

    /* Mark the control block as open */
    p_instance_ctrl->open = ICU_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Enable external interrupt for specified channel. Implements @ref external_irq_api_t::enable.
 *
 * @retval FSP_SUCCESS                 Interrupt Enabled successfully.
 * @retval FSP_ERR_ASSERTION           The p_instance_ctrl parameter was null.
 * @retval FSP_ERR_NOT_OPEN            The channel is not opened.
 * @retval FSP_ERR_IRQ_BSP_DISABLED    Requested IRQ is not defined in this system
 **********************************************************************************************************************/
fsp_err_t R_ICU_ExternalIrqEnable (external_irq_ctrl_t * const p_ctrl)
{
    icu_instance_ctrl_t * p_instance_ctrl = (icu_instance_ctrl_t *) p_ctrl;

#if ICU_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ICU_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(p_instance_ctrl->irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);
#endif

    /* Clear the interrupt status and Pending bits, before the interrupt is enabled. */
    R_BSP_IrqEnable(p_instance_ctrl->irq);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Disable external interrupt for specified channel. Implements @ref external_irq_api_t::disable.
 *
 * @retval FSP_SUCCESS                 Interrupt disabled successfully.
 * @retval FSP_ERR_ASSERTION           The p_instance_ctrl parameter was null.
 * @retval FSP_ERR_NOT_OPEN            The channel is not opened.
 * @retval FSP_ERR_IRQ_BSP_DISABLED    Requested IRQ is not defined in this system
 **********************************************************************************************************************/
fsp_err_t R_ICU_ExternalIrqDisable (external_irq_ctrl_t * const p_ctrl)
{
    icu_instance_ctrl_t * p_instance_ctrl = (icu_instance_ctrl_t *) p_ctrl;

#if ICU_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ICU_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(p_instance_ctrl->irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);
#endif

    /* Disable the interrupt, and then clear the interrupt pending bits and interrupt status. */
    R_BSP_IrqDisable(p_instance_ctrl->irq);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements external_irq_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ICU_ExternalIrqCallbackSet (external_irq_ctrl_t * const p_ctrl,
                                        void (                    * p_callback)(
                                            external_irq_callback_args_t *),
                                        void const * const                   p_context,
                                        external_irq_callback_args_t * const p_callback_memory)
{
    icu_instance_ctrl_t * p_instance_ctrl = p_ctrl;

#if ICU_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ICU_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ASSERT(NULL != p_callback);
#endif

    FSP_PARAMETER_NOT_USED(p_callback_memory);

    /* Store callback and context */
    p_instance_ctrl->p_callback = p_callback;
    p_instance_ctrl->p_context  = p_context;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Close the external interrupt channel. Implements @ref external_irq_api_t::close.
 *
 * @retval     FSP_SUCCESS          Successfully closed.
 * @retval     FSP_ERR_ASSERTION    The parameter p_instance_ctrl is NULL.
 * @retval     FSP_ERR_NOT_OPEN     The channel is not opened.
 **********************************************************************************************************************/
fsp_err_t R_ICU_ExternalIrqClose (external_irq_ctrl_t * const p_ctrl)
{
    icu_instance_ctrl_t * p_instance_ctrl = (icu_instance_ctrl_t *) p_ctrl;

#if ICU_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ICU_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Cleanup. Disable interrupt */
    if (p_instance_ctrl->irq >= 0)
    {
        /* Disable the interrupt, and then clear the interrupt pending bits and interrupt status. */
        R_BSP_IrqDisable(p_instance_ctrl->irq);
        R_FSP_IsrContextSet(p_instance_ctrl->irq, NULL);
    }

    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup ICU)
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * ICU External Interrupt ISR.
 **********************************************************************************************************************/
void r_icu_isr (void)
{
    ICU_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;

    IRQn_Type             irq             = R_FSP_CurrentIrqGet();
    icu_instance_ctrl_t * p_instance_ctrl = (icu_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    if ((NULL != p_instance_ctrl) && (NULL != p_instance_ctrl->p_callback))
    {
        /* Set data to identify callback to user, then call user callback. */
        external_irq_callback_args_t args;
        args.channel   = p_instance_ctrl->channel;
        args.p_context = p_instance_ctrl->p_context;
        p_instance_ctrl->p_callback(&args);
    }

    /* Restore context if RTOS is used */

    FSP_CONTEXT_RESTORE;

    ICU_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}
