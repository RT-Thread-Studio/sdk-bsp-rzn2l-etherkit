/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/

#include <string.h>

#include "bsp_api.h"
#include "r_gmac.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

#ifndef GMAC_ERROR_RETURN

/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
 #define GMAC_ERROR_RETURN(a, err)    FSP_ERROR_RETURN((a), (err))
#endif

#define GMAC_ETHERC_REG_SIZE                           (0x400UL)
#define GMAC_ETHERC_EDMAC_REG_SIZE                     (0x100UL)

#define GMAC_ETHERC_INITIALIZATION_WAIT_CYCLE          (0x64UL)

/** "ETHE" in ASCII.  Used to determine if the control block is open. */
#define GMAC_OPEN                                      (0x45544845U)

/* Definition of the maximum of data that can be sent at one time in the Ethernet */
#define GMAC_MINIMUM_FRAME_SIZE                        (60U) /* Minimum number of transmitted data */

/* The number of byte for FCS in ethernet frame */
#define GMAC_FCS_SIZE                                  (4U)

/* Bit definitions of control/status member of Transmit Descriptor-0 */
#define GMAC_TDES0_OWN                                 (1UL << 31)      /* Own Bit */
#define GMAC_TDES0_IC                                  (1UL << 30)      /* Interrupt on Completion */
#define GMAC_TDES0_LS                                  (1UL << 29)      /* Last Segment */
#define GMAC_TDES0_FS                                  (1UL << 28)      /* First Segment */
#define GMAC_TDES0_DC                                  (1UL << 27)      /* Disable CRC */
#define GMAC_TDES0_DP                                  (1UL << 26)      /* Disable Pad */
#define GMAC_TDES0_TTSE                                (1UL << 25)      /* Transmit Timestamp Enable */
#define GMAC_TDES0_CIC_MASK                            (3UL << 22)      /* Checksum Insertion Control */
#define GMAC_TDES0_TER                                 (1UL << 21)      /* Transmit End of Ring */
#define GMAC_TDES0_TCH                                 (1UL << 20)      /* Second Address Chained */
#define GMAC_TDES0_TTSS                                (1UL << 17)      /* Transmit Timestamp Status */
#define GMAC_TDES0_IHE                                 (1UL << 16)      /* IP Header Error */
#define GMAC_TDES0_ES                                  (1UL << 15)      /* Error Summary */
#define GMAC_TDES0_JT                                  (1UL << 14)      /* Jabber Timeout */
#define GMAC_TDES0_FF                                  (1UL << 13)      /* Frame Flushed */
#define GMAC_TDES0_IPE                                 (1UL << 12)      /* IP Payload Error */
#define GMAC_TDES0_LOC                                 (1UL << 11)      /* Loss of Carrier */
#define GMAC_TDES0_NC                                  (1UL << 10)      /* No Carrier */
#define GMAC_TDES0_LC                                  (1UL << 9)       /* Late Collision */
#define GMAC_TDES0_EC                                  (1UL << 8)       /* Excessive Collision */
#define GMAC_TDES0_VF                                  (1UL << 7)       /* VLAN Frame */
#define GMAC_TDES0_CC_MASK                             (0xfUL << 3)     /* Collision Count (Status field) */
#define GMAC_TDES0_ED                                  (1UL << 2)       /* Excessive Deferral */
#define GMAC_TDES0_UF                                  (1UL << 1)       /* Underflow Error */
#define GMAC_TDES0_DB                                  (1UL << 0)       /* Deferred Bit */

/* Bit definitions of control/status member of Transmit Descriptor-1 */
#define GMAC_RDES1_TBS2_MASK                           (0x1fffUL << 16) /* Receive Buffer 2 Size - Mask */
#define GMAC_RDES1_TBS2_SHFIT                          16               /* Receive Buffer 2 Size - Shift */
#define GMAC_RDES1_TBS1_MASK                           (0x1fffUL << 0)  /* Receive Buffer 1 Size -  */
#define GMAC_RDES1_TBS1_SHFIT                          0                /* Receive Buffer 1 Size */

/* Bit definitions of control/status member of Receive Descriptor-0 */
#define GMAC_RDES0_OWN                                 (1UL << 31)      /* Own Bit */
#define GMAC_RDES0_AFM                                 (1UL << 30)      /* Destination Address Filter Fail */
#define GMAC_RDES0_FL_MASK                             (0x3fffUL << 16) /* Frame Length - Mask */
#define GMAC_RDES0_FL_SHIFT                            16               /* Frame Length - Shift */
#define GMAC_RDES0_ES                                  (1UL << 15)      /* Error Summary */
#define GMAC_RDES0_DE                                  (1UL << 14)      /* Descriptor Error */
#define GMAC_RDES0_SAF                                 (1UL << 13)      /* Source Address Filter Fail */
#define GMAC_RDES0_LE                                  (1UL << 12)      /* Length Error */
#define GMAC_RDES0_VLAN                                (1UL << 10)      /* VLAN Tag */
#define GMAC_RDES0_FS                                  (1UL << 9)       /* First Descriptor */
#define GMAC_RDES0_LS                                  (1UL << 8)       /* Last Descriptor */
#define GMAC_RDES0_TAGF                                (1UL << 7)       /* Timestamp Available or Giant Frame */
#define GMAC_RDES0_LC                                  (1UL << 6)       /* Late Collision */
#define GMAC_RDES0_FT                                  (1UL << 5)       /* Frame Type */
#define GMAC_RDES0_RWT                                 (1UL << 4)       /* Receive Watchdog Timeout */
#define GMAC_RDES0_RE                                  (1UL << 3)       /* Receive Error */
#define GMAC_RDES0_DBE                                 (1UL << 2)       /* Dribble Bit Error */
#define GMAC_RDES0_CE                                  (1UL << 1)       /* CRC Error */
#define GMAC_RDES0_ESA                                 (1UL << 0)       /* Extended Status Available */

/* Bit definitions of control/status member of Receive Descriptor-1 */
#define GMAC_RDES1_DIC                                 (1UL << 31)      /* Disable Interrupt on Completion */
#define GMAC_RDES1_RBS2_MASK                           (0x1fffUL << 16) /* Receive Buffer 2 Size - Mask */
#define GMAC_RDES1_RBS2_SHFIT                          16               /* Receive Buffer 2 Size - Shift */
#define GMAC_RDES1_RER                                 (1UL << 15)      /* Receive End of Ring */
#define GMAC_RDES1_RCH                                 (1UL << 14)      /* Second Address Chained */
#define GMAC_RDES1_RBS1_MASK                           (0x1fffUL << 0)  /* Receive Buffer 1 Size -  */
#define GMAC_RDES1_RBS1_SHFIT                          0                /* Receive Buffer 1 Size */

/* Bit definition of Interrupt Enable Register */
#define GMAC_INTERRUPT_ENABLE_NIE                      (1UL << 16)      /* Normal Interrupt Summary Enable */
#define GMAC_INTERRUPT_ENABLE_AIE                      (1UL << 15)      /* Abnormal Interrupt Summary Enable */
#define GMAC_INTERRUPT_ENABLE_ERE                      (1UL << 14)      /* Early Receive Interrupt Enable */
#define GMAC_INTERRUPT_ENABLE_FBE                      (1UL << 13)      /* Fatal Bus Error Enable */
#define GMAC_INTERRUPT_ENABLE_ETE                      (1UL << 10)      /* Early Transmit Interrupt Enable */
#define GMAC_INTERRUPT_ENABLE_RWE                      (1UL << 9)       /* Receive Watchdog Timeout Enable */
#define GMAC_INTERRUPT_ENABLE_RSE                      (1UL << 8)       /* Receive Stopped Enable */
#define GMAC_INTERRUPT_ENABLE_RUE                      (1UL << 7)       /* Receive Buffer Unavailable Enable */
#define GMAC_INTERRUPT_ENABLE_RIE                      (1UL << 6)       /* Receive Interrupt Enable */
#define GMAC_INTERRUPT_ENABLE_UNE                      (1UL << 5)       /* Underflow Interrupt Enable */
#define GMAC_INTERRUPT_ENABLE_OVE                      (1UL << 4)       /* Overflow Interrupt Enable */
#define GMAC_INTERRUPT_ENABLE_TJE                      (1UL << 3)       /* Transmit Jabber Timeout Enable */
#define GMAC_INTERRUPT_ENABLE_TUE                      (1UL << 2)       /* Transmit Buffer Unavailable Enable */
#define GMAC_INTERRUPT_ENABLE_TSE                      (1UL << 1)       /* Transmit Stopped Enable */
#define GMAC_INTERRUPT_ENABLE_TIE                      (1UL << 0)       /* Transmit Interrupt Enable */

/* Bit definition of MAC Frame Filter Register */
#define GMAC_MAC_FRAME_FILTER_RA                       (1UL << 31)      /* Receive All */
#define GMAC_MAC_FRAME_FILTER_VTFE                     (1UL << 16)      /* VLAN Tag Filter Enable */
#define GMAC_MAC_FRAME_FILTER_HPF                      (1UL << 10)      /* Hash or Perfect Filter */
#define GMAC_MAC_FRAME_FILTER_SAF                      (1UL << 9)       /* Source Address Filter Enable */
#define GMAC_MAC_FRAME_FILTER_SAIF                     (1UL << 8)       /* SA Inverse Filtering */
#define GMAC_MAC_FRAME_FILTER_PCF_1                    (1UL << 7)       /* Pass Control Frames 1 */
#define GMAC_MAC_FRAME_FILTER_PCF_0                    (1UL << 6)       /* Pass Control Frames 0 */
#define GMAC_MAC_FRAME_FILTER_DBF                      (1UL << 5)       /* Disable Broadcast Frames */
#define GMAC_MAC_FRAME_FILTER_PM                       (1UL << 4)       /* Pass All Multicast */
#define GMAC_MAC_FRAME_FILTER_DAIF                     (1UL << 3)       /* DA Inverse Filtering */
#define GMAC_MAC_FRAME_FILTER_HMC                      (1UL << 2)       /* Hash Multicast */
#define GMAC_MAC_FRAME_FILTER_HUC                      (1UL << 1)       /* Hash Unicast */
#define GMAC_MAC_FRAME_FILTER_PR                       (1UL << 0)       /* Promiscuous Mode */

/* Bit definition of Flow Control Register */
#define GMAC_FLOW_CONTROL_PT_VALUE                     (0x0000FFFFUL)
#define GMAC_FLOW_CONTROL_PLT_VALUE                    (0x00000002UL)

#define GMAC_FLOW_CONTROL_PT                           (GMAC_FLOW_CONTROL_PT_VALUE << 16) /* Pause Time (b31-b16) */
#define GMAC_FLOW_CONTROL_DZPQ                         (1UL << 7)                         /* Disable Zero-Quanta Pause */
#define GMAC_FLOW_CONTROL_PLT                          (GMAC_FLOW_CONTROL_PLT_VALUE << 4) /* Pause Low Threshold */
#define GMAC_FLOW_CONTROL_UP                           (1UL << 3)                         /* Unicast Pause Frame Detect */
#define GMAC_FLOW_CONTROL_RFE                          (1UL << 2)                         /* Receive Flow Control Enable */
#define GMAC_FLOW_CONTROL_TFE                          (1UL << 1)                         /* Transmit Flow Control Enable */
#define GMAC_FLOW_CONTROL_FCA_BPA                      (1UL << 0)                         /* Flow Control Busy or Backpressure Activate */

/* Bit definition of Flow Control Register */
#define GMAC_PMT_CONTROL_STATUS_RWKFILTRST             (1UL << 31)                        /* Wake-Up Frame Filter Register Pointer Reset */
#define GMAC_PMT_CONTROL_STATUS_GLBLUCAST              (1UL << 9)                         /* Global Unicas */
#define GMAC_PMT_CONTROL_STATUS_RWKPRCVD               (1UL << 6)                         /* Wake-Up Frame Received */
#define GMAC_PMT_CONTROL_STATUS_MGKPRCVD               (1UL << 5)                         /* Magic Packet Received */
#define GMAC_PMT_CONTROL_STATUS_RWKPKTEN               (1UL << 2)                         /* Wake-Up Frame Enable */
#define GMAC_PMT_CONTROL_STATUS_MGKPKTEN               (1UL << 1)                         /* Magic Packet Enable */
#define GMAC_PMT_CONTROL_STATUS_PWRDWN                 (1UL << 0)                         /* Power Down */

/* Bit definition of MMC_Receive_Interrupt_Mask/MMC_Transmit_Interrupt_Mask Register */
#define GMAC_MMC_RECEIVE_INTERRUPT_MASK_ALL_BIT        0x03FFFFFF
#define GMAC_MMC_TRANSMIT_INTERRUPT_MASK_ALL_BIT       0x03FFFFFF

/* Number of entries in PAUSE resolution table */
#define GMAC_PAUSE_TABLE_ENTRIES                       (8)

