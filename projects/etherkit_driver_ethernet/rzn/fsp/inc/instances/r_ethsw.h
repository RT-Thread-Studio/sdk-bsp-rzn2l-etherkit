/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup ETHER_SWITCH
 * @{
 **********************************************************************************************************************/

#ifndef R_ETHSW_H
#define R_ETHSW_H

#include "bsp_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_ether_switch_cfg.h"
#include "r_ether_switch_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define ETHSW_MAC_ADDR_LENGTH    (6U)

/** Ethernet port macros */
#define ETHSW_PORT_MGMT          (3U)  ///< Management port number
#define ETHSW_PORT_BIT(x)    (1U << (x))

/* RZN2L have the EFP feture, RZT2M don't have it. */
#ifdef R_ETHSW_P0_QSTMACU0_MACA_Pos
 #define ETHSW_EFP_FEATURE_SUPPORTED
#endif                                       // R_ETHSW_P0_QSTMACU0_MACA_Pos

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED
 #define ETHSW_VLAN_VID_LEN             (2U)
#endif                                       /* ETHSW_EFP_FEATURE_SUPPORTED */

#define ETHER_SWITCH_TDMA_GPIO_COUNT    (8U) ///< Noumber of TDMA_GPIO
#ifdef  ETHSW_EFP_FEATURE_SUPPORTED
 #define ETHSW_EFP_SID_COUNT            (8U) ///< Number of Active Stream Identification table entry in EFP
 #define ETHSW_EFP_MEATER_COUNT         (8U) ///< Number of Meater in EFP
#endif                                       /* ETHSW_EFP_FEATURE_SUPPORTED */
#define ETHSW_QUEUE_COUNT               (8U) ///< Number of queues ETHSW has
#define ETHSW_QUEUE_COUNT               (8U) ///< Number of queues ETHSW has
#define ETHSW_TDMA_GPIO_COUNT           (8U) ///< Number of TDMA_GPIO

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** ETHER SWITCH control block. DO NOT INITIALIZE.  Initialization occurs when @ref ether_switch_api_t::open is called. */
typedef struct st_ethsw_instance_ctrl
{
    uint32_t open;                           ///< Used to determine if the channel is configured

    /* Configuration of Ethernet SWITCH-LSI module. */
    ether_switch_cfg_t const * p_switch_cfg; ///< Pointer to initial configurations.

    /* Interface for Ethernet Swith */
    R_ETHSW_Type     * p_reg_switch;         ///< Pointer to Ethernet Switch peripheral registers.
    R_ETHSS_Type     * p_reg_ethss;          ///< Pointer to Ethernet Subsystem peripheral registers.
    R_ETHSW_PTP_Type * p_reg_ethsw_ptp;      ///< Pointer to PTP Timer Pulse Control Registers.
} ethsw_instance_ctrl_t;

/** Enable/Disable Management Port Specific Frame Tagging */
typedef enum e_ethsw_specific_tag
{
    ETHSW_SPECIFIC_TAG_DISABLE = 0,    ///< Disable
    ETHSW_SPECIFIC_TAG_ENABLE  = 1     ///< Enable
} ethsw_specific_tag_t;

/** Enable/Disable PHYLINK Change Interrupt */
typedef enum e_ethsw_phylink
{
    ETHSW_PHYLINK_DISABLE = 0,         ///< Disable
    ETHSW_PHYLINK_ENABLE  = 1          ///< Enable
} ethsw_phylink_t;

/** Extend Configuration parameters. */
typedef struct st_ethsw_extend_cfg
{
    /* Management port specific frmame tag */
    ethsw_specific_tag_t specific_tag;    ///< Enable or Disable Management port specific frmame tag
    uint16_t             specific_tag_id; ///< Management port specific frmame tag ID, when specific_tag is enable

    /* Enable or disable link status change by PHYLINK. Call callback function when enable */
    ethsw_phylink_t phylink;
} ethsw_extend_cfg_t;

/** Speed and duplex of the port */
typedef enum e_ethsw_link_speed
{
    ETHSW_LINK_SPEED_NO_LINK = 0,      ///< Link is not established
    ETHSW_LINK_SPEED_10H     = 1,      ///< Link status is 10Mbit/s and half duplex
    ETHSW_LINK_SPEED_10F     = 2,      ///< Link status is 10Mbit/s and full duplex
    ETHSW_LINK_SPEED_100H    = 3,      ///< Link status is 100Mbit/s and half duplex
    ETHSW_LINK_SPEED_100F    = 4,      ///< Link status is 100Mbit/s and full duplex
    ETHSW_LINK_SPEED_1000H   = 5,      ///< Link status is 1000Mbit/s and half duplex
    ETHSW_LINK_SPEED_1000F   = 6       ///< Link status is 1000Mbit/s and full duplex
} ethsw_link_speed_t;

/** Port mask */
typedef struct  st_ethsw_port_mask
{
    uint32_t mask : 4;                 ///< [3.. 0] Port Mask: bit0=Port0, bit1=Port1, bit2=Port2, bit3=Port3(Management Port)
    uint32_t      : 28;                ///< [4..31]
} ethsw_port_mask_t;

/** MAC address */
typedef uint8_t ethsw_mac_addr_t[ETHSW_MAC_ADDR_LENGTH];

/** Address in MAC table entry */
typedef struct st_ethsw_mac_table_entry_addr
{
    ethsw_mac_addr_t * mac_addr;       ///< MAC address pointer
    uint16_t           vlan_id;        ///< VLAN ID
} ethsw_mac_table_entry_addr_t;

/** Information in MAC table entry */
typedef struct st_ethsw_mac_table_entry_info
{
    ethsw_port_mask_t port_mask;       ///< port mask
    uint32_t          priority;        ///< switching priority
} ethsw_mac_table_entry_info_t;

/** MAC table config */
typedef struct st_ethsw_mac_table_config
{
    bool learning;                     ///< Enable/disable MAC learning
    bool aging;                        ///< Enable/disable address aging
    bool migration;                    ///< Enable/disable the migration of devices from one port to another
    bool independent_vlan;             ///< Enable/disable independent VLAN learning
    bool discard_unknown;              ///< Enable/disable discarding of unknown destination frames
} ethsw_mac_table_config_t;

/** MAC table clear modes */
typedef enum e_ethsw_mac_table_clear_mode
{
    ETHSW_MAC_TABLE_CLEAR_STATIC  = 0, ///< Static MAC table entries
    ETHSW_MAC_TABLE_CLEAR_DYNAMIC = 1, ///< Learned MAC table entries
    ETHSW_MAC_TABLE_CLEAR_ALL     = 2, ///< Static and learn entries
} ethsw_mac_table_clear_mode_t;

/** Flood domain configuration for unknown frames */
typedef struct st_ethsw_flood_unknown_config
{
    ethsw_port_mask_t port_mask_bcast; ///< flood domain port mask for broadcasts with unkown destination
    ethsw_port_mask_t port_mask_mcast; ///< flood domain port mask for multicasts with unkown destination
    ethsw_port_mask_t port_mask_ucast; ///< flood domain port mask for unicasts with unkown destination
} ethsw_flood_unknown_config_t;

/** Link status */
typedef enum e_ethsw_link_status
{
    ETHSW_LINK_STATE_DOWN = 0,         ///< Link Down
    ETHSW_LINK_STATE_UP   = 1,         ///< Link Up
} ethsw_link_status_t;

/** DLR events for the DLR State Machine */
typedef enum e_ethsw_dlr_event
{
    ETHSW_DLR_EVENT_LINKLOST      = 0, ///< Link was lost
    ETHSW_DLR_EVENT_OWNFRAME      = 1, ///< the device received its own frame
    ETHSW_DLR_EVENT_LINKRESTORED  = 2, ///< Link is restored
    ETHSW_DLR_EVENT_BEACONFRAME   = 3, ///< Beacon Frame received
    ETHSW_DLR_EVENT_BEACONTIMEOUT = 4, ///< Beacon Timer timed out
    ETHSW_DLR_EVENT_NEWSUPERVISOR = 5, ///< a new Ring Supervisor was detected
    ETHSW_DLR_EVENT_NEWSTATE      = 6, ///< Beacon Hardware detected new state
} ethsw_dlr_event_t;

/** DLR initilize */
typedef struct st_ethsw_dlr_init
{
    ethsw_mac_addr_t * p_host_addr;    ///< host MAC address pointer
    void (* p_dlr_callback)(           ///< callback function pointer
        ethsw_dlr_event_t event,
        uint32_t          port);
} ethsw_dlr_init_t;

/** DLR beacon frame receive status */
typedef enum e_ethsw_dlr_beacon_state
{
    ETHSW_DLR_BEACON_INVALID = 0,      ///< Not receiving the valid beacon frmae
    ETHSW_DLR_BEACON_VALID   = 1,      ///< Receiving the valid beacon frmae
} ethsw_dlr_beacon_state_t;

/** DLR node status */
typedef enum e_ethsw_dlr_node_state
{
    ETHSW_DLR_NODE_IDLE   = 0,         ///< Idole State
    ETHSW_DLR_NODE_NORMAL = 1,         ///< Normal State
    ETHSW_DLR_NODE_FAULT  = 2,         ///< Fault State
} ethsw_dlr_node_state_t;

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

/** VLAN ID of an entry of the Active Stream Identification */
typedef uint8_t ethsw_vlan_vid_t[ETHSW_VLAN_VID_LEN];

/** The parameter for active steam Id table settings **/
typedef struct
{
    uint8_t dmaci[6];                  ///< DMACI/VIDI are the DMAC and VID of the incoming frame,
    uint8_t vidi[2];                   ///< which are matched to this table entry. A full match is required.
    uint8_t dmaco[6];                  ///< If a frame matches this entry,
    uint8_t vido[2];                   ///< its DMAC, VID, and PCP are replaced by these values.
    uint8_t pcpo;                      ///< The DEI bit keeps its value.
    uint8_t sid;                       ///< This is the frame handle assigned to the matched frame.
    ///< In case of this implementation, it directly refers
    ///< to a filter entry in the filter table.
    bool     fs;                       ///< If set, the filter referenced by SID is applied. If cleared, no filtering is applied.
    bool     ev;                       ///< If set, this table entry is valid. If cleared, this table entry shall be ignored.
    uint32_t port;
} ethsw_efp_asi_t;

/** The parameter for vlan input verification mode settings **/
typedef enum e_ethsw_vlan_input_verification_mode
{
    ETHSW_VICM_ALL_FRAMES_PASS = 0U,                          ///< Disabled, all frames pass
    ETHSW_VICM_MODE_ADMIT_ONLY_VLAN_TAGGED_FRAMES,            ///< Admit Only VLAN-tagged frames
    ETHSW_VICM_ADMIT_ONLY_UNTAGGED_OR_PRIORITY_TAGGED_FRAMES, ///< Admit Only Untagged or Priority-tagged frames
    ETHSW_VICM_DROP_ALL_FRAMES,                               ///< Do not use (drop all frames)
} ethsw_vlan_input_verification_mode_t;

/** The parameter for priority regeneration settings **/
typedef enum e_ethsw_vlan_input_tagging_mode
{
    ETHSW_VITM_VLAN_TAG_IS_NEITHER_ADDED_NOR_OVERWRITTEN = 0, ///< Disabled, VLAN tag is neither added nor overwritten.
    ETHSW_VITM_SINGLE_TAGGING_WITH_PASSTHROUGH,               ///< Single Tagging with Passthrough / VLAN ID (VID) Overwrite.
    ETHSW_VITM_SINGLE_TAGGING_WITH_REPLACE,                   ///< Single Tagging with Replace
    ETHSW_VITM_TAG_ALWAYS,                                    ///< Tag always
} ethsw_vlan_input_tagging_mode_t;

typedef struct
{
    uint8_t vid[2];                       ///< Port Domain VID
    uint8_t dei;                          ///< Port Drop Eligible Indicator
    uint8_t pcp;                          ///< Port Priority Code Point
    uint8_t tpid[2];                      ///< Port Tag Protocol Identifier
    ethsw_vlan_input_tagging_mode_t vitm; ///< VLAN input verification mode
    uint8_t pcp_remap[8];                 ///< VLAN Priority Code Point (PCP) Remap
} ethsw_efp_priority_regeneration_t;

/** The parameter for filtering function settings **/
typedef enum e_ethsw_vlan_tag_mode
{
    ETHSW_VLAN_TAG_MODE_UN_TAGGED = 0,             ///< Un Tagged (the frame must be untagged to match to this filter)
    ETHSW_VLAN_TAG_MODE_C_TAGGED,                  ///< C-Tagged (the frame must be C-Tagged to match to this filter)
    ///< ( 10 : Reserved)
    ETHSW_VLAN_TAG_MODE_DO_NOT_CHECK_VLAN_TAG = 3, ///< Do not check VLAN Tag (VLAN Tag check is skipped. All frames either untagged
    ///< or tagged with any VLAN/PCP/DEI may match this filter.)
} ethsw_vlan_tag_mode_t;

/** The parameter for EFP interrupt source id **/
typedef enum e_ethsw_efp_event
{
    ETHSW_EFP_INT_SRC_GATE_ERROR_DROP = 0, ///< Qci stream gate error interrupt (gating error drop)
    ETHSW_EFP_INT_SRC_SDU_OVER_DROP,       ///< Qci stream filter error interrupt (max SDU over drop)
    ETHSW_EFP_INT_SRC_FLOW_METER_DROP,     ///< Qci meter error interrupt (flow meter drop)
    ETHSW_EFP_INT_SRC_RUNTIME_ERROR        ///< Frame parser error interrupt (runtime error)
} ethsw_efp_event_t;

typedef struct
{
    uint8_t               qdasa;       ///< MAC address (MACA) select
    uint8_t               qmac[6];     ///< Qci stream filter table MAC address
    uint8_t               qmam[6];     ///< Qci stream filter table MAC address bit mask
    ethsw_vlan_tag_mode_t tagmd;       ///< Qci stream filter table VLAN Tag Mode
    uint8_t               vlanid[2];   ///< Qci stream filter table VLAN ID[11:0]
    uint8_t               dei;         ///< Qci stream filter table VLAN DEI
    uint8_t               pcp;         ///< Qci stream filter table VLAN PCP[2:0]
    uint8_t               vlanidm[2];  ///< Qci stream filter table VLAN ID[11:0] bit mask
    uint8_t               deim;        ///< Qci stream filter table VLAN DEI bit mask
    uint8_t               pcpm;        ///< Qci stream filter table VLAN PCP[2:0] bit mask
} ethsw_efp_qci_stream_filter_t;

/** The parameter for gating check settings **/
typedef struct
{
    bool    gaidv;                     ///< Qci stream filter Gating check enable
    uint8_t gaid;                      ///< Qci filter table gate ID
    bool    qgmod;                     ///< Flow gate mode
} ethsw_efp_gating_check_t;

/** The parameter for sdu max verification settings **/
typedef struct
{
    bool     msdue;                    ///< Qci stream filter table max SDU size Check enable
    uint16_t msdu;                     ///< Qci stream filter table max SDU size
    bool     qsmsm;                    ///< Qci stream MSDU mode
} ethsw_efp_sdu_max_verification_t;

/** The parameter for flow meter settings **/
typedef struct
{
    bool     meidv;                    ///< Qci stream filter table Meter ID Valid
    uint8_t  meid;                     ///< Qci stream filter table Meter ID
    bool     cf;                       ///< Coupling flag for meter
    bool     mm;                       ///< Flow meter mode
    bool     rfd;                      ///< Red frame drop
    uint32_t cbs;                      ///< CBS (Committed Burst Size)
    uint32_t cir;                      ///< CIR (Committed Information Rate)
    bool     me;                       ///< Enable meter
} ethsw_efp_flow_metering_t;

/** The parameter for configuration of interrupt source **/
typedef struct
{
    uint8_t qsmoi;                     ///< Qci Stream Filter Error Interrupt Enable/Disable
    uint8_t qgmoi;                     ///< Qci Gate Error Interrupt Enable/Disable
    uint8_t qrfi;                      ///< Qci Meter Error Interrupt Enable/Disable
    uint8_t errmask;                   ///< Frame Parser Runtime Error Mask
} ethsw_efp_interrupt_source_t;

/** The parameter for getting statistics information of EFP **/
typedef struct
{
    uint32_t qsmois;                   ///< Qci Stream Filter Error Interrupt Status (SDU Oversize)
    uint32_t qgmois;                   ///< Qci Gate Error Interrupt Status
    uint32_t qrfis;                    ///< Qci Meter Error Interrupt Status
    uint32_t error_status;             ///< Frame Parser Runtime Error Status
} ethsw_efp_status_t;
#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

/** Operating mode of RX Pattern Matcher */
typedef enum e_ethsw_rx_pattern_mode
{
    ETHSW_RX_PATTER_MODE_FIXED = 0,       ///< Mode1: Fixed 12-byte match
    ETHSW_RX_PATTER_MODE_LOOKUP,          ///< Mode2: 2-byte lookup mode
    ETHSW_RX_PATTER_MODE_RANGE_MATCH,     ///< Mode3: 2-byte range match at set offset
    ETHSW_RX_PATTER_MODE_RANGE_NOT_MATCH, ///< Mode4: 2-byte range not-match at set offset
} ethsw_rx_pattern_mode_t;

/** RX Pattern Matcher Function Control */
typedef struct st_ethsw_rx_pattern_ctrl
{
    bool match_not;                    ///< The functions of this control are executed if the pattern does not match.
    bool mgmt_fwd;                     ///< The frame is forwarded to the management port only
    bool discard;                      ///< The frame is discarded.
    bool set_prio;                     ///< Set frame priority, overriding normal classification.

    ethsw_rx_pattern_mode_t mode;      ///< Operating mode

    bool    timer_sel_ovr;             ///< Overrides the default timer to use by timestamp operations.
    bool    force_forward;             ///< The frame is forwarded to the ports indicated in PORTMASK,
    bool    hub_trigger;               ///< The port defined in the PORTMASK setting is allowed for transmitting one frame. Usable for Hub mode only.
    bool    match_red;                 ///< Enable the pattern matcher only when the TDMA indicates that this is the RED period.
    bool    match_not_red;             ///< Enable the pattern matcher only when the TDMA indicates that this is not the RED period.
    bool    vlan_skip;                 ///< Skips VLAN tag (4 bytes)
    uint8_t priority;                  ///< Priority of the frame used when SET_PRIO is set.
    bool    learning_dis;              ///< The hardware learning function is not executed.

    ethsw_port_mask_t port_mask;       ///< A port mask used depending on the control bits (for example, HUBTRIGGER).

    bool    imc_trigger;               ///< The ports defined in the PORTMASK setting are allowed for transmitting one frame from the queues indicated by QUEUESEL.
    bool    imc_trigger_dely;          ///< The ports defined in the PORTMASK setting are allowed for transmitting one frame from the queues indicated by QUEUESEL. Delayed by the value programmed in MMCTL_DLY_QTRIGGER_CTRL.
    bool    swap_bytes;                ///< The byte order is swapped from the order received by the frame.
    bool    match_lt;                  ///< The Length/Type field in the frame is compared against the value in length_type in the compare register.
    uint8_t timer_sel;                 ///< Override value to use when TIMER_SEL_OVR is set to 1 for selecting the timer for this frame.
    uint8_t queue_sel;                 ///< A queue selector for the HUBTRIGGER function.
} ethsw_rx_pattern_ctrl_t;

/** RX Pattern Matcher Function Data */
typedef union st_ethsw_rx_pattern_data
{
    /* Mode1: Fixed 12-byte match */
    struct
    {
        uint8_t cmp[12];               ///< Compare value 0 .. 12
        uint8_t msk[12];               ///< Masks bits 0 .. 12
    } fixed_mode;

    /* Mode2: 2-byte lookup mode */
    struct
    {
        uint16_t cmp0;                 ///< Compare value 0
        uint16_t cmp1;                 ///< Compare value 1
        uint16_t cmp2;                 ///< Compare value2
        uint16_t cmp3;                 ///< Compare value 3
        uint16_t cmp4;                 ///< Compare value 4
        uint16_t cmp5;                 ///< Compare value 5
        uint16_t mask;                 ///< Masks bits
        uint16_t length_type;          ///< Value to compare against the frame's Length/Type when MATCH_LT is set in the config register.
        uint8_t  offset;               ///< Byte offset to do the match after the MAC source address of the frame.
        uint8_t  reserve;
        uint16_t and_mask;             ///< Indicates that this pattern matcher matches if its own matching conditions match AND the pattern matchers which have their bits set to 1 in and_mask also match.
        uint16_t cmp6;                 ///< Compare value 6
        uint16_t cmp7;                 ///< Compare value 7
    } lookup_mode;

    /* Mode3: 2-byte range match at set offset */
    /* Mode4: 2-byte range not-match at set offset */
    struct
    {
        uint16_t min;                  ///< Minimum length to compare the 2 bytes at the offset.
        uint16_t max;                  ///< Maximum length to compare the 2 bytes at the offset.
        uint16_t reserve_1;
        uint16_t reserve_2;
        uint16_t reserve_3;
        uint16_t reserve_4;
        uint16_t mask;                 ///< Masks bits
        uint16_t length_type;          ///< Value to compare against the frame's Length/Type when MATCH_LT is set in the config register.
        uint8_t  offset;               ///< Byte offset to do the match after the MAC source address of the frame.
        uint8_t  reserve_5;
        uint16_t and_mask;             ///< Indicates that this pattern matcher matches if its own matching conditions match AND the pattern matchers which have their bits set to 1 in and_mask also match.
        uint16_t reserve_6;
        uint16_t reserve_7;
    } range_mode;

    /* register */
    struct
    {
        uint32_t cmp_30;               ///< Value to set in PTRN_CMP_30
        uint32_t cmp_74;               ///< Value to set in PTRN_CMP_74
        uint32_t cmp_118;              ///< Value to set in PTRN_CMP_118
        uint32_t msk_30;               ///< Value to set in PTRN_MSK_30
        uint32_t msk_74;               ///< Value to set in PTRN_MSK_74
        uint32_t msk_118;              ///< Value to set in PTRN_MSK_118
    } reg;
} ethsw_rx_pattern_data_t;

/** The parameter for set/get the Rx pattern matcher function */
typedef struct st_ethsw_rx_pattern_matcher
{
    uint16_t          pattern_sel;            ///< Pattern matcher number (0..11)
    ethsw_port_mask_t pattern_port_mask;      ///< Port mask
    bool              pattern_int;            ///< When true, called back from ISR

    ethsw_rx_pattern_ctrl_t * p_pattern_ctrl; ///< RX Pattern Matcher Function Control
    ethsw_rx_pattern_data_t * p_pattern_data; ///< RX Pattern Matcher Function Data
} ethsw_rx_pattern_matcher_t;

/** Callback event of the Rx pattern matcher */
typedef enum e_ethsw_rx_pattern_event
{
    ETHSW_RX_PATTERN_MATCHER,          ///< Rx pattern matcher event
} ethsw_rx_pattern_event_t;

/** Callback parameter of the Rx pattern matcher */
typedef struct st_ethsw_rx_pattern_event_data
{
    uint16_t match_int;                ///< Pattern mask to indicate pattern match
    uint16_t error_int;                ///< Error for each port
} ethsw_rx_pattern_event_data_t;

/** The parameter for set preemptable queue */
typedef struct
{
    union
    {
        uint8_t preempt_queues;        ///< Per-queue enable bit to configure which queues are used for preemptable traffic. Set to 1 the corresponding bit to configure a queue to be preemptable. The express queues always have priority over preemptable queues.
        struct
        {
            uint8_t q0 : 1;            ///< queue0
            uint8_t q1 : 1;            ///< queue1
            uint8_t q2 : 1;            ///< queue2
            uint8_t q3 : 1;            ///< queue3
            uint8_t q4 : 1;            ///< queue4
            uint8_t q5 : 1;            ///< queue5
            uint8_t q6 : 1;            ///< queue6
            uint8_t q7 : 1;            ///< queue7
        } preempt_queues_b;
    };
    union
    {
        uint8_t preempt_on_qclose;     ///< Per-queue configuration bit to enable preempting a frame when the queue goes from OPENto CLOSED. When the corresponding bit is set to 1 and the queue is configured as preemptable in PREEMPT_ENA, a queue close event causes the current frame to be preempted, if preemption is operational.
        struct
        {
            uint8_t q0 : 1;            ///< queue0
            uint8_t q1 : 1;            ///< queue1
            uint8_t q2 : 1;            ///< queue2
            uint8_t q3 : 1;            ///< queue3
            uint8_t q4 : 1;            ///< queue4
            uint8_t q5 : 1;            ///< queue5
            uint8_t q6 : 1;            ///< queue6
            uint8_t q7 : 1;            ///< queue7
        } preempt_on_qclose_b;
    };
} ethsw_preempt_queue_t;

/** The parameter for set frame configuration of frame preemption for each ports */
typedef struct
{
    bool    verify_dis;                ///< When set to 1, disables the verify process required for preemption operation. When PREEMPT_ENA is set to 1, preemption is not operational until the verify process validates that the peer port is also capable of performing frame preemption. Setting VERIFY_DIS disables this process and allows frame preemption without running the verify process.
    bool    response_dis;              ///< When set to 1 prevents the MAC from responding to "verify" frames. "verify" frames are always replied to unless PREEMPT_ENA is set to 0 and RESPONSE_DIS is set to 1. This bit must be kept at 0 to be compliant with 802.3br.
    uint8_t addfragsize;               ///< Minimum fragment size in increments of 64 bytes. Sets the minimum mPacket size when preempting a Preemptable frame. The default value of 0 corresponds to 64 bytes, a value of 1 corresponds to 128 bytes, and so on.
    uint8_t tx_verify_time;            ///< Preemption verification timeout in milliseconds. When preemption is enabled and verification is running, this is the timeout to wait for a RESPONSE frame after sending a VERIFY frames +/? 1 ms. VERIFY frames are sent after the timeout up to two more times. Default value is 10 ms. When the value is set to less than 5 ms, it does not meet the 20% precision required by the 802.3br, so it is recommended to use larger times.
    bool    rx_strict_pre;             ///< When set to 1, the preamble is checked so all bytes except the SFD are 0x55. When set to 0, only the last 2 bytes of the preamble are checked (SFD/SMD and FRAG_COUNT). It is recommended to set this bit to 1 to comply with the 802.3br specification. This bit must be set to 0 if only non-802.3br traffic is expected (for example, normal Ethernet traffic) and if custom preamble is used.
    bool    rx_br_smd_dis;             ///< When set to 1, the receiver does not decode the 802.3br SMDs and assumes all frames areexpress frames. This bit must be set to 0 for correct operation with 802.3br, and can be set to 1 when 802.3br is not enabled to avoid false detection of SMDs. Setting this bit to 1 also causes any preempted frame to be aborted, bringing the re- assembly state machine to IDLE.
    bool    rx_strict_br_ctl;          ///< When set to 1, strict checking of VERIFY and RESPONSE frames is enabled. When set to 1, the frame contents and frame length checks are also performed on these frames. The mCRC is always checked regardless of the value of this register. This bit must be set to 0 to be compliant with the functionality described in IEEE 802.3br.
    bool    tx_mcrc_inv;               ///< When set to 1, the 32-bit XOR mask used to calculate the mCRC for transmitted frames is inverted. This bit must always be written to 0 and only used for debugging.
    bool    rx_mcrc_inv;               ///< When set to 1, the 32-bit XOR mask used to calculate the mCRC for received frames is inverted. This bit must always be written to 0 and only used for debugging.
} ethsw_preempt_port_ctrl_config_t;

/** Port status of verification for frame preemption */
typedef enum
{
    ETHSW_PREEMPT_VERIFY_STATUS_UNKNOWN = 0, ///< Unknown (during reset)
    ETHSW_PREEMPT_VERIFY_STATUS_INITIAL,     ///< Initial (when preemption is disabled)
    ETHSW_PREEMPT_VERIFY_STATUS_VERIFYING,   ///< Verifying
    ETHSW_PREEMPT_VERIFY_STATUS_SUCCEEDED,   ///< Succeeded
    ETHSW_PREEMPT_VERIFY_STATUS_FAILED,      ///< Failed
    ETHSW_PREEMPT_VERIFY_STATUS_DISABLED     ///< Disabled (if preempt verification is disabled)
} ethsw_preempt_verify_status_t;

/** The parameter for get preemption status */
typedef struct
{
    ethsw_preempt_verify_status_t verify_status;
    bool    preempt_state;             ///< A per-port bit that indicates if a port is in a preempted state. This is a real-time indication meant for debugging.
    bool    hold_req_state;            ///< A per-port bit that indicates if a port is preempted using MM_CTL.request (hold_req). This isa real-time indication meant for debugging.
    uint8_t min_pfrm_adj;              ///< Adjust the minimum mPacket length, in increments of 4 bytes.
    uint8_t last_pfrm_adj;             ///< Adjust the preemptable threshold when reaching the end of the frame, in increments of 4 bytes. Incrementing this value increments the length of the last mPacket.
} ethsw_preempt_status_t;

/** queue gate action */
typedef enum e_ethsw_mmctl_qgate_action
{
    ETHSW_MMCTL_QGATE_NOT_CHANGE = 0,
    ETHSW_MMCTL_QGATE_TOGGLE     = 1,
    ETHSW_MMCTL_QGATE_CLOSE      = 2,
    ETHSW_MMCTL_QGATE_OPEN       = 3,
} ethsw_mmctl_qgate_action_t;

/** queue gate */
typedef struct st_ethsw_mmctl_qgate
{
    ethsw_port_mask_t          port_mask;                 ///< Per-port bit mask
    ethsw_mmctl_qgate_action_t action[ETHSW_QUEUE_COUNT]; ///< Gate action for queue number 0..7
} ethsw_mmclt_qgate_t;

/** MMCTL pool ID. */
typedef enum e_ethsw_mmctl_pool_id
{
    ETHSW_MMCTL_POOL_ID_0      = 0,    ///< Pool 0
    ETHSW_MMCTL_POOL_ID_1      = 1,    ///< Pool 1
    ETHSW_MMCTL_POOL_ID_GLOBAL = 8,    ///< Global Pool
} ethsw_mmctl_pool_id_t;

/** The parameter for set/get memory pool size function  */
typedef struct st_ethsw_mmctl_pool_size
{
    ethsw_mmctl_pool_id_t pool_id;     ///< MMCTL pool ID.
    uint16_t              cells;       ///< Memory pool size, in sells.
} ethsw_mmctl_pool_size_t;

/** The parameter for function that assins a memory pool for a queue. */
typedef struct st_ethsw_mmctl_queue_assign
{
    uint8_t               queue_num;   ///< Queue number
    ethsw_mmctl_pool_id_t pool_id;     ///< MMCTL pool ID.
} ethsw_mmctl_queue_assign_t;

/** The parameter for set / get length in bytes of the YELLOW period. */
typedef struct st_ethsw_yellow_length
{
    bool     enable;                   ///< When true, length is valid.
    uint16_t length;                   ///< Specify the byte length of the YELLOW period in units of 4 bytes.
} ethsw_yellow_length_t;

/** The action of queue flush event. */
typedef enum e_ethsw_queu_flush_action
{
    ETHSW_QUEUE_FLUSH_DISABLE = 0,           ///< Disable flush
    ETHSW_QUEUE_FLUSH_WHEN_QUEUE_IS_CLOSING, ///< Flush when queue is closed, discarding any new frames
    ETHSW_QUEUE_FLUSH_WHEN_QUEUE_CLOSE,      ///< Flush on queue close until empty, then stops flushing
    ETHSW_QUEUE_FLUSH_TRIGGER,               ///< TRIGGER flush until empty, then return to current flush mode
} ethsw_queu_flush_action_t;

/** The parameter for set / get queue flush event. */
typedef struct st_ethsw_queue_flush_event
{
    ethsw_port_mask_t         port_mask;  ///< Per-port bit mask
    uint8_t                   queue_mask; ///< 1 bit per queue indicating for which queues of the ports indicated by port_mask the flush state is changed as indicated in action.
    ethsw_queu_flush_action_t action;     ///< Selects the flush state for the queues indicated by queue_mask in the ports indicated by port_mask.
} ethsw_queue_flush_event_t;

/** The parameter for QclosedNonempty status */
typedef uint8_t ethsw_mmctl_qclosed_nonempty_t[4];

/* Switch Base Statistics counter */
typedef struct st_ethsw_statistic_switch_base
{
    uint32_t total_disc;               ///< Discarded Frame Total Number
    uint32_t total_byt_disc;           ///< Discarded Frame Total Bytes
    uint32_t total_frame;              ///< Processed Frame Total Number
    uint32_t total_byt_frame;          ///< Processed Frame Total Bytes
} ethsw_statistics_switch_base_t;

/* MAC Statistics counter */
typedef struct st_ethsw_statistic_mac
{
    uint64_t a_frames_transmitted_ok;              ///< MAC Transmitted Valid Frame Count
    uint64_t a_frames_received_ok;                 ///< MAC Received Valid Frame Count
    uint64_t a_frame_check_sequence_errors;        ///< MAC FCS Error Frame Count
    uint64_t a_alignment_errors;                   ///< MAC Alignment Error Frame Count
    uint64_t a_octets_transmitted_ok;              ///< MAC Transmitted Valid Frame Octets Count
    uint64_t a_octets_received_ok;                 ///< MAC Received Valid Frame Octets Count
    uint64_t a_tx_pause_mac_ctrl_frames;           ///< MAC Transmitted Pause Frame Count
    uint64_t a_rx_pause_mac_ctrl_frames;           ///< MAC Received Pause Frame Count
    uint64_t if_in_errors;                         ///< MAC Input Error Count
    uint64_t if_out_errors;                        ///< MAC Output Error Count
    uint64_t if_in_ucast_pkts;                     ///< MAC Received Unicast Frame Count
    uint64_t if_in_multicast_pkts;                 ///< MAC Received Multicast Frame Count
    uint64_t if_in_broadcast_pkts;                 ///< MAC Received Broadcast Frame Count
    uint64_t if_out_discards;                      ///< MAC Discarded Outbound Frame Count
    uint64_t if_out_ucast_pkts;                    ///< MAC Transmitted Unicast Frame Count
    uint64_t if_out_multicast_pkts;                ///< MAC Transmitted Multicast Frame Count
    uint64_t if_out_broadcast_pkts;                ///< MAC Transmitted Broadcast Frame Count
    uint64_t ether_stats_drop_events;              ///< MAC Dropped Frame Count
    uint64_t ether_stats_octets;                   ///< MAC All Frame Octets Count
    uint64_t ether_stats_pkts;                     ///< MAC All Frame Count
    uint64_t ether_stats_undersize_pkts;           ///< MAC Too Short Frame Count
    uint64_t ether_stats_oversize_pkts;            ///< MAC Too Long Frame Count
    uint64_t ether_stats_pkts_64_octets;           ///< MAC 64 Octets Frame Count
    uint64_t ether_stats_pkts_65_to_127_octets;    ///< MAC 65 to 127 Octets Frame Count
    uint64_t ether_stats_pkts_128_to_255_octets;   ///< MAC 128 to 255 Octets Frame Count
    uint64_t ether_stats_pkts_256_to_511_octets;   ///< MAC 256 to 511 Octets Frame Count
    uint64_t ether_stats_pkts_512_to_1023_octets;  ///< MAC 512 to 1023 Octets Frame Count
    uint64_t ether_stats_pkts_1024_to_1518_octets; ///< MAC 1024 to 1518 Octets Frame Count
    uint64_t ether_stats_pkts_1519_to_x_octets;    ///< MAC Over 1519 Octets Frame Count
    uint64_t ether_stats_jabbers;                  ///< MAC Jabbers Frame Count
    uint64_t ether_stats_fragments;                ///< MAC Fragment Frame Count
    uint64_t vlan_received_ok;                     ///< MAC Received VLAN Tagged Frame Count
    uint64_t vlan_transmitted_ok;                  ///< MAC Transmitted VLAN Tagged Frame Count
    uint64_t frames_retransmitted;                 ///< MAC Retransmitted Frame Count
    uint64_t a_deferred;                           ///< MAC Deferred Count
    uint64_t a_multiple_collisions;                ///< MAC Multiple Collision Count
    uint64_t a_single_collisions;                  ///< MAC Single Collision Count
    uint64_t a_late_collisions;                    ///< MAC Late Collision Count
    uint64_t a_excessive_collisions;               ///< MAC Excessive Collision Count
    uint64_t a_carriersense_errors;                ///< MAC Carrier Sense Error Count
} ethsw_statistics_mac_t;

#ifdef ETHSW_EFP_FEATURE_SUPPORTED

/* EFP Statistics counter */
typedef struct st_ethsw_statistics_efp
{
    uint32_t qci_stream_match_pkts[ETHSW_EFP_SID_COUNT];   ///< Qci Stream Match Packet Count (sid 0-7)
    uint32_t qci_msdu_passed_pkts[ETHSW_EFP_SID_COUNT];    ///< Qci MSDU Passed Packet Count (sid 0-7)
    uint32_t qci_msdu_reject_pkts[ETHSW_EFP_SID_COUNT];    ///< Qci MSDU Reject Packet Count (sid 0-7)
    uint32_t qci_gate_passed_pkts;                         ///< Qci Gate (All) Passed Packet Count
    uint32_t qci_gate_dropped_pkts[ETHSW_TDMA_GPIO_COUNT]; ///< Qci Gate g Dropped Packet Count (gid 0-7)
    uint32_t qci_mater_green_pkts[ETHSW_EFP_MEATER_COUNT]; ///< Qci Meter Green Packet Count (mid 0-7)
    uint32_t qci_mater_red_pkts[ETHSW_EFP_MEATER_COUNT];   ///< Qci Meter Red Packet Count (mid 0-7)
    uint32_t vlan_in_drop;                                 ///< VLAN Ingress Check Drop Frame Counter
    uint32_t lookup_hit;                                   ///< DST Address Lookup Hit Counter
} ethsw_statistics_efp_t;

#endif                                                     /* ETHSW_EFP_FEATURE_SUPPORTED */

/* Global Discard and 802.3br Statistics counter */
typedef struct st_ethsw_statistics_8023br
{
    uint32_t out_disc;                 ///< Discarded Outgoing Frame Count
    uint32_t in_disc_vlan;             ///< Discarded Incoming VLAN Tagged Frame Count
    uint32_t in_disc_untagged;         ///< Discarded Incoming VLAN Untagged Frame Count
    uint32_t in_disc_blocked;          ///< Discarded Incoming Blocked Frame Count
    uint32_t in_disc_any;              ///< Discarded Any Frame Count
    uint32_t in_disc_src_filter;       ///< Discarded Address Source Count

    uint32_t tx_hold_req;              ///< TX Hold Request Count
    uint32_t tx_frag;                  ///< TX for Preemption Count
    uint32_t rx_frag;                  ///< RX Continuation Count
    uint32_t rx_assy_ok;               ///< RX Preempted Frame Success Count

    uint16_t rx_assy_err;              ///< RX Preempted Frame Incorrect Count
    uint16_t rx_smd_err;               ///< RX SMD Frame Count

    uint8_t tx_verify_ok;              ///< TX VERIFY Frame Count
    uint8_t tx_response_ok;            ///< TX RESPONSE Frame Count
    uint8_t rx_verify_ok;              ///< RX VERIFY Frame Count
    uint8_t rx_response_ok;            ///< RX RESPONSE Frame Count
    uint8_t rx_verify_bad;             ///< RX Error VERIFY Frame Count
    uint8_t rx_response_bad;           ///< RX Error RESPONSE Count
} ethsw_statistics_8023br_t;

/* DLR Statistics counter */
typedef struct st_ethsw_statistics_dlr
{
    uint32_t rx_beacon_ok0;            ///< In port 0, Beacon Frames Received count
    uint32_t rx_beacon_err0;           ///< In port 0, Beacon Frames Received count with CRC Error
    uint32_t rx_disc_lf0;              ///< In port 0, Discarded frames count due to loop filtering
    uint32_t rx_beacon_ok1;            ///< In port 1, Beacon Frames Received count
    uint32_t rx_beacon_err1;           ///< In port 1, Beacon Frames Received count with CRC Error
    uint32_t rx_disc_lf1;              ///< In port 1, Discarded frames count due to loop filtering
} ethsw_statistics_dlr_t;

/** The parameter for set CQF enable configuration */
typedef struct
{
    union
    {
        uint8_t cqf_enable_priority;   ///< A per-queue enable to select which ingress priorities are queued in the two CQF queues.
        struct
        {
            uint8_t p0 : 1;            ///< priority0
            uint8_t p1 : 1;            ///< priority1
            uint8_t p2 : 1;            ///< priority2
            uint8_t p3 : 1;            ///< priority3
            uint8_t p4 : 1;            ///< priority4
            uint8_t p5 : 1;            ///< priority5
            uint8_t p6 : 1;            ///< priority6
            uint8_t p7 : 1;            ///< priority7
        } cqf_enable_priority_b;
    };
    uint8_t cqf_queue;                 ///< Select which two physical queues are used for CQF. The queues used are QUEUE_SEL0 and QUEUE_SEL0 + 1. Frames are written into QUEUE_SEL0 when the gate control selected with GATE_SEL0 is 0, and into QUEUE_SEL0 + 1 when the gate control is 1.
    uint8_t cqf_gate_sel;              ///< Select which gate control signal is used for selecting the output queue (these signals are the same as the ETHSW_TDMAOUT pins).
    bool    use_sop;                   ///< When set to 1, the CFQ queue is determined when the SOP is received at the frame writerin the memory controller. When set to 0, the queue is determined when the EOP is received at the frame writer.
    bool    cqf_gate_ref_sel;          ///< Select whether the gate control signal used for the CQF group is based on the egress port when set to 0, or the ingress port when set to 1.
} ethsw_cqf_enable_t;

/* Snoop offset type */
typedef enum e_ethsw_snoop_offset_type
{
    ETHSW_SNOOP_OFFS_ETHER1 = 0,       ///< The offset starts counting from the first byte of the MAC destination address.
    ETHSW_SNOOP_OFFS_ETHER2,           ///< The offset can either be specified starting after the MAC source address.
    ETHSW_SNOOP_OFFS_VLAN,             ///< The offset can either be specified starting starting after any optional VLAN tags.
    ETHSW_SNOOP_OFFS_IPDATA,           ///< The offset can either be specified starting starting after an IP header.
    ETHSW_SNOOP_OFFS_IPPROT,           ///< The offset is ignored. The compare value is compared with the protocol field located within the IP.
} ethsw_snoop_offset_type_t;

/* Snoop comparison type */
typedef enum e_ethsw_snoop_comp_type
{
    ETHSW_SNOOP_COMP_8 = 0,            ///< Applies a bitmask (AND) and compare single byte value.
    ETHSW_SNOOP_COMP_8OR,              ///< Compare two different single byte value.
    ETHSW_SNOOP_COMP_16,               ///< Compare a 16-bit value.
    ETHSW_SNOOP_COMP_16PLUS,           ///< Repeats the 16-bit comparison at offset + 2, if the 16-bit comparison at offset failed.
} ethsw_snoop_comp_type_t;

/** Snoop parser config */
typedef struct st_ethsw_snoop_parser_config
{
    uint8_t arith_block;                 ///< Arithmetic blkoc ID (0..1).
    uint8_t parser_id;                   ///< Parser ID (0..3).
    bool    enable;                      ///< true:enble, false:disable.
    ethsw_snoop_offset_type_t offs_type; ///< Snooping mode.
    ethsw_snoop_comp_type_t   comp_type; ///< How to handle matched frames.
    uint8_t offset;                      ///< An offset in bytes to locate the data for comparison within the frame.
    uint8_t comp_value;                  ///< The value to compare the frame data with at the given offset.
    uint8_t mask_value2;                 ///< When ETHSW_SNOOP_COMP_8, bitmask for single byte compares. When ETHSW_SNOOP_COMP_8OR, 2nd compare value. When ETHSW_SNOOP_COMP_16 or ETHSW_SNOOP_COMP_16PLUS, least significant bits of a 16-bit compare value.
} ethsw_snoop_parser_config_t;

/** Snooping actions */
typedef enum e_ethsw_snoop_action
{
    ETHSW_SNOOP_ACTION_DISABLE   = 0,  ///< Disabled, no snooping occurs (forward normally).
    ETHSW_SNOOP_ACTION_ONLY_MGMT = 1,  ///< Forward to management port only.
    ETHSW_SNOOP_ACTION_COPY_MGMT = 2,  ///< Forward normally and copy to management port.
    ETHSW_SNOOP_ACTION_DISCARD   = 3   ///< Discard the frame
} ethsw_snoop_action_t;

/* Operation of paraser result */
typedef enum e_ethsw_snoop_operat
{
    ETHSW_SNOOP_OPERAT_AND = 0,        ///< AND all selected inputs
    ETHSW_SNOOP_OPERAT_OR  = 1         ///< OR all selected inputs
} ethsw_snoop_operat_t;

/** Snoop arithmetic config */
typedef struct st_ethsw_snoop_arith_config
{
    uint8_t              arith_block;  ///< Arithmetic blkoc ID (0..1).
    ethsw_snoop_operat_t operat;       ///< Operation of paraser result
    ethsw_snoop_action_t action;       ///< Arithmetic action.
} ethsw_snoop_arith_config_t;

/** EEE */
typedef struct st_ethsw_eee
{
    bool     enable;                   ///< State of EEE auto mode (true:enabled/false:disabled)
    uint32_t idle_time;                ///< Idle time of EEE
    uint32_t wakeup_time;              ///< Wakeup time of EEE
} ethsw_eee_t;

/** QoS mode */
typedef struct st_ethsw_qos_mode
{
    bool    vlan_enable;               ///< VLAN Priority Enable
    bool    ip_enable;                 ///< IP Priority Enable
    bool    mac_enable;                ///< MAC Based Priority Enable
    bool    type_enable;               ///< TYPE Based Priority Enable
    uint8_t default_pri;               ///< Default Priority Enable Setting
} ethsw_qos_mode_t;

typedef struct st_ethsw_qos_prio_vlan
{
    uint32_t priority0 : 3;            ///< Priority 0 Setting
    uint32_t priority1 : 3;            ///< Priority 1 Setting
    uint32_t priority2 : 3;            ///< Priority 2 Setting
    uint32_t priority3 : 3;            ///< Priority 3 Setting
    uint32_t priority4 : 3;            ///< Priority 4 Setting
    uint32_t priority5 : 3;            ///< Priority 5 Setting
    uint32_t priority6 : 3;            ///< Priority 6 Setting
    uint32_t priority7 : 3;            ///< Priority 7 Setting
    uint32_t           : 8;            ///< Reserved
} ethsw_qos_prio_vlan_t;

/** QoS IP priority */
typedef struct st_ethsw_qos_prio_ip
{
    uint8_t diffserv;                  ///< The DiffServ field of the IP packet
    uint8_t priority;                  ///< The priority to assign.
} ethsw_qos_prio_ip_t;

/** QoS Ethertype priority */
typedef struct st_ethsw_qos_prio_type
{
    uint16_t type1;                    ///< The first type to match against. A value of 0 disables that match.
    uint8_t  prio1;                    ///< The priority to be assigned for the first match.
    uint16_t type2;                    ///< The second type to match against. A value of 0 disables that match.
    uint8_t  prio2;                    ///< The priority to be assigned for the second match.
} ethsw_qos_prio_type_t;

/** port mirroring modes */
typedef enum e_ethsw_mirr_mode
{
    ETHSW_MIRR_MODE_DISABLE,           ///< Disable
    ETHSW_MIRR_MODE_EGRESS_DA,         ///< Enable, Egress DA match
    ETHSW_MIRR_MODE_EGRESS_SA,         ///< Enable, Egress SA match
    ETHSW_MIRR_MODE_INGRESS_DA,        ///< Enable, Ingress DA match
    ETHSW_MIRR_MODE_INGRESS_SA,        ///< Enable, Ingress SA match
    ETHSW_MIRR_MODE_INGRESS_PORT,      ///< Enable, Ingress port match
    ETHSW_MIRR_MODE_EGRESS_PORT        ///< Enable, Egress port match
} ethsw_mirr_mode_t;

/** mirror port config */
typedef struct st_ethsw_mirror_conf
{
    uint32_t           mirror_port;    ///< The port id that will received all mirrored frames.
    ethsw_mirr_mode_t  mirror_mode;    ///< The mode of mirroring to configure.
    ethsw_port_mask_t  port_map;       ///< The port map to which the mirror configuration will be applied to.
    ethsw_mac_addr_t * mac_addr;       ///< MAC address pointer
} ethsw_mirror_conf_t;

/** The parameter for set/get pulse generator. */
typedef struct
{
    uint8_t  pulse_num;                ///< Pulse generator number (0..3)
    bool     enable;                   ///< When true, enable
    uint32_t start_sec;                ///< Pulse start second
    uint32_t start_ns;                 ///< Pulse start nanosecond
    uint16_t wide;                     ///< Pulse width
    uint32_t period_sec;               ///< Pulse period second
    uint32_t period_ns;                ///< Pulse period anosecond
} ethsw_ts_pulse_generator_t;

/** VLAN input processing modes */
typedef enum e_ethsw_vlan_in_mode
{
    ETHSW_VLANIN_PASSTHROUGH_OVERRIDE, ///< Single Tagging with Passthrough/VID Overwrite
    ETHSW_VLANIN_REPLACE,              ///< Single Tagging with Replace
    ETHSW_VLANIN_TAG_ALWAYS,           ///< Tag always
    ETHSW_VLANIN_DISABLE = 0xFF        ///< Disable VLAN
} ethsw_vlan_in_mode_t;

/** VLAN output processing mode */
typedef enum e_ethsw_vlan_out_mode
{
    ETHSW_VLANOUT_DISABLE,             ///< No manipulation
    ETHSW_VLANOUT_STRIP,               ///< Strip mode
    ETHSW_VLANOUT_TAGTHROUGH,          ///< Tag Thru mode
    ETHSW_VLANOUT_DOMAINTRANSP         ///< Domain / Transparent mode
} ethsw_vlan_out_mode_t;

/** tdma callback event */
typedef enum e_ethsw_tdma_event
{
    ETHSW_TDMA_CALLBACK_TCV_INT      = 0x100, ///< Active event of TCV sequence entry
    ETHSW_TDMA_CALLBACK_COUNTER1_INT = 0x101, ///< Active event of TDMA count1
} ethsw_tdma_event_t;

/** tdma callback data */
typedef union
{
    uint16_t tcv_s_idx;                ///< index of TCV sequence entry (valu of TDMA_IRQ_STAT_ACK_b.TCV_IDX_ACK)
} ethsw_tdma_callback_data_t;

/** tdma enable parameter */
typedef struct st_ethsw_tdma_enable
{
    bool              enable;          ///< True is enable, false:disabel.
    uint8_t           time_num;        ///< Timer number to use as TDMA time source.
    ethsw_port_mask_t port_mask;       ///< Port mask that TDMA operat.
    uint32_t          tdma_start;      ///< Start nano time for the first cycle of TDMA
    uint32_t          tdma_modulo;     ///< System timer modulo for 1 second.
    uint32_t          tdma_cycle;      ///< Periodic cycle time for TDMA scheduler

    void (* p_tdma_callback)(          ///< Pointer of callback function
        ethsw_tdma_event_t           event,
        ethsw_tdma_callback_data_t * p_tdma_data);
} ethsw_tdma_enable_t;

/** TDMA schedule entry parameter */
typedef struct st_ethsw_tdma_schedule_entry
{
    uint32_t          time_offset;     ///< Time offset from the TDMA Cycle Start
    ethsw_port_mask_t port_mask;       ///< Bit mask (one per output port) that controls which queues of the ports are gated, triggered, hold request generation, and which ports change their Cut-Through mode setting.
    uint8_t           gate_state;      ///< Bit mask, gate state of queue 0 to 7 (bit0:queue0, bit1:queue1, .., bit7:queue7). Value 0 is close, value 1 is open.
    uint8_t           gpio_mask;       ///< Generic bits that control the output pins ETHSW_TDMAOUT0..3.
    bool              interrupt;       ///< Indicates this entry generates an interrupt to the CPU when activated.
    bool              hold_req;        ///< Preemption hold request. Generates a hold request to ports enabled in port_mask.
    bool              gate_mode;       ///< Gate mode enable when set to 1.
    bool              trigger_mode;    ///< Trigger mode enable when set to 1. GATE_MODE must be 0, otherwise, GATE_MODE has precedence.
    bool              in_ct_ena;       ///< Input Cut-Through Enable.
    bool              out_ct_ena;      ///< Output Cut-Through Enable.
    bool              red_period;      ///< Period Color Control (for Profinet IRT)
    bool              inc_crt1;        ///< Increment Control for Counter 1.
    bool              inc_crt0;        ///< Increment Control for Counter 0.
} ethsw_tdma_schedule_entry_t;

/** TDMA GPIO operating mode */
typedef enum e_ethsw_tdma_gpio_mode
{
    ETHSW_TDMA_GPIO_MODE_LEVEL  = 0,   ///< level mode
    ETHSW_TDMA_GPIO_MODE_STROBE = 1,   ///< strobe mode
    ETHSW_TDMA_GPIO_MODE_TOGGLE = 2,   ///< toggle mode
} ethsw_tdma_gpio_mode_t;

/** The parameter for set/get TDMA counter 1 function */
typedef struct st_ethsw_tdma_counter1
{
    bool    write_value;               ///< When true, set value to counter 1
    uint8_t value;                     ///< Setted counter 1 value
    uint8_t max_value;                 ///< Maximum value of counter 1
    bool    int_enable;                ///< When true, enable counter 1 interrupt
    uint8_t int_value;                 ///< Counter 1 value when the interrupt occurred
} ethsw_tdma_counter1_t;

/** Timer enable/disable parameter */
typedef struct st_ethsw_time_enable
{
    uint8_t time_num;                  ///< Timer number (range is 0..1).
    bool    enable;                    ///< Enables/disables the timer specified by time_num. (true is enable, false is disable)
    uint8_t clock_period;              ///< Clock period (specify 8).
} ethsw_time_enable_t;

/** Timer callback event */
typedef enum e_ethsw_time_event
{
    ETHSW_TIME_CALLBACK_TIMESTAMP = 0x10,
} ethsw_time_event_t;

/** Timestamp parameter */
typedef struct st_ethsw_timestamp
{
    uint8_t  time_num;                 ///< Timer number
    uint32_t time_sec;                 ///< Second
    uint32_t time_nsec;                ///< Nanosecond
    uint32_t timestamp_id;             ///< Timeatamp ID
} ethsw_timestamp_t;

/** Transmit timestamp parameter */
typedef struct st_ethsw_time_transmit
{
    ethsw_port_mask_t port_mask;          ///< Per Port Transmit Timestamp Capture Interrupt Enable.
    ///< (Bit0=Port0, Bit1=Port1, Bit2=Port2)
    void (* p_time_callback)(             ///< Pointer to the callback function that receives the
        ethsw_time_event_t  event,        ///< Callback event
        uint32_t            port,         ///< Port number
        ethsw_timestamp_t * p_timestamp); ///< Pointer to the timestamp parameter.
} ethsw_time_transmit_t;

/** Peer delay info */
typedef struct st_ethsw_time_peerdelay
{
    uint8_t  time_num;                 ///< Timer number
    uint32_t peerdelay;                ///< Peer delay value
} ethsw_time_peerdelay_t;

/** offset correction parameter */
typedef struct st_ethsw_time_offset_correction
{
    uint8_t  time_num;                 ///< Timer number (0 or 1)
    uint8_t  offs_inc;                 ///< Offset correction increment
    uint32_t offs_corr;                ///< Offset correction counter
    uint32_t offset;                   ///< Offset correction value
} ethsw_time_offset_correction_t;

/** Rate correction parameter */
typedef struct st_ethsw_time_rate_correction
{
    uint8_t time_num;                  ///< timer number (0 or 1)
    int32_t rate;                      ///< Correction rate
} ethsw_time_rate_correction_t;

/* Domain parameter */
typedef struct st_ethsw_time_domain
{
    uint8_t time_num;                  ///< Timer number
    uint8_t domain_num;                ///< Domain number
} ethsw_time_domain_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const ether_switch_api_t g_ether_switch_on_ethsw;

/** @endcond */

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_Open(ether_switch_ctrl_t * const p_ctrl, ether_switch_cfg_t const * const p_cfg);
fsp_err_t R_ETHSW_Close(ether_switch_ctrl_t * const p_ctrl);

fsp_err_t R_ETHSW_SpeedCfg(ether_switch_ctrl_t * const p_ctrl, uint32_t const port, ethsw_link_speed_t const speed);

/* Forwarding extension API function */
fsp_err_t R_ETHSW_MacTableSet(ether_switch_ctrl_t * const    p_ctrl,
                              ethsw_mac_table_entry_addr_t * p_mac_entry_addr,
                              ethsw_mac_table_entry_info_t * p_mac_entry_info);
fsp_err_t R_ETHSW_MacTableGet(ether_switch_ctrl_t * const    p_ctrl,
                              ethsw_mac_table_entry_addr_t * p_mac_entry_addr,
                              ethsw_mac_table_entry_info_t * p_mac_entry_info);
fsp_err_t R_ETHSW_MacTableConfigSet(ether_switch_ctrl_t * const p_ctrl, ethsw_mac_table_config_t * p_mac_table_config);
fsp_err_t R_ETHSW_MacTableClear(ether_switch_ctrl_t * const p_ctrl, ethsw_mac_table_clear_mode_t * p_mac_table_clear);
fsp_err_t R_ETHSW_LearningSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable);
fsp_err_t R_ETHSW_PortForwardAdd(ether_switch_ctrl_t * const p_ctrl, uint32_t port);
fsp_err_t R_ETHSW_PortForwardDel(ether_switch_ctrl_t * const p_ctrl, uint32_t port);
fsp_err_t R_ETHSW_FloodUnknownSet(ether_switch_ctrl_t * const p_ctrl, ethsw_flood_unknown_config_t * p_flood_config);

