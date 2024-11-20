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

#ifndef R_ADC_H
#define R_ADC_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdlib.h>

/* Fixed width integer support. */
#include <stdint.h>

/* bool support */
#include <stdbool.h>
#include "bsp_api.h"
#include "r_adc_cfg.h"
#include "r_adc_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/*******************************************************************************************************************//**
 * @addtogroup ADC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Typical values that can be used to modify the sample states.
 * The minimum sample state count value is either 6 or 7 depending on the clock ratios.
 * It is fixed to 7 based on the fact that at the lowest ADC conversion clock supported (1 MHz)
 * this extra state will lead to at worst a "1 microsecond" increase in conversion time.
 * At 60 MHz the extra sample state will add 16.7 ns to the conversion time.
 */
#define ADC_SAMPLE_STATE_COUNT_MIN             (7U)
#define ADC_SAMPLE_STATE_COUNT_MAX             (255U)

/* Typical values that can be used for the sample and hold counts for the channels 0-2*/
/* Minimum sample and hold states */
#define ADC_SAMPLE_STATE_HOLD_COUNT_MIN        (4U)

/* Default sample and hold states */
#define ADC_SAMPLE_STATE_HOLD_COUNT_DEFAULT    (24U)

/** For ADC Scan configuration adc_channel_cfg_t::scan_mask, adc_channel_cfg_t::scan_mask_group_b,
 *  adc_channel_cfg_t::add_mask and adc_channel_cfg_t::sample_hold_mask.
 *  Use bitwise OR to combine these masks for desired channels and sensors.    */
typedef enum e_adc_mask
{
    ADC_MASK_OFF        = (0U),        ///< No channels selected
    ADC_MASK_CHANNEL_0  = (1U << 0U),  ///< Channel 0 mask
    ADC_MASK_CHANNEL_1  = (1U << 1U),  ///< Channel 1 mask
    ADC_MASK_CHANNEL_2  = (1U << 2U),  ///< Channel 2 mask
    ADC_MASK_CHANNEL_3  = (1U << 3U),  ///< Channel 3 mask
    ADC_MASK_CHANNEL_4  = (1U << 4U),  ///< Channel 4 mask
    ADC_MASK_CHANNEL_5  = (1U << 5U),  ///< Channel 5 mask
    ADC_MASK_CHANNEL_6  = (1U << 6U),  ///< Channel 6 mask
    ADC_MASK_CHANNEL_7  = (1U << 7U),  ///< Channel 7 mask
    ADC_MASK_CHANNEL_8  = (1U << 8U),  ///< Channel 8 mask
    ADC_MASK_CHANNEL_9  = (1U << 9U),  ///< Channel 9 mask
    ADC_MASK_CHANNEL_10 = (1U << 10U), ///< Channel 10 mask
    ADC_MASK_CHANNEL_11 = (1U << 11U), ///< Channel 11 mask
    ADC_MASK_CHANNEL_12 = (1U << 12U), ///< Channel 12 mask
    ADC_MASK_CHANNEL_13 = (1U << 13U), ///< Channel 13 mask
    ADC_MASK_CHANNEL_14 = (1U << 14U), ///< Channel 14 mask
    ADC_MASK_CHANNEL_15 = (1U << 15U), ///< Channel 15 mask
} adc_mask_t;

/** ADC data sample addition and averaging options */
typedef enum e_adc_add
{
    ADC_ADD_OFF             = 0,       ///< Addition turned off for channels/sensors
    ADC_ADD_TWO             = 1,       ///< Add two samples
    ADC_ADD_THREE           = 2,       ///< Add three samples
    ADC_ADD_FOUR            = 3,       ///< Add four samples
    ADC_ADD_SIXTEEN         = 5,       ///< Add sixteen samples
    ADC_ADD_AVERAGE_TWO     = 0x81,    ///< Average two samples
    ADC_ADD_AVERAGE_THREE   = 0x82,    ///< Average three samples
    ADC_ADD_AVERAGE_FOUR    = 0x83,    ///< Average four samples
    ADC_ADD_AVERAGE_SIXTEEN = 0x85,    ///< Average sixteen samples
} adc_add_t;

/** ADC clear after read definitions */
typedef enum e_adc_clear
{
    ADC_CLEAR_AFTER_READ_OFF = 0,      ///< Clear after read off
    ADC_CLEAR_AFTER_READ_ON  = 1       ///< Clear after read on
} adc_clear_t;

