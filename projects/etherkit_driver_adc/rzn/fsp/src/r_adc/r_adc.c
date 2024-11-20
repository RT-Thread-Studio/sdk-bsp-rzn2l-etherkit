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
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

#include "bsp_api.h"

/* Configuration for this package. */
#include "r_adc_cfg.h"

/* Private header file for this package. */
#include "r_adc.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define ADC_PRV_USEC_PER_SEC                    (1000000U)
#define ADC_PRV_MIN_ADCLK_HZ                    (18750000U)

#define ADC_PRV_HZ_PER_KHZ                      (1000U)

#define ADC_SHIFT_LEFT_ALIGNED_32_BIT           (16U)

#define ADC_OPEN                                (0x52414443U)

#define ADC_ADADC_AVEE_BIT                      (0x80U)

/* Sample and hold bypass applies to these channels. */
#define ADC_MASK_SAMPLE_HOLD_BYPASS_CHANNELS    (0x7U)

/* Sample and hold bypass starts at bit 8. */
#define ADC_MASK_SAMPLE_HOLD_BYPASS_SHIFT       (8U)

/* Stabilization time when BGR is enabled */
#define ADC_BGR_STABILIZATION_DELAY_US          (150U)

#define ADC_PRV_ADCSR_ADST_TRGE_MASK            (R_ADC121_ADCSR_ADST_Msk | R_ADC121_ADCSR_TRGE_Msk)
#define ADC_PRV_ADCSR_CLEAR_ADST_TRGE           (~ADC_PRV_ADCSR_ADST_TRGE_MASK)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
#if ADC_CFG_PARAM_CHECKING_ENABLE
static fsp_err_t r_adc_open_cfg_check(adc_cfg_t const * const p_cfg);
static fsp_err_t r_adc_open_cfg_resolution_check(adc_cfg_t const * const p_cfg);
static fsp_err_t r_adc_sample_state_cfg_check(adc_instance_ctrl_t * p_instance_ctrl, adc_sample_state_t * p_sample);

static fsp_err_t r_adc_scan_cfg_check_sample_hold(adc_instance_ctrl_t * const     p_instance_ctrl,
                                                  adc_channel_cfg_t const * const p_channel_cfg);

#endif

static void r_adc_open_sub(adc_instance_ctrl_t * const p_instance_ctrl, adc_cfg_t const * const p_cfg);

#if ADC_CFG_PARAM_CHECKING_ENABLE

static fsp_err_t r_adc_scan_cfg_check(adc_instance_ctrl_t * const     p_instance_ctrl,
                                      adc_channel_cfg_t const * const p_channel_cfg);

#endif

static void r_adc_scan_cfg(adc_instance_ctrl_t * const     p_instance_ctrl,
                           adc_channel_cfg_t const * const p_channel_cfg);
void           adc_scan_end_b_isr(void);
void           adc_scan_end_c_isr(void);
void           adc_scan_end_isr(void);
static int32_t r_adc_lowest_channel_get(uint32_t adc_mask);
static void    r_adc_scan_end_common_isr(adc_event_t event);

#if ADC_CFG_PARAM_CHECKING_ENABLE

/** Mask of valid channels on this MCU. */
static const uint32_t g_adc_valid_channels[] =
{
    BSP_FEATURE_ADC_UNIT_0_CHANNELS,
 #if BSP_FEATURE_ADC_UNIT_1_CHANNELS
    BSP_FEATURE_ADC_UNIT_1_CHANNELS
 #endif
};
#endif

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/** ADC Implementation of ADC. */
const adc_api_t g_adc_on_adc =
{
    .open           = R_ADC_Open,
    .scanCfg        = R_ADC_ScanCfg,
    .infoGet        = R_ADC_InfoGet,
    .scanStart      = R_ADC_ScanStart,
    .scanGroupStart = R_ADC_ScanGroupStart,
    .scanStop       = R_ADC_ScanStop,
    .scanStatusGet  = R_ADC_StatusGet,
    .read           = R_ADC_Read,
    .read32         = R_ADC_Read32,
    .close          = R_ADC_Close,
    .calibrate      = R_ADC_Calibrate,
    .offsetSet      = R_ADC_OffsetSet,
    .callbackSet    = R_ADC_CallbackSet,
};

/*******************************************************************************************************************//**
 * @addtogroup ADC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Sets the operational mode, trigger sources, interrupt  priority, and configurations for the peripheral as a whole.
 * If interrupt is enabled, the function registers a callback function pointer for notifying the user whenever a scan
 * has completed.
 *
 * @retval FSP_SUCCESS                     Module is ready for use.
 * @retval FSP_ERR_ASSERTION               An input argument is invalid.
 * @retval FSP_ERR_ALREADY_OPEN            The instance control structure has already been opened.
 * @retval FSP_ERR_IRQ_BSP_DISABLED        A callback is provided, but the interrupt is not enabled.
 * @retval FSP_ERR_IP_CHANNEL_NOT_PRESENT  The requested unit does not exist on this MCU.
 * @retval FSP_ERR_INVALID_HW_CONDITION    The ADC clock must be at least 1 MHz
 **********************************************************************************************************************/
