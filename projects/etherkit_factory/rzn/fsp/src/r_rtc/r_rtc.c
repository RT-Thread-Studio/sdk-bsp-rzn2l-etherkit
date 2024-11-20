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
#include "r_rtc.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define RTC_MASK_MSB                                 (0x0F)
#define RTC_MASK_LSB                                 (0xF0)

#define RTC_FIRST_DAY_OF_A_MONTH                     (1)

/* Month : valid range between 0 to 11.*/
#define RTC_MONTHS_IN_A_YEAR                         (11)
#define RTC_LAST_DAY_OF_LEAP_FEB_MONTH               (29)
#define RTC_YEAR_VALUE_MIN                           (100)
#define RTC_YEAR_VALUE_MAX                           (199)

/* Seconds : valid range between 0 to 59.*/
#define RTC_SECONDS_IN_A_MINUTE                      (59)

/* Minute : valid range between 0 to 59. */
#define RTC_MINUTES_IN_A_HOUR                        (59)

/* Hours : valid range between 0 to 23. */
#define RTC_HOURS_IN_A_DAY                           (23)

/* In Zeller algorithm value of (-[Y/100] + [Y/400]) is 15 for Y = 2000 to Y = 2099) */
#define RTC_ZELLER_ALGM_CONST_FIFTEEN                (15)

/* Macro definitions for February and March months */
#define RTC_FEBRUARY_MONTH                           (2U)
#define RTC_MARCH_MONTH                              (3U)

#define RTC_TIME_H_MONTH_OFFSET                      (1)

/*The RTC has a 100 year calendar to match the starting year 2000, year offset(1900) is added like 117 + 1900 = 2017 */
#define RTC_TIME_H_YEAR_OFFSET                       (1900)

/** "RTC" in ASCII, used to determine if device is open. */
#define RTC_OPEN                                     (0x00525443ULL)

/* As per HW manual, value of Year is between 0 to 99, the RTC has a 100 year calendar from 2000 to 2099.
 * But as per C standards, tm_year is years since 1900.*/
#define RTC_C_TIME_OFFSET                            (100)

#define RTC_RTCA0ALM_DISABLE_VALUE                   (0x7FU)

#define RTC_RTCA0ALH_DISABLE_VALUE                   (0x3FU)

#define RTC_RTCA0CTRL1_RTCA0CT_ADJUSTMENT_VALUE_0    (11)
#define RTC_RTCA0CTRL1_RTCA0CT_ADJUSTMENT_VALUE_1    (12)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static uint8_t rtc_dec_to_bcd(uint8_t to_convert);
static uint8_t rtc_bcd_to_dec(uint8_t to_convert);
void           rtc_alarm_periodic_isr(void);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/** RTC Implementation of Real Time Clock  */
const rtc_api_t g_rtc_on_rtc =
{
    .open               = R_RTC_Open,
    .close              = R_RTC_Close,
    .clockSourceSet     = R_RTC_ClockSourceSet,
    .calendarTimeGet    = R_RTC_CalendarTimeGet,
    .calendarTimeSet    = R_RTC_CalendarTimeSet,
    .calendarAlarmGet   = R_RTC_CalendarAlarmGet,
    .calendarAlarmSet   = R_RTC_CalendarAlarmSet,
    .periodicIrqRateSet = R_RTC_PeriodicIrqRateSet,
    .infoGet            = R_RTC_InfoGet,
    .errorAdjustmentSet = R_RTC_ErrorAdjustmentSet,
    .callbackSet        = R_RTC_CallbackSet,
    .timeCaptureSet     = R_RTC_TimeCaptureSet,
    .timeCaptureGet     = R_RTC_TimeCaptureGet,
};

#if RTC_CFG_PARAM_CHECKING_ENABLE

/* Number of days in each months start from January to December */
static const uint8_t days_in_months[12] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
#endif

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

static void r_rtc_set_clock_source(rtc_cfg_t const * const p_cfg);

static void r_rtc_config_rtc_interrupts(rtc_instance_ctrl_t * const p_instance_ctrl, rtc_cfg_t const * const p_cfg);

static void r_rtc_call_callback(rtc_instance_ctrl_t * p_instance_ctrl, rtc_event_t event);