/* MAC extension API function */
fsp_err_t R_ETHSW_LinkStateGet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_link_status_t * p_state_link);
fsp_err_t R_ETHSW_FrameSizeMaxSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint32_t frame_size_max);

/* DLR extension API function */
fsp_err_t R_ETHSW_DlrInitSet(ether_switch_ctrl_t * const p_ctrl, ethsw_dlr_init_t * p_dlr_init);
fsp_err_t R_ETHSW_DlrUninitSet(ether_switch_ctrl_t * const p_ctrl);
fsp_err_t R_ETHSW_DlrEnableSet(ether_switch_ctrl_t * const p_ctrl);
fsp_err_t R_ETHSW_DlrDisableSet(ether_switch_ctrl_t * const p_ctrl);
fsp_err_t R_ETHSW_DlrBeaconStateGet(ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    ethsw_dlr_beacon_state_t  * p_beacon_state);
fsp_err_t R_ETHSW_DlrNodeStateGet(ether_switch_ctrl_t * const p_ctrl, ethsw_dlr_node_state_t * p_node_state);
fsp_err_t R_ETHSW_DlrSvIpGet(ether_switch_ctrl_t * const p_ctrl, uint32_t * p_ip_addr);
fsp_err_t R_ETHSW_DlrSvPriorityGet(ether_switch_ctrl_t * const p_ctrl, uint8_t * p_priority);
fsp_err_t R_ETHSW_DlrVlanGet(ether_switch_ctrl_t * const p_ctrl, uint16_t * p_vlan_info);
fsp_err_t R_ETHSW_DlrSvMacGet(ether_switch_ctrl_t * const p_ctrl, ethsw_mac_addr_t * p_addr_mac);

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