/* Local device and link partner PAUSE settings */
#define GMAC_PAUSE_XMIT_OFF                            (0)     /* The pause frame transmission is prohibited. */
#define GMAC_PAUSE_RECV_OFF                            (0)     /* The pause frame reception is prohibited.    */
#define GMAC_PAUSE_XMIT_ON                             (1)     /* The pause frame transmission is permitted.  */
#define GMAC_PAUSE_RECV_ON                             (1)     /* The pause frame reception is permitted.     */

/* Macro definition for buffer alignment adjustment */
#define GMAC_BUFFER_4BYTE_ALIGNMENT_MASK               (0x3UL) /* Mask for checking whether or not 4-byte alignment. */

/* PAUSE link mask and shift values */

/*
 * The mask value and shift value which are for that shift the bits form a line and
 * for comparing the bit information of PAUSE function which support the local device and
 * Link partner with the assorted table(pause_resolution) which enable or disable the PAUSE frame.
 */
#define GMAC_LINK_RESOLUTION_ABILITY_MASK              (3)
#define GMAC_LINK_RESOLUTION_LOCAL_ABILITY_BITSHIFT    (2)

/* Etherc mode */
#define GMAC_NO_USE_MAGIC_PACKET_DETECT                (0)
#define GMAC_USE_MAGIC_PACKET_DETECT                   (1)

/* GMAC_NO_DATA is the return value that indicates that no received data. */
#define GMAC_NO_DATA                                   (0)

#define GMAC_8BIT_MASK                                 (0xff)

/* Define for TCM */
#define TCM_CPU_ADR_END                                (0x0FFFFFFF)
#define TCM_DMA_ADR_ST                                 (0x20000000)
#define TCM_DMA_ADR_END                                (0x2010FFFF)
#define TCM_DMA_ADR_OFFSET                             (TCM_DMA_ADR_ST)

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
static fsp_err_t gmac_open_param_check(gmac_instance_ctrl_t const * const p_instance_ctrl,
                                       ether_cfg_t const * const          p_cfg);

#endif                                 /* GMAC_CFG_PARAM_CHECKING_ENABLE */

static fsp_err_t gmac_read_param_check(gmac_instance_ctrl_t * p_instance_ctrl,
                                       void * const           p_buffer,
                                       uint32_t * const       length_bytes);

static fsp_err_t gmac_write_param_check(gmac_instance_ctrl_t * p_instance_ctrl,
                                        void * const           p_buffer,
                                        uint32_t const         frame_length);

static void gmac_enable_icu(gmac_instance_ctrl_t * const p_instance_ctrl);
static void gmac_disable_icu(gmac_instance_ctrl_t * const p_instance_ctrl);
static void gmac_reset_mac(volatile R_GMAC_Type * const p_reg);

#ifdef GMAC_IMPLEMENT_ETHSW
static fsp_err_t gmac_open_ethsw(gmac_instance_ctrl_t * p_instance_ctrl);

#endif                                 // GMAC_IMPLEMENT_ETHSW

static fsp_err_t gmac_open_phy(gmac_instance_ctrl_t * p_instance_ctrl);
static void      gmac_init_descriptors(gmac_instance_ctrl_t * const p_instance_ctrl);
static fsp_err_t gmac_buffer_get(gmac_instance_ctrl_t * const p_instance_ctrl,
                                 void ** const                p_buffer,
                                 uint32_t                   * p_buffer_size);
static void gmac_config_ethernet(gmac_instance_ctrl_t const * const p_instance_ctrl, const uint8_t mode);
static void gmac_pause_resolution(uint32_t const local_ability,
                                  uint32_t const partner_ability,
                                  uint32_t     * p_tx_pause,
                                  uint32_t     * p_rx_pause);
static void gmac_configure_mac(gmac_instance_ctrl_t * const p_instance_ctrl,
                               const uint8_t                mac_addr[],
                               const uint8_t                mode);
static void gmac_disable(gmac_instance_ctrl_t * const p_instance_ctrl);
static void gmac_call_user_callback(gmac_instance_ctrl_t * p_instance_ctrl,
                                    ether_event_t          event,
                                    gmac_port_mask_t       link_status);
static fsp_err_t          gmac_link_process(gmac_instance_ctrl_t * p_instance_ctrl, uint32_t port);
static gmac_link_change_t gmac_link_change_check(gmac_instance_ctrl_t * p_instance_ctrl, uint32_t port);
static fsp_err_t          gmac_do_link(gmac_instance_ctrl_t * const p_instance_ctrl, const uint32_t port);
static void               gmac_configure_operation(gmac_instance_ctrl_t * const p_instance_ctrl);
static void               gmac_magic_packet_detection(gmac_instance_ctrl_t * const p_instance_ctrl);

static fsp_err_t gmac_link_status_check(gmac_instance_ctrl_t const * const p_instance_ctrl, uint32_t port);
static uint8_t   gmac_check_magic_packet_detection_bit(gmac_instance_ctrl_t const * const p_instance_ctrl);

static void gmac_call_callback(gmac_instance_ctrl_t * p_instance_ctrl, ether_callback_args_t * p_callback_args);
void        gmac_isr_sbd(void);
void        gmac_isr_pmt(void);

#ifdef GMAC_IMPLEMENT_ETHSW
void gmac_callback_ethsw(ether_switch_callback_args_t * const p_arg);

#endif                                 // GMAC_IMPLEMENT_ETHSW

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "ether";
#endif

#if defined(__GNUC__)

/* This structure is affected by warnings from a GCC compiler bug. This pragma suppresses the warnings in this
 * structure only.*/

/*LDRA_INSPECTED 69 S */
 #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

/** ETHER HAL API mapping for Ethernet Controller interface */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const ether_api_t g_ether_on_gmac =
{
    .open            = R_GMAC_Open,
    .close           = R_GMAC_Close,
    .read            = R_GMAC_Read,
    .bufferRelease   = R_GMAC_BufferRelease,
    .rxBufferUpdate  = R_GMAC_RxBufferUpdate,
    .write           = R_GMAC_Write,
    .linkProcess     = R_GMAC_LinkProcess,
    .wakeOnLANEnable = R_GMAC_WakeOnLANEnable,
    .txStatusGet     = R_GMAC_TxStatusGet,
    .callbackSet     = R_GMAC_CallbackSet,
};

/*
 * PAUSE Resolution as documented in IEEE 802.3-2008_section2 Annex
 * 28B, Table 28B-3. The following table codify logic that
 * determines how the PAUSE is configured for local transmitter
 * and receiver and partner transmitter and receiver.
 */
static const gmac_pause_resolution_t pause_resolution[GMAC_PAUSE_TABLE_ENTRIES] =
{
    {GMAC_PAUSE_MASKC, GMAC_PAUSE_VAL0, GMAC_PAUSE_XMIT_OFF, GMAC_PAUSE_RECV_OFF  },
    {GMAC_PAUSE_MASKE, GMAC_PAUSE_VAL4, GMAC_PAUSE_XMIT_OFF, GMAC_PAUSE_RECV_OFF  },
    {GMAC_PAUSE_MASKF, GMAC_PAUSE_VAL6, GMAC_PAUSE_XMIT_OFF, GMAC_PAUSE_RECV_OFF  },
    {GMAC_PAUSE_MASKF, GMAC_PAUSE_VAL7, GMAC_PAUSE_XMIT_ON,  GMAC_PAUSE_RECV_OFF  },
    {GMAC_PAUSE_MASKE, GMAC_PAUSE_VAL8, GMAC_PAUSE_XMIT_OFF, GMAC_PAUSE_RECV_OFF  },
    {GMAC_PAUSE_MASKA, GMAC_PAUSE_VALA, GMAC_PAUSE_XMIT_ON,  GMAC_PAUSE_RECV_ON   },
    {GMAC_PAUSE_MASKF, GMAC_PAUSE_VALC, GMAC_PAUSE_XMIT_OFF, GMAC_PAUSE_RECV_OFF  },
    {GMAC_PAUSE_MASKF, GMAC_PAUSE_VALD, GMAC_PAUSE_XMIT_OFF, GMAC_PAUSE_RECV_ON   }
};

static const gmac_port_mask_t g_gmac_port_mask[BSP_FEATURE_GMAC_MAX_PORTS] =
{GMAC_PORT_MASK_0, GMAC_PORT_MASK_1, GMAC_PORT_MASK_2};

/*******************************************************************************************************************//**
 * @addtogroup ETHER
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/********************************************************************************************************************//**
 * After ETHERC, EDMAC and PHY-LSI are reset in software, an auto negotiation of PHY-LSI is begun.
 * Afterwards, the link signal change interrupt is permitted. Implements @ref ether_api_t::open.
 *
 * @retval  FSP_SUCCESS                             Channel opened successfully.
 * @retval  FSP_ERR_ASSERTION                       Pointer to ETHER control block or configuration structure is NULL.
 * @retval  FSP_ERR_ALREADY_OPEN                    Control block has already been opened or channel is being used by another
 *                                                  instance. Call close() then open() to reconfigure.
 * @retval  FSP_ERR_ETHER_ERROR_PHY_COMMUNICATION   Initialization of PHY-LSI failed.
 * @retval  FSP_ERR_INVALID_CHANNEL                 Invalid channel number is given.
 * @retval  FSP_ERR_INVALID_POINTER                 Pointer to MAC address is NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT                Interrupt is not enabled.
 * @retval  FSP_ERR_ETHER_PHY_ERROR_LINK            Initialization of PHY-LSI failed.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_Open (ether_ctrl_t * const p_ctrl, ether_cfg_t const * const p_cfg)
{
    fsp_err_t              err             = FSP_SUCCESS;
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    volatile R_GMAC_Type * p_reg_gmac      = (R_GMAC_Type *) R_GMAC_BASE;
    gmac_extend_cfg_t    * p_extend;
    uint32_t               i;

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)

    /** Check parameters. */
    err = gmac_open_param_check(p_instance_ctrl, p_cfg); /** check arguments */
    GMAC_ERROR_RETURN((FSP_SUCCESS == err), err);
#endif

    /** Make sure this channel exists. */
    p_instance_ctrl->p_reg_gmac = (void *) p_reg_gmac;

    /* Initialize configuration of Ethernet module. */
    p_instance_ctrl->p_gmac_cfg = p_cfg;

    /* Initialize the flags */
    p_instance_ctrl->magic_packet = GMAC_MAGIC_PACKET_NOT_DETECTED;

    /* Clear Link status */
    p_instance_ctrl->link_status           = (gmac_port_mask_t) 0;
    p_instance_ctrl->previous_link_status  = (gmac_port_mask_t) 0;
    p_instance_ctrl->link_establish_status = (gmac_port_mask_t) 0;

    /* Initialize the transmit and receive descriptor */
    p_extend = (gmac_extend_cfg_t *) p_cfg->p_extend;

    memset(p_extend->p_rx_descriptors,
           0x00,
           sizeof(gmac_instance_descriptor_t) *
           p_instance_ctrl->p_gmac_cfg->num_rx_descriptors);

    memset(p_extend->p_tx_descriptors,
           0x00,
           sizeof(gmac_instance_descriptor_t) *
           p_instance_ctrl->p_gmac_cfg->num_tx_descriptors);

    /* Initialize the Ether buffer */
    for (i = 0;
         i < (p_instance_ctrl->p_gmac_cfg->num_tx_descriptors + p_instance_ctrl->p_gmac_cfg->num_rx_descriptors);
         i++)
    {
        memset(p_instance_ctrl->p_gmac_cfg->pp_ether_buffers[i], 0x00, p_instance_ctrl->p_gmac_cfg->ether_buffer_size);
    }

    /* Set callback and context pointers, if configured */
    p_instance_ctrl->p_callback        = p_cfg->p_callback;
    p_instance_ctrl->p_context         = p_cfg->p_context;
    p_instance_ctrl->p_callback_memory = NULL;

    /* Module start */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_GMAC, p_cfg->channel);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_GMAC0_PCLKH);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_GMAC0_PCLKM);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

#ifdef GMAC_IMPLEMENT_ETHSW

    /* Open Ether-Switch Driver */
    err = gmac_open_ethsw(p_instance_ctrl);
#endif                                 // GMAC_IMPLEMENT_ETHSW

    if (FSP_SUCCESS == err)
    {
        /* Open Ether-Phy Driver */
        err = gmac_open_phy(p_instance_ctrl);

        if (FSP_ERR_ETHER_PHY_ERROR_LINK == err)
        {
            err = FSP_ERR_ETHER_ERROR_PHY_COMMUNICATION;
        }
    }

    if (FSP_SUCCESS == err)
    {
        /* Software reset */
        gmac_reset_mac(p_instance_ctrl->p_reg_gmac);

        p_reg_gmac->Interrupt_Mask_b.LPIIM = 1; /* LPI Interrupt Disable */
        p_reg_gmac->Interrupt_Mask_b.TSIM  = 1; /* Timestamo Interrupt Disable */
        p_reg_gmac->Interrupt_Mask_b.PMTIM = 1; /* PMT Interrupt Disable */

        gmac_enable_icu(p_instance_ctrl);
        p_instance_ctrl->open = GMAC_OPEN;
    }

    return err;
}                                      /* End of function R_GMAC_Open() */

