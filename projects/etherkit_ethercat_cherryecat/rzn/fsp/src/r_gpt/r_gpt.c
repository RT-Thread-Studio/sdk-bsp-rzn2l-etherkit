/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_gpt.h"
#include "r_gpt_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* "GPT" in ASCII, used to determine if channel is open. */
#define GPT_OPEN                                         (0x00475054ULL)

#define GPT_PRV_GTWP_RESET_VALUE                         (0xA500U)
#define GPT_PRV_GTWP_WRITE_PROTECT                       (0xA51FU)

#define GPT_PRV_GTIOR_STOP_LEVEL_BIT                     (6)
#define GPT_PRV_GTIOR_INITIAL_LEVEL_BIT                  (4)

#define GPT_PRV_GTIO_HIGH_COMPARE_MATCH_LOW_CYCLE_END    (0x6U)
#define GPT_PRV_GTIO_LOW_COMPARE_MATCH_HIGH_CYCLE_END    (0x9U)

#define GPT_PRV_GTIO_TOGGLE_COMPARE_MATCH                (0x3U)

#define GPT_PRV_GTBER_BUFFER_ENABLE_FORCE_TRANSFER       (0x550000U)

#define GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE             (0x80000000U)

#define GPT_PRV_GTCCRA                                   (0U)
#define GPT_PRV_GTCCRB                                   (1U)
#define GPT_PRV_GTCCRC                                   (2U)
#define GPT_PRV_GTCCRE                                   (3U)
#define GPT_PRV_GTCCRD                                   (4U)
#define GPT_PRV_GTCCRF                                   (5U)

/* GPT_CFG_OUTPUT_SUPPORT_ENABLE is set to 2 to enable extra features. */
#define GPT_PRV_EXTRA_FEATURES_ENABLED                   (2U)

#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

/* Macro definition of GPT_INT4 */
 #define GPT_INT4                                        (4)