fsp_err_t R_ADC_Open (adc_ctrl_t * p_ctrl, adc_cfg_t const * const p_cfg)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

    /*  Perform parameter checking */
#if ADC_CFG_PARAM_CHECKING_ENABLE

    /* Verify the pointers are valid */
    FSP_ASSERT(NULL != p_instance_ctrl);

    /* Verify the configuration parameters are valid   */
    fsp_err_t err = r_adc_open_cfg_check(p_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Check for valid argument values for options that are unique to the IP */
    err = r_adc_open_cfg_resolution_check(p_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Verify this unit has not already been initialized   */
    FSP_ERROR_RETURN(ADC_OPEN != p_instance_ctrl->opened, FSP_ERR_ALREADY_OPEN);

    /* If a callback is used, then make sure the scan end interrupt is enabled */
    if (NULL != p_cfg->p_callback)
    {
        FSP_ERROR_RETURN(p_cfg->scan_end_irq >= 0, FSP_ERR_IRQ_BSP_DISABLED);

        /* Group B interrupts are never required since group B can be configured in continuous scan mode when group A
         * has priority over group B. */
    }
#endif

    /* Save configurations. */
    p_instance_ctrl->p_cfg = p_cfg;

    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;

    /* Calculate the register base address. */
    uint32_t base_address;
    if (0U == p_cfg->unit)
    {
        base_address = (uint32_t) R_ADC120;
    }
    else
    {
        base_address = (uint32_t) R_ADC121;
    }

    p_instance_ctrl->p_reg = (R_ADC121_Type *) base_address;

    /* Initialize the hardware based on the configuration. */
    r_adc_open_sub(p_instance_ctrl, p_cfg);

    /* Set the interrupt priorities. */
    if (p_instance_ctrl->p_cfg->scan_end_irq >= 0)
    {
        R_BSP_IrqCfgEnable(p_instance_ctrl->p_cfg->scan_end_irq, p_cfg->scan_end_ipl, p_instance_ctrl);
    }

    if (p_instance_ctrl->p_cfg->scan_end_b_irq >= 0)
    {
        R_BSP_IrqCfgEnable(p_instance_ctrl->p_cfg->scan_end_b_irq, p_cfg->scan_end_b_ipl, p_instance_ctrl);
    }

    if (p_instance_ctrl->p_cfg->scan_end_c_irq >= 0)
    {
        R_BSP_IrqCfgEnable(p_instance_ctrl->p_cfg->scan_end_c_irq, p_cfg->scan_end_c_ipl, p_instance_ctrl);
    }

    /* Invalid scan mask (initialized for later). */
    p_instance_ctrl->scan_mask = 0U;

    /* Mark driver as opened by initializing it to "RADC" in its ASCII equivalent for this unit. */
    p_instance_ctrl->opened = ADC_OPEN;

    /* Return the error code */
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Configures the ADC scan parameters. Channel specific settings are set in this function. Pass a pointer to
 * @ref adc_channel_cfg_t to p_channel_cfg.
 *
 * @note This starts group B scans if adc_channel_cfg_t::priority_group_a is set to ADC_GROUP_A_GROUP_B_CONTINUOUS_SCAN.
 *
 * @retval FSP_SUCCESS                 Channel specific settings applied.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_ScanCfg (adc_ctrl_t * p_ctrl, void const * const p_channel_cfg)
{
    adc_channel_cfg_t const * p_adc_channel_cfg = (adc_channel_cfg_t const *) p_channel_cfg;
    adc_instance_ctrl_t     * p_instance_ctrl   = (adc_instance_ctrl_t *) p_ctrl;
    fsp_err_t                 err               = FSP_SUCCESS;

#if ADC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_adc_channel_cfg);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);

    err = r_adc_scan_cfg_check(p_instance_ctrl, p_adc_channel_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
#endif

    /* Configure the hardware based on the configuration */
    r_adc_scan_cfg(p_instance_ctrl, p_adc_channel_cfg);

    /* Save the scan mask locally; this is required for the infoGet function. */
    p_instance_ctrl->scan_mask = p_adc_channel_cfg->scan_mask;

    /* Return the error code */
    return err;
}

/*******************************************************************************************************************//**
 * Updates the user callback and has option of providing memory for callback structure.
 * Implements adc_api_t::callbackSet
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ADC_CallbackSet (adc_ctrl_t * const          p_ctrl,
                             void (                    * p_callback)(adc_callback_args_t *),
                             void const * const          p_context,
                             adc_callback_args_t * const p_callback_memory)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

#if (ADC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Starts a software scan or enables the hardware trigger for a scan depending on how the triggers were configured in
 * the R_ADC_Open call. If the unit was configured for ELC or external hardware triggering, then this function allows
 * the trigger signal to get to the ADC unit. The function is not able to control the generation of the trigger itself.
 * If the unit was configured for software triggering, then this function starts the software triggered scan.
 *
 * @pre Call R_ADC_ScanCfg after R_ADC_Open before starting a scan.
 *
 * @retval FSP_SUCCESS                 Scan started (software trigger) or hardware triggers enabled.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 * @retval FSP_ERR_IN_USE              Another scan is still in progress (software trigger).
 **********************************************************************************************************************/
fsp_err_t R_ADC_ScanStart (adc_ctrl_t * p_ctrl)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

    /* Perform parameter checking  */
#if ADC_CFG_PARAM_CHECKING_ENABLE

    /* Verify the pointers are valid */
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);
    if (ADC_GROUP_A_GROUP_B_CONTINUOUS_SCAN != p_instance_ctrl->p_reg->ADGSPCR)
    {
        FSP_ERROR_RETURN(0U == p_instance_ctrl->p_reg->ADCSR_b.ADST, FSP_ERR_IN_USE);
    }
#endif

    /* Enable hardware trigger or start software scan depending on mode. */
    p_instance_ctrl->p_reg->ADCSR = p_instance_ctrl->scan_start_adcsr;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @ref adc_api_t::scanGroupStart is not supported on the ADC. Use scanStart instead.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_ADC_ScanGroupStart (adc_ctrl_t * p_ctrl, adc_group_mask_t group_mask)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(group_mask);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * Stops the software scan or disables the unit from being triggered by the hardware trigger (ELC or external) based on
 * what type of trigger the unit was configured for in the R_ADC_Open function. Stopping a hardware triggered scan via
 * this function does not abort an ongoing scan, but prevents the next scan from occurring. Stopping a software
 * triggered scan aborts an ongoing scan.
 *
 * @retval FSP_SUCCESS                 Scan stopped (software trigger) or hardware triggers disabled.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_ScanStop (adc_ctrl_t * p_ctrl)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

    /*  Perform parameter checking */
#if ADC_CFG_PARAM_CHECKING_ENABLE

    /* Verify the pointers are valid */
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);
#endif

    /* Disable hardware trigger or stop software scan depending on mode. */
    p_instance_ctrl->p_reg->ADCSR = 0U;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Provides the status of any scan process that was started, including scans started by ELC or external triggers.
 *
 * @retval FSP_SUCCESS                 Module status stored in the provided pointer p_status
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_StatusGet (adc_ctrl_t * p_ctrl, adc_status_t * p_status)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

#if ADC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_status);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);
#endif

    /* Read the status of the ADST bit. ADST is set when a scan is in progress. */
    p_status->state = (adc_state_t) p_instance_ctrl->p_reg->ADCSR_b.ADST;

    /* Return the error code */
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Reads conversion results from a single channel or sensor.
 *
 * @retval FSP_SUCCESS                 Data read into provided p_data.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_Read (adc_ctrl_t * p_ctrl, adc_channel_t const reg_id, uint16_t * const p_data)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

    /* Perform parameter checking. */
#if ADC_CFG_PARAM_CHECKING_ENABLE

    /* Verify the pointers are valid */
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_data);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);

    /* Verify that the channel is valid for this MCU */
    if ((reg_id >= ADC_CHANNEL_0) && ((uint32_t) reg_id <= 31U))
    {
        uint32_t requested_channel_mask = (1U << (uint32_t) reg_id);
        FSP_ASSERT(0 != (requested_channel_mask & g_adc_valid_channels[p_instance_ctrl->p_cfg->unit]));
    }
    else
    {
        FSP_ASSERT((reg_id == ADC_CHANNEL_DUPLEX) || (reg_id == ADC_CHANNEL_DUPLEX_A) ||
                   (reg_id == ADC_CHANNEL_DUPLEX_B));
    }
