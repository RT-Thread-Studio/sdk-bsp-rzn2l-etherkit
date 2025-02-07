/* generated HAL source file - do not edit */
#include "hal_data.h"
#if defined(BSP_MCU_GROUP_RZT2M) || defined(BSP_MCU_GROUP_RZN2L)
#define ETHER_BUFFER_PLACE_IN_SECTION BSP_PLACE_IN_SECTION(".noncache_buffer.eth")
#else
#define ETHER_BUFFER_PLACE_IN_SECTION
#endif
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
#if (FSP_NOT_DEFINED == FSP_NOT_DEFINED)
                    NULL,
#else
                    &FSP_NOT_DEFINED,
#endif
#undef FSP_NOT_DEFINED
            };

            gmac_instance_ctrl_t g_ether0_ctrl;

            uint8_t g_ether0_mac_address[6] = { 0x00,0x11,0x22,0x33,0x44,0x55 };

            __attribute__((__aligned__(16))) gmac_instance_descriptor_t g_ether0_tx_descriptors[8] ETHER_BUFFER_PLACE_IN_SECTION;
            __attribute__((__aligned__(16))) gmac_instance_descriptor_t g_ether0_rx_descriptors[8] ETHER_BUFFER_PLACE_IN_SECTION;

            __attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer0[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer1[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer2[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer3[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer4[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer5[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer6[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer7[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer8[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer9[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer10[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer11[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer12[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer13[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer14[2048]ETHER_BUFFER_PLACE_IN_SECTION;
__attribute__((__aligned__(32)))uint8_t g_ether0_ether_buffer15[2048]ETHER_BUFFER_PLACE_IN_SECTION;


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
                .promiscuous             = ETHER_PROMISCUOUS_ENABLE,
                .flow_control            = ETHER_FLOW_CONTROL_DISABLE,
                .broadcast_filter        = 0, // Unused
                .p_mac_address           = g_ether0_mac_address,

                .num_tx_descriptors      = 8,
                .num_rx_descriptors      = 8,

                .pp_ether_buffers        = pp_g_ether0_ether_buffers,

                .ether_buffer_size       = 2048,

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