#if RTC_CFG_PARAM_CHECKING_ENABLE
static fsp_err_t r_rtc_time_and_date_validate(rtc_time_t * const p_time);

static fsp_err_t r_rtc_time_validate(rtc_time_t * const p_time);

static fsp_err_t r_rtc_date_validate(rtc_time_t * const p_time);

static fsp_err_t r_rtc_alarm_time_and_date_validate(rtc_alarm_time_t * const p_time);

static fsp_err_t r_rtc_alarm_time_validate(rtc_alarm_time_t * const p_time);

static fsp_err_t r_rtc_periodic_irq_rate_validate(rtc_periodic_irq_select_t const rate);

#endif

/*******************************************************************************************************************//**
 * @addtogroup RTC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Opens and configures the RTC driver module. Implements @ref rtc_api_t::open.
 * Configuration includes clock source, and interrupt callback function.
 *
 * @retval FSP_SUCCESS          Initialization was successful and RTC has started.
 * @retval FSP_ERR_ASSERTION    Invalid p_ctrl or p_cfg pointer.
 * @retval FSP_ERR_ALREADY_OPEN Module is already open.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid parameter.
 **********************************************************************************************************************/
fsp_err_t R_RTC_Open (rtc_ctrl_t * const p_ctrl, rtc_cfg_t const * const p_cfg)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;
    fsp_err_t             err             = FSP_SUCCESS;

    /* Parameter checking */
#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ERROR_RETURN(RTC_OPEN != p_instance_ctrl->open, FSP_ERR_ALREADY_OPEN);
#endif

    /* Save the configuration  */
    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;

#if RTC_CFG_PARAM_CHECKING_ENABLE

    /* Verify the frequency comparison value for RTCA0SCMP */
    FSP_ERROR_RETURN(R_RTC_RTCA0SCMP_RTCA0SCMP_Msk >= p_cfg->freq_compare_value, FSP_ERR_INVALID_ARGUMENT);
#endif

    r_rtc_config_rtc_interrupts(p_instance_ctrl, p_cfg);

    /* Set the clock source for RTC. */
    r_rtc_set_clock_source(p_cfg);

    /** Mark driver as open by initializing it to "RTC" in its ASCII equivalent. */
    p_instance_ctrl->open = RTC_OPEN;

    return err;
}

/*******************************************************************************************************************//**
 * Close the RTC driver.
 * Implements @ref rtc_api_t::close
 *
 * @retval FSP_SUCCESS          De-Initialization was successful and RTC driver closed.
 * @retval FSP_ERR_ASSERTION    Invalid p_ctrl.
 * @retval FSP_ERR_NOT_OPEN     Driver not open already for close.
 **********************************************************************************************************************/
fsp_err_t R_RTC_Close (rtc_ctrl_t * const p_ctrl)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;

    /* Parameter checking */
#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    if (p_instance_ctrl->p_cfg->periodic_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->periodic_irq);
        R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->periodic_irq, NULL);
    }

    if (p_instance_ctrl->p_cfg->alarm_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->alarm_irq);
        R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->alarm_irq, NULL);
    }

    /* Clear the interrupt enable bit and pulse output bit */
    R_RTC->RTCA0CTL1 = 0U;

    /* Clock counter operation stopped */
    R_RTC->RTCA0CTL0_b.RTCA0CE = 0U;

    /* Check that the clock counter has stopped */
    FSP_HARDWARE_REGISTER_WAIT(R_RTC->RTCA0CTL0_b.RTCA0CEST, 0U);

    /* Disable clock to the RTC block */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_STOP(FSP_IP_RTC, 0);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    p_instance_ctrl->open = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Set the calendar time.
 *
 * Implements @ref rtc_api_t::calendarTimeSet.
 *
 * @retval FSP_SUCCESS              Calendar time set operation was successful.
 * @retval FSP_ERR_ASSERTION        Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN         Driver not open already for operation.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid time parameter field.
 **********************************************************************************************************************/