/** ADC sample state registers */
typedef enum e_adc_sample_state_reg
{
    ADC_SAMPLE_STATE_REG_CHANNEL_0 = 0,         ///< Sample state register channel 0
    ADC_SAMPLE_STATE_REG_CHANNEL_1,             ///< Sample state register channel 1
    ADC_SAMPLE_STATE_REG_CHANNEL_2,             ///< Sample state register channel 2
    ADC_SAMPLE_STATE_REG_CHANNEL_3,             ///< Sample state register channel 3
    ADC_SAMPLE_STATE_REG_CHANNEL_4,             ///< Sample state register channel 4
    ADC_SAMPLE_STATE_REG_CHANNEL_5,             ///< Sample state register channel 5
    ADC_SAMPLE_STATE_REG_CHANNEL_6,             ///< Sample state register channel 6
    ADC_SAMPLE_STATE_REG_CHANNEL_7,             ///< Sample state register channel 7
    ADC_SAMPLE_STATE_REG_CHANNEL_8,             ///< Sample state register channel 8
    ADC_SAMPLE_STATE_REG_CHANNEL_9,             ///< Sample state register channel 9
    ADC_SAMPLE_STATE_REG_CHANNEL_10,            ///< Sample state register channel 10
    ADC_SAMPLE_STATE_REG_CHANNEL_11,            ///< Sample state register channel 11
    ADC_SAMPLE_STATE_REG_CHANNEL_12,            ///< Sample state register channel 12
    ADC_SAMPLE_STATE_REG_CHANNEL_13,            ///< Sample state register channel 13
    ADC_SAMPLE_STATE_REG_CHANNEL_14,            ///< Sample state register channel 14
    ADC_SAMPLE_STATE_REG_CHANNEL_15,            ///< Sample state register channel 15
    ADC_SAMPLE_STATE_REG_CHANNEL_16_TO_31 = -3, ///< Sample state register channel 16 to 31
} adc_sample_state_reg_t;

/** ADC action for group A interrupts group B scan.
 * This enumeration is used to specify the priority between Group A and B in group mode.  */
typedef enum e_adc_group_a
{
    ADC_GROUP_A_PRIORITY_OFF             = 0,      ///< Group A ignored and does not interrupt ongoing group B scan
    ADC_GROUP_A_GROUP_B_WAIT_FOR_TRIGGER = 1,      ///< Group A interrupts Group B(single scan) which restarts at next Group B trigger
    ADC_GROUP_A_GROUP_B_RESTART_SCAN     = 3,      ///< Group A interrupts Group B(single scan) which restarts immediately after Group A scan is complete
    ADC_GROUP_A_GROUP_B_RESUME_SCAN      = 0x4003, ///< Resume scanning of interrupted channels
    ADC_GROUP_A_GROUP_B_CONTINUOUS_SCAN  = 0x8001, ///< Group A interrupts Group B(continuous scan) which continues scanning without a new Group B trigger
} adc_group_a_t;

