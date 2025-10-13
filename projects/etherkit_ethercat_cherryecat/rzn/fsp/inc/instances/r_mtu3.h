/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef R_MTU3_H
#define R_MTU3_H

/*******************************************************************************************************************//**
 * @addtogroup MTU3
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_timer_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** I/O Level Select */
typedef enum e_mtu3_io_pin_level
{
    /* Output */
    MTU3_IO_PIN_LEVEL_NO_OUTPUT                   = 0,    ///< Output prohibited
    MTU3_IO_PIN_LEVEL_INITIAL_LOW_COMPARE_LOW     = 1,    ///< Initial output is low. Low output at compare match.
    MTU3_IO_PIN_LEVEL_INITIAL_LOW_COMPARE_HIGH    = 2,    ///< Initial output is low. High output at compare match.
    MTU3_IO_PIN_LEVEL_INITIAL_LOW_COMPARE_TOGGLE  = 3,    ///< Initial output is low. Toggle output at compare match.
    MTU3_IO_PIN_LEVEL_INITIAL_HIGH_COMPARE_LOW    = 5,    ///< Initial output is high. Low output at compare match.
    MTU3_IO_PIN_LEVEL_INITIAL_HIGH_COMPARE_HIGH   = 6,    ///< Initial output is high. High output at compare match.
    MTU3_IO_PIN_LEVEL_INITIAL_HIGH_COMPARE_TOGGLE = 7,    ///< Initial output is high. Toggle output at compare match.

    /* Input(Common to A and B) */
    MTU3_IO_PIN_LEVEL_INPUT_RISING_EDGE  = 8,             ///< Input capture at rising edge.
    MTU3_IO_PIN_LEVEL_INPUT_FALLING_EDGE = 9,             ///< Input capture at falling edge.
    MTU3_IO_PIN_LEVEL_INPUT_BOTH_EDGE    = 10,            ///< Input capture at both edge.

    /* Input A(IOA) */
    MTU3_IO_PIN_LEVEL_INPUT_A_MTU1_TCNT_CH0         = 12, ///< Capture input source is the clock source for counting in MTU1. (ch0)
    MTU3_IO_PIN_LEVEL_INPUT_A_MTU0_TGRA_COMPARE_CH1 = 12, ///< Input capture at generation of MTU0.TGRA compare match/input capture. (ch1)

    /* Input B(IOB) */
    MTU3_IO_PIN_LEVEL_INPUT_B_MTU1_TCNT_CH0_8 = 12,       ///< Capture input source is the clock source for counting in MTU1. (ch0, 8)
} mtu3_io_pin_level_t;

/** I/O Level Select(MTU5) */
typedef enum e_mtu3_io_pin_level_uvw
{
    MTU3_IO_PIN_LEVEL_UVW_NO_FUNC                    = 0x0,  ///< No function
    MTU3_IO_PIN_LEVEL_UVW_INPUT_RISING_EDGE          = 0x11, ///< Input capture at rising edge.
    MTU3_IO_PIN_LEVEL_UVW_INPUT_FALLING_EDGE         = 0x12, ///< Input capture at falling edge.
    MTU3_IO_PIN_LEVEL_UVW_INPUT_BOTH_EDGE            = 0x13, ///< Input capture at both edges.
    MTU3_IO_PIN_LEVEL_UVW_INPUT_MTU8_TGRC_COMPARE    = 0x14, ///< Input capture on generation of compare match with MTU8.TGRC
    MTU3_IO_PIN_LEVEL_UVW_EXTERNAL_LOW_TROUGH        = 0x19, ///< Measurement of low pulse width of external input signal.Capture at trough in complementary PWM mode.
    MTU3_IO_PIN_LEVEL_UVW_EXTERNAL_LOW_CREST         = 0x1A, ///< Measurement of low pulse width of external input signal.Capture at crest of complementary PWM mode.
    MTU3_IO_PIN_LEVEL_UVW_EXTERNAL_LOW_CREST_TROUGH  = 0x1B, ///< Measurement of low pulse width of external input signal.Capture at crest and trough of complementary PWM mode.
    MTU3_IO_PIN_LEVEL_UVW_EXTERNAL_HIGH_TROUGH       = 0x1D, ///< Measurement of high pulse width of external input signal.Capture at trough in complementary PWM mode.
    MTU3_IO_PIN_LEVEL_UVW_EXTERNAL_HIGH_CREST        = 0x1E, ///< Measurement of high pulse width of external input signal.Capture at crest of complementary PWM mode.
    MTU3_IO_PIN_LEVEL_UVW_EXTERNAL_HIGH_CREST_TROUGH = 0x1F, ///< Measurement of high pulse width of external input signal.Capture at crest and trough of complementary PWM mode.
} mtu3_io_pin_level_uvw_t;

