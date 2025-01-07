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
#include "r_cmt.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* "CMT" in ASCII, used to determine if channel is open. */
#define CMT_OPEN                     (0x00434d54ULL)

#define CMT_PRV_UNIT(channel)       (channel / 2)
#define CMT_PRV_CHANNEL(channel)    (channel % 2)
#define CMT_CMSTR0_VALUE_MASK        (0x03U)
#define CMT_CMSTR0_STR_VALUE_MASK    (0x01U)
#define CMT_CMCOR_VALUE_MASK         (0xFFFFU)
#define CMT_CMCNT_VALUE_MASK         (0xFFFFU)
#define CMT_CMCR_CKS_VALUE_MASK      (0x03U) ///< CKS: 2 bits
#define CMT_CMCR_VALUE_MASK          (0x7FU)

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void cmt_hardware_initialize(cmt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg);

static void cmt_common_open(cmt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg);

static uint32_t cmt_clock_frequency_get(cmt_instance_ctrl_t * const p_instance_ctrl);

static void r_cmt_call_callback(cmt_instance_ctrl_t * p_instance_ctrl, timer_event_t event);

/***********************************************************************************************************************
 * ISR prototypes
 **********************************************************************************************************************/
void cmt_cm_int_isr(void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/* CMT implementation of timer interface  */
const timer_api_t g_timer_on_cmt =
{
    .open         = R_CMT_Open,
    .stop         = R_CMT_Stop,
    .start        = R_CMT_Start,
    .reset        = R_CMT_Reset,
    .enable       = R_CMT_Enable,
    .disable      = R_CMT_Disable,
    .periodSet    = R_CMT_PeriodSet,
    .dutyCycleSet = R_CMT_DutyCycleSet,
    .infoGet      = R_CMT_InfoGet,
    .statusGet    = R_CMT_StatusGet,
    .callbackSet  = R_CMT_CallbackSet,
    .close        = R_CMT_Close
};

/*******************************************************************************************************************//**
 * @addtogroup CMT
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initializes the timer module and applies configurations. Implements @ref timer_api_t::open.
 *
 * @retval FSP_SUCCESS                    Initialization was successful and timer has started.
 * @retval FSP_ERR_ASSERTION              A required input pointer is NULL or the source divider is invalid.
 * @retval FSP_ERR_ALREADY_OPEN           Module is already open.
 * @retval FSP_ERR_IRQ_BSP_DISABLED       timer_cfg_t::mode is ::TIMER_MODE_ONE_SHOT or timer_cfg_t::p_callback is not
 *                                        NULL, but ISR is not enabled.  ISR must be enabled to use one-shot mode or
 *                                        callback.
 * @retval FSP_ERR_IP_CHANNEL_NOT_PRESENT The channel requested in the p_cfg parameter is not available on this device.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Open (timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT((0U != (p_cfg->source_div % 2U)) && (p_cfg->source_div > 2));

    FSP_ERROR_RETURN(CMT_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
    FSP_ERROR_RETURN(((1U << p_cfg->channel) & BSP_FEATURE_CMT_VALID_CHANNEL_MASK), FSP_ERR_IP_CHANNEL_NOT_PRESENT);
    if ((p_cfg->p_callback) || (TIMER_MODE_ONE_SHOT == p_cfg->mode))
    {
        FSP_ERROR_RETURN(p_cfg->cycle_end_irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);
    }
#endif

    /* Power on the CMT channel. */
    uint8_t unit = CMT_PRV_UNIT(p_cfg->channel);

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_CMT, unit);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Initialize control structure based on configurations. */
    cmt_common_open(p_instance_ctrl, p_cfg);

    cmt_hardware_initialize(p_instance_ctrl, p_cfg);

    p_instance_ctrl->open = CMT_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Stops timer. Implements @ref timer_api_t::stop.
 *
 * @retval FSP_SUCCESS                 Timer successfully stopped.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Stop (timer_ctrl_t * const p_ctrl)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Stop timer */
    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    uint16_t cmstr0 = (uint16_t) (~(1U << channel));
    p_instance_ctrl->p_reg->UNT[unit].CMSTR0 &= cmstr0;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Starts timer. Implements @ref timer_api_t::start.
 *
 * @retval FSP_SUCCESS                 Timer successfully started.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Start (timer_ctrl_t * const p_ctrl)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Start timer */
    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    uint16_t cmstr0 = (uint16_t) (1U << channel) & CMT_CMSTR0_VALUE_MASK;
    p_instance_ctrl->p_reg->UNT[unit].CMSTR0 |= cmstr0;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Resets the counter value to 0. Implements @ref timer_api_t::reset.
 *
 * @retval FSP_SUCCESS                 Counter value written successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Reset (timer_ctrl_t * const p_ctrl)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT = 0;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Enables external event triggers that start, stop, clear, or capture the counter. Implements @ref timer_api_t::enable.
 *
 * @retval FSP_ERR_UNSUPPORTED                 API not supported by CMT.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Enable (timer_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Disables external event triggers that start, stop, clear, or capture the counter. Implements @ref timer_api_t::disable.
 *
 * @retval FSP_ERR_UNSUPPORTED                 API not supported by CMT.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Disable (timer_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Updates period. The new period is updated immediately and the counter is reset to 0.
 *
 * Implements @ref timer_api_t::periodSet.
 *
 * @retval FSP_SUCCESS                 Period value written successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_PeriodSet (timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Update period count. */
    uint32_t new_cmcor = period_counts - 1U;

    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    /* Update period register and reset counter. */
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].COR = new_cmcor & CMT_CMCOR_VALUE_MASK;

    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT = 0;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Sets duty cycle on requested pin. Implements @ref timer_api_t::dutyCycleSet.
 *
 * @retval FSP_ERR_UNSUPPORTED                 API not supported by CMT.
 **********************************************************************************************************************/
fsp_err_t R_CMT_DutyCycleSet (timer_ctrl_t * const p_ctrl, uint32_t const duty_cycle_counts, uint32_t const pin)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(duty_cycle_counts);
    FSP_PARAMETER_NOT_USED(pin);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Get timer information and store it in provided pointer p_info. Implements @ref timer_api_t::infoGet.
 *
 * @retval FSP_SUCCESS                 Period, count direction, frequency, and ELC event written to caller's
 *                                     structure successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_info was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_InfoGet (timer_ctrl_t * const p_ctrl, timer_info_t * const p_info)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_info);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    /* Get and store period */
    uint32_t cmcor = p_instance_ctrl->p_reg->UNT[unit].CM[channel].COR;

    p_info->period_counts = cmcor + 1;

    /* Get and store clock frequency */
    p_info->clock_frequency = cmt_clock_frequency_get(p_instance_ctrl);

    /* Get and store clock counting direction. Read count direction setting */
    p_info->count_direction = TIMER_DIRECTION_UP;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Get current timer status and store it in provided pointer p_status. Implements @ref timer_api_t::statusGet.
 *
 * @retval FSP_SUCCESS                 Current timer state and counter value set successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_status was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_StatusGet (timer_ctrl_t * const p_ctrl, timer_status_t * const p_status)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_status);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    /* Get counter state. */
    uint16_t cmstr0 = p_instance_ctrl->p_reg->UNT[unit].CMSTR0;
    p_status->state = (timer_state_t) ((uint8_t) (cmstr0 >> channel) & CMT_CMSTR0_STR_VALUE_MASK);

    /* Get counter value */
    p_status->counter = p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Set counter value.
 *
 * @note Do not call this API while the counter is counting.
 *
 * @retval FSP_SUCCESS                 Counter value updated.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_status was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_CounterSet (timer_ctrl_t * const p_ctrl, uint32_t counter)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;