#endif

    /* Read the data from the requested ADC conversion register and return it */
    *p_data = p_instance_ctrl->p_reg->ADDR[reg_id];

    /* Return the error code */
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Reads conversion results from a single channel or sensor register into a 32-bit result.
 *
 * @retval FSP_SUCCESS                 Data read into provided p_data.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_Read32 (adc_ctrl_t * p_ctrl, adc_channel_t const reg_id, uint32_t * const p_data)
{
    uint16_t result    = 0U;
    uint32_t result_32 = 0U;

#if ADC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_data);
#endif

    fsp_err_t err = R_ADC_Read(p_ctrl, reg_id, &result);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    result_32 = result;

    /* Left shift the result into the upper 16 bits if the unit is configured for left alignment. */
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;
    if (ADC_ALIGNMENT_LEFT == p_instance_ctrl->p_cfg->alignment)
    {
        result_32 <<= ADC_SHIFT_LEFT_ALIGNED_32_BIT;
    }

    *p_data = result_32;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Sets the sample state count for individual channels. This only needs to be set for special use cases. Normally, use
 * the default values out of reset.
 *
 * @note The sample states for the temperature and voltage sensor are set in R_ADC_ScanCfg.
 *
 * @retval FSP_SUCCESS                 Sample state count updated.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_SampleStateCountSet (adc_ctrl_t * p_ctrl, adc_sample_state_t * p_sample)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;
    fsp_err_t             err             = FSP_SUCCESS;

    /* Perform parameter checking */
#if ADC_CFG_PARAM_CHECKING_ENABLE

    /* Verify the pointers are valid */
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_sample);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);

    /* Verify arguments are legal */
    err = r_adc_sample_state_cfg_check(p_instance_ctrl, p_sample);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
#endif

    /* Set the sample state count for the specified register */
    p_instance_ctrl->p_reg->ADSSTR[p_sample->reg_id] = p_sample->num_states;

    /* Return the error code */
    return err;
}