/** Clock Edge Select */
typedef enum e_mtu3_clock_edge
{
    MTU3_CLOCK_EDGE_RISING  = 0x0,     ///< Count at rising edge
    MTU3_CLOCK_EDGE_FALLING = 0x1,     ///< Count at falling edge
    MTU3_CLOCK_EDGE_BOTH    = 0x2,     ///< Count at both edges
} mtu3_clock_edge_t;

/** Time Prescaler Select */
typedef enum e_mtu3_div
{
    MTU3_DIV_PCLKH_1  = 0x0,                ///< PCLKH divided by 1  (common ch)
    MTU3_DIV_PCLKH_4  = 0x1,                ///< PCLKH divided by 4  (common ch)
    MTU3_DIV_PCLKH_16 = 0x2,                ///< PCLKH divided by 16 (common ch)
    MTU3_DIV_PCLKH_64 = 0x3,                ///< PCLKH divided by 64 (common ch)
    MTU3_DIV_PCLKH_2  = 0x8,                ///< PCLKH divided by 2 (common ch)
    MTU3_DIV_PCLKH_8  = 0x10,               ///< PCLKH divided by 8 (common ch)
    MTU3_DIV_PCLKH_32 = 0x18,               ///< PCLKH divided by 32 (common ch)

    MTU3_DIV_MTCLKA_CH_0     = 0x4,         ///< External clock: counts on MTCLKA pin input (ch0)
    MTU3_DIV_MTCLKB_CH_0     = 0x5,         ///< External clock: counts on MTCLKB pin input (ch0)
    MTU3_DIV_MTCLKC_CH_0     = 0x6,         ///< External clock: counts on MTCLKC pin input (ch0)
    MTU3_DIV_MTCLKD_CH_0     = 0x7,         ///< External clock: counts on MTCLKD pin input (ch0)
    MTU3_DIV_PCLKH_256_CH_0  = 0x20,        ///< PCLKH divided by 256 (ch0)
    MTU3_DIV_PCLKH_1024_CH_0 = 0x28,        ///< PCLKH divided by 1024 (ch0)
    MTU3_DIV_MTIOC1A_CH_0    = 0x38,        ///< External clock: counts on MTIOC1A pin input (ch0)

    MTU3_DIV_MTCLKA_CH_1     = 0x4,         ///< External clock: counts on MTCLKA pin input (ch1)
    MTU3_DIV_MTCLKB_CH_1     = 0x5,         ///< External clock: counts on MTCLKB pin input (ch1)
    MTU3_DIV_PCLKH_256_CH_1  = 0x6,         ///< PCLKH divided by 256 (ch1)
    MTU3_DIV_TCNT_CH1        = 0x7,         ///< Overflow/underflow of MTU2.TCNT
    MTU3_DIV_PCLKH_1024_CH_1 = 0x20,        ///< PCLKH divided by 1024 (ch1)

    MTU3_DIV_MTCLKA_CH_2     = 0x4,         ///< External clock: counts on MTCLKA pin input (ch2)
    MTU3_DIV_MTCLKB_CH_2     = 0x5,         ///< External clock: counts on MTCLKB pin input (ch2)
    MTU3_DIV_MTCLKC_CH_2     = 0x6,         ///< External clock: counts on MTCLKC pin input (ch2)
    MTU3_DIV_PCLKH_1024_CH_2 = 0x7,         ///< PCLKH divided by 1024 (ch2)
    MTU3_DIV_PCLKH_256_CH_2  = 0x20,        ///< PCLKH divided by 256 (ch2)

    MTU3_DIV_PCLKH_256_CH_3_4_6_7_8  = 0x4, ///< PCLKH divided by 256 (ch3-4, 6-8)
    MTU3_DIV_PCLKH_1024_CH_3_4_6_7_8 = 0x5, ///< PCLKH divided by 1024 (ch3-4, 6-8)
    MTU3_DIV_MTCLKA_CH_3_4_6_7_8     = 0x6, ///< External clock: counts on MTCLKA pin input (ch3-4, 6-8)
    MTU3_DIV_MTCLKB_CH_3_4_6_7_8     = 0x7, ///< External clock: counts on MTCLKB pin input (ch3-4, 6-8)
} mtu3_div_t;