/* Define the GPT_INTERRUPT_SELECT_ENABLE macro */
 #define GPT_INTERRUPT_SELECT_ENABLE(irq, source_selected, selected, function)                                  \
    if ((irq) > FSP_INVALID_VECTOR)                                                                             \
    {                                                                                                           \
        if ((GPT_INT4) != (source_selected))                                                                    \
        {                                                                                                       \
            R_BSP_IrqGptSelectedSet((irq), (BSP_IRQ_GPT_SELECTED_EVENT_GPT_ ## selected));                      \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
            R_BSP_IrqGptCombinedTableSet((irq), (BSP_IRQ_GPT_COMBINED_EVENT_GPT_ ## selected), p_instance_ctrl, \
                                         function);                                                             \
            R_BSP_IrqGptCombinedMaskClear((irq), (BSP_IRQ_GPT_COMBINED_EVENT_GPT_ ## selected));                \
        }                                                                                                       \
    }

/* Define the GPT_INTERRUPT_SELECT_DISABLE macro. */
 #define GPT_INTERRUPT_SELECT_DISABLE(irq, source_selected, selected)                             \
    if ((irq) > FSP_INVALID_VECTOR)                                                               \
    {                                                                                             \
        if ((GPT_INT4) != (source_selected))                                                      \
        {                                                                                         \
            R_BSP_IrqGptSelectedClear((irq));                                                     \
        }                                                                                         \
        else                                                                                      \
        {                                                                                         \
            R_BSP_IrqGptCombinedMaskSet((irq), (BSP_IRQ_GPT_COMBINED_EVENT_GPT_ ## selected));    \
            R_BSP_IrqGptCombinedTableClear((irq), (BSP_IRQ_GPT_COMBINED_EVENT_GPT_ ## selected)); \
        }                                                                                         \
    }
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Duty cycle mode. */
typedef enum e_gpt_duty_cycle_mode
{
    GPT_DUTY_CYCLE_MODE_REGISTER    = 0, // Duty cycle depends on compare match
    GPT_DUTY_CYCLE_MODE_0_PERCENT   = 2, // Output low
    GPT_DUTY_CYCLE_MODE_100_PERCENT = 3, // Output high
} gpt_duty_cycle_mode_t;

/* Count direction */
typedef enum e_gpt_dir
{
    GPT_DIR_COUNT_DOWN = 0,
    GPT_DIR_COUNT_UP   = 1
} gpt_dir_t;

typedef struct st_gpt_prv_duty_registers
{
    uint32_t gtccr_buffer;
    uint32_t omdty;
} gpt_prv_duty_registers_t;

typedef enum e_gpt_prv_capture_event
{
    GPT_PRV_CAPTURE_EVENT_A,
    GPT_PRV_CAPTURE_EVENT_B,
} gpt_prv_capture_event_t;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void      gpt_counter_initialize(gpt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg);
static fsp_err_t r_gpt_open_cfg_check(timer_cfg_t const * const p_cfg);
static void      gpt_hardware_initialize(gpt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg);

static void gpt_common_open(gpt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg);

static uint32_t gpt_clock_frequency_get(gpt_instance_ctrl_t * const p_instance_ctrl);

static void gpt_hardware_events_disable(gpt_instance_ctrl_t * p_instance_ctrl);

static void r_gpt_disable_irq(IRQn_Type irq);
static void gpt_disable_interrupt(gpt_instance_ctrl_t * const p_instance_ctrl);

static inline void r_gpt_write_protect_enable(gpt_instance_ctrl_t * const p_instance_ctrl);
static inline void r_gpt_write_protect_disable(gpt_instance_ctrl_t * const p_instance_ctrl);

/* Noinline attribute added to reduce code size for CM23 GCC build. */
static void r_gpt_enable_irq(IRQn_Type const irq, uint32_t priority, void * p_context) __attribute__((noinline));
static void gpt_enable_interrupt(gpt_instance_ctrl_t * const p_instance_ctrl);

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE

static void gpt_calculate_duty_cycle(gpt_instance_ctrl_t * const p_instance_ctrl,
                                     uint32_t const              duty_cycle_counts,
                                     gpt_prv_duty_registers_t  * p_duty_reg);

static uint32_t gpt_gtior_calculate(timer_cfg_t const * const p_cfg, gpt_pin_level_t const stop_level);

#endif

static void r_gpt_call_callback(gpt_instance_ctrl_t * p_ctrl, timer_event_t event, uint32_t capture);

/***********************************************************************************************************************
 * ISR prototypes
 **********************************************************************************************************************/
void gpt_counter_overflow_isr(void);

#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
void gpt_counter_underflow_isr(void);

#endif
void gpt_dead_time_isr(void);
void gpt_capture_a_isr(void);
void gpt_capture_b_isr(void);

#if 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE

/* Function for GPT_INT0-3 */
void gpt_int_select_isr(void);

#endif

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/* GPT implementation of timer interface  */
const timer_api_t g_timer_on_gpt =
{
    .open         = R_GPT_Open,
    .stop         = R_GPT_Stop,
    .start        = R_GPT_Start,
    .reset        = R_GPT_Reset,
    .enable       = R_GPT_Enable,
    .disable      = R_GPT_Disable,
    .periodSet    = R_GPT_PeriodSet,
    .dutyCycleSet = R_GPT_DutyCycleSet,
    .infoGet      = R_GPT_InfoGet,
    .statusGet    = R_GPT_StatusGet,
    .callbackSet  = R_GPT_CallbackSet,
    .close        = R_GPT_Close,
};

/*******************************************************************************************************************//**
 * @addtogroup GPT
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initializes the timer module and applies configurations. Implements @ref timer_api_t::open.
 *
 * GPT hardware does not support one-shot functionality natively.  When using one-shot mode, the timer will be stopped
 * in an ISR after the requested period has elapsed.
 *
 * The GPT implementation of the general timer can accept a gpt_extended_cfg_t extension parameter.
 *
 * @retval FSP_SUCCESS                    Initialization was successful and timer has started.
 * @retval FSP_ERR_ASSERTION              A required input pointer is NULL or the source divider is invalid.
 * @retval FSP_ERR_ALREADY_OPEN           Module is already open.
 * @retval FSP_ERR_IRQ_BSP_DISABLED       timer_cfg_t::mode is ::TIMER_MODE_ONE_SHOT or timer_cfg_t::p_callback is not
 *                                        NULL, but ISR is not enabled.  ISR must be enabled to use one-shot mode or
 *                                        callback.
 * @retval FSP_ERR_INVALID_MODE           Triangle wave PWM is only supported if GPT_CFG_OUTPUT_SUPPORT_ENABLE is 2.
 * @retval FSP_ERR_IP_CHANNEL_NOT_PRESENT The channel requested in the p_cfg parameter is not available on this device.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Open (timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_cfg->p_extend);
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT((p_cfg->source_div != 7U) && (p_cfg->source_div != 9U) && (p_cfg->source_div <= 10));
 #if GPT_PRV_EXTRA_FEATURES_ENABLED != GPT_CFG_OUTPUT_SUPPORT_ENABLE
    FSP_ERROR_RETURN(p_cfg->mode <= TIMER_MODE_PWM, FSP_ERR_INVALID_MODE);
 #endif
    FSP_ERROR_RETURN(GPT_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
#endif

#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE

    /** mask_bit GPTunit0 0-6bit, GPTunit1 0-6bit, GPTunit2 0-3bit*/
    if (p_cfg->channel <= GPT_CHANNEL_UNIT0_6)
    {
        p_instance_ctrl->channel_mask = (1U << p_cfg->channel);
    }
    else if ((GPT_CHANNEL_UNIT1_0 <= p_cfg->channel) && (p_cfg->channel <= GPT_CHANNEL_UNIT1_6))
    {
        p_instance_ctrl->channel_mask = (1U << (p_cfg->channel - GPT_CHANNEL_UNIT1_0));
    }
    else
    {
        p_instance_ctrl->channel_mask = (1U << (p_cfg->channel - GPT_CHANNEL_UNIT2_0));
    }

#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    if (p_cfg->channel < BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL)
    {
        p_instance_ctrl->channel_mask = (1U << p_cfg->channel % BSP_FEATURE_GPT_LLPP_CHANNEL_PER_UNIT);
    }
    else if ((BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL <= p_cfg->channel) &&
             (p_cfg->channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL))
    {
        p_instance_ctrl->channel_mask = (1U << (p_cfg->channel - BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL));
    }
    else
    {
        p_instance_ctrl->channel_mask = (1U << (p_cfg->channel - BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL));
    }
#endif

    /* Verify the configuration parameters are valid   */
    fsp_err_t err = r_gpt_open_cfg_check(p_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Initialize control structure based on configurations. */
    gpt_common_open(p_instance_ctrl, p_cfg);

    gpt_hardware_initialize(p_instance_ctrl, p_cfg);

    p_instance_ctrl->open = GPT_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Stops timer. Implements @ref timer_api_t::stop.
 *
 * @retval FSP_SUCCESS                 Timer successfully stopped.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Stop (timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Stop timer */
    p_instance_ctrl->p_reg->GTSTP = p_instance_ctrl->channel_mask;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Starts timer. Implements @ref timer_api_t::start.
 *
 * @retval FSP_SUCCESS                 Timer successfully started.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Start (timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Start timer */
    p_instance_ctrl->p_reg->GTSTR = p_instance_ctrl->channel_mask;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Resets the counter value to 0. Implements @ref timer_api_t::reset.
 *
 * @note This function also updates to the new period if no counter overflow has occurred since the last call to
 * R_GPT_PeriodSet().
 *
 * @retval FSP_SUCCESS                 Counter value written successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Reset (timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Clear timer counter. */
    p_instance_ctrl->p_reg->GTCLR = p_instance_ctrl->channel_mask;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Enables external event triggers that start, stop, clear, or capture the counter. Implements @ref timer_api_t::enable.
 *
 * @retval FSP_SUCCESS                 External events successfully enabled.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Enable (timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Enable use of GTSTR, GTSTP, and GTCLR for this channel. */
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;
    uint32_t             gtssr    = GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE;
    uint32_t             gtpsr    = GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE;
    uint32_t             gtcsr    = GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE;

    /* OR with user settings. */
    gtssr |= p_extend->start_source;
    gtpsr |= p_extend->stop_source;
    gtcsr |= p_extend->clear_source;

    r_gpt_write_protect_disable(p_instance_ctrl);

    /* Set the count sources. Ensure stop and clear sources are set before start source, and capture sources are set
     * after start source. */
    p_instance_ctrl->p_reg->GTPSR   = gtpsr;
    p_instance_ctrl->p_reg->GTCSR   = gtcsr;
    p_instance_ctrl->p_reg->GTSSR   = gtssr;
    p_instance_ctrl->p_reg->GTICASR = p_extend->capture_a_source;
    p_instance_ctrl->p_reg->GTICBSR = p_extend->capture_b_source;

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Disables external event triggers that start, stop, clear, or capture the counter. Implements @ref timer_api_t::disable.
 *
 * @note The timer could be running after R_GPT_Disable(). To ensure it is stopped, call R_GPT_Stop().
 *
 * @retval FSP_SUCCESS                 External events successfully disabled.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Disable (timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    r_gpt_write_protect_disable(p_instance_ctrl);

    gpt_hardware_events_disable(p_instance_ctrl);

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Sets period value provided. If the timer is running, the period will be updated after the next counter overflow.
 * If the timer is stopped, this function resets the counter and updates the period.
 * Implements @ref timer_api_t::periodSet.
 *
 * @warning If periodic output is used, the duty cycle buffer registers are updated after the period buffer register.
 * If this function is called while the timer is running and a GPT overflow occurs during processing, the duty cycle
 * will not be the desired 50% duty cycle until the counter overflow after processing completes.
 *
 * @retval FSP_SUCCESS                 Period value written successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_PeriodSet (timer_ctrl_t * const p_ctrl, uint32_t const period_counts)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    r_gpt_write_protect_disable(p_instance_ctrl);

    /* Update period buffer register. The actual period is one cycle longer than the register value for saw waves
     * and twice the register value for triangle waves. Reference section "General PWM Timer Cycle Setting
     * Register (GTPR)". The setting passed to the configuration is expected to be half the desired period for
     * triangle waves. */
    uint32_t new_gtpr = period_counts - 1U;
#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    if (p_instance_ctrl->p_cfg->mode >= TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM)
    {
        new_gtpr = period_counts;
    }
#endif

    p_instance_ctrl->p_reg->GTPBR = new_gtpr;

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE

    /* Set a 50% duty cycle so the period of the waveform on the output pin matches the requested period. */
    if (TIMER_MODE_PERIODIC == p_instance_ctrl->p_cfg->mode)
    {
        /* The  GTIOCA/GTIOCB pins transition 1 cycle after compare match when buffer operation is used (see
         * Section "PWM Output Operating Mode" in the RZ microprocessor User's Manual for details). To get a duty cycle
         * as close to 50% as possible, duty cycle (register) = (period (counts) / 2) - 1. */
        uint32_t duty_cycle_50_percent = (period_counts >> 1) - 1U;
        p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRC] = duty_cycle_50_percent;
        p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRE] = duty_cycle_50_percent;
    }
#endif

    /* If the counter is not counting, update period register and reset counter. */
    if (0U == p_instance_ctrl->p_reg->GTCR_b.CST)
    {
        p_instance_ctrl->p_reg->GTPR = new_gtpr;

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE
        p_instance_ctrl->p_reg->GTBER = GPT_PRV_GTBER_BUFFER_ENABLE_FORCE_TRANSFER;
#endif

        p_instance_ctrl->p_reg->GTCLR = p_instance_ctrl->channel_mask;
    }

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Sets duty cycle on requested pin. Implements @ref timer_api_t::dutyCycleSet.
 *
 * Duty cycle is updated in the buffer register. The updated duty cycle is reflected after the next cycle end (counter
 * overflow).
 *
 * @param[in] p_ctrl                   Pointer to instance control block.
 * @param[in] duty_cycle_counts        Duty cycle to set in counts.
 * @param[in] pin                      Use gpt_io_pin_t to select GPT_IO_PIN_GTIOCA, GPT_IO_PIN_GTIOCB, or
 *                                     GPT_IO_PIN_GTIOCA_AND_GTIOCB.
 *
 * @retval FSP_SUCCESS                 Duty cycle updated successfully.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL or the pin is not one of gpt_io_pin_t
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 * @retval FSP_ERR_INVALID_ARGUMENT    Duty cycle is larger than period.
 * @retval FSP_ERR_UNSUPPORTED         GPT_CFG_OUTPUT_SUPPORT_ENABLE is 0.
 **********************************************************************************************************************/
fsp_err_t R_GPT_DutyCycleSet (timer_ctrl_t * const p_ctrl, uint32_t const duty_cycle_counts, uint32_t const pin)
{
#if GPT_CFG_OUTPUT_SUPPORT_ENABLE
    uint32_t              tmp_pin         = pin & 3U;
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
 #if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(tmp_pin <= GPT_IO_PIN_GTIOCA_AND_GTIOCB);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(duty_cycle_counts <= (p_instance_ctrl->p_reg->GTPBR + 1), FSP_ERR_INVALID_ARGUMENT);
 #endif

    /* Set duty cycle. */
    gpt_prv_duty_registers_t duty_regs = {UINT32_MAX, 0};
    gpt_calculate_duty_cycle(p_instance_ctrl, duty_cycle_counts, &duty_regs);

    r_gpt_write_protect_disable(p_instance_ctrl);

    /* Only update GTCCR if 0% or 100% duty is not requested */
    if (!duty_regs.omdty)
    {
        uint32_t reg_offset = 2U;
        if (0 != (pin & GPT_IO_PIN_GTIOCA_AND_GTIOCB))
        {
            p_instance_ctrl->p_reg->GTCCR[reg_offset]     = duty_regs.gtccr_buffer;
            p_instance_ctrl->p_reg->GTCCR[reg_offset + 1] = duty_regs.gtccr_buffer;
        }
        else
        {
            p_instance_ctrl->p_reg->GTCCR[tmp_pin + reg_offset] = duty_regs.gtccr_buffer;
        }
    }

    /* Read modify write bitfield access is used to update GTUDDTYC to make sure we don't clobber settings for the
     * other pin. */

    uint32_t gtuddtyc = p_instance_ctrl->p_reg->GTUDDTYC;
    if (GPT_IO_PIN_GTIOCB != tmp_pin)
    {
        /* GTIOCA or both GTIOCA and GTIOCB. */
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtuddtyc &= ~R_GPT7_GTUDDTYC_OADTY_Msk;
        gtuddtyc |= duty_regs.omdty << R_GPT7_GTUDDTYC_OADTY_Pos;
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtuddtyc &= (uint32_t) ~R_GPT09_0_GTUDDTYC_OADTY_Msk;
        gtuddtyc |= duty_regs.omdty << R_GPT09_0_GTUDDTYC_OADTY_Pos;
 #endif
    }

    if (GPT_IO_PIN_GTIOCA != tmp_pin)
    {
        /* GTIOCB or both GTIOCA and GTIOCB. */
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtuddtyc &= ~R_GPT7_GTUDDTYC_OBDTY_Msk;
        gtuddtyc |= duty_regs.omdty << R_GPT7_GTUDDTYC_OBDTY_Pos;
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtuddtyc &= (uint32_t) ~R_GPT09_0_GTUDDTYC_OBDTY_Msk;
        gtuddtyc |= duty_regs.omdty << R_GPT09_0_GTUDDTYC_OBDTY_Pos;
 #endif
    }

    p_instance_ctrl->p_reg->GTUDDTYC = gtuddtyc;

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(duty_cycle_counts);
    FSP_PARAMETER_NOT_USED(pin);

    FSP_RETURN(FSP_ERR_UNSUPPORTED);
#endif
}

/*******************************************************************************************************************//**
 * Get timer information and store it in provided pointer p_info. Implements @ref timer_api_t::infoGet.
 *
 * @retval FSP_SUCCESS                 Period, count direction, frequency, and ELC event written to caller's
 *                                     structure successfully.(External clock(GTETRGA - GTETRGD) cannot be acquired.)
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_info was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_InfoGet (timer_ctrl_t * const p_ctrl, timer_info_t * const p_info)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_info);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Get and store period */
    uint32_t gtpr          = p_instance_ctrl->p_reg->GTPR;
    uint32_t period_counts = gtpr + 1;
#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    if (p_instance_ctrl->p_cfg->mode >= TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM)
    {
        period_counts = gtpr;
    }
#endif
    p_info->period_counts = period_counts;

    /* Get and store clock frequency */
    p_info->clock_frequency = gpt_clock_frequency_get(p_instance_ctrl);

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
fsp_err_t R_GPT_StatusGet (timer_ctrl_t * const p_ctrl, timer_status_t * const p_status)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_status);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Get counter state. */
    p_status->state = (timer_state_t) p_instance_ctrl->p_reg->GTCR_b.CST;

    /* Get counter value */
    p_status->counter = p_instance_ctrl->p_reg->GTCNT;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Set counter value.
 *
 * @note Do not call this API while the counter is counting.  The counter value can only be updated while the counter
 * is stopped.
 *
 * @retval FSP_SUCCESS                 Counter value updated.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_status was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 * @retval FSP_ERR_IN_USE              The timer is running.  Stop the timer before calling this function.
 **********************************************************************************************************************/
fsp_err_t R_GPT_CounterSet (timer_ctrl_t * const p_ctrl, uint32_t counter)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(0U == p_instance_ctrl->p_reg->GTCR_b.CST, FSP_ERR_IN_USE);
#endif

    r_gpt_write_protect_disable(p_instance_ctrl);

    /* Set counter value */
    p_instance_ctrl->p_reg->GTCNT = counter;

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Enable output for GTIOCA and/or GTIOCB.
 *
 * @retval FSP_SUCCESS                 Output is enabled.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_status was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_OutputEnable (timer_ctrl_t * const p_ctrl, gpt_io_pin_t pin)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    r_gpt_write_protect_disable(p_instance_ctrl);

    uint32_t gtior = p_instance_ctrl->p_reg->GTIOR;
    if (GPT_IO_PIN_GTIOCB != pin)
    {
        /* GTIOCA or both GTIOCA and GTIOCB. */
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= R_GPT7_GTIOR_OAE_Msk;
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= R_GPT09_0_GTIOR_OAE_Msk;
#endif
    }

    if (GPT_IO_PIN_GTIOCA != pin)
    {
        /* GTIOCB or both GTIOCA and GTIOCB. */
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= R_GPT7_GTIOR_OBE_Msk;
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= R_GPT09_0_GTIOR_OBE_Msk;
#endif
    }

    p_instance_ctrl->p_reg->GTIOR = gtior;

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Disable output for GTIOCA and/or GTIOCB.
 *
 * @retval FSP_SUCCESS                 Output is disabled.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_status was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_OutputDisable (timer_ctrl_t * const p_ctrl, gpt_io_pin_t pin)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    r_gpt_write_protect_disable(p_instance_ctrl);

    uint32_t gtior = p_instance_ctrl->p_reg->GTIOR;
    if (GPT_IO_PIN_GTIOCB != pin)
    {
        /* GTIOCA or both GTIOCA and GTIOCB. */
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior &= ~R_GPT7_GTIOR_OAE_Msk;
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior &= (uint32_t) ~R_GPT09_0_GTIOR_OAE_Msk;
#endif
    }

    if (GPT_IO_PIN_GTIOCA != pin)
    {
        /* GTIOCB or both GTIOCA and GTIOCB. */
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior &= ~R_GPT7_GTIOR_OBE_Msk;
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior &= (uint32_t) ~R_GPT09_0_GTIOR_OBE_Msk;
#endif
    }

    p_instance_ctrl->p_reg->GTIOR = gtior;

    r_gpt_write_protect_enable(p_instance_ctrl);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Set A/D converter start request compare match value.
 *
 * @retval FSP_SUCCESS                 Counter value updated.
 * @retval FSP_ERR_ASSERTION           p_ctrl or p_status was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_AdcTriggerSet (timer_ctrl_t * const    p_ctrl,
                               gpt_adc_compare_match_t which_compare_match,
                               uint32_t                compare_match_value)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ASSERT(p_instance_ctrl->p_cfg->channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL);
#endif

    r_gpt_write_protect_disable(p_instance_ctrl);

    /* Set A/D converter start request compare match value. */
    volatile uint32_t * p_gtadtr = &p_instance_ctrl->p_reg->GTADTRA;
    p_gtadtr[which_compare_match] = compare_match_value;

    r_gpt_write_protect_enable(p_instance_ctrl);

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
fsp_err_t R_GPT_CallbackSet (timer_ctrl_t * const          p_ctrl,
                             void (                      * p_callback)(timer_callback_args_t *),
                             void const * const            p_context,
                             timer_callback_args_t * const p_callback_memory)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;

#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Stops counter, disables output pins, and clears internal driver data. Implements @ref timer_api_t::close.
 *
 * @retval FSP_SUCCESS                 Successful close.
 * @retval FSP_ERR_ASSERTION           p_ctrl was NULL.
 * @retval FSP_ERR_NOT_OPEN            The instance is not opened.
 **********************************************************************************************************************/
fsp_err_t R_GPT_Close (timer_ctrl_t * const p_ctrl)
{
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) p_ctrl;
    fsp_err_t             err             = FSP_SUCCESS;

#if GPT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(GPT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Clear open flag. */
    p_instance_ctrl->open = 0U;

    r_gpt_write_protect_disable(p_instance_ctrl);

    /* Stop counter. */
    p_instance_ctrl->p_reg->GTSTP = p_instance_ctrl->channel_mask;

    /* Disable output. */
    p_instance_ctrl->p_reg->GTIOR = 0U;

    r_gpt_write_protect_enable(p_instance_ctrl);

    /* Disable interrupts. */
    gpt_disable_interrupt(p_instance_ctrl);

    return err;
}

/** @} (end addtogroup GPT) */

/*******************************************************************************************************************//**
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Enables write protection.
 *
 * @param[in]  p_instance_ctrl         Instance control block.
 **********************************************************************************************************************/
static inline void r_gpt_write_protect_enable (gpt_instance_ctrl_t * const p_instance_ctrl)
{
#if GPT_CFG_WRITE_PROTECT_ENABLE
    p_instance_ctrl->p_reg->GTWP = GPT_PRV_GTWP_WRITE_PROTECT;
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
#endif
}

/*******************************************************************************************************************//**
 * Disables write protection.
 *
 * @param[in]  p_instance_ctrl         Instance control block.
 **********************************************************************************************************************/
static inline void r_gpt_write_protect_disable (gpt_instance_ctrl_t * const p_instance_ctrl)
{
#if GPT_CFG_WRITE_PROTECT_ENABLE
    p_instance_ctrl->p_reg->GTWP = GPT_PRV_GTWP_RESET_VALUE;
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
#endif
}

/*******************************************************************************************************************//**
 * Validates the configuration arguments for illegal combinations or options.
 *
 * @param[in]  p_cfg                      Pointer to timer configuration.
 *
 * @retval FSP_SUCCESS                    No configuration errors detected
 * @retval FSP_ERR_ASSERTION              An input argument is invalid.
 * @retval FSP_ERR_IRQ_BSP_DISABLED       timer_cfg_t::mode is ::TIMER_MODE_ONE_SHOT or timer_cfg_t::p_callback is not
 *                                        NULL, but ISR is not enabled.  ISR must be enabled to use one-shot mode or
 *                                        callback.
 * @retval FSP_ERR_IP_CHANNEL_NOT_PRESENT The channel requested in the p_cfg parameter is not available on this device.
 **********************************************************************************************************************/
static fsp_err_t r_gpt_open_cfg_check (timer_cfg_t const * const p_cfg)
{
#if GPT_CFG_PARAM_CHECKING_ENABLE
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    FSP_ERROR_RETURN((p_cfg->channel <= GPT_CHANNEL_UNIT2_3), FSP_ERR_IP_CHANNEL_NOT_PRESENT);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    FSP_ERROR_RETURN((p_cfg->channel < BSP_FEATURE_GPT_CHANNEL), FSP_ERR_IP_CHANNEL_NOT_PRESENT);
 #endif

    if ((p_cfg->p_callback) || (TIMER_MODE_ONE_SHOT == p_cfg->mode))
    {
        FSP_ERROR_RETURN(p_cfg->cycle_end_irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);
    }

 #if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE

    /* Callback is required if underflow interrupt is enabled. */
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_cfg->p_extend;

    gpt_extended_pwm_cfg_t const * p_pwm_cfg = p_extend->p_pwm_cfg;
    if (NULL != p_pwm_cfg)
    {
        if (p_pwm_cfg->trough_irq >= 0)
        {
            FSP_ASSERT(NULL != p_cfg->p_callback);
        }
    }
 #endif
#else
    FSP_PARAMETER_NOT_USED(p_cfg);
#endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Initializes control structure based on configuration.
 *
 * @param[in]  p_instance_ctrl         Instance control block.
 * @param[in]  p_cfg                   Pointer to timer configuration.
 **********************************************************************************************************************/
static void gpt_common_open (gpt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg)
{
    /* Initialize control structure.  */
    p_instance_ctrl->p_cfg = p_cfg;

    /* If callback is not null or timer mode is one shot, make sure the IRQ is enabled and store callback in the
     *  control block.
     *  @note The GPT hardware does not support one-shot mode natively.  To support one-shot mode, the timer will be
     *  stopped and cleared using software in the ISR. *//* Determine if this is a 32-bit or a 16-bit timer. */
    p_instance_ctrl->variant = TIMER_VARIANT_16_BIT;
    if (0U != (p_instance_ctrl->channel_mask & BSP_FEATURE_GPT_32BIT_CHANNEL_MASK))
    {
        p_instance_ctrl->variant = TIMER_VARIANT_32_BIT;
    }

#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE

    /* Save register base address. */
    uint32_t base_address;
    if (p_cfg->channel <= GPT_CHANNEL_UNIT0_6)
    {
        base_address = (uint32_t) R_GPT0 + ((uint32_t) p_cfg->channel * ((uint32_t) R_GPT1 - (uint32_t) R_GPT0));
    }
    else if ((GPT_CHANNEL_UNIT1_0 <= p_cfg->channel) && (p_cfg->channel <= GPT_CHANNEL_UNIT1_6))
    {
        base_address = (uint32_t) R_GPT7 +
                       (((uint32_t) p_cfg->channel - GPT_CHANNEL_UNIT1_0) * ((uint32_t) R_GPT8 - (uint32_t) R_GPT7));
    }
    else
    {
        base_address = (uint32_t) R_GPT14 +
                       (((uint32_t) p_cfg->channel - GPT_CHANNEL_UNIT2_0) * ((uint32_t) R_GPT15 - (uint32_t) R_GPT14));
    }

#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE

    /* Save register base address. */
    uintptr_t base_address;
 #if (BSP_FEATURE_GPT_LLPP1_BASE_CHANNEL != 0)
    if (p_cfg->channel < BSP_FEATURE_GPT_LLPP1_BASE_CHANNEL)
 #else
    if (p_cfg->channel < BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL)
 #endif
    {
        /* LLPP base address setting */
        base_address = (uintptr_t) BSP_FEATURE_GPT_LLPP_BASE_ADDRESS +
                       (uintptr_t) p_cfg->channel / BSP_FEATURE_GPT_LLPP_CHANNEL_PER_UNIT *
                       BSP_FEATURE_GPT_LLPP_UNIT_ADDRESS_OFFSET +
                       (uintptr_t) p_cfg->channel % BSP_FEATURE_GPT_LLPP_CHANNEL_PER_UNIT *
                       BSP_FEATURE_GPT_LLPP_CHANNEL_ADDRESS_OFFSET;
    }

 #if (BSP_FEATURE_GPT_LLPP1_BASE_CHANNEL != 0)
    else if ((p_cfg->channel >= BSP_FEATURE_GPT_LLPP1_BASE_CHANNEL) &&
             (p_cfg->channel < BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL))
    {
        /* LLPP1 base address setting */
        base_address = (uintptr_t) BSP_FEATURE_GPT_LLPP1_BASE_ADDRESS +
                       ((uintptr_t) p_cfg->channel - BSP_FEATURE_GPT_LLPP1_BASE_CHANNEL) /
                       BSP_FEATURE_GPT_LLPP_CHANNEL_PER_UNIT *
                       BSP_FEATURE_GPT_LLPP1_UNIT_ADDRESS_OFFSET +
                       (uintptr_t) p_cfg->channel % BSP_FEATURE_GPT_LLPP_CHANNEL_PER_UNIT *
                       BSP_FEATURE_GPT_LLPP1_CHANNEL_ADDRESS_OFFSET;
    }
 #endif
    else if ((p_cfg->channel >= BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL) &&
             (p_cfg->channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL))
    {
        base_address =
            (uintptr_t) BSP_FEATURE_GPT_NONSAFETY_BASE_ADDRESS +
            ((uintptr_t) (p_cfg->channel - BSP_FEATURE_GPT_NONSAFETY_BASE_CHANNEL) *
             BSP_FEATURE_GPT_NONSAFETY_CHANNEL_ADDRESS_OFFSET);
    }
    else
    {
        base_address =
            (uintptr_t) BSP_FEATURE_GPT_SAFETY_BASE_ADDRESS +
            ((uintptr_t) (p_cfg->channel - BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL) *
             BSP_FEATURE_GPT_SAFETY_CHANNEL_ADDRESS_OFFSET);
    }
#endif

#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    p_instance_ctrl->p_reg = (R_GPT0_Type *) base_address;
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    p_instance_ctrl->p_reg = (R_GPT00_0_Type *) base_address;
#endif
#if 1U == BSP_FEATURE_GPT_INPUT_CAPTURE_SIGNAL_SELECTABLE

    /* Since it's a common channel, it keeps the previous value. */
    p_instance_ctrl->p_reg_com = R_GPT_IC;
#endif

    /* Set callback and context pointers, if configured */
    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;
}

/*******************************************************************************************************************//**
 * Performs hardware initialization of the GPT.
 *
 * @param[in]  p_instance_ctrl        Instance control block.
 * @param[in]  p_cfg                  Pointer to timer configuration.
 **********************************************************************************************************************/
static void gpt_hardware_initialize (gpt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg)
{
    /* Save pointer to extended configuration structure. */
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_cfg->p_extend;

    /* Counter related initialization */
    gpt_counter_initialize(p_instance_ctrl, p_cfg);

    uint32_t gtuddtyc = 0U;
    uint32_t gtior    = 0U;

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE

    /* For one shot mode, the compare match buffer register must be loaded with a value that exceeds the timer
     * cycle end value so that second compare match event would never occur and hence there will be only a
     * single pulse.  Writing to the upper bits is ignored for 16-bit timers. */
    gpt_prv_duty_registers_t duty_regs = {UINT32_MAX, 0};

    if (TIMER_MODE_PERIODIC == p_cfg->mode)
    {
        /* The  GTIOCA/GTIOCB pins transition 1 cycle after compare match when buffer operation is used (see
         * Section "PWM Output Operating Mode" in the RZ microprocessor User's Manual for details). To get a duty cycle
         * as close to 50% as possible, duty cycle (register) = (period (counts) / 2) - 1. */
        uint32_t duty_cycle_50_percent = (p_cfg->period_counts >> 1) - 1U;
        duty_regs.gtccr_buffer = duty_cycle_50_percent;
    }

    if (p_cfg->mode >= TIMER_MODE_PWM)
    {
        gpt_calculate_duty_cycle(p_instance_ctrl, p_cfg->duty_cycle_counts, &duty_regs);
    }

    /* Set the compare match and compare match buffer registers based on previously calculated values. */
    p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRC] = duty_regs.gtccr_buffer;
    p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRE] = duty_regs.gtccr_buffer;

    /* If the requested duty cycle is 0% or 100%, set this in the registers. */
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    gtuddtyc |= duty_regs.omdty << R_GPT7_GTUDDTYC_OADTY_Pos;
    gtuddtyc |= duty_regs.omdty << R_GPT7_GTUDDTYC_OBDTY_Pos;
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    gtuddtyc |= duty_regs.omdty << R_GPT09_0_GTUDDTYC_OADTY_Pos;
    gtuddtyc |= duty_regs.omdty << R_GPT09_0_GTUDDTYC_OBDTY_Pos;
 #endif

    /* Calculate GTIOR. */
    if (p_extend->gtioca.output_enabled)
    {
        uint32_t gtioca_gtior = gpt_gtior_calculate(p_cfg, p_extend->gtioca.stop_level);
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= gtioca_gtior << R_GPT7_GTIOR_GTIOA_Pos;
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= gtioca_gtior << R_GPT09_0_GTIOR_GTIOA_Pos;
 #endif
    }

    if (p_extend->gtiocb.output_enabled)
    {
        uint32_t gtiocb_gtior = gpt_gtior_calculate(p_cfg, p_extend->gtiocb.stop_level);
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= gtiocb_gtior << R_GPT7_GTIOR_GTIOB_Pos;
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= gtiocb_gtior << R_GPT09_0_GTIOR_GTIOB_Pos;
 #endif
    }
#endif

    /* It must be cleared before setting. When modifying the
     * IVTT[2:0] bits, first set the IVTC[1:0] bits to 00b. */
    p_instance_ctrl->p_reg->GTITC  = 0U;
    p_instance_ctrl->p_reg->GTEITC = 0U;

#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    gpt_extended_pwm_cfg_t const * p_pwm_cfg = p_extend->p_pwm_cfg;
    if (NULL != p_pwm_cfg)
    {
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        p_instance_ctrl->p_reg->GTINTAD = ((uint32_t) p_pwm_cfg->output_disable << R_GPT7_GTINTAD_GRPDTE_Pos) |
                                          ((uint32_t) p_pwm_cfg->poeg_link << R_GPT7_GTINTAD_GRP_Pos) |
                                          ((uint32_t) p_pwm_cfg->adc_trigger << R_GPT7_GTINTAD_ADTRAUEN_Pos);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        p_instance_ctrl->p_reg->GTINTAD = ((uint32_t) p_pwm_cfg->output_disable << R_GPT09_0_GTINTAD_GRPDTE_Pos) |
                                          ((uint32_t) p_pwm_cfg->poeg_link << R_GPT09_0_GTINTAD_GRP_Pos) |
                                          ((uint32_t) p_pwm_cfg->adc_trigger << R_GPT09_0_GTINTAD_ADTRAUEN_Pos);
 #endif
        p_instance_ctrl->p_reg->GTDVU = p_pwm_cfg->dead_time_count_up;

        /* Set GTDTCR.TDE only if one of the dead time values is non-zero. */
        uint32_t gtdtcr =
            ((p_pwm_cfg->dead_time_count_up > 0) || (p_pwm_cfg->dead_time_count_down > 0));
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        p_instance_ctrl->p_reg->GTITC = ((uint32_t) p_pwm_cfg->interrupt_skip_source << R_GPT7_GTITC_IVTC_Pos) |
                                        ((uint32_t) p_pwm_cfg->interrupt_skip_count << R_GPT7_GTITC_IVTT_Pos) |
                                        ((uint32_t) p_pwm_cfg->interrupt_skip_adc << R_GPT7_GTITC_ADTAL_Pos);
        p_instance_ctrl->p_reg->GTEITC =
            ((uint32_t) p_pwm_cfg->interrupt_skip_source_ext1 << R_GPT7_GTEITC_EIVTC1_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_count_ext1 << R_GPT7_GTEITC_EIVTT1_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_source_ext2 <<
                R_GPT7_GTEITC_EIVTC2_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_count_ext2 << R_GPT7_GTEITC_EIVTT2_Pos);
        p_instance_ctrl->p_reg->GTEITLI1 =
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_ovf << R_GPT7_GTEITLI1_EITLV_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_unf << R_GPT7_GTEITLI1_EITLU_Pos);
        p_instance_ctrl->p_reg->GTEITLI2 =
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_adc_a << R_GPT7_GTEITLI2_EADTAL_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_adc_b <<
                R_GPT7_GTEITLI2_EADTBL_Pos);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        p_instance_ctrl->p_reg->GTITC = ((uint32_t) p_pwm_cfg->interrupt_skip_source << R_GPT09_0_GTITC_IVTC_Pos) |
                                        ((uint32_t) p_pwm_cfg->interrupt_skip_count << R_GPT09_0_GTITC_IVTT_Pos) |
                                        ((uint32_t) p_pwm_cfg->interrupt_skip_adc << R_GPT09_0_GTITC_ADTAL_Pos);
        p_instance_ctrl->p_reg->GTEITC =
            ((uint32_t) p_pwm_cfg->interrupt_skip_source_ext1 << R_GPT09_0_GTEITC_EIVTC1_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_count_ext1 << R_GPT09_0_GTEITC_EIVTT1_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_source_ext2 <<
                R_GPT09_0_GTEITC_EIVTC2_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_count_ext2 << R_GPT09_0_GTEITC_EIVTT2_Pos);
        p_instance_ctrl->p_reg->GTEITLI1 =
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_ovf << R_GPT09_0_GTEITLI1_EITLV_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_unf << R_GPT09_0_GTEITLI1_EITLU_Pos);
        p_instance_ctrl->p_reg->GTEITLI2 =
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_adc_a << R_GPT09_0_GTEITLI2_EADTAL_Pos) |
            ((uint32_t) p_pwm_cfg->interrupt_skip_func_adc_b <<
                R_GPT09_0_GTEITLI2_EADTBL_Pos);
 #endif
        p_instance_ctrl->p_reg->GTDVD = p_pwm_cfg->dead_time_count_down;

 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE

        /* GADTRm does not exist in the SAFETY area, so it is not accessed. */
        if (0 ==
            ((1U << (p_cfg->channel)) & (BSP_FEATURE_GPT_SAFETY_CHANNEL_MASK << BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)))
        {
            p_instance_ctrl->p_reg->GTADTRA = p_pwm_cfg->adc_a_compare_match;
            p_instance_ctrl->p_reg->GTADTRB = p_pwm_cfg->adc_b_compare_match;
        }

 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        if (p_cfg->channel < BSP_FEATURE_GPT_SAFETY_BASE_CHANNEL)
        {
            p_instance_ctrl->p_reg->GTADTRA = p_pwm_cfg->adc_a_compare_match;
            p_instance_ctrl->p_reg->GTADTRB = p_pwm_cfg->adc_b_compare_match;
        }
 #endif

        /* If custom GTIOR settings are not provided, set gtioca_disable_settings and gtiocb_disable_settings. */
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= (uint32_t) (p_pwm_cfg->gtioca_disable_setting << R_GPT7_GTIOR_OADF_Pos);
        gtior |= (uint32_t) (p_pwm_cfg->gtiocb_disable_setting << R_GPT7_GTIOR_OBDF_Pos);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtior |= (uint32_t) (p_pwm_cfg->gtioca_disable_setting << R_GPT09_0_GTIOR_OADF_Pos);
        gtior |= (uint32_t) (p_pwm_cfg->gtiocb_disable_setting << R_GPT09_0_GTIOR_OBDF_Pos);
 #endif
        p_instance_ctrl->p_reg->GTDTCR = gtdtcr;
    }
    else
#endif
    {
        /* GTADTR* registers are unused if GTINTAD is cleared. */
        p_instance_ctrl->p_reg->GTINTAD = 0U;
        p_instance_ctrl->p_reg->GTDTCR  = 0U;

        /* GTDVU, GTDVD, GTDBU, GTDBD, and GTSOTR are not used if GTDTCR is cleared. */
    }

    /* Configure the noise filter for the GTIOC pins. */
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    gtior |= (uint32_t) (p_extend->capture_filter_gtioca << R_GPT7_GTIOR_NFAEN_Pos);
    gtior |= (uint32_t) (p_extend->capture_filter_gtiocb << R_GPT7_GTIOR_NFBEN_Pos);
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    gtior |= (uint32_t) (p_extend->capture_filter_gtioca << R_GPT09_0_GTIOR_NFAEN_Pos);
    gtior |= (uint32_t) (p_extend->capture_filter_gtiocb << R_GPT09_0_GTIOR_NFBEN_Pos);
#endif

    /* Enable the compare match buffer. */
    p_instance_ctrl->p_reg->GTBER = GPT_PRV_GTBER_BUFFER_ENABLE_FORCE_TRANSFER;

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE
    if (TIMER_MODE_ONE_SHOT == p_cfg->mode)
    {
        /* In one shot mode, the output pin toggles when counting starts, then again when the period expires.
         * The buffer is enabled to set the compare match to UINT32_MAX after the one shot pulse is output
         * so that the pin level will not change if the period expires again before the timer is stopped in
         * the interrupt.*/
        p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRA] = 0U;
        p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRB] = 0U;
    }
#endif

    /* Reset counter to 0. */
    p_instance_ctrl->p_reg->GTCLR = p_instance_ctrl->channel_mask;

    /* Set the I/O control register. */
    p_instance_ctrl->p_reg->GTIOR = gtior;

    /* Configure duty cycle and force timer to count up. GTUDDTYC must be set, then cleared to force the count
     * direction to be reflected when counting starts (see Section "General PWM Timer Count Direction
     * and Duty Setting Register (GTUDDTYC)" in the RZ microprocessor User's Manual for details). */
    p_instance_ctrl->p_reg->GTUDDTYC = gtuddtyc | 3U;
    p_instance_ctrl->p_reg->GTUDDTYC = gtuddtyc | 1U;

#if 1U == BSP_FEATURE_GPT_INPUT_CAPTURE_SIGNAL_SELECTABLE
    p_instance_ctrl->p_reg_com->GTIOCSEL |= p_extend->gtioc_isel;
#endif

    r_gpt_write_protect_enable(p_instance_ctrl);

    gpt_enable_interrupt(p_instance_ctrl);
}

/*******************************************************************************************************************//**
 * Counter initialization of the GPT.
 *
 * @param[in]  p_instance_ctrl        Instance control block.
 * @param[in]  p_cfg                  Pointer to timer configuration.
 **********************************************************************************************************************/
static void gpt_counter_initialize (gpt_instance_ctrl_t * const p_instance_ctrl, timer_cfg_t const * const p_cfg)
{
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_cfg->p_extend;

    /* Power on GPT before setting any hardware registers. Make sure the counter is stopped before setting mode
     * register, PCLK divisor register, and counter register. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_GPT, p_cfg->channel);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Initialize all registers that may affect operation of this driver to reset values.  Skip these since they
     * affect all channels, and are initialized in GTCR and GTCNT: GTSTR, GTSTP, GTCLR. GTCR is set immediately after
     * clearing the module stop bit to ensure the timer is stopped before proceeding with configuration. */
    p_instance_ctrl->p_reg->GTWP  = GPT_PRV_GTWP_RESET_VALUE;
    p_instance_ctrl->p_reg->GTCR  = 0U;
    p_instance_ctrl->p_reg->GTST  = 0U;
    p_instance_ctrl->p_reg->GTCNT = 0U;

    /* GTPR, GTCCRn, GTIOR, GTSSR, GTPSR, GTCSR, GTUPSR, GTDNSR, GTPBR, and GTUDDTYC are set by this driver. */

    /* Initialization sets all register required for up counting as described in hardware manual
     * (see Section "Counter Operation" in the RZ microprocessor User's Manual for details) and
     * other registers required by the driver. */

    /* Dividers for GPT are half the enum value. */
    uint32_t gtcr_tpcs = p_cfg->source_div;
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    uint32_t gtcr = ((uint32_t) (p_extend->icds << R_GPT7_GTCR_ICDS_Pos) | (gtcr_tpcs << R_GPT7_GTCR_TPCS_Pos));
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    uint32_t gtcr = ((uint32_t) (p_extend->icds << R_GPT09_0_GTCR_ICDS_Pos) | (gtcr_tpcs << R_GPT09_0_GTCR_TPCS_Pos));
#endif

    /* Store period register setting. The actual period and is one cycle longer than the register value for saw waves
     * and twice the register value for triangle waves. Reference section "General PWM Timer Cycle Setting
     * Register (GTPR)". The setting passed to the configuration is expected to be half the desired period for
     * triangle waves. */
    uint32_t gtpr = p_cfg->period_counts - 1U;
#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE

    /* Saw-wave PWM mode is set in GTCR.MD for all modes except TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM and
     * TIMER_MODE_TRIANGLE_WAVE_ASYMMETRIC_PWM. */
    if (p_cfg->mode >= TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM)
    {
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtcr |= ((uint32_t) p_cfg->mode << R_GPT7_GTCR_MD_Pos);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        gtcr |= ((uint32_t) p_cfg->mode << R_GPT09_0_GTCR_MD_Pos);
 #endif
        gtpr = p_cfg->period_counts;
    }
#endif

    /* Counter must be stopped to update TPCS (see Section "General PWM Timer Control Register (GTCR)"
     * in the RZ microprocessor User's Manual for details). */
    p_instance_ctrl->p_reg->GTCR = gtcr;

    gpt_hardware_events_disable(p_instance_ctrl);

    /* Configure the up/down count sources. These are not affected by enable/disable. */
    p_instance_ctrl->p_reg->GTUPSR = p_extend->count_up_source;
    p_instance_ctrl->p_reg->GTDNSR = p_extend->count_down_source;

    /* Set period. The actual period is one cycle longer than the register value. Reference section 19.2.21
     * "General PWM Timer Cycle Setting Register (GTPR)". */
    p_instance_ctrl->p_reg->GTPBR = gtpr;
    p_instance_ctrl->p_reg->GTPR  = gtpr;
}

/*******************************************************************************************************************//**
 * Disables hardware events that would cause the timer to start, stop, clear, or capture.
 *
 * @param[in]  p_instance_ctrl         Instance control structure
 **********************************************************************************************************************/
static void gpt_hardware_events_disable (gpt_instance_ctrl_t * p_instance_ctrl)
{
    /* Enable use of GTSTR, GTSTP, and GTCLR for this channel. */
    p_instance_ctrl->p_reg->GTSSR   = GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE;
    p_instance_ctrl->p_reg->GTPSR   = GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE;
    p_instance_ctrl->p_reg->GTCSR   = GPT_PRV_ENABLE_GROUP_SOFTWARE_UPDATE;
    p_instance_ctrl->p_reg->GTICASR = GPT_SOURCE_NONE;
    p_instance_ctrl->p_reg->GTICBSR = GPT_SOURCE_NONE;
}

/*******************************************************************************************************************//**
 * Disables interrupt if it is a valid vector number.
 *
 * @param[in]  irq                     Interrupt number
 **********************************************************************************************************************/
static void r_gpt_disable_irq (IRQn_Type irq)
{
    /* Disable interrupts. */
    if (irq >= 0)
    {
        R_BSP_IrqDisable(irq);
        R_FSP_IsrContextSet(irq, NULL);
    }
}

/*******************************************************************************************************************//**
 * Configures and enables interrupt if it is a valid vector number.
 *
 * @param[in]  irq                     Interrupt number
 * @param[in]  priority                NVIC priority of the interrupt
 * @param[in]  p_context               The interrupt context is a pointer to data required in the ISR.
 **********************************************************************************************************************/
static void r_gpt_enable_irq (IRQn_Type const irq, uint32_t priority, void * p_context)
{
    if (irq >= 0)
    {
        R_BSP_IrqCfgEnable(irq, priority, p_context);
    }
}

/*******************************************************************************************************************//**
 * Disable interrupts
 *
 * @param[in]  p_instance_ctrl           Instance control block
 **********************************************************************************************************************/
static void gpt_disable_interrupt (gpt_instance_ctrl_t * const p_instance_ctrl)
{
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;
    if (p_instance_ctrl->p_cfg->cycle_end_irq > FSP_INVALID_VECTOR)
    {
#if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        p_instance_ctrl->p_reg->GTINTAD &= ~(1U << R_GPT7_GTINTAD_GTINTPR_Pos);
#elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
        p_instance_ctrl->p_reg->GTINTAD &= ~(1U << R_GPT09_0_GTINTAD_GTINTPR_Pos);
#endif
    }

    if (p_extend->capture_a_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GTINTA = 0U;
    }

    if (p_extend->capture_b_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GTINTB = 0U;
    }

    if (p_extend->dead_time_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GRPDTE = 0U;
    }

#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Use the GPT_INTERRUPT_SELECT_DISABLE macro for disable the cycle end interrupt. */
    GPT_INTERRUPT_SELECT_DISABLE(p_instance_ctrl->p_cfg->cycle_end_irq, p_extend->cycle_end_source_select, OVF);

    /* Use the GPT_INTERRUPT_SELECT_DISABLE macro for disable interrupt capture a. */
    GPT_INTERRUPT_SELECT_DISABLE(p_extend->capture_a_irq, p_extend->capture_a_source_select, CCMPA);

    /* Use the GPT_INTERRUPT_SELECT_DISABLE macro for disable interrupt capture b. */
    GPT_INTERRUPT_SELECT_DISABLE(p_extend->capture_b_irq, p_extend->capture_b_source_select, CCMPB);

    /* Use the GPT_INTERRUPT_SELECT_DISABLE macro for disable interrupt deadtime error. */
    GPT_INTERRUPT_SELECT_DISABLE(p_extend->dead_time_irq, p_extend->dead_time_error_source_select, DTE);
#endif

    r_gpt_disable_irq(p_instance_ctrl->p_cfg->cycle_end_irq);
    r_gpt_disable_irq(p_extend->capture_a_irq);
    r_gpt_disable_irq(p_extend->capture_b_irq);
    r_gpt_disable_irq(p_extend->dead_time_irq);

#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    gpt_extended_pwm_cfg_t const * p_pwm_cfg = p_extend->p_pwm_cfg;
    if (NULL != p_pwm_cfg)
    {
        if (p_pwm_cfg->trough_irq > FSP_INVALID_VECTOR)
        {
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
            p_instance_ctrl->p_reg->GTINTAD &= ~(2U << R_GPT7_GTINTAD_GTINTPR_Pos);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
            p_instance_ctrl->p_reg->GTINTAD &= ~(2U << R_GPT09_0_GTINTAD_GTINTPR_Pos);
 #endif
        }

 #if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

        /* Use the GPT_INTERRUPT_SELECT_DISABLE macro for disable the trough interrupt. */
        GPT_INTERRUPT_SELECT_DISABLE(p_pwm_cfg->trough_irq, p_extend->trough_source_select, UDF);
 #endif

        r_gpt_disable_irq(p_pwm_cfg->trough_irq);
    }
#endif
}

/*******************************************************************************************************************//**
 * Enable interrupts
 *
 * @param[in]  p_instance_ctrl           Instance control block
 **********************************************************************************************************************/
static void gpt_enable_interrupt (gpt_instance_ctrl_t * const p_instance_ctrl)
{
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;

    /* Enable CPU interrupts if callback is not null.  Also enable interrupts for one shot mode.
     *  @note The GPT hardware does not support one-shot mode natively. To support one-shot mode, the timer will be
     *  stopped and cleared using software in the ISR. */
    if (p_instance_ctrl->p_cfg->cycle_end_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GTINTPR |= 0x01U;
    }

    if (p_extend->capture_a_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GTINTA = 1U;
    }

    if (p_extend->capture_b_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GTINTB = 1U;
    }

    if (p_extend->dead_time_irq > FSP_INVALID_VECTOR)
    {
        p_instance_ctrl->p_reg->GTINTAD_b.GRPDTE = 1U;
    }

#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* GPT_INT select  and combine enable */

    /* Use the GPT_INTERRUPT_SELECT_ENABLE macro for enable the cycle end interrupt. */
    GPT_INTERRUPT_SELECT_ENABLE(p_instance_ctrl->p_cfg->cycle_end_irq,
                                p_extend->cycle_end_source_select,
                                OVF,
                                gpt_counter_overflow_isr);

    /* Use the GPT_INTERRUPT_SELECT_ENABLE macro for enable interrupt capture a. */
    GPT_INTERRUPT_SELECT_ENABLE(p_extend->capture_a_irq, p_extend->capture_a_source_select, CCMPA, gpt_capture_a_isr);

    /* Use the GPT_INTERRUPT_SELECT_ENABLE macro for enable interrupt capture b. */
    GPT_INTERRUPT_SELECT_ENABLE(p_extend->capture_b_irq, p_extend->capture_b_source_select, CCMPB, gpt_capture_b_isr);

    /* Use the GPT_INTERRUPT_SELECT_ENABLE macro for enable interrupt deadtime error. */
    GPT_INTERRUPT_SELECT_ENABLE(p_extend->dead_time_irq, p_extend->dead_time_error_source_select, DTE,
                                gpt_dead_time_isr);
#endif

    r_gpt_enable_irq(p_instance_ctrl->p_cfg->cycle_end_irq, p_instance_ctrl->p_cfg->cycle_end_ipl, p_instance_ctrl);
    r_gpt_enable_irq(p_extend->capture_a_irq, p_extend->capture_a_ipl, p_instance_ctrl);
    r_gpt_enable_irq(p_extend->capture_b_irq, p_extend->capture_b_ipl, p_instance_ctrl);
    r_gpt_enable_irq(p_extend->dead_time_irq, p_extend->dead_time_ipl, p_instance_ctrl);
#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    gpt_extended_pwm_cfg_t const * p_pwm_cfg = p_extend->p_pwm_cfg;
    if (NULL != p_pwm_cfg)
    {
        if (p_pwm_cfg->trough_irq > FSP_INVALID_VECTOR)
        {
            p_instance_ctrl->p_reg->GTINTAD_b.GTINTPR |= 0x02U;
        }

 #if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

        /* Use the GPT_INTERRUPT_SELECT_ENABLE macro for enable the trough interrupt. */
        GPT_INTERRUPT_SELECT_ENABLE(p_pwm_cfg->trough_irq,
                                    p_extend->trough_source_select,
                                    UDF,
                                    gpt_counter_underflow_isr);
 #endif

        r_gpt_enable_irq(p_pwm_cfg->trough_irq, p_pwm_cfg->trough_ipl, p_instance_ctrl);
    }
#endif
}

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE

/*******************************************************************************************************************//**
 * Calculates duty cycle register values.  GTPBR must be set before entering this function.
 *
 * @param[in]  p_instance_ctrl         Instance control structure
 * @param[in]  duty_cycle_counts       Duty cycle to set
 * @param[out] p_duty_reg              Duty cycle register values
 **********************************************************************************************************************/
static void gpt_calculate_duty_cycle (gpt_instance_ctrl_t * const p_instance_ctrl,
                                      uint32_t const              duty_cycle_counts,
                                      gpt_prv_duty_registers_t  * p_duty_reg)
{
    /* Determine the current period. The actual period is one cycle longer than the register value for saw waves
     * and twice the register value for triangle waves. Reference section "General PWM Timer Cycle Setting Buffer
     * Register (GTPBR)". The setting passed to the configuration is expected to be half the desired duty cycle for
     * triangle waves. */
    uint32_t current_period = p_instance_ctrl->p_reg->GTPBR;
 #if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    if (p_instance_ctrl->p_cfg->mode < TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM)
 #endif
    {
        current_period++;
    }

    /* 0% and 100% duty cycle are supported in OADTY/OBDTY. */
    if (0U == duty_cycle_counts)
    {
        p_duty_reg->omdty = GPT_DUTY_CYCLE_MODE_0_PERCENT;
    }
    else if (duty_cycle_counts >= current_period)
    {
        p_duty_reg->omdty = GPT_DUTY_CYCLE_MODE_100_PERCENT;
    }
    else
    {
        uint32_t temp_duty_cycle = duty_cycle_counts;

 #if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
        if (p_instance_ctrl->p_cfg->mode >= TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM)
        {
            p_duty_reg->gtccr_buffer = temp_duty_cycle;
        }
        else
 #endif
        {
            /* The GTIOCA/GTIOCB pins transition 1 cycle after compare match when buffer operation is used (see
             * Section "PWM Output Operating Mode" in the RZ microprocessor User's Manual for details). */
            temp_duty_cycle--;
            p_duty_reg->gtccr_buffer = temp_duty_cycle;
        }
    }
}

#endif

/*******************************************************************************************************************//**
 * Calculates clock frequency of GPT counter.  Divides GPT clock by GPT clock divisor.
 *
 * @param[in]  p_instance_ctrl           Instance control block
 *
 * @return     Clock frequency of the GPT counter.
 **********************************************************************************************************************/
static uint32_t gpt_clock_frequency_get (gpt_instance_ctrl_t * const p_instance_ctrl)
{
    uint32_t pclk_freq_hz;

    /* Look up PCLK frequency and divide it by GPT PCLK divider. */
    timer_source_div_t pclk_divisor = (timer_source_div_t) (p_instance_ctrl->p_reg->GTCR_b.TPCS);
    if (0U < (p_instance_ctrl->channel_mask & BSP_FEATURE_GPT_LLPP_CHANNEL_MASK))
    {
        pclk_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKGPTL);
    }
    else
    {
        pclk_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKM);
    }

    return pclk_freq_hz >> pclk_divisor;
}

#if GPT_CFG_OUTPUT_SUPPORT_ENABLE

/*******************************************************************************************************************//**
 * Calculates GTIOR settings for given mode and stop level.
 *
 * @param[in]  p_instance_ctrl         Instance control block
 * @param[in]  p_cfg                   Timer configuration
 * @param[in]  level                   Output level after timer stops
 **********************************************************************************************************************/
static uint32_t gpt_gtior_calculate (timer_cfg_t const * const p_cfg, gpt_pin_level_t const stop_level)
{
    /* The stop level is used as both the initial level and the stop level. */
 #if 1U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    uint32_t gtior = R_GPT7_GTIOR_OAE_Msk | ((uint32_t) stop_level << R_GPT7_GTIOR_OADFLT_Pos) |
                     ((uint32_t) stop_level << GPT_PRV_GTIOR_INITIAL_LEVEL_BIT);
 #elif 2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE
    uint32_t gtior = R_GPT09_0_GTIOR_OAE_Msk | ((uint32_t) stop_level << R_GPT09_0_GTIOR_OADFLT_Pos) |
                     ((uint32_t) stop_level << GPT_PRV_GTIOR_INITIAL_LEVEL_BIT);
 #endif
    uint32_t gtion = GPT_PRV_GTIO_LOW_COMPARE_MATCH_HIGH_CYCLE_END;

    if (TIMER_MODE_PWM == p_cfg->mode)
    {
        /* Use default: GTIOn is high at cycle end, then low at compare match. */
    }

 #if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    else if (p_cfg->mode >= TIMER_MODE_TRIANGLE_WAVE_SYMMETRIC_PWM)
    {
        gtion = GPT_PRV_GTIO_TOGGLE_COMPARE_MATCH;
    }
 #endif
    else
    {
        /* In one-shot mode, the output pin goes high after the first compare match (one cycle after the timer starts counting). */
        if (GPT_PIN_LEVEL_LOW == stop_level)
        {
            gtion = GPT_PRV_GTIO_HIGH_COMPARE_MATCH_LOW_CYCLE_END;
        }
    }

    gtior |= gtion;

    return gtior;
}

#endif

/*******************************************************************************************************************//**
 * Calls user callback.
 *
 * @param[in]     p_ctrl     Pointer to GPT instance control block
 * @param[in]     event      Event code
 * @param[in]     capture    Event capture counts (if applicable)
 **********************************************************************************************************************/
static void r_gpt_call_callback (gpt_instance_ctrl_t * p_ctrl, timer_event_t event, uint32_t capture)
{
    timer_callback_args_t args;

    /* Store callback arguments in memory provided by user if available.  This allows callback arguments to be
     * stored in non-secure memory so they can be accessed by a non-secure callback function. */
    timer_callback_args_t * p_args = p_ctrl->p_callback_memory;
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
    p_args->capture   = capture;
    p_args->p_context = p_ctrl->p_context;

    /* If the project is not Trustzone Secure, then it will never need to change security state in order to call the callback. */
    p_ctrl->p_callback(p_args);

    if (NULL != p_ctrl->p_callback_memory)
    {
        /* Restore callback memory in case this is a nested interrupt. */
        *p_ctrl->p_callback_memory = args;
    }
}

/*******************************************************************************************************************//**
 * Common processing for input capture interrupt.
 *
 * @param[in]  event  Which input capture event occurred
 **********************************************************************************************************************/
static void r_gpt_capture_common_isr (gpt_prv_capture_event_t event)
{
    GPT_CFG_MULTIPLEX_INTERRUPT_ENABLE;

#if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;
#endif

    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Recover ISR context saved in open. */
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) R_FSP_IsrContextGet(irq);
    uint32_t              counter;

    /* Get captured value. */
    if (event == GPT_PRV_CAPTURE_EVENT_A)
    {
        counter = p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRA];
    }
    else
    {
        counter = p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRB];
    }

    /* If we captured a one-shot pulse, then disable future captures. */
    if (TIMER_MODE_ONE_SHOT == p_instance_ctrl->p_cfg->mode)
    {
        /* Disable captures. */
        gpt_hardware_events_disable(p_instance_ctrl);

        /* Clear pending interrupt to make sure it doesn't fire again if another overflow has already occurred. */
        R_BSP_IrqClearPending(irq);
    }

    /* If a callback is provided, then call it with the captured counter value. */
    if (NULL != p_instance_ctrl->p_callback)
    {
        r_gpt_call_callback(p_instance_ctrl,
                            (timer_event_t) ((uint32_t) TIMER_EVENT_CAPTURE_A + (uint32_t) event),
                            counter);
    }