/********************************************************************************************************************//**
 * Disables interrupts. Removes power and releases hardware lock. Implements @ref ether_api_t::close.
 *
 * @retval  FSP_SUCCESS                                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened
 *
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_Close (ether_ctrl_t * const p_ctrl)
{
    fsp_err_t               err = FSP_SUCCESS;
    uint32_t                i;
    ether_phy_instance_t ** pp_phy_instance;
    gmac_extend_cfg_t     * p_extend;

    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    volatile R_GMAC_Type * p_reg_gmac;

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    /* Disable Ethernet interrupt. */
    gmac_disable_icu(p_instance_ctrl);

    p_extend        = (gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend;
    pp_phy_instance = (ether_phy_instance_t **) p_extend->pp_phy_instance;

    for (i = 0; BSP_FEATURE_GMAC_MAX_PORTS > i; i++)
    {
        if (NULL != pp_phy_instance[i])
        {
            pp_phy_instance[i]->p_api->close(pp_phy_instance[i]->p_ctrl);
        }
    }

#ifdef GMAC_IMPLEMENT_ETHSW
    p_extend->p_ethsw_instance->p_api->close(p_extend->p_ethsw_instance->p_ctrl);
#endif                                 // GMAC_IMPLEMENT_ETHSW

    p_reg_gmac->Interrupt_Enable = 0;

    /* Disable TE and RE  */
    p_reg_gmac->MAC_Configuration_b.RE = 0;
    p_reg_gmac->MAC_Configuration_b.TE = 0;

    /* Initialize the flags */
    p_instance_ctrl->magic_packet = GMAC_MAGIC_PACKET_NOT_DETECTED;

    /* Clear Link status */
    p_instance_ctrl->link_status           = (gmac_port_mask_t) 0;
    p_instance_ctrl->previous_link_status  = (gmac_port_mask_t) 0;
    p_instance_ctrl->link_establish_status = (gmac_port_mask_t) 0;

    /** Clear configure block parameters. */
    p_instance_ctrl->p_gmac_cfg = NULL;

    /** Mark the channel not open so other APIs cannot use it. */
    p_instance_ctrl->open = 0U;

    return err;
}                                      /* End of function R_GMAC_Close() */

/********************************************************************************************************************//**
 * Move to the next buffer in the circular receive buffer list. Implements @ref ether_api_t::bufferRelease.
 *
 * @retval  FSP_SUCCESS                             Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                       Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                        The control block has not been opened
 * @retval  FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE   As a Magic Packet is being detected, transmission and reception is
 *                                                  not enabled.
 * @retval  FSP_ERR_ETHER_ERROR_LINK                Link is down.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_BufferRelease (ether_ctrl_t * const p_ctrl)
{
    fsp_err_t              err             = FSP_SUCCESS;
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    volatile R_GMAC_Type * p_reg_gmac;

    /* Check argument */
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* When the Link up processing is not completed, return error */
    GMAC_ERROR_RETURN(0 != p_instance_ctrl->previous_link_status, FSP_ERR_ETHER_ERROR_LINK);

    /* In case of detection mode of magic packet, return error. */
    GMAC_ERROR_RETURN(0 == gmac_check_magic_packet_detection_bit(p_instance_ctrl),
                      FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE);

    /* When receive data exists */
    if (GMAC_RDES0_OWN != (p_instance_ctrl->p_rx_descriptor->des0 & GMAC_RDES0_OWN))
    {
        /* Enable current descriptor */
        p_instance_ctrl->p_rx_descriptor->des0 |= GMAC_RDES0_OWN;

        /* Update pointer to the currently referenced receive descriptor  */
        if (GMAC_RDES1_RER == (p_instance_ctrl->p_rx_descriptor->des1 & GMAC_RDES1_RER))
        {
            p_instance_ctrl->p_rx_descriptor =
                ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_rx_descriptors;
        }
        else
        {
            p_instance_ctrl->p_rx_descriptor++;
        }
    }

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;
    p_reg_gmac->Receive_Poll_Demand_b.RPD = 0;

    err = FSP_SUCCESS;

    return err;
}                                      /* End of function R_GMAC_BufferRelease() */

/********************************************************************************************************************//**
 * Change the buffer pointer of the current rx buffer descriptor. Implements @ref ether_api_t::rxBufferUpdate.
 *
 * @retval  FSP_SUCCESS                             Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                       A pointer argument is NULL.
 * @retval  FSP_ERR_NOT_OPEN                        The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER                 The pointer of buffer is NULL or not aligned on a 32-bit boundary.
 * @retval  FSP_ERR_INVALID_MODE                    Driver is configured to non zero copy mode.
 * @retval  FSP_ERR_ETHER_RECEIVE_BUFFER_ACTIVE     All descriptor is active.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_RxBufferUpdate (ether_ctrl_t * const p_ctrl, void * const p_buffer)
{
    fsp_err_t              err             = FSP_SUCCESS;
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    volatile R_GMAC_Type * p_reg_gmac;

    /* Check argument */
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    GMAC_ERROR_RETURN(NULL != p_buffer, FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN(ETHER_ZEROCOPY_ENABLE == p_instance_ctrl->p_gmac_cfg->zerocopy, FSP_ERR_INVALID_MODE);
#endif

    /* When receive data exists */
    if (GMAC_RDES0_OWN != (p_instance_ctrl->p_rx_descriptor->des0 & GMAC_RDES0_OWN))
    {
        if (TCM_CPU_ADR_END >= (uint32_t) p_buffer)
        {
            /* Offset for DMA */
            p_instance_ctrl->p_rx_descriptor->des2 = (uint32_t) p_buffer + TCM_DMA_ADR_OFFSET;
        }
        else
        {
            p_instance_ctrl->p_rx_descriptor->des2 = (uint32_t) p_buffer;
        }

        /* Enable current descriptor */
        p_instance_ctrl->p_rx_descriptor->des0 |= GMAC_RDES0_OWN;

        /* Update pointer to the currently referenced receive descriptor  */
        if (GMAC_RDES1_RER == (p_instance_ctrl->p_rx_descriptor->des1 & GMAC_RDES1_RER))
        {
            p_instance_ctrl->p_rx_descriptor =
                ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_rx_descriptors;
        }
        else
        {
            p_instance_ctrl->p_rx_descriptor++;
        }

        p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;
        p_reg_gmac->Receive_Poll_Demand_b.RPD = 0;
    }
    else
    {
        err = FSP_ERR_ETHER_RECEIVE_BUFFER_ACTIVE;
    }

    return err;
}                                      /* End of function R_GMAC_RxBufferUpdate() */

/********************************************************************************************************************//**
 * The Link up processing, the Link down processing, and the magic packet detection processing are executed.
 * Implements @ref ether_api_t::linkProcess.
 *
 * @retval  FSP_SUCCESS                             Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                       Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                        The control block has not been opened.
 * @retval  FSP_ERR_ETHER_ERROR_PHY_COMMUNICATION   When reopening the PHY interface initialization of the PHY-LSI failed.
 * @retval  FSP_ERR_ALREADY_OPEN                    When reopening the PHY interface it was already opened.
 * @retval  FSP_ERR_INVALID_CHANNEL                 When reopening the PHY interface an invalid channel was passed.
 * @retval  FSP_ERR_INVALID_POINTER                 When reopening the PHY interface the MAC address pointer was NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT                When reopening the PHY interface the interrupt was not enabled.
 * @retval  FSP_ERR_ETHER_PHY_ERROR_LINK            Initialization of the PHY-LSI failed.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_LinkProcess (ether_ctrl_t * const p_ctrl)
{
    fsp_err_t              err             = FSP_SUCCESS;
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;

    ether_cfg_t const     * p_gmac_cfg;
    gmac_port_mask_t        temp_previous_link_status;
    gmac_port_mask_t        previous_link_establish_status;
    ether_phy_instance_t ** pp_phy_instance;
    gmac_extend_cfg_t     * p_extend;
    void (* temp_p_callback)(ether_callback_args_t *);
    ether_callback_args_t * temp_p_callback_memory;
    void const            * temp_p_context;

    uint32_t i;

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_gmac_cfg = p_instance_ctrl->p_gmac_cfg;

    /* When the magic packet is detected. */
    if (GMAC_MAGIC_PACKET_DETECTED == p_instance_ctrl->magic_packet)
    {
        p_instance_ctrl->magic_packet = GMAC_MAGIC_PACKET_NOT_DETECTED;

        gmac_call_user_callback(p_instance_ctrl, ETHER_EVENT_WAKEON_LAN, (gmac_port_mask_t) 0);

        /*
         * After the close function is called, the open function is called
         * to have to set ETHERC to a usual operational mode
         * to usually communicate after magic packet is detected.
         *//* back up previous_link_status */
        temp_previous_link_status = p_instance_ctrl->previous_link_status;

        /* back up callback and contex */
        temp_p_callback        = p_instance_ctrl->p_callback;
        temp_p_callback_memory = p_instance_ctrl->p_callback_memory;
        temp_p_context         = p_instance_ctrl->p_context;

        err = R_GMAC_Close((ether_ctrl_t *) p_instance_ctrl);
        GMAC_ERROR_RETURN(FSP_SUCCESS == err, err);

        err = R_GMAC_Open((ether_ctrl_t *) p_instance_ctrl, (ether_cfg_t *) p_gmac_cfg);
        GMAC_ERROR_RETURN(FSP_SUCCESS == err, err);

        /* restore previous_link_status */
        p_instance_ctrl->previous_link_status = temp_previous_link_status;

        /* restore callback and contex */
        p_instance_ctrl->p_callback        = temp_p_callback;
        p_instance_ctrl->p_callback_memory = temp_p_callback_memory;
        p_instance_ctrl->p_context         = temp_p_context;
    }

    previous_link_establish_status = p_instance_ctrl->link_establish_status;

    p_extend        = (gmac_extend_cfg_t *) p_gmac_cfg->p_extend;
    pp_phy_instance = (ether_phy_instance_t **) p_extend->pp_phy_instance;

    for (i = 0; BSP_FEATURE_GMAC_MAX_PORTS > i; i++)
    {
        if (NULL != pp_phy_instance[i])
        {
            /* Open Ether-Phy */
            (void) gmac_link_process(p_instance_ctrl, i);
        }
    }

    if (0 == previous_link_establish_status)
    {
        if (0 != p_instance_ctrl->link_establish_status)
        {
            /* Previous link status of all ports are down */
            /* Initialize the transmit and receive descriptor */
            memset(p_extend->p_rx_descriptors, 0x00,
                   sizeof(gmac_instance_descriptor_t) * p_gmac_cfg->num_rx_descriptors);
            memset(p_extend->p_tx_descriptors, 0x00,
                   sizeof(gmac_instance_descriptor_t) * p_gmac_cfg->num_tx_descriptors);

            /* Initialize the Ether buffer */
            for (i = 0;
                 i < (p_gmac_cfg->num_tx_descriptors + p_gmac_cfg->num_rx_descriptors);
                 i++)
            {
                memset(p_gmac_cfg->pp_ether_buffers[i], 0x00, p_gmac_cfg->ether_buffer_size);
            }

            /*
             * ETHERC and EDMAC are set after ETHERC and EDMAC are reset in software
             * and sending and receiving is permitted.
             */
            gmac_configure_mac(p_instance_ctrl, p_gmac_cfg->p_mac_address, GMAC_NO_USE_MAGIC_PACKET_DETECT);
            gmac_configure_operation(p_instance_ctrl);

            if (GMAC_MAGIC_PACKET_DETECTING == p_instance_ctrl->magic_packet)
            {
                R_GMAC_WakeOnLANEnable(p_ctrl);
            }
        }
    }
    else
    {
        if (0 == p_instance_ctrl->previous_link_status)
        {
            // Reset GMAC to force "MTL RX FIFO Read Controller State" to idle.
            // This is a countermeasure because the register Debug.RRCSTS does not become 0 (IDLE state).

            volatile R_GMAC_Type * p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

            /* Reset GMAC. */
            gmac_reset_mac(p_instance_ctrl->p_reg_gmac);

            p_reg_gmac->Interrupt_Mask_b.LPIIM = 1; /* LPI Interrupt Disable */
            p_reg_gmac->Interrupt_Mask_b.TSIM  = 1; /* Timestamo Interrupt Disable */
            p_reg_gmac->Interrupt_Mask_b.PMTIM = 1; /* PMT Interrupt Disable */
        }
    }

    return err;
}                                      /* End of function R_GMAC_LinkProcess() */

