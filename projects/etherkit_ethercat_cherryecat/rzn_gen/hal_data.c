/* generated HAL source file - do not edit */
#include "hal_data.h"


#if defined(BSP_MCU_GROUP_RZT2M) || defined(BSP_MCU_GROUP_RZN2L)
#define ETHER_BUFFER_PLACE_IN_SECTION BSP_PLACE_IN_SECTION(".noncache_buffer.eth")
#else
#define ETHER_BUFFER_PLACE_IN_SECTION
#endif
mtu3_instance_ctrl_t g_mtu3_ctrl;
#if 0
const mtu3_extended_uvw_cfg_t g_mtu3_uvw_extend =
{
    .tgru_val                       = 0x0000,
    .tgrv_val                       = 0x0000,
    .tgrw_val                       = 0x0000,
    .mtu3_clk_div_u                 = MTU3_DIV_UVW_PCLKH_1,
    .mtu3_clk_div_v                 = MTU3_DIV_UVW_PCLKH_1,
    .mtu3_clk_div_w                 = MTU3_DIV_UVW_PCLKH_1,
    .output_pin_level_u             = MTU3_IO_PIN_LEVEL_UVW_NO_FUNC,
    .output_pin_level_v             = MTU3_IO_PIN_LEVEL_UVW_NO_FUNC,
    .output_pin_level_w             = MTU3_IO_PIN_LEVEL_UVW_NO_FUNC,
    .noise_filter_mtioc_setting_uvw = (mtu3_noise_filter_setting_uvw_t)(MTU3_NOISE_FILTER_UVW_DISABLE),
    .noise_filter_mtioc_clk_uvw     = MTU3_NOISE_FILTER_CLOCK_PCLKH_DIV_1,

#if defined(VECTOR_NUMBER_TGIU5)
    .capture_u_ipl                  = (BSP_IRQ_DISABLED),
    .capture_u_irq                  = VECTOR_NUMBER_TGIU5,
    .capture_u_select_event         = (bsp_irq_gpt_selected_event_t)0,
#elif defined(VECTOR_NUMBER_GPT_GPT00_0_INT)
    .capture_u_ipl                  = FSP_NOT_DEFINED,
    .capture_u_irq                  = VECTOR_NUMBER_GPT_GPT00_0_INT,
    .capture_u_select_event         = BSP_IRQ_GPT_SELECTED_EVENT_TCIU5,
#else
    .capture_u_ipl                  = (BSP_IRQ_DISABLED),
    .capture_u_irq                  = FSP_INVALID_VECTOR,
    .capture_u_select_event         = (bsp_irq_gpt_selected_event_t)0,
#endif
#if defined(VECTOR_NUMBER_TGIV5)
    .capture_v_ipl                  = (BSP_IRQ_DISABLED),
    .capture_v_irq                  = VECTOR_NUMBER_TGIV5,
    .capture_v_select_event         = (bsp_irq_gpt_selected_event_t)0,
#elif defined(VECTOR_NUMBER_GPT_GPT00_0_INT)
    .capture_v_ipl                  = FSP_NOT_DEFINED,
    .capture_v_irq                  = VECTOR_NUMBER_GPT_GPT00_0_INT,
    .capture_v_select_event         = BSP_IRQ_GPT_SELECTED_EVENT_TCIV5,
#else
    .capture_v_ipl                  = (BSP_IRQ_DISABLED),
    .capture_v_irq                  = FSP_INVALID_VECTOR,
    .capture_v_select_event         = (bsp_irq_gpt_selected_event_t)0,
#endif
#if defined(VECTOR_NUMBER_TGIW5)
    .capture_w_ipl                  = (BSP_IRQ_DISABLED),
    .capture_w_irq                  = VECTOR_NUMBER_TGIW5,
    .capture_w_select_event         = (bsp_irq_gpt_selected_event_t)0,
#elif defined(VECTOR_NUMBER_GPT_GPT00_0_INT)
    .capture_w_ipl                  = FSP_NOT_DEFINED,
    .capture_w_irq                  = VECTOR_NUMBER_GPT_GPT00_0_INT,
    .capture_w_select_event         = BSP_IRQ_GPT_SELECTED_EVENT_TGIW5,
#else
    .capture_w_ipl                  = (BSP_IRQ_DISABLED),
    .capture_w_irq                  = FSP_INVALID_VECTOR,
    .capture_w_select_event         = (bsp_irq_gpt_selected_event_t)0,
#endif
};
const mtu3_extended_pwm_cfg_t g_mtu3_pwm_extend =
{
    .interrupt_skip_mode_a          = MTU3_INTERRUPT_SKIP_MODE_1,
    .interrupt_skip_mode_b          = MTU3_INTERRUPT_SKIP_MODE_1,
    .adc_a_compare_match            = 0x0000,
    .adc_b_compare_match            = 0x0000,
    .interrupt_skip_count_tciv4     = MTU3_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_count_tgia3     = MTU3_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_count_tciv7     = MTU3_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_count_tgia6     = MTU3_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_count_tgr4an_bn = MTU3_INTERRUPT_SKIP_COUNT_0,
    .interrupt_skip_count_tgr7an_bn = MTU3_INTERRUPT_SKIP_COUNT_0,
};
#endif
const mtu3_extended_cfg_t g_mtu3_extend =
{
    .tgra_val            = 0xFFFF,
    .tgrb_val            = 0xFFFF,
    .tgrc_val            = 0xFFFF,
    .tgrd_val            = 0xFFFF,
    .mtu3_clk_div        = MTU3_DIV_PCLKH_1,
    .clk_edge            = MTU3_CLOCK_EDGE_RISING,
    .mtu3_clear          = MTU3_TCNT_CLEAR_DISABLE,
    .mtioc_ctrl_setting  = { .output_pin_level_a = MTU3_IO_PIN_LEVEL_NO_OUTPUT,
                             .output_pin_level_b = MTU3_IO_PIN_LEVEL_NO_OUTPUT
                           },
    .bit_mode            = MTU3_BIT_MODE_NORMAL_16BIT,
    .counting_mode       = MTU3_PHASE_COUNTING_MODE_NONE,
    .external_clock      = MTU3_EXTERNAL_CLOCK_MTCLKA_B,
#if defined(VECTOR_NUMBER_TGIA0)
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_a_irq       = VECTOR_NUMBER_TGIA0,
    .capture_a_source_select = BSP_IRQ_DISABLED,
    .capture_a_select_event  = (bsp_irq_gpt_selected_event_t)0,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .capture_a_ipl       = FSP_NOT_DEFINED,
    .capture_a_irq       = VECTOR_NUMBER_GPT00_0_INT,
    .capture_a_source_select = ,
    .capture_a_select_event  = BSP_IRQ_GPT_SELECTED_EVENT_TGIA0,
#else
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_a_irq       = FSP_INVALID_VECTOR,
    .capture_a_source_select = BSP_IRQ_DISABLED,
    .capture_a_select_event = (bsp_irq_gpt_selected_event_t)0,
#endif
#if defined(VECTOR_NUMBER_TGIB0)
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_irq       = VECTOR_NUMBER_TGIB0,
    .capture_b_source_select = BSP_IRQ_DISABLED,
    .capture_b_select_event  = (bsp_irq_gpt_selected_event_t)0,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .capture_b_ipl       = FSP_NOT_DEFINED,
    .capture_b_irq       = VECTOR_NUMBER_GPT00_0_INT,
    .capture_b_source_select = ,
    .capture_b_select_event  = BSP_IRQ_GPT_SELECTED_EVENT_TGIB0,
#else
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_irq       = FSP_INVALID_VECTOR,
    .capture_b_source_select = BSP_IRQ_DISABLED,
    .capture_b_select_event  = (bsp_irq_gpt_selected_event_t)0,
#endif
    .noise_filter_mtioc_setting  = (mtu3_noise_filter_setting_t)( MTU3_NOISE_FILTER_DISABLE),
    .noise_filter_mtioc_clk      = MTU3_NOISE_FILTER_CLOCK_PCLKH_DIV_1,
    .noise_filter_mtclk_setting  = (mtu3_noise_filter_mtclk_setting_t)( MTU3_NOISE_FILTER_EXTERNAL_DISABLE),
    .noise_filter_mtclk_clk      = MTU3_NOISE_FILTER_EXTERNAL_CLOCK_PCLKH_DIV_1,
    .adc_request_enable          = 0,
    .mtu3_elc_event_operation    ={.mtu0_elc_operation = MTU3_EVENT_OPERATION_SELECT_DISABLE,
                                   .mtu3_elc_operation = MTU3_EVENT_OPERATION_SELECT_DISABLE,
                                   .mtu4_elc_operation = MTU3_EVENT_OPERATION_SELECT_DISABLE
                                  },
#if 0
    .p_uvw_cfg                   = &g_mtu3_uvw_extend,
    .p_pwm_cfg                   = &g_mtu3_pwm_extend,
#else
    .p_uvw_cfg                   = NULL,
    .p_pwm_cfg                   = NULL,
#endif

#if defined(VECTOR_NUMBER_TCIV0)
    .cycle_end_source_select = BSP_IRQ_DISABLED,
    .cycle_end_select_event  = (bsp_irq_gpt_selected_event_t)0,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .cycle_end_source_select = ,
    .cycle_end_select_event  = BSP_IRQ_GPT_SELECTED_EVENT_TCIV0,
#else
    .cycle_end_source_select = BSP_IRQ_DISABLED,
    .cycle_end_select_event  = (bsp_irq_gpt_selected_event_t)0,
#endif
};
const timer_cfg_t g_mtu3_cfg =
{
    .mode                = TIMER_MODE_PERIODIC,
    .channel             = 0,
#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)
    .p_callback          = NULL,
#else
    .p_callback          = g_mtu3_callback,
#endif
    .p_context           = NULL,
    .p_extend            = &g_mtu3_extend,
#if defined(VECTOR_NUMBER_TCIV0)
    .cycle_end_ipl       = (8),
    .cycle_end_irq       = VECTOR_NUMBER_TCIV0,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .cycle_end_ipl       = FSP_NOT_DEFINED,
    .cycle_end_irq       = VECTOR_NUMBER_GPT00_0_INT,
#else
    .cycle_end_ipl       = (8),
    .cycle_end_irq       = FSP_INVALID_VECTOR,
#endif
};
/* Instance structure to use this module. */
const timer_instance_t g_mtu3 =
{
    .p_ctrl        = &g_mtu3_ctrl,
    .p_cfg         = &g_mtu3_cfg,
    .p_api         = &g_timer_on_mtu3
};
gpt_instance_ctrl_t g_timer0_ctrl;
#if 0
const gpt_extended_pwm_cfg_t g_timer0_pwm_extend =
{
#if defined(VECTOR_NUMBER_GPT0_UDF)
    .trough_ipl          = (BSP_IRQ_DISABLED),
    .trough_irq          = VECTOR_NUMBER_GPT0_UDF,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .trough_ipl          = FSP_NOT_DEFINED,
    .trough_irq          = VECTOR_NUMBER_GPT00_0_INT,
#else
    .trough_ipl          = (BSP_IRQ_DISABLED),
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
#if defined(VECTOR_NUMBER_GPT0_CCMPA)
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_a_irq       = VECTOR_NUMBER_GPT0_CCMPA,
    .capture_a_source_select = BSP_IRQ_DISABLED,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .capture_a_ipl       = FSP_NOT_DEFINED,
    .capture_a_irq       = VECTOR_NUMBER_GPT00_0_INT,
    .capture_a_source_select = ,
#else
    .capture_a_ipl       = (BSP_IRQ_DISABLED),
    .capture_a_irq       = FSP_INVALID_VECTOR,
    .capture_a_source_select = BSP_IRQ_DISABLED,
#endif
#if defined(VECTOR_NUMBER_GPT0_CCMPB)
    .capture_b_irq       = VECTOR_NUMBER_GPT0_CCMPB,
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_source_select = BSP_IRQ_DISABLED,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .capture_b_irq       = VECTOR_NUMBER_GPT00_0_INT,
    .capture_b_ipl       = FSP_NOT_DEFINED,
    .capture_b_source_select = ,
#else
    .capture_b_ipl       = (BSP_IRQ_DISABLED),
    .capture_b_irq       = FSP_INVALID_VECTOR,
    .capture_b_source_select = BSP_IRQ_DISABLED,
#endif
    .capture_filter_gtioca       = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb       = GPT_CAPTURE_FILTER_NONE,
#if 0
    .p_pwm_cfg                   = &g_timer0_pwm_extend,
#else
    .p_pwm_cfg                   = NULL,
#endif
#if defined(VECTOR_NUMBER_GPT0_DTE)
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
    .dead_time_irq       = VECTOR_NUMBER_GPT0_DTE,
    .dead_time_error_source_select = BSP_IRQ_DISABLED,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .dead_time_ipl       = FSP_NOT_DEFINED,
    .dead_time_irq       = VECTOR_NUMBER_GPT00_0_INT,
    .dead_time_error_source_select = ,
#else
    .dead_time_ipl       = (BSP_IRQ_DISABLED),
    .dead_time_irq       = FSP_INVALID_VECTOR,
    .dead_time_error_source_select = BSP_IRQ_DISABLED,
#endif
    .icds                = 0,
#if (2U == BSP_FEATURE_GPT_REGISTER_MASK_TYPE)
 #if (1U == BSP_FEATURE_GPT_INPUT_CAPTURE_SIGNAL_SELECTABLE)
    .gtioc_isel          = 0,
 #endif
#endif
#if defined(VECTOR_NUMBER_GPT0_OVF)
    .cycle_end_source_select = BSP_IRQ_DISABLED,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .cycle_end_source_select = ,
#else
    .cycle_end_source_select = BSP_IRQ_DISABLED,
#endif
#if defined(VECTOR_NUMBER_GPT0_UDF)
    .trough_source_select = BSP_IRQ_DISABLED,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .trough_source_select = ,
#else
    .trough_source_select  = BSP_IRQ_DISABLED,
#endif
};
const timer_cfg_t g_timer0_cfg =
{
    .mode                = TIMER_MODE_PERIODIC,
    /* Actual period: 10.73741824 seconds. Actual duty: 50%. */ .period_counts = (uint32_t) 0x100000000, .duty_cycle_counts = 0x80000000, .source_div = (timer_source_div_t)0,
    .channel             = GPT_CHANNEL_UNIT0_0,
#if (1 == BSP_FEATURE_BSP_IRQ_GPT_SEL_SUPPORTED)
    .p_callback          = NULL,
#else
    .p_callback          = timer0_esc_callback,
#endif
    .p_context           = NULL,
    .p_extend            = &g_timer0_extend,
#if defined(VECTOR_NUMBER_GPT0_OVF)
    .cycle_end_ipl       = (10),
    .cycle_end_irq       = VECTOR_NUMBER_GPT0_OVF,
#elif defined(VECTOR_NUMBER_GPT00_0_INT)
    .cycle_end_ipl       = FSP_NOT_DEFINED,
    .cycle_end_irq       = VECTOR_NUMBER_GPT00_0_INT,
#else
    .cycle_end_ipl       = (10),
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

#define ETHER_MAC_ADDRESS_INVALID (0)
#define ETHER_MAC_ADDRESS_VALID   (1)

            uint8_t g_ether0_mac_address[6] = { 0x00,0x11,0x22,0x33,0x44,0x55 };

#if ETHER_MAC_ADDRESS_INVALID == ETHER_MAC_ADDRESS_VALID
            uint8_t g_ether0_mac_address_1[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

#if ETHER_MAC_ADDRESS_INVALID == ETHER_MAC_ADDRESS_VALID
            uint8_t g_ether0_mac_address_2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

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
                .p_ethsw_instance        = &g_ethsw0,
#endif // GMAC_IMPLEMENT_ETHSW

#if ETHER_MAC_ADDRESS_INVALID == ETHER_MAC_ADDRESS_VALID
                .p_mac_address1          = g_ether0_mac_address_1,
#else
                .p_mac_address1          = NULL,
#endif
#if ETHER_MAC_ADDRESS_INVALID == ETHER_MAC_ADDRESS_VALID
                .p_mac_address2          = g_ether0_mac_address_2
#else
                .p_mac_address2          = NULL,
#endif
            };

            const ether_cfg_t g_ether0_cfg =
            {
                .channel                 = 0,
                .zerocopy                = ETHER_ZEROCOPY_ENABLE,
                .multicast               = (ether_multicast_t) 0,    // Unused
                .promiscuous             = ETHER_PROMISCUOUS_ENABLE,
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