#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    /* Set counter value */
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT = counter & CMT_CMCNT_VALUE_MASK;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Updates the user callback with the option to provide memory for the callback argument structure.
 * Implements @ref timer_api_t::callbackSet.
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_CallbackSet (timer_ctrl_t * const          p_ctrl,
                             void (                      * p_callback)(timer_callback_args_t *),
                             void const * const            p_context,
                             timer_callback_args_t * const p_callback_memory)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;

#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Stops counter, clears internal driver data. Implements @ref timer_api_t::close.
 *
 * @retval FSP_SUCCESS                 Successful close.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_CMT_Close (timer_ctrl_t * const p_ctrl)
{
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) p_ctrl;

#if CMT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(CMT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    timer_cfg_t * p_cfg = (timer_cfg_t *) p_instance_ctrl->p_cfg;

    /* Stop counter. */
    uint8_t unit    = CMT_PRV_UNIT(p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_cfg->channel);

    uint16_t cmstr0 = (uint16_t) (~(1U << channel));
    p_instance_ctrl->p_reg->UNT[unit].CMSTR0 &= cmstr0;

    /* Disable interrupts. */
    if (p_cfg->cycle_end_irq >= 0)
    {
        R_BSP_IrqDisable(p_cfg->cycle_end_irq);
        R_FSP_IsrContextSet(p_cfg->cycle_end_irq, NULL);
    }

    /* Power off the CMT channel. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_STOP(FSP_IP_CMT, unit);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Clear open flag. */
    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/** @} (end addtogroup CMT) */