/* Extended Frame Parser (EFP) extension API function */
fsp_err_t R_ETHSW_EfpInitilizeAsiTable(ether_switch_ctrl_t * const p_ctrl, ethsw_port_mask_t port_mask);
fsp_err_t R_ETHSW_EfpAsiTableSet(ether_switch_ctrl_t * const p_ctrl, ethsw_efp_asi_t * p_efp_asi_entry);
fsp_err_t R_ETHSW_EfpAsiTableDelete(ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    ethsw_mac_addr_t          * dmaci,
                                    ethsw_vlan_vid_t          * vidi);
fsp_err_t R_ETHSW_EfpVlanVerificationModeSet(ether_switch_ctrl_t * const          p_ctrl,
                                             uint32_t                             port,
                                             ethsw_vlan_input_verification_mode_t vicm);
fsp_err_t R_ETHSW_EfpPriorityRegenerationSet(ether_switch_ctrl_t * const         p_ctrl,
                                             uint32_t                            port,
                                             ethsw_efp_priority_regeneration_t * p_pri_regen);
fsp_err_t R_ETHSW_EfpFilterTableSet(ether_switch_ctrl_t * const     p_ctrl,
                                    uint32_t                        port,
                                    uint32_t                        sid,
                                    ethsw_efp_qci_stream_filter_t * p_flt_entry);
