/* generated HAL source file - do not edit */
#include "hal_data.h"

/* Macros to tie dynamic ELC links to ADC_TRIGGER_SYNC_ELC option in adc_trigger_t. */
#define ADC_TRIGGER_ADC0_A      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC0_B      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_A      ADC_TRIGGER_SYNC_ELC
#define ADC_TRIGGER_ADC1_B      ADC_TRIGGER_SYNC_ELC
#if defined(BSP_MCU_GROUP_RZT2M) || defined(BSP_MCU_GROUP_RZN2L)
#define ETHER_BUFFER_PLACE_IN_SECTION BSP_PLACE_IN_SECTION(".noncache_buffer.eth")
#else
#define ETHER_BUFFER_PLACE_IN_SECTION
#endif
gpt_instance_ctrl_t g_timer17_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer17_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT10_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT10_UDF,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      =  GPT_OUTPUT_DISABLE_NONE,
    .adc_trigger         =  GPT_ADC_TRIGGER_NONE,
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .interrupt_skip_source_ext1 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext1  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_source_ext2 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext2  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_func_ovf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_unf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_a  = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_b  = GPT_INTERRUPT_SKIP_SELECT_NONE,
};
#endif
const gpt_extended_cfg_t g_timer17_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
#if (0 == (0))
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
#else
    .count_up_source     = (gpt_source_t) ((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0x000FFFFU),
    .count_down_source   = (gpt_source_t) (((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0xFFFF0000U) >> 16),
#endif
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (12),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT10_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT10_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT10_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT10_CCMPB,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer17_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT10_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT10_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer17_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 1 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x5f5e100, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT1_3,
    .p_callback          = gpt17_timing_callback,
    .p_context           = NULL,
    .p_extend            = &g_timer17_extend,
    .cycle_end_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT10_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT10_OVF,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer17 =
{
    .p_ctrl        = &g_timer17_ctrl,
    .p_cfg         = &g_timer17_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t g_timer14_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer14_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT8_UDF,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      =  GPT_OUTPUT_DISABLE_NONE,
    .adc_trigger         =  GPT_ADC_TRIGGER_NONE,
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .interrupt_skip_source_ext1 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext1  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_source_ext2 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext2  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_func_ovf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_unf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_a  = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_b  = GPT_INTERRUPT_SKIP_SELECT_NONE,
};
#endif
const gpt_extended_cfg_t g_timer14_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
#if (0 == (0))
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
#else
    .count_up_source     = (gpt_source_t) ((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0x000FFFFU),
    .count_down_source   = (gpt_source_t) (((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0xFFFF0000U) >> 16),
#endif
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (12),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT8_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT8_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT8_CCMPB,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer14_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT8_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT8_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer14_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 1 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x5f5e100, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT1_1,
    .p_callback          = gpt14_timing_callback,
    .p_context           = NULL,
    .p_extend            = &g_timer14_extend,
    .cycle_end_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT8_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT8_OVF,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer14 =
{
    .p_ctrl        = &g_timer14_ctrl,
    .p_cfg         = &g_timer14_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t g_timer15_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer15_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT2_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT2_UDF,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      =  GPT_OUTPUT_DISABLE_NONE,
    .adc_trigger         =  GPT_ADC_TRIGGER_NONE,
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .interrupt_skip_source_ext1 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext1  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_source_ext2 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext2  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_func_ovf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_unf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_a  = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_b  = GPT_INTERRUPT_SKIP_SELECT_NONE,
};
#endif
const gpt_extended_cfg_t g_timer15_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
#if (0 == (0))
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
#else
    .count_up_source     = (gpt_source_t) ((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0x000FFFFU),
    .count_down_source   = (gpt_source_t) (((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0xFFFF0000U) >> 16),
#endif
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (12),
    .capture_b_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT2_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT2_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT2_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT2_CCMPB,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer15_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT2_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT2_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer15_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 1 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x17d78400, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT0_2,
    .p_callback          = gpt15_timing_callback,
    .p_context           = NULL,
    .p_extend            = &g_timer15_extend,
    .cycle_end_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT2_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT2_OVF,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer15 =
{
    .p_ctrl        = &g_timer15_ctrl,
    .p_cfg         = &g_timer15_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t g_timer6_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer6_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT1_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT1_UDF,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      =  GPT_OUTPUT_DISABLE_NONE,
    .adc_trigger         =  GPT_ADC_TRIGGER_NONE,
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .interrupt_skip_source_ext1 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext1  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_source_ext2 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext2  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_func_ovf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_unf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_a  = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_b  = GPT_INTERRUPT_SKIP_SELECT_NONE,
};
#endif
const gpt_extended_cfg_t g_timer6_extend =
{
    .gtioca = { .output_enabled = false,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
#if (0 == (0))
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
#else
    .count_up_source     = (gpt_source_t) ((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0x000FFFFU),
    .count_down_source   = (gpt_source_t) (((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0xFFFF0000U) >> 16),
#endif
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (12),
    .capture_b_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT1_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT1_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT1_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT1_CCMPB,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer6_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT1_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT1_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer6_cfg =
{
    .mode                = TIMER_MODE_ONE_SHOT,
    /* Actual period: 1 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x17d78400, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT0_1,
    .p_callback          = gpt6_timing_callback,
    .p_context           = NULL,
    .p_extend            = &g_timer6_extend,
    .cycle_end_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT1_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT1_OVF,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer6 =
{
    .p_ctrl        = &g_timer6_ctrl,
    .p_cfg         = &g_timer6_cfg,
    .p_api         = &g_timer_on_gpt
};
gpt_instance_ctrl_t g_timer5_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer5_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT3_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT3_UDF,
#else
    .trough_irq          = FSP_INVALID_VECTOR,
#endif
    .poeg_link           = GPT_POEG_LINK_POEG0,
    .output_disable      =  GPT_OUTPUT_DISABLE_NONE,
    .adc_trigger         =  GPT_ADC_TRIGGER_NONE,
    .dead_time_count_up  = 0,
    .dead_time_count_down = 0,
    .adc_a_compare_match = 0,
    .adc_b_compare_match = 0,
    .interrupt_skip_source = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_adc    = GPT_INTERRUPT_SKIP_ADC_NONE,
    .gtioca_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .gtiocb_disable_setting = GPT_GTIOC_DISABLE_PROHIBITED,
    .interrupt_skip_source_ext1 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext1  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_source_ext2 = GPT_INTERRUPT_SKIP_SOURCE_NONE,
    .interrupt_skip_count_ext2  = GPT_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_func_ovf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_unf    = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_a  = GPT_INTERRUPT_SKIP_SELECT_NONE,
    .interrupt_skip_func_adc_b  = GPT_INTERRUPT_SKIP_SELECT_NONE,
};
#endif
const gpt_extended_cfg_t g_timer5_extend =
{
    .gtioca = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = { .output_enabled = true,
                .stop_level     = GPT_PIN_LEVEL_LOW
              },
    .start_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
    .stop_source         = (gpt_source_t) ( GPT_SOURCE_NONE),
    .clear_source        = (gpt_source_t) ( GPT_SOURCE_NONE),
#if (0 == (0))
    .count_up_source     = (gpt_source_t) ( GPT_SOURCE_NONE),
    .count_down_source   = (gpt_source_t) ( GPT_SOURCE_NONE),
#else
    .count_up_source     = (gpt_source_t) ((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0x000FFFFU),
    .count_down_source   = (gpt_source_t) (((GPT_PHASE_COUNTING_MODE_1_UP | (GPT_PHASE_COUNTING_MODE_1_DN << 16)) & 0xFFFF0000U) >> 16),
#endif
    .capture_a_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_b_source    = (gpt_source_t) ( GPT_SOURCE_NONE),
    .capture_a_ipl       = (12),
    .capture_b_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT3_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT3_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT3_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT3_CCMPB,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer5_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT3_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT3_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer5_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 1 seconds. Actual duty: 0%. */ .period_counts = (uint32_t) 0x17d78400, .duty_cycle_counts = 0x0, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT0_3,
    .p_callback          = gpt5_timing_callback,
    .p_context           = NULL,
    .p_extend            = &g_timer5_extend,
    .cycle_end_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT3_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT3_OVF,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer5 =
{
    .p_ctrl        = &g_timer5_ctrl,
    .p_cfg         = &g_timer5_cfg,
    .p_api         = &g_timer_on_gpt
};
/* Nominal and Data bit timing configuration */

can_bit_timing_cfg_t g_canfd1_bit_timing_cfg =
{
    /* Actual bitrate: 500000 Hz. Actual sample point: 75 %. */
    .baud_rate_prescaler = 1,
    .time_segment_1 = 59,
    .time_segment_2 = 20,
    .synchronization_jump_width = 4
};

can_bit_timing_cfg_t g_canfd1_data_timing_cfg =
{
    /* Actual bitrate: 2000000 Hz. Actual sample point: 75 %. */
    .baud_rate_prescaler = 1,
    .time_segment_1 = 14,
    .time_segment_2 = 5,
    .synchronization_jump_width = 1
};


extern const canfd_afl_entry_t p_canfd1_afl[CANFD_CFG_AFL_CH1_RULE_NUM];

#ifndef CANFD_PRV_GLOBAL_CFG
#define CANFD_PRV_GLOBAL_CFG
canfd_global_cfg_t g_canfd_global_cfg =
{
    .global_interrupts = CANFD_CFG_GLOBAL_ERR_SOURCES,
    .global_config     = (CANFD_CFG_TX_PRIORITY | CANFD_CFG_DLC_CHECK | CANFD_CFD_CLOCK_SOURCE | CANFD_CFG_FD_OVERFLOW),
    .rx_mb_config      = (CANFD_CFG_RXMB_NUMBER | (CANFD_CFG_RXMB_SIZE << R_CANFD_CFDRMNB_RMPLS_Pos)),
    .global_err_ipl = CANFD_CFG_GLOBAL_ERR_IPL,
    .rx_fifo_ipl    = CANFD_CFG_RX_FIFO_IPL,
    .rx_fifo_config    =
    {
        ((CANFD_CFG_RXFIFO0_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO0_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO0_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO0_INT_MODE) | (CANFD_CFG_RXFIFO0_ENABLE)),
        ((CANFD_CFG_RXFIFO1_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO1_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO1_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO1_INT_MODE) | (CANFD_CFG_RXFIFO1_ENABLE)),
        ((CANFD_CFG_RXFIFO2_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO2_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO2_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO2_INT_MODE) | (CANFD_CFG_RXFIFO2_ENABLE)),
        ((CANFD_CFG_RXFIFO3_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO3_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO3_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO3_INT_MODE) | (CANFD_CFG_RXFIFO3_ENABLE)),
        ((CANFD_CFG_RXFIFO4_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO4_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO4_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO4_INT_MODE) | (CANFD_CFG_RXFIFO4_ENABLE)),
        ((CANFD_CFG_RXFIFO5_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO5_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO5_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO5_INT_MODE) | (CANFD_CFG_RXFIFO5_ENABLE)),
        ((CANFD_CFG_RXFIFO6_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO6_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO6_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO6_INT_MODE) | (CANFD_CFG_RXFIFO6_ENABLE)),
        ((CANFD_CFG_RXFIFO7_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO7_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO7_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO7_INT_MODE) | (CANFD_CFG_RXFIFO7_ENABLE)),
    },
};
#endif

canfd_extended_cfg_t g_canfd1_extended_cfg =
{
    .p_afl              = p_canfd1_afl,
    .txmb_txi_enable    = ((1ULL << 0) | (1ULL << 1) |  0ULL),
    .error_interrupts   = ( 0U),
    .p_data_timing      = &g_canfd1_data_timing_cfg,
    .delay_compensation = (1),
    .p_global_cfg       = &g_canfd_global_cfg,
};

canfd_instance_ctrl_t g_canfd1_ctrl;
const can_cfg_t g_canfd1_cfg =
{
    .channel                = 1,
    .p_bit_timing           = &g_canfd1_bit_timing_cfg,
    .p_callback             = canfd1_callback,
    .p_extend               = &g_canfd1_extended_cfg,
    .p_context              = NULL,
    .ipl                    = (12),
#if defined(VECTOR_NUMBER_CAN1_TX)
    .tx_irq             = VECTOR_NUMBER_CAN1_TX,
#else
    .tx_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_CAN1_CHERR)
    .error_irq             = VECTOR_NUMBER_CAN1_CHERR,
#else
    .error_irq             = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const can_instance_t g_canfd1 =
{
    .p_ctrl        = &g_canfd1_ctrl,
    .p_cfg         = &g_canfd1_cfg,
    .p_api         = &g_canfd_on_canfd
};
rtc_instance_ctrl_t g_rtc_ctrl;
const rtc_cfg_t g_rtc_cfg =
{
    .clock_source            = RTC_CLOCK_SOURCE_MAINCLK,
    .freq_compare_value      = 195311,
    .p_err_cfg               = NULL,
    .p_callback              = rtc_callback,
    .p_context               = NULL,
    .p_extend                = NULL,
    .alarm_ipl               = (12),
    .periodic_ipl            = (12),
    .carry_ipl               = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_RTC_ALM)
    .alarm_irq               = VECTOR_NUMBER_RTC_ALM,
#else
    .alarm_irq               = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_RTC_PRD)
    .periodic_irq            = VECTOR_NUMBER_RTC_PRD,
#else
    .periodic_irq            = FSP_INVALID_VECTOR,
#endif
    .carry_irq               = FSP_INVALID_VECTOR,
};
/* Instance structure to use this module. */
const rtc_instance_t g_rtc =
{
    .p_ctrl        = &g_rtc_ctrl,
    .p_cfg         = &g_rtc_cfg,
    .p_api         = &g_rtc_on_rtc
};
sci_uart_instance_ctrl_t     g_uart5_ctrl;

            #define FSP_NOT_DEFINED (1)
            #if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

            /* If the transfer module is DMAC, define a DMAC transfer callback. */
            extern void sci_uart_tx_dmac_callback(sci_uart_instance_ctrl_t * p_instance_ctrl);

            void g_uart5_tx_transfer_callback (transfer_callback_args_t * p_args)
            {
                FSP_PARAMETER_NOT_USED(p_args);
                sci_uart_tx_dmac_callback(&g_uart5_ctrl);
            }
            #endif

            #if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

            /* If the transfer module is DMAC, define a DMAC transfer callback. */
            extern void sci_uart_rx_dmac_callback(sci_uart_instance_ctrl_t * p_instance_ctrl);

            void g_uart5_rx_transfer_callback (transfer_callback_args_t * p_args)
            {
                FSP_PARAMETER_NOT_USED(p_args);
                sci_uart_rx_dmac_callback(&g_uart5_ctrl);
            }
            #endif
            #undef FSP_NOT_DEFINED

            sci_baud_setting_t           g_uart5_baud_setting =
            {
                /* Baud rate calculated with 0.160% error. */ .baudrate_bits_b.abcse = 0, .baudrate_bits_b.abcs = 0, .baudrate_bits_b.bgdm = 1, .baudrate_bits_b.cks = 1, .baudrate_bits_b.brr = 155, .baudrate_bits_b.mddr = (uint8_t) 256, .baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t g_uart5_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &g_uart5_baud_setting,
#if 1
                .clock_source           = SCI_UART_CLOCK_SOURCE_SCI5ASYNCCLK,
#else
                .clock_source           = SCI_UART_CLOCK_SOURCE_PCLKM,
#endif
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting          = {
                    .enable = SCI_UART_RS485_ENABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                    .assertion_time = 1,
                    .negation_time = 1,
                },
            };

            /** UART interface configuration */
            const uart_cfg_t g_uart5_cfg =
            {
                .channel             = 5,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = rs485_callback,
                .p_context           = NULL,
                .p_extend            = &g_uart5_cfg_extend,
                .p_transfer_tx       = g_uart5_P_TRANSFER_TX,
                .p_transfer_rx       = g_uart5_P_TRANSFER_RX,
                .rxi_ipl             = (12),
                .txi_ipl             = (12),
                .tei_ipl             = (12),
                .eri_ipl             = (12),
#if defined(VECTOR_NUMBER_SCI5_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI5_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI5_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI5_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI5_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI5_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI5_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI5_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t g_uart5 =
{
    .p_ctrl        = &g_uart5_ctrl,
    .p_cfg         = &g_uart5_cfg,
    .p_api         = &g_uart_on_sci
};
/* Nominal and Data bit timing configuration */

can_bit_timing_cfg_t g_canfd0_bit_timing_cfg =
{
    /* Actual bitrate: 500000 Hz. Actual sample point: 75 %. */
    .baud_rate_prescaler = 1,
    .time_segment_1 = 59,
    .time_segment_2 = 20,
    .synchronization_jump_width = 4
};

can_bit_timing_cfg_t g_canfd0_data_timing_cfg =
{
    /* Actual bitrate: 2000000 Hz. Actual sample point: 75 %. */
    .baud_rate_prescaler = 1,
    .time_segment_1 = 14,
    .time_segment_2 = 5,
    .synchronization_jump_width = 1
};


extern const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM];

#ifndef CANFD_PRV_GLOBAL_CFG
#define CANFD_PRV_GLOBAL_CFG
canfd_global_cfg_t g_canfd_global_cfg =
{
    .global_interrupts = CANFD_CFG_GLOBAL_ERR_SOURCES,
    .global_config     = (CANFD_CFG_TX_PRIORITY | CANFD_CFG_DLC_CHECK | CANFD_CFD_CLOCK_SOURCE | CANFD_CFG_FD_OVERFLOW),
    .rx_mb_config      = (CANFD_CFG_RXMB_NUMBER | (CANFD_CFG_RXMB_SIZE << R_CANFD_CFDRMNB_RMPLS_Pos)),
    .global_err_ipl = CANFD_CFG_GLOBAL_ERR_IPL,
    .rx_fifo_ipl    = CANFD_CFG_RX_FIFO_IPL,
    .rx_fifo_config    =
    {
        ((CANFD_CFG_RXFIFO0_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO0_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO0_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO0_INT_MODE) | (CANFD_CFG_RXFIFO0_ENABLE)),
        ((CANFD_CFG_RXFIFO1_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO1_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO1_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO1_INT_MODE) | (CANFD_CFG_RXFIFO1_ENABLE)),
        ((CANFD_CFG_RXFIFO2_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO2_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO2_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO2_INT_MODE) | (CANFD_CFG_RXFIFO2_ENABLE)),
        ((CANFD_CFG_RXFIFO3_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO3_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO3_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO3_INT_MODE) | (CANFD_CFG_RXFIFO3_ENABLE)),
        ((CANFD_CFG_RXFIFO4_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO4_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO4_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO4_INT_MODE) | (CANFD_CFG_RXFIFO4_ENABLE)),
        ((CANFD_CFG_RXFIFO5_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO5_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO5_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO5_INT_MODE) | (CANFD_CFG_RXFIFO5_ENABLE)),
        ((CANFD_CFG_RXFIFO6_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO6_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO6_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO6_INT_MODE) | (CANFD_CFG_RXFIFO6_ENABLE)),
        ((CANFD_CFG_RXFIFO7_INT_THRESHOLD << R_CANFD_CFDRFCC_RFIGCV_Pos) | (CANFD_CFG_RXFIFO7_DEPTH << R_CANFD_CFDRFCC_RFDC_Pos) | (CANFD_CFG_RXFIFO7_PAYLOAD << R_CANFD_CFDRFCC_RFPLS_Pos) | (CANFD_CFG_RXFIFO7_INT_MODE) | (CANFD_CFG_RXFIFO7_ENABLE)),
    },
};
#endif

canfd_extended_cfg_t g_canfd0_extended_cfg =
{
    .p_afl              = p_canfd0_afl,
    .txmb_txi_enable    = ((1ULL << 0) |  0ULL),
    .error_interrupts   = ( 0U),
    .p_data_timing      = &g_canfd0_data_timing_cfg,
    .delay_compensation = (1),
    .p_global_cfg       = &g_canfd_global_cfg,
};

canfd_instance_ctrl_t g_canfd0_ctrl;
const can_cfg_t g_canfd0_cfg =
{
    .channel                = 0,
    .p_bit_timing           = &g_canfd0_bit_timing_cfg,
    .p_callback             = canfd0_callback,
    .p_extend               = &g_canfd0_extended_cfg,
    .p_context              = NULL,
    .ipl                    = (12),
#if defined(VECTOR_NUMBER_CAN0_TX)
    .tx_irq             = VECTOR_NUMBER_CAN0_TX,
#else
    .tx_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_CAN0_CHERR)
    .error_irq             = VECTOR_NUMBER_CAN0_CHERR,
#else
    .error_irq             = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const can_instance_t g_canfd0 =
{
    .p_ctrl        = &g_canfd0_ctrl,
    .p_cfg         = &g_canfd0_cfg,
    .p_api         = &g_canfd_on_canfd
};
iic_master_instance_ctrl_t g_i2c_master0_ctrl;

#define FSP_NOT_DEFINED (1)
#if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
extern void iic_master_tx_dmac_callback(iic_master_instance_ctrl_t * p_instance_ctrl);

void g_i2c_master0_tx_transfer_callback (transfer_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    iic_master_tx_dmac_callback(&g_i2c_master0_ctrl);
}
#endif

#if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
extern void iic_master_rx_dmac_callback(iic_master_instance_ctrl_t * p_instance_ctrl);

void g_i2c_master0_rx_transfer_callback (transfer_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    iic_master_rx_dmac_callback(&g_i2c_master0_ctrl);
}
#endif
#undef FSP_NOT_DEFINED

const iic_master_extended_cfg_t g_i2c_master0_extend =
{
    .timeout_mode             = IIC_MASTER_TIMEOUT_MODE_SHORT,
    .timeout_scl_low          = IIC_MASTER_TIMEOUT_SCL_LOW_ENABLED,
    /* Actual calculated bitrate: 98425. Actual calculated duty cycle: 50%. */ .clock_settings.brl_value = 28, .clock_settings.brh_value = 28, .clock_settings.cks_value = 3
};
const i2c_master_cfg_t g_i2c_master0_cfg =
{
    .channel             = 0,
    .rate                = I2C_MASTER_RATE_STANDARD,
    .slave               = 0x00,
    .addr_mode           = I2C_MASTER_ADDR_MODE_7BIT,
    .p_transfer_tx       = g_i2c_master0_P_TRANSFER_TX,
    .p_transfer_rx       = g_i2c_master0_P_TRANSFER_RX,
    .p_callback          = NULL,
    .p_context           = NULL,
#if defined(VECTOR_NUMBER_IIC0_RXI)
    .rxi_irq             = VECTOR_NUMBER_IIC0_RXI,
#else
    .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC0_TXI)
    .txi_irq             = VECTOR_NUMBER_IIC0_TXI,
#else
    .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC0_TEI)
    .tei_irq             = VECTOR_NUMBER_IIC0_TEI,
#else
    .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC0_EEI)
    .eri_irq             = VECTOR_NUMBER_IIC0_EEI,
#else
    .eri_irq             = FSP_INVALID_VECTOR,
#endif
    .ipl                 = (12),
    .p_extend            = &g_i2c_master0_extend,
};
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c_master0 =
{
    .p_ctrl        = &g_i2c_master0_ctrl,
    .p_cfg         = &g_i2c_master0_cfg,
    .p_api         = &g_i2c_master_on_iic
};
adc_instance_ctrl_t g_adc1_ctrl;
const adc_extended_cfg_t g_adc1_cfg_extend =
{
    .add_average_count   = ADC_ADD_OFF,
    .clearing            = ADC_CLEAR_AFTER_READ_ON,
    .trigger_group_b     = ADC_TRIGGER_SYNC_ELC,
    .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
    .adc_start_trigger_a  = ADC_ACTIVE_TRIGGER_DISABLED,
    .adc_start_trigger_b  = ADC_ACTIVE_TRIGGER_DISABLED,
    .adc_start_trigger_c_enabled = 0,
    .adc_start_trigger_c  = ADC_ACTIVE_TRIGGER_DISABLED,
    .adc_elc_ctrl        = ADC_ELC_SINGLE_SCAN,
};
const adc_cfg_t g_adc1_cfg =
{
    .unit                = 1,
    .mode                = ADC_MODE_SINGLE_SCAN,
    .resolution          = ADC_RESOLUTION_12_BIT,
    .alignment           = (adc_alignment_t)ADC_ALIGNMENT_RIGHT,
    .trigger             = ADC_TRIGGER_SOFTWARE,
    .p_callback          = NULL,
    .p_context           = NULL,
    .p_extend            = &g_adc1_cfg_extend,
#if (1 == BSP_FEATURE_ADC_REGISTER_MASK_TYPE)
#if defined(VECTOR_NUMBER_ADC1_ADI)
    .scan_end_irq        = VECTOR_NUMBER_ADC1_ADI,
#else
    .scan_end_irq        = FSP_INVALID_VECTOR,
#endif
    .scan_end_ipl        = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC1_GBADI)
    .scan_end_b_irq      = VECTOR_NUMBER_ADC1_GBADI,
#else
    .scan_end_b_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_b_ipl      = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC1_GCADI)
    .scan_end_c_irq      = VECTOR_NUMBER_ADC1_GCADI,
#else
    .scan_end_c_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_c_ipl      = (BSP_IRQ_DISABLED),
#endif
#if (3 == BSP_FEATURE_ADC_REGISTER_MASK_TYPE)
#if defined(VECTOR_NUMBER_ADC121_ADI)
    .scan_end_irq        = VECTOR_NUMBER_ADC121_ADI,
#else
    .scan_end_irq        = FSP_INVALID_VECTOR,
#endif
    .scan_end_ipl        = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC121_GBADI)
    .scan_end_b_irq      = VECTOR_NUMBER_ADC121_GBADI,
#else
    .scan_end_b_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_b_ipl      = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC121_GCADI)
    .scan_end_c_irq      = VECTOR_NUMBER_ADC121_GCADI,
#else
    .scan_end_c_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_c_ipl      = (BSP_IRQ_DISABLED),
#endif
};
const adc_channel_cfg_t g_adc1_channel_cfg =
{
    .scan_mask           = ADC_MASK_CHANNEL_0 | ADC_MASK_CHANNEL_1 | ADC_MASK_CHANNEL_2 | ADC_MASK_CHANNEL_3 |  0,
    .scan_mask_group_b   =  0,
    .priority_group_a    = ADC_GROUP_A_PRIORITY_OFF,
    .add_mask            =  0,
    .sample_hold_mask    =  0,
    .sample_hold_states  = 24,
    .scan_mask_group_c   =  0,
};
/* Instance structure to use this module. */
const adc_instance_t g_adc1 =
{
    .p_ctrl    = &g_adc1_ctrl,
    .p_cfg     = &g_adc1_cfg,
    .p_channel_cfg = &g_adc1_channel_cfg,
    .p_api     = &g_adc_on_adc
};
adc_instance_ctrl_t g_adc0_ctrl;
const adc_extended_cfg_t g_adc0_cfg_extend =
{
    .add_average_count   = ADC_ADD_OFF,
    .clearing            = ADC_CLEAR_AFTER_READ_ON,
    .trigger_group_b     = ADC_TRIGGER_SYNC_ELC,
    .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
    .adc_start_trigger_a  = ADC_ACTIVE_TRIGGER_DISABLED,
    .adc_start_trigger_b  = ADC_ACTIVE_TRIGGER_DISABLED,
    .adc_start_trigger_c_enabled = 0,
    .adc_start_trigger_c  = ADC_ACTIVE_TRIGGER_DISABLED,
    .adc_elc_ctrl        = ADC_ELC_SINGLE_SCAN,
};
const adc_cfg_t g_adc0_cfg =
{
    .unit                = 0,
    .mode                = ADC_MODE_SINGLE_SCAN,
    .resolution          = ADC_RESOLUTION_12_BIT,
    .alignment           = (adc_alignment_t)ADC_ALIGNMENT_RIGHT,
    .trigger             = ADC_TRIGGER_SOFTWARE,
    .p_callback          = NULL,
    .p_context           = NULL,
    .p_extend            = &g_adc0_cfg_extend,
#if (1 == BSP_FEATURE_ADC_REGISTER_MASK_TYPE)
#if defined(VECTOR_NUMBER_ADC0_ADI)
    .scan_end_irq        = VECTOR_NUMBER_ADC0_ADI,
#else
    .scan_end_irq        = FSP_INVALID_VECTOR,
#endif
    .scan_end_ipl        = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC0_GBADI)
    .scan_end_b_irq      = VECTOR_NUMBER_ADC0_GBADI,
#else
    .scan_end_b_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_b_ipl      = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC0_GCADI)
    .scan_end_c_irq      = VECTOR_NUMBER_ADC0_GCADI,
#else
    .scan_end_c_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_c_ipl      = (BSP_IRQ_DISABLED),
#endif
#if (3 == BSP_FEATURE_ADC_REGISTER_MASK_TYPE)
#if defined(VECTOR_NUMBER_ADC120_ADI)
    .scan_end_irq        = VECTOR_NUMBER_ADC120_ADI,
#else
    .scan_end_irq        = FSP_INVALID_VECTOR,
#endif
    .scan_end_ipl        = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC120_GBADI)
    .scan_end_b_irq      = VECTOR_NUMBER_ADC120_GBADI,