fsp_err_t R_RTC_CalendarTimeSet (rtc_ctrl_t * const p_ctrl, rtc_time_t * const p_time)
{
    fsp_err_t             err             = FSP_SUCCESS;
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;

#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(p_time);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    /* Verify the seconds, minutes, hours, year ,day of the week, day of the month, month and year are valid values */
    FSP_ERROR_RETURN(FSP_SUCCESS == r_rtc_time_and_date_validate(p_time), FSP_ERR_INVALID_ARGUMENT);
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
#endif

    /* See section "Writing to Counters While Clock Counter Operation is Enabled" and "Initial Setting" of
     *  the RZ microprocessor manual */

    if (1U == R_RTC->RTCA0CTL0_b.RTCA0CE)
    {
        /* Check that previous write operations to RTCA0SEC to RTCA0YEAR have been completed */
        FSP_HARDWARE_REGISTER_WAIT(R_RTC->RTCA0CTL2_b.RTCA0WST, 0U);

        /* Stop RTCA0SEC to RTCA0YEAR counter operation */
        R_RTC->RTCA0CTL2_b.RTCA0WAIT = 1U;

        /* Check that the setting of RTCA0WAIT is enabled */
        FSP_HARDWARE_REGISTER_WAIT(R_RTC->RTCA0CTL2_b.RTCA0WST, 1U);
    }

    /* Set the year, month, day of the week, ... */
    R_RTC->RTCA0SEC  = rtc_dec_to_bcd((uint8_t) p_time->tm_sec);
    R_RTC->RTCA0MIN  = rtc_dec_to_bcd((uint8_t) p_time->tm_min);
    R_RTC->RTCA0HOUR = rtc_dec_to_bcd((uint8_t) p_time->tm_hour);
    R_RTC->RTCA0WEEK = rtc_dec_to_bcd((uint8_t) p_time->tm_wday);
    R_RTC->RTCA0DAY  = rtc_dec_to_bcd((uint8_t) p_time->tm_mday);

    /* Add one to match with HW register */
    R_RTC->RTCA0MONTH = rtc_dec_to_bcd((uint8_t) (p_time->tm_mon + 1));

    /* Subtract 100 to match with HW register */
    R_RTC->RTCA0YEAR = rtc_dec_to_bcd((uint8_t) (p_time->tm_year - RTC_C_TIME_OFFSET));

    if (1U == R_RTC->RTCA0CTL0_b.RTCA0CE)
    {
        /* RTCA0SEC to RTCA0YEAR counter operation enable */
        R_RTC->RTCA0CTL2_b.RTCA0WAIT = 0U;
    }
    else
    {
        /* Clock counter operation enable */
        R_RTC->RTCA0CTL0_b.RTCA0CE = 1U;
    }

    return err;
}

/*******************************************************************************************************************//**
 * Get the calendar time.
 *
 * Implements @ref rtc_api_t::calendarTimeGet
 *
 * @retval FSP_SUCCESS          Calendar time get operation was successful.
 * @retval FSP_ERR_ASSERTION    Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN     Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_CalendarTimeGet (rtc_ctrl_t * const p_ctrl, rtc_time_t * const p_time)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;
#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(p_time);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
#endif

    fsp_err_t err = FSP_SUCCESS;

    /* See section "Reading Clock Counters While Clock Counter Operation is Enabled" of the RZ microprocessor manual */

    /* Check that previous write operations to RTCA0SEC to RTCA0YEAR have been completed */
    FSP_HARDWARE_REGISTER_WAIT(R_RTC->RTCA0CTL2_b.RTCA0WST, 0U);

    /* Stop RTCA0SEC to RTCA0YEAR counter operation */
    R_RTC->RTCA0CTL2_b.RTCA0WAIT = 1U;

    /* Check that the setting of RTCA0WAIT is enabled */
    FSP_HARDWARE_REGISTER_WAIT(R_RTC->RTCA0CTL2_b.RTCA0WST, 1U);

    p_time->tm_sec  = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0SEC);
    p_time->tm_min  = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0MIN);
    p_time->tm_hour = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0HOUR);
    p_time->tm_wday = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0WEEK);
    p_time->tm_mday = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0DAY);

    /* Subtract one to match with C time.h standards */
    p_time->tm_mon = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0MONTH) - 1;

    /* Add 100 to match with C time.h standards */
    p_time->tm_year = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0YEAR) + RTC_C_TIME_OFFSET;

    /* RTCA0SEC to RTCA0YEAR counter operation enable */
    R_RTC->RTCA0CTL2_b.RTCA0WAIT = 0U;

    return err;
}