fsp_err_t R_ETHSW_EfpQsfTableEnable(ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint32_t sid, bool qste);
fsp_err_t R_ETHSW_EfpGatingCheckSet(ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    uint32_t                    sid,
                                    ethsw_efp_gating_check_t  * p_gt_chk);
fsp_err_t R_ETHSW_EfpSDUmaxVerificationSet(ether_switch_ctrl_t * const        p_ctrl,
                                           uint32_t                           port,
                                           uint32_t                           sid,
                                           ethsw_efp_sdu_max_verification_t * p_sdumax);
fsp_err_t R_ETHSW_EfpFlowMeteringSet(ether_switch_ctrl_t * const p_ctrl,
                                     uint32_t                    port,
                                     uint32_t                    sid,
                                     ethsw_efp_flow_metering_t * p_meter);
fsp_err_t R_ETHSW_EfpInterruptEnable(ether_switch_ctrl_t * const    p_ctrl,
                                     uint32_t                       port,
                                     ethsw_efp_interrupt_source_t * p_conf,
                                     void (                       * p_efp_callback_func)(uint32_t,
                                                                                         ethsw_efp_event_t));
fsp_err_t R_ETHSW_EfpStatusGet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_efp_status_t * p_info);
fsp_err_t R_ETHSW_EfpChannelEnable(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable);

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