#else
    .scan_end_b_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_b_ipl      = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_ADC120_GCADI)
    .scan_end_c_irq      = VECTOR_NUMBER_ADC120_GCADI,
#else
    .scan_end_c_irq      = FSP_INVALID_VECTOR,
#endif
    .scan_end_c_ipl      = (BSP_IRQ_DISABLED),
#endif
};
const adc_channel_cfg_t g_adc0_channel_cfg =
{
    .scan_mask           = ADC_MASK_CHANNEL_0 | ADC_MASK_CHANNEL_1 | ADC_MASK_CHANNEL_2 | ADC_MASK_CHANNEL_3 |  0,
    .scan_mask_group_b   =  0,
    .priority_group_a    = ADC_GROUP_A_PRIORITY_OFF,
    .add_mask            =  0,
    .sample_hold_mask    =  0,
    .sample_hold_states  = 24,
    .scan_mask_group_c   =  0,
};
/* Instance structure to use this module. */
const adc_instance_t g_adc0 =
{
    .p_ctrl    = &g_adc0_ctrl,
    .p_cfg     = &g_adc0_cfg,
    .p_channel_cfg = &g_adc0_channel_cfg,
    .p_api     = &g_adc_on_adc
};
xspi_hyper_instance_ctrl_t g_hyperbus0_ctrl;