/*******************************************************************************************************************//**
 * Set the calendar alarm time.
 *
 * Implements @ref rtc_api_t::calendarAlarmSet.
 *
 * @pre The calendar counter must be running before the alarm can be set.
 *
 * @retval FSP_SUCCESS              Calendar alarm time set operation was successful.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid time parameter field.
 * @retval FSP_ERR_ASSERTION        Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN         Driver not open already for operation.
 * @retval FSP_ERR_IRQ_BSP_DISABLED User IRQ parameter not valid
 **********************************************************************************************************************/
fsp_err_t R_RTC_CalendarAlarmSet (rtc_ctrl_t * const p_ctrl, rtc_alarm_time_t * const p_alarm)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;
    fsp_err_t             err             = FSP_SUCCESS;

#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(p_alarm);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(p_instance_ctrl->p_cfg->alarm_irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);

    /* Verify the seconds, minutes, hours, year ,day of the week, day of the month and month are valid values */
    FSP_ERROR_RETURN(FSP_SUCCESS == r_rtc_alarm_time_and_date_validate(p_alarm), FSP_ERR_INVALID_ARGUMENT);
#endif

    if (p_instance_ctrl->p_cfg->alarm_irq >= 0)
    {
        /* Disable the ICU alarm interrupt request */
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->alarm_irq);
    }

    /* Set alarm time */
    volatile uint8_t field;

    if (p_alarm->min_match)
    {
        field = rtc_dec_to_bcd((uint8_t) p_alarm->time.tm_min);
    }
    else
    {
        field = RTC_RTCA0ALM_DISABLE_VALUE;
    }

    R_RTC->RTCA0ALM = field;

    if (p_alarm->hour_match)
    {
        field = rtc_dec_to_bcd((uint8_t) p_alarm->time.tm_hour);
    }
    else
    {
        field = RTC_RTCA0ALH_DISABLE_VALUE;
    }

    R_RTC->RTCA0ALH = field;

    R_RTC->RTCA0ALW = (uint8_t) ((p_alarm->sunday_match << R_RTC_RTCA0ALW_RTCA0ALW0_Pos) |
                                 (p_alarm->monday_match << R_RTC_RTCA0ALW_RTCA0ALW1_Pos) |
                                 (p_alarm->tuesday_match << R_RTC_RTCA0ALW_RTCA0ALW2_Pos) |
                                 (p_alarm->wednesday_match << R_RTC_RTCA0ALW_RTCA0ALW3_Pos) |
                                 (p_alarm->thursday_match << R_RTC_RTCA0ALW_RTCA0ALW4_Pos) |
                                 (p_alarm->friday_match << R_RTC_RTCA0ALW_RTCA0ALW5_Pos) |
                                 (p_alarm->saturday_match << R_RTC_RTCA0ALW_RTCA0ALW6_Pos));

    /* Enable the alarm interrupt */
    R_RTC->RTCA0CTL1_b.RTCA0ALME = 1U;

    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->alarm_irq);

    return err;
}

/*******************************************************************************************************************//**
 * Get the calendar alarm time.
 *
 * Implements @ref rtc_api_t::calendarAlarmGet
 *
 * @retval FSP_SUCCESS           Calendar alarm time get operation was successful.
 * @retval FSP_ERR_ASSERTION     Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN      Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_CalendarAlarmGet (rtc_ctrl_t * const p_ctrl, rtc_alarm_time_t * const p_alarm)
{
#if RTC_CFG_PARAM_CHECKING_ENABLE
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_alarm);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
#endif

    /* Get alarm time */
    p_alarm->time.tm_min  = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0ALM);
    p_alarm->time.tm_hour = rtc_bcd_to_dec((uint8_t) R_RTC->RTCA0ALH);

    p_alarm->sunday_match    = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW0;
    p_alarm->monday_match    = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW1;
    p_alarm->tuesday_match   = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW2;
    p_alarm->wednesday_match = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW3;
    p_alarm->thursday_match  = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW4;
    p_alarm->friday_match    = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW5;
    p_alarm->saturday_match  = (bool) R_RTC->RTCA0ALW_b.RTCA0ALW6;

    p_alarm->min_match  = (RTC_RTCA0ALM_DISABLE_VALUE != R_RTC->RTCA0ALM);
    p_alarm->hour_match = (RTC_RTCA0ALH_DISABLE_VALUE != R_RTC->RTCA0ALH);

    /* Set unsupported items to false */
    p_alarm->sec_match       = false;
    p_alarm->mday_match      = false;
    p_alarm->mon_match       = false;
    p_alarm->year_match      = false;
    p_alarm->dayofweek_match = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Set the periodic interrupt rate and enable periodic interrupt.
 *
 * Implements @ref rtc_api_t::periodicIrqRateSet
 *
 * @note To start the RTC @ref R_RTC_CalendarTimeSet must be called at least once.
 *
 * @retval FSP_SUCCESS               The periodic interrupt rate was successfully set.
 * @retval FSP_ERR_ASSERTION         Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN          Driver not open already for operation.
 * @retval FSP_ERR_IRQ_BSP_DISABLED  User IRQ parameter not valid
 * @retval FSP_ERR_INVALID_ARGUMENT  Invalid periodic IRQ rate value
 **********************************************************************************************************************/