/** Defines the registers settings for the ADC trigger. */
typedef enum e_adc_active_trigger
{
    ADC_ACTIVE_TRIGGER_EXTERNAL = (0x00U),            ///< Input pin for the trigger

    /* ADC_TRIGGER_TRGXX is unit 0 only */
    ADC_ACTIVE_TRIGGER_TRGA1N    = (0x02U),           ///< Compare match with or input capture to MTU1.TGRA
    ADC_ACTIVE_TRIGGER_TRGA2N    = (0x03U),           ///< Compare match with or input capture to MTU2.TGRA
    ADC_ACTIVE_TRIGGER_TRGA3N    = (0x04U),           ///< Compare match with or input capture to MTU3.TGRA
    ADC_ACTIVE_TRIGGER_TRGA0N    = (0x01U),           ///< Compare match with or input capture to MTU0.TGRA
    ADC_ACTIVE_TRIGGER_TRGA4N    = (0x05U),           ///< Compare match with or input capture to MTU4.TGRA, or an underflow of MTU4.TCNT (in the trough) in complementary PWM mode
    ADC_ACTIVE_TRIGGER_TRGA6N    = (0x06U),           ///< Compare match with MTU6.TGRA
    ADC_ACTIVE_TRIGGER_TRGA7N    = (0x07U),           ///< Compare match with MTU7.TGRA
    ADC_ACTIVE_TRIGGER_TRG0N     = (0x08U),           ///< Compare match with MTU0.TGRE
    ADC_ACTIVE_TRIGGER_TRG4AN    = (0x09U),           ///< Compare match between MTU4.TADCORA and MTU4.TCNT
    ADC_ACTIVE_TRIGGER_TRG4BN    = (0x0AU),           ///< Compare match between MTU4.TADCORB and MTU4.TCNT
    ADC_ACTIVE_TRIGGER_TRG4AN_BN = (0x0BU),           ///< Compare match between MTU4.TADCORA and MTU4.TCNT, or between MTU4.TADCORB and MTU4.TCNT
    ADC_ACTIVE_TRIGGER_TRG4ABN   = (0x0CU),           ///< Compare match between MTU4.TADCORA and MTU4.TCNT, and between MTU4.TADCORB and MTU4.TCNT (when interrupt skipping function 2 is in use)
    ADC_ACTIVE_TRIGGER_TRG7AN    = (0x0DU),           ///< Compare match between MTU7.TADCORA and MTU7.TCNT
    ADC_ACTIVE_TRIGGER_TRG7BN    = (0x0EU),           ///< Compare match between MTU7.TADCORB and MTU7.TCNT
    ADC_ACTIVE_TRIGGER_TRG7AN_BN = (0x0FU),           ///< Compare match between MTU7.TADCORA and MTU7.TCNT, or between MTU7.TADCORB and MTU7.TCNT
    ADC_ACTIVE_TRIGGER_TRG7ABN   = (0x10U),           ///< Compare match between MTU7.TADCORA and MTU7.TCNT, and between MTU7.TADCORB and MTU7.TCNT
    ///< (when interrupt skipping function 2 is in use)

    ADC_ACTIVE_TRIGGER_ELC_TRIGGER         = (0x11U), ///< A/D Startup source A from ELC
    ADC_ACTIVE_TRIGGER_ELC_TRIGGER_GROUP_B = (0x12U), ///< A/D Startup source B from ELC
    ADC_ACTIVE_TRIGGER_DISABLED            = (0x3FU)  ///< A/D Start trigger disabled
} adc_active_trigger_t;

/** ADC double-trigger mode definitions */
typedef enum e_adc_double_trigger
{
    ADC_DOUBLE_TRIGGER_DISABLED         = 0, ///< Double-triggering disabled
    ADC_DOUBLE_TRIGGER_ENABLED          = 1, ///< Double-triggering enabled
    ADC_DOUBLE_TRIGGER_ENABLED_EXTENDED = 2, ///< Double-triggering enabled on both ADC ELC events
} adc_double_trigger_t;

/** AD event link control definitions. */
typedef enum e_adc_elc
{
    ADC_ELC_SINGLE_SCAN      = 0x00U,  ///< At the end of a single scan    GCELC = 0b, ELCC[1:0] = 00b
    ADC_ELC_CONTINUOUS_SCAN  = 0x00U,  ///< At the end of a contiuous scan GCELC = 0b, ELCC[1:0] = 00b
    ADC_ELC_GROUP_A_SCAN     = 0x00U,  ///< At the end of a group_a scan   GCELC = 0b, ELCC[1:0] = 00b
    ADC_ELC_GROUP_B_SCAN     = 0x01U,  ///< At the end of a group_b scan   GCELC = 0b, ELCC[1:0] = 01b
    ADC_ELC_GROUP_A_B_C_SCAN = 0x02U,  ///< At the end of a group_abc scan GCELC = 0b, ELCC[1:0] = 1xb
    ADC_ELC_GROUP_C_SCAN     = 0x04U,  ///< At the end of a group_c scan   GCELC = 1b, ELCC[1:0] = 00b
} adc_elc_t;

/** ADC sample state configuration */
typedef struct st_adc_sample_state
{
    adc_sample_state_reg_t reg_id;     ///< Sample state register ID
    uint8_t                num_states; ///< Number of sampling states for conversion. Ch16-20/21 use the same value.
} adc_sample_state_t;