static xspi_hyper_cs_timing_setting_t g_hyperbus0_cs_timing_settings =
{
	.transaction_interval = XSPI_HYPER_TRANSACTION_INTERVAL_CLOCKS_8,
	.cs_pullup_lag        = XSPI_HYPER_CS_PULLUP_CLOCKS_NO_EXTENSION,
	.cs_pulldown_lead     = XSPI_HYPER_CS_PULLDOWN_CLOCKS_NO_EXTENSION,
};

static xspi_hyper_extended_cfg_t g_hyperbus0_extended_cfg =
{
    .unit                                    = 0,
    .chip_select                             = XSPI_HYPER_CHIP_SELECT_1,
    .memory_size                             = XSPI_HYPER_MEMORY_SIZE_32MB,
    .data_latching_delay_clock               = 0x08,
    .p_cs_timing_settings                    = &g_hyperbus0_cs_timing_settings,
    .p_autocalibration_preamble_pattern_addr = 0x00,
#if 0 == 0
    .prefetch_en                             = (xspi_hyper_prefetch_function_t) XSPI_HYPER_CFG_UNIT_0_PREFETCH_FUNCTION,
#else
    .prefetch_en                             = (xspi_hyper_prefetch_function_t) XSPI_HYPER_CFG_UNIT_1_PREFETCH_FUNCTION,
#endif
};
const hyperbus_cfg_t g_hyperbus0_cfg =
{
    .burst_type                   = HYPERBUS_BURST_TYPE_LINEAR,
    .access_space                 = HYPERBUS_SPACE_SELECT_MEMORY_SPACE,
    .read_latency_count           = HYPERBUS_LATENCY_COUNT_7,
    .memory_write_latency_count   = HYPERBUS_LATENCY_COUNT_7,
    .register_write_latency_count = HYPERBUS_LATENCY_COUNT_0,
    .p_extend                     = &g_hyperbus0_extended_cfg,
};
/** This structure encompasses everything that is needed to use an instance of this interface. */
const hyperbus_instance_t g_hyperbus0 =
{
    .p_ctrl = &g_hyperbus0_ctrl,
    .p_cfg  = &g_hyperbus0_cfg,
    .p_api  = &g_hyperbus_on_xspi_hyper,
};
ethsw_instance_ctrl_t g_ethsw0_ctrl;