fsp_err_t R_RTC_PeriodicIrqRateSet (rtc_ctrl_t * const p_ctrl, rtc_periodic_irq_select_t const rate)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;
#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    FSP_ERROR_RETURN(p_instance_ctrl->p_cfg->periodic_irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);

    /* Verify the rate is valid value */
    FSP_ERROR_RETURN(FSP_SUCCESS == r_rtc_periodic_irq_rate_validate(rate), FSP_ERR_INVALID_ARGUMENT);
#endif
    fsp_err_t err = FSP_SUCCESS;

    if (p_instance_ctrl->p_cfg->periodic_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->periodic_irq);
    }

    /* Adjust the rate value to match the register */
    if (RTC_PERIODIC_IRQ_SELECT_2_SECOND > rate)
    {
        R_RTC->RTCA0CTL1_b.RTCA0CT = (uint8_t) (rate - RTC_RTCA0CTRL1_RTCA0CT_ADJUSTMENT_VALUE_0) &
                                     R_RTC_RTCA0CTL1_RTCA0CT_Msk;
    }
    else
    {
        R_RTC->RTCA0CTL1_b.RTCA0CT = (uint8_t) (rate - RTC_RTCA0CTRL1_RTCA0CT_ADJUSTMENT_VALUE_1) &
                                     R_RTC_RTCA0CTL1_RTCA0CT_Msk;
    }

    R_BSP_IrqEnable(p_instance_ctrl->p_cfg->periodic_irq);

    return err;
}

/*******************************************************************************************************************//**
 * Get RTC clock source and running status information and store it in provided pointer p_rtc_info.
 * Before or immediately after the first call to @ref R_RTC_CalendarTimeSet, RTC_STATUS_STOPPED will be returned.
 * Two PCLKRTC after the first call to @ref R_RTC_CalendarTimeSet, RTC_STATUS_RUNNING will be returned.
 *
 * Implements @ref rtc_api_t::infoGet
 *
 * @retval FSP_SUCCESS          Get information Successful.
 * @retval FSP_ERR_ASSERTION    Invalid input argument.
 * @retval FSP_ERR_NOT_OPEN     Driver not open already for operation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_InfoGet (rtc_ctrl_t * const p_ctrl, rtc_info_t * const p_rtc_info)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;
#if RTC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_rtc_info);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
#endif

    p_rtc_info->clock_source = RTC_CLOCK_SOURCE_MAINCLK;
    p_rtc_info->status       = (rtc_status_t) R_RTC->RTCA0CTL0_b.RTCA0CEST;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @ref rtc_api_t::clockSourceSet is not supported on the RTC.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_ClockSourceSet (rtc_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @ref rtc_api_t::errorAdjustmentSet is not supported on the RTC.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_ErrorAdjustmentSet (rtc_ctrl_t * const p_ctrl, rtc_error_adjustment_cfg_t const * const err_adj_cfg)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(err_adj_cfg);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements rtc_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened
 **********************************************************************************************************************/
fsp_err_t R_RTC_CallbackSet (rtc_ctrl_t * const          p_ctrl,
                             void (                    * p_callback)(rtc_callback_args_t *),
                             void const * const          p_context,
                             rtc_callback_args_t * const p_callback_memory)
{
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) p_ctrl;

