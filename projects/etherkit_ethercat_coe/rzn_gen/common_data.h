/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "r_cmt.h"
#include "r_timer_api.h"
#include "r_ether_selector.h"
#include "r_ether_selector_api.h"
#include "r_ether_phy.h"
#include "r_ether_phy_api.h"
#include "rm_ethercat_ssc_port.h"
#include "rm_ethercat_ssc_port_api.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
/** Timer on CMT Instance. */
extern const timer_instance_t g_timer0;

/** Access the CMT instance using these structures when calling API functions directly (::p_api is not used). */
extern cmt_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef rm_ssc_port_timer_interrupt
void rm_ssc_port_timer_interrupt(timer_callback_args_t * p_args);
#endif
/** ether_selector on ether_selector Instance. */
extern const ether_selector_instance_t g_ether_selector1;

/** Access the Ethernet Selector instance using these structures when calling API functions directly (::p_api is not used). */
extern ether_selector_instance_ctrl_t g_ether_selector1_ctrl;
extern const ether_selector_cfg_t g_ether_selector1_cfg;
#ifndef ETHER_PHY_LSI_TYPE_KIT_COMPONENT
  #define ETHER_PHY_LSI_TYPE_KIT_COMPONENT ETHER_PHY_LSI_TYPE_DEFAULT
#endif

#ifndef phy_rtl8211f_initial
void phy_rtl8211f_initial(ether_phy_instance_ctrl_t * p_instance_ctrl);
#endif

/** ether_phy on ether_phy Instance. */
extern const ether_phy_instance_t g_ether_phy1;

/** Access the Ethernet PHY instance using these structures when calling API functions directly (::p_api is not used). */
extern ether_phy_instance_ctrl_t g_ether_phy1_ctrl;
extern const ether_phy_cfg_t g_ether_phy1_cfg;
/** ether_selector on ether_selector Instance. */
extern const ether_selector_instance_t g_ether_selector0;

/** Access the Ethernet Selector instance using these structures when calling API functions directly (::p_api is not used). */
extern ether_selector_instance_ctrl_t g_ether_selector0_ctrl;
extern const ether_selector_cfg_t g_ether_selector0_cfg;
#ifndef ETHER_PHY_LSI_TYPE_KIT_COMPONENT
  #define ETHER_PHY_LSI_TYPE_KIT_COMPONENT ETHER_PHY_LSI_TYPE_DEFAULT
#endif

#ifndef phy_rtl8211f_initial
void phy_rtl8211f_initial(ether_phy_instance_ctrl_t * p_instance_ctrl);
#endif

/** ether_phy on ether_phy Instance. */
extern const ether_phy_instance_t g_ether_phy0;

/** Access the Ethernet PHY instance using these structures when calling API functions directly (::p_api is not used). */
extern ether_phy_instance_ctrl_t g_ether_phy0_ctrl;
extern const ether_phy_cfg_t g_ether_phy0_cfg;
/** ethercat_ssc_port on ethercat_ssc_port Instance. */
extern const ethercat_ssc_port_instance_t g_ethercat_ssc_port0;

/** Access the EtherCAT SSC Port instance using these structures when calling API functions directly (::p_api is not used). */
extern ethercat_ssc_port_instance_ctrl_t g_ethercat_ssc_port0_ctrl;
extern const ethercat_ssc_port_extend_cfg_t g_ethercat_ssc_port0_ext_cfg;
extern const ethercat_ssc_port_cfg_t g_ethercat_ssc_port0_cfg;
extern ethercat_ssc_port_instance_t const * gp_ethercat_ssc_port;

#ifndef NULL
void NULL(ethercat_ssc_port_callback_args_t * p_args);
#endif
/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