/** Extended configuration structure for ADC. */
typedef struct st_adc_extended_cfg
{
    adc_add_t            add_average_count;           ///< Add or average samples
    adc_clear_t          clearing;                    ///< Clear after read
    adc_trigger_t        trigger_group_b;             ///< Group B trigger source; valid only for group mode
    adc_double_trigger_t double_trigger_mode;         ///< Double-trigger mode setting
    adc_active_trigger_t adc_start_trigger_a;         ///< A/D Conversion Start Trigger Group A
    adc_active_trigger_t adc_start_trigger_b;         ///< A/D Conversion Start Trigger Group B
    bool                 adc_start_trigger_c_enabled; ///< Set to true to enable Group C, false to disable Group C
    adc_active_trigger_t adc_start_trigger_c;         ///< A/D Conversion Start Trigger Group C
    adc_elc_t            adc_elc_ctrl;                ///< A/D Event Link Control
} adc_extended_cfg_t;

/** ADC channel(s) configuration       */
typedef struct st_adc_channel_cfg
{
    uint32_t      scan_mask;           ///< Channels/bits: bit 0 is ch0; bit 15 is ch15.
    uint32_t      scan_mask_group_b;   ///< Valid for group modes.
    uint32_t      scan_mask_group_c;   ///< Valid for group modes.
    uint32_t      add_mask;            ///< Valid if add enabled in Open().
    adc_group_a_t priority_group_a;    ///< Valid for group modes.
    uint8_t       sample_hold_mask;    ///< Channels/bits 0-2.
    uint8_t       sample_hold_states;  ///< Number of states to be used for sample and hold. Affects channels 0-2.
} adc_channel_cfg_t;

/* Sample and hold Channel mask. Sample and hold is only available for channel 0,1,2*/
#define ADC_SAMPLE_HOLD_CHANNELS    (0x07U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** ADC instance control block. DO NOT INITIALIZE.  Initialized in @ref adc_api_t::open(). */
typedef struct
{
    R_ADC121_Type   * p_reg;                    // Base register for this unit
    adc_cfg_t const * p_cfg;
    uint32_t          opened;                   // Boolean to verify that the Unit has been initialized
    uint32_t          scan_mask;                // Scan mask used for Normal scan.
    uint16_t          scan_start_adcsr;

    void (* p_callback)(adc_callback_args_t *); // Pointer to callback that is called when an adc_event_t occurs.
    adc_callback_args_t * p_callback_memory;    // Pointer to non-secure memory that can be used to pass arguments to a callback in non-secure memory.

    /* Pointer to context to be passed into callback function */
    void const * p_context;
} adc_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Interface Structure for user access */
extern const adc_api_t g_adc_on_adc;

/** @endcond */

/***********************************************************************************************************************
 * Public APIs
 **********************************************************************************************************************/
fsp_err_t R_ADC_Open(adc_ctrl_t * p_ctrl, adc_cfg_t const * const p_cfg);
fsp_err_t R_ADC_ScanCfg(adc_ctrl_t * p_ctrl, void const * const p_channel_cfg);
fsp_err_t R_ADC_InfoGet(adc_ctrl_t * p_ctrl, adc_info_t * p_adc_info);
fsp_err_t R_ADC_ScanStart(adc_ctrl_t * p_ctrl);
fsp_err_t R_ADC_ScanGroupStart(adc_ctrl_t * p_ctrl, adc_group_mask_t group_mask);
fsp_err_t R_ADC_ScanStop(adc_ctrl_t * p_ctrl);
fsp_err_t R_ADC_StatusGet(adc_ctrl_t * p_ctrl, adc_status_t * p_status);
fsp_err_t R_ADC_Read(adc_ctrl_t * p_ctrl, adc_channel_t const reg_id, uint16_t * const p_data);
fsp_err_t R_ADC_Read32(adc_ctrl_t * p_ctrl, adc_channel_t const reg_id, uint32_t * const p_data);
fsp_err_t R_ADC_SampleStateCountSet(adc_ctrl_t * p_ctrl, adc_sample_state_t * p_sample);
fsp_err_t R_ADC_Close(adc_ctrl_t * p_ctrl);
fsp_err_t R_ADC_OffsetSet(adc_ctrl_t * const p_ctrl, adc_channel_t const reg_id, int32_t offset);
fsp_err_t R_ADC_Calibrate(adc_ctrl_t * const p_ctrl, void const * p_extend);
fsp_err_t R_ADC_CallbackSet(adc_ctrl_t * const          p_ctrl,
                            void (                    * p_callback)(adc_callback_args_t *),
                            void const * const          p_context,
                            adc_callback_args_t * const p_callback_memory);

/*******************************************************************************************************************//**
 * @} (end defgroup ADC)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