#if (RTC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(RTC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @ref rtc_api_t::timeCaptureSet is not supported on the RTC.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_TimeCaptureSet (rtc_ctrl_t * const p_ctrl, rtc_time_capture_t * const p_time_capture)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_time_capture);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @ref rtc_api_t::timeCaptureGet is not supported on the RTC.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_RTC_TimeCaptureGet (rtc_ctrl_t * const p_ctrl, rtc_time_capture_t * const p_time_capture)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_time_capture);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup RTC)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Set the RTC clock source
 *
 * @param[in]  p_cfg                   Pointer to RTC configuration.
 **********************************************************************************************************************/
static void r_rtc_set_clock_source (rtc_cfg_t const * const p_cfg)
{
    /* Enable clock to the RTC block */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_RTC, 0);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Force RTC to 24 hour mode. Set RTCA0AMPM bit in the RTCA0CTL0 register */
    R_RTC->RTCA0CTL0_b.RTCA0AMPM = 1U;

    /* The setting of the RTCA0SCMP resister is valid when RTCA0SLSB = 1 */
    R_RTC->RTCA0CTL0_b.RTCA0SLSB = 1U;
    R_RTC->RTCA0SCMP_b.RTCA0SCMP = (p_cfg->freq_compare_value) & R_RTC_RTCA0SCMP_RTCA0SCMP_Msk;
}

/*******************************************************************************************************************//**
 * Set IRQ priority and control info for IRQ handler.
 *
 * @param[in]  p_instance_ctrl         Instance control block
 * @param[in]  p_cfg                   Pointer to RTC configuration.
 **********************************************************************************************************************/
static void r_rtc_config_rtc_interrupts (rtc_instance_ctrl_t * const p_instance_ctrl, rtc_cfg_t const * const p_cfg)
{
    if (p_cfg->periodic_irq >= 0)
    {
        R_BSP_IrqCfg(p_cfg->periodic_irq, p_cfg->periodic_ipl, p_instance_ctrl);
    }

    if (p_cfg->alarm_irq >= 0)
    {
        R_BSP_IrqCfg(p_cfg->alarm_irq, p_cfg->alarm_ipl, p_instance_ctrl);
    }
}

/*******************************************************************************************************************//**
 * Calls user callback.
 *
 * @param[in]     p_instance_ctrl     Pointer to RTC instance control block
 * @param[in]     event               Event code
 **********************************************************************************************************************/