/*******************************************************************************************************************//**
 * Returns the address of the lowest number configured channel and the total number of bytes to be read in order to
 * read the results of the configured channels and return the ELC Event name. If no channels are configured, then a
 * length of 0 is returned.
 *
 * @note In group mode, information is returned for group A only.  Calculating information for group B is not currently
 * supported.
 *
 * @retval FSP_SUCCESS                 Information stored in p_adc_info.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_InfoGet (adc_ctrl_t * p_ctrl, adc_info_t * p_adc_info)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;
    fsp_err_t             err             = FSP_SUCCESS;
    uint32_t              adc_mask        = 0;

#if ADC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_adc_info);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);
#endif

    /* Retrieve the scan mask of active channels from the control structure */
    adc_mask = p_instance_ctrl->scan_mask;

    /* If at least one channel is configured, determine the highest and lowest configured channels. */
    if (adc_mask != 0U)
    {
        int32_t lowest_channel = r_adc_lowest_channel_get(adc_mask);
        p_adc_info->p_address = &p_instance_ctrl->p_reg->ADDR[lowest_channel];

        /* Determine the highest channel that is configured. */
        int32_t highest_channel = 31 - __CLZ(adc_mask);

        /* Determine the size of data that must be read to read all the channels between and including the
         * highest and lowest channels.*/
        p_adc_info->length = (uint32_t) ((highest_channel - lowest_channel) + 1);
    }
    else
    {
        /* If no channels are configured, set the return length 0. */
        p_adc_info->length = 0U;
    }

    p_adc_info->transfer_size = TRANSFER_SIZE_2_BYTE;

    /* Specify the peripheral name in the ELC list */
    p_adc_info->elc_event =
        (elc_event_t) ((uint32_t) ELC_EVENT_ADC0_ADI +
                       (p_instance_ctrl->p_cfg->unit *
                        ((uint32_t) ELC_EVENT_ADC1_ADI - (uint32_t) ELC_EVENT_ADC0_ADI)));

    p_adc_info->elc_peripheral = (elc_peripheral_t) (ELC_PERIPHERAL_ADC0_A + (2U * p_instance_ctrl->p_cfg->unit));

    return err;
}

/*******************************************************************************************************************//**
 * This function ends any scan in progress, disables interrupts, and removes power to the A/D peripheral.
 *
 * @retval FSP_SUCCESS                 Module closed.
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 * @retval FSP_ERR_NOT_OPEN            Unit is not open.
 **********************************************************************************************************************/
fsp_err_t R_ADC_Close (adc_ctrl_t * p_ctrl)
{
    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) p_ctrl;

#if ADC_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ERROR_RETURN(ADC_OPEN == p_instance_ctrl->opened, FSP_ERR_NOT_OPEN);
#endif

    /* Mark driver as closed   */
    p_instance_ctrl->opened = 0U;

    /* Disable interrupts. */
    if (p_instance_ctrl->p_cfg->scan_end_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->scan_end_irq);
        R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->scan_end_irq, NULL);
    }

    if (p_instance_ctrl->p_cfg->scan_end_b_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->scan_end_b_irq);
        R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->scan_end_b_irq, NULL);
    }

    if (p_instance_ctrl->p_cfg->scan_end_c_irq >= 0)
    {
        R_BSP_IrqDisable(p_instance_ctrl->p_cfg->scan_end_c_irq);
        R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->scan_end_c_irq, NULL);
    }

    /* Disable triggers. */
    p_instance_ctrl->p_reg->ADSTRGR  = 0U;
    p_instance_ctrl->p_reg->ADGCTRGR = 0U;

    /* Stop the ADC. */
    p_instance_ctrl->p_reg->ADCSR = 0U;

#if BSP_FEATURE_ADC_HAS_SAMPLE_HOLD_REG
    p_instance_ctrl->p_reg->ADSHCR = 0U;
#endif

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_STOP(FSP_IP_ADC12, p_instance_ctrl->p_cfg->unit);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Return the error code */
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @ref adc_api_t::R_ADC_Calibrate is not supported on the ADC.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_ADC_Calibrate (adc_ctrl_t * const p_ctrl, void const * p_extend)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_extend);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @ref adc_api_t::offsetSet is not supported on the ADC.
 *
 * @retval FSP_ERR_UNSUPPORTED         Function not supported in this implementation.
 **********************************************************************************************************************/
fsp_err_t R_ADC_OffsetSet (adc_ctrl_t * const p_ctrl, adc_channel_t const reg_id, int32_t offset)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(reg_id);
    FSP_PARAMETER_NOT_USED(offset);

    /* Return the unsupported error. */
    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup ADC)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

#if ADC_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * Checks the sample state configuration.
 *
 * @param[in]  p_instance_ctrl             Pointer to instance control structure
 * @param[in]  p_sample                    Pointer to sample state configuration
 *
 * @retval FSP_SUCCESS                     No configuration errors detected
 * @retval FSP_ERR_ASSERTION               An input argument is invalid.
 **********************************************************************************************************************/