#if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;
#endif

    GPT_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Stops the timer if one-shot mode, clears interrupts, and calls callback if one was provided in the open function.
 **********************************************************************************************************************/
void gpt_counter_overflow_isr (void)
{
    GPT_CFG_MULTIPLEX_INTERRUPT_ENABLE;

#if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;
#endif
    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Recover ISR context saved in open. */
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* If one-shot mode is selected, stop the timer since period has expired. */
    if (TIMER_MODE_ONE_SHOT == p_instance_ctrl->p_cfg->mode)
    {
        r_gpt_write_protect_disable(p_instance_ctrl);

        p_instance_ctrl->p_reg->GTSTP = p_instance_ctrl->channel_mask;

        /* Clear the GPT counter and the overflow flag after the one shot pulse has being generated */
        p_instance_ctrl->p_reg->GTCNT                 = 0;
        p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRA] = 0;
        p_instance_ctrl->p_reg->GTCCR[GPT_PRV_GTCCRB] = 0;

        r_gpt_write_protect_enable(p_instance_ctrl);

        /* Clear pending interrupt to make sure it doesn't fire again if another overflow has already occurred. */
        R_BSP_IrqClearPending(irq);
    }

    if (NULL != p_instance_ctrl->p_callback)
    {
        r_gpt_call_callback(p_instance_ctrl, TIMER_EVENT_CYCLE_END, 0);
    }