/* Rx Pattern Matcher extension API function */
fsp_err_t R_ETHSW_RxPatternMatcherSet(ether_switch_ctrl_t * const  p_ctrl,
                                      ethsw_rx_pattern_matcher_t * p_rx_pattern_matcher);
fsp_err_t R_ETHSW_RxPatternMatcherCallback(
    ether_switch_ctrl_t * const         p_ctrl,
    void (                            * p_rx_pattern_callback_func)(
        ethsw_rx_pattern_event_t        event,
        ethsw_rx_pattern_event_data_t * p_data));

/* Frame Preemption extension API function */
fsp_err_t R_ETHSW_PreemptQueueSet(ether_switch_ctrl_t * const p_ctrl, ethsw_preempt_queue_t * p_preempt_queue);
fsp_err_t R_ETHSW_PreemptPortControlConfigSet(ether_switch_ctrl_t * const        p_ctrl,
                                              uint32_t                           port,
                                              ethsw_preempt_port_ctrl_config_t * p_preempt_port_ctrl);
fsp_err_t R_ETHSW_PreemptPortControlEnableSet(ether_switch_ctrl_t * const p_ctrl,
                                              uint32_t                    port,
                                              bool                        frame_preempt_enable);
fsp_err_t R_ETHSW_PreemptHoldReqForceSet(ether_switch_ctrl_t * const p_ctrl,
                                         ethsw_port_mask_t           holdreq_request_port_mask);