static fsp_err_t r_adc_sample_state_cfg_check (adc_instance_ctrl_t * p_instance_ctrl, adc_sample_state_t * p_sample)
{
    /* Used to prevent compiler warning */
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);

    adc_sample_state_reg_t reg_id = p_sample->reg_id;

    /* Verify the requested channel exists on the MCU. */
    if (reg_id >= ADC_SAMPLE_STATE_REG_CHANNEL_0)
    {
        uint32_t requested_channel_mask = (1U << (uint32_t) reg_id);
        FSP_ASSERT(0 != (requested_channel_mask & g_adc_valid_channels[p_instance_ctrl->p_cfg->unit]));
    }

    /* Verify the requested sample states is not less than the minimum. */
    FSP_ASSERT(p_sample->num_states >= ADC_SAMPLE_STATE_COUNT_MIN);

    return FSP_SUCCESS;
}

#endif

#if ADC_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * Validates the configuration arguments for illegal combinations or options.
 *
 * @param[in]  p_cfg                   Pointer to configuration structure
 *
 * @retval FSP_SUCCESS                     No configuration errors detected
 * @retval FSP_ERR_ASSERTION               An input argument is invalid.
 * @retval FSP_ERR_IP_CHANNEL_NOT_PRESENT  ADC unit not present on this MCU
 * @retval FSP_ERR_INVALID_HW_CONDITION    The ADC clock must be at least 1 MHz
 **********************************************************************************************************************/
static fsp_err_t r_adc_open_cfg_check (adc_cfg_t const * const p_cfg)
{
    FSP_ASSERT(NULL != p_cfg);

    /* Verify the unit exists on the MCU. */
    FSP_ERROR_RETURN(((1U << p_cfg->unit) & BSP_FEATURE_ADC_VALID_UNIT_MASK), FSP_ERR_IP_CHANNEL_NOT_PRESENT);

    /* Verify the ADC clock frequency is 25 MHz. (ADCCLK is fixed at 25MHz) */
    uint32_t freq_hz = R_FSP_SystemClockHzGet(BSP_FEATURE_ADC_CLOCK_SOURCE);
    FSP_ERROR_RETURN(freq_hz >= ADC_PRV_MIN_ADCLK_HZ, FSP_ERR_INVALID_HW_CONDITION);

    /* Check for valid argument values for addition/averaging. Reference section "A/D-Converted Value
     * Addition/Average Count Select Register (ADADC)" in the RZ microprocessor User's Manual for details. */
    adc_extended_cfg_t const * p_cfg_extend = (adc_extended_cfg_t const *) p_cfg->p_extend;
    if (ADC_ADD_OFF != p_cfg_extend->add_average_count)
    {
 #if BSP_FEATURE_ADC_ADDITION_SUPPORTED

        /* 3-time conversion and 16-time conversion in average mode are prohibited. */
        FSP_ASSERT(ADC_ADD_AVERAGE_THREE != p_cfg_extend->add_average_count);
        FSP_ASSERT(ADC_ADD_AVERAGE_SIXTEEN != p_cfg_extend->add_average_count);
 #else

        /* The ADC16 supports averaging only, it does not support addition. */
        FSP_ASSERT(0U != (ADC_ADADC_AVEE_BIT & p_cfg_extend->add_average_count));
 #endif
    }

    /* Only synchronous triggers (ELC) allowed in group scan mode (reference TRSA documentation in section
     * A/D conversion in Double Trigger Mode)" in the RZ microprocessor User's Manual for details. */
    if ((ADC_MODE_GROUP_SCAN == p_cfg->mode) || (ADC_DOUBLE_TRIGGER_DISABLED != p_cfg_extend->double_trigger_mode))
    {
        FSP_ASSERT(ADC_TRIGGER_SYNC_ELC == p_cfg->trigger);

        if ((ADC_MODE_GROUP_SCAN == p_cfg->mode))
        {
            FSP_ASSERT(ADC_TRIGGER_SYNC_ELC == p_cfg_extend->trigger_group_b);
        }
    }

    return FSP_SUCCESS;
}

#endif

#if ADC_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * This function validates the resolution configuration arguments for illegal combinations or options.
 *
 * @param[in]  p_cfg                   Pointer to configuration structure
 *
 * @retval FSP_SUCCESS                 No configuration errors detected
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 **********************************************************************************************************************/
static fsp_err_t r_adc_open_cfg_resolution_check (adc_cfg_t const * const p_cfg)
{
    FSP_ASSERT(ADC_RESOLUTION_12_BIT == p_cfg->resolution);

    return FSP_SUCCESS;
}

#endif

#if ADC_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * Checks the sample and hold arguments
 *
 * @param[in]  p_instance_ctrl         Pointer to instance control block
 * @param[in]  p_channel_cfg           Pointer to channel configuration
 *
 * @retval FSP_SUCCESS                 No configuration errors detected
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 **********************************************************************************************************************/
static fsp_err_t r_adc_scan_cfg_check_sample_hold (adc_instance_ctrl_t * const     p_instance_ctrl,
                                                   adc_channel_cfg_t const * const p_channel_cfg)
{
 #if !BSP_FEATURE_ADC_HAS_SAMPLE_HOLD_REG

    /* If the MCU does not have sample and hold, verify the sample and hold feature is not used. */
    FSP_ASSERT(0U == p_channel_cfg->sample_hold_mask);
    FSP_PARAMETER_NOT_USED(p_instance_ctrl);
 #else
    if (0U != p_channel_cfg->sample_hold_mask)
    {
        /* Sample and Hold channels can only be 0, 1, 2(unit 0 only) and must have at least minimum state count specified (reference
         * section "A/D Sample and Hold Circuit Control Register (ADSHCR)" in the RZ microprocessor User's Manual for details. */
        FSP_ASSERT(0U == p_instance_ctrl->p_cfg->unit);
        FSP_ASSERT(p_channel_cfg->sample_hold_mask <= ADC_SAMPLE_HOLD_CHANNELS);
        FSP_ASSERT(p_channel_cfg->sample_hold_states >= ADC_SAMPLE_STATE_HOLD_COUNT_MIN);
    }
 #endif

    return FSP_SUCCESS;
}