/** Time Prescaler Select(MTU5) */
typedef enum e_mtu3_div_uvw
{
    MTU3_DIV_UVW_PCLKH_1              = 0x0,  ///< PCLKH divided by 1
    MTU3_DIV_UVW_PCLKH_4              = 0x1,  ///< PCLKH divided by 4
    MTU3_DIV_UVW_PCLKH_16             = 0x2,  ///< PCLKH divided by 16
    MTU3_DIV_UVW_PCLKH_64             = 0x3,  ///< PCLKH divided by 64
    MTU3_DIV_UVW_PCLKH_2              = 0x4,  ///< PCLKH divided by 2
    MTU3_DIV_UVW_PCLKH_8              = 0x8,  ///< PCLKH divided by 8
    MTU3_DIV_UVW_PCLKH_32             = 0xC,  ///< PCLKH divided by 32
    MTU3_DIV_UVW_PCLKH_256            = 0x10, ///< PCLKH divided by 256
    MTU3_DIV_UVW_PCLKH_1024           = 0x14, ///< PCLKH divided by 1024
    MTU3_DIV_UVW_MTIOC1A_RISING_EDGE  = 0x1C, ///< External clock: Counts at the rising edge on MTIOC1A pin input
    MTU3_DIV_UVW_MTIOC1A_FALLING_EDGE = 0x3C, ///< External clock: Counts at the falling edge on MTIOC1A pin input
    MTU3_DIV_UVW_MTIOC1A_BOTH_EDGE    = 0x5C, ///< External clock: Counts at the both edges on MTIOC1A pin input
} mtu3_div_uvw_t;

/** Counter Clear Source Select */
typedef enum e_mtu3_tcnt_clear
{
    MTU3_TCNT_CLEAR_DISABLE = 0x0,     ///< TCNT clearing disabled
    MTU3_TCNT_CLEAR_TGRA    = 0x1,     ///< TCNT cleared by TGRA compare match/input capture
    MTU3_TCNT_CLEAR_TGRB    = 0x2,     ///< TCNT cleared by TGRB compare match/input capture
} mtu3_tcnt_clear_t;

/** Level of MTU3 pin */
typedef enum e_mtu3_io_pin
{
    MTU3_IO_PIN_MTIOCA            = 0, ///< MTIOCA
    MTU3_IO_PIN_MTIOCB            = 1, ///< MTIOCB
    MTU3_IO_PIN_MTIOCA_AND_MTIOCB = 2, ///< MTIOCA and MTIOCB
} mtu3_io_pin_t;

/** MTU3 event group operation
 *  @note Event link must be configured by the ELC
 */