/*******************************************************************************************************************//**
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initializes control structure based on configuration.
 *
 * @param[in]  p_instance_ctrl         Instance control block.
 * @param[in]  p_cfg                   Pointer to timer configuration.
 **********************************************************************************************************************/
static void cmt_common_open (cmt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg)
{
    /* Initialize control structure.  */
    p_instance_ctrl->p_cfg = p_cfg;

    /* Determine the timer variant. */
    p_instance_ctrl->variant = TIMER_VARIANT_16_BIT;

    /* Save register base address. */
    p_instance_ctrl->p_reg = R_CMT;

    /* Set callback and context pointers, if configured */
    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;
}

/*******************************************************************************************************************//**
 * Performs hardware initialization of the CMT.
 *
 * @param[in]  p_instance_ctrl        Instance control block.
 * @param[in]  p_cfg                  Pointer to timer configuration.
 **********************************************************************************************************************/
static void cmt_hardware_initialize (cmt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg)
{
    p_instance_ctrl->p_cfg = p_cfg;

    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    /* Initialize all registers that may affect operation of this driver to reset values. Skip these
     * since they affect all channels, and are initialized in CMCR and CMCNT: CMSTR. CMCR is set
     * immediately after clearing the module stop bit to ensure the timer is stopped before proceeding
     * with configuration. */
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CR  = 0U;
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT = 0U;

    /* Initialization sets all register required for up counting as described in hardware manual
     * (RZ microprocessor manual) and other registers required by the driver. */

    /* For setting CMCR.CKS, use formula (source_div - 3) / 2
     * TIMER_SOURCE_DIV_8 = 3 -> CMCR.CKS = 0 (8 division)
     * TIMER_SOURCE_DIV_32 = 5 -> CMCR.CKS = 1 (32 division)
     * TIMER_SOURCE_DIV_128 = 7 -> CMCR.CKS = 2 (128 division)
     * TIMER_SOURCE_DIV_512 = 9 -> CMCR.CKS = 3 (512 division) */
    uint32_t cmcr_cks = (((p_cfg->source_div & 0x0FU) - 3) / 2) & CMT_CMCR_CKS_VALUE_MASK;

    /* Enable CMI interrupts. */
    uint32_t cmcr = (cmcr_cks << R_CMT_UNT_CM_CR_CKS_Pos) | (R_CMT_UNT_CM_CR_CMIE_Msk);

    /* Store period register setting. */
    uint32_t cmcor = p_cfg->period_counts - 1U;

    /* Counter must be stopped to update CMCR. */
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CR = cmcr & CMT_CMCR_VALUE_MASK;

    /* Set period. The actual period is one cycle longer than the register value. */
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].COR = cmcor & CMT_CMCOR_VALUE_MASK;

    /* Reset counter to 0. */
    p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT = 0;

    /* Enable CPU interrupts. */
    if (p_cfg->cycle_end_irq >= 0)
    {
        R_BSP_IrqCfgEnable(p_cfg->cycle_end_irq, p_cfg->cycle_end_ipl, p_instance_ctrl);
    }
}