#endif

/*******************************************************************************************************************//**
 * The Open function applies power to the A/D peripheral, sets the operational mode, trigger sources, and
 * configurations common to all channels and sensors.
 *
 * @param[in]  p_instance_ctrl         Pointer to instance control block
 * @param[in]  p_cfg                   Pointer to configuration structure
 **********************************************************************************************************************/
static void r_adc_open_sub (adc_instance_ctrl_t * const p_instance_ctrl, adc_cfg_t const * const p_cfg)
{
    adc_extended_cfg_t const * p_cfg_extend = (adc_extended_cfg_t const *) p_cfg->p_extend;

    /* Determine the value for ADCSR:
     *   * The configured mode is set in ADCSR.ADCS.
     *   * ADCSR.GBADIE is always set by this driver. It will only trigger an interrupt in group mode if the group B
     *     interrupt is enabled.
     *   * If double-trigger mode is selected ADCSR.DBLANS is set to the chosen double-trigger scan channel and
     *     ADCSR.DBLE is set to 1; otherwise, both are set to 0.
     *   * The configured trigger mode is set in ADCSR.EXTRG and ADCSR.TRGE.
     *   * The value to set in ADCSR to start a scan is stored in the control structure. ADCSR.ADST is set in
     *     R_ADC_ScanStart if software trigger mode is used.
     */
    uint32_t adcsr = (uint32_t) (p_cfg->mode << R_ADC121_ADCSR_ADCS_Pos);
    adcsr |= (uint32_t) (R_ADC121_ADCSR_GBADIE_Msk);
    adcsr |= (uint32_t) (R_ADC121_ADCSR_ADIE_Msk);
    adcsr |= ((uint32_t) p_cfg->trigger << R_ADC121_ADCSR_EXTRG_Pos);

    if (ADC_DOUBLE_TRIGGER_DISABLED != p_cfg_extend->double_trigger_mode)
    {
        adcsr |= R_ADC121_ADCSR_TRGE_Msk | R_ADC121_ADCSR_DBLE_Msk;
    }
    else if (ADC_TRIGGER_SOFTWARE == p_cfg->trigger)
    {
        adcsr |= R_ADC121_ADCSR_ADST_Msk;
    }
    else
    {
        /* Do nothing. */
    }

    p_instance_ctrl->scan_start_adcsr = (uint16_t) adcsr;

    /* Determine the value for ADCER:
     *   * The resolution is set as configured in ADCER.ADPRC (on MCUs that have this bitfield).
     *   * The alignment is set as configured in ADCER.ADFMT (on MCUs that have this bitfield).
     *   * The clearing option is set as configured in ADCER.ACE.
     *   * Always select data range of 0 - 32767 in ADCER.INV (on MCUs that have this bitfield).
     *   * Always disable self-diagnosis (unsupported in this module).
     */
    uint32_t adcer = 0U;
#if BSP_FEATURE_ADC_HAS_ADCER_ADPRC
    adcer |= (uint32_t) p_cfg->resolution << R_ADC121_ADCER_ADPRC_Pos;
#endif
#if BSP_FEATURE_ADC_HAS_ADCER_ADRFMT
    adcer |= (uint32_t) p_cfg->alignment << R_ADC121_ADCER_ADRFMT_Pos;
#endif
    adcer |= (uint32_t) p_cfg_extend->clearing << R_ADC121_ADCER_ACE_Pos;

    /* Determine the value for ADADC:
     *   * The addition/averaging modes are set as configured in ADADC.ADC and ADADC.AVEE.
     *   * On MCUs that do not have the ADADC.AVEE bit (addition not supported), the ADADC.AVEE bit is cleared.
     */
    uint32_t adadc = p_cfg_extend->add_average_count;
#if !BSP_FEATURE_ADC_ADDITION_SUPPORTED
    adadc &= ~ADC_ADADC_AVEE_BIT;
#endif

    /* Apply clock to peripheral. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_ADC12, p_cfg->unit);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Set the predetermined values for ADCSR, ADSTRGR, ADGCTRGR, ADCER, and ADADC without setting ADCSR.ADST or ADCSR.TRGE.
     * ADCSR.ADST or ADCSR.TRGE are set as configured in R_ADC_ScanStart. */
    p_instance_ctrl->p_reg->ADCSR   = (uint16_t) (adcsr & ADC_PRV_ADCSR_CLEAR_ADST_TRGE);
    p_instance_ctrl->p_reg->ADSTRGR = (uint16_t) ((p_cfg_extend->adc_start_trigger_a << R_ADC121_ADSTRGR_TRSA_Pos) |
                                                  (p_cfg_extend->adc_start_trigger_b << R_ADC121_ADSTRGR_TRSB_Pos));
    p_instance_ctrl->p_reg->ADCER   = (uint16_t) adcer;
    p_instance_ctrl->p_reg->ADADC   = (uint8_t) adadc;
    p_instance_ctrl->p_reg->ADELCCR = (uint8_t) p_cfg_extend->adc_elc_ctrl;

    if (true == p_cfg_extend->adc_start_trigger_c_enabled)
    {
        p_instance_ctrl->p_reg->ADGCTRGR =
            (uint8_t) ((p_cfg_extend->adc_start_trigger_c << R_ADC121_ADGCTRGR_TRSC_Pos) |
                       (uint8_t) (R_ADC121_ADGCTRGR_GCADIE_Msk |
                                  R_ADC121_ADGCTRGR_GRCE_Msk));
    }
}

