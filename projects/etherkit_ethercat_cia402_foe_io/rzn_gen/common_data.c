/* generated common source file - do not edit */
#include "common_data.h"

cmt_instance_ctrl_t g_timer0_ctrl;
const timer_cfg_t g_timer0_cfg =
{
    .mode                = TIMER_MODE_PERIODIC,
    /* Actual period: 0.001 seconds. */ .period_counts = (uint32_t) 0x186a, .source_div = (timer_source_div_t)3,
    .channel             = 0,
    .p_callback          = rm_ssc_port_timer_interrupt,
    .p_context           = NULL,
    .p_extend            = NULL,
    .cycle_end_ipl       = (13),
#if defined(VECTOR_NUMBER_CMT0_CMI)
    .cycle_end_irq       = VECTOR_NUMBER_CMT0_CMI,
#else
    .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{
    .p_ctrl        = &g_timer0_ctrl,
    .p_cfg         = &g_timer0_cfg,
    .p_api         = &g_timer_on_cmt
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
    .port_type           = ETHER_PHY_PORT_TYPE_ETHER_CAT,
    .mdio_type           = ETHER_PHY_MDIO_GMAC,
    .bps                 = ETHER_PHY_SPEED_100,
    .duplex              = ETHER_PHY_DUPLEX_FULL,
    .auto_negotiation    = ETHER_PHY_AUTO_NEGOTIATION_ON,
    .phy_reset_pin       = BSP_IO_PORT_13_PIN_4,
    .phy_reset_time      = 15000,
    .p_selector_instance = (ether_selector_instance_t *)&g_ether_selector1,
    .p_target_init       = phy_rtl8211f_initial,
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
    .port_type           = ETHER_PHY_PORT_TYPE_ETHER_CAT,
    .mdio_type           = ETHER_PHY_MDIO_GMAC,
    .bps                 = ETHER_PHY_SPEED_100,
    .duplex              = ETHER_PHY_DUPLEX_FULL,
    .auto_negotiation    = ETHER_PHY_AUTO_NEGOTIATION_ON,
    .phy_reset_pin       = BSP_IO_PORT_13_PIN_4,
    .phy_reset_time      = 15000,
    .p_selector_instance = (ether_selector_instance_t *)&g_ether_selector0,
    .p_target_init       = phy_rtl8211f_initial,
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
ethercat_ssc_port_instance_ctrl_t g_ethercat_ssc_port0_ctrl;

const ethercat_ssc_port_extend_cfg_t g_ethercat_ssc_port0_ext_cfg =
{
    .eeprom_size             = ETHERCAT_SSC_PORT_EEPROM_SIZE_UNDER_32KBIT,
    .txc0                    = ETHERCAT_SSC_PORT_TXC_DELAY_00NS,
    .txc1                    = ETHERCAT_SSC_PORT_TXC_DELAY_00NS,
    .txc2                    = ETHERCAT_SSC_PORT_TXC_DELAY_00NS,
    .p_ether_phy_instance[0] =
#define FSP_NOT_DEFINED (1)
#if (FSP_NOT_DEFINED == g_ether_phy0)
                    NULL,
#else
                    &g_ether_phy0,
#endif
    .p_ether_phy_instance[1] =
#if (FSP_NOT_DEFINED == g_ether_phy1)
                    NULL,
#else
                    &g_ether_phy1,
#endif
    .p_ether_phy_instance[2] =
#if (FSP_NOT_DEFINED == FSP_NOT_DEFINED)
                    NULL,
#else
                    &FSP_NOT_DEFINED,
#endif
};

const ethercat_ssc_port_cfg_t g_ethercat_ssc_port0_cfg =
{
    .reset_hold_time = 1,
    .reset_wait_time = 500000,
    .address_offset  = 1,

#if defined(VECTOR_NUMBER_ESC_CAT)
    .common_irq   = VECTOR_NUMBER_ESC_CAT,
#else
    .common_irq   = FSP_INVALID_VECTOR,
#endif
    .common_ipl   = (12),
 #if defined(VECTOR_NUMBER_ESC_SYNC0)
    .sync0_irq   = VECTOR_NUMBER_ESC_SYNC0,
#else
    .sync0_irq   = FSP_INVALID_VECTOR,
#endif
    .sync0_ipl = (12),
 #if defined(VECTOR_NUMBER_ESC_SYNC1)
    .sync1_irq   = VECTOR_NUMBER_ESC_SYNC1,
#else
    .sync1_irq   = FSP_INVALID_VECTOR,
#endif
    .sync1_ipl = (12),

    .p_callback       = NULL,

    .p_timer_instance =
#if (FSP_NOT_DEFINED == g_timer0)
                    NULL,
#else
                    &g_timer0,
#endif

    .p_context = NULL,
    .p_extend  = &g_ethercat_ssc_port0_ext_cfg,
};

/* Instance structure to use this module. */
const ethercat_ssc_port_instance_t g_ethercat_ssc_port0 =
{
    .p_ctrl        = &g_ethercat_ssc_port0_ctrl,
    .p_cfg         = &g_ethercat_ssc_port0_cfg,
    .p_api         = &g_ethercat_ssc_port_on_ethercat_ssc_port
};

ethercat_ssc_port_instance_t const * gp_ethercat_ssc_port = &g_ethercat_ssc_port0;
/** IOPORT interface configuration for event link **/
            const ioport_extend_cfg_t  g_ioport_cfg_extend =
            {
                .port_group_output_cfg[IOPORT_PORT_GROUP_1] =
                {
                    .pin_select   = (uint8_t)( IOPORT_EVENT_PIN_SELECTION_NONE),
                    .operation    = IOPORT_EVENT_OUTPUT_OPERATION_LOW
                },
                .port_group_output_cfg[IOPORT_PORT_GROUP_2] =
                {
                    .pin_select   = (uint8_t)( IOPORT_EVENT_PIN_SELECTION_NONE),
                    .operation    = IOPORT_EVENT_OUTPUT_OPERATION_LOW
                },
                .port_group_input_cfg[IOPORT_PORT_GROUP_1] =
                {
                    .event_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .pin_select    = (uint8_t)( IOPORT_EVENT_PIN_SELECTION_NONE),
                    .edge_detection    = IOPORT_EVENT_DETECTION_RISING_EDGE,
                    .overwrite_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .buffer_init_value = IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 7U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 6U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 5U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 3U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 2U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 1U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW
                },
                .port_group_input_cfg[IOPORT_PORT_GROUP_2] =
                {
                    .event_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .pin_select    = (uint8_t)( IOPORT_EVENT_PIN_SELECTION_NONE),
                    .edge_detection    = IOPORT_EVENT_DETECTION_RISING_EDGE,
                    .overwrite_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .buffer_init_value = IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 6U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 5U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 4U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 3U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 2U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW << 1U | IOPORT_EVENT_INITIAL_BUFFER_VALUE_LOW
                },
                .single_port_cfg[IOPORT_SINGLE_PORT_0] =
                {
                    .event_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .direction = IOPORT_EVENT_DIRECTION_OUTPUT,
                    .port_num  = (uint16_t)BSP_IO_PORT_16_PIN_0,
                    .operation = IOPORT_EVENT_OUTPUT_OPERATION_LOW,
                    .edge_detection = IOPORT_EVENT_DETECTION_RISING_EDGE
                },
                .single_port_cfg[IOPORT_SINGLE_PORT_1] =
                {
                    .event_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .direction = IOPORT_EVENT_DIRECTION_OUTPUT,
                    .port_num  = (uint16_t)BSP_IO_PORT_16_PIN_0,
                    .operation = IOPORT_EVENT_OUTPUT_OPERATION_LOW,
                    .edge_detection = IOPORT_EVENT_DETECTION_RISING_EDGE
                },
                .single_port_cfg[IOPORT_SINGLE_PORT_2] =
                {
                    .event_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .direction = IOPORT_EVENT_DIRECTION_OUTPUT,
                    .port_num  = (uint16_t)BSP_IO_PORT_16_PIN_0,
                    .operation = IOPORT_EVENT_OUTPUT_OPERATION_LOW,
                    .edge_detection = IOPORT_EVENT_DETECTION_RISING_EDGE
                },
                .single_port_cfg[IOPORT_SINGLE_PORT_3] =
                {
                    .event_control = IOPORT_EVENT_CONTROL_DISABLE,
                    .direction = IOPORT_EVENT_DIRECTION_OUTPUT,
                    .port_num  = (uint16_t)BSP_IO_PORT_16_PIN_0,
                    .operation = IOPORT_EVENT_OUTPUT_OPERATION_LOW,
                    .edge_detection = IOPORT_EVENT_DETECTION_RISING_EDGE
                }
            };

ioport_instance_ctrl_t g_ioport_ctrl;

const ioport_instance_t g_ioport =
        {
            .p_api = &g_ioport_on_ioport,
            .p_ctrl = &g_ioport_ctrl,
            .p_cfg = &g_bsp_pin_cfg
        };



void g_common_init(void) {
}