/*******************************************************************************************************************//**
 * Calculates clock frequency of CMT counter.  Divides CMT clock by CMT clock divisor.
 *
 * @param[in]  p_instance_ctrl           Instance control block
 *
 * @return     Clock frequency of the CMT counter.
 **********************************************************************************************************************/
static uint32_t cmt_clock_frequency_get (cmt_instance_ctrl_t * const p_instance_ctrl)
{
    /* Look up PCLKL frequency and divide it by CMT PCLKL divider. */
    uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
    uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

    /* Use formula (CMCR.CKS * 2) + 3
     * CMCR.CKS = 0 (8 division) -> TIMER_SOURCE_DIV_8 = 3
     * CMCR.CKS = 1 (32 division) -> TIMER_SOURCE_DIV_32 = 5
     * CMCR.CKS = 2 (128 division) -> TIMER_SOURCE_DIV_128 = 7
     * CMCR.CKS = 3 (512 division) -> TIMER_SOURCE_DIV_512 = 9  */
    timer_source_div_t pclk_divisor =
        (timer_source_div_t) (p_instance_ctrl->p_reg->UNT[unit].CM[channel].CR_b.CKS * 2 + 3);
    uint32_t pclk_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKL);

    return pclk_freq_hz >> pclk_divisor;
}

/*******************************************************************************************************************//**
 * Calls user callback.
 *
 * @param[in]     p_instance_ctrl     Pointer to CMT instance control block
 * @param[in]     event               Event code
 **********************************************************************************************************************/
static void r_cmt_call_callback (cmt_instance_ctrl_t * p_instance_ctrl, timer_event_t event)
{
    timer_callback_args_t args;

    /* Store callback arguments in memory provided by user if available. */
    timer_callback_args_t * p_args = p_instance_ctrl->p_callback_memory;
    if (NULL == p_args)
    {
        /* Store on stack */
        p_args = &args;
    }
    else
    {
        /* Save current arguments on the stack in case this is a nested interrupt. */
        args = *p_args;
    }

    p_args->event     = event;
    p_args->p_context = p_instance_ctrl->p_context;

    p_instance_ctrl->p_callback(p_args);

    if (NULL != p_instance_ctrl->p_callback_memory)
    {
        /* Restore callback memory in case this is a nested interrupt. */
        *p_instance_ctrl->p_callback_memory = args;
    }
}

/*******************************************************************************************************************//**
 * Calls callback if one was provided in the open function.
 **********************************************************************************************************************/
void cmt_cm_int_isr (void)
{
    CMT_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Recover ISR context saved in open. */
    cmt_instance_ctrl_t * p_instance_ctrl = (cmt_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Clear pending interrupt to make sure it doesn't fire again if another overflow has already occurred. */
    R_BSP_IrqClearPending(irq);

    /* If timer mode is one-shot mode, stop and reset timer here. */
    if (TIMER_MODE_ONE_SHOT == p_instance_ctrl->p_cfg->mode)
    {
        uint8_t unit    = CMT_PRV_UNIT(p_instance_ctrl->p_cfg->channel);
        uint8_t channel = CMT_PRV_CHANNEL(p_instance_ctrl->p_cfg->channel);

        /* Stop timer. */
        uint16_t cmstr0 = (uint16_t) (~(1U << channel));
        p_instance_ctrl->p_reg->UNT[unit].CMSTR0 &= cmstr0;

        /* Reset counter. */
        p_instance_ctrl->p_reg->UNT[unit].CM[channel].CNT = 0;
    }

    if (NULL != p_instance_ctrl->p_callback)
    {
        r_cmt_call_callback(p_instance_ctrl, TIMER_EVENT_CYCLE_END);
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;

    CMT_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}