const ethsw_extend_cfg_t g_ethsw0_extend_cfg =
            {
    .specific_tag    = ETHSW_SPECIFIC_TAG_DISABLE,
    .specific_tag_id = 0xE001,
    .phylink         = ETHSW_PHYLINK_DISABLE,

};

const ether_switch_cfg_t g_ethsw0_cfg =
{
    .channel    = 0,

#if defined(VECTOR_NUMBER_ETHSW_INTR)
    .irq        = VECTOR_NUMBER_ETHSW_INTR,
#else
    .irq        = FSP_INVALID_VECTOR,
#endif
    .ipl        = (12),
    .p_callback = gmac_callback_ethsw,
    .p_context  = &g_ether0_ctrl,
    .p_extend   = &g_ethsw0_extend_cfg
};

/* Instance structure to use this module. */
const ether_switch_instance_t g_ethsw0 =
{
    .p_ctrl        = &g_ethsw0_ctrl,
    .p_cfg         = &g_ethsw0_cfg,
    .p_api         = &g_ether_switch_on_ethsw
};
ether_selector_instance_ctrl_t g_ether_selector2_ctrl;

const ether_selector_cfg_t g_ether_selector2_cfg =
{
    .channel                   = 2,
    .phylink                   = ETHER_SELECTOR_PHYLINK_POLARITY_LOW,
    .interface                 = ETHER_SELECTOR_INTERFACE_RGMII,
    .speed                     = ETHER_SELECTOR_SPEED_100_MBPS,
    .duplex                    = ETHER_SELECTOR_DUPLEX_FULL,
    .ref_clock                 = ETHER_SELECTOR_REF_CLOCK_INPUT,
    .p_extend                  = NULL,
};