/********************************************************************************************************************//**
 * Get link status of specificed port.
 *
 * @retval  FSP_SUCCESS                                 Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER                     Value of the pointer is NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT                    When reopening the PHY interface the interrupt was not enabled.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_GetLinkStatus (ether_ctrl_t * const p_ctrl, uint8_t port, gmac_link_status_t * p_status)
{
    gmac_port_mask_t port_status;
    fsp_err_t        err = FSP_SUCCESS;

    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;

    /* Check argument */
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    GMAC_ERROR_RETURN(NULL != p_status, FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN(BSP_FEATURE_GMAC_MAX_PORTS > port, FSP_ERR_INVALID_ARGUMENT);

    GMAC_ERROR_RETURN(0 != (*((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->pp_phy_instance)[port],
                      FSP_ERR_INVALID_ARGUMENT);
#endif

    port_status = g_gmac_port_mask[port];

    if (port_status == (p_instance_ctrl->previous_link_status & port_status))
    {
        if (port_status == (p_instance_ctrl->link_establish_status & port_status))
        {
            *p_status = GMAC_LINK_STATUS_READY;
        }
        else
        {
            *p_status = GMAC_LINK_STATUS_UP;
        }
    }
    else
    {
        *p_status = GMAC_LINK_STATUS_DOWN;
    }

    return err;
}                                      /* End of function R_GMAC_GetLinkStatus() */

/********************************************************************************************************************//**
 * The setting of ETHERC is changed from normal sending and receiving mode to magic packet detection mode.
 * Implements @ref ether_api_t::wakeOnLANEnable.
 *
 * @retval  FSP_SUCCESS                                 Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_WakeOnLANEnable (ether_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;

    /* Check argument */
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_instance_ctrl->magic_packet = GMAC_MAGIC_PACKET_DETECTING;

    /* When the Link up processing is completed */
    /* Change to the magic packet detection mode.  */
    gmac_configure_mac(p_instance_ctrl, p_instance_ctrl->p_gmac_cfg->p_mac_address, GMAC_USE_MAGIC_PACKET_DETECT);
    gmac_magic_packet_detection(p_instance_ctrl);

    return err;
}                                      /* End of function R_GMAC_WakeOnLANEnable() */

/********************************************************************************************************************//**
 * Receive Ethernet frame. Receives data to the location specified by the pointer to the receive buffer.
 * In zero copy mode, the address of the receive buffer is returned.
 * In non zero copy mode, the received data in the internal buffer is copied to the pointer passed by the argument.
 * Implements @ref ether_api_t::read.
 *
 * @retval  FSP_SUCCESS                                 Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened.
 * @retval  FSP_ERR_ETHER_ERROR_NO_DATA                 There is no data in receive buffer.
 * @retval  FSP_ERR_ETHER_ERROR_LINK                    Auto-negotiation is not completed, and reception is not enabled.
 * @retval  FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE       As a Magic Packet is being detected, transmission and reception
 *                                                      is not enabled.
 * @retval  FSP_ERR_INVALID_POINTER                     Value of the pointer is NULL.
 *
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_Read (ether_ctrl_t * const p_ctrl, void * const p_buffer, uint32_t * const length_bytes)
{
    fsp_err_t              err             = FSP_SUCCESS;
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    uint8_t              * p_read_buffer   = NULL; /* Buffer location controlled by the Ethernet driver */
    uint32_t               received_size   = GMAC_NO_DATA;
    uint8_t             ** pp_read_buffer  = (uint8_t **) p_buffer;

    /* Check argument */
    err = gmac_read_param_check(p_instance_ctrl, p_buffer, length_bytes);

    while (FSP_SUCCESS == err)
    {
        /* When receive data exists. */

        if (GMAC_RDES0_OWN != (p_instance_ctrl->p_rx_descriptor->des0 & GMAC_RDES0_OWN))
        {
            if ((GMAC_RDES0_ES == (p_instance_ctrl->p_rx_descriptor->des0 & GMAC_RDES0_ES)) ||
                ((GMAC_RDES0_FS | GMAC_RDES0_LS) !=
                 (p_instance_ctrl->p_rx_descriptor->des0 & (GMAC_RDES0_FS | GMAC_RDES0_LS))))
            {
                /* The buffer is released when it is at the error or when single-frame is not in single descriptor. */
                err = R_GMAC_BufferRelease((ether_ctrl_t *) p_instance_ctrl);
                continue;
            }

            /* If put in TCM, adjust offset for DMA */
            if ((p_instance_ctrl->p_rx_descriptor->des2 >= TCM_DMA_ADR_ST) &&
                (p_instance_ctrl->p_rx_descriptor->des2 <= TCM_DMA_ADR_END))
            {
                p_read_buffer = (uint8_t *) (p_instance_ctrl->p_rx_descriptor->des2 - TCM_DMA_ADR_OFFSET);
            }
            else
            {
                p_read_buffer = (uint8_t *) p_instance_ctrl->p_rx_descriptor->des2;
            }

            received_size = (p_instance_ctrl->p_rx_descriptor->des0 & GMAC_RDES0_FL_MASK) >>
                            GMAC_RDES0_FL_SHIFT;

            if (GMAC_MINIMUM_FRAME_SIZE > received_size)
            {
                /* bytes received is short */
                err = R_GMAC_BufferRelease((ether_ctrl_t *) p_instance_ctrl);
                continue;
            }
        }
        else
        {
            err = FSP_ERR_ETHER_ERROR_NO_DATA;
        }

        break;
    }

    if (FSP_SUCCESS == err)
    {
        /* When there is data to receive */
        if (received_size > GMAC_NO_DATA)
        {
            if (ETHER_ZEROCOPY_DISABLE == p_instance_ctrl->p_gmac_cfg->zerocopy)
            {
                /* (2) Copy the data read from the receive buffer which is controlled
                 *     by the descriptor to the buffer which is specified by the user (up to 1024 bytes). */
                memcpy(p_buffer, p_read_buffer, received_size);

                /* (3) Read the receive data from the receive buffer controlled by the descriptor,
                 * and then release the receive buffer. */
                err = R_GMAC_BufferRelease((ether_ctrl_t *) p_instance_ctrl);
            }
            else
            {
                *pp_read_buffer = p_read_buffer;
            }

            *length_bytes = received_size;
        }
        /* When there is no data to receive */
        else
        {
            err = FSP_ERR_ETHER_ERROR_NO_DATA;
        }
    }

    return err;
}                                      /* End of function R_GMAC_Read() */

/********************************************************************************************************************//**
 * Transmit Ethernet frame. Transmits data from the location specified by the pointer to the transmit
 *  buffer, with the data size equal to the specified frame length.
 *  In the non zero copy mode, transmits data after being copied to the internal buffer.
 *  Implements @ref ether_api_t::write.
 *
 * @retval  FSP_SUCCESS                                Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                          Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                           The control block has not been opened.
 * @retval  FSP_ERR_ETHER_ERROR_LINK                    Auto-negotiation is not completed, and reception is not enabled.
 * @retval  FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE       As a Magic Packet is being detected, transmission and reception
 *                                                     is not enabled.
 * @retval  FSP_ERR_ETHER_ERROR_TRANSMIT_BUFFER_FULL    Transmit buffer is not empty.
 * @retval  FSP_ERR_INVALID_POINTER                    Value of the pointer is NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT                   Value of the send frame size is out of range.
 *
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_Write (ether_ctrl_t * const p_ctrl, void * const p_buffer, uint32_t const frame_length)
{
    fsp_err_t              err             = FSP_SUCCESS;
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    volatile R_GMAC_Type * p_reg_gmac;

    uint8_t * p_write_buffer;
    uint32_t  write_buffer_size;

    /* Check argument */
    err = gmac_write_param_check(p_instance_ctrl, p_buffer, frame_length);
    GMAC_ERROR_RETURN((FSP_SUCCESS == err), err);

    if (ETHER_ZEROCOPY_DISABLE == p_instance_ctrl->p_gmac_cfg->zerocopy)
    {
        /* (1) Retrieve the transmit buffer location controlled by the  descriptor. */
        err = gmac_buffer_get(p_instance_ctrl, (void **) &p_write_buffer, &write_buffer_size);

        /* Writing to the transmit buffer (buf) is enabled. */
        if (FSP_SUCCESS == err)
        {
            if (write_buffer_size < frame_length)
            {
                err = FSP_ERR_ETHER_ERROR_TRANSMIT_BUFFER_FULL; /* Transmit buffer overflow */
            }
            else
            {
                /* Write the transmit data to the transmit buffer. */

                /* (2) Write the data to the transmit buffer controlled by the  descriptor. */
                memcpy(p_write_buffer, p_buffer, frame_length);
            }
        }
    }

    /* Writing to the transmit buffer (buf) is enabled. */
    if (FSP_SUCCESS == err)
    {
        /* (3) Enable the EDMAC to transmit data in the transmit buffer. */
        /* When the Link up processing is not completed, return error */

        /* The data of the buffer is made active.  */
        if (ETHER_ZEROCOPY_ENABLE == p_instance_ctrl->p_gmac_cfg->zerocopy)
        {
            p_instance_ctrl->p_tx_descriptor->des2 = (uint32_t) p_buffer; // Set Buffer 1 Address of TDES

            /* Offset for DMA */
            if (p_instance_ctrl->p_tx_descriptor->des2 <= TCM_CPU_ADR_END)
            {
                p_instance_ctrl->p_tx_descriptor->des2 += TCM_DMA_ADR_OFFSET;
            }
        }

        p_instance_ctrl->p_tx_descriptor->des1 = frame_length;

        /* Update pointer to the currently referenced transmit descriptor  */
        if (GMAC_TDES0_TER == (p_instance_ctrl->p_tx_descriptor->des0 & GMAC_TDES0_TER))
        {
            p_instance_ctrl->p_tx_descriptor->des0 = (GMAC_TDES0_OWN |
                                                      GMAC_TDES0_IC |
                                                      GMAC_TDES0_LS |
                                                      GMAC_TDES0_FS |
                                                      GMAC_TDES0_TER);

            p_instance_ctrl->p_tx_descriptor =
                ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_tx_descriptors;
        }
        else
        {
            p_instance_ctrl->p_tx_descriptor->des0 = (GMAC_TDES0_OWN |
                                                      GMAC_TDES0_IC |
                                                      GMAC_TDES0_LS |
                                                      GMAC_TDES0_FS);
            p_instance_ctrl->p_tx_descriptor++;
        }

        /* Restart transmit */
        p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;
        __asm("dmb");
        p_reg_gmac->Transmit_Poll_Demand = 0;
    }

    return err;
}                                      /* End of function R_GMAC_Write() */

/**********************************************************************************************************************//**
 * Provides status of Ethernet driver in the user provided pointer. Implements @ref ether_api_t::txStatusGet.
 *
 * @retval  FSP_SUCCESS                  Transmit buffer address is stored in provided p_buffer_address.
 * @retval  FSP_ERR_ASSERTION            Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER      p_status is NULL.
 * @retval  FSP_ERR_NOT_FOUND            Transmit buffer address has been overwritten in transmit descriptor.
 ***********************************************************************************************************************/
fsp_err_t R_GMAC_TxStatusGet (ether_ctrl_t * const p_ctrl, void * const p_buffer_address)
{
    gmac_instance_ctrl_t       * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    volatile R_GMAC_Type       * p_reg_gmac;
    gmac_instance_descriptor_t * p_descriptor;
    uint8_t ** p_sent_buffer_address = (uint8_t **) p_buffer_address;
    fsp_err_t  err = FSP_ERR_NOT_FOUND;
    uint32_t   current_descriptor;

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    GMAC_ERROR_RETURN(NULL != p_buffer_address, FSP_ERR_INVALID_POINTER);
#endif

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    current_descriptor = p_reg_gmac->Current_Host_Transmit_Descriptor;

    /* If put in TCM, adjust offset for DMA */
    if ((current_descriptor >= TCM_DMA_ADR_ST) &&
        (current_descriptor <= TCM_DMA_ADR_END))
    {
        current_descriptor -= TCM_DMA_ADR_OFFSET;
    }

    p_descriptor = (gmac_instance_descriptor_t *) current_descriptor;

    /* Descriptor is NULL, when no packet transmitted. */
    if (NULL != p_descriptor)
    {
        uint32_t num_tx_descriptors = p_instance_ctrl->p_gmac_cfg->num_tx_descriptors;
        gmac_instance_descriptor_t * p_tx_descriptors =
            ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_tx_descriptors;

        p_descriptor = (gmac_instance_descriptor_t *) ((uint8_t *) p_descriptor - sizeof(gmac_instance_descriptor_t));

        if (p_descriptor < p_tx_descriptors)
        {
            p_descriptor = &p_tx_descriptors[num_tx_descriptors - 1];
        }

        /* Check that the descriptor is not overridden. */
        if ((0 != p_descriptor->des2) && (GMAC_TDES0_OWN != (p_descriptor->des0 & GMAC_TDES0_OWN)))
        {
            if ((p_descriptor->des2 >= TCM_DMA_ADR_ST) &&
                (p_descriptor->des2 <= TCM_DMA_ADR_END))
            {
                *p_sent_buffer_address = (uint8_t *) (p_descriptor->des2 - TCM_DMA_ADR_OFFSET);
            }
            else
            {
                *p_sent_buffer_address = (uint8_t *) p_descriptor->des2;
            }

            err = FSP_SUCCESS;
        }
        else
        {
            ;
        }
    }

    return err;
}                                      /* End of function R_GMAC_TxStatusGet() */

