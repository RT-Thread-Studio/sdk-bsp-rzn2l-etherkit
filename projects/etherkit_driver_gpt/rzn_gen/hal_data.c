/* generated HAL source file - do not edit */
#include "hal_data.h"

gpt_instance_ctrl_t g_timer5_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer5_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT5_UDF,
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
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT5_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT5_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT5_CCMPB,
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
#if defined(VECTOR_NUMBER_GPT5_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT5_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer5_cfg =
{
    .mode                = TIMER_MODE_PWM,
    /* Actual period: 2 seconds. Actual duty: 50%. */ .period_counts = (uint32_t) 0x2faf0800, .duty_cycle_counts = 0x17d78400, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT0_5,
    .p_callback          = NULL,
    .p_context           = NULL,
    .p_extend            = &g_timer5_extend,
    .cycle_end_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT5_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT5_OVF,
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
gpt_instance_ctrl_t g_timer0_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer0_pwm_extend =
{
    .trough_ipl          = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT0_UDF)
    .trough_irq          = VECTOR_NUMBER_GPT0_UDF,
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
const gpt_extended_cfg_t g_timer0_extend =
{
    .gtioca = { .output_enabled = false,
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
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT0_CCMPA)
    .capture_a_irq       = VECTOR_NUMBER_GPT0_CCMPA,
#else
    .capture_a_irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_GPT0_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT0_CCMPB,
#else
    .capture_b_irq       = FSP_INVALID_VECTOR,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer0_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_GPT0_DTE)
    .dead_time_irq       = VECTOR_NUMBER_GPT0_DTE,
#else
    .dead_time_irq       = FSP_INVALID_VECTOR,
#endif
    .icds                = 0,
};
const timer_cfg_t g_timer0_cfg =
{
    .mode                = TIMER_MODE_PERIODIC,
    /* Actual period: 10.73741824 seconds. Actual duty: 50%. */ .period_counts = (uint32_t) 0x100000000, .duty_cycle_counts = 0x80000000, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT0_0,
    .p_callback          = timer0_callback,
    .p_context           = NULL,
    .p_extend            = &g_timer0_extend,
    .cycle_end_ipl       = (12),
#if defined(VECTOR_NUMBER_GPT0_OVF)
    .cycle_end_irq       = VECTOR_NUMBER_GPT0_OVF,
#else
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{
    .p_ctrl        = &g_timer0_ctrl,
    .p_cfg         = &g_timer0_cfg,
    .p_api         = &g_timer_on_gpt
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
