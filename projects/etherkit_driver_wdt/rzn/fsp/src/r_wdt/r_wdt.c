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
#include "r_wdt.h"
#include "bsp_api.h"
#include "bsp_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define WDT_OPEN    (0X00574454ULL)

/* Lookup functions for WDT settings.  Using function like macro for stringification. */
#define WDT_PRV_WDTCR_SETTING_GET(setting,                                                   \
                                  wdtcr)    (((wdtcr >> WDT_PRV_WDTCR_ ## setting ## _BIT) & \
                                              WDT_PRV_WDTCR_ ## setting ## _MASK));
#define WDT_PRV_WDTCR_SETTING_SET(setting,                                                   \
                                  value)    ((value & WDT_PRV_WDTCR_ ## setting ## _MASK) << \
                                             WDT_PRV_WDTCR_ ## setting ## _BIT);

/* WDT register settings. */
#define WDT_PRV_WDTSR_COUNTER_MASK           (0x3FFFU)
#define WDT_PRV_WDTSR_FLAGS_MASK             (0xC000U)

#define WDT_PRV_WDTCR_TIMEOUT_BIT            (0)
#define WDT_PRV_WDTCR_CLOCK_DIVISION_BIT     (4)
#define WDT_PRV_WDTCR_WINDOW_END_BIT         (8)
#define WDT_PRV_WDTCR_WINDOW_START_BIT       (12)

#define WDT_PRV_WDTRCR_RESET_CONTROL_BIT     (7)
#define WDT_PRV_WDTCSTPR_STOP_CONTROL_BIT    (7)

#define WDT_PRV_WDTCR_TIMEOUT_MASK           (0x3U) // Bits 0-1
#define WDT_PRV_WDTCR_CLOCK_DIVISION_MASK    (0xFU) // Bits 4-7
#define WDT_PRV_WDTCR_WINDOW_END_MASK        (0x3U) // Bits 8-9
#define WDT_PRV_WDTCR_WINDOW_START_MASK      (0x3U) // Bits 12-13

/* Refresh register values */
#define WDT_PRV_REFRESH_STEP_1               (0U)
#define WDT_PRV_REFRESH_STEP_2               (0xFFU)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static uint32_t r_wdt_clock_divider_get(wdt_clock_division_t division);

static fsp_err_t r_wdt_parameter_checking(wdt_instance_ctrl_t * const p_instance_ctrl, wdt_cfg_t const * const p_cfg);

/***********************************************************************************************************************
 * ISR prototypes
 **********************************************************************************************************************/
void wdt_underflow_isr(uint32_t id);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static const uint8_t g_wdtcr_timeout[] =
{
    0xFFU,                             // WDTCR value for WDT_TIMEOUT_128 (not supported by WDT).
    0xFFU,                             // WDTCR value for WDT_TIMEOUT_512 (not supported by WDT).
    0x00U,                             // WDTCR value for WDT_TIMEOUT_1024.
    0xFFU,                             // WDTCR value for WDT_TIMEOUT_2048 (not supported by WDT).
    0x01U,                             // WDTCR value for WDT_TIMEOUT_4096.
    0x02U,                             // WDTCR value for WDT_TIMEOUT_8192.
    0x03U,                             // WDTCR value for WDT_TIMEOUT_16384.
};

/* Convert WDT timeout value to an integer */
static const uint32_t g_wdt_timeout[] =
{
    128U,
    512U,
    1024U,
    2048U,
    4096U,
    8192U,
    16384U
};

/* Converts WDT division enum to log base 2 of the division value, used to shift the PCLKL frequency. */
static const uint8_t g_wdt_division_lookup[] =
{
    0U,                                // log base 2(1)    = 0
    2U,                                // log base 2(4)    = 2
    4U,                                // log base 2(16)   = 4
    5U,                                // log base 2(32)   = 5
    6U,                                // log base 2(64)   = 6
    8U,                                // log base 2(256)  = 8
    9U,                                // log base 2(512)  = 9
    11U,                               // log base 2(2048) = 11
    13U,                               // log base 2(8192) = 13
};

/** Global pointer to control structure for use by the callback.  */
static volatile wdt_instance_ctrl_t * gp_wdt_ctrl = NULL;

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/

/** Watchdog implementation of WDT Driver  */
const wdt_api_t g_wdt_on_wdt =
{
    .open        = R_WDT_Open,
    .refresh     = R_WDT_Refresh,
    .statusGet   = R_WDT_StatusGet,
    .statusClear = R_WDT_StatusClear,
    .counterGet  = R_WDT_CounterGet,
    .timeoutGet  = R_WDT_TimeoutGet,
    .callbackSet = R_WDT_CallbackSet,
};

/*******************************************************************************************************************//**
 * @addtogroup WDT WDT
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Configure the WDT in register start mode. Implements
 * @ref wdt_api_t::open.
 *
 * This function should only be called once as WDT configuration registers can only be written to once so subsequent
 * calls will have no effect.
 *
 * @retval FSP_SUCCESS              WDT successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 **********************************************************************************************************************/
fsp_err_t R_WDT_Open (wdt_ctrl_t * const p_ctrl, wdt_cfg_t const * const p_cfg)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;
    fsp_err_t             err;

    /* Check validity of the parameters */
    err = r_wdt_parameter_checking(p_instance_ctrl, p_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Register callback. */
    p_instance_ctrl->p_callback = p_cfg->p_callback;
    p_instance_ctrl->p_context  = p_cfg->p_context;

    p_instance_ctrl->p_reg = R_WDT0;

    /* Error notification to ICU is permitted. */
    p_instance_ctrl->p_reg->WDTRCR_b.RSTIRQS = 0;

    uint32_t wdtcr = WDT_PRV_WDTCR_SETTING_SET(TIMEOUT, (uint16_t) g_wdtcr_timeout[p_cfg->timeout]);
    wdtcr |= WDT_PRV_WDTCR_SETTING_SET(CLOCK_DIVISION, (uint16_t) p_cfg->clock_division);
    wdtcr |= WDT_PRV_WDTCR_SETTING_SET(WINDOW_START, (uint16_t) p_cfg->window_start);
    wdtcr |= WDT_PRV_WDTCR_SETTING_SET(WINDOW_END, (uint16_t) p_cfg->window_end);

    p_instance_ctrl->p_reg->WDTCR = (uint16_t) wdtcr;

    gp_wdt_ctrl = p_ctrl;

    p_instance_ctrl->wdt_open = WDT_OPEN;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Read timeout information for the watchdog timer. Implements @ref wdt_api_t::timeoutGet.
 *
 * @retval FSP_SUCCESS              WDT timeout information retrieved successfully.
 * @retval FSP_ERR_ASSERTION        Null Pointer.
 * @retval FSP_ERR_NOT_OPEN         Instance control block is not initialized.
 **********************************************************************************************************************/
fsp_err_t R_WDT_TimeoutGet (wdt_ctrl_t * const p_ctrl, wdt_timeout_values_t * const p_timeout)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;

#if WDT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_timeout);
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(WDT_OPEN == p_instance_ctrl->wdt_open, FSP_ERR_NOT_OPEN);
#endif

    uint32_t             shift;
    uint32_t             index;
    uint32_t             timeout = 0;
    wdt_clock_division_t clock_division;

    /* Read the configuration of the watchdog */
    uint32_t wdtcr = p_instance_ctrl->p_reg->WDTCR;
    clock_division = (wdt_clock_division_t) WDT_PRV_WDTCR_SETTING_GET(CLOCK_DIVISION, wdtcr);
    timeout        = WDT_PRV_WDTCR_SETTING_GET(TIMEOUT, wdtcr);

    /* Get timeout value from WDTCR register. */
    for (index = 0U; index < (sizeof(g_wdtcr_timeout)); index++)
    {
        if (g_wdtcr_timeout[index] == timeout)
        {
            p_timeout->timeout_clocks = g_wdt_timeout[index];
        }
    }

    /* Get the frequency of the clock supplying the watchdog */
    uint32_t pckl_frequency = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKL);
    shift = r_wdt_clock_divider_get(clock_division);

    p_timeout->clock_frequency_hz = pckl_frequency >> shift;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Refresh the watchdog timer. Implements @ref wdt_api_t::refresh.
 *
 * In addition to refreshing the watchdog counter this function can be used to start the counter in register start mode.
 *
 * @retval FSP_SUCCESS              WDT successfully refreshed.
 * @retval FSP_ERR_ASSERTION        p_ctrl is NULL.
 * @retval FSP_ERR_NOT_OPEN         Instance control block is not initialized.
 *
 * @note This function only returns FSP_SUCCESS. If the refresh fails due to being performed outside of the
 *       permitted refresh period the device will trigger an ISR to run.
 **********************************************************************************************************************/
fsp_err_t R_WDT_Refresh (wdt_ctrl_t * const p_ctrl)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;

#if WDT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(WDT_OPEN == p_instance_ctrl->wdt_open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->p_reg->WDTRR = WDT_PRV_REFRESH_STEP_1;
    p_instance_ctrl->p_reg->WDTRR = WDT_PRV_REFRESH_STEP_2;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Read the WDT status flags. Implements @ref wdt_api_t::statusGet.
 *
 * Indicates both status and error conditions.
 *
 * @retval FSP_SUCCESS              WDT status successfully read.
 * @retval FSP_ERR_ASSERTION        Null pointer as a parameter.
 * @retval FSP_ERR_NOT_OPEN         Instance control block is not initialized.
 **********************************************************************************************************************/
fsp_err_t R_WDT_StatusGet (wdt_ctrl_t * const p_ctrl, wdt_status_t * const p_status)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;

#if WDT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_status);
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(WDT_OPEN == p_instance_ctrl->wdt_open, FSP_ERR_NOT_OPEN);
#endif

    /* Check for refresh or underflow errors. */
    *p_status = (wdt_status_t) (p_instance_ctrl->p_reg->WDTSR >> 14);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Clear the WDT status and error flags. Implements @ref wdt_api_t::statusClear.
 *
 * @retval FSP_SUCCESS              WDT flag(s) successfully cleared.
 * @retval FSP_ERR_ASSERTION        Null pointer as a parameter.
 * @retval FSP_ERR_NOT_OPEN         Instance control block is not initialized.
 **********************************************************************************************************************/
fsp_err_t R_WDT_StatusClear (wdt_ctrl_t * const p_ctrl, const wdt_status_t status)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;

    uint16_t value;
    uint16_t read_value;

#if WDT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(WDT_OPEN == p_instance_ctrl->wdt_open, FSP_ERR_NOT_OPEN);
#endif

    /* Casts to uint16_t to ensure value is handled as unsigned. */
    value = (uint16_t) status;

    /* Write zero to clear flags. */
    value = (uint16_t) ~value;
    value = (uint16_t) (value << 14);

    /* Read back status flags until required flag(s) cleared. */
    /* Flags cannot be cleared until the clock cycle after they are set.  */
    do
    {
        p_instance_ctrl->p_reg->WDTSR = value;
        read_value = p_instance_ctrl->p_reg->WDTSR;

        /* Isolate flags to clear. */
        read_value &= (uint16_t) ((uint16_t) status << 14);
    } while (0U != read_value);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Read the current count value of the WDT. Implements @ref wdt_api_t::counterGet.
 *
 * @retval FSP_SUCCESS          WDT current count successfully read.
 * @retval FSP_ERR_ASSERTION    Null pointer passed as a parameter.
 * @retval FSP_ERR_NOT_OPEN     Instance control block is not initialized.
 **********************************************************************************************************************/
fsp_err_t R_WDT_CounterGet (wdt_ctrl_t * const p_ctrl, uint32_t * const p_count)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;

#if WDT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_count);
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(WDT_OPEN == p_instance_ctrl->wdt_open, FSP_ERR_NOT_OPEN);
#endif

    *p_count  = (uint32_t) p_instance_ctrl->p_reg->WDTSR;
    *p_count &= WDT_PRV_WDTSR_COUNTER_MASK;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements wdt_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_WDT_CallbackSet (wdt_ctrl_t * const          p_ctrl,
                             void (                    * p_callback)(wdt_callback_args_t *),
                             void const * const          p_context,
                             wdt_callback_args_t * const p_callback_memory)
{
    wdt_instance_ctrl_t * p_instance_ctrl = (wdt_instance_ctrl_t *) p_ctrl;

#if WDT_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(WDT_OPEN == p_instance_ctrl->wdt_open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup WDT)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Gets clock division shift value.
 *
 * @param[in]   division     Right shift value used to divide clock frequency.
 **********************************************************************************************************************/
static uint32_t r_wdt_clock_divider_get (wdt_clock_division_t division)
{
    uint32_t shift;

    if (WDT_CLOCK_DIVISION_128 == division)
    {
        shift = 7U;                    /* log base 2(128) = 7 */
    }
    else
    {
        shift = g_wdt_division_lookup[division];
    }

    return shift;
}

/*******************************************************************************************************************//**
 * Parameter checking function for WDT Open
 *
 * @param[in]    p_instance_ctrl   Pointer to instance control structure
 * @param[in]    p_cfg             Pointer to configuration structure
 *
 * @retval FSP_SUCCESS              WDT successfully configured.
 * @retval FSP_ERR_ASSERTION        Null pointer, or one or more configuration options is invalid.
 * @retval FSP_ERR_ALREADY_OPEN     Module is already open.  This module can only be opened once.
 **********************************************************************************************************************/
static fsp_err_t r_wdt_parameter_checking (wdt_instance_ctrl_t * const p_instance_ctrl, wdt_cfg_t const * const p_cfg)
{
#if WDT_CFG_PARAM_CHECKING_ENABLE

    /* Check that control and config structure pointers are valid. */
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(WDT_OPEN != p_instance_ctrl->wdt_open, FSP_ERR_ALREADY_OPEN);

    /* Check timeout parameter is supported by WDT. */

    /* Enum checking is done here because some enums in wdt_timeout_t are not supported by the WDT peripheral (they are
     * included for other implementations of the watchdog interface). */
    FSP_ASSERT((p_cfg->timeout == WDT_TIMEOUT_1024) || (p_cfg->timeout == WDT_TIMEOUT_4096) || \
               (p_cfg->timeout == WDT_TIMEOUT_8192) || (p_cfg->timeout == WDT_TIMEOUT_16384));
#else
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);
#endif

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * WDT ISR.
 **********************************************************************************************************************/
void wdt_underflow_isr (uint32_t id)
{
    WDT_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    FSP_PARAMETER_NOT_USED(id);

    /* Call user registered callback */
    if (NULL != gp_wdt_ctrl)
    {
        if (NULL != gp_wdt_ctrl->p_callback)
        {
            wdt_callback_args_t args;

            /* Store callback arguments in memory provided by user if available.  */
            wdt_callback_args_t * p_args = gp_wdt_ctrl->p_callback_memory;
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

            p_args->p_context = gp_wdt_ctrl->p_context;

            gp_wdt_ctrl->p_callback(p_args);

            if (NULL != gp_wdt_ctrl->p_callback_memory)
            {
                /* Restore callback memory in case this is a nested interrupt. */
                *gp_wdt_ctrl->p_callback_memory = args;
            }
        }
    }

    WDT_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}