typedef enum e_mtu3_elc_operation_option
{
    MTU3_EVENT_OPERATION_SELECT_COUNT_START   = 0x0, ///< The MTU event operation is set to start counting
    MTU3_EVENT_OPERATION_SELECT_COUNT_RESTART = 0x1, ///< The MTU event operation is set to restart counting
    MTU3_EVENT_OPERATION_SELECT_INPUT_CAPTURE = 0x2, ///< The MTU event operation is set to input capture
    MTU3_EVENT_OPERATION_SELECT_DISABLE       = 0x3, ///< The MTU event operation is set to disable
} mtu3_elc_operation_option_t;

/** Configurations for output pins. */
typedef struct st_mtu3_output_pin
{
    mtu3_io_pin_level_t output_pin_level_a; ///< I/O Control A
    mtu3_io_pin_level_t output_pin_level_b; ///< I/O Control B
} mtu3_output_pin_t;

/** Disables or enables the noise filter for input from the MTIOCnA pin */
typedef enum e_mtu3_noise_filter
{
    MTU3_NOISE_FILTER_DISABLE  = 0U,   ///< The noise filter for the MTIOC pin is disabled
    MTU3_NOISE_FILTER_A_ENABLE = 1U,   ///< The noise filter for the MTIOCA pin is enabled
    MTU3_NOISE_FILTER_B_ENABLE = 2U,   ///< The noise filter for the MTIOCB pin is enabled
} mtu3_noise_filter_setting_t;

/** Disables or enables the noise filter for the external clock input pins of the MTU */
typedef enum e_mtu3_noise_filter_external
{
    MTU3_NOISE_FILTER_EXTERNAL_DISABLE  = 0U, ///< The noise filter for the MTCLK pin is disabled
    MTU3_NOISE_FILTER_EXTERNAL_A_ENABLE = 1U, ///< The noise filter for the MTCLKA pin is enabled.
    MTU3_NOISE_FILTER_EXTERNAL_B_ENABLE = 2U, ///< The noise filter for the MTCLKB pin is enabled.
    MTU3_NOISE_FILTER_EXTERNAL_C_ENABLE = 4U, ///< The noise filter for the MTCLKC pin is enabled.
    MTU3_NOISE_FILTER_EXTERNAL_D_ENABLE = 8U, ///< The noise filter for the MTCLKD pin is enabled.
} mtu3_noise_filter_mtclk_setting_t;

/** Disables or enables the noise filter for input from the MTIC5U, MTIC5V, MTIC5W pin */
typedef enum e_mtu3_noise_filter_uvw
{
    MTU3_NOISE_FILTER_UVW_DISABLE  = 0U, ///< The noise filter for the MTIOC pin is disabled
    MTU3_NOISE_FILTER_UVW_U_ENABLE = 1U, ///< The noise filter for the MTIC5U pin is enabled
    MTU3_NOISE_FILTER_UVW_V_ENABLE = 2U, ///< The noise filter for the MTIC5V pin is enabled
    MTU3_NOISE_FILTER_UVW_W_ENABLE = 4U, ///< The noise filter for the MTIC5W pin is enabled
} mtu3_noise_filter_setting_uvw_t;

typedef enum e_mtu3_noise_filter_clock
{
    MTU3_NOISE_FILTER_CLOCK_PCLKH_DIV_1  = 0U, ///< PCLK/1 - fast sampling
    MTU3_NOISE_FILTER_CLOCK_PCLKH_DIV_8  = 1U, ///< PCLK/8
    MTU3_NOISE_FILTER_CLOCK_PCLKH_DIV_32 = 2U, ///< PCLK/32 - slow sampling
    MTU3_NOISE_FILTER_CLOCK_SOURCE       = 3U, ///< Clock source for counting
} mtu3_noise_filter_clock_t;