#if ADC_CFG_PARAM_CHECKING_ENABLE

/*******************************************************************************************************************//**
 * This function does extensive checking on channel mask settings based upon operational mode.
 *
 * @param[in]  p_instance_ctrl         Pointer to instance control block
 * @param[in]  p_channel_cfg           Pointer to channel configuration
 *
 * @retval FSP_SUCCESS                 No configuration errors detected
 * @retval FSP_ERR_ASSERTION           An input argument is invalid.
 **********************************************************************************************************************/
static fsp_err_t r_adc_scan_cfg_check (adc_instance_ctrl_t * const     p_instance_ctrl,
                                       adc_channel_cfg_t const * const p_channel_cfg)
{
    fsp_err_t err;
    uint16_t  unit = p_instance_ctrl->p_cfg->unit;
    adc_extended_cfg_t const * p_cfg_extend = (adc_extended_cfg_t const *) p_instance_ctrl->p_cfg->p_extend;

    /* Verify at least one channel is selected for normal / group A. */
    uint32_t valid_channels = g_adc_valid_channels[unit];
    FSP_ASSERT((0U != p_channel_cfg->scan_mask) && (0U == (p_channel_cfg->scan_mask & (~valid_channels))));

    if (ADC_MODE_GROUP_SCAN == p_instance_ctrl->p_cfg->mode)
    {
        /* Verify at least one channel is selected for group B. */
        FSP_ASSERT((0U != p_channel_cfg->scan_mask_group_b) &&
                   (0U == (p_channel_cfg->scan_mask_group_b & (~valid_channels))));

        /* Cannot have the same channel in both groups. */
        FSP_ASSERT(0 == (p_channel_cfg->scan_mask & p_channel_cfg->scan_mask_group_b));

        if (true == p_cfg_extend->adc_start_trigger_c_enabled)
        {
            /* Verify at least one channel is selected for group C. */
            FSP_ASSERT((0U != p_channel_cfg->scan_mask_group_c) &&
                       (0U == (p_channel_cfg->scan_mask_group_c & (~valid_channels))));

            /* Cannot have the same channel in both groups. */
            FSP_ASSERT(0 == (p_channel_cfg->scan_mask & p_channel_cfg->scan_mask_group_c));
        }
    }
    else
    {
        /* If group mode is not enabled, no channels can be selected for group B. */
        FSP_ASSERT(ADC_MASK_OFF == p_channel_cfg->scan_mask_group_b);

        /* If group mode is not enabled, no channels can be selected for group C. */
        FSP_ASSERT(ADC_MASK_OFF == p_channel_cfg->scan_mask_group_c);
    }

    /* Verify that if addition is enabled, then at least one channel is selected. */
    if (ADC_ADD_OFF != p_cfg_extend->add_average_count)
    {
        /* Addition mask should not include bits from inactive channels.
         * This also serves as a check for valid channels in the addition mask */
        uint32_t tmp_mask = p_channel_cfg->scan_mask_group_b | p_channel_cfg->scan_mask;
        FSP_ASSERT((0U == (p_channel_cfg->add_mask & ~tmp_mask)) && (0U != p_channel_cfg->add_mask));
    }
    else
    {
        /* Channels cannot be selected for addition if addition is not used. */
        FSP_ASSERT(ADC_MASK_OFF == p_channel_cfg->add_mask);
    }

    /* Check sample and hold settings. */
    err = r_adc_scan_cfg_check_sample_hold(p_instance_ctrl, p_channel_cfg);

    return err;
}

#endif

/*******************************************************************************************************************//**
 * This function does extensive checking on channel mask settings based upon operational mode. Mask registers are
 * initialized and interrupts enabled in peripheral. Interrupts are also enabled in ICU if corresponding priority
 * is not 0.
 *
 * @param[in]  p_instance_ctrl         Pointer to instance control block
 * @param[in]  p_channel_cfg           Pointer to channel configuration
 **********************************************************************************************************************/
