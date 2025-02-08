/* generated HAL source file - do not edit */
#include "hal_data.h"
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