/*******************************************************************************************************************//**
 * Updates the user callback with the option to provide memory for the callback argument structure.
 * Implements @ref ether_api_t::callbackSet.
 *
 * @retval  FSP_SUCCESS                  Callback updated successfully.
 * @retval  FSP_ERR_ASSERTION            A required pointer is NULL.
 * @retval  FSP_ERR_NOT_OPEN             The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_GMAC_CallbackSet (ether_ctrl_t * const          p_ctrl,
                              void (                      * p_callback)(ether_callback_args_t *),
                              void const * const            p_context,
                              ether_callback_args_t * const p_callback_memory)
{
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;

#if ETHER_CFG_PARAM_CHECKING_ENABLE
    FSP_ASSERT(p_instance_ctrl);
    FSP_ASSERT(p_callback);
    FSP_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Store callback and context */
    p_instance_ctrl->p_callback        = p_callback;
    p_instance_ctrl->p_context         = p_context;
    p_instance_ctrl->p_callback_memory = p_callback_memory;

    return FSP_SUCCESS;
}                                      /* End of function R_GMAC_CallbackSet() */

/*******************************************************************************************************************//**
 * @} (end addtogroup ETHER)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)

/*******************************************************************************************************************//**
 * Parameter error check function for open.
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 * @param[in] p_cfg             Pointer to the configuration structure specific to UART mode
 *
 * @retval  FSP_SUCCESS                  No parameter error found
 * @retval  FSP_ERR_ASSERTION            Pointer to ETHER control block or configuration structure is NULL
 * @retval  FSP_ERR_ALREADY_OPEN         Control block has already been opened
 * @retval  FSP_ERR_INVALID_CHANNEL      Invalid channel number is given.
 * @retval  FSP_ERR_INVALID_POINTER      Pointer to MAC address is NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT      Irq number lower then 0.
 **********************************************************************************************************************/
