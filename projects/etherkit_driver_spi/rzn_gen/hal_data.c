/* generated HAL source file - do not edit */
#include "hal_data.h"
sci_spi_instance_ctrl_t g_sci3_ctrl;

#define FSP_NOT_DEFINED (1)
#if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
extern void sci_spi_tx_dmac_callback(sci_spi_instance_ctrl_t * p_instance_ctrl);

void g_sci3_tx_transfer_callback (transfer_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    sci_spi_tx_dmac_callback(&g_sci3_ctrl);
}
#endif

#if (FSP_NOT_DEFINED) != (FSP_NOT_DEFINED)

/* If the transfer module is DMAC, define a DMAC transfer callback. */
extern void sci_spi_rx_dmac_callback(sci_spi_instance_ctrl_t * p_instance_ctrl);

void g_sci3_rx_transfer_callback (transfer_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    sci_spi_rx_dmac_callback(&g_sci3_ctrl);
}
#endif
#undef FSP_NOT_DEFINED

/** SPI extended configuration */
const sci_spi_extended_cfg_t g_sci3_cfg_extend =
{
    .clk_div = {
        /* Actual calculated bitrate: 8000000. */ .cks = 0, .brr = 2,
    },
#if 1
    .clock_source = SCI_SPI_CLOCK_SOURCE_SCI3ASYNCCLK,
#else
    .clock_source = SCI_SPI_CLOCK_SOURCE_PCLKM,
#endif
};

const spi_cfg_t g_sci3_cfg =
{
    .channel         = 3,
    .operating_mode  = SPI_MODE_MASTER,
    .clk_phase       = SPI_CLK_PHASE_EDGE_ODD,
    .clk_polarity    = SPI_CLK_POLARITY_LOW,
    .mode_fault      = SPI_MODE_FAULT_ERROR_DISABLE,
    .bit_order       = SPI_BIT_ORDER_MSB_FIRST,
    .p_transfer_tx   = g_sci3_P_TRANSFER_TX,
    .p_transfer_rx   = g_sci3_P_TRANSFER_RX,
    .p_callback      = sci_spi_irq_callback,
    .p_context       = NULL,
    .rxi_irq         = VECTOR_NUMBER_SCI3_RXI,
    .txi_irq         = VECTOR_NUMBER_SCI3_TXI,
    .tei_irq         = VECTOR_NUMBER_SCI3_TEI,
    .eri_irq         = VECTOR_NUMBER_SCI3_ERI,
    .rxi_ipl         = (12),
    .txi_ipl         = (12),
    .tei_ipl         = (12),
    .eri_ipl         = (12),
    .p_extend        = &g_sci3_cfg_extend,
};
/* Instance structure to use this module. */
const spi_instance_t g_sci3 =
{
    .p_ctrl          = &g_sci3_ctrl,
    .p_cfg           = &g_sci3_cfg,
    .p_api           = &g_spi_on_sci
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