typedef enum e_mtu3_noise_filter_external_clock
{
    MTU3_NOISE_FILTER_EXTERNAL_CLOCK_PCLKH_DIV_1  = 0U, ///< PCLK/1 - fast sampling
    MTU3_NOISE_FILTER_EXTERNAL_CLOCK_PCLKH_DIV_2  = 1U, ///< PCLK/2
    MTU3_NOISE_FILTER_EXTERNAL_CLOCK_PCLKH_DIV_8  = 2U, ///< PCLK/8
    MTU3_NOISE_FILTER_EXTERNAL_CLOCK_PCLKH_DIV_32 = 3U, ///< PCLK/32 - slow sampling
} mtu3_noise_filter_external_clock_t;

/** Interrupt Skipping Function Select */
typedef enum e_mtu3_interrupt_skip_mode
{
    MTU3_INTERRUPT_SKIP_MODE_1 = 0U,   ///< Setting the TITCR1A or TITCR1B register enables
    MTU3_INTERRUPT_SKIP_MODE_2 = 1U,   ///< Setting the TITCR2A or TITCR2B register enables
} mtu3_interrupt_skip_mode_t;

/** Number of interrupts to skip between events */
typedef enum e_mtu3_interrupt_skip_count
{
    MTU3_INTERRUPT_SKIP_COUNT_0 = 0U,  ///< Do not skip interrupts
    MTU3_INTERRUPT_SKIP_COUNT_1,       ///< Skip one interrupt
    MTU3_INTERRUPT_SKIP_COUNT_2,       ///< Skip two interrupts
    MTU3_INTERRUPT_SKIP_COUNT_3,       ///< Skip three interrupts
    MTU3_INTERRUPT_SKIP_COUNT_4,       ///< Skip four interrupts
    MTU3_INTERRUPT_SKIP_COUNT_5,       ///< Skip five interrupts
    MTU3_INTERRUPT_SKIP_COUNT_6,       ///< Skip six interrupts
    MTU3_INTERRUPT_SKIP_COUNT_7,       ///< Skip seven interrupts
} mtu3_interrupt_skip_count_t;

/** Trigger options to start A/D conversion. */
typedef enum e_mtu3_adc_compare_match
{
    MTU3_ADC_COMPARE_MATCH_ADC_A = 0x1U, ///< Set A/D conversion start request value for MTU3 A/D converter start request A
    MTU3_ADC_COMPARE_MATCH_ADC_B = 0x2U, ///< Set A/D conversion start request value for MTU3 A/D converter start request B
} mtu3_adc_compare_match_t;

/** Current timer status. */
typedef struct st_mtu3_status
{
    uint32_t counter;                  ///< Current counter value

    /* MTU3 ch5 specific parameters */
    uint32_t tgru_counter;             ///< Current counter value(TGRU)
    uint32_t tgrv_counter;             ///< Current counter value(TGRV)
    uint32_t tgrw_counter;             ///< Current counter value(TGRW)

    timer_state_t state;               ///< Current timer state (running or stopped)
} mtu3_status_t;

/** MTU3 configuration for event link function*/
typedef struct st_mtu3_elc_operation
{
    mtu3_elc_operation_option_t mtu0_elc_operation; ///< MTU0 operation select
    mtu3_elc_operation_option_t mtu3_elc_operation; ///< MTU3 operation select
    mtu3_elc_operation_option_t mtu4_elc_operation; ///< MTU4 operation select
} mtu3_elc_operation_t;

/** MTU3 set counter. */
typedef struct st_mtu3_counter
{
    uint32_t tgra_counts;              ///< Counter value(TGRA)
    uint32_t tgrb_counts;              ///< Counter value(TGRB)

    /* MTU3 ch5 specific parameters */
    uint32_t tgru_counts;              ///< Counter value(TGRU)
    uint32_t tgrv_counts;              ///< Counter value(TGRV)
    uint32_t tgrw_counts;              ///< Counter value(TGRW)
} mtu3_counter_t;