static fsp_err_t gmac_open_param_check (gmac_instance_ctrl_t const * const p_instance_ctrl,
                                        ether_cfg_t const * const          p_cfg)
{
    gmac_extend_cfg_t * p_extend;

    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN((NULL != p_cfg), FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN((NULL != p_cfg->p_mac_address), FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN((BSP_FEATURE_GMAC_MAX_CHANNELS > p_cfg->channel), FSP_ERR_INVALID_CHANNEL);

    GMAC_ERROR_RETURN((0 <= p_cfg->irq), FSP_ERR_INVALID_ARGUMENT);
    GMAC_ERROR_RETURN((NULL != p_cfg->p_extend), FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN((NULL != p_cfg->pp_ether_buffers), FSP_ERR_INVALID_POINTER);

    p_extend = (gmac_extend_cfg_t *) p_cfg->p_extend;

    GMAC_ERROR_RETURN((NULL != p_extend->p_rx_descriptors), FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN((NULL != p_extend->p_tx_descriptors), FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN((0 <= p_extend->pmt_irq), FSP_ERR_INVALID_ARGUMENT);

    GMAC_ERROR_RETURN((GMAC_OPEN != p_instance_ctrl->open), FSP_ERR_ALREADY_OPEN);

    return FSP_SUCCESS;
}                                      /* End of function gmac_open_param_check() */

#endif

/*******************************************************************************************************************//**
 * Parameter error check function for read.
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 * @param[in] p_buffer          Pointer to where to store read data.
 * @param[in] length_bytes      Number of bytes in buffer
 *
 * @retval  FSP_SUCCESS                                 Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened.
 * @retval  FSP_ERR_ETHER_ERROR_LINK                     Auto-negotiation is not completed, and reception is not enabled.
 * @retval  FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE        As a Magic Packet is being detected, transmission and reception
 *                                                      is not enabled.
 * @retval  FSP_ERR_INVALID_POINTER                     Value of the pointer is NULL.
 **********************************************************************************************************************/
static fsp_err_t gmac_read_param_check (gmac_instance_ctrl_t * p_instance_ctrl,
                                        void * const           p_buffer,
                                        uint32_t * const       length_bytes)
{
    /* Check argument */
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    GMAC_ERROR_RETURN(NULL != p_buffer, FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN(NULL != length_bytes, FSP_ERR_INVALID_POINTER);
#else
    FSP_PARAMETER_NOT_USED(p_buffer);
    FSP_PARAMETER_NOT_USED(length_bytes);
#endif

    /* (1) Retrieve the receive buffer location controlled by the  descriptor. */
    /* When the Link up processing is not completed, return error */
    GMAC_ERROR_RETURN(0 != p_instance_ctrl->link_establish_status, FSP_ERR_ETHER_ERROR_LINK);

    /* In case of detection mode of magic packet, return error. */
    GMAC_ERROR_RETURN(0 == gmac_check_magic_packet_detection_bit(p_instance_ctrl),
                      FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE);

    return FSP_SUCCESS;
}                                      /* End of function gmac_read_param_check() */

/*******************************************************************************************************************//**
 * Parameter error check function for write.
 *
 * @param[in]  p_instance_ctrl  Pointer to the control block for the channel.
 * @param[in]  p_buffer         Pointer to data to write.
 * @param[in]  frame_length     Send ethernet frame size (without 4 bytes of CRC data size).
 *
 * @retval  FSP_SUCCESS                                Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION                          Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                           The control block has not been opened.
 * @retval  FSP_ERR_ETHER_ERROR_LINK                    Auto-negotiation is not completed, and reception is not enabled.
 * @retval  FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE       As a Magic Packet is being detected, transmission and reception
 *                                                     is not enabled.
 * @retval  FSP_ERR_INVALID_POINTER                    Value of the pointer is NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT                   Value of the send frame size is out of range.
 **********************************************************************************************************************/
static fsp_err_t gmac_write_param_check (gmac_instance_ctrl_t * p_instance_ctrl,
                                         void * const           p_buffer,
                                         uint32_t const         frame_length)
{
#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    GMAC_ERROR_RETURN(NULL != p_buffer, FSP_ERR_INVALID_POINTER);
    GMAC_ERROR_RETURN(p_instance_ctrl->p_gmac_cfg->ether_buffer_size >= frame_length, FSP_ERR_INVALID_ARGUMENT);
#else
    FSP_PARAMETER_NOT_USED(p_buffer);
    FSP_PARAMETER_NOT_USED(frame_length);
#endif

    /* When the Link up processing is not completed, return error */
    GMAC_ERROR_RETURN(0 != p_instance_ctrl->link_establish_status, FSP_ERR_ETHER_ERROR_LINK);

    /* In case of detection mode of magic packet, return error. */
    GMAC_ERROR_RETURN(0 == gmac_check_magic_packet_detection_bit(p_instance_ctrl),
                      FSP_ERR_ETHER_ERROR_MAGIC_PACKET_MODE);

    return FSP_SUCCESS;
}                                      /* End of function gmac_write_param_check() */

/*******************************************************************************************************************//**
 * Reset The GMAC are reset through the software reset.
 *
 * @param[out]  p_reg       Base address of regeister for GMAC
 *
 * @retval  none
 **********************************************************************************************************************/
static void gmac_reset_mac (volatile R_GMAC_Type * const p_reg)
{
    p_reg->Bus_Mode_b.SWR = 1;

    /* Wait for reset to complete */
    while (p_reg->Bus_Mode_b.SWR == 1)
    {
        ;
    }

    /* Settings for TCM access */
    p_reg->Bus_Mode_b.AAL       = 1;
    p_reg->Bus_Mode_b.FB        = 1;
    p_reg->Bus_Mode_b.USP       = 0;
    p_reg->Bus_Mode_b.PBL       = 4;
    p_reg->AXI_Bus_Mode_b.BLEN4 = 1;
}                                      /* End of function gmac_reset_mac() */

#ifdef GMAC_IMPLEMENT_ETHSW

/*******************************************************************************************************************//**
 * Open ether switch driver.
 *
 * @param[in]   p_instance_ctrl     Pointer to the control block for the channel
 *
 * @retval  FSP_SUCCESS                 Channel opened successfully.
 **********************************************************************************************************************/
fsp_err_t gmac_open_ethsw (gmac_instance_ctrl_t * p_instance_ctrl)
{
    ether_switch_instance_t const * p_ethsw_instance; ///< Pointer to ETHER_SWITCH instance
    fsp_err_t err;

    p_ethsw_instance = ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_ethsw_instance;

    err = p_ethsw_instance->p_api->open(p_ethsw_instance->p_ctrl, p_ethsw_instance->p_cfg);
    if (FSP_SUCCESS != err)
    {
        p_ethsw_instance->p_api->close(p_ethsw_instance->p_ctrl);
    }

    return err;
}                                      /* End of function gmac_open_ethsw() */

#endif // GMAC_IMPLEMENT_ETHSW

/*******************************************************************************************************************//**
 * Open ether phy driver.
 *
 * @param[in]   p_instance_ctrl     Pointer to the control block for the channel
 *
 * @retval  FSP_SUCCESS                             Channel opened successfully.
 **********************************************************************************************************************/
fsp_err_t gmac_open_phy (gmac_instance_ctrl_t * p_instance_ctrl)
{
    ether_phy_instance_t const *(*pp_phy_instance)[BSP_FEATURE_GMAC_MAX_PORTS];
    ether_phy_instance_t const * p_phy_instance;

    fsp_err_t err = FSP_SUCCESS;
    uint32_t  i;

    pp_phy_instance = ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->pp_phy_instance;

    for (i = 0; BSP_FEATURE_GMAC_MAX_PORTS > i; i++)
    {
        p_phy_instance = (*pp_phy_instance)[i];

        if (NULL != p_phy_instance)
        {
            /* Open Ether-Phy */
            err = p_phy_instance->p_api->open(p_phy_instance->p_ctrl, p_phy_instance->p_cfg);
            if (FSP_SUCCESS == err)
            {
                /* Start Auto-Negotiate */
                p_phy_instance->p_api->startAutoNegotiate(p_phy_instance->p_ctrl);
            }
            else
            {
                break;
            }
        }
    }

    if (FSP_SUCCESS != err)
    {
        for (i = 0; BSP_FEATURE_GMAC_MAX_PORTS > i; i++)
        {
            p_phy_instance = (*pp_phy_instance)[i];

            if (NULL != p_phy_instance)
            {
                p_phy_instance->p_api->close(p_phy_instance->p_ctrl);
            }
        }
    }

    return err;
}                                      /* End of function gmac_open_phy() */

/*******************************************************************************************************************//**
 * The DMAC descriptors and the driver buffers are initialized.
 *
 * @param[in]   p_instance_ctrl     Pointer to the control block for the channel
 *
 * @retval  none
 **********************************************************************************************************************/
static void gmac_init_descriptors (gmac_instance_ctrl_t * const p_instance_ctrl)
{
    gmac_instance_descriptor_t * p_descriptor = NULL;
    uint32_t i;

    /* Initialize the receive descriptors */
    for (i = 0; i < p_instance_ctrl->p_gmac_cfg->num_rx_descriptors; i++)
    {
        p_descriptor = &((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_rx_descriptors[i];

        p_descriptor->des0 = GMAC_RDES0_OWN; /* receive status */
        p_descriptor->des1 =                 /* Buffer 1 Byte Counte */
                             p_instance_ctrl->p_gmac_cfg->ether_buffer_size;
        p_descriptor->des2 =                 /* Buffer 1 Address */
                             (uint32_t) p_instance_ctrl->p_gmac_cfg->pp_ether_buffers[i];

        /* Offset for DMA */
        if (p_descriptor->des2 <= TCM_CPU_ADR_END)
        {
            p_descriptor->des2 += TCM_DMA_ADR_OFFSET;
        }

        p_descriptor->des3 = 0;        /* Buffer 2 Address */
    }

    if (NULL != p_descriptor)
    {
        /* The last descriptor points back to the start */
        p_descriptor->des1 |= GMAC_RDES1_RER; /* Receive End of Ring */

        /* Initialize application receive descriptor pointer */
        p_instance_ctrl->p_rx_descriptor =
            &((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_rx_descriptors[0];
    }

    /* Initialize the transmit descriptors */
    for (i = 0; i < p_instance_ctrl->p_gmac_cfg->num_tx_descriptors; i++)
    {
        p_descriptor = &((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_tx_descriptors[i];

        p_descriptor->des0 = 0;        /* transmit control/status */
        p_descriptor->des1 = 0;        /* Buffer 1 Byte Counte */
        p_descriptor->des2 =           /* Buffer 1 Address */
                             (uint32_t) p_instance_ctrl->p_gmac_cfg->pp_ether_buffers[p_instance_ctrl->p_gmac_cfg->
                                                                                      num_rx_descriptors + i];

        /* Offset for DMA */
        if (p_descriptor->des2 <= TCM_CPU_ADR_END)
        {
            p_descriptor->des2 += TCM_DMA_ADR_OFFSET;
        }

        p_descriptor->des3 = 0;        /* Buffer 2 Address */
    }

    if (NULL != p_descriptor)
    {
        /* The last descriptor points back to the start */
        p_descriptor->des0 |= GMAC_TDES0_TER; /* Transmit End of Ring */

        /* Initialize application transmit descriptor pointer */
        p_instance_ctrl->p_tx_descriptor =
            &((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_tx_descriptors[0];
    }
}                                      /* End of function gmac_init_descriptors() */

/********************************************************************************************************************//**
 * Get Points to the buffer pointer used by the stack.
 *
 * @param[in]  p_instance_ctrl                              Ethernet driver control block.
 * @param[out] p_buffer                                     Pointer to location to store the buffer pointer
 * @param[out] p_buffer_size                                Pointer to location to store the buffer size
 *
 * @retval     FSP_SUCCESS                                  Processing completed successfully.
 * @retval     FSP_ERR_ETHER_ERROR_TRANSMIT_BUFFER_FULL     Transmit buffer is not empty.
 ***********************************************************************************************************************/
static fsp_err_t gmac_buffer_get (gmac_instance_ctrl_t * const p_instance_ctrl,
                                  void ** const                p_buffer,
                                  uint32_t                   * p_buffer_size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* When the Link up processing is completed */
    /* All transmit buffers are full */
    if (GMAC_TDES0_OWN == (p_instance_ctrl->p_tx_descriptor->des0 & GMAC_TDES0_OWN))
    {
        err = FSP_ERR_ETHER_ERROR_TRANSMIT_BUFFER_FULL;
    }
    else
    {
        /* Give application another buffer to work with */
        if ((p_instance_ctrl->p_tx_descriptor->des2 >= TCM_DMA_ADR_ST) &&
            (p_instance_ctrl->p_tx_descriptor->des2 <= TCM_DMA_ADR_END))
        {
            (*p_buffer) = (void *) (p_instance_ctrl->p_tx_descriptor->des2 - TCM_DMA_ADR_OFFSET);
        }
        else
        {
            (*p_buffer) = (void *) p_instance_ctrl->p_tx_descriptor->des2;
        }

        (*p_buffer_size) = p_instance_ctrl->p_gmac_cfg->ether_buffer_size;
        err              = FSP_SUCCESS;
    }

    return err;
}                                      /* End of function gmac_buffer_get() */

/********************************************************************************************************************//**
 * Configure the GMAC
 *
 * @param[in]   p_instance_ctrl         Ethernet driver control block.
 * @param[in]   mode                    The operational mode is specified.
 *                                      NO_USE_MAGIC_PACKET_DETECT (0) - Communicate mode usually
 *                                      USE_MAGIC_PACKET_DETECT    (1) - Magic packet detection mode
 *
 * @retval      none
 ***********************************************************************************************************************/
static void gmac_config_ethernet (gmac_instance_ctrl_t const * const p_instance_ctrl, const uint8_t mode)
{
    R_GMAC_Type * p_reg_gmac;

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument */
    if ((NULL == p_instance_ctrl) || (GMAC_OPEN != p_instance_ctrl->open))
    {
        return;
    }
#endif

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    /* Initialize Receive Descriptor List Address */
    p_reg_gmac->Receive_Descriptor_List_Address =
        (uint32_t) p_instance_ctrl->p_rx_descriptor & (~3U);

    /* Initialize Transmit Descriptor List Address */
    p_reg_gmac->Transmit_Descriptor_List_Address =
        (uint32_t) p_instance_ctrl->p_tx_descriptor & (~3U);

    /* Offset for DMA */
    if (p_reg_gmac->Receive_Descriptor_List_Address <= TCM_CPU_ADR_END)
    {
        p_reg_gmac->Receive_Descriptor_List_Address += TCM_DMA_ADR_OFFSET;
    }

    if (p_reg_gmac->Transmit_Descriptor_List_Address <= TCM_CPU_ADR_END)
    {
        p_reg_gmac->Transmit_Descriptor_List_Address += TCM_DMA_ADR_OFFSET;
    }

    /* Initialize MAC Frame Filter */
    p_reg_gmac->MAC_Frame_Filter_b.PR = /* promiscuous mode */
                                        p_instance_ctrl->p_gmac_cfg->promiscuous;

    /* Initialize MMC Receive/Transmit Interrupt Mask */
    p_reg_gmac->MMC_Receive_Interrupt_Mask  = GMAC_MMC_RECEIVE_INTERRUPT_MASK_ALL_BIT;
    p_reg_gmac->MMC_Transmit_Interrupt_Mask = GMAC_MMC_TRANSMIT_INTERRUPT_MASK_ALL_BIT;

    /* Magic packet detection mode */
    if (GMAC_USE_MAGIC_PACKET_DETECT == mode)
    {
        p_reg_gmac->Interrupt_Mask_b.PMTIM = 0; /* PMT Interrupt Enable */
    }
    /* Normal mode */
    else
    {
        p_reg_gmac->Interrupt_Mask_b.PMTIM = 1; /* PMT Interrupt Disable */

        /* Interrupt Enable Register */
        p_reg_gmac->Interrupt_Enable =
            GMAC_INTERRUPT_ENABLE_NIE |         /* Normal Interrupt Summary Enable */
            GMAC_INTERRUPT_ENABLE_AIE |         /* Abnormal Interrupt Summary Enable */
            GMAC_INTERRUPT_ENABLE_ERE |         /* Early Receive Interrupt Enable */
            GMAC_INTERRUPT_ENABLE_FBE |         /* Fatal Bus Error Enable */
            GMAC_INTERRUPT_ENABLE_ETE |         /* Early Transmit Interrupt Enable */
            GMAC_INTERRUPT_ENABLE_RWE |         /* Receive Watchdog Timeout Enable */
            GMAC_INTERRUPT_ENABLE_RUE |         /* Receive Buffer Unavailable Enable */
            GMAC_INTERRUPT_ENABLE_RIE |         /* Receive Interrupt Enable */
            GMAC_INTERRUPT_ENABLE_UNE |         /* Underflow Interrupt Enable */
            GMAC_INTERRUPT_ENABLE_OVE |         /* Overflow Interrupt Enable */
            GMAC_INTERRUPT_ENABLE_TIE;          /* Transmit Interrupt Enable */
    }
}                                               /* End of function gmac_config_ethernet() */

/********************************************************************************************************************//**
 * Description  : Determines PAUSE frame generation and handling. Uses the resolution Table 28B-3 of IEEE 802.3-2008.
 *
 * @param[in]   local_ability       local PAUSE capability (2 least significant bits)
 * @param[in]   partner_ability     link partner PAUSE capability (2 least significant bits)
 * @param[in]   p_tx_pause          pointer to location to store the result of the table lookup for transmit PAUSE.
 *                                  1 is enable, 0 is disable.
 * @param[in]   p_rx_pause          pointer to location to store the result of the table lookup for receive PAUSE.
 *                                  1 is enable, 0 is disable.
 *
 * @retval      none
 ***********************************************************************************************************************/
static void gmac_pause_resolution (uint32_t const local_ability,
                                   uint32_t const partner_ability,
                                   uint32_t     * p_tx_pause,
                                   uint32_t     * p_rx_pause)
{
    uint32_t i;
    uint32_t ability_compare;

    /*
     * Arrange the bits so that they correspond to the Table 28B-3
     * of the IEEE 802.3 values.
     */
    ability_compare =
        (uint32_t) (((local_ability & GMAC_LINK_RESOLUTION_ABILITY_MASK) <<
                     GMAC_LINK_RESOLUTION_LOCAL_ABILITY_BITSHIFT) |
                    (partner_ability & GMAC_LINK_RESOLUTION_ABILITY_MASK));

    /* Walk through the look up table */
    for (i = 0; i < GMAC_PAUSE_TABLE_ENTRIES; i++)
    {
        if ((ability_compare & pause_resolution[i].mask) == pause_resolution[i].value)
        {
            (*p_tx_pause) = pause_resolution[i].transmit;
            (*p_rx_pause) = pause_resolution[i].receive;

            return;
        }
    }
}                                      /* End of function gmac_pause_resolution() */

/********************************************************************************************************************//**
 * Description  : GMAC are configured.
 *
 * @param[in]   p_instance_ctrl         Ethernet driver control block.
 * @param[in]   mac_addr                The MAC address of GMAC
 * @param[in]   mode                    The operational mode is specified.
 *                                      NO_USE_MAGIC_PACKET_DETECT (0) - Communicate mode usually
 *                                      USE_MAGIC_PACKET_DETECT    (1) - Magic packet detection mode
 *
 * @retval      none
 ***********************************************************************************************************************/
static void gmac_configure_mac (gmac_instance_ctrl_t * const p_instance_ctrl,
                                const uint8_t                mac_addr[],
                                const uint8_t                mode)
{
    volatile R_GMAC_Type * p_reg_gmac;
    uint32_t               mac_h;
    uint32_t               mac_l;

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument */
    if ((NULL == p_instance_ctrl) || (GMAC_OPEN != p_instance_ctrl->open))
    {
        return;
    }
#endif

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    /* Disable receive and transmit. */
    gmac_disable(p_instance_ctrl);

    /* Set MAC address */
    mac_h = ((uint32_t) mac_addr[5] << 8) |
            ((uint32_t) mac_addr[4] << 0);

    mac_l = ((uint32_t) mac_addr[3] << 24) |
            ((uint32_t) mac_addr[2] << 16) |
            ((uint32_t) mac_addr[1] << 8) |
            ((uint32_t) mac_addr[0] << 0);

    p_reg_gmac->MAR0_H_b.ADDRHI = (uint16_t) mac_h;
    p_reg_gmac->MAR0_L_b.ADDRLO = mac_l;

    /* Set MAC address 1 */
    uint8_t * mac_addr1 = ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_mac_address1;
    if (0 != mac_addr1)
    {
        mac_h = ((uint32_t) mac_addr1[5] << 8) |
                ((uint32_t) mac_addr1[4] << 0);

        mac_l = ((uint32_t) mac_addr1[3] << 24) |
                ((uint32_t) mac_addr1[2] << 16) |
                ((uint32_t) mac_addr1[1] << 8) |
                ((uint32_t) mac_addr1[0] << 0);

        p_reg_gmac->MAR1_H_b.ADDRHI = (uint16_t) mac_h;
        p_reg_gmac->MAR1_H_b.MBC    = 0;
        p_reg_gmac->MAR1_H_b.SA     = 0;
        p_reg_gmac->MAR1_H_b.AE     = 1;
        p_reg_gmac->MAR1_L_b.ADDRLO = mac_l;
    }

    /* Set MAC address 2 */
    uint8_t * mac_addr2 = ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->p_mac_address2;
    if (0 != mac_addr2)
    {
        mac_h = ((uint32_t) mac_addr2[5] << 8) |
                ((uint32_t) mac_addr2[4] << 0);

        mac_l = ((uint32_t) mac_addr2[3] << 24) |
                ((uint32_t) mac_addr2[2] << 16) |
                ((uint32_t) mac_addr2[1] << 8) |
                ((uint32_t) mac_addr2[0] << 0);

        p_reg_gmac->MAR2_H_b.ADDRHI = (uint16_t) mac_h;
        p_reg_gmac->MAR2_H_b.MBC    = 0;
        p_reg_gmac->MAR2_H_b.SA     = 0;
        p_reg_gmac->MAR2_H_b.AE     = 1;
        p_reg_gmac->MAR2_L_b.ADDRLO = mac_l;
    }

    /* Initialize receive and transmit descriptors */
    gmac_init_descriptors(p_instance_ctrl);

    /* Perform reset of hardware interface configuration */

    gmac_config_ethernet(p_instance_ctrl, mode);
}                                      /* End of function gmac_configure_mac() */

/*******************************************************************************************************************//**
 * Disable receive and transmit
 *
 * @param[in] p_instance_ctrl           Ethernet driver control block.
 *
 * @retval      none
 ***********************************************************************************************************************/
static void gmac_disable (gmac_instance_ctrl_t * const p_instance_ctrl)
{
    volatile R_GMAC_Type * p_reg_gmac;

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    p_reg_gmac->MAC_Configuration_b.TE = 0;
    p_reg_gmac->MAC_Configuration_b.RE = 0;
}                                      /* End of function gmac_disable() */

/*******************************************************************************************************************//**
 * Call user's callback function from R_GMAC_LinkProcess
 *
 * @param[in] p_instance_ctrl           Ethernet driver control block.
 * @param[in] event                     Event code.
 * @param[in] link_status               Link status.
 *
 * @retval      none
 ***********************************************************************************************************************/
void gmac_call_user_callback (gmac_instance_ctrl_t * p_instance_ctrl, ether_event_t event, gmac_port_mask_t link_status)
{
    ether_cfg_t const   * p_gmac_cfg = p_instance_ctrl->p_gmac_cfg;
    ether_callback_args_t callback_arg;

    if (NULL != p_instance_ctrl->p_callback)
    {
        callback_arg.channel      = p_gmac_cfg->channel;
        callback_arg.event        = event;
        callback_arg.status_ether = 0;
        callback_arg.status_link  = link_status;
        callback_arg.p_context    = p_gmac_cfg->p_context;

        gmac_call_callback(p_instance_ctrl, &callback_arg);
    }
}                                      /* End of function gmac_call_user_callback() */

/*******************************************************************************************************************//**
 * Run Link process by port
 *
 * @param[in] p_instance_ctrl       Ethernet driver control block.
 * @param[in] port                  The port number.
 *
 * @retval     FSP_SUCCESS                      Processing completed successfully.
 * @retval     FSP_ERR_ASSERTION                Pointer to ETHER control block or configuration structure is NULL.
 * @retval     FSP_ERR_NOT_OPEN                 The control block has not been opened.
 * @retval     FSP_ERR_ETHER_ERROR_LINK         Auto-negotiation is not completed, and reception is not enabled.
 ***********************************************************************************************************************/
static fsp_err_t gmac_link_process (gmac_instance_ctrl_t * p_instance_ctrl, uint32_t port)
{
    fsp_err_t          err = FSP_SUCCESS;
    gmac_link_change_t link_change;
    gmac_port_mask_t   port_status = g_gmac_port_mask[port];

    link_change = gmac_link_change_check(p_instance_ctrl, port);

    /* When the change status is Link-up */
    if (GMAC_LINK_CHANGE_LINK_UP == link_change)
    {
        err = gmac_do_link(p_instance_ctrl, port);
        if (FSP_SUCCESS == err)
        {
            p_instance_ctrl->link_establish_status |= port_status;

            gmac_call_user_callback(p_instance_ctrl, ETHER_EVENT_LINK_ON, port_status);
        }
        else
        {
            /* When PHY auto-negotiation is not completed */
            p_instance_ctrl->link_establish_status &= ~port_status;
        }
    }
    /* When the change status is Link-down */
    else if (GMAC_LINK_CHANGE_LINK_DOWN == link_change)
    {
        p_instance_ctrl->link_establish_status &= ~port_status; // the establesh status is down

        gmac_call_user_callback(p_instance_ctrl, ETHER_EVENT_LINK_OFF, port_status);

        err = FSP_ERR_ETHER_ERROR_LINK;
    }
    /* When the change status is no change */
    else
    {
        /* when the previous link status is Link-up and the establesh status is down */
        if ((0 != (p_instance_ctrl->previous_link_status & port_status)) &&
            (0 == (p_instance_ctrl->link_establish_status & port_status)))
        {
            err = gmac_do_link(p_instance_ctrl, port);
            if (FSP_SUCCESS == err)
            {
                p_instance_ctrl->link_establish_status |= port_status; // the establesh status is up

                gmac_call_user_callback(p_instance_ctrl, ETHER_EVENT_LINK_ON, port_status);
            }
        }
        else
        {
            /* no operation */
        }
    }

    return err;
}                                      /* End of function gmac_link_process() */

/*******************************************************************************************************************//**
 * Check Link status change
 *
 * @param[in] p_instance_ctrl       Ethernet driver control block.
 * @param[in] port                  The port number.
 *
 * @retval     GMAC_LINK_CHANGE_LINK_UP         Change from link down to link up.
 * @retval     GMAC_LINK_CHANGE_LINK_DOWN       Change from link down to link up.
 * @retval     GMAC_LINK_CHANGE_NO_CHANGE       No change link status.
 ***********************************************************************************************************************/
static gmac_link_change_t gmac_link_change_check (gmac_instance_ctrl_t * p_instance_ctrl, uint32_t port)
{
    gmac_link_change_t link_change = GMAC_LINK_CHANGE_NO_CHANGE;
    gmac_port_mask_t   port_status = g_gmac_port_mask[port];
    fsp_err_t          err;

    if (GMAC_PHYLINK_ENABLE == ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->phylink)
    {
        if (0 != (p_instance_ctrl->link_status & port_status))
        {
            /* When becoming Link up */
            if (0 == (p_instance_ctrl->previous_link_status & port_status))
            {
                /* When the previous link status is Link-down, the change status is Link-up */
                link_change = GMAC_LINK_CHANGE_LINK_UP;

                /* Update previous link status */
                p_instance_ctrl->previous_link_status |= port_status;
            }
        }
        else
        {
            /* When becoming Link down */
            if (0 != (p_instance_ctrl->previous_link_status & port_status))
            {
                /* When the previous link status is Link-up, the change status is Link-down */
                link_change = GMAC_LINK_CHANGE_LINK_DOWN;

                /* Update previous link status */
                p_instance_ctrl->previous_link_status &= ~port_status;
            }
        }
    }
    else
    {
        err = gmac_link_status_check(p_instance_ctrl, port);

        /* The state of the link status in PHY-LSI is confirmed and Link Up/Down is judged. */
        if (FSP_SUCCESS == err)
        {
            /* When becoming Link-up */
            if (0 == (p_instance_ctrl->previous_link_status & port_status))
            {
                /* When the previous link status is Link-down, the change status is Link-up */
                link_change = GMAC_LINK_CHANGE_LINK_UP;

                /* Update previous link status */
                p_instance_ctrl->previous_link_status |= port_status;
            }
        }
        else
        {
            /* When becoming Link-down */
            if (0 != (p_instance_ctrl->previous_link_status & port_status))
            {
                /* When the previous link status is Link-up, the change status is Link-down */
                link_change = GMAC_LINK_CHANGE_LINK_DOWN;

                /* Update previous link status */
                p_instance_ctrl->previous_link_status &= ~port_status;
            }
        }
    }

    return link_change;
}                                      /* End of function gmac_link_change_check() */

/*******************************************************************************************************************//**
 * Determines the partner PHY capability through auto-negotiation process.
 * The link abilities are handled to determine duplex, speed and flow control (PAUSE frames).
 *
 * @param[in] p_instance_ctrl           Pointer to the control block for the channel
 * @param[in] port                      Port number
 *
 * @retval  FSP_SUCCESS                 Processing completed successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block or configuration structure is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_ETHER_ERROR_LINK    Auto-negotiation of PHY-LSI is not completed
 *                                      or result of Auto-negotiation is abnormal.
 ***********************************************************************************************************************/
static fsp_err_t gmac_do_link (gmac_instance_ctrl_t * const p_instance_ctrl, const uint32_t port)
{
    ether_phy_instance_t const *(*pp_phy_instance)[BSP_FEATURE_GMAC_MAX_PORTS];

    fsp_err_t err = FSP_SUCCESS;
#ifdef GMAC_IMPLEMENT_ETHSW
    ethsw_link_speed_t speed;
#endif                                 // GMAC_IMPLEMENT_ETHSW

#if (GMAC_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    GMAC_ERROR_RETURN(GMAC_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Get the link status */
    pp_phy_instance = ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->pp_phy_instance;

    err = (*pp_phy_instance)[port]->p_api->linkPartnerAbilityGet((*pp_phy_instance)[port]->p_ctrl,
                                                                 &p_instance_ctrl->link_speed_duplex,
                                                                 &p_instance_ctrl->local_pause_bits,
                                                                 &p_instance_ctrl->partner_pause_bits);
#ifdef GMAC_IMPLEMENT_ETHSW
    if (FSP_SUCCESS == err)
    {
        switch (p_instance_ctrl->link_speed_duplex)
        {
            /* Half duplex link */
            case ETHER_PHY_LINK_SPEED_1000H:
            {
                speed = ETHSW_LINK_SPEED_1000H;
                break;
            }

            case ETHER_PHY_LINK_SPEED_100H:
            {
                speed = ETHSW_LINK_SPEED_100H;
                break;
            }

            case ETHER_PHY_LINK_SPEED_10H:
            {
                speed = ETHSW_LINK_SPEED_10H;
                break;
            }

            /* Full duplex link */
            case ETHER_PHY_LINK_SPEED_1000F:
            {
                speed = ETHSW_LINK_SPEED_1000F;
                break;
            }

            case ETHER_PHY_LINK_SPEED_100F:
            {
                speed = ETHSW_LINK_SPEED_100F;
                break;
            }

            case ETHER_PHY_LINK_SPEED_10F:
            {
                speed = ETHSW_LINK_SPEED_10F;
                break;
            }

            default:
            {
                err = FSP_ERR_ETHER_ERROR_LINK;
                break;
            }
        }
    }
#endif                                 // GMAC_IMPLEMENT_ETHSW

    if (FSP_SUCCESS == err)
    {
#ifdef GMAC_IMPLEMENT_ETHSW
        ether_switch_instance_t const * p_ethsw_instance = ///< Pointer to ETHER_SWITCH instance
                                                           ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)
                                                           ->
                                                           p_ethsw_instance;

        err = R_ETHSW_SpeedCfg(p_ethsw_instance->p_ctrl, port, speed);
#endif                                 // GMAC_IMPLEMENT_ETHSW
    }
    else
    {
        err = FSP_ERR_ETHER_ERROR_LINK;
    }

    return err;
}                                      /* End of function gmac_do_link() */

/*******************************************************************************************************************//**
 * Set the GMAC operation mode and enable transmission/reception.
 *
 * Note: The flow control cannot be used in the mode of transmitting and receiving via ETHSW.
 *       Therefore, currently, the code for the flow control is left for compatibility with RA.
 *       Operation is not guaranteed when the flow control is enabled.
 *
 * @param[in] p_instance_ctrl           Pointer to the control block for the channel
 *
 * @retval  none
 ***********************************************************************************************************************/
void gmac_configure_operation (gmac_instance_ctrl_t * const p_instance_ctrl)
{
    volatile R_GMAC_Type * p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    uint32_t transmit_pause_set = 0;
    uint32_t receive_pause_set  = 0;
    bool     full_duplex        = false;
    uint32_t flow_control;

    /* Initialize MAC Configuration */
#ifdef GMAC_IMPLEMENT_ETHSW
    {
        p_reg_gmac->MAC_Configuration_b.PS  = 0; /* Port Selec: 1Gbps */
        p_reg_gmac->MAC_Configuration_b.FES = 1; /* Speed: 100Mbps */
        p_reg_gmac->MAC_Configuration_b.DM  = 1; /* Duplex Mode: Full-duple */
    }
#else                                            // GMAC_IMPLEMENT_ETHSW
    {
        switch (p_instance_ctrl->link_speed_duplex)
        {
            /* Half duplex link */
            case ETHER_PHY_LINK_SPEED_1000H:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 0; /* Port Selec: 1Gbps */
                p_reg_gmac->MAC_Configuration_b.FES = 1;
                p_reg_gmac->MAC_Configuration_b.DM  = 0; /* Duplex Mode: Half-duple */
                break;
            }

            case ETHER_PHY_LINK_SPEED_100H:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 1; /* Port Selec: 10Mbps or 100Mbps */
                p_reg_gmac->MAC_Configuration_b.FES = 1; /* Speed Select: 100Mbps */
                p_reg_gmac->MAC_Configuration_b.DM  = 0; /* Duplex Mode: Half-duple */
                break;
            }

            case ETHER_PHY_LINK_SPEED_10H:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 1; /* Port Selec: 10Mbps or 100Mbps */
                p_reg_gmac->MAC_Configuration_b.FES = 0; /* Speed Select: 10Mbps */
                p_reg_gmac->MAC_Configuration_b.DM  = 0; /* Duplex Mode: Half-duple */
                break;
            }

            /* Full duplex link */
            case ETHER_PHY_LINK_SPEED_1000F:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 0; /* Port Selec: 1Gbps */
                p_reg_gmac->MAC_Configuration_b.FES = 1;
                p_reg_gmac->MAC_Configuration_b.DM  = 1; /* Duplex Mode: Full-duple */
                full_duplex = true;
                break;
            }

            case ETHER_PHY_LINK_SPEED_100F:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 1; /* Port Selec: 10Mbps or 100Mbps */
                p_reg_gmac->MAC_Configuration_b.FES = 1; /* Speed Select: 100Mbps */
                p_reg_gmac->MAC_Configuration_b.DM  = 1; /* Duplex Mode: Full-duple */
                full_duplex = true;
                break;
            }

            case ETHER_PHY_LINK_SPEED_10F:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 1; /* Port Selec: 10Mbps or 100Mbps */
                p_reg_gmac->MAC_Configuration_b.FES = 0; /* Speed Select: 10Mbps */
                p_reg_gmac->MAC_Configuration_b.DM  = 1; /* Duplex Mode: Full-duple */
                full_duplex = true;
                break;
            }

            /* Unknown */
            default:
            {
                p_reg_gmac->MAC_Configuration_b.PS  = 0; /* Port Selec: 1Gbps */
                p_reg_gmac->MAC_Configuration_b.FES = 1;
                p_reg_gmac->MAC_Configuration_b.DM  = 1; /* Duplex Mode: Full-duple */
                break;
            }
        }
    }
#endif                                 // GMAC_IMPLEMENT_ETHSW

    /* When pause frame is used */
    if ((ETHER_FLOW_CONTROL_ENABLE == p_instance_ctrl->p_gmac_cfg->flow_control) &&
        (true == full_duplex))
    {
        /* pause frame can be used when  when passing through the switch */
        /* NOTE: not confirmed this operation !!! */

        flow_control = GMAC_FLOW_CONTROL_PT | /* Pause Time (b31-b16) */
                       GMAC_FLOW_CONTROL_PLT; /* Pause Low Threshold */

        /**
         * Enable PAUSE for full duplex link depending on
         * the pause resolution results
         */
        gmac_pause_resolution(p_instance_ctrl->local_pause_bits,
                              p_instance_ctrl->partner_pause_bits,
                              &transmit_pause_set,
                              &receive_pause_set);

        if (GMAC_PAUSE_XMIT_ON == transmit_pause_set)
        {
            /* Enable automatic PAUSE frame transmission */
            flow_control |= GMAC_FLOW_CONTROL_TFE; /* Transmit Flow Control Enable */

            p_reg_gmac->Operation_Mode_b.RFD = 2;  /* Threshold for Deactivating Flow Control = FULL - 3KB */
            p_reg_gmac->Operation_Mode_b.RFA = 1;  /* Threshold for Activating Flow Control = FULL - 2KB */
            p_reg_gmac->Operation_Mode_b.EFC = 1;  /* Enable HW Flow Control */
        }

        if (GMAC_PAUSE_RECV_ON == receive_pause_set)
        {
            /* Enable reception of PAUSE frames */
            flow_control |= GMAC_FLOW_CONTROL_RFE; /* Receive Flow Control Enable */
        }

        p_reg_gmac->Flow_Control = flow_control;
    }
    /* When pause frame is not used */
    else
    {
        p_reg_gmac->Operation_Mode_b.EFC = 0;
        p_reg_gmac->Flow_Control         = 0;
    }

    /* Initialize Operation Mode */
    p_reg_gmac->Operation_Mode_b.RSF = 1; /* Receive Store and Forward */
    p_reg_gmac->Operation_Mode_b.TSF = 1; /* Transmit Store and Forward */
    p_reg_gmac->Operation_Mode_b.SR  = 1; /* Start or Stop Receive */
    p_reg_gmac->Operation_Mode_b.ST  = 1; /* Start or Stop Transmission Command */

    /* CRC Stripping for Type Frames */
    p_reg_gmac->MAC_Configuration_b.CST = 1;

    /* Automatic Pad or CRC Stripping */
    p_reg_gmac->MAC_Configuration_b.ACS = 1;

    /* Enable receive and transmit. */
    p_reg_gmac->MAC_Configuration_b.RE = 1;
    p_reg_gmac->MAC_Configuration_b.TE = 1;
}

/*******************************************************************************************************************//**
 * The magic packet detection is permitted.
 *
 * @param[in]   p_instance_ctrl             Pointer to the control block for the channel
 *
 * @retval      non
 ***********************************************************************************************************************/
void gmac_magic_packet_detection (gmac_instance_ctrl_t * const p_instance_ctrl)
{
    volatile R_GMAC_Type * p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    /* The magic packet detection is permitted. */
    p_reg_gmac->PMT_Control_Status =
        GMAC_PMT_CONTROL_STATUS_MGKPKTEN | /* Magic Packet Enable */
        GMAC_PMT_CONTROL_STATUS_PWRDWN;    /* Power Down */

    /* Because data is not transmitted for the magic packet detection waiting,
     * only the reception is permitted. */
    p_reg_gmac->MAC_Configuration_b.RE = 1;
}

/*******************************************************************************************************************//**
 * The magic packet detection is permitted.
 *
 * @param[in]   p_instance_ctrl             Pointer to the control block for the channel
 *
 * @retval      1: Magic Packet detection is enabled.
 *              0: Magic Packet detection is disabled.
 ***********************************************************************************************************************/
static uint8_t gmac_check_magic_packet_detection_bit (gmac_instance_ctrl_t const * const p_instance_ctrl)
{
    uint8_t ret = 0;

    volatile R_GMAC_Type * p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    if (1 == p_reg_gmac->PMT_Control_Status_b.MGKPKTEN)
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}                                      /* End of function gmac_check_magic_packet_detection_bit() */

/*******************************************************************************************************************//**
 * Verifies the Etherent link is up or not.
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 * @param[in] port              Port number
 *
 * @retval  FSP_SUCCESS:                  Link is up
 * @retval  FSP_ERR_ETHER_ERROR_LINK      Link is down
 * @retval  FSP_ERR_ETHER_PHY_ERROR_LINK  Initialization of PHY-LSI failed.
 * @retval  FSP_ERR_INVALID_ARGUMENT      The ether_phy driver for the specified port cannot be used.
 **********************************************************************************************************************/
static fsp_err_t gmac_link_status_check (gmac_instance_ctrl_t const * const p_instance_ctrl, uint32_t port)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_err_t link_status;

    ether_phy_instance_t const *(*pp_phy_instance)[BSP_FEATURE_GMAC_MAX_PORTS];

    pp_phy_instance = ((gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend)->pp_phy_instance;

    if (NULL == (*pp_phy_instance)[port])
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    link_status = (*pp_phy_instance)[port]->p_api->linkStatusGet((*pp_phy_instance)[port]->p_ctrl);

    if (FSP_ERR_ETHER_PHY_ERROR_LINK == link_status)
    {
        /* Link is down */
        err = FSP_ERR_ETHER_ERROR_LINK;
    }
    else
    {
        /* Link is up */
        err = FSP_SUCCESS;
    }

    return err;
}                                      /* End of function gmac_link_status_check() */

#ifdef GMAC_IMPLEMENT_ETHSW

/*******************************************************************************************************************//**
 * Callback function for link status change from switch
 *
 * @param[in] p_arg     Pointer to the callback parameter
 *
 * @retval  non
 **********************************************************************************************************************/
void gmac_callback_ethsw (ether_switch_callback_args_t * const p_arg)
{
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_arg->p_context;

    if (0 != p_instance_ctrl)
    {
        p_instance_ctrl->link_status = (gmac_port_mask_t) p_arg->status_link;
    }
}                                      /* End of function gmac_callback_ethsw() */

#endif // GMAC_IMPLEMENT_ETHSW

/*******************************************************************************************************************//**
 * Calls user callback.
 *
 * @param[in]     p_instance_ctrl      Pointer to ether instance control block
 * @param[in]     p_callback_args      Pointer to callback args
 **********************************************************************************************************************/
static void gmac_call_callback (gmac_instance_ctrl_t * p_instance_ctrl, ether_callback_args_t * p_callback_args)
{
    ether_callback_args_t args;

    /* Store callback arguments in memory provided by user if available.  This allows callback arguments to be
     * stored in non-secure memory so they can be accessed by a non-secure callback function. */
    ether_callback_args_t * p_args = p_instance_ctrl->p_callback_memory;
    if (NULL == p_args)
    {
        /* Store on stack */
        p_args = &args;
    }
    else
    {
        /* Save current arguments on the stack in case this is a nested interrupt. */
        args = *p_args;
    }

    p_args->channel      = p_instance_ctrl->p_gmac_cfg->channel;
    p_args->event        = p_callback_args->event;
    p_args->status_ether = p_callback_args->status_ether;
    p_args->status_link  = p_callback_args->status_link;
    p_args->p_context    = p_instance_ctrl->p_context;

    /* If the project is not Trustzone Secure, then it will never need to change security state in order to call the callback. */
    p_instance_ctrl->p_callback(p_args);

    if (NULL != p_instance_ctrl->p_callback_memory)
    {
        /* Restore callback memory in case this is a nested interrupt. */
        *p_instance_ctrl->p_callback_memory = args;
    }
}                                      /* End of function gmac_call_callback() */

/*******************************************************************************************************************//**
 * Interrupt handler for SBD interrupts.
 *
 * @retval      none
 **********************************************************************************************************************/
void gmac_isr_sbd (void)
{
    ether_callback_args_t  callback_arg;
    volatile R_GMAC_Type * p_reg_gmac;
    uint32_t               status;
    volatile uint32_t      dammy_read;

    IRQn_Type              irq             = R_FSP_CurrentIrqGet();
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    status             = p_reg_gmac->Status;
    p_reg_gmac->Status = status;

    /* Dammy read for Countermeasure when the CPU clock is 400 or 800MHz */
    dammy_read = p_reg_gmac->Status;
    dammy_read = dammy_read;

    /* Callback : Interrupt handler */
    if (NULL != p_instance_ctrl->p_callback)
    {
        callback_arg.channel      = p_instance_ctrl->p_gmac_cfg->channel;
        callback_arg.event        = ETHER_EVENT_SBD_INTERRUPT;
        callback_arg.status_ether = status;
        callback_arg.status_link  = 0;
        callback_arg.p_context    = p_instance_ctrl->p_gmac_cfg->p_context;

        gmac_call_callback(p_instance_ctrl, &callback_arg);
    }
}                                      /* End of function gmac_isr_sbd() */

/*******************************************************************************************************************//**
 * Interrupt handler for PMT interrupts.
 *
 * @retval      none
 **********************************************************************************************************************/
void gmac_isr_pmt (void)
{
    ether_callback_args_t  callback_arg;
    volatile R_GMAC_Type * p_reg_gmac;
    uint32_t               pmt_control_status;

    IRQn_Type              irq             = R_FSP_CurrentIrqGet();
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    p_reg_gmac = (R_GMAC_Type *) p_instance_ctrl->p_reg_gmac;

    pmt_control_status = p_reg_gmac->PMT_Control_Status;

    /* Callback : Interrupt handler */
    if (NULL != p_instance_ctrl->p_callback)
    {
        callback_arg.channel      = p_instance_ctrl->p_gmac_cfg->channel;
        callback_arg.event        = ETHER_EVENT_PMT_INTERRUPT;
        callback_arg.status_ether = pmt_control_status;
        callback_arg.status_link  = 0;
        callback_arg.p_context    = p_instance_ctrl->p_gmac_cfg->p_context;

        gmac_call_callback(p_instance_ctrl, &callback_arg);
    }

    if ((GMAC_PMT_CONTROL_STATUS_MGKPRCVD == (GMAC_PMT_CONTROL_STATUS_MGKPRCVD & pmt_control_status)) ||
        (GMAC_PMT_CONTROL_STATUS_PWRDWN != (GMAC_PMT_CONTROL_STATUS_PWRDWN & pmt_control_status)))
    {
        p_instance_ctrl->magic_packet = GMAC_MAGIC_PACKET_DETECTED;
    }
    else
    {
        p_reg_gmac->PMT_Control_Status =
            GMAC_PMT_CONTROL_STATUS_MGKPKTEN | /* Magic Packet Enable */
            GMAC_PMT_CONTROL_STATUS_PWRDWN;    /* Power Down */
    }
}                                              /* End of function gmac_isr_pmt() */

/*******************************************************************************************************************//**
 * Enable ICU
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 *
 * @retval      none
 **********************************************************************************************************************/
static void gmac_enable_icu (gmac_instance_ctrl_t * const p_instance_ctrl)
{
    gmac_extend_cfg_t * p_extend = (gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend;

    /** Configure the Ethernet interrupt. */
    R_BSP_IrqCfgEnable(p_instance_ctrl->p_gmac_cfg->irq,
                       p_instance_ctrl->p_gmac_cfg->interrupt_priority,
                       p_instance_ctrl);

    R_BSP_IrqCfgEnable(p_extend->pmt_irq, p_extend->pmt_interrupt_priority, p_instance_ctrl);
}                                      /* End of function gmac_enable_icu() */

/*******************************************************************************************************************//**
 * Disable ICU
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 *
 * @retval      none
 **********************************************************************************************************************/
static void gmac_disable_icu (gmac_instance_ctrl_t * const p_instance_ctrl)
{
    gmac_extend_cfg_t * p_extend = (gmac_extend_cfg_t *) p_instance_ctrl->p_gmac_cfg->p_extend;

    R_BSP_IrqDisable(p_instance_ctrl->p_gmac_cfg->irq);
    R_FSP_IsrContextSet(p_instance_ctrl->p_gmac_cfg->irq, NULL);

    R_BSP_IrqDisable(p_extend->pmt_irq);
    R_FSP_IsrContextSet(p_extend->pmt_irq, NULL);
}                                      /* End of function gmac_disable_icu() */