#if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;
#endif
    GPT_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

#if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE

/*******************************************************************************************************************//**
 * Only supported for asymmetric triangle-wave PWM. Notifies application of trough event.
 **********************************************************************************************************************/
void gpt_counter_underflow_isr (void)
{
    GPT_CFG_MULTIPLEX_INTERRUPT_ENABLE;

 #if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;
 #endif

    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Recover ISR context saved in open. */
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Call user callback. */
    r_gpt_call_callback(p_instance_ctrl, TIMER_EVENT_TROUGH, 0);

 #if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;
 #endif

    GPT_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

#endif

/*******************************************************************************************************************//**
 * Calls callback if one was provided in the open function.
 **********************************************************************************************************************/
void gpt_dead_time_isr (void)
{
    GPT_CFG_MULTIPLEX_INTERRUPT_ENABLE;

#if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;
#endif

    IRQn_Type irq = R_FSP_CurrentIrqGet();

    /* Recover ISR context saved in open. */
    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Call user callback. */
    r_gpt_call_callback(p_instance_ctrl, TIMER_EVENT_DEAD_TIME, 0);

#if (0 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;
#endif

    GPT_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Interrupt triggered by a capture A source.
 *
 * Clears interrupt, disables captures if one-shot mode, and calls callback if one was provided in the open function.
 **********************************************************************************************************************/
void gpt_capture_a_isr (void)
{
    r_gpt_capture_common_isr(GPT_PRV_CAPTURE_EVENT_A);
}

/*******************************************************************************************************************//**
 * Interrupt triggered by a capture B source.
 *
 * Clears interrupt, disables captures if one-shot mode, and calls callback if one was provided in the open function.
 **********************************************************************************************************************/
void gpt_capture_b_isr (void)
{
    r_gpt_capture_common_isr(GPT_PRV_CAPTURE_EVENT_B);
}

#if 1U == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED

/*******************************************************************************************************************//**
 * Interrupt triggered by a INT0-3.
 *
 * Executes each interrupt process according to the cause of the interrupt.
 **********************************************************************************************************************/
void gpt_int_select_isr (void)
{
    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;

    IRQn_Type irq = R_FSP_CurrentIrqGet();

    gpt_instance_ctrl_t * p_instance_ctrl = (gpt_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Save pointer to extended configuration structure. */
    gpt_extended_cfg_t * p_extend = (gpt_extended_cfg_t *) p_instance_ctrl->p_cfg->p_extend;
 #if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    gpt_extended_pwm_cfg_t const * p_pwm_cfg = p_extend->p_pwm_cfg;
 #endif

    /* Clearing event state and executing ISR function. */
    if (irq == p_extend->capture_a_irq)
    {
        gpt_capture_a_isr();
    }
    else if (irq == p_extend->capture_b_irq)
    {
        gpt_capture_b_isr();
    }
    else if (irq == p_instance_ctrl->p_cfg->cycle_end_irq)
    {
        gpt_counter_overflow_isr();
    }
    else if (irq == p_extend->dead_time_irq)
    {
        gpt_dead_time_isr();
    }

 #if GPT_PRV_EXTRA_FEATURES_ENABLED == GPT_CFG_OUTPUT_SUPPORT_ENABLE
    else if (irq == p_pwm_cfg->trough_irq)
    {
        gpt_counter_underflow_isr();
    }
 #endif
    else
    {
        /* Do Nothing */
        ;
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;
}

#endif