/** MTU3 information structure to store various information for a timer resource */
typedef struct st_mtu3_info
{
    timer_direction_t count_direction;    ///< Clock counting direction of the timer.
    uint32_t          clock_frequency[3]; ///< Clock frequency of the timer counter.(For ch5, use clock_frequency[0]-[2].)

    mtu3_counter_t count_param;
} mtu3_info_t;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when @ref timer_api_t::open is called. */
typedef struct st_mtu3_instance_ctrl
{
    uint32_t            open;                     ///< Whether or not channel is open
    const timer_cfg_t * p_cfg;                    ///< Pointer to initial configurations
    void              * p_reg;                    ///< Base register for this channel
    R_MTU_Type        * p_reg_com;                ///< Base register for this channel(common ch)
    void              * p_reg_nf;                 ///< Base register for this channel(noise fileter)
    uint32_t            channel_mask;             ///< Channel bitmask

    void (* p_callback)(timer_callback_args_t *); ///< Pointer to callback
    timer_callback_args_t * p_callback_memory;    ///< Pointer to optional callback argument memory
    void const            * p_context;            ///< Pointer to context to be passed into callback function
} mtu3_instance_ctrl_t;

/** MTU3 extension for advanced PWM features. */
typedef struct st_mtu3_extended_pwm_cfg
{
    mtu3_interrupt_skip_mode_t interrupt_skip_mode_a;           ///< Selects interrupt skipping function 1 or 2(TIMTRA)
    mtu3_interrupt_skip_mode_t interrupt_skip_mode_b;           ///< Selects interrupt skipping function 1 or 2(TIMTRB)
    uint16_t adc_a_compare_match;                               ///< Timer A/D Converter Start Request Cycle A (MTU4 or MTU7)
    uint16_t adc_b_compare_match;                               ///< Timer A/D Converter Start Request Cycle B (MTU4 or MTU7)
    mtu3_interrupt_skip_count_t interrupt_skip_count_tciv4;     ///< TCIV4 Interrupt Skipping Count Setting(TITCR1A)
    mtu3_interrupt_skip_count_t interrupt_skip_count_tgia3;     ///< TGIA3 Interrupt Skipping Count Setting(TITCR1A)
    mtu3_interrupt_skip_count_t interrupt_skip_count_tciv7;     ///< TCIV7 Interrupt Skipping Count Setting(TITCR1B)
    mtu3_interrupt_skip_count_t interrupt_skip_count_tgia6;     ///< TGIA6 Interrupt Skipping Count Setting(TITCR1B)
    mtu3_interrupt_skip_count_t interrupt_skip_count_tgr4an_bn; ///< TRG4AN/TRG4BN Interrupt Skipping Count Setting(TITCR2A)
    mtu3_interrupt_skip_count_t interrupt_skip_count_tgr7an_bn; ///< TRG7AN/TRG7BN Interrupt Skipping Count Setting(TITCR2B)
} mtu3_extended_pwm_cfg_t;

/** MTU3 extension for ch5. */
typedef struct st_mtu3_extended_uvw_cfg
{
    /* Used only with MTU5(TGRU5, TGRV5, TGRW5) */
    uint16_t tgru_val;                          ///< Capture/Compare match U register
    uint16_t tgrv_val;                          ///< Capture/Compare match V register
    uint16_t tgrw_val;                          ///< Capture/Compare match W register

    mtu3_div_uvw_t mtu3_clk_div_u;              ///< Time Prescaler Select U register
    mtu3_div_uvw_t mtu3_clk_div_v;              ///< Time Prescaler Select V register
    mtu3_div_uvw_t mtu3_clk_div_w;              ///< Time Prescaler Select W register

    mtu3_io_pin_level_uvw_t output_pin_level_u; ///< I/O Control U register
    mtu3_io_pin_level_uvw_t output_pin_level_v; ///< I/O Control V register
    mtu3_io_pin_level_uvw_t output_pin_level_w; ///< I/O Control W register

    /* Debounce filter for MTIC5U, MTIC5V or MTIC5W input signal pin. */
    mtu3_noise_filter_setting_uvw_t noise_filter_mtioc_setting_uvw;
    mtu3_noise_filter_clock_t       noise_filter_mtioc_clk_uvw;

    uint8_t   capture_u_ipl;                             ///< Capture/Compare match U interrupt priority
    uint8_t   capture_v_ipl;                             ///< Capture/Compare match V interrupt priority
    uint8_t   capture_w_ipl;                             ///< Capture/Compare match W interrupt priority
    IRQn_Type capture_u_irq;                             ///< Capture/Compare match U interrupt
    IRQn_Type capture_v_irq;                             ///< Capture/Compare match V interrupt
    IRQn_Type capture_w_irq;                             ///< Capture/Compare match W interrupt

    bsp_irq_gpt_selected_event_t capture_u_select_event; ///< Selected capture u event
    bsp_irq_gpt_selected_event_t capture_v_select_event; ///< Selected capture v event
    bsp_irq_gpt_selected_event_t capture_w_select_event; ///< Selected capture w event
} mtu3_extended_uvw_cfg_t;