static void r_rtc_call_callback (rtc_instance_ctrl_t * p_instance_ctrl, rtc_event_t event)
{
    rtc_callback_args_t args;

    /* Store callback arguments in memory provided by user if available. */
    rtc_callback_args_t * p_args = p_instance_ctrl->p_callback_memory;
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

#if RTC_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * Validate time and date fields of time parameter fields
 * Checking for seconds, minutes, hours are valid values by calling sub-function time validate.
 * Checking for year, month, day of the week and day of a month are valid values by calling sub-function
 * date validate.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_time_and_date_validate (rtc_time_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;
    err = r_rtc_time_validate(p_time);
    FSP_ERROR_RETURN(err == FSP_SUCCESS, err);
    err = r_rtc_date_validate(p_time);
    FSP_ERROR_RETURN(err == FSP_SUCCESS, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Validate time fields of time type parameter
 * Checking for the seconds, minutes, hours values for valid specified range.
 * Seconds 0 to 59.
 * Minutes 0 to 59.
 * Hours   0 to 23.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_time_validate (rtc_time_t * p_time)
{
    fsp_err_t err;
    err = FSP_SUCCESS;
    if ((p_time->tm_sec < 0) || (p_time->tm_sec > RTC_SECONDS_IN_A_MINUTE) ||
        (p_time->tm_min < 0) || (p_time->tm_min > RTC_MINUTES_IN_A_HOUR) ||
        (p_time->tm_hour < 0) || (p_time->tm_hour > RTC_HOURS_IN_A_DAY))
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * Validate date fields of time type parameter
 * validating r_rtc date fields and setting day of a Week using Zeller's congruence.
 * Checking for year, month, day of the week and day of a month are valid values.
 * Leap year validation and Week of the day is calculated and updated in RTC time.
 * Day of week between 0 to 6
 * Day between 1 to 31
 * Month between 0 to 11 as per standard time.h, There's a mismatch between hardware configuration,
 * UM indicates that "a decimal value of 01 to 12 must be set in BCD code" for Month Count Buffer register in the RTC.
 * This difference will be taken care in the Set and Get functions.
 *
 * As per HW manual, value of Year is between 0 to 99, the RTC has a 100 year calendar from 2000 to 2099.
 * (see section "Overview" of the RZ microprocessor manual)
 * But as per C standards, tm_year is years since 1900.
 * A sample year set in an application would be like time.tm_year = 2019-1900; (to set year 2019)
 * Since RTC API follows the Date and time structure defined in C standard library <time.h>, the valid value of year is
 * between 100 and 199, which will be internally converted to HW required value.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_date_validate (rtc_time_t * const p_time)
{
    uint32_t day_of_week;
    uint32_t num_days_month;
    uint32_t day_of_a_month;
    uint32_t temp_month;
    uint32_t temp_year;

    day_of_a_month = (uint32_t) p_time->tm_mday;
    temp_month     = (uint32_t) (p_time->tm_mon + RTC_TIME_H_MONTH_OFFSET);

    /* The valid value of year is between 100 to 199, The RTC has a 100 year calendar from 2000 to 2099
     * to match the starting year 2000, a sample year offset(1900) is added like 117 + 1900 = 2017*/
    temp_year = (uint32_t) (p_time->tm_year + RTC_TIME_H_YEAR_OFFSET);

    /* Checking the error condition for year and months values, here valid value of year is between 100 to 199
     * and for month 0 to 11*/
    if ((p_time->tm_year < RTC_YEAR_VALUE_MIN) || (p_time->tm_year > RTC_YEAR_VALUE_MAX) ||
        (p_time->tm_mon < 0) || (p_time->tm_mon > RTC_MONTHS_IN_A_YEAR))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /*For particular valid month, number of days in a month is updated */
    num_days_month = days_in_months[p_time->tm_mon];

    /* Checking for February month and Conditions for Leap year : Every fourth year is a leap year,
     * The RTC has a 100 year calendar from 2000 to 2099  */
    if ((RTC_FEBRUARY_MONTH == temp_month) && ((temp_year % 4U) == 0))
    {
        num_days_month = RTC_LAST_DAY_OF_LEAP_FEB_MONTH;
    }

    /* Checking for day of a month values for valid range */
    if ((p_time->tm_mday >= RTC_FIRST_DAY_OF_A_MONTH) && (day_of_a_month <= num_days_month))
    {
        /* Adjust month to run from 3 to 14 for March to February */
        if (temp_month < RTC_MARCH_MONTH)
        {
            temp_month = (temp_month + 12U);

            /* Adjust year if January or February*/
            --temp_year;
        }

        /*For the Gregorian calendar, Zeller's congruence formulas is
         * h = ( q + [13(m+1)/5] + Y + [Y/4] - [Y/100] + [Y/400])mod 7 (mod : modulo)
         * h is the day of the week , q is the day of the month,
         * m is the month (3 = March, 4 = April,..., 14 = February)
         * Y is year, which is Y - 1 during January and February */
        day_of_week = (uint32_t) p_time->tm_mday + ((13 * (temp_month + 1)) / 5) + temp_year + (temp_year / 4);
        day_of_week = (day_of_week - RTC_ZELLER_ALGM_CONST_FIFTEEN) % 7;

        /* Day of week between 0 to 6 :- Sunday to Saturday */
        /* d = (h + 6)mod 7 (mod : modulo) */
        p_time->tm_wday = (int16_t) ((day_of_week + 6U) % 7U);

        return FSP_SUCCESS;
    }

    return FSP_ERR_INVALID_ARGUMENT;
}

/*******************************************************************************************************************//**
 * Validate alarm time and date of Alarm time type parameter
 * Checking that alarm enable bit are valid values.
 * Checking that alarm time with minutes and hours are valid values.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_alarm_time_and_date_validate (rtc_alarm_time_t * const p_time)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Checking for alarm enable bit for day of week, year, month, day of the month, second */
    if ((p_time->dayofweek_match) || (p_time->year_match) || (p_time->mon_match) || (p_time->mday_match) ||
        (p_time->sec_match))
    {
        err = FSP_ERR_INVALID_ARGUMENT;
        FSP_ERROR_RETURN(err == FSP_SUCCESS, err);
    }
    else
    {
        err = r_rtc_alarm_time_validate(p_time);
        FSP_ERROR_RETURN(err == FSP_SUCCESS, err);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Validate alarm time fields of Alarm time type parameter
 * Checking for alarm enable bit with the minutes, hours value for valid specified range.
 * Minutes 0 to 59.
 * Hours   0 to 23.
 *
 * @param[in]  p_time                     Pointer to rtc_time_t
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid field in rtc_time_t structure
 **********************************************************************************************************************/
static fsp_err_t r_rtc_alarm_time_validate (rtc_alarm_time_t * const p_time)
{
    fsp_err_t err;
    err = FSP_SUCCESS;
    if (((p_time->min_match) &&
         ((p_time->time.tm_min < 0) || (p_time->time.tm_min > RTC_MINUTES_IN_A_HOUR))) ||
        ((p_time->hour_match) &&
         ((p_time->time.tm_hour < 0) || (p_time->time.tm_hour > RTC_HOURS_IN_A_DAY))))
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * Validate periodic IRQ rate
 *
 * @param[in] rate                        Periodic IRQ rate
 * @retval FSP_SUCCESS                    Validation successful
 * @retval FSP_ERR_INVALID_ARGUMENT       Invalid periodic IRQ rate value
 **********************************************************************************************************************/
static fsp_err_t r_rtc_periodic_irq_rate_validate (rtc_periodic_irq_select_t const rate)
{
    fsp_err_t err = FSP_SUCCESS;

    if ((RTC_PERIODIC_IRQ_SELECT_1_DIV_BY_4_SECOND == rate) ||
        (RTC_PERIODIC_IRQ_SELECT_1_DIV_BY_2_SECOND == rate) ||
        (RTC_PERIODIC_IRQ_SELECT_1_SECOND == rate) ||
        (RTC_PERIODIC_IRQ_SELECT_1_MINUTE == rate) ||
        (RTC_PERIODIC_IRQ_SELECT_1_HOUR == rate) ||
        (RTC_PERIODIC_IRQ_SELECT_1_DAY == rate) ||
        (RTC_PERIODIC_IRQ_SELECT_1_MONTH == rate))
    {
        /* Do nothing */
    }
    else
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

#endif

/*******************************************************************************************************************//**
 * RTC Callback ISR for alarm and periodic interrupt.
 *
 * Saves context if RTOS is used, calls callback if one was provided in the open function, and restores context
 * if RTOS is used.
 **********************************************************************************************************************/
void rtc_alarm_periodic_isr (void)
{
    RTC_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;

    IRQn_Type             irq             = R_FSP_CurrentIrqGet();
    rtc_instance_ctrl_t * p_instance_ctrl = (rtc_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* Call the callback routine if one is available */
    if (NULL != p_instance_ctrl->p_callback)
    {
        /* Set data to identify callback to user, then call user callback. */
        rtc_event_t event;
        if (irq == p_instance_ctrl->p_cfg->alarm_irq)
        {
            event = RTC_EVENT_ALARM_IRQ;
        }
        else
        {
            event = RTC_EVENT_PERIODIC_IRQ;
        }

        /* Call callback */
        r_rtc_call_callback(p_instance_ctrl, event);
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;

    RTC_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * Convert decimal to BCD
 *
 * @param[in] to_convert   Decimal Value to be converted
 **********************************************************************************************************************/
static uint8_t rtc_dec_to_bcd (uint8_t to_convert)
{
    return (uint8_t) ((((to_convert / (uint8_t) 10) << 4) & (uint8_t) RTC_MASK_LSB) | (to_convert % (uint8_t) 10));
}

/*******************************************************************************************************************//**
 * Convert BCD to decimal
 *
 * @param[in] to_convert   BCD Value to be converted
 **********************************************************************************************************************/
static uint8_t rtc_bcd_to_dec (uint8_t to_convert)
{
    return (uint8_t) ((((to_convert & (uint8_t) RTC_MASK_LSB) >> 4) * (uint8_t) 10) +
                      (to_convert & (uint8_t) RTC_MASK_MSB));
}