static void r_adc_scan_cfg (adc_instance_ctrl_t * const p_instance_ctrl, adc_channel_cfg_t const * const p_channel_cfg)
{
    adc_extended_cfg_t const * p_cfg_extend = (adc_extended_cfg_t const *) p_instance_ctrl->p_cfg->p_extend;

    p_instance_ctrl->p_reg->ADANSA0 = (uint16_t) (p_channel_cfg->scan_mask);
    p_instance_ctrl->p_reg->ADANSB0 = (uint16_t) (p_channel_cfg->scan_mask_group_b);
    p_instance_ctrl->p_reg->ADANSC0 = (uint16_t) (p_channel_cfg->scan_mask_group_c);
    p_instance_ctrl->p_reg->ADADS0  = (uint16_t) (p_channel_cfg->add_mask);

#if BSP_FEATURE_ADC_HAS_SAMPLE_HOLD_REG

    /* Configure sample and hold. */
    uint32_t adshcr = p_channel_cfg->sample_hold_states;
    adshcr |= (p_channel_cfg->sample_hold_mask & ADC_MASK_SAMPLE_HOLD_BYPASS_CHANNELS) <<
              ADC_MASK_SAMPLE_HOLD_BYPASS_SHIFT;
    p_instance_ctrl->p_reg->ADSHCR = (uint16_t) adshcr;
#endif

    /* Set group A priority action (not interrupt priority!)
     * This will also start the Group B scans if configured for ADC_GROUP_A_GROUP_B_CONTINUOUS_SCAN.
     */
    p_instance_ctrl->p_reg->ADGSPCR = (uint16_t) p_channel_cfg->priority_group_a;

    /* In double-trigger mode set the channel select bits to the highest selected channel number then return. */
    if (ADC_DOUBLE_TRIGGER_DISABLED != p_cfg_extend->double_trigger_mode)
    {
        uint32_t adcsr = p_instance_ctrl->p_reg->ADCSR;
        adcsr = (adcsr & ~R_ADC121_ADCSR_DBLANS_Msk) + (31U - __CLZ(p_channel_cfg->scan_mask));

        p_instance_ctrl->p_reg->ADCSR      = (uint16_t) adcsr;
        p_instance_ctrl->scan_start_adcsr |= (uint16_t) adcsr;
    }
}

/*******************************************************************************************************************//**
 * Returns the lowest channel index that is configured  in order to read the results of the configured channels.
 *
 * @param[in]  adc_mask  scan mask of active channels retrieved from the control structure
 *
 * @retval  adc_mask_count  index value of lowest channel
 **********************************************************************************************************************/
static int32_t r_adc_lowest_channel_get (uint32_t adc_mask)
{
    /* Initialize the mask result */
    uint32_t adc_mask_result = 0U;
    int32_t  adc_mask_count  = -1;
    while (0U == adc_mask_result)
    {
        /* Increment channel until a channel is found in the mask. */
        adc_mask_count++;
        adc_mask_result = (uint32_t) (adc_mask & (1U << adc_mask_count));
    }

    return adc_mask_count;
}

/*******************************************************************************************************************//**
 * Clears interrupt flag and calls a callback to notify application of the event.
 *
 * @param[in]  event                   Event that triggered the ISR
 **********************************************************************************************************************/
static void r_adc_scan_end_common_isr (adc_event_t event)
{
    ADC_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE;

    adc_instance_ctrl_t * p_instance_ctrl = (adc_instance_ctrl_t *) R_FSP_IsrContextGet(R_FSP_CurrentIrqGet());
    adc_callback_args_t   args;

    /* Store callback arguments in memory provided by user if available.  This allows callback arguments to be
     * stored in non-secure memory so they can be accessed by a non-secure callback function. */
    adc_callback_args_t * p_args = p_instance_ctrl->p_callback_memory;
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

    p_args->event = event;

    /* Store the unit number into the callback argument */
    p_args->unit = p_instance_ctrl->p_cfg->unit;

    /* Initialize the channel to 0.  It is not used in this implementation. */
    p_args->channel = ADC_CHANNEL_0;

    /* Populate the context field. */
    p_args->p_context = p_instance_ctrl->p_context;

    /* If a callback was provided, call it with the argument */
    if (NULL != p_instance_ctrl->p_callback)
    {
        /* If the project is not Trustzone Secure, then it will never need to change security state in order to call the callback. */
        p_instance_ctrl->p_callback(p_args);
    }

    if (NULL != p_instance_ctrl->p_callback_memory)
    {
        /* Restore callback memory in case this is a nested interrupt. */
        *p_instance_ctrl->p_callback_memory = args;
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE;

    ADC_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}

/*******************************************************************************************************************//**
 * This function implements the unit 0 interrupt handler for normal/Group A/double trigger scan complete.
 **********************************************************************************************************************/
void adc_scan_end_isr (void)
{
    r_adc_scan_end_common_isr(ADC_EVENT_SCAN_COMPLETE);
}

/*******************************************************************************************************************//**
 * This function implements the interrupt handler for Group B scan complete.
 **********************************************************************************************************************/
void adc_scan_end_b_isr (void)
{
    r_adc_scan_end_common_isr(ADC_EVENT_SCAN_COMPLETE_GROUP_B);
}

/*******************************************************************************************************************//**
 * This function implements the interrupt handler for Group C scan complete.
 **********************************************************************************************************************/
void adc_scan_end_c_isr (void)
{
    r_adc_scan_end_common_isr(ADC_EVENT_SCAN_COMPLETE_GROUP_C);
}