typedef enum e_mtu3_phase_counting_mode
{
    MTU3_PHASE_COUNTING_MODE_NONE,     ///< Disable Counting Mode
    MTU3_PHASE_COUNTING_MODE_1,        ///< Mode 1
    MTU3_PHASE_COUNTING_MODE_200,      ///< Mode 2 (00)
    MTU3_PHASE_COUNTING_MODE_201,      ///< Mode 2 (01)
    MTU3_PHASE_COUNTING_MODE_210,      ///< Mode 2 (1x)
    MTU3_PHASE_COUNTING_MODE_300,      ///< Mode 3 (00)
    MTU3_PHASE_COUNTING_MODE_301,      ///< Mode 3 (01)
    MTU3_PHASE_COUNTING_MODE_310,      ///< Mode 3 (1x)
    MTU3_PHASE_COUNTING_MODE_4,        ///< Mode 4
    MTU3_PHASE_COUNTING_MODE_50,       ///< Mode 5 (0x)
    MTU3_PHASE_COUNTING_MODE_51,       ///< Mode 5 (10)
} mtu3_phase_counting_mode_t;

typedef enum e_mtu3_bit_mode
{
    MTU3_BIT_MODE_NORMAL_16BIT,        ///< normal mode(16bit mode)
    MTU3_BIT_MODE_NORMAL_32BIT,        ///< normal mode(32bit mode)
} mtu3_bit_mode_t;

typedef enum e_mtu3_external_clock
{
    MTU3_EXTERNAL_CLOCK_MTCLKA_B = 0x0, ///< MTCLKA, MTCLKB
    MTU3_EXTERNAL_CLOCK_MTCLKC_D = 0x1, ///< MTCLKC, MTCLKD
} mtu3_external_clock_t;