/* Instance structure to use this module. */
const ether_selector_instance_t g_ether_selector2 =
{
    .p_ctrl        = &g_ether_selector2_ctrl,
    .p_cfg         = &g_ether_selector2_cfg,
    .p_api         = &g_ether_selector_on_ether_selector
};
ether_phy_instance_ctrl_t g_ether_phy2_ctrl;

const ether_phy_extend_cfg_t g_ether_phy2_extend =
{
    .port_type           = ETHER_PHY_PORT_TYPE_ETHERNET,
    .mdio_type           = ETHER_PHY_MDIO_GMAC,
    .bps                 = ETHER_PHY_SPEED_100,
    .duplex              = ETHER_PHY_DUPLEX_FULL,
    .auto_negotiation    = ETHER_PHY_AUTO_NEGOTIATION_ON,
    .phy_reset_pin       = BSP_IO_PORT_13_PIN_4,
    .phy_reset_time      = 15000,
    .p_selector_instance = (ether_selector_instance_t *)&g_ether_selector2,
    .p_target_init       = ether_phy_targets_initialize_rtl8211_rgmii,
};

const ether_phy_cfg_t g_ether_phy2_cfg =
{

    .channel                   = 2,
    .phy_lsi_address           = 3,
    .phy_reset_wait_time       = 0x00020000,
    .mii_bit_access_wait_time  = 0,                         // Unused
    .phy_lsi_type              = ETHER_PHY_LSI_TYPE_CUSTOM,
    .flow_control              = ETHER_PHY_FLOW_CONTROL_DISABLE,
    .mii_type                  = (ether_phy_mii_type_t) 0,  // Unused
    .p_context                 = NULL,
    .p_extend                  = &g_ether_phy2_extend
};