fsp_err_t R_ETHSW_PreemptHoldReqReleaseSet(ether_switch_ctrl_t * const p_ctrl,
                                           ethsw_port_mask_t           holdreq_release_port_mask);
fsp_err_t R_ETHSW_PreemptStatusGet(ether_switch_ctrl_t * const p_ctrl,
                                   uint32_t                    port,
                                   ethsw_preempt_status_t    * p_preempt_status);

/* MMCTL extension API function */
fsp_err_t R_ETHSW_MmctlQgateSet(ether_switch_ctrl_t * const p_ctrl, ethsw_mmclt_qgate_t * p_mmctl_qgate);
fsp_err_t R_ETHSW_MmctlPoolSizeSet(ether_switch_ctrl_t * const p_ctrl, ethsw_mmctl_pool_size_t * p_pool_size);
fsp_err_t R_ETHSW_MmctlQueueAssignSet(ether_switch_ctrl_t * const p_ctrl, ethsw_mmctl_queue_assign_t * p_queue_assign);
fsp_err_t R_ETHSW_MmctlYellowLengthSet(ether_switch_ctrl_t * const p_ctrl,
                                       uint32_t                    port,
                                       ethsw_yellow_length_t     * p_yellow_length);
fsp_err_t R_ETHSW_QueueFlushEventSet(ether_switch_ctrl_t * const p_ctrl,
                                     ethsw_queue_flush_event_t * p_queue_flush_event);