/** The MTU3 extension constitutes a unique feature of MTU3. */
typedef struct st_mtu3_extended_cfg
{
    uint32_t          tgra_val;           ///< Capture/Compare match A register
    uint32_t          tgrb_val;           ///< Capture/Compare match B register
    uint32_t          tgrc_val;           ///< Capture/Compare match C register (Does not exist in MTU ch1-2)
    uint32_t          tgrd_val;           ///< Capture/Compare match D register (Does not exist in MTU ch1-2)
    mtu3_div_t        mtu3_clk_div;       ///< Time Prescaler Select
    mtu3_clock_edge_t clk_edge;           ///< Clock Edge Select
    mtu3_tcnt_clear_t mtu3_clear;         ///< Counter Clear Source Select
    mtu3_output_pin_t mtioc_ctrl_setting; ///< I/O Control A, B

    /* Debounce filter for MTIOCxA or MTIOCxB input signal pin. */
    mtu3_noise_filter_setting_t noise_filter_mtioc_setting;
    mtu3_noise_filter_clock_t   noise_filter_mtioc_clk;

    /* Debounce filter for MTCLKx input signal pin. */
    mtu3_noise_filter_mtclk_setting_t  noise_filter_mtclk_setting;
    mtu3_noise_filter_external_clock_t noise_filter_mtclk_clk;

    bool adc_request_enable;                             ///< A/D Converter Start Request Enable(ch5 and ch8 are excluded)

    mtu3_elc_operation_t mtu3_elc_event_operation;       ///< MTU3 event link operation

    /* Used for other than MTU5 */

    uint8_t   capture_a_ipl;                             ///< Capture/Compare match A interrupt priority
    uint8_t   capture_b_ipl;                             ///< Capture/Compare match B interrupt priority
    IRQn_Type capture_a_irq;                             ///< Capture/Compare match A interrupt
    IRQn_Type capture_b_irq;                             ///< Capture/Compare match B interrupt

    mtu3_extended_uvw_cfg_t const * p_uvw_cfg;           ///< Advanced MTU ch5 features, optional
    mtu3_extended_pwm_cfg_t const * p_pwm_cfg;           ///< Advanced PWM features, optional
    mtu3_phase_counting_mode_t      counting_mode;       ///< Select the counting mode.
    mtu3_bit_mode_t                 bit_mode;            ///< Select bit mode
    mtu3_external_clock_t           external_clock;      ///< Select External Clock Input Pins

    uint8_t capture_a_source_select;                     ///< Identify the GPT_INT for capture a
    uint8_t capture_b_source_select;                     ///< Identify the GPT_INT for capture b
    uint8_t cycle_end_source_select;                     ///< Identify the GPT_INT for capture overflow/underflow

    bsp_irq_gpt_selected_event_t capture_a_select_event; ///< Selected capture a event
    bsp_irq_gpt_selected_event_t capture_b_select_event; ///< Selected capture b event
    bsp_irq_gpt_selected_event_t cycle_end_select_event; ///< Selected cycle end event
} mtu3_extended_cfg_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const timer_api_t g_timer_on_mtu3;

/** @endcond */

/***********************************************************************************************************************
 * Public APIs
 **********************************************************************************************************************/
fsp_err_t R_MTU3_Open(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg);
fsp_err_t R_MTU3_Stop(timer_ctrl_t * const p_ctrl);
fsp_err_t R_MTU3_Start(timer_ctrl_t * const p_ctrl);
fsp_err_t R_MTU3_Reset(timer_ctrl_t * const p_ctrl);
fsp_err_t R_MTU3_PeriodSet(timer_ctrl_t * const p_ctrl, mtu3_counter_t * const p_counter);
fsp_err_t R_MTU3_InfoGet(timer_ctrl_t * const p_ctrl, mtu3_info_t * const p_info);
fsp_err_t R_MTU3_StatusGet(timer_ctrl_t * const p_ctrl, mtu3_status_t * const p_status);
fsp_err_t R_MTU3_CounterSet(timer_ctrl_t * const p_ctrl, uint32_t counter);
fsp_err_t R_MTU3_OutputEnable(timer_ctrl_t * const p_ctrl, mtu3_output_pin_t pin_level);
fsp_err_t R_MTU3_OutputDisable(timer_ctrl_t * const p_ctrl, mtu3_io_pin_t pin);
fsp_err_t R_MTU3_AdcTriggerSet(timer_ctrl_t * const     p_ctrl,
                               mtu3_adc_compare_match_t which_compare_match,
                               uint16_t                 compare_match_value);
fsp_err_t R_MTU3_CallbackSet(timer_ctrl_t * const          p_ctrl,
                             void (                      * p_callback)(timer_callback_args_t *),
                             void const * const            p_context,
                             timer_callback_args_t * const p_callback_memory);
fsp_err_t R_MTU3_Close(timer_ctrl_t * const p_ctrl);

/*******************************************************************************************************************//**
 * @} (end defgroup MTU3)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