/* Instance structure to use this module. */
const ether_phy_instance_t g_ether_phy2 =
{
    .p_ctrl        = &g_ether_phy2_ctrl,
    .p_cfg         = &g_ether_phy2_cfg,
    .p_api         = &g_ether_phy_on_ether_phy
};
ether_selector_instance_ctrl_t g_ether_selector1_ctrl;

const ether_selector_cfg_t g_ether_selector1_cfg =
{
    .channel                   = 1,
    .phylink                   = ETHER_SELECTOR_PHYLINK_POLARITY_LOW,
    .interface                 = ETHER_SELECTOR_INTERFACE_RGMII,
    .speed                     = ETHER_SELECTOR_SPEED_100_MBPS,
    .duplex                    = ETHER_SELECTOR_DUPLEX_FULL,
    .ref_clock                 = ETHER_SELECTOR_REF_CLOCK_INPUT,
    .p_extend                  = NULL,
};

/* Instance structure to use this module. */
const ether_selector_instance_t g_ether_selector1 =
{
    .p_ctrl        = &g_ether_selector1_ctrl,
    .p_cfg         = &g_ether_selector1_cfg,
    .p_api         = &g_ether_selector_on_ether_selector
};
ether_phy_instance_ctrl_t g_ether_phy1_ctrl;