fsp_err_t R_ETHSW_MmctlQueueClosedNonemptyStatusGet(ether_switch_ctrl_t * const      p_ctrl,
                                                    ethsw_mmctl_qclosed_nonempty_t * p_qclosed_nonempty);

/* Statistics extension API function */
fsp_err_t R_ETHSW_StatisticsSwitchGet(ether_switch_ctrl_t * const      p_ctrl,
                                      bool                             clear,
                                      ethsw_statistics_switch_base_t * p_statistics_switch);
fsp_err_t R_ETHSW_StatisticsMacGet(ether_switch_ctrl_t * const p_ctrl,
                                   uint32_t                    port,
                                   ethsw_statistics_mac_t    * p_statistics_mac);
fsp_err_t R_ETHSW_StatisticsMacClear(ether_switch_ctrl_t * const p_ctrl);

#ifdef ETHSW_EFP_FEATURE_SUPPORTED
fsp_err_t R_ETHSW_StatisticsEfpGet(ether_switch_ctrl_t * const p_ctrl,
                                   uint32_t                    port,
                                   ethsw_statistics_efp_t    * p_statistics_efp);

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */
fsp_err_t R_ETHSW_Statistics8023brGet(ether_switch_ctrl_t * const p_ctrl,
                                      uint32_t                    port,
                                      bool                        clear,
                                      ethsw_statistics_8023br_t * p_statistics_8023br);
fsp_err_t R_ETHSW_StatisticsDlrGet(ether_switch_ctrl_t * const p_ctrl, ethsw_statistics_dlr_t * p_statistics_dlr);

/* CQF extension API function */
fsp_err_t R_ETHSW_CqfEnableSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_cqf_enable_t * p_cqf_enable);

/* Snooping extension API function */
fsp_err_t R_ETHSW_SnoopParserSet(ether_switch_ctrl_t * const p_ctrl, ethsw_snoop_parser_config_t * p_parser_cnf);
fsp_err_t R_ETHSW_SnoopArithSet(ether_switch_ctrl_t * const p_ctrl, ethsw_snoop_arith_config_t * p_arith_cnf);

/* EEE extension API function */
fsp_err_t R_ETHSW_EeeSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_eee_t * p_cnf_eee);

/* Storm protection extension API function */
fsp_err_t R_ETHSW_StormTimeSet(ether_switch_ctrl_t * const p_ctrl, uint16_t storm_time);
fsp_err_t R_ETHSW_BcastLimitSet(ether_switch_ctrl_t * const p_ctrl, uint16_t storm_frames);
fsp_err_t R_ETHSW_McastLimitSet(ether_switch_ctrl_t * const p_ctrl, uint16_t storm_frames);
fsp_err_t R_ETHSW_TxRateSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, float rate);

/* QoS extension API function */
fsp_err_t R_ETHSW_QosModeSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_qos_mode_t * p_qos_mode);
fsp_err_t R_ETHSW_QosPrioValnSet(ether_switch_ctrl_t * const p_ctrl,
                                 uint32_t                    port,
                                 ethsw_qos_prio_vlan_t     * p_qos_prio_vlan);
fsp_err_t R_ETHSW_QosPrioIpSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_qos_prio_ip_t * p_qos_prio_ip);
fsp_err_t R_ETHSW_QosPrioTypeSet(ether_switch_ctrl_t * const p_ctrl, ethsw_qos_prio_type_t * p_qos_prio_ethtype);

/* Mirroring extension API function */
fsp_err_t R_ETHSW_MirrorSet(ether_switch_ctrl_t * const p_ctrl, ethsw_mirror_conf_t * p_mirror_conf);

/* Cut Through extension API function */
fsp_err_t R_ETHSW_CtEnableSet(ether_switch_ctrl_t * const p_ctrl, ethsw_port_mask_t port_mask);
fsp_err_t R_ETHSW_CtDelaySet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint32_t ct_delay);

/* Pulse Generator extension API function */
fsp_err_t R_ETHSW_PulseGeneratorInit(ether_switch_ctrl_t * const p_ctrl, uint32_t time_num);
fsp_err_t R_ETHSW_PulseGeneratorSet(ether_switch_ctrl_t * const p_ctrl, ethsw_ts_pulse_generator_t * p_pulse);

/* Bridge management extension API function */
fsp_err_t R_ETHSW_PortAuthSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool auth_state);
fsp_err_t R_ETHSW_PortCtrlDirSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool dir_state);
fsp_err_t R_ETHSW_PortEapolSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool eapol_state);
fsp_err_t R_ETHSW_BpduSet(ether_switch_ctrl_t * const p_ctrl, bool bpdu_state);

/* VLAN management extension API function */
fsp_err_t R_ETHSW_VlanDefaultSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint16_t vlan_id);
fsp_err_t R_ETHSW_VlanPortAdd(ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint16_t vlan_id);
fsp_err_t R_ETHSW_VlanPortRemove(ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint16_t vlan_id);
fsp_err_t R_ETHSW_VlanInModeSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_vlan_in_mode_t vlan_in_mode);
fsp_err_t R_ETHSW_VlanOutModeSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port,
                                 ethsw_vlan_out_mode_t vlan_out_mode);
fsp_err_t R_ETHSW_VlanVerifySet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable);
fsp_err_t R_ETHSW_VlanDiscardUnknownSet(ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable);

/* TDMA extension API function */
fsp_err_t R_ETHSW_TdmaEnableSet(ether_switch_ctrl_t * const p_ctrl, ethsw_tdma_enable_t * p_tdma_enable);
fsp_err_t R_ETHSW_TdmaScheduleSet(ether_switch_ctrl_t * const   p_ctrl,
                                  ethsw_tdma_schedule_entry_t * p_tdma_schedule_entry,
                                  uint16_t                      tdma_schedule_entry_count);
fsp_err_t R_ETHSW_TdmaGpioModeSet(ether_switch_ctrl_t * const p_ctrl, uint8_t gpio_num,
                                  ethsw_tdma_gpio_mode_t gpio_mode);
fsp_err_t R_ETHSW_TdmaCounter0Set(ether_switch_ctrl_t * const p_ctrl, uint32_t tdma_counter0);
fsp_err_t R_ETHSW_TdmaCounter0Get(ether_switch_ctrl_t * const p_ctrl, uint32_t * p_tdma_counter0);
fsp_err_t R_ETHSW_TdmaCounter1Set(ether_switch_ctrl_t * const p_ctrl, ethsw_tdma_counter1_t * p_tdma_counter1);
fsp_err_t R_ETHSW_TdmaCounter1Get(ether_switch_ctrl_t * const p_ctrl, ethsw_tdma_counter1_t * p_tdma_counter1);
fsp_err_t R_ETHSW_TdmaHoldReqClear(ether_switch_ctrl_t * const p_ctrl);

/* Timestamp extension API function */
fsp_err_t R_ETHSW_TimeEnableSet(ether_switch_ctrl_t * const p_ctrl, ethsw_time_enable_t * p_time_enable);
fsp_err_t R_ETHSW_TimeTransmitTimestampSet(ether_switch_ctrl_t * const p_ctrl, ethsw_time_transmit_t * p_time_transmit);
fsp_err_t R_ETHSW_TimeValueSet(ether_switch_ctrl_t * const p_ctrl, ethsw_timestamp_t * p_timestamp);
fsp_err_t R_ETHSW_TimeValueGet(ether_switch_ctrl_t * const p_ctrl, ethsw_timestamp_t * p_timestamp);
fsp_err_t R_ETHSW_TimeValueGetAll(ether_switch_ctrl_t * const p_ctrl,
                                  ethsw_timestamp_t         * p_timer0,
                                  ethsw_timestamp_t         * p_timer1);
fsp_err_t R_ETHSW_TimePeerDelaySet(ether_switch_ctrl_t * const p_ctrl,
                                   uint32_t                    port,
                                   ethsw_time_peerdelay_t    * p_peerdelay);
fsp_err_t R_ETHSW_TimeOffsetSet(ether_switch_ctrl_t * const p_ctrl, ethsw_time_offset_correction_t * p_offset);
fsp_err_t R_ETHSW_TimeRateSet(ether_switch_ctrl_t * const p_ctrl, ethsw_time_rate_correction_t * p_rate);
fsp_err_t R_ETHSW_TimeDomainSet(ether_switch_ctrl_t * const p_ctrl, ethsw_time_domain_t * p_domain);

/*******************************************************************************************************************//**
 * @} (end addtogroup ETHSW)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // R_ETHSW_H