const ether_phy_extend_cfg_t g_ether_phy1_extend =
{
    .port_type           = ETHER_PHY_PORT_TYPE_ETHERNET,
    .mdio_type           = ETHER_PHY_MDIO_GMAC,
    .bps                 = ETHER_PHY_SPEED_100,
    .duplex              = ETHER_PHY_DUPLEX_FULL,
    .auto_negotiation    = ETHER_PHY_AUTO_NEGOTIATION_ON,
    .phy_reset_pin       = BSP_IO_PORT_13_PIN_4,
    .phy_reset_time      = 15000,
    .p_selector_instance = (ether_selector_instance_t *)&g_ether_selector1,
    .p_target_init       = ether_phy_targets_initialize_rtl8211_rgmii,
};

const ether_phy_cfg_t g_ether_phy1_cfg =
{

    .channel                   = 1,
    .phy_lsi_address           = 2,
    .phy_reset_wait_time       = 0x00020000,
    .mii_bit_access_wait_time  = 0,                         // Unused
    .phy_lsi_type              = ETHER_PHY_LSI_TYPE_CUSTOM,
    .flow_control              = ETHER_PHY_FLOW_CONTROL_DISABLE,
    .mii_type                  = (ether_phy_mii_type_t) 0,  // Unused
    .p_context                 = NULL,
    .p_extend                  = &g_ether_phy1_extend
};

/* Instance structure to use this module. */
const ether_phy_instance_t g_ether_phy1 =
{
    .p_ctrl        = &g_ether_phy1_ctrl,
    .p_cfg         = &g_ether_phy1_cfg,
    .p_api         = &g_ether_phy_on_ether_phy
};
ether_selector_instance_ctrl_t g_ether_selector0_ctrl;

const ether_selector_cfg_t g_ether_selector0_cfg =
{
    .channel                   = 0,
    .phylink                   = ETHER_SELECTOR_PHYLINK_POLARITY_LOW,
    .interface                 = ETHER_SELECTOR_INTERFACE_RGMII,
    .speed                     = ETHER_SELECTOR_SPEED_100_MBPS,
    .duplex                    = ETHER_SELECTOR_DUPLEX_FULL,
    .ref_clock                 = ETHER_SELECTOR_REF_CLOCK_INPUT,
    .p_extend                  = NULL,
};

/* Instance structure to use this module. */
const ether_selector_instance_t g_ether_selector0 =
{
    .p_ctrl        = &g_ether_selector0_ctrl,
    .p_cfg         = &g_ether_selector0_cfg,
    .p_api         = &g_ether_selector_on_ether_selector
};
ether_phy_instance_ctrl_t g_ether_phy0_ctrl;

const ether_phy_extend_cfg_t g_ether_phy0_extend =
{
    .port_type           = ETHER_PHY_PORT_TYPE_ETHERNET,
    .mdio_type           = ETHER_PHY_MDIO_GMAC,
    .bps                 = ETHER_PHY_SPEED_100,
    .duplex              = ETHER_PHY_DUPLEX_FULL,
    .auto_negotiation    = ETHER_PHY_AUTO_NEGOTIATION_ON,
    .phy_reset_pin       = BSP_IO_PORT_13_PIN_4,
    .phy_reset_time      = 15000,
    .p_selector_instance = (ether_selector_instance_t *)&g_ether_selector0,
    .p_target_init       = ether_phy_targets_initialize_rtl8211_rgmii,
};

const ether_phy_cfg_t g_ether_phy0_cfg =
{

    .channel                   = 0,
    .phy_lsi_address           = 1,
    .phy_reset_wait_time       = 0x00020000,
    .mii_bit_access_wait_time  = 0,                         // Unused
    .phy_lsi_type              = ETHER_PHY_LSI_TYPE_CUSTOM,
    .flow_control              = ETHER_PHY_FLOW_CONTROL_DISABLE,
    .mii_type                  = (ether_phy_mii_type_t) 0,  // Unused
    .p_context                 = NULL,
    .p_extend                  = &g_ether_phy0_extend
};

/* Instance structure to use this module. */
const ether_phy_instance_t g_ether_phy0 =
{
    .p_ctrl        = &g_ether_phy0_ctrl,
    .p_cfg         = &g_ether_phy0_cfg,
    .p_api         = &g_ether_phy_on_ether_phy
};
const ether_phy_instance_t *g_ether0_phy_instance[BSP_FEATURE_GMAC_MAX_PORTS] =
            {
#define FSP_NOT_DEFINED (1)
#if (FSP_NOT_DEFINED == g_ether_phy0)
                    NULL,
#else
                    &g_ether_phy0,
#endif
#if (FSP_NOT_DEFINED == g_ether_phy1)
                    NULL,
#else
                    &g_ether_phy1,
#endif
#if (FSP_NOT_DEFINED == g_ether_phy2)
                    NULL,
#else
                    &g_ether_phy2,
#endif
#undef FSP_NOT_DEFINED
            };

            gmac_instance_ctrl_t g_ether0_ctrl;

            uint8_t g_ether0_mac_address[6] = { 0x00,0x11,0x22,0x33,0x44,0x55 };

            __attribute__((__aligned__(16))) gmac_instance_descriptor_t g_ether0_tx_descriptors[8] ETHER_BUFFER_PLACE_IN_SECTION;
            __attribute__((__aligned__(16))) gmac_instance_descriptor_t g_ether0_rx_descriptors[8] ETHER_BUFFER_PLACE_IN_SECTION;

            __attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer0[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer1[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer2[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer3[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer4[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer5[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer6[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer7[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer8[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer9[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer10[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer11[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer12[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer13[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer14[1524]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer15[1524]ETHER_BUFFER_PLACE_IN_SECTION;


uint8_t *pp_g_ether0_ether_buffers[( 8 + 8 )] = {
            (uint8_t *) &g_ether0_ether_buffer0[0],
(uint8_t *) &g_ether0_ether_buffer1[0],
(uint8_t *) &g_ether0_ether_buffer2[0],
(uint8_t *) &g_ether0_ether_buffer3[0],
(uint8_t *) &g_ether0_ether_buffer4[0],
(uint8_t *) &g_ether0_ether_buffer5[0],
(uint8_t *) &g_ether0_ether_buffer6[0],
(uint8_t *) &g_ether0_ether_buffer7[0],
(uint8_t *) &g_ether0_ether_buffer8[0],
(uint8_t *) &g_ether0_ether_buffer9[0],
(uint8_t *) &g_ether0_ether_buffer10[0],
(uint8_t *) &g_ether0_ether_buffer11[0],
(uint8_t *) &g_ether0_ether_buffer12[0],
(uint8_t *) &g_ether0_ether_buffer13[0],
(uint8_t *) &g_ether0_ether_buffer14[0],
(uint8_t *) &g_ether0_ether_buffer15[0],

            };

            const gmac_extend_cfg_t g_ether0_extend_cfg =
            {
                .p_rx_descriptors        = g_ether0_rx_descriptors,
                .p_tx_descriptors        = g_ether0_tx_descriptors,

                .phylink                 = GMAC_PHYLINK_DISABLE,

#if defined(VECTOR_NUMBER_GMAC_PMT)
                .pmt_irq                 = VECTOR_NUMBER_GMAC_PMT,
#else
                .pmt_irq                 = FSP_INVALID_VECTOR,
#endif
                .pmt_interrupt_priority  = (12),

                .pp_phy_instance         = (ether_phy_instance_t const *(*)[BSP_FEATURE_GMAC_MAX_PORTS]) g_ether0_phy_instance,

#if defined(GMAC_IMPLEMENT_ETHSW)
                .p_ethsw_instance        = &g_ethsw0
#endif // GMAC_IMPLEMENT_ETHSW
            };

            const ether_cfg_t g_ether0_cfg =
            {
                .channel                 = 0,
                .zerocopy                = ETHER_ZEROCOPY_DISABLE,
                .multicast               = (ether_multicast_t) 0,    // Unused
                .promiscuous             = ETHER_PROMISCUOUS_DISABLE,
                .flow_control            = ETHER_FLOW_CONTROL_DISABLE,
                .broadcast_filter        = 0, // Unused
                .p_mac_address           = g_ether0_mac_address,

                .num_tx_descriptors      = 8,
                .num_rx_descriptors      = 8,

                .pp_ether_buffers        = pp_g_ether0_ether_buffers,

                .ether_buffer_size       = 1524,

#if defined(VECTOR_NUMBER_GMAC_SBD)
                .irq                     = VECTOR_NUMBER_GMAC_SBD,
#else
                .irq                     = FSP_INVALID_VECTOR,
#endif
                .interrupt_priority      = (12),

                .p_callback              = user_ether0_callback,
                .p_ether_phy_instance    = (ether_phy_instance_t *) 0, // Unused
                .p_context               = NULL,
                .p_extend                = &g_ether0_extend_cfg
            };

/* Instance structure to use this module. */
const ether_instance_t g_ether0 =
{
    .p_ctrl        = &g_ether0_ctrl,
    .p_cfg         = &g_ether0_cfg,
    .p_api         = &g_ether_on_gmac
};
sci_uart_instance_ctrl_t     g_uart0_ctrl;

            #define FSP_NOT_DEFINED (1)
            #if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

            /* If the transfer module is DMAC, define a DMAC transfer callback. */
            extern void sci_uart_tx_dmac_callback(sci_uart_instance_ctrl_t * p_instance_ctrl);

            void g_uart0_tx_transfer_callback (transfer_callback_args_t * p_args)
            {
                FSP_PARAMETER_NOT_USED(p_args);
                sci_uart_tx_dmac_callback(&g_uart0_ctrl);
            }
            #endif

            #if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

            /* If the transfer module is DMAC, define a DMAC transfer callback. */
            extern void sci_uart_rx_dmac_callback(sci_uart_instance_ctrl_t * p_instance_ctrl);

            void g_uart0_rx_transfer_callback (transfer_callback_args_t * p_args)
            {
                FSP_PARAMETER_NOT_USED(p_args);
                sci_uart_rx_dmac_callback(&g_uart0_ctrl);
            }
            #endif
            #undef FSP_NOT_DEFINED

            sci_baud_setting_t           g_uart0_baud_setting =
            {
                /* Baud rate calculated with 0.160% error. */ .baudrate_bits_b.abcse = 0, .baudrate_bits_b.abcs = 0, .baudrate_bits_b.bgdm = 1, .baudrate_bits_b.cks = 0, .baudrate_bits_b.brr = 51, .baudrate_bits_b.mddr = (uint8_t) 256, .baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t g_uart0_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &g_uart0_baud_setting,
#if 1
                .clock_source           = SCI_UART_CLOCK_SOURCE_SCI0ASYNCCLK,
#else
                .clock_source           = SCI_UART_CLOCK_SOURCE_PCLKM,
#endif
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting          = {
                    .enable = SCI_UART_RS485_DISABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                    .assertion_time = 1,
                    .negation_time = 1,
                },
            };

            /** UART interface configuration */
            const uart_cfg_t g_uart0_cfg =
            {
                .channel             = 0,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = user_uart0_callback,
                .p_context           = NULL,
                .p_extend            = &g_uart0_cfg_extend,
                .p_transfer_tx       = g_uart0_P_TRANSFER_TX,
                .p_transfer_rx       = g_uart0_P_TRANSFER_RX,
                .rxi_ipl             = (12),
                .txi_ipl             = (12),
                .tei_ipl             = (12),
                .eri_ipl             = (12),
#if defined(VECTOR_NUMBER_SCI0_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI0_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI0_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI0_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI0_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t g_uart0 =
{
    .p_ctrl        = &g_uart0_ctrl,
    .p_cfg         = &g_uart0_cfg,
    .p_api         = &g_uart_on_sci
};
void g_hal_init(void) {
g_common_init();
}
