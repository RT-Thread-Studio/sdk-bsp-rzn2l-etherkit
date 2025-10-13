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
#include "r_ethsw.h"
#include <math.h>

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

#ifndef ETHSW_ERROR_RETURN

/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
 #define ETHSW_ERROR_RETURN(a, err)    FSP_ERROR_RETURN((a), (err))
#endif                                 /* !ETHSW_ERROR_RETURN */

/** "SWIT" in ASCII.  Used to determine if the control block is open. */
#define ETHSW_OPEN                            (0x53574954U)

/* Initial value of down counter for timeout detection */
#define ETHSW_TIMEOUT_COUNT                   (1000000000U)

/* Maximum Frame Length */
#define ETHSW_FRAME_SIZE                      (0x2408U)

/* ENABLE PORT Bit Definitions(PORT_ENA,UCAST_DEFAULT_MASK,BCAST_DEFAULT_MASK,MCAST_DEFAULT_MASK Register) */
#define ETHSW_ENABLE_PORT0                    (1UL << 0)
#define ETHSW_ENABLE_PORT1                    (1UL << 1)
#define ETHSW_ENABLE_PORT2                    (1UL << 2)
#define ETHSW_ENABLE_PORT3                    (1UL << 3)

/* COMMAND_CONFIG Register Bit Definitions */
#define ETHSW_COMMAND_CONFIG_ENA_10           (1UL << 25)
#define ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK    (1UL << 24)
#define ETHSW_COMMAND_CONFIG_CNTL_FRM_ENA     (1UL << 23)
#define ETHSW_COMMAND_CONFIG_LOOP_ENA         (1UL << 15)
#define ETHSW_COMMAND_CONFIG_SW_RESET         (1UL << 13)
#define ETHSW_COMMAND_CONFIG_TX_CRC_APPEND    (1UL << 11)
#define ETHSW_COMMAND_CONFIG_HD_ENA           (1UL << 10)
#define ETHSW_COMMAND_CONFIG_TX_ADDR_INS      (1UL << 9)
#define ETHSW_COMMAND_CONFIG_PAUSE_IGNORE     (1UL << 8)
#define ETHSW_COMMAND_CONFIG_PAUSE_FWD        (1UL << 7)
#define ETHSW_COMMAND_CONFIG_CRC_FWD          (1UL << 6)
#define ETHSW_COMMAND_CONFIG_PAD_EN           (1UL << 5)
#define ETHSW_COMMAND_CONFIG_PROMIS_EN        (1UL << 4)
#define ETHSW_COMMAND_CONFIG_ETH_SPEED        (1UL << 3)
#define ETHSW_COMMAND_CONFIG_RX_ENA           (1UL << 1)
#define ETHSW_COMMAND_CONFIG_TX_ENA           (1UL << 0)

/* INT_CONFIG Register Bit Number Definitions */
#define ETHSW_INT_CONFIG_PATTERN_INT          (1UL << 31)
#define ETHSW_INT_CONFIG_TDMA_INT             (1UL << 30)
#define ETHSW_INT_CONFIG_TSM_INT              (1UL << 29)
#ifdef  ETHSW_EFP_FEATURE_SUPPORTED
 #define ETHSW_INT_CONFIG_EFP_INT             (1UL << 27)
#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */
#define ETHSW_INT_CONFIG_IRQ_LNK_SHIFT        (8U)
#define ETHSW_INT_CONFIG_IRQ_LNK_MASK         (0x7 << ETHSW_INT_CONFIG_IRQ_LNK_SHIFT)
#define ETHSW_INT_CONFIG_DLR_INT              (1UL << 5)
#define ETHSW_INT_CONFIG_IRQ_EN               (1UL << 0)

#define ETHSW_HEX_000000FF                    (0x000000ffU)
#define ETHSW_HEX_0000FF00                    (0x0000ff00U)
#define ETHSW_HEX_00FF0000                    (0x00ff0000U)
#define ETHSW_HEX_FF000000                    (0xff000000U)
#define ETHSW_HEX_0000FFFF                    (0x0000ffffU)
#define ETHSW_DEC_100                         (100U)
#define ETHSW_DEC_512                         (512U)
#define ETHSW_DEC_1000                        (1000U)

#define ETHSW_HOST_PORT_NUMBER                (3U)
#define ETHSW_PORT_COUNT                      (3U) ///< Number of External port
#define ETHSW_PORT_COUNT_ALL                  (4U) ///< Number of All port (include managemant port)

/* Bit Mask for register access */
#define ETHSW_1BIT_MASK                       (0x1U)
#define ETHSW_2BITS_MASK                      (0x3U)
#define ETHSW_3BITS_MASK                      (0x7U)
#define ETHSW_4BITS_MASK                      (0xfU)
#define ETHSW_9BITS_MASK                      (0x1ffU)
#define ETHSW_12BITS_MASK                     (0xfffU)

/* LK_CTRL Register Bit Definitions */
#define ETHSW_LK_MASK                         (0x9E)
#define ETHSW_LK_PERVLAN                      (7)
#define ETHSW_LK_DISCUNKN                     (4)
#define ETHSW_LK_MIGRAT                       (3)
#define ETHSW_LK_AGING                        (2)
#define ETHSW_LK_LEARN                        (1)
#define ETHSW_LK_ADDR_CLEAR                   (1 << 29)
#define ETHSW_LK_ADDR_LOOKUP                  (1 << 28)
#define ETHSW_LK_ADDR_VALID                   (1)
#define ETHSW_LK_ADDR_WRITE                   (1 << 25)
#define ETHSW_LK_ADDR_REG_VALID               (1 << 16)
#define ETHSW_LK_ADDR_REG_TYPE                (1 << 17)
#define ETHSW_LK_ADDR_PORTMASK                (0xF << 21)
#define ETHSW_LK_ADDR_PRIOMASK                (0x7 << 18)
#define ETHSW_LK_ADDR_PORT_OFFSET             (5)
#define ETHSW_LK_ADDR_PORT_REG_OFFSET         (21)
#define ETHSW_LK_ADDR_PRIO_REG_OFFSET         (18)
#define ETHSW_LK_ADDR_MASKANDFLAGS_MASK       (0xFFFFU << 16)

/* FRM_LENGTH_P[n] Register Bit Definitions */
#define ETHSW_FRAMESIZE_MASK                  0x3FFF ///< maximum frame size mask

#define ETHSW_PORT_TX_MSK                     (1)    ///< COMMAND_CONFIG_P[n] - mask for tx_ena
#define ETHSW_PORT_RX_MSK                     (2)    ///< COMMAND_CONFIG_P[n] - mask for rx_ena
#define ETHSW_PORT_RXTX                       (ETHSW_PORT_TX_MSK | ETHSW_PORT_RX_MSK)

#define ETHSW_PHY_LINK                        (4)

/* Register offset for Port-n */
#define ETHSW_PORT_OFFSET                     (0x400)
#define ETHSW_PORT_OFFSET_4BYTES              (0x04)
#define ETHSW_PORT_OFFSET_8BYTES              (0x08)
#define ETHSW_PORT_OFFSET_16BYTES             (0x10)
#define ETHSW_PORT_OFFSET_32BYTES             (0x20)

/* DLR_IRQ_CONTROL Register Bit Definitions */
#define ETHSW_TGT_DLR_INT_NEWSTATUS           (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_state_chng_ena_Pos)   ///< new ring status
#define ETHSW_TGT_DLR_INT_BEACONTMO_0         (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_bec_tmr0_exp_ena_Pos) ///< beacon timeout port 0
#define ETHSW_TGT_DLR_INT_BEACONTMO_1         (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_bec_tmr1_exp_ena_Pos) ///< beacon timeout port 1
#define ETHSW_TGT_DLR_INT_SUVICHNG            (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_supr_chng_ena_Pos)    ///< Supervisor changed
#define ETHSW_TGT_DLR_INT_LNKCHNG_0           (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_link_chng0_ena_Pos)   ///< link status changed port 0
#define ETHSW_TGT_DLR_INT_LNKCHNG_1           (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_link_chng1_ena_Pos)   ///< link status changed port 1
#define ETHSW_TGT_DLR_INT_BECONRCV_0          (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_bec_rcv0_ena_Pos)     ///< beacon frame received port 0
#define ETHSW_TGT_DLR_INT_BECONRCV_1          (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_bec_rcv1_ena_Pos)     ///< beacon frame received port 1
#define ETHSW_TGT_DLR_INT_LOCALLOOP_0         (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_frm_dscrd0_Pos)       ///< loop detected port 0
#define ETHSW_TGT_DLR_INT_LOCALLOOP_1         (1 << R_ETHSW_DLR_IRQ_CONTROL_IRQ_frm_dscrd1_Pos)       ///< loop detected port 1

#define ETHSW_RN_ADVSW_DLR_ETHTYPE            (0x80E1U)                                               ///< DLR EtherType for DLR module
#define ETHSW_RN_ADVSW_DLR_TICKS              (200U)                                                  ///< ticks for 1 us

/* Multicast MAC Address used by DLR */
#define ETHSW_MULTICAST_BEACON                {0x01, 0x21, 0x6c, 0x00, 0x00, 0x01}
#define ETHSW_MULTICAST_DLR                   {0x01, 0x21, 0x6c, 0x00, 0x00, 0x00}

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

/* extended frame parser related definitions */
 #define ETHSW_EFP_DMACI_SZ                   (6U)
 #define ETHSW_EFP_DVIDI_SZ                   (2U)
 #define ETHSW_EFP_VICM_NUM_MAX               (4U)
 #define ETHSW_EFP_SID_NUM_MAX                (8U)
 #define ETHSW_EFP_GAID_NUM_MAX               (8U)
 #define ETHSW_EFP_MEID_NUM_MAX               (8U)
 #define ETHSW_EFP_ASI_ADDR_NUM               (128U)
 #define ETHSW_EFP_ASI_ADDR_SZ                (7U)
 #define ETHSW_EFP_ASI_HASH_DA_SZ             (48U)
 #define ETHSW_EFP_ASI_HASH_VID_SZ            (12U)
 #define ETHSW_EFP_ASI_HASH_VID_MASK          (0x0FFFU)
 #define ETHSW_EFP_ASI_HASH_SZ                (64U)

 #define ETHSW_COMMAND_CONFIG_OFFSET          (0x100U)
 #define ETHSW_VLAN_MODE_OFFSET               (0x100U)
 #define ETHSW_VLAN_TAG_OFFSET                (0x100U)
 #define ETHSW_PCP_REMAP_OFFSET               (0x100U)
 #define ETHSW_QSFTBL_OFFSET_PORT             (0x100U)
 #define ETHSW_QSFTBL_OFFSET_SID              (0x0AU)
 #define ETHSW_QSTMACU_OFFSET_PORT            (0x100U)
 #define ETHSW_QSTMACU_OFFSET_SID             (0x0AU)
 #define ETHSW_QSTMACD_OFFSET_PORT            (0x100U)
 #define ETHSW_QSTMACD_OFFSET_SID             (0x0AU)
 #define ETHSW_QSTMAMU_OFFSET_PORT            (0x100U)
 #define ETHSW_QSTMAMU_OFFSET_SID             (0x0AU)
 #define ETHSW_QSTMAMD_OFFSET_PORT            (0x100U)
 #define ETHSW_QSTMAMD_OFFSET_SID             (0x0AU)
 #define ETHSW_QSFTVL_OFFSET_PORT             (0x100U)
 #define ETHSW_QSFTVL_OFFSET_SID              (0x0AU)
 #define ETHSW_QSFTVLM_OFFSET_PORT            (0x100U)
 #define ETHSW_QSFTVLM_OFFSET_SID             (0x0AU)
 #define ETHSW_QSFTBL_OFFSET_PORT             (0x100U)
 #define ETHSW_QSFTBL_OFFSET_SID              (0x0AU)
 #define ETHSW_QGMOD_OFFSET                   (0x100U)
 #define ETHSW_QMDESC_OFFSET_PORT             (0x100U)
 #define ETHSW_QMDESC_OFFSET_METER            (0x05U)
 #define ETHSW_QMCBSC_OFFSET_PORT             (0x100U)
 #define ETHSW_QMCBSC_OFFSET_METER            (0x05U)
 #define ETHSW_QMCIRC_OFFSET_PORT             (0x100U)
 #define ETHSW_QMCIRC_OFFSET_METER            (0x05U)
 #define ETHSW_QMEC_OFFSET                    (0x100U)
 #define ETHSW_QSEIE_OFFSET                   (0x100U)
 #define ETHSW_QSEID_OFFSET                   (0x100U)
 #define ETHSW_QGEIE_OFFSET                   (0x100U)
 #define ETHSW_QGEID_OFFSET                   (0x100U)
 #define ETHSW_QMEIE_OFFSET                   (0x100U)
 #define ETHSW_QMEID_OFFSET                   (0x100U)
 #define ETHSW_ERROR_MASK_OFFSET              (0x100U)
 #define ETHSW_QSEIS_OFFSET                   (0x100U)
 #define ETHSW_QGEIS_OFFSET                   (0x100U)
 #define ETHSW_QMEIS_OFFSET                   (0x100U)
 #define ETHSW_ERROR_STATUS_OFFSET            (0x100U)
 #define ETHSW_QSMOI_MASK                     (0xFFU)
 #define ETHSW_QGMOI_MASK                     (0xFFU)
 #define ETHSW_QRFI_MASK                      (0xFFU)
 #define ETHSW_ERROR_STATUS_MASK              (R_ETHSW_P0_ERROR_STATUS_SOPERR_Msk | R_ETHSW_P0_ERROR_STATUS_PUNDSZ_Msk | \
                                               R_ETHSW_P0_ERROR_STATUS_POVRSZ_Msk | R_ETHSW_P0_ERROR_STATUS_EUNDSZ_Msk | \
                                               R_ETHSW_P0_ERROR_STATUS_EOVRSZ_Msk)
 #define ETHSW_CHANNEL_ENABLE_MASK            (R_ETHSW_CHANNEL_ENABLE_CH0ENA_Msk | R_ETHSW_CHANNEL_ENABLE_CH1ENA_Msk | \
                                               R_ETHSW_CHANNEL_ENABLE_CH2ENA_Msk)
 #define ETHSW_CHANNEL_DISABLE_MASK           (R_ETHSW_CHANNEL_DISABLE_CH0DIS_Msk | R_ETHSW_CHANNEL_DISABLE_CH1DIS_Msk | \
                                               R_ETHSW_CHANNEL_DISABLE_CH2DIS_Msk)
#endif                                              /* ETHSW_EFP_FEATURE_SUPPORTED */

/* driver local define */
#define ETHSW_RX_PATTERN_MATCHER_COUNT        (12U) ///< Number of RX Pattern Matcher
#define ETHSW_TIME_COUNT                      (2U)  ///< Number of timer
#define ETHSW_GPIO_COUNT                      (8U)  ///< Number of timer
#define ETHSW_PULSE_GENERATOR_COUNT           (4U)  ///< Number of Pulse Gennrator

/* FramePreemption register access offset Definitions */
#define ETHSW_BR_CONTROL_OFFSET               (0x100)
#define ETHSW_STATUS_OFFSET                   (0x100)

/* MMCTL_QTRIG Register Bit Definitions */
#define ETHSW_MMCTL_QTRIG_QUEUE_TRIG_SHIFT    (16U)
#define ETHSW_MMCTL_QTRIG_QUEUE_TRIG_MASK     (0xffffU << ETHSW_MMCTL_QTRIG_QUEUE_TRIG_SHIFT)
#define ETHSW_MMCTL_QTRIG_PORT_MASK_SHIFT     (0U)
#define ETHSW_MMCTL_QTRIG_PORT_MASK_MASK      (0xfU << ETHSW_MMCTL_QTRIG_PORT_MASK_SHIFT)

/* Define for BCAST_STORM_LIMIT register */
#define ETHSW_OPERATING_CLOCK_CYCLES          (200U * 1000 * 1000)
#define ETHSW_DOS_TIMEFACTOR                  (65536U)

/* Define for constant value */
#define ETHSW_DEC_100                         (100U)
#define ETHSW_DEC_512                         (512U)

/* ASI_MEM_ADDR Register Bit Definitions */
#define ETHSW_MEM_WEN_ENABLE                  (0x80U)
#define ETHSW_MEM_REQ_ALL_ACCESS              (0x700U)

/* Nanosecond count value for 1 second */
#define NANO_COUNT_FOR_1SEC                   (1000000000)

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/

typedef union
{
    ethsw_mac_addr_t mac_addr;         /* MAC address                                  */
    uint32_t         reg_data[2];      /* datas for LK_DATA_LO and LK_DATA_HI register */
} ethsw_mac_addr_reg_t;

/** MAC table entry(for register access) */
typedef struct st_ethsw_mactab_entry_reg
{
    ethsw_mac_addr_t mac;              ///< MAC address
    uint16_t         mask_and_flags;   ///< Port bit mask and flags
} ethsw_mactab_entry_reg_t;

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

static void ethsw_enable_icu(ethsw_instance_ctrl_t * const p_instance_ctrl);

static void ethsw_disable_icu(ethsw_instance_ctrl_t * const p_instance_ctrl);

void ethsw_isr_intr(void);

static void ethsw_isr_dlr(ethsw_instance_ctrl_t * p_instance_ctrl);
static void ethsw_isr_tdma(ethsw_instance_ctrl_t * p_instance_ctrl);
static void ethsw_isr_tsm(ethsw_instance_ctrl_t * p_instance_ctrl);

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED
static void    ethsw_isr_efp(ethsw_instance_ctrl_t * p_instance_ctrl);
static void    r_ethsw_make_crc(char * bit_str, uint32_t len, char * crc_buf);
static uint8_t r_ethsw_calc_hash(ethsw_mac_addr_t * dmaci, ethsw_vlan_vid_t * vidi);

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

static void ethsw_isr_rx_pattern_matcher(ethsw_instance_ctrl_t * p_instance_ctrl);

static void r_ethsw_reg_protection_disable(void * const p_reg_base);
static void r_ethsw_reg_protection_enable(void * const p_reg_base);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

#if defined(__GNUC__)

/* This structure is affected by warnings from a GCC compiler bug. This pragma suppresses the warnings in this
 * structure only.*/

/*LDRA_INSPECTED 69 S */
 #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

/** ETHER SWITCH HAL API mapping for Ethernet Controller interface */
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const ether_switch_api_t g_ether_switch_on_ethsw =
{
    .open  = R_ETHSW_Open,
    .close = R_ETHSW_Close,
};

void (* gp_ethsw_dlr_callback)(        ///< pointer of callback function called when an DLR Interupt occurs
    ethsw_dlr_event_t event,
    uint32_t          port);

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED
void (* gp_ethsw_efp_callback)(        ///< pointer of callback function called when an EFP Interupt occurs
    uint32_t          port,
    ethsw_efp_event_t id);
#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

void (* gp_ethsw_rx_pattern_callback)( ///< pointer of callback function called when an Rx pattern matcher interupt occurs
    ethsw_rx_pattern_event_t        event,
    ethsw_rx_pattern_event_data_t * p_data);

void (* gp_ethsw_tdma_callback)(       ///< pointer of callback function called when an TDMA Interupt occurs
    ethsw_tdma_event_t           event,
    ethsw_tdma_callback_data_t * p_tdma_data);

void (* gp_ethsw_time_callback)(       ///< pointer of callback function called when an PTP Interupt occurs
    ethsw_time_event_t  event,
    uint32_t            port,
    ethsw_timestamp_t * p_timestamp);

/*******************************************************************************************************************//**
 * @addtogroup ETHER_SWITCH
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/********************************************************************************************************************//**
 * Open the switch ports and gets it ready to pass through the frames.
 *
 * @retval  FSP_SUCCESS                 Channel opened successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER SWITCH control block or configuration structure is NULL.
 * @retval  FSP_ERR_ALREADY_OPEN        Control block has already been opened or channel is being used by another
 *                                      instance. Call close() then open() to reconfigure.
 * @retval  FSP_ERR_INVALID_CHANNEL     Invalid channel number is given.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Interrupt is not enabled.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_Open (ether_switch_ctrl_t * const p_ctrl, ether_switch_cfg_t const * const p_cfg)
{
    fsp_err_t               err             = FSP_SUCCESS;
    ethsw_instance_ctrl_t * p_instance_ctrl =
        (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN((NULL != p_cfg), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((BSP_FEATURE_ETHSW_MAX_CHANNELS > p_cfg->channel), FSP_ERR_INVALID_CHANNEL);
    ETHSW_ERROR_RETURN((0 <= p_cfg->irq), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((ETHSW_OPEN != p_instance_ctrl->open), FSP_ERR_ALREADY_OPEN);

    if (ETHSW_PHYLINK_ENABLE == ((ethsw_extend_cfg_t *) p_cfg->p_extend)->phylink)
    {
        ETHSW_ERROR_RETURN((NULL != p_cfg->p_callback), FSP_ERR_INVALID_POINTER);
        ETHSW_ERROR_RETURN((NULL != p_cfg->p_context), FSP_ERR_INVALID_POINTER);
    }
#endif

    p_instance_ctrl->p_reg_switch    = (void *) R_ETHSW;
    p_instance_ctrl->p_reg_ethsw_ptp = (void *) R_ETHSW_PTP;
    p_instance_ctrl->p_reg_ethss     = (void *) R_ETHSS;

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* Initialize configuration of Ethernet switch module. */
    p_instance_ctrl->p_switch_cfg = p_cfg;

    /* Module start */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_MODULE_START(FSP_IP_ETHSW, p_cfg->channel);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_ETHSW);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* set Maximum Frame Length size */
    p_reg_switch->FRM_LENGTH_P0_b.FRM_LENGTH = ETHSW_FRAME_SIZE;
    p_reg_switch->FRM_LENGTH_P1_b.FRM_LENGTH = ETHSW_FRAME_SIZE;
    p_reg_switch->FRM_LENGTH_P2_b.FRM_LENGTH = ETHSW_FRAME_SIZE;
    p_reg_switch->FRM_LENGTH_P3_b.FRM_LENGTH = ETHSW_FRAME_SIZE;

    /* Initialize COMMAND_CONFIG_p[0] Reg */
    p_reg_switch->COMMAND_CONFIG_P0 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND |
                                       ETHSW_COMMAND_CONFIG_ETH_SPEED | ETHSW_COMMAND_CONFIG_RX_ENA |
                                       ETHSW_COMMAND_CONFIG_TX_ENA);

    /* Initialize COMMAND_CONFIG_p[1] Reg */
    p_reg_switch->COMMAND_CONFIG_P1 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND |
                                       ETHSW_COMMAND_CONFIG_ETH_SPEED | ETHSW_COMMAND_CONFIG_RX_ENA |
                                       ETHSW_COMMAND_CONFIG_TX_ENA);

    /* Initialize COMMAND_CONFIG_p[2] Reg */
    p_reg_switch->COMMAND_CONFIG_P2 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND |
                                       ETHSW_COMMAND_CONFIG_ETH_SPEED | ETHSW_COMMAND_CONFIG_RX_ENA |
                                       ETHSW_COMMAND_CONFIG_TX_ENA);

    /* Initialize COMMAND_CONFIG_p[3] Reg */
    p_reg_switch->COMMAND_CONFIG_P3 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND |
                                       ETHSW_COMMAND_CONFIG_ETH_SPEED | ETHSW_COMMAND_CONFIG_RX_ENA |
                                       ETHSW_COMMAND_CONFIG_TX_ENA);

    /* Initialize PORT ENABLE Reg */
    p_reg_switch->PORT_ENA_b.RXENA =
        (ETHSW_ENABLE_PORT0 | ETHSW_ENABLE_PORT1 | ETHSW_ENABLE_PORT2 | ETHSW_ENABLE_PORT3);
    p_reg_switch->PORT_ENA_b.TXENA =
        (ETHSW_ENABLE_PORT0 | ETHSW_ENABLE_PORT1 | ETHSW_ENABLE_PORT2 | ETHSW_ENABLE_PORT3);

    /* Initialize Learning/Lookup function global configuration Reg */
    p_reg_switch->LK_CTRL            = 0;
    p_reg_switch->LK_CTRL_b.ALW_MGRT = 1;
    p_reg_switch->LK_CTRL_b.AGING_EN = 1;
    p_reg_switch->LK_CTRL_b.LEARN_EN = 1;
    p_reg_switch->LK_CTRL_b.LKUP_EN  = 1;

    /* enable Default unicast resolution */
    p_reg_switch->UCAST_DEFAULT_MASK0_b.UCASTDM =
        (ETHSW_ENABLE_PORT0 | ETHSW_ENABLE_PORT1 | ETHSW_ENABLE_PORT2 | ETHSW_ENABLE_PORT3);

    /* enable Default broadcast resolution */
    p_reg_switch->BCAST_DEFAULT_MASK0_b.BCASTDM =
        (ETHSW_ENABLE_PORT0 | ETHSW_ENABLE_PORT1 | ETHSW_ENABLE_PORT2 | ETHSW_ENABLE_PORT3);

    /* enable Default multicast resolution */
    p_reg_switch->MCAST_DEFAULT_MASK0_b.MCASTDM =
        (ETHSW_ENABLE_PORT0 | ETHSW_ENABLE_PORT1 | ETHSW_ENABLE_PORT2 | ETHSW_ENABLE_PORT3);

    /* set port in authorized state(Port 0 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[0].AUTH = 1;

    /* set port in authorized state(Port 1 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[1].AUTH = 1;

    /* set port in authorized state(Port 2 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[2].AUTH = 1;

    /* set port in authorized state(Port 3 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[3].AUTH = 1;

    /* Initialize MGMT_TAG_CONFIG Reg */
    if (ETHSW_SPECIFIC_TAG_ENABLE == ((ethsw_extend_cfg_t *) p_cfg->p_extend)->specific_tag)
    {
        p_reg_switch->MGMT_TAG_CONFIG_b.TAGFIELD  = ((ethsw_extend_cfg_t *) p_cfg->p_extend)->specific_tag_id;
        p_reg_switch->MGMT_TAG_CONFIG_b.TYPE2_EN  = 0;
        p_reg_switch->MGMT_TAG_CONFIG_b.TYPE1_EN  = 0;
        p_reg_switch->MGMT_TAG_CONFIG_b.AL_FRAMES = 1;
        p_reg_switch->MGMT_TAG_CONFIG_b.ENABLE    = 1;
    }
    else
    {
        p_reg_switch->MGMT_TAG_CONFIG_b.ENABLE = 0;
    }

    /* Set Ethernet Switch interrupt level and enable */
    ethsw_enable_icu(p_instance_ctrl);

    if (ETHSW_PHYLINK_ENABLE == ((ethsw_extend_cfg_t *) p_cfg->p_extend)->phylink)
    {
        /* Set line port phy link change interrupt enable Bit (PORT0 - 2) */
        p_reg_switch->INT_CONFIG_b.IRQ_LINK = (ETHSW_ENABLE_PORT0 | ETHSW_ENABLE_PORT1 | ETHSW_ENABLE_PORT2);

        /* Set Interrupt global enable */
        p_reg_switch->INT_CONFIG_b.IRQ_EN = 1;
    }

    p_instance_ctrl->open = ETHSW_OPEN;

    return err;
}                                      /* End of function R_ETHSW_Open() */

/********************************************************************************************************************//**
 * Close the switch ports
 *
 * @retval  FSP_SUCCESS                                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened
 *
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_Close (ether_switch_ctrl_t * const p_ctrl)
{
    fsp_err_t               err             = FSP_SUCCESS;
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;

    R_ETHSW_Type * p_reg_switch;

    p_reg_switch = p_instance_ctrl->p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    /* Set line port phy link change interrupt disable Bit (PORT0 - 2) */
    p_reg_switch->INT_CONFIG_b.IRQ_LINK = 0;

    /* Set Interrupt global disable */
    p_reg_switch->INT_CONFIG_b.IRQ_EN = 0;

    /* set port in unauthorized state(Port 0 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[0].AUTH = 0;

    /* set port in unauthorized state(Port 1 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[1].AUTH = 0;

    /* set port in unauthorized state(Port 2 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[2].AUTH = 0;

    /* set port in unauthorized state(Port 3 authentication control and configuration Reg) */
    p_reg_switch->AUTH_PORT_b[3].AUTH = 0;

    /* clear Maximum Frame Length Value */
    p_reg_switch->FRM_LENGTH_P0_b.FRM_LENGTH = ETHSW_FRAME_SIZE;
    p_reg_switch->FRM_LENGTH_P1_b.FRM_LENGTH = ETHSW_FRAME_SIZE;
    p_reg_switch->FRM_LENGTH_P2_b.FRM_LENGTH = ETHSW_FRAME_SIZE;
    p_reg_switch->FRM_LENGTH_P3_b.FRM_LENGTH = ETHSW_FRAME_SIZE;

    /* Disable PORT Rx Tx */
    p_reg_switch->PORT_ENA_b.RXENA = 0U;
    p_reg_switch->PORT_ENA_b.TXENA = 0U;

    /* Disable the MAC reception function(COMMAND_CONFIG_p[1] Reg) */
    p_reg_switch->COMMAND_CONFIG_P0 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND);

    /* Disable the MAC reception function(COMMAND_CONFIG_p[2] Reg) */
    p_reg_switch->COMMAND_CONFIG_P1 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND);

    /* Disable the MAC reception function(COMMAND_CONFIG_p[2] Reg) */
    p_reg_switch->COMMAND_CONFIG_P2 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND);

    /* Disable the MAC reception function(COMMAND_CONFIG_p[3] Reg) */
    p_reg_switch->COMMAND_CONFIG_P3 = (ETHSW_COMMAND_CONFIG_NO_LGTH_CHECK | ETHSW_COMMAND_CONFIG_TX_CRC_APPEND);

    /* Disable Ethernet interrupt. */
    ethsw_disable_icu(p_instance_ctrl);

    /** Clear configure block parameters. */
    p_instance_ctrl->p_switch_cfg = NULL;

    /** Mark the channel not open so other APIs cannot use it. */
    p_instance_ctrl->open = 0U;

    return err;
}                                      /* End of function R_ETHER_Close() */

/********************************************************************************************************************//**
 * Configs speed and duplex of the port
 *
 * @retval  FSP_SUCCESS                                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION                           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT                    Invalid input parameter.
 *
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_SpeedCfg (ether_switch_ctrl_t * const p_ctrl, uint32_t const port, ethsw_link_speed_t const speed)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint32_t volatile     * p_reg_command_config;
    uint32_t                command_config;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    switch (port)
    {
        case 0:
        {
            p_reg_command_config = &p_reg_switch->COMMAND_CONFIG_P0;
            break;
        }

        case 1:
        {
            p_reg_command_config = &p_reg_switch->COMMAND_CONFIG_P1;
            break;
        }

        case 2:
        {
            p_reg_command_config = &p_reg_switch->COMMAND_CONFIG_P2;
            break;
        }

        default:
        {
            err = FSP_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    if (err == FSP_SUCCESS)
    {
        command_config = *p_reg_command_config;

        command_config &= (uint32_t) ~(ETHSW_COMMAND_CONFIG_ENA_10 |
                                       ETHSW_COMMAND_CONFIG_HD_ENA |
                                       ETHSW_COMMAND_CONFIG_ETH_SPEED);
        switch (speed)
        {
            case ETHSW_LINK_SPEED_10H:
            {
                command_config |= (ETHSW_COMMAND_CONFIG_ENA_10 |
                                   ETHSW_COMMAND_CONFIG_HD_ENA);
                break;
            }

            case ETHSW_LINK_SPEED_10F:
            {
                command_config |= (ETHSW_COMMAND_CONFIG_ENA_10);
                break;
            }

            case ETHSW_LINK_SPEED_100H:
            {
                command_config |= (ETHSW_COMMAND_CONFIG_HD_ENA);
                break;
            }

            case ETHSW_LINK_SPEED_100F:
            {
                break;
            }

            case ETHSW_LINK_SPEED_1000H:
            {
                command_config |= (ETHSW_COMMAND_CONFIG_HD_ENA |
                                   ETHSW_COMMAND_CONFIG_ETH_SPEED);
                break;
            }

            case ETHSW_LINK_SPEED_1000F:
            {
                command_config |= (ETHSW_COMMAND_CONFIG_ETH_SPEED);
                break;
            }

            default:
            {
                err = FSP_ERR_INVALID_ARGUMENT;
                break;
            }
        }

        *p_reg_command_config = command_config;
    }

    return err;
}                                      /* End of function R_ETHSW_ConfSpeed() */

/*******************************************************************************************************************//**
 * Sets the static MAC address entry for the given MAC address.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_MacTableSet (ether_switch_ctrl_t * const    p_ctrl,
                               ethsw_mac_table_entry_addr_t * p_mac_entry_addr,
                               ethsw_mac_table_entry_info_t * p_mac_entry_info)
{
    ethsw_mac_addr_reg_t entry = {0};
    uint64_t             timeout;      /* timeout */
    uint32_t             port_mask;    /* converted portmask */
    volatile uint32_t    dammy_read;

    R_ETHSW_Type volatile * p_switch_reg;
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_mac_entry_addr), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_mac_entry_addr->mac_addr), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != *p_mac_entry_addr->mac_addr), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_mac_entry_info), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    port_mask = p_mac_entry_info->port_mask.mask;

    /* copy mac address to get valid alignment */
    memcpy(entry.mac_addr, *p_mac_entry_addr->mac_addr, ETHSW_MAC_ADDR_LENGTH);

    p_switch_reg->LK_DATA_LO = entry.reg_data[0];
    p_switch_reg->LK_DATA_HI = entry.reg_data[1];

    p_switch_reg->LK_DATA_HI2_b.MEMDATA = p_mac_entry_addr->vlan_id &
                                          (R_ETHSW_LK_DATA_HI2_MEMDATA_Msk >> R_ETHSW_LK_DATA_HI2_MEMDATA_Pos);

    /* damy read */
    dammy_read = p_switch_reg->LK_DATA_LO;
    dammy_read = p_switch_reg->LK_DATA_HI;
    dammy_read = dammy_read;

    /* Delete entry? */
    if (0 == port_mask)
    {
        p_switch_reg->LK_ADDR_CTRL |= (ETHSW_LK_ADDR_CLEAR | ETHSW_LK_ADDR_LOOKUP);
    }
    else
    {
        timeout = ETHSW_TIMEOUT_COUNT;

        /* Perform lookup to get address entry */
        p_switch_reg->LK_ADDR_CTRL |= ETHSW_LK_ADDR_LOOKUP;
        while (true == p_switch_reg->LK_ADDR_CTRL_b.BUSY)
        {
            ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
            timeout--;
        }

        /* Address is already there. Just update the port mask and priority */
        if (p_switch_reg->LK_DATA_HI & ETHSW_LK_ADDR_REG_VALID)
        {
            /* update port bit mask  */
            p_switch_reg->LK_DATA_HI &= ~(uint32_t) ETHSW_LK_ADDR_PORTMASK;
            p_switch_reg->LK_DATA_HI |= (port_mask << ETHSW_LK_ADDR_PORT_REG_OFFSET);

            /* updated priority */
            p_switch_reg->LK_DATA_HI &= ~(uint32_t) ETHSW_LK_ADDR_PRIOMASK;
            p_switch_reg->LK_DATA_HI |= (p_mac_entry_info->priority << ETHSW_LK_ADDR_PRIO_REG_OFFSET) &
                                        ETHSW_LK_ADDR_PRIOMASK;
        }
        else
        {
            /* New static entry to be created */
            p_switch_reg->LK_DATA_HI &= ~ETHSW_LK_ADDR_MASKANDFLAGS_MASK;
            p_switch_reg->LK_DATA_HI |= ETHSW_LK_ADDR_REG_VALID;
            p_switch_reg->LK_DATA_HI |= ETHSW_LK_ADDR_REG_TYPE;
            p_switch_reg->LK_DATA_HI |= port_mask << ETHSW_LK_ADDR_PORT_REG_OFFSET;
            p_switch_reg->LK_DATA_HI |= (p_mac_entry_info->priority << ETHSW_LK_ADDR_PRIO_REG_OFFSET) &
                                        ETHSW_LK_ADDR_PRIOMASK;
        }

        /* trigger MAC table write */
        p_switch_reg->LK_ADDR_CTRL |= ETHSW_LK_ADDR_WRITE;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_MacTableSet() */

/*******************************************************************************************************************//**
 * Retrieves the port mask for the given MAC address from the static MAC table.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_MacTableGet (ether_switch_ctrl_t * const    p_ctrl,
                               ethsw_mac_table_entry_addr_t * p_mac_entry_addr,
                               ethsw_mac_table_entry_info_t * p_mac_entry_info)
{
    ethsw_mac_addr_reg_t entry = {0};
    uint64_t             timeout;      /* timeout */
    volatile uint32_t    dammy_read;
    volatile uint32_t    lk_data_hi;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_mac_entry_addr), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_mac_entry_addr->mac_addr), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != *p_mac_entry_addr->mac_addr), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_mac_entry_info), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* copy mac address to get valid alignment */
    memcpy(entry.mac_addr, *p_mac_entry_addr->mac_addr, ETHSW_MAC_ADDR_LENGTH);

    p_switch_reg->LK_DATA_LO = entry.reg_data[0];
    p_switch_reg->LK_DATA_HI = entry.reg_data[1];

    p_switch_reg->LK_DATA_HI2_b.MEMDATA = p_mac_entry_addr->vlan_id &
                                          (R_ETHSW_LK_DATA_HI2_MEMDATA_Msk >> R_ETHSW_LK_DATA_HI2_MEMDATA_Pos);

    /* damy read */
    dammy_read = p_switch_reg->LK_DATA_LO;
    dammy_read = p_switch_reg->LK_DATA_HI;
    dammy_read = dammy_read;

    timeout = ETHSW_TIMEOUT_COUNT;

    /* Perform lookup to get address entry */
    p_switch_reg->LK_ADDR_CTRL |= ETHSW_LK_ADDR_LOOKUP;
    while (true == p_switch_reg->LK_ADDR_CTRL_b.BUSY)
    {
        ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
        timeout--;
    }

    /* Entry found? */
    lk_data_hi = p_switch_reg->LK_DATA_HI;

    if (ETHSW_LK_ADDR_REG_VALID & lk_data_hi)
    {
        /* prrt */
        p_mac_entry_info->port_mask.mask = (lk_data_hi & ETHSW_LK_ADDR_PORTMASK) >> ETHSW_LK_ADDR_PORT_REG_OFFSET;

        /* priority */
        p_mac_entry_info->priority = (p_switch_reg->LK_DATA_HI & ETHSW_LK_ADDR_PRIOMASK) >>
                                     ETHSW_LK_ADDR_PRIO_REG_OFFSET;
    }
    else
    {
        p_mac_entry_info->port_mask.mask = 0;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_MacTableGet() */

/*******************************************************************************************************************//**
 *  Configures the MAC learning and aging parameters of MAC table.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_MacTableConfigSet (ether_switch_ctrl_t * const p_ctrl, ethsw_mac_table_config_t * p_mac_table_config)
{
    uint32_t reg_val;                  /* Register value */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_mac_table_config), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    reg_val  = p_switch_reg->LK_CTRL & ~((uint32_t) ETHSW_LK_MASK);
    reg_val |= ((uint32_t) p_mac_table_config->learning << ETHSW_LK_LEARN) |
               ((uint32_t) p_mac_table_config->aging << ETHSW_LK_AGING) |
               ((uint32_t) p_mac_table_config->migration << ETHSW_LK_MIGRAT) |
               ((uint32_t) p_mac_table_config->discard_unknown << ETHSW_LK_DISCUNKN) |
               ((uint32_t) p_mac_table_config->independent_vlan << ETHSW_LK_PERVLAN);

    p_switch_reg->LK_CTRL = reg_val;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_MacTableConfSet() */

/*******************************************************************************************************************//**
 * Clears specific types of entries from the MAC table or clears the whole table.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_MacTableClear (ether_switch_ctrl_t * const p_ctrl, ethsw_mac_table_clear_mode_t * p_mac_table_clear)
{
    fsp_err_t res = FSP_ERR_TIMEOUT;   /* result */
    uint64_t  timeout;                 /* timeout */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_mac_table_clear), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    timeout = ETHSW_TIMEOUT_COUNT;

    switch (*p_mac_table_clear)
    {
        case ETHSW_MAC_TABLE_CLEAR_ALL:
        {
            p_switch_reg->LK_CTRL_b.CLRTBL = true;

            while (true == p_switch_reg->LK_CTRL_b.CLRTBL)
            {
                ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
                timeout--;
            }

            res = FSP_SUCCESS;
            break;
        }

        case ETHSW_MAC_TABLE_CLEAR_DYNAMIC:
        {
            p_switch_reg->LK_ADDR_CTRL_b.CLR_DYNAMIC = true;

            while (true == p_switch_reg->LK_ADDR_CTRL_b.CLR_DYNAMIC)
            {
                ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
                timeout--;
            }

            res = FSP_SUCCESS;
            break;
        }

        case ETHSW_MAC_TABLE_CLEAR_STATIC:
        {
            p_switch_reg->LK_ADDR_CTRL_b.CLR_STATIC = true;

            while (true == p_switch_reg->LK_ADDR_CTRL_b.CLR_STATIC)
            {
                ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
                timeout--;
            }

            res = FSP_SUCCESS;
            break;
        }

        default:
        {
            res = FSP_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return res;
}                                      /* End of function R_ETHSW_MacTableClear() */

/*******************************************************************************************************************//**
 * Sets learning state for the given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_LearningSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type          * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* set learning state */
    if (true == enable)
    {
        p_switch_reg->INPUT_LEARN_BLOCK_b.LEARNDIS &= ~(1U << port) & ETHSW_4BITS_MASK;
    }
    else
    {
        p_switch_reg->INPUT_LEARN_BLOCK_b.LEARNDIS = (p_switch_reg->INPUT_LEARN_BLOCK_b.LEARNDIS | (1U << port)) &
                                                     ETHSW_4BITS_MASK;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_LearningSet() */

/*******************************************************************************************************************//**
 * Enables the forwarding of incoming frames on a port for RSTP.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_PortForwardAdd (ether_switch_ctrl_t * const p_ctrl, uint32_t port)
{
    uint32_t block;                    /* register value */
    uint32_t tx;                       /* register value */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    block  = p_switch_reg->INPUT_LEARN_BLOCK_b.BLOCKEN;
    block &= ~(1U << port);

    tx  = p_switch_reg->PORT_ENA_b.TXENA;
    tx |= (1U << port);

    p_switch_reg->INPUT_LEARN_BLOCK_b.BLOCKEN = block & ETHSW_4BITS_MASK;

    p_switch_reg->PORT_ENA_b.TXENA = tx & ETHSW_4BITS_MASK;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Disables the forwarding of incoming frames on a port for RSTP
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_PortForwardDel (ether_switch_ctrl_t * const p_ctrl, uint32_t port)
{
    uint32_t block;                    /* register value */
    uint32_t tx;                       /* register value */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    block  = p_switch_reg->INPUT_LEARN_BLOCK_b.BLOCKEN;
    block |= (1U << port);

    tx  = p_switch_reg->PORT_ENA_b.TXENA;
    tx &= ~(1U << port);

    p_switch_reg->INPUT_LEARN_BLOCK_b.BLOCKEN = block & ETHSW_4BITS_MASK;

    p_switch_reg->PORT_ENA_b.TXENA = tx & ETHSW_4BITS_MASK;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_PortForwardAdd() */

/*******************************************************************************************************************//**
 * Set the flood domain port masks for frames with unknown destinations
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_FloodUnknownSet (ether_switch_ctrl_t * const p_ctrl, ethsw_flood_unknown_config_t * p_flood_config)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_flood_config), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->BCAST_DEFAULT_MASK0_b.BCASTDM = p_flood_config->port_mask_bcast.mask &
                                                  (R_ETHSW_BCAST_DEFAULT_MASK0_BCASTDM_Msk >>
                                                   R_ETHSW_BCAST_DEFAULT_MASK0_BCASTDM_Pos);

    p_switch_reg->MCAST_DEFAULT_MASK0_b.MCASTDM = p_flood_config->port_mask_mcast.mask &
                                                  (R_ETHSW_MCAST_DEFAULT_MASK0_MCASTDM_Msk >>
                                                   R_ETHSW_MCAST_DEFAULT_MASK0_MCASTDM_Pos);

    p_switch_reg->UCAST_DEFAULT_MASK0_b.UCASTDM = p_flood_config->port_mask_ucast.mask &
                                                  (R_ETHSW_UCAST_DEFAULT_MASK0_UCASTDM_Msk >>
                                                   R_ETHSW_UCAST_DEFAULT_MASK0_UCASTDM_Pos);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_FloodUnknownSet() */

/*******************************************************************************************************************//**
 * Return link state for given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_LinkStateGet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_link_status_t * p_state_link)
{
    const volatile uint32_t * p_reg;   /* Register */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_state_link), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_reg = (const volatile uint32_t *) ((uint8_t *) &p_switch_reg->STATUS_P0 +
                                         ETHSW_PORT_OFFSET * port);

    *p_state_link = (*p_reg & ETHSW_PHY_LINK) ?
                    ETHSW_LINK_STATE_UP : ETHSW_LINK_STATE_DOWN;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_LinkStateGet() */

/*******************************************************************************************************************//**
 * Sets maximum frame size of given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_FrameSizeMaxSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint32_t frame_size_max)
{
    volatile uint32_t * p_reg;         /* Register */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* calculate per port address of the frame size register */
    p_reg = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->FRM_LENGTH_P0 + (ETHSW_PORT_OFFSET * port));

    /* assign masked command data to register */
    *p_reg = frame_size_max & ETHSW_FRAMESIZE_MASK;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_FrameSizeMaxSet() */

/*******************************************************************************************************************//**
 * Initialize DLR module
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrInitSet (ether_switch_ctrl_t * const p_ctrl, ethsw_dlr_init_t * p_dlr_init)
{
    ethsw_mac_table_entry_addr_t mac_entry_addr = {0}; // Address in MAC table entry
    ethsw_mac_table_entry_info_t mac_entry_info = {0}; // Information in MAC table entry
    fsp_err_t res;                                     // result
    uint8_t   cnt;                                     // loop counter

    /* Multicast MAC for Beacon Frames */
    uint8_t dlr_mac[ETHSW_MAC_ADDR_LENGTH] = ETHSW_MULTICAST_BEACON;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_dlr_init), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Forced Forwarding via Static MAC Table entries */
    mac_entry_addr.mac_addr = (ethsw_mac_addr_t *) &dlr_mac;

    /* --> Beacon Frames (01:21:6c:00:00:01) ports 0, 1 */
    dlr_mac[5] = 0x01;
    mac_entry_info.port_mask.mask = ETHSW_PORT_BIT(0) | ETHSW_PORT_BIT(1);

    res = R_ETHSW_MacTableSet(p_ctrl, &mac_entry_addr, &mac_entry_info);
    if (FSP_SUCCESS != res)
    {
        return res;
    }

    /* --> Neighbor_Check/SignOn Frames (01:21:6c:00:00:02) port 3 */
    dlr_mac[5] = 0x02;
    mac_entry_info.port_mask.mask = ETHSW_PORT_BIT(ETHSW_PORT_MGMT);

    res = R_ETHSW_MacTableSet(p_ctrl, &mac_entry_addr, &mac_entry_info);
    if (FSP_SUCCESS != res)
    {
        return res;
    }

    /* --> other Frames (01:21:6c:00:00:{03..05}) ports 0, 1, 3 */
    mac_entry_info.port_mask.mask = ETHSW_PORT_BIT(0) | ETHSW_PORT_BIT(1) | ETHSW_PORT_BIT(ETHSW_PORT_MGMT);
    for (cnt = 3; cnt < 6; cnt++)
    {
        dlr_mac[5] = cnt;
        res        = R_ETHSW_MacTableSet(p_ctrl, &mac_entry_addr, &mac_entry_info);
        if (FSP_SUCCESS != res)
        {
            return res;
        }
    }

    /* write local MAC for Loop Filter */
    uint8_t * p_host_addr = (uint8_t *) p_dlr_init->p_host_addr;

    p_switch_reg->DLR_LOC_MAClo = (((uint32_t) p_host_addr[0] << 0) |
                                   ((uint32_t) p_host_addr[1] << 8) |
                                   ((uint32_t) p_host_addr[2] << 16) |
                                   ((uint32_t) p_host_addr[3] << 24));

    p_switch_reg->DLR_LOC_MAChi = (((uint32_t) p_host_addr[4] << 0) |
                                   ((uint32_t) p_host_addr[5] << 8));

    /* callback function pointer  */
    gp_ethsw_dlr_callback = p_dlr_init->p_dlr_callback;

    /* set DLR EtherType */
    p_switch_reg->DLR_ETH_TYP_b.DLR_ETH_TYP = ETHSW_RN_ADVSW_DLR_ETHTYPE;

    /* set number of cycles for 1 us */
    /* 200 MHz -> 200 Ticks = 1 us */
    p_switch_reg->DLR_CONTROL_b.US_TIME = ETHSW_RN_ADVSW_DLR_TICKS;

    /* discard Bacon frames with invalid Timeout values  */
    p_switch_reg->DLR_CONTROL_b.IGNORE_INVTM = 0;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrInitSet() */

/*******************************************************************************************************************//**
 * Uninitialize DLR module
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrUninitSet (ether_switch_ctrl_t * const p_ctrl)
{
    uint8_t dlr_mac[ETHSW_MAC_ADDR_LENGTH]      = ETHSW_MULTICAST_DLR; // Multicast MAC for DLR
    ethsw_mac_table_entry_addr_t mac_entry_addr = {0};                 // Address in MAC table entry
    ethsw_mac_table_entry_info_t mac_entry_info = {0};                 // Information in MAC table entry
    fsp_err_t res;                                                     // result
    uint32_t  cnt;                                                     // count

    /* initialize MAC Table entries */
    mac_entry_addr.mac_addr = (ethsw_mac_addr_t *) &dlr_mac;

    for (cnt = 1; cnt < 6; cnt++)
    {
        /* set DLR Multicast MAC address */
        dlr_mac[5] = (uint8_t) cnt;

        /* delete address entry from MAC Table */
        mac_entry_info.port_mask.mask = 0;

        res = R_ETHSW_MacTableSet(p_ctrl, &mac_entry_addr, &mac_entry_info);
        if (FSP_SUCCESS != res)
        {
            return res;
        }
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrUninitSet() */

/*******************************************************************************************************************//**
 * Enable DLR module
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrEnableSet (ether_switch_ctrl_t * const p_ctrl)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* interrupts */
    p_switch_reg->DLR_IRQ_STAT_ACK = ETHSW_HEX_0000FFFF;

    p_switch_reg->INT_CONFIG_b.IRQ_EN  = 1;
    p_switch_reg->INT_CONFIG_b.DLR_INT = 1;

    p_switch_reg->DLR_IRQ_CONTROL = ETHSW_TGT_DLR_INT_BEACONTMO_0 |
                                    ETHSW_TGT_DLR_INT_BEACONTMO_1 |
                                    ETHSW_TGT_DLR_INT_SUVICHNG |
                                    ETHSW_TGT_DLR_INT_LNKCHNG_0 |
                                    ETHSW_TGT_DLR_INT_LNKCHNG_1 |
                                    ETHSW_TGT_DLR_INT_NEWSTATUS |
                                    ETHSW_TGT_DLR_INT_LOCALLOOP_0 |
                                    ETHSW_TGT_DLR_INT_LOCALLOOP_1;

    p_switch_reg->DLR_CONTROL_b.LOOP_FILTER_ENA = 1;
    p_switch_reg->DLR_CONTROL_b.ENABLE          = 1;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrEnableSet() */

/*******************************************************************************************************************//**
 * Disable DLR module.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrDisableSet (ether_switch_ctrl_t * const p_ctrl)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->DLR_CONTROL_b.ENABLE          = 0;
    p_switch_reg->DLR_CONTROL_b.LOOP_FILTER_ENA = 0;

    p_switch_reg->INT_CONFIG_b.DLR_INT = 0;
    p_switch_reg->DLR_IRQ_CONTROL      = 0;
    p_switch_reg->DLR_IRQ_STAT_ACK     = ETHSW_HEX_0000FFFF;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrDisableSet() */

/*******************************************************************************************************************//**
 * Gets the beacon receive status of the specified port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrBeaconStateGet (ether_switch_ctrl_t * const p_ctrl,
                                     uint32_t                    port,
                                     ethsw_dlr_beacon_state_t  * p_beacon_state)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_beacon_state), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((2 > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (((0 == port) && (0 != (p_switch_reg->DLR_STATUS_b.LastBcnRcvPort & 0x01))) ||
        ((1 == port) && (0 != (p_switch_reg->DLR_STATUS_b.LastBcnRcvPort & 0x02))))
    {
        *p_beacon_state = ETHSW_DLR_BEACON_VALID;
    }
    else
    {
        *p_beacon_state = ETHSW_DLR_BEACON_INVALID;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrBeaconStateGet() */

/*******************************************************************************************************************//**
 * Gets DLR node status.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrNodeStateGet (ether_switch_ctrl_t * const p_ctrl, ethsw_dlr_node_state_t * p_node_state)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_node_state), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    *p_node_state = (ethsw_dlr_node_state_t) p_switch_reg->DLR_STATUS_b.NODE_STATE;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrNodeStateGet() */

/*******************************************************************************************************************//**
 * Gets IP address of DLR supervisor.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrSvIpGet (ether_switch_ctrl_t * const p_ctrl, uint32_t * p_ip_addr)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_ip_addr), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    *p_ip_addr = p_switch_reg->DLR_SUPR_IPADR;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrSvIpGet() */

/*******************************************************************************************************************//**
 * Gets preference of DLR supervisor.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrSvPriorityGet (ether_switch_ctrl_t * const p_ctrl, uint8_t * p_priority)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_priority), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    *p_priority = p_switch_reg->DLR_SUPR_MAChi_b.PRECE;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrSvPriorityGet() */

/*******************************************************************************************************************//**
 * Gets VLAN ID of DLR beacon frame.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrVlanGet (ether_switch_ctrl_t * const p_ctrl, uint16_t * p_vlan_info)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_vlan_info), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    *p_vlan_info = p_switch_reg->DLR_STATE_VLAN_b.VLANINFO;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrVlanGet() */

/*******************************************************************************************************************//**
 * Gets MAC address of DLR beacon frame.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_DlrSvMacGet (ether_switch_ctrl_t * const p_ctrl, ethsw_mac_addr_t * pp_addr_mac)
{
    uint32_t regVal;                   /* register value */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != pp_addr_mac), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    regVal            = p_switch_reg->DLR_SUPR_MAClo;
    (*pp_addr_mac)[0] = (uint8_t) (regVal & ETHSW_HEX_000000FF) >> 0;
    (*pp_addr_mac)[1] = (uint8_t) ((regVal & ETHSW_HEX_0000FF00) >> 8);
    (*pp_addr_mac)[2] = (uint8_t) ((regVal & ETHSW_HEX_00FF0000) >> 16);
    (*pp_addr_mac)[3] = (uint8_t) ((regVal & ETHSW_HEX_FF000000) >> 24);

    regVal            = p_switch_reg->DLR_SUPR_MAChi;
    (*pp_addr_mac)[4] = (uint8_t) (regVal & ETHSW_HEX_000000FF);
    (*pp_addr_mac)[5] = (uint8_t) ((regVal & ETHSW_HEX_0000FF00) >> 8);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_DlrSvMacGet() */

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

/********************************************************************************************************************//**
 * Initialization of the Active Stream Identification Table
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpInitilizeAsiTable (ether_switch_ctrl_t * const p_ctrl, ethsw_port_mask_t port_mask)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /**************************************************************************
     * 28.4.19.4 Active Stream Identification
     **************************************************************************/

    /** Figure 28.55 Active Stream Identification initialization **/
    p_reg_switch->ASI_MEM_WDATA[0] = 0U;
    p_reg_switch->ASI_MEM_WDATA[1] = 0U;
    p_reg_switch->ASI_MEM_WDATA[2] = 0U;
    p_reg_switch->ASI_MEM_WDATA[3] = 0U;

    for (uint8_t addr = 0; addr < ETHSW_EFP_ASI_ADDR_NUM; addr++)
    {
        p_reg_switch->ASI_MEM_ADDR = (uint32_t) (ETHSW_MEM_REQ_ALL_ACCESS | ETHSW_MEM_WEN_ENABLE | addr); /* < 0x700 : MEM_REQ(10:8)=7 | 0x80 : MEM_WEN(7)=1 */
    }

    /* Extended Frame Parser (EFP) Enable */
    volatile uint32_t * command_config_pn = &p_reg_switch->COMMAND_CONFIG_P0;
    for (uint8_t port = 0; port < ETHSW_PORT_COUNT_ALL; port++)
    {
        if ((port_mask.mask & (1U << port)))
        {
            *command_config_pn = (
                ((1U << R_ETHSW_COMMAND_CONFIG_P0_EFPI_SELECT_Pos) & R_ETHSW_COMMAND_CONFIG_P0_EFPI_SELECT_Msk) |
                ((*command_config_pn) & ~R_ETHSW_COMMAND_CONFIG_P0_EFPI_SELECT_Msk)
                );
        }

        command_config_pn += ETHSW_COMMAND_CONFIG_OFFSET;
    }

    return err;
}                                      /* End of function R_ETHSW_EfpInitilizeAsiTable() */

/********************************************************************************************************************//**
 * Active Stream Identification entry setting
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_WRITE_FAILED;       Write operation failed ( The table entry is occupied )
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpAsiTableSet (ether_switch_ctrl_t * const p_ctrl, ethsw_efp_asi_t * p_efp_asi_entry)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint8_t                 addr;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN((NULL != p_efp_asi_entry), FSP_ERR_INVALID_POINTER);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /** Figure 28.56 Active Stream Identification entry setting **/
    /* Hash calculation (CRC-7) */
    addr =
        r_ethsw_calc_hash((ethsw_mac_addr_t *) &(p_efp_asi_entry->dmaci),
                          (ethsw_vlan_vid_t *) &(p_efp_asi_entry->vidi));

    p_reg_switch->ASI_MEM_ADDR_b.ADDR    = (addr & R_ETHSW_ASI_MEM_ADDR_ADDR_Msk);
    p_reg_switch->ASI_MEM_ADDR_b.MEM_WEN = 0x0U;
    p_reg_switch->ASI_MEM_ADDR_b.MEM_REQ =
        (uint32_t) ((1UL << p_efp_asi_entry->port) &
                    (R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Msk >> R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Pos));

    if ((R_ETHSW->ASI_MEM_RDATA[0] & 0x1U) == 0)
    {
        p_reg_switch->ASI_MEM_WDATA[3] = (uint32_t) (
            (p_efp_asi_entry->dmaci[0] << 24) |
            (p_efp_asi_entry->dmaci[1] << 16) |
            (p_efp_asi_entry->dmaci[2] << 8) |
            (p_efp_asi_entry->dmaci[3])
            );
        p_reg_switch->ASI_MEM_WDATA[2] = (uint32_t) (
            (p_efp_asi_entry->dmaci[4] << 24) |
            (p_efp_asi_entry->dmaci[5] << 16) |
            (p_efp_asi_entry->vidi[0] << 12) |
            (p_efp_asi_entry->vidi[1] << 4) |
            (p_efp_asi_entry->dmaco[0] >> 4)
            );
        p_reg_switch->ASI_MEM_WDATA[1] = (uint32_t) (
            ((p_efp_asi_entry->dmaco[0] & 0x0F) << 28) |
            (p_efp_asi_entry->dmaco[1] << 20) |
            (p_efp_asi_entry->dmaco[2] << 12) |
            (p_efp_asi_entry->dmaco[3] << 4) |
            (p_efp_asi_entry->dmaco[4] >> 4)
            );
        p_reg_switch->ASI_MEM_WDATA[0] = (uint32_t) (
            ((p_efp_asi_entry->dmaco[4] & 0x0F) << 28) |
            (p_efp_asi_entry->dmaco[5] << 20) |
            ((p_efp_asi_entry->vido[0] & 0x0F) << 16) |
            (p_efp_asi_entry->vido[1] << 8) |
            ((p_efp_asi_entry->pcpo & 0x07) << 5) |
            (p_efp_asi_entry->sid << 2) |
            (p_efp_asi_entry->fs << 1) |
            (p_efp_asi_entry->ev)
            );

        p_reg_switch->ASI_MEM_ADDR_b.ADDR    = (addr & R_ETHSW_ASI_MEM_ADDR_ADDR_Msk);
        p_reg_switch->ASI_MEM_ADDR_b.MEM_WEN = 0x1U;
        p_reg_switch->ASI_MEM_ADDR_b.MEM_REQ =
            (uint32_t) ((1UL << p_efp_asi_entry->port) &
                        (R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Msk >> R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Pos));
    }
    else
    {
        /* The table entry is occupied */
        err = FSP_ERR_WRITE_FAILED;    /* < Write operation failed */
    }

 #ifdef ASI_TBL_DEBUG

    /* verify */
    p_reg_switch->ASI_MEM_ADDR_b.ADDR    = (addr & R_ETHSW_ASI_MEM_ADDR_ADDR_Msk);
    p_reg_switch->ASI_MEM_ADDR_b.MEM_WEN = 0x0U;
    p_reg_switch->ASI_MEM_ADDR_b.MEM_REQ =
        (uint32_t) ((1UL << p_efp_asi_entry->port) &
                    (R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Msk >> R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Pos));

    if ((R_ETHSW->ASI_MEM_RDATA[0] & 0x1U) != 0)
    {
        app_printf("table entry [3]:[%08X], [2]:[%08X], [1]:[%08X], [0]:[%08X]\n",
                   R_ETHSW->ASI_MEM_RDATA[3],
                   R_ETHSW->ASI_MEM_RDATA[2],
                   R_ETHSW->ASI_MEM_RDATA[1],
                   R_ETHSW->ASI_MEM_RDATA[0]);
    }
 #endif

    return err;
}                                      /* End of function R_ETHSW_EfpAsiTableSet() */

/********************************************************************************************************************//**
 * Active Stream Identification entry deletion
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpAsiTableDelete (ether_switch_ctrl_t * const p_ctrl,
                                     uint32_t                    port,
                                     ethsw_mac_addr_t          * dmaci,
                                     ethsw_vlan_vid_t          * vidi)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint8_t                 addr;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != dmaci), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != vidi), FSP_ERR_INVALID_POINTER);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /** Figure 28.57 Active Stream Identification entry deletion **/
    /* Hash calculation (CRC-7) */
    addr = r_ethsw_calc_hash(dmaci, vidi);

    p_reg_switch->ASI_MEM_ADDR_b.ADDR    = (addr & R_ETHSW_ASI_MEM_ADDR_ADDR_Msk);
    p_reg_switch->ASI_MEM_ADDR_b.MEM_WEN = 0x0U;
    p_reg_switch->ASI_MEM_ADDR_b.MEM_REQ = (1U << port) &
                                           (R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Msk >> R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Pos);

    if ((R_ETHSW->ASI_MEM_RDATA[0] & 0x1U) != 0)
    {
        p_reg_switch->ASI_MEM_WDATA[0] = 0U;
        p_reg_switch->ASI_MEM_WDATA[1] = 0U;
        p_reg_switch->ASI_MEM_WDATA[2] = 0U;
        p_reg_switch->ASI_MEM_WDATA[3] = 0U;

        p_reg_switch->ASI_MEM_ADDR_b.ADDR    = (addr & R_ETHSW_ASI_MEM_ADDR_ADDR_Msk);
        p_reg_switch->ASI_MEM_ADDR_b.MEM_WEN = 0x1U;
        p_reg_switch->ASI_MEM_ADDR_b.MEM_REQ = (1U << port) &
                                               (R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Msk >> R_ETHSW_ASI_MEM_ADDR_MEM_REQ_Pos);
    }

    return err;
}                                      /* End of function R_ETHSW_EfpAsiTableDelete() */

/********************************************************************************************************************//**
 * Set vlan ingress verification mode
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpVlanVerificationModeSet (ether_switch_ctrl_t * const          p_ctrl,
                                              uint32_t                             port,
                                              ethsw_vlan_input_verification_mode_t vicm)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_EFP_VICM_NUM_MAX > vicm, FSP_ERR_INVALID_ARGUMENT);
 #endif

    /**************************************************************************
     * 28.4.19.5 VLAN Ingress Verification
     **************************************************************************/
    p_reg_switch = p_instance_ctrl->p_reg_switch;

    volatile uint32_t * pn_vlan_mode = &p_reg_switch->P0_VLAN_MODE;
    pn_vlan_mode += (ETHSW_VLAN_MODE_OFFSET * port);

    *pn_vlan_mode = (uint32_t) (
        ((uint32_t) (vicm << R_ETHSW_P0_VLAN_MODE_VICM_Pos) & R_ETHSW_P0_VLAN_MODE_VICM_Msk) |
        (*pn_vlan_mode & R_ETHSW_P0_VLAN_MODE_VITM_Msk)
        );

    return err;
}                                      /* End of function R_ETHSW_EfpVlanVerificationModeSet() */

/********************************************************************************************************************//**
 * Set priority reegeneration parameters
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpPriorityRegenerationSet (ether_switch_ctrl_t * const         p_ctrl,
                                              uint32_t                            port,
                                              ethsw_efp_priority_regeneration_t * p_pri_regen)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_pri_regen), FSP_ERR_INVALID_POINTER);
 #endif

    /**************************************************************************
     * 28.4.19.6 Priority Regeneration
     **************************************************************************/
    p_reg_switch = p_instance_ctrl->p_reg_switch;

    volatile uint32_t * pn_vlan_mode = &p_reg_switch->P0_VLAN_MODE;
    pn_vlan_mode += (ETHSW_VLAN_MODE_OFFSET * port);
    *pn_vlan_mode = (uint32_t) (
        ((uint32_t) (p_pri_regen->vitm << R_ETHSW_P0_VLAN_MODE_VITM_Pos) & R_ETHSW_P0_VLAN_MODE_VITM_Msk) |
        (*pn_vlan_mode & R_ETHSW_P0_VLAN_MODE_VICM_Msk)
        );

    volatile uint32_t * pn_vlan_tag = &p_reg_switch->P0_VLAN_TAG;
    pn_vlan_tag += (ETHSW_VLAN_TAG_OFFSET * port);
    *pn_vlan_tag = (uint32_t) (
        ((uint32_t) ((p_pri_regen->tpid[0] << 8 | p_pri_regen->tpid[1]) << R_ETHSW_P0_VLAN_TAG_TPID_Pos) &
         R_ETHSW_P0_VLAN_TAG_TPID_Msk) |
        ((uint32_t) (p_pri_regen->pcp << R_ETHSW_P0_VLAN_TAG_PCP_Pos) & R_ETHSW_P0_VLAN_TAG_PCP_Msk) |
        ((uint32_t) (p_pri_regen->dei << R_ETHSW_P0_VLAN_TAG_DEI_Pos) & R_ETHSW_P0_VLAN_TAG_DEI_Msk) |
        ((uint32_t) ((p_pri_regen->vid[0] << 8 | p_pri_regen->vid[1]) << R_ETHSW_P0_VLAN_TAG_VID_Pos) &
         R_ETHSW_P0_VLAN_TAG_VID_Msk)
        );

    volatile uint32_t * pn_pcp_remap = &p_reg_switch->P0_PCP_REMAP;
    pn_pcp_remap += (ETHSW_PCP_REMAP_OFFSET * port);
    *pn_pcp_remap = (uint32_t) (
        ((uint32_t) (p_pri_regen->pcp_remap[7] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP7_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP7_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[6] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP6_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP6_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[5] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP5_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP5_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[4] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP4_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP4_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[3] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP3_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP3_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[2] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP2_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP2_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[1] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP1_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP1_Msk) |
        ((uint32_t) (p_pri_regen->pcp_remap[0] << R_ETHSW_P2_PCP_REMAP_PCP_REMAP0_Pos) &
         R_ETHSW_P2_PCP_REMAP_PCP_REMAP0_Msk)
        );

    return err;
}                                      /* End of function R_ETHSW_EfpPriorityRegenerationSet()*/

/********************************************************************************************************************//**
 * Set filter function parameters
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpFilterTableSet (ether_switch_ctrl_t * const     p_ctrl,
                                     uint32_t                        port,
                                     uint32_t                        sid,
                                     ethsw_efp_qci_stream_filter_t * p_flt_entry)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_EFP_SID_NUM_MAX > sid, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_flt_entry), FSP_ERR_INVALID_POINTER);
 #endif

    /**************************************************************************
     * 28.4.19.7 Filtering Function
     **************************************************************************/
    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* Figure 28.61 Qci stream filter table setting flow */
    volatile uint32_t * pn_qsftbls = &p_reg_switch->P0_QSFTBL0;
    pn_qsftbls  += ((ETHSW_QSFTBL_OFFSET_PORT * port) + (ETHSW_QSFTBL_OFFSET_SID * sid));
    *pn_qsftbls &= (uint32_t) ~(R_ETHSW_P0_QSFTBL0_QSTE_Msk);
    while ((*pn_qsftbls & R_ETHSW_P0_QSFTBL0_QSTE_Msk) != 0)
    {
        ;
    }

    volatile uint32_t * pn_qstmacus = &p_reg_switch->P0_QSTMACU0;
    pn_qstmacus += ((ETHSW_QSTMACU_OFFSET_PORT * port) + (ETHSW_QSTMACU_OFFSET_SID * sid));
    *pn_qstmacus = (uint32_t) (
        (((uint32_t) p_flt_entry->qdasa << R_ETHSW_P1_QSTMACU0_DASA_Pos) & R_ETHSW_P1_QSTMACU0_DASA_Msk) |
        (((uint32_t) (p_flt_entry->qmac[0] << 8 | p_flt_entry->qmac[1]) << R_ETHSW_P0_QSTMACU0_MACA_Pos) &
         R_ETHSW_P0_QSTMACU0_MACA_Msk)
        );

    volatile uint32_t * pn_qstmacds = &p_reg_switch->P0_QSTMACD0;
    pn_qstmacds += ((ETHSW_QSTMACD_OFFSET_PORT * port) + (ETHSW_QSTMACD_OFFSET_SID * sid));
    *pn_qstmacds = (
        ((uint32_t) p_flt_entry->qmac[2] << 24U) |
        ((uint32_t) p_flt_entry->qmac[3] << 16U) |
        ((uint32_t) p_flt_entry->qmac[4] << 8U) |
        ((uint32_t) p_flt_entry->qmac[5])
        );

    volatile uint32_t * pn_qstmamus = &p_reg_switch->P0_QSTMAMU0;
    pn_qstmamus += ((ETHSW_QSTMAMU_OFFSET_PORT * port) + (ETHSW_QSTMAMU_OFFSET_SID * sid));
    *pn_qstmamus = ((uint32_t) (p_flt_entry->qmam[0] << 8 | p_flt_entry->qmam[1]) << R_ETHSW_P0_QSTMAMU0_MACAM_Pos) &
                   R_ETHSW_P0_QSTMAMU0_MACAM_Msk;

    volatile uint32_t * pn_qstmamds = &p_reg_switch->P0_QSTMAMD0;
    pn_qstmamds += ((ETHSW_QSTMAMD_OFFSET_PORT * port) + (ETHSW_QSTMAMD_OFFSET_SID * sid));
    *pn_qstmamds = (
        ((uint32_t) p_flt_entry->qmam[2] << 24U) |
        ((uint32_t) p_flt_entry->qmam[3] << 16U) |
        ((uint32_t) p_flt_entry->qmam[4] << 8U) |
        ((uint32_t) p_flt_entry->qmam[5])
        );

    volatile uint32_t * pn_qsftvls = &p_reg_switch->P0_QSFTVL0;
    pn_qsftvls += ((ETHSW_QSFTVL_OFFSET_PORT * port) + (ETHSW_QSFTVL_OFFSET_SID * sid));
    *pn_qsftvls = (uint32_t) (
        ((uint32_t) (p_flt_entry->tagmd << R_ETHSW_P0_QSFTVL0_TAGMD_Pos) & R_ETHSW_P0_QSFTVL0_TAGMD_Msk) |
        ((uint32_t) (p_flt_entry->dei << R_ETHSW_P0_QSFTVL0_DEI_Pos) & R_ETHSW_P0_QSFTVL0_DEI_Msk) |
        ((uint32_t) (p_flt_entry->pcp << R_ETHSW_P0_QSFTVL0_PCP_Pos) & R_ETHSW_P0_QSFTVL0_PCP_Msk) |
        ((uint32_t) ((p_flt_entry->vlanid[0] << 8 | p_flt_entry->vlanid[1]) << R_ETHSW_P0_QSFTVL0_VLANID_Pos) &
         R_ETHSW_P0_QSFTVL0_VLANID_Msk)
        );

    volatile uint32_t * pn_qsftvlms = &p_reg_switch->P0_QSFTVLM0;
    pn_qsftvlms += ((ETHSW_QSFTVLM_OFFSET_PORT * port) + (ETHSW_QSFTVLM_OFFSET_SID * sid));
    *pn_qsftvlms = (uint32_t) (
        ((uint32_t) (p_flt_entry->deim << R_ETHSW_P0_QSFTVLM0_DEIM_Pos) & R_ETHSW_P0_QSFTVLM0_DEIM_Msk) |
        ((uint32_t) (p_flt_entry->pcpm << R_ETHSW_P0_QSFTVLM0_PCPM_Pos) & R_ETHSW_P0_QSFTVLM0_PCPM_Msk) |
        ((uint32_t) ((p_flt_entry->vlanidm[0] << 8 | p_flt_entry->vlanidm[1]) << R_ETHSW_P0_QSFTVLM0_VLANIDM_Pos) &
         R_ETHSW_P0_QSFTVLM0_VLANIDM_Msk)
        );

    return err;
}                                      /* End of function R_ETHSW_EfpFilterTableSet()*/

/********************************************************************************************************************//**
 * Qci stream filter table Enable
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpQsfTableEnable (ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint32_t sid, bool qste)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_EFP_SID_NUM_MAX > sid, FSP_ERR_INVALID_ARGUMENT);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    volatile uint32_t * pn_qsftbls = &p_reg_switch->P0_QSFTBL0;
    pn_qsftbls  += ((ETHSW_QSFTBL_OFFSET_PORT * port) + (ETHSW_QSFTBL_OFFSET_SID * sid));
    *pn_qsftbls &= (uint32_t) ~R_ETHSW_P0_QSFTBL0_QSTE_Msk;
    *pn_qsftbls |= (uint32_t) ((uint32_t) (qste << R_ETHSW_P0_QSFTBL0_QSTE_Pos) & R_ETHSW_P0_QSFTBL0_QSTE_Msk);

    return err;
}                                      /* End of function R_ETHSW_EfpQsfTableEnable()*/

/********************************************************************************************************************//**
 * Set gating check parameters
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpGatingCheckSet (ether_switch_ctrl_t * const p_ctrl,
                                     uint32_t                    port,
                                     uint32_t                    sid,
                                     ethsw_efp_gating_check_t  * p_gt_chk)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_EFP_SID_NUM_MAX > sid, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_gt_chk), FSP_ERR_INVALID_POINTER);
 #endif

    /**************************************************************************
     * 28.4.19.8 Gating Check
     **************************************************************************/
    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* Figure 28.62 Gating check process flow */
    volatile uint32_t * pn_qsftbls = &p_reg_switch->P0_QSFTBL0;
    pn_qsftbls  += ((ETHSW_QSFTBL_OFFSET_PORT * port) + (ETHSW_QSFTBL_OFFSET_SID * sid));
    *pn_qsftbls &= (uint32_t) ~R_ETHSW_P0_QSFTBL0_GAIDV_Msk;
    *pn_qsftbls |=
        (uint32_t) (((uint32_t) p_gt_chk->gaidv << R_ETHSW_P0_QSFTBL0_GAIDV_Pos) & R_ETHSW_P0_QSFTBL0_GAIDV_Msk);
    *pn_qsftbls &= (uint32_t) ~R_ETHSW_P0_QSFTBL0_GAID_Msk;
    *pn_qsftbls |=
        (uint32_t) (((uint32_t) p_gt_chk->gaid << R_ETHSW_P0_QSFTBL0_GAID_Pos) & R_ETHSW_P0_QSFTBL0_GAID_Msk);

    volatile uint32_t * pn_qgmod = &p_reg_switch->P0_QGMOD;
    pn_qgmod  += (ETHSW_QGMOD_OFFSET * port);
    *pn_qgmod &= ~((1U << p_gt_chk->gaid) << R_ETHSW_P0_QGMOD_QGMOD_Pos);
    *pn_qgmod |= (uint32_t) ((uint32_t) (p_gt_chk->qgmod << p_gt_chk->gaid) & R_ETHSW_P0_QGMOD_QGMOD_Msk);

    return err;
}                                      /* End of function R_ETHSW_EfpGatingCheckSet()*/

/********************************************************************************************************************//**
 * Set SDUmax verification parameters
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpSDUmaxVerificationSet (ether_switch_ctrl_t * const        p_ctrl,
                                            uint32_t                           port,
                                            uint32_t                           sid,
                                            ethsw_efp_sdu_max_verification_t * p_sdumax)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_EFP_SID_NUM_MAX > sid, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_sdumax), FSP_ERR_INVALID_POINTER);
 #endif

    /**************************************************************************
     * 28.4.19.9 SDUmax Verification
     **************************************************************************/
    p_reg_switch = p_instance_ctrl->p_reg_switch;

    volatile uint32_t * pn_qsftbls = &p_reg_switch->P0_QSFTBL0;
    pn_qsftbls  += ((ETHSW_QSFTBL_OFFSET_PORT * port) + (ETHSW_QSFTBL_OFFSET_SID * sid));
    *pn_qsftbls &=
        (uint32_t) ~(R_ETHSW_P0_QSFTBL0_QSMSM_Msk | R_ETHSW_P0_QSFTBL0_MSDUE_Msk | R_ETHSW_P0_QSFTBL0_MSDU_Msk);
    *pn_qsftbls |= (uint32_t) (
        (((uint32_t) p_sdumax->qsmsm << R_ETHSW_P0_QSFTBL0_QSMSM_Pos) & R_ETHSW_P0_QSFTBL0_QSMSM_Msk) |
        (((uint32_t) p_sdumax->msdue << R_ETHSW_P0_QSFTBL0_MSDUE_Pos) & R_ETHSW_P0_QSFTBL0_MSDUE_Msk) |
        (((uint32_t) p_sdumax->msdu << R_ETHSW_P0_QSFTBL0_MSDU_Pos) & R_ETHSW_P0_QSFTBL0_MSDU_Msk)
        );

    return err;
}                                      /* End of function R_ETHSW_EfpSDUmaxVerificationSet()*/

/********************************************************************************************************************//**
 * Set flow metering parameters
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpFlowMeteringSet (ether_switch_ctrl_t * const p_ctrl,
                                      uint32_t                    port,
                                      uint32_t                    sid,
                                      ethsw_efp_flow_metering_t * p_meter)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_EFP_SID_NUM_MAX > sid, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_meter), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(ETHSW_EFP_MEID_NUM_MAX > p_meter->meid, FSP_ERR_INVALID_ARGUMENT);
 #endif

    /**************************************************************************
     * 28.4.19.10 Flow Metering
     **************************************************************************/
    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* Figure 28.66 Flow Meter setting flow */
    volatile uint32_t * pn_qsftbls = &p_reg_switch->P0_QSFTBL0;
    pn_qsftbls  += ((ETHSW_QSFTBL_OFFSET_PORT * port) + (ETHSW_QSFTBL_OFFSET_SID * sid));
    *pn_qsftbls &= (uint32_t) ~(R_ETHSW_P0_QSFTBL0_MEIDV_Msk | R_ETHSW_P0_QSFTBL0_MEID_Msk);
    *pn_qsftbls |= (uint32_t) (
        (((uint32_t) p_meter->meidv << R_ETHSW_P0_QSFTBL0_MEIDV_Pos) & R_ETHSW_P0_QSFTBL0_MEIDV_Msk) |
        (((uint32_t) p_meter->meid << R_ETHSW_P0_QSFTBL0_MEID_Pos) & R_ETHSW_P0_QSFTBL0_MEID_Msk)
        );

    volatile uint32_t * pn_qmdescm = &p_reg_switch->P0_QMDESC0;
    pn_qmdescm  += ((ETHSW_QMDESC_OFFSET_PORT * port) + (ETHSW_QMDESC_OFFSET_METER * p_meter->meid));
    *pn_qmdescm &= (uint32_t) ~(R_ETHSW_P0_QMDESC0_CF_Msk | R_ETHSW_P0_QMDESC0_MM_Msk | R_ETHSW_P0_QMDESC0_RFD_Msk);
    *pn_qmdescm |= (uint32_t) (
        (((uint32_t) p_meter->cf << R_ETHSW_P0_QMDESC0_CF_Pos) |
         ((uint32_t) p_meter->mm << R_ETHSW_P0_QMDESC0_MM_Pos) |
         ((uint32_t) p_meter->rfd << R_ETHSW_P0_QMDESC0_RFD_Pos)) &
        (R_ETHSW_P0_QMDESC0_CF_Msk | R_ETHSW_P0_QMDESC0_MM_Msk | R_ETHSW_P0_QMDESC0_RFD_Msk)
        );

    volatile uint32_t * pn_qmcbscm = &p_reg_switch->P0_QMCBSC0;
    pn_qmcbscm += ((ETHSW_QMCBSC_OFFSET_PORT * port) + (ETHSW_QMCBSC_OFFSET_METER * p_meter->meid));
    *pn_qmcbscm = (p_meter->cbs << R_ETHSW_P0_QMCBSC0_CBS_Pos) & R_ETHSW_P0_QMCBSC0_CBS_Msk;

    volatile uint32_t * pn_qmcircm = &p_reg_switch->P0_QMCIRC0;
    pn_qmcircm += ((ETHSW_QMCIRC_OFFSET_PORT * port) + (ETHSW_QMCIRC_OFFSET_METER * p_meter->meid));
    *pn_qmcircm = (p_meter->cir << R_ETHSW_P0_QMCIRC0_CIR_Pos) & R_ETHSW_P0_QMCIRC0_CIR_Msk;

    volatile uint32_t * pn_qmec = &p_reg_switch->P0_QMEC;
    pn_qmec  += (ETHSW_QMEC_OFFSET * port);
    *pn_qmec &= ~((1U << p_meter->meid) << R_ETHSW_P0_QMEC_ME_Pos);
    *pn_qmec |= (uint32_t) (((uint32_t) p_meter->me << p_meter->meid) & R_ETHSW_P0_QMEC_ME_Msk);

    return err;
}                                      /* End of function R_ETHSW_EfpFlowMeteringSet()*/

/********************************************************************************************************************//**
 * Configuration of interrupt source for Extended Frame Parser
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpInterruptEnable (ether_switch_ctrl_t * const p_ctrl,
                                      uint32_t port,
                                      ethsw_efp_interrupt_source_t * p_conf,
                                      void (* p_efp_callback_func)(uint32_t, ethsw_efp_event_t))
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_conf), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_efp_callback_func), FSP_ERR_INVALID_POINTER);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* Qci stream filter error interrupt (max SDU over drop) */
    volatile uint32_t * pn_qseie = &p_reg_switch->P0_QSEIE;
    volatile uint32_t * pn_qseid = &p_reg_switch->P0_QSEID;
    pn_qseie  += (ETHSW_QSEIE_OFFSET * port);
    pn_qseid  += (ETHSW_QSEID_OFFSET * port);
    *pn_qseie |= (uint32_t) ((uint32_t) p_conf->qsmoi & R_ETHSW_P0_QSEIE_QSMOIE_Msk);
    *pn_qseid |= (uint32_t) ((uint32_t) (p_conf->qsmoi ^ ETHSW_QSMOI_MASK) & R_ETHSW_P0_QSEID_QSMOID_Msk);

    /* Qci stream gate error interrupt (gating error drop) */
    volatile uint32_t * pn_qgeie = &p_reg_switch->P0_QGEIE;
    volatile uint32_t * pn_qgeid = &p_reg_switch->P0_QGEID;
    pn_qgeie  += (ETHSW_QGEIE_OFFSET * port);
    pn_qgeid  += (ETHSW_QGEID_OFFSET * port);
    *pn_qgeie |= (uint32_t) ((uint32_t) p_conf->qgmoi & R_ETHSW_P0_QGEIE_QGMOIE_Msk);
    *pn_qgeid |= (uint32_t) ((uint32_t) (p_conf->qgmoi ^ ETHSW_QGMOI_MASK) & R_ETHSW_P0_QGEID_QGMOID_Msk);

    /* Qci meter error interrupt (flow meter drop) */
    volatile uint32_t * pn_qmeie = &p_reg_switch->P0_QMEIE;
    volatile uint32_t * pn_qmeid = &p_reg_switch->P0_QMEID;
    pn_qmeie  += (ETHSW_QMEIE_OFFSET * port);
    pn_qmeid  += (ETHSW_QMEID_OFFSET * port);
    *pn_qmeie |= (uint32_t) ((uint32_t) p_conf->qrfi & R_ETHSW_P0_QMEIE_QRFIE_Msk);
    *pn_qmeid |= (uint32_t) ((uint32_t) (p_conf->qrfi ^ ETHSW_QRFI_MASK) & R_ETHSW_P0_QMEID_QRFID_Msk);

    /* Frame parser error interrupt (runtime error) */
    volatile uint32_t * pn_error_mask = &p_reg_switch->P0_ERROR_MASK;
    pn_error_mask += (ETHSW_ERROR_MASK_OFFSET * port);
    uint32_t err_msk_msk = (
        R_ETHSW_P0_ERROR_MASK_MEOVRSZ_Msk | R_ETHSW_P0_ERROR_MASK_MEUNDSZ_Msk |
        R_ETHSW_P0_ERROR_MASK_MPOVRSZ_Msk | R_ETHSW_P0_ERROR_MASK_MPUNDSZ_Msk |
        R_ETHSW_P0_ERROR_MASK_MSOPERR_Msk
        );
    *pn_error_mask &= ~(err_msk_msk);
    *pn_error_mask  = ((uint32_t) p_conf->errmask & err_msk_msk);

    /* Register Extended Frame Parser (EFP) interrupt handler */
    gp_ethsw_efp_callback = p_efp_callback_func;

    /* Enable Interrupt for Extended Frame Parser */
    if ((p_conf->qsmoi != 0) || (p_conf->qgmoi != 0) || (p_conf->qrfi != 0))
    {
        p_reg_switch->INT_CONFIG_b.EFP_INT = 1;
    }
    else
    {
        p_reg_switch->INT_CONFIG_b.EFP_INT = 0;
    }

    return err;
}                                      /* End of function R_ETHSW_EfpInterruptEnable()*/

/********************************************************************************************************************//**
 * Get statistics information related Extended Frame Parser
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpStatusGet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_efp_status_t * p_info)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((NULL != p_info), FSP_ERR_INVALID_POINTER);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* Qci Stream Filter Error Interrupt Status */
    volatile uint32_t * pn_qseis = &p_reg_switch->P0_QSEIS;
    pn_qseis      += (ETHSW_QSEIS_OFFSET * port);
    p_info->qsmois = (*pn_qseis & R_ETHSW_P0_QSEIS_QSMOIS_Msk);

    /* Qci Gate Error Interrupt Status */
    volatile uint32_t * pn_qgeis = &p_reg_switch->P0_QGEIS;
    pn_qgeis      += (ETHSW_QGEIS_OFFSET * port);
    p_info->qgmois = (*pn_qgeis & R_ETHSW_P0_QGEIS_QGMOIS_Msk);

    /* Qci Meter Error Interrupt Statuss */
    volatile uint32_t * pn_qmeis = &p_reg_switch->P0_QMEIS;
    pn_qmeis     += (ETHSW_QMEIS_OFFSET * port);
    p_info->qrfis = (*pn_qmeis & R_ETHSW_P0_QMEIS_QRFIS_Msk);

    /* Frame Parser Runtime Error Status */
    volatile uint32_t * pn_error_status = &p_reg_switch->P0_ERROR_STATUS;
    pn_error_status     += (ETHSW_ERROR_STATUS_OFFSET * port);
    p_info->error_status = (*pn_error_status & ETHSW_ERROR_STATUS_MASK);

    return err;
}                                      /* End of function R_ETHSW_EfpStatusGet()*/

/********************************************************************************************************************//**
 * Enable channel by port for Extended Frame Parser
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_EfpChannelEnable (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
 #endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;
    if (enable)
    {
        p_reg_switch->CHANNEL_ENABLE |= (uint32_t) ((1U << port) & ETHSW_CHANNEL_ENABLE_MASK);
        while (((p_reg_switch->CHANNEL_STATE >> port) & 0x1U) != 1U)
        {
            ;
        }
    }
    else
    {
        p_reg_switch->CHANNEL_DISABLE |= (uint32_t) ((1U << port) & ETHSW_CHANNEL_DISABLE_MASK);
        while (((p_reg_switch->CHANNEL_STATE >> port) & 0x1U) != 0U)
        {
            ;
        }
    }

    return err;
}                                      /* End of function R_ETHSW_EfpChannelEnable()*/

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

/********************************************************************************************************************//**
 * Sets Rx pattern matcher
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_RxPatternMatcherSet (ether_switch_ctrl_t * const  p_ctrl,
                                       ethsw_rx_pattern_matcher_t * p_rx_pattern_matcher)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;

    R_ETHSW_Type volatile * p_reg_switch;

    uint32_t pattern_mask;
    uint32_t pattern_ctrl;
    uint32_t port_num;
    uint32_t pattern_port_mask;
    uint32_t port_mask;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_rx_pattern_matcher), FSP_ERR_INVALID_POINTER);

    ETHSW_ERROR_RETURN(ETHSW_RX_PATTERN_MATCHER_COUNT > p_rx_pattern_matcher->pattern_sel, FSP_ERR_INVALID_ARGUMENT)
    ETHSW_ERROR_RETURN((NULL != p_rx_pattern_matcher->p_pattern_ctrl), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_rx_pattern_matcher->p_pattern_data),
                       FSP_ERR_INVALID_POINTER);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;
    pattern_mask = 1U << p_rx_pattern_matcher->pattern_sel;

    // Disable Rx Patter Mattcher
    p_reg_switch->RXMATCH_CONFIG_b[0].PATTERN_EN &= ~pattern_mask & R_ETHSW_RXMATCH_CONFIG_PATTERN_EN_Msk;
    p_reg_switch->RXMATCH_CONFIG_b[1].PATTERN_EN &= ~pattern_mask & R_ETHSW_RXMATCH_CONFIG_PATTERN_EN_Msk;
    p_reg_switch->RXMATCH_CONFIG_b[2].PATTERN_EN &= ~pattern_mask & R_ETHSW_RXMATCH_CONFIG_PATTERN_EN_Msk;
    p_reg_switch->RXMATCH_CONFIG_b[3].PATTERN_EN &= ~pattern_mask & R_ETHSW_RXMATCH_CONFIG_PATTERN_EN_Msk;

    pattern_port_mask = p_rx_pattern_matcher->pattern_port_mask.mask;

    if (0 != pattern_port_mask)        // Enable
    {
        // Set Rx patter mattcher control
        pattern_ctrl  = 0;
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->match_not << R_ETHSW_PATTERN_CTRL_MATCH_NOT_Pos) &
            R_ETHSW_PATTERN_CTRL_MATCH_NOT_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->mgmt_fwd << R_ETHSW_PATTERN_CTRL_MGMTFWD_Pos) &
            R_ETHSW_PATTERN_CTRL_MGMTFWD_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->discard << R_ETHSW_PATTERN_CTRL_DISCARD_Pos) &
            R_ETHSW_PATTERN_CTRL_DISCARD_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->set_prio << R_ETHSW_PATTERN_CTRL_SET_PRIO_Pos) &
            R_ETHSW_PATTERN_CTRL_SET_PRIO_Msk);
        pattern_ctrl |=
            (uint32_t) ((uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->mode << R_ETHSW_PATTERN_CTRL_MODE_Pos) &
                        R_ETHSW_PATTERN_CTRL_MODE_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->timer_sel_ovr <<
                        R_ETHSW_PATTERN_CTRL_TIMER_SEL_OVR_Pos) &
            R_ETHSW_PATTERN_CTRL_TIMER_SEL_OVR_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->force_forward <<
                        R_ETHSW_PATTERN_CTRL_FORCE_FORWARD_Pos) &
            R_ETHSW_PATTERN_CTRL_FORCE_FORWARD_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->hub_trigger << R_ETHSW_PATTERN_CTRL_HUBTRIGGER_Pos) &
            R_ETHSW_PATTERN_CTRL_HUBTRIGGER_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->match_red << R_ETHSW_PATTERN_CTRL_MATCH_RED_Pos) &
            R_ETHSW_PATTERN_CTRL_MATCH_RED_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->match_not_red <<
                        R_ETHSW_PATTERN_CTRL_MATCH_NOT_RED_Pos) &
            R_ETHSW_PATTERN_CTRL_MATCH_NOT_RED_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->vlan_skip << R_ETHSW_PATTERN_CTRL_VLAN_SKIP_Pos) &
            R_ETHSW_PATTERN_CTRL_VLAN_SKIP_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->priority << R_ETHSW_PATTERN_CTRL_PRIORITY_Pos) &
            R_ETHSW_PATTERN_CTRL_PRIORITY_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->learning_dis << R_ETHSW_PATTERN_CTRL_LEARNING_DIS_Pos) &
            R_ETHSW_PATTERN_CTRL_LEARNING_DIS_Msk);

        port_mask = p_rx_pattern_matcher->p_pattern_ctrl->port_mask.mask;

        pattern_ctrl |=
            (uint32_t) ((port_mask << R_ETHSW_PATTERN_CTRL_PORTMASK_Pos) & R_ETHSW_PATTERN_CTRL_PORTMASK_Msk);

        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->imc_trigger << R_ETHSW_PATTERN_CTRL_IMC_TRIGGER_Pos) &
            R_ETHSW_PATTERN_CTRL_IMC_TRIGGER_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->imc_trigger_dely <<
                        R_ETHSW_PATTERN_CTRL_IMC_TRIGGER_DLY_Pos) & R_ETHSW_PATTERN_CTRL_IMC_TRIGGER_DLY_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->swap_bytes << R_ETHSW_PATTERN_CTRL_SWAP_BYTES_Pos) &
            R_ETHSW_PATTERN_CTRL_SWAP_BYTES_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->match_lt << R_ETHSW_PATTERN_CTRL_MATCH_LT_Pos) &
            R_ETHSW_PATTERN_CTRL_MATCH_LT_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->timer_sel << R_ETHSW_PATTERN_CTRL_TIMER_SEL_Pos) &
            R_ETHSW_PATTERN_CTRL_TIMER_SEL_Msk);
        pattern_ctrl |= (uint32_t) (
            (uint32_t) (p_rx_pattern_matcher->p_pattern_ctrl->queue_sel << R_ETHSW_PATTERN_CTRL_QUEUESEL_Pos) &
            R_ETHSW_PATTERN_CTRL_QUEUESEL_Msk);

        p_reg_switch->PATTERN_CTRL[p_rx_pattern_matcher->pattern_sel] = pattern_ctrl;

        // Set Rx patter mattcher data
        p_reg_switch->PATTERN_SEL  = p_rx_pattern_matcher->pattern_sel;
        p_reg_switch->PTRN_CMP_30  = p_rx_pattern_matcher->p_pattern_data->reg.cmp_30;
        p_reg_switch->PTRN_CMP_74  = p_rx_pattern_matcher->p_pattern_data->reg.cmp_74;
        p_reg_switch->PTRN_CMP_118 = p_rx_pattern_matcher->p_pattern_data->reg.cmp_118;
        p_reg_switch->PTRN_MSK_30  = p_rx_pattern_matcher->p_pattern_data->reg.msk_30;
        p_reg_switch->PTRN_MSK_74  = p_rx_pattern_matcher->p_pattern_data->reg.msk_74;
        p_reg_switch->PTRN_MSK_118 = p_rx_pattern_matcher->p_pattern_data->reg.msk_118;

        if (true == p_rx_pattern_matcher->pattern_int) // IRQ enable
        {
            p_reg_switch->PATTERN_IRQ_CONTROL_b.MATCHINT =
                (p_reg_switch->PATTERN_IRQ_CONTROL_b.MATCHINT | pattern_mask) &
                R_ETHSW_PATTERN_IRQ_CONTROL_MATCHINT_Msk;
            p_reg_switch->PATTERN_IRQ_CONTROL_b.ERROR_INT = 0xF;
            p_reg_switch->INT_CONFIG_b.PATTERN_INT        = 1;
        }
        else                           // IRQ disabe
        {
            p_reg_switch->PATTERN_IRQ_CONTROL_b.MATCHINT &= ~pattern_mask & R_ETHSW_PATTERN_IRQ_CONTROL_MATCHINT_Msk;
            if (0 == p_reg_switch->PATTERN_IRQ_CONTROL_b.MATCHINT)
            {
                p_reg_switch->PATTERN_IRQ_CONTROL_b.ERROR_INT = 0;
                p_reg_switch->INT_CONFIG_b.PATTERN_INT        = 0;
            }
        }

        // Enable Rx Patter Mattcher
        for (port_num = 0; port_num <= ETHSW_HOST_PORT_NUMBER; port_num++)
        {
            if (pattern_port_mask & (1U << port_num))
            {
                p_reg_switch->RXMATCH_CONFIG[port_num] |= pattern_mask;
            }
        }
    }
    else                               // Disable
    {
        p_reg_switch->PATTERN_CTRL[p_rx_pattern_matcher->pattern_sel] = 0;

        p_reg_switch->PATTERN_SEL  = p_rx_pattern_matcher->pattern_sel;
        p_reg_switch->PTRN_CMP_30  = 0;
        p_reg_switch->PTRN_CMP_74  = 0;
        p_reg_switch->PTRN_CMP_118 = 0;
        p_reg_switch->PTRN_MSK_30  = 0;
        p_reg_switch->PTRN_MSK_74  = 0;
        p_reg_switch->PTRN_MSK_118 = 0;

        // IRQ disabe
        p_reg_switch->PATTERN_IRQ_CONTROL_b.MATCHINT &= ~pattern_mask & R_ETHSW_PATTERN_IRQ_CONTROL_MATCHINT_Msk;
        if (0 == p_reg_switch->PATTERN_IRQ_CONTROL_b.MATCHINT)
        {
            p_reg_switch->PATTERN_IRQ_CONTROL_b.ERROR_INT = 0;
            p_reg_switch->INT_CONFIG_b.PATTERN_INT        = 0;
        }
    }

    return err;
}

/********************************************************************************************************************//**
 * Register the callback function for getting events from Rx pattern matcher.
 * Unregister if NULL is specified in the callback function.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_RxPatternMatcherCallback (
    ether_switch_ctrl_t * const         p_ctrl,
    void (                            * p_rx_pattern_callback_func)(
        ethsw_rx_pattern_event_t        event,
        ethsw_rx_pattern_event_data_t * p_data))
{
    fsp_err_t err = FSP_SUCCESS;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;

    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#else
    FSP_PARAMETER_NOT_USED(p_ctrl);
#endif

    gp_ethsw_rx_pattern_callback = p_rx_pattern_callback_func;

    return err;
}

/********************************************************************************************************************//**
 * Set Preemptable queues configures
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PreemptQueueSet (ether_switch_ctrl_t * const p_ctrl, ethsw_preempt_queue_t * p_preempt_queue)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_preempt_queue), FSP_ERR_INVALID_POINTER);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MMCTL_PREEMPT_QUEUES_b.PREEMPT_ENA       = p_preempt_queue->preempt_queues;
    p_reg_switch->MMCTL_PREEMPT_QUEUES_b.PREEMPT_ON_QCLOSE = p_preempt_queue->preempt_on_qclose;

    return err;
}                                      /* End of function R_ETHSW_PreemptQueueSet() */

/********************************************************************************************************************//**
 * Set Qbu frame configuration of each ports
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PreemptPortControlConfigSet (ether_switch_ctrl_t * const        p_ctrl,
                                               uint32_t                           port,
                                               ethsw_preempt_port_ctrl_config_t * p_preempt_port_ctrl)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_preempt_port_ctrl), FSP_ERR_INVALID_POINTER);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT > port), FSP_ERR_INVALID_ARGUMENT);                 /* port           : 0 to 2 */
    ETHSW_ERROR_RETURN(((R_ETHSW_BR_CONTROL_P0_ADDFRAGSIZE_Msk >> R_ETHSW_BR_CONTROL_P0_ADDFRAGSIZE_Pos) >=
                        p_preempt_port_ctrl->addfragsize),
                       FSP_ERR_INVALID_ARGUMENT);                                            /* addfragsize    : 0 to 3 */
    ETHSW_ERROR_RETURN((0 < p_preempt_port_ctrl->tx_verify_time), FSP_ERR_INVALID_ARGUMENT); /* tx_verity_time : 1 to 127 */
    ETHSW_ERROR_RETURN(((R_ETHSW_BR_CONTROL_P0_TX_VERIFY_TIME_Msk >> R_ETHSW_BR_CONTROL_P0_TX_VERIFY_TIME_Pos) >=
                        p_preempt_port_ctrl->tx_verify_time),
                       FSP_ERR_INVALID_ARGUMENT);                                            /*                           */
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* set to BR_CONTROL_Pn ( n = port : 0 to 2 ) */
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->VERIFY_DIS = p_preempt_port_ctrl->verify_dis;
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->RESPONSE_DIS = p_preempt_port_ctrl->response_dis;
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET *
      port))->ADDFRAGSIZE =
        (p_preempt_port_ctrl->addfragsize &
         (R_ETHSW_BR_CONTROL_P0_ADDFRAGSIZE_Msk >> R_ETHSW_BR_CONTROL_P0_ADDFRAGSIZE_Pos));
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET *
      port))->TX_VERIFY_TIME =
        (p_preempt_port_ctrl->tx_verify_time &
         (R_ETHSW_BR_CONTROL_P0_TX_VERIFY_TIME_Msk >> R_ETHSW_BR_CONTROL_P0_TX_VERIFY_TIME_Pos));
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->RX_STRICT_PRE = p_preempt_port_ctrl->rx_strict_pre;
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->RX_BR_SMD_DIS = p_preempt_port_ctrl->rx_br_smd_dis;
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->RX_STRICT_BR_CTL = p_preempt_port_ctrl->rx_strict_br_ctl;
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->TX_MCRC_INV = p_preempt_port_ctrl->tx_mcrc_inv;
    ((&p_reg_switch->BR_CONTROL_P0_b) +
     (ETHSW_BR_CONTROL_OFFSET * port))->RX_MCRC_INV = p_preempt_port_ctrl->rx_mcrc_inv;

    return err;
}                                      /* End of function R_ETHSW_PreemptPortControlConfigSet() */

/********************************************************************************************************************//**
 * Enable/Disable Qbu frame preempt at each ports
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PreemptPortControlEnableSet (ether_switch_ctrl_t * const p_ctrl,
                                               uint32_t                    port,
                                               bool                        frame_preempt_enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT > port), FSP_ERR_INVALID_ARGUMENT); /* port : 0 to 2 */
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    /* set to BR_CONTROL_Pn.PREEMPT_ENA ( n = port : 0 to 2 ) */
    ((&p_reg_switch->BR_CONTROL_P0_b) + (ETHSW_BR_CONTROL_OFFSET * port))->PREEMPT_ENA = frame_preempt_enable;

    return err;
}                                      /* End of function R_ETHSW_PreemptPortControlEnableSet() */

/********************************************************************************************************************//**
 * forces a preempt request using MM_CTL.request(hold_req).
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PreemptHoldReqForceSet (ether_switch_ctrl_t * const p_ctrl,
                                          ethsw_port_mask_t           holdreq_request_port_mask)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(((R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_FORCE_Msk >>
                         R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_FORCE_Pos) >= holdreq_request_port_mask.mask),
                       FSP_ERR_INVALID_ARGUMENT); /* portmask : 000b to 111b */
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MMCTL_HOLD_CONTROL_b.Q_HOLD_REQ_FORCE = holdreq_request_port_mask.mask &
                                                          (R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_FORCE_Msk >>
                                                           R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_FORCE_Pos);

    return err;
}                                      /* End of function R_ETHSW_PreemptHoldReqForceSet() */

/********************************************************************************************************************//**
 * forces a release of preemption request using MM_CTL.request(hold_req).
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PreemptHoldReqReleaseSet (ether_switch_ctrl_t * const p_ctrl,
                                            ethsw_port_mask_t           holdreq_release_port_mask)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(((R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_RELEASE_Msk >>
                         R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_RELEASE_Pos) >= holdreq_release_port_mask.mask),
                       FSP_ERR_INVALID_ARGUMENT); /* portmask : 000b to 111b */
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MMCTL_HOLD_CONTROL_b.Q_HOLD_REQ_RELEASE = holdreq_release_port_mask.mask &
                                                            (R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_RELEASE_Msk >>
                                                             R_ETHSW_MMCTL_HOLD_CONTROL_Q_HOLD_REQ_RELEASE_Pos);

    return err;
}                                      /* End of function R_ETHSW_PreemptHoldReqReleaseSet() */

/********************************************************************************************************************//**
 * Get a status of frame preemption
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PreemptStatusGet (ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    ethsw_preempt_status_t    * p_preempt_status)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_preempt_status), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT > port), FSP_ERR_INVALID_ARGUMENT); /* port : 0 to 2 */
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_preempt_status->verify_status =
        (ethsw_preempt_verify_status_t) (((&p_reg_switch->STATUS_P0_b) + (ETHSW_STATUS_OFFSET * port))->BR_VERIF_ST);
    p_preempt_status->preempt_state  = (bool) ((p_reg_switch->MMCTL_PREEMPT_STATUS_b.PREEMPT_STATE >> port) & 0x1);
    p_preempt_status->hold_req_state = (bool) ((p_reg_switch->MMCTL_PREEMPT_STATUS_b.HOLD_REQ_STATE >> port) & 0x1);
    p_preempt_status->min_pfrm_adj   =
        (uint8_t) (p_reg_switch->MMCTL_PREEMPT_EXTRA_b.MIN_PFRM_ADJ &
                   (R_ETHSW_MMCTL_PREEMPT_EXTRA_MIN_PFRM_ADJ_Msk >> R_ETHSW_MMCTL_PREEMPT_EXTRA_MIN_PFRM_ADJ_Pos));
    p_preempt_status->last_pfrm_adj =
        (uint8_t) (p_reg_switch->MMCTL_PREEMPT_EXTRA_b.LAST_PFRM_ADJ &
                   (R_ETHSW_MMCTL_PREEMPT_EXTRA_LAST_PFRM_ADJ_Msk >> R_ETHSW_MMCTL_PREEMPT_EXTRA_LAST_PFRM_ADJ_Pos));

    return err;
}                                      /* End of function R_ETHSW_PreemptStatusGet() */

/*******************************************************************************************************************//**
 * Sets queue gate.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_MmctlQgateSet (ether_switch_ctrl_t * const p_ctrl, ethsw_mmclt_qgate_t * p_mmctl_qgate)
{
    uint32_t mmctl_qgate;
    uint32_t port_mask;
    uint32_t queue_gate;
    uint32_t index;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_mmctl_qgate), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    port_mask = p_mmctl_qgate->port_mask.mask;

    queue_gate = 0;
    for (index = 0; index < ETHSW_QUEUE_COUNT; index++)
    {
        queue_gate |= (uint32_t) (p_mmctl_qgate->action[index] & 0x3) << (index * 2);
    }

    mmctl_qgate  = (port_mask << R_ETHSW_MMCTL_QGATE_PORT_MASK_Pos) & R_ETHSW_MMCTL_QGATE_PORT_MASK_Msk;
    mmctl_qgate |= (uint32_t) ((queue_gate << R_ETHSW_MMCTL_QGATE_QUEUE_GATE_Pos) & R_ETHSW_MMCTL_QGATE_QUEUE_GATE_Msk);

    p_switch_reg->MMCTL_QGATE = mmctl_qgate;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_MmctlQgateSet() */

/********************************************************************************************************************//**
 * Sets memory pool size, in sells
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_MmctlPoolSizeSet (ether_switch_ctrl_t * const p_ctrl, ethsw_mmctl_pool_size_t * p_pool_size)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_pool_size), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(((ETHSW_MMCTL_POOL_ID_0 == p_pool_size->pool_id) ||
                        (ETHSW_MMCTL_POOL_ID_1 == p_pool_size->pool_id) ||
                        (ETHSW_MMCTL_POOL_ID_GLOBAL == p_pool_size->pool_id)),
                       FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    switch (p_pool_size->pool_id)
    {
        case ETHSW_MMCTL_POOL_ID_0:
        {
            p_reg_switch->MMCTL_POOL0_CTR_b.CELLS = p_pool_size->cells &
                                                    (R_ETHSW_MMCTL_POOL0_CTR_CELLS_Msk >>
                                                     R_ETHSW_MMCTL_POOL0_CTR_CELLS_Pos);
            break;
        }

        case ETHSW_MMCTL_POOL_ID_1:
        {
            p_reg_switch->MMCTL_POOL1_CTR_b.CELLS = p_pool_size->cells &
                                                    (R_ETHSW_MMCTL_POOL1_CTR_CELLS_Msk >>
                                                     R_ETHSW_MMCTL_POOL1_CTR_CELLS_Pos);
            break;
        }

        case ETHSW_MMCTL_POOL_ID_GLOBAL:
        {
            p_reg_switch->MMCTL_POOL_GLOBAL_b.CELLS = p_pool_size->cells & R_ETHSW_MMCTL_POOL_GLOBAL_CELLS_Msk;
            break;
        }

        default:
        {
            break;
        }
    }

    if ((0 != p_reg_switch->MMCTL_POOL0_CTR_b.CELLS) ||
        (0 != p_reg_switch->MMCTL_POOL1_CTR_b.CELLS))
    {
        p_reg_switch->IMC_CONFIG_b.RSV_ENA = 1;
    }
    else
    {
        p_reg_switch->IMC_CONFIG_b.RSV_ENA = 0;
    }

    return err;
}                                      /* End of function R_ETHSW_MmctlPoolSizeSet() */

/********************************************************************************************************************//**
 * Assigns a pool to the specified queue.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_MmctlQueueAssignSet (ether_switch_ctrl_t * const p_ctrl, ethsw_mmctl_queue_assign_t * p_queue_assign)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint32_t                qmap;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_queue_assign), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(((ETHSW_MMCTL_POOL_ID_0 == p_queue_assign->pool_id) ||
                        (ETHSW_MMCTL_POOL_ID_1 == p_queue_assign->pool_id) ||
                        (ETHSW_MMCTL_POOL_ID_GLOBAL == p_queue_assign->pool_id)),
                       FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN(ETHSW_QUEUE_COUNT > p_queue_assign->queue_num, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    qmap  = p_reg_switch->MMCTL_POOL_QMAP;
    qmap &= (uint32_t)
            ~((R_ETHSW_MMCTL_POOL_QMAP_Q0_MAP_Msk | R_ETHSW_MMCTL_POOL_QMAP_Q0_ENA_Msk) <<
              (p_queue_assign->queue_num * 4));

    if (ETHSW_MMCTL_POOL_ID_GLOBAL != p_queue_assign->pool_id)
    {
        qmap |= (1U << R_ETHSW_MMCTL_POOL_QMAP_Q0_ENA_Pos) << (p_queue_assign->queue_num * 4);
        qmap |= (uint32_t)
                ((uint32_t) (p_queue_assign->pool_id << R_ETHSW_MMCTL_POOL_QMAP_Q0_MAP_Pos) &
                 R_ETHSW_MMCTL_POOL_QMAP_Q0_MAP_Msk) << (p_queue_assign->queue_num * 4);
    }

    p_reg_switch->MMCTL_POOL_QMAP = qmap;

    return err;
}                                      /* End of function R_ETHSW_MmctlQueueAssignSet() */

/********************************************************************************************************************//**
 * Sets length in bytes of the YELLOW period for specified port.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_MmctlYellowLengthSet (ether_switch_ctrl_t * const p_ctrl,
                                        uint32_t                    port,
                                        ethsw_yellow_length_t     * p_yellow_length)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint32_t                mmctl_yellow_byte_length;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_yellow_length), FSP_ERR_INVALID_POINTER);
#endif

    mmctl_yellow_byte_length =
        (uint32_t) ((uint32_t) (p_yellow_length->enable << R_ETHSW_MMCTL_YELLOW_BYTE_LENGTH_P_YLEN_EN_Pos) &
                    R_ETHSW_MMCTL_YELLOW_BYTE_LENGTH_P_YLEN_EN_Msk);
    mmctl_yellow_byte_length |=
        (uint32_t) ((uint32_t) (p_yellow_length->length << R_ETHSW_MMCTL_YELLOW_BYTE_LENGTH_P_YELLOW_LEN_Pos) &
                    R_ETHSW_MMCTL_YELLOW_BYTE_LENGTH_P_YELLOW_LEN_Msk);

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MMCTL_YELLOW_BYTE_LENGTH_P[port] = mmctl_yellow_byte_length;

    return err;
}                                      /* End of function R_ETHSW_MmctlYellowLengthSet() */

/********************************************************************************************************************//**
 * Sets Queue Flush Event.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_QueueFlushEventSet (ether_switch_ctrl_t * const p_ctrl,
                                      ethsw_queue_flush_event_t * p_queue_flush_event)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint32_t                mmctl_qflush;
    uint32_t                port_mask;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_queue_flush_event), FSP_ERR_INVALID_POINTER);
#endif

    /* remove portmask bit and translate host port if set */
    port_mask = p_queue_flush_event->port_mask.mask;

    mmctl_qflush = (port_mask << R_ETHSW_MMCTL_QFLUSH_PORT_MASK_Pos) & R_ETHSW_MMCTL_QFLUSH_PORT_MASK_Msk;

    mmctl_qflush |= (uint32_t) (((uint32_t) p_queue_flush_event->queue_mask << R_ETHSW_MMCTL_QFLUSH_QUEUE_MASK_Pos) &
                                R_ETHSW_MMCTL_QFLUSH_QUEUE_MASK_Msk);
    mmctl_qflush |= (uint32_t) ((uint32_t) (p_queue_flush_event->action << R_ETHSW_MMCTL_QFLUSH_ACTION_Pos) &
                                R_ETHSW_MMCTL_QFLUSH_ACTION_Msk);

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MMCTL_QFLUSH = mmctl_qflush;

    return err;
}                                      /* End of function R_ETHSW_QueueFlushEventSet() */

/********************************************************************************************************************//**
 * Get the status shows whether there are frames left in the closed queue.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_MmctlQueueClosedNonemptyStatusGet (ether_switch_ctrl_t * const      p_ctrl,
                                                     ethsw_mmctl_qclosed_nonempty_t * p_qclosed_nonempty)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_qclosed_nonempty), FSP_ERR_INVALID_POINTER);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    uint32_t temp = p_reg_switch->MMCTL_P0_3_QCLOSED_NONEMPTY;

#define MASK_8_BIT    (0xff)

    (*p_qclosed_nonempty)[0] =
        (uint8_t) (((temp & R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P0_Q_STATUS_Msk) >>
                    R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P0_Q_STATUS_Pos) & MASK_8_BIT);
    (*p_qclosed_nonempty)[1] =
        (uint8_t) (((temp & R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P1_Q_STATUS_Msk) >>
                    R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P1_Q_STATUS_Pos) & MASK_8_BIT);
    (*p_qclosed_nonempty)[2] =
        (uint8_t) (((temp & R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P2_Q_STATUS_Msk) >>
                    R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P2_Q_STATUS_Pos) & MASK_8_BIT);
    (*p_qclosed_nonempty)[3] =
        (uint8_t) (((temp & R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P3_Q_STATUS_Msk) >>
                    R_ETHSW_MMCTL_P0_3_QCLOSED_NONEMPTY_P3_Q_STATUS_Pos) & MASK_8_BIT);

    return err;
}                                      /* End of function R_ETHSW_MMctlQueueClosedNonemptyStatusGet() */

/*******************************************************************************************************************//**
 * Gets Switch Base Statistics counters.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_StatisticsSwitchGet (ether_switch_ctrl_t * const      p_ctrl,
                                       bool                             clear,
                                       ethsw_statistics_switch_base_t * p_statistics_switch)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_statistics_switch), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_statistics_switch->total_disc      = p_switch_reg->TOTAL_DISC;
    p_statistics_switch->total_byt_disc  = p_switch_reg->TOTAL_BYT_DISC;
    p_statistics_switch->total_frame     = p_switch_reg->TOTAL_FRM;
    p_statistics_switch->total_byt_frame = p_switch_reg->TOTAL_BYT_FRM;

    if (true == clear)
    {
        /* Clear the Statistics counters */
        p_switch_reg->MODE_CONFIG_b.STATSRESET = 1;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_StatisticSwitchGet() */

/*******************************************************************************************************************//**
 * Gets MAC Statistics counters for the specified port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_StatisticsMacGet (ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    ethsw_statistics_mac_t    * p_statistics_mac)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#define SHIFT_32_BITS    32

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_statistics_mac), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_statistics_mac->a_frames_transmitted_ok =
        ((uint64_t) *(&p_switch_reg->AFRAMESTRANSMITTEDOK_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_frames_transmitted_ok |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_frames_received_ok =
        ((uint64_t) *(&p_switch_reg->AFRAMESRECEIVEDOK_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_frames_received_ok |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_frame_check_sequence_errors =
        ((uint64_t) *(&p_switch_reg->AFRAMECHECKSEQUENCEERRORS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_frame_check_sequence_errors |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_alignment_errors =
        ((uint64_t) *(&p_switch_reg->AALIGNMENTERRORS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_alignment_errors |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_octets_transmitted_ok =
        ((uint64_t) *(&p_switch_reg->AOCTETSTRANSMITTEDOK_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_octets_transmitted_ok |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_octets_received_ok =
        ((uint64_t) *(&p_switch_reg->AOCTETSRECEIVEDOK_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_octets_received_ok |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_tx_pause_mac_ctrl_frames =
        ((uint64_t) *(&p_switch_reg->ATXPAUSEMACCTRLFRAMES_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_tx_pause_mac_ctrl_frames |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_rx_pause_mac_ctrl_frames =
        ((uint64_t) *(&p_switch_reg->ARXPAUSEMACCTRLFRAMES_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_rx_pause_mac_ctrl_frames |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_in_errors =
        ((uint64_t) *(&p_switch_reg->IFINERRORS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_in_errors |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_out_errors =
        ((uint64_t) *(&p_switch_reg->IFOUTERRORS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_out_errors |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_in_ucast_pkts =
        ((uint64_t) *(&p_switch_reg->IFINUCASTPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_in_ucast_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_in_multicast_pkts =
        ((uint64_t) *(&p_switch_reg->IFINMULTICASTPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_in_multicast_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_in_broadcast_pkts =
        ((uint64_t) *(&p_switch_reg->IFINBROADCASTPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_in_broadcast_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_out_discards =
        ((uint64_t) *(&p_switch_reg->IFOUTDISCARDS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_out_discards |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_out_ucast_pkts =
        ((uint64_t) *(&p_switch_reg->IFOUTUCASTPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_out_ucast_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_out_multicast_pkts =
        ((uint64_t) *(&p_switch_reg->IFOUTMULTICASTPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_out_multicast_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->if_out_broadcast_pkts =
        ((uint64_t) *(&p_switch_reg->IFOUTBROADCASTPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->if_out_broadcast_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_drop_events =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSDROPEVENTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_drop_events |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSOCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_undersize_pkts =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSUNDERSIZEPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_undersize_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_oversize_pkts =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSOVERSIZEPKTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_oversize_pkts |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_64_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS64OCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_64_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_65_to_127_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS65TO127OCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_65_to_127_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_128_to_255_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS128TO255OCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_128_to_255_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_256_to_511_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS256TO511OCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_256_to_511_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_512_to_1023_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS512TO1023OCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_512_to_1023_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_1024_to_1518_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS1024TO1518OCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_1024_to_1518_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_pkts_1519_to_x_octets =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSPKTS1519TOXOCTETS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_pkts_1519_to_x_octets |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_jabbers =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSJABBERS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_jabbers |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->ether_stats_fragments =
        ((uint64_t) *(&p_switch_reg->ETHERSTATSFRAGMENTS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->ether_stats_fragments |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->vlan_received_ok =
        ((uint64_t) *(&p_switch_reg->VLANRECEIVEDOK_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->vlan_received_ok |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->vlan_transmitted_ok =
        ((uint64_t) *(&p_switch_reg->VLANTRANSMITTEDOK_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->vlan_transmitted_ok |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->frames_retransmitted =
        ((uint64_t) *(&p_switch_reg->FRAMESRETRANSMITTED_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->frames_retransmitted |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_deferred =
        ((uint64_t) *(&p_switch_reg->ADEFERRED_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_deferred |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_multiple_collisions =
        ((uint64_t) *(&p_switch_reg->AMULTIPLECOLLISIONS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_multiple_collisions |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_single_collisions =
        ((uint64_t) *(&p_switch_reg->ASINGLECOLLISIONS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_single_collisions |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_late_collisions =
        ((uint64_t) *(&p_switch_reg->ALATECOLLISIONS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_late_collisions |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_excessive_collisions =
        ((uint64_t) *(&p_switch_reg->AEXCESSIVECOLLISIONS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_excessive_collisions |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    p_statistics_mac->a_carriersense_errors =
        ((uint64_t) *(&p_switch_reg->ACARRIERSENSEERRORS_P0 + (port * ETHSW_PORT_OFFSET / 4)));
    p_statistics_mac->a_carriersense_errors |=
        (((uint64_t) *(&p_switch_reg->STATS_HIWORD_P0 + (port * ETHSW_PORT_OFFSET / 4))) << SHIFT_32_BITS);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_StatisticsMacGet() */

/*******************************************************************************************************************//**
 * Clears MAC Statistics counters for all ports.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_StatisticsMacClear (ether_switch_ctrl_t * const p_ctrl)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->COMMAND_CONFIG_P0_b.SW_RESET = 1;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_StatisticsMacClear() */

#ifdef ETHSW_EFP_FEATURE_SUPPORTED

/*******************************************************************************************************************//**
 * Gets EFP Statistics counters for the specified port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_StatisticsEfpGet (ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    ethsw_statistics_efp_t    * p_statistics_efp)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

 #if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_statistics_efp), FSP_ERR_INVALID_POINTER);
 #endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

 #define ETHSW_QCI_SID_OFFSET      (0x028U / 4) // Register offset for Qci SID in uint32_t
 #define ETHSW_QCI_GATE_OFFSET     (0x008U / 4) // Register offset for Qci Gate in uint32_t
 #define ETHSW_QCI_MATER_OFFSET    (0x014U / 4) // Register offset for Qci Mate in uint32_t

    /* Qci Stream Match Packet Count */
    volatile const uint32_t * pn_qsmfcs = &p_switch_reg->P0_QSMFC0;
    pn_qsmfcs += ((ETHSW_PORT_OFFSET / 4) * port);
    for (uint32_t sid = 0; sid < ETHSW_EFP_SID_COUNT; sid++)
    {
        p_statistics_efp->qci_stream_match_pkts[sid] = (*pn_qsmfcs & R_ETHSW_P0_QSMFC0_QSMFC_Msk);
        pn_qsmfcs += ETHSW_QCI_SID_OFFSET;
    }

    /* Qci MSDU Passed Packet Count */
    volatile const uint32_t * pn_qmsppcs = &p_switch_reg->P0_QMSPPC0;
    pn_qmsppcs += ((ETHSW_PORT_OFFSET / 4) * port);
    for (uint32_t sid = 0; sid < ETHSW_EFP_SID_COUNT; sid++)
    {
        p_statistics_efp->qci_msdu_passed_pkts[sid] = (*pn_qmsppcs & R_ETHSW_P0_QMSPPC0_QMSPPC_Msk);
        pn_qmsppcs += ETHSW_QCI_SID_OFFSET;
    }

    /* Qci MSDU Reject Packet Count */
    volatile const uint32_t * pn_qmsrpcs = &p_switch_reg->P0_QMSRPC0;
    pn_qmsrpcs += ((ETHSW_PORT_OFFSET / 4) * port);
    for (uint32_t sid = 0; sid < ETHSW_EFP_SID_COUNT; sid++)
    {
        p_statistics_efp->qci_msdu_reject_pkts[sid] = (*pn_qmsrpcs & R_ETHSW_P0_QMSRPC0_QMSRPC_Msk);
        pn_qmsrpcs += ETHSW_QCI_SID_OFFSET;
    }

    /* Qci Gate (All) Passed Packet Count */
    volatile const uint32_t * pn_qgppc = &p_switch_reg->P0_QGPPC;
    pn_qgppc += ((ETHSW_PORT_OFFSET / 4) * port);
    p_statistics_efp->qci_gate_passed_pkts = (*pn_qgppc & R_ETHSW_P0_QGPPC_QGPPC_Msk);

    /* Qci Gate g Dropped Packet Count */
    volatile const uint32_t * pn_qgdpcg = &p_switch_reg->P0_QGDPC0;
    pn_qgdpcg += ((ETHSW_PORT_OFFSET / 4) * port);
    for (uint32_t gaid = 0; gaid < ETHER_SWITCH_TDMA_GPIO_COUNT; gaid++)
    {
        p_statistics_efp->qci_gate_dropped_pkts[gaid] = (*pn_qgdpcg & R_ETHSW_P0_QGDPC0_QGDPC_Msk);
        pn_qgdpcg += ETHSW_QCI_GATE_OFFSET;
    }

    /* Qci Meter Green Packet Count */
    volatile const uint32_t * pn_qmgpcm = &p_switch_reg->P0_QMGPC0;
    pn_qmgpcm += ((ETHSW_PORT_OFFSET / 4) * port);
    for (uint32_t meid = 0; meid < ETHSW_EFP_MEATER_COUNT; meid++)
    {
        p_statistics_efp->qci_mater_green_pkts[meid] = (*pn_qmgpcm & R_ETHSW_P0_QMGPC0_QMGPC_Msk);
        pn_qmgpcm += ETHSW_QCI_MATER_OFFSET;
    }

    /* Qci Meter Red Packet Count */
    volatile const uint32_t * pn_qmrpcm = &p_switch_reg->P0_QMRPC0;
    pn_qmrpcm += ((ETHSW_PORT_OFFSET / 4) * port);
    for (uint32_t meid = 0; meid < ETHSW_EFP_MEATER_COUNT; meid++)
    {
        p_statistics_efp->qci_mater_red_pkts[meid] = (*pn_qmrpcm & R_ETHSW_P0_QMRPC0_QMRPC_Msk);
        pn_qmrpcm += ETHSW_QCI_MATER_OFFSET;
    }

    /* VLAN Ingress Check Drop Frame Counter */
    volatile const uint32_t * pn_vic_drop_cnt = &p_switch_reg->P0_VIC_DROP_CNT;
    pn_vic_drop_cnt               += ((ETHSW_PORT_OFFSET / 4) * port);
    p_statistics_efp->vlan_in_drop = (*pn_vic_drop_cnt & R_ETHSW_P0_VIC_DROP_CNT_VIC_DROP_CNT_Msk);

    /* DST Address Lookup Hit Counter */
    volatile const uint32_t * pn_lookup_hit_cnt = &p_switch_reg->P0_LOOKUP_HIT_CNT;
    pn_lookup_hit_cnt           += ((ETHSW_PORT_OFFSET / 4) * port);
    p_statistics_efp->lookup_hit = (*pn_lookup_hit_cnt & R_ETHSW_P0_LOOKUP_HIT_CNT_LOOKUP_HIT_CNT_Msk);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_StatisticsEfpGet() */

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

/*******************************************************************************************************************//**
 * Gets Global Discard and 802.3br Statistics counters.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_Statistics8023brGet (ether_switch_ctrl_t * const p_ctrl,
                                       uint32_t                    port,
                                       bool                        clear,
                                       ethsw_statistics_8023br_t * p_statistics_8023br)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_statistics_8023br), FSP_ERR_INVALID_POINTER);
#endif

#define ETHSW_8023BR_PORT_OFFSET    (0x048U / 4) // Register offset for Global Discard and 802.3br Statistics Registers in uint32_t

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->STATN_CONFIG_b.CLEAR_ON_READ = (true == clear) ? 1 : 0;

    p_statistics_8023br->out_disc           = *(&p_switch_reg->ODISC0 + port * ETHSW_8023BR_PORT_OFFSET);
    p_statistics_8023br->in_disc_vlan       = *(&p_switch_reg->IDISC_VLAN0 + port * ETHSW_8023BR_PORT_OFFSET);
    p_statistics_8023br->in_disc_untagged   = *(&p_switch_reg->IDISC_UNTAGGED0 + port * ETHSW_8023BR_PORT_OFFSET);
    p_statistics_8023br->in_disc_blocked    = *(&p_switch_reg->IDISC_BLOCKED0 + port * ETHSW_8023BR_PORT_OFFSET);
    p_statistics_8023br->in_disc_any        = *(&p_switch_reg->IDISC_ANY0 + port * ETHSW_8023BR_PORT_OFFSET);
    p_statistics_8023br->in_disc_src_filter = *(&p_switch_reg->IDISC_SRCFLT0 + port * ETHSW_8023BR_PORT_OFFSET);

    if (ETHSW_PORT_COUNT > port)
    {
        p_statistics_8023br->tx_hold_req = *(&p_switch_reg->TX_HOLD_REQ_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->tx_frag     = *(&p_switch_reg->TX_FRAG_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_frag     = *(&p_switch_reg->RX_FRAG_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_assy_ok  = *(&p_switch_reg->RX_ASSY_OK_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);

        p_statistics_8023br->rx_assy_err =
            (uint16_t) *(&p_switch_reg->RX_ASSY_ERR_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_smd_err =
            (uint16_t) *(&p_switch_reg->RX_SMD_ERR_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);

        p_statistics_8023br->tx_verify_ok =
            (uint8_t) *(&p_switch_reg->TX_VERIFY_OK_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->tx_response_ok =
            (uint8_t) *(&p_switch_reg->TX_RESPONSE_OK_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_verify_ok =
            (uint8_t) *(&p_switch_reg->RX_VERIFY_OK_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_response_ok =
            (uint8_t) *(&p_switch_reg->RX_RESPONSE_OK_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_verify_bad =
            (uint8_t) *(&p_switch_reg->RX_VERIFY_BAD_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
        p_statistics_8023br->rx_response_bad =
            (uint8_t) *(&p_switch_reg->RX_RESPONSE_BAD_CNT0 + port * ETHSW_8023BR_PORT_OFFSET);
    }
    else
    {
        p_statistics_8023br->tx_hold_req     = 0;
        p_statistics_8023br->tx_frag         = 0;
        p_statistics_8023br->rx_frag         = 0;
        p_statistics_8023br->rx_assy_ok      = 0;
        p_statistics_8023br->rx_assy_err     = 0;
        p_statistics_8023br->rx_smd_err      = 0;
        p_statistics_8023br->tx_verify_ok    = 0;
        p_statistics_8023br->tx_response_ok  = 0;
        p_statistics_8023br->rx_verify_ok    = 0;
        p_statistics_8023br->rx_response_ok  = 0;
        p_statistics_8023br->rx_verify_bad   = 0;
        p_statistics_8023br->rx_response_bad = 0;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_Statistics8023brGet() */

/*******************************************************************************************************************//**
 * Gets DLR Statistics counters for the specified port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_StatisticsDlrGet (ether_switch_ctrl_t * const p_ctrl, ethsw_statistics_dlr_t * p_statistics_dlr)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_statistics_dlr), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_statistics_dlr->rx_beacon_ok0  = p_switch_reg->DLR_RX_STAT0;
    p_statistics_dlr->rx_beacon_err0 = p_switch_reg->DLR_RX_ERR_STAT0;
    p_statistics_dlr->rx_disc_lf0    = p_switch_reg->DLR_RX_LF_STAT0;
    p_statistics_dlr->rx_beacon_ok1  = p_switch_reg->DLR_RX_STAT1;
    p_statistics_dlr->rx_beacon_err1 = p_switch_reg->DLR_RX_ERR_STAT1;
    p_statistics_dlr->rx_disc_lf1    = p_switch_reg->DLR_RX_LF_STAT1;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_StatisticsDlrGet() */

/*******************************************************************************************************************//**
 * Sets enable CQF.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_CqfEnableSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_cqf_enable_t * p_cqf_enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_cqf_enable), FSP_ERR_INVALID_POINTER);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);                        /* port         : 0 to 3 */
    ETHSW_ERROR_RETURN((ETHSW_QUEUE_COUNT - 1 > p_cqf_enable->cqf_queue), FSP_ERR_INVALID_ARGUMENT);    /* cqf_queue    : 0 to 6 */
    ETHSW_ERROR_RETURN((ETHSW_TDMA_GPIO_COUNT > p_cqf_enable->cqf_gate_sel), FSP_ERR_INVALID_ARGUMENT); /* cqf_gate_sel : 0 to 7 */
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MMCTL_CQF_CTRL_P_b[port].PRIO_ENABLE0 = p_cqf_enable->cqf_enable_priority;
    p_reg_switch->MMCTL_CQF_CTRL_P_b[port].QUEUE_SEL0   =
        (p_cqf_enable->cqf_queue &
         (R_ETHSW_MMCTL_CQF_CTRL_P_QUEUE_SEL0_Msk >> R_ETHSW_MMCTL_CQF_CTRL_P_QUEUE_SEL0_Pos));
    p_reg_switch->MMCTL_CQF_CTRL_P_b[port].GATE_SEL0 =
        (p_cqf_enable->cqf_gate_sel &
         (R_ETHSW_MMCTL_CQF_CTRL_P_GATE_SEL0_Msk >> R_ETHSW_MMCTL_CQF_CTRL_P_GATE_SEL0_Pos));
    p_reg_switch->MMCTL_CQF_CTRL_P_b[port].USE_SOP0 = p_cqf_enable->use_sop;
    p_reg_switch->MMCTL_CQF_CTRL_P_b[port].REF_SEL0 = p_cqf_enable->cqf_gate_ref_sel;

    return err;
}                                      /* End of function R_ETHSW_CqfEnableSet() */

/*******************************************************************************************************************//**
 * Sets a snooping parser configuration
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_SnoopParserSet (ether_switch_ctrl_t * const p_ctrl, ethsw_snoop_parser_config_t * p_parser_cnf)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;
    uint32_t volatile     * p_gparser;
    uint32_t volatile     * p_garith;
    uint32_t                gparser;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_parser_cnf), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((2 > p_parser_cnf->arith_block), FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((4 > p_parser_cnf->parser_id), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (0 == p_parser_cnf->arith_block)
    {
        p_gparser = &p_switch_reg->GPARSER0 + p_parser_cnf->parser_id;
        p_garith  = &p_switch_reg->GARITH3;
    }
    else
    {
        p_gparser = &p_switch_reg->GPARSER4 + p_parser_cnf->parser_id;
        p_garith  = &p_switch_reg->GARITH7;
    }

    /* Disable parser */
    *p_gparser &= (uint32_t) ~R_ETHSW_GPARSER0_VALID_Msk;
    *p_garith  &= ~((1U << p_parser_cnf->parser_id) << R_ETHSW_GARITH0_SEL_MATCH_Pos);

    if (true == p_parser_cnf->enable)
    {
        switch (p_parser_cnf->offs_type)
        {
            case ETHSW_SNOOP_OFFS_ETHER1:
            {
                gparser = R_ETHSW_GPARSER0_OFFSET_DA_Msk;
                break;
            }

            case ETHSW_SNOOP_OFFS_ETHER2:
            {
                gparser = 0;
                break;
            }

            case ETHSW_SNOOP_OFFS_VLAN:
            {
                gparser = R_ETHSW_GPARSER0_SKIPVLAN_Msk;
                break;
            }

            case ETHSW_SNOOP_OFFS_IPDATA:
            {
                gparser = R_ETHSW_GPARSER0_IPDATA_Msk;
                break;
            }

            case ETHSW_SNOOP_OFFS_IPPROT:
            {
                gparser = R_ETHSW_GPARSER0_IPPROTOCOL_Msk;
                break;
            }

            default:
            {
                gparser = 0;
                break;
            }
        }

        switch (p_parser_cnf->comp_type)
        {
            case ETHSW_SNOOP_COMP_8:
            {
                break;
            }

            case ETHSW_SNOOP_COMP_8OR:
            {
                gparser |= R_ETHSW_GPARSER0_CMP_MASK_OR_Msk;
                break;
            }

            case ETHSW_SNOOP_COMP_16:
            {
                gparser |= R_ETHSW_GPARSER0_CMP16_Msk;
                break;
            }

            case ETHSW_SNOOP_COMP_16PLUS:
            {
                gparser |= R_ETHSW_GPARSER0_CMP16_Msk;
                gparser |= R_ETHSW_GPARSER0_OFFSET_PLUS2_Msk;
                break;
            }

            default:
            {
                break;
            }
        }

        if (ETHSW_SNOOP_OFFS_IPPROT != p_parser_cnf->offs_type)
        {
            gparser |=
                (uint32_t) (((uint32_t) p_parser_cnf->offset << R_ETHSW_GPARSER0_OFFSET_Pos) &
                            R_ETHSW_GPARSER0_OFFSET_Msk);
        }

        gparser |= (uint32_t) (((uint32_t) p_parser_cnf->comp_value << R_ETHSW_GPARSER0_COMPARE_VAL_Pos) &
                               R_ETHSW_GPARSER0_COMPARE_VAL_Msk);
        gparser |= (uint32_t) (((uint32_t) p_parser_cnf->mask_value2 << R_ETHSW_GPARSER3_MASK_VAL2_Pos) &
                               R_ETHSW_GPARSER3_MASK_VAL2_Msk);

        gparser |= R_ETHSW_GPARSER0_VALID_Msk;

        *p_gparser = gparser;

        /* Enable parser */
        *p_garith |= (1U << p_parser_cnf->parser_id) << R_ETHSW_GARITH0_SEL_MATCH_Pos;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Sets snooping arithmetic configuration
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_SnoopArithSet (ether_switch_ctrl_t * const p_ctrl, ethsw_snoop_arith_config_t * p_arith_cnf)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;
    uint32_t volatile     * p_garith;
    uint32_t                garith;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_arith_cnf), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((2 > p_arith_cnf->arith_block), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (0 == p_arith_cnf->arith_block)
    {
        p_garith = &p_switch_reg->GARITH3;
    }
    else
    {
        p_garith = &p_switch_reg->GARITH7;
    }

    /* Read Snoop Configuration for Arithmetic Register */
    garith = *p_garith;

    /* Set AND/OR operation*/
    garith &= (uint32_t) ~R_ETHSW_GARITH0_OP_Msk;
    if (ETHSW_SNOOP_OPERAT_OR == p_arith_cnf->operat)
    {
        garith |= R_ETHSW_GARITH0_OP_Msk;
    }

    /* Set Snoop Mode*/
    garith &= (uint32_t) ~R_ETHSW_GARITH0_SNP_MD_Msk;
    garith |= (uint32_t) (((uint32_t) p_arith_cnf->action << R_ETHSW_GARITH0_SNP_MD_Pos) & R_ETHSW_GARITH0_SNP_MD_Msk);

    /* Write Snoop Configuration for Arithmetic Register */
    *p_garith = garith;

    return FSP_SUCCESS;
}                                      /* End of ethsw_snoop_init() */

/*******************************************************************************************************************//**
 * Enables/disables and configure the Energy Efficient Ethernet Mode of the switch.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_EeeSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_eee_t * p_cnf_eee)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_cnf_eee), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Disable EEE */
    (&p_switch_reg->EEE_CTL_STAT_P0_b + (ETHSW_PORT_OFFSET / 4) * port)->EEE_AUTO = 0;

    if (true == p_cnf_eee->enable)
    {
        /* Enable EEE */
        (&p_switch_reg->EEE_CTL_STAT_P0_b + (ETHSW_PORT_OFFSET / 4) * port)->EEE_AUTO = 1;

        *(&p_switch_reg->EEE_IDLE_TIME_P0 + (ETHSW_PORT_OFFSET / 4) * port) = p_cnf_eee->idle_time;

        *(&p_switch_reg->EEE_TWSYS_TIME_P0 + (ETHSW_PORT_OFFSET / 4) * port) = p_cnf_eee->wakeup_time;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_EeeSet() */

/*******************************************************************************************************************//**
 * Sets the timeframe for the broadcast/multicast rate limiting in ms.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_StormTimeSet (ether_switch_ctrl_t * const p_ctrl, uint16_t storm_time)
{
    double val;                        /* value */
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#define ETHSW_ROUND_DECIMAL_PLACES_DOUBLE    ((double) 0.5)

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    val  = (double) storm_time;
    val  = val * (ETHSW_OPERATING_CLOCK_CYCLES / ETHSW_DEC_1000) / ETHSW_DOS_TIMEFACTOR;
    val += ETHSW_ROUND_DECIMAL_PLACES_DOUBLE;

    p_switch_reg->BCAST_STORM_LIMIT_b.TMOUT = (uint16_t) val;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_StormTimeSet() */

/*******************************************************************************************************************//**
 * Sets the max. allowed broadcast frames that can be received in a given time.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_BcastLimitSet (ether_switch_ctrl_t * const p_ctrl, uint16_t storm_frames)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->BCAST_STORM_LIMIT_b.BCASTLIMIT = storm_frames;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_BcastLimitSet() */

/*******************************************************************************************************************//**
 * Sets the max. allowed multicast frames that can be received in a given time.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_McastLimitSet (ether_switch_ctrl_t * const p_ctrl, uint16_t storm_frames)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->MCAST_STORM_LIMIT_b.MCASTLIMIT = storm_frames;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_McastLimitSet() */

/*******************************************************************************************************************//**
 * Sets the max. allowed bandwidth for the given port in percent.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TxRateSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, float rate)
{
    float               idleslope;     /* Idle Slope value */
    volatile uint32_t * p_reg;         /* register */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#define ETHSW_RATE_IS_MAX                  (80U)
#define ETHSW_IDLE_SLOPE_MAX               (2046U)
#define ETHSW_ROUND_DECIMAL_PLACES_FLOT    (0.5F)

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((ETHSW_RATE_IS_MAX >= rate), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* calculate Traffic Shaper bandwidth control register addresse */
    p_reg = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->IDLE_SLOPE_P0 + (ETHSW_PORT_OFFSET * port));

    /* Calc idle slope and set it */
    idleslope  = (ETHSW_DEC_512 * (rate / ETHSW_DEC_100)) / (1 - (rate / ETHSW_DEC_100));
    idleslope += ETHSW_ROUND_DECIMAL_PLACES_FLOT;

    /* check if calculated slope rate exceeds maximum */
    if (ETHSW_IDLE_SLOPE_MAX < idleslope)
    {
        /* limit slope rate to maximum value */
        idleslope = ETHSW_IDLE_SLOPE_MAX;
    }

    /* write slope rate to port specific register (IDLE_SLOPE_P[n]) */
    *p_reg = (uint16_t) idleslope;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TxRateSet() */

/*******************************************************************************************************************//**
 * Sets the priority resolution mode to use for a specific port.
 * It also defines the default priority if no priority resolution produced a result.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_QosModeSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_qos_mode_t * p_qos_mode)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_qos_mode), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->PRIORITY_CFG_b[port].VLANEN  = (true == p_qos_mode->vlan_enable) ? 1 : 0;
    p_switch_reg->PRIORITY_CFG_b[port].IPEN    = (true == p_qos_mode->ip_enable) ? 1 : 0;
    p_switch_reg->PRIORITY_CFG_b[port].MACEN   = (true == p_qos_mode->mac_enable) ? 1 : 0;
    p_switch_reg->PRIORITY_CFG_b[port].TYPE_EN = (true == p_qos_mode->type_enable) ? 1 : 0;

    p_switch_reg->PRIORITY_CFG_b[port].DEFAULTPRI = p_qos_mode->default_pri &
                                                    (R_ETHSW_PRIORITY_CFG_DEFAULTPRI_Msk >>
                                                     R_ETHSW_PRIORITY_CFG_DEFAULTPRI_Pos);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_QosModeSet() */

/*******************************************************************************************************************//**
 * Sets the priority mapping between VLAN tag priorities and the switch core priorities.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_QosPrioValnSet (ether_switch_ctrl_t * const p_ctrl,
                                  uint32_t                    port,
                                  ethsw_qos_prio_vlan_t     * p_qos_prio_vlan)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_qos_prio_vlan), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY0 = p_qos_prio_vlan->priority0;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY1 = p_qos_prio_vlan->priority1;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY2 = p_qos_prio_vlan->priority2;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY3 = p_qos_prio_vlan->priority3;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY4 = p_qos_prio_vlan->priority4;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY5 = p_qos_prio_vlan->priority5;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY6 = p_qos_prio_vlan->priority6;
    p_switch_reg->VLAN_PRIORITY_b[port].PRIORITY7 = p_qos_prio_vlan->priority7;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_QosPrioValnSet() */

/*******************************************************************************************************************//**
 * Sets the priority for the given DiffServ field of an IP packet for the given port.
 * Up to 64 different DiffServ entries may be defined per port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_QosPrioIpSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_qos_prio_ip_t * p_qos_prio_ip)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_qos_prio_ip), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->IP_PRIORITY[port] = (uint32_t) (
        (((uint32_t) p_qos_prio_ip->priority << R_ETHSW_IP_PRIORITY_PRIORITY_Pos) & R_ETHSW_IP_PRIORITY_PRIORITY_Msk) |
        (((uint32_t) p_qos_prio_ip->diffserv << R_ETHSW_IP_PRIORITY_ADDRESS_Pos) &
         R_ETHSW_IP_PRIORITY_ADDRESS_Msk));

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_QosPrioIpSet() */

/*******************************************************************************************************************//**
 * Sets the priority for the given Ethertypes to the given values.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_QosPrioTypeSet (ether_switch_ctrl_t * const p_ctrl, ethsw_qos_prio_type_t * p_qos_prio_ethtype)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_qos_prio_ethtype), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (0 == p_qos_prio_ethtype->type1)
    {
        p_switch_reg->PRIORITY_TYPE1_b.VALID    = 0;
        p_switch_reg->PRIORITY_TYPE1_b.PRIORITY = 0;
        p_switch_reg->PRIORITY_TYPE1_b.TYPEVAL  = 0;
    }
    else
    {
        p_switch_reg->PRIORITY_TYPE1_b.PRIORITY = (uint32_t) p_qos_prio_ethtype->prio1 &
                                                  (R_ETHSW_PRIORITY_TYPE1_PRIORITY_Msk >>
                                                   R_ETHSW_PRIORITY_TYPE1_PRIORITY_Pos);
        p_switch_reg->PRIORITY_TYPE1_b.TYPEVAL = p_qos_prio_ethtype->type1;
        p_switch_reg->PRIORITY_TYPE1_b.VALID   = 1;
    }

    if (0 == p_qos_prio_ethtype->type2)
    {
        p_switch_reg->PRIORITY_TYPE2_b.VALID    = 0;
        p_switch_reg->PRIORITY_TYPE2_b.PRIORITY = 0;
        p_switch_reg->PRIORITY_TYPE2_b.TYPEVAL  = 0;
    }
    else
    {
        p_switch_reg->PRIORITY_TYPE2_b.PRIORITY = (uint32_t) p_qos_prio_ethtype->prio2 &
                                                  (R_ETHSW_PRIORITY_TYPE2_PRIORITY_Msk >>
                                                   R_ETHSW_PRIORITY_TYPE2_PRIORITY_Pos);
        p_switch_reg->PRIORITY_TYPE2_b.TYPEVAL = p_qos_prio_ethtype->type2;
        p_switch_reg->PRIORITY_TYPE2_b.VALID   = 1;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_QosPrioTypeSet() */

/*******************************************************************************************************************//**
 * Sets the dedicated port that will received all frames that are mirrored as well as the type of mirrored frames.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_MirrorSet (ether_switch_ctrl_t * const p_ctrl, ethsw_mirror_conf_t * p_mirror_conf)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;
    ethsw_mac_addr_reg_t    entry = {0};

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_mirror_conf), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(((!(ETHSW_MIRR_MODE_EGRESS_DA == p_mirror_conf->mirror_mode) &&
                         !(ETHSW_MIRR_MODE_EGRESS_SA == p_mirror_conf->mirror_mode) &&
                         !(ETHSW_MIRR_MODE_INGRESS_DA == p_mirror_conf->mirror_mode) &&
                         !(ETHSW_MIRR_MODE_INGRESS_SA == p_mirror_conf->mirror_mode)) ||
                        (NULL != p_mirror_conf->mac_addr)),
                       FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Disable mirroring */
    p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN = 0;

    /* Set mirror port */
    p_switch_reg->MIRROR_CONTROL_b.PORT = p_mirror_conf->mirror_port &
                                          (R_ETHSW_MIRROR_CONTROL_PORT_Msk >> R_ETHSW_MIRROR_CONTROL_PORT_Pos);

    switch (p_mirror_conf->mirror_mode)
    {
        /* Match egress dest. address */
        case ETHSW_MIRR_MODE_EGRESS_DA:
        {
            memcpy(entry.mac_addr, *p_mirror_conf->mac_addr, ETHSW_MAC_ADDR_LENGTH);
            p_switch_reg->MIRROR_EDST_0 = entry.reg_data[0];
            p_switch_reg->MIRROR_EDST_1 = entry.reg_data[1];

            p_switch_reg->MIRROR_CONTROL_b.EG_DA_MATCH = 1;
            p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN   = 1;
            break;
        }

        /* Match egress src. address */
        case ETHSW_MIRR_MODE_EGRESS_SA:
        {
            memcpy(entry.mac_addr, *p_mirror_conf->mac_addr, ETHSW_MAC_ADDR_LENGTH);
            p_switch_reg->MIRROR_ESRC_0 = entry.reg_data[0];
            p_switch_reg->MIRROR_ESRC_1 = entry.reg_data[1];

            p_switch_reg->MIRROR_CONTROL_b.EG_SA_MATCH = 1;
            p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN   = 1;
            break;
        }

        /* Match ingress dest. address */
        case ETHSW_MIRR_MODE_INGRESS_DA:
        {
            memcpy(entry.mac_addr, *p_mirror_conf->mac_addr, ETHSW_MAC_ADDR_LENGTH);
            p_switch_reg->MIRROR_IDST_0 = entry.reg_data[0];
            p_switch_reg->MIRROR_IDST_1 = entry.reg_data[1];

            p_switch_reg->MIRROR_CONTROL_b.ING_DA_MATCH = 1;
            p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN    = 1;
            break;
        }

        /* Match ingress src. address */
        case ETHSW_MIRR_MODE_INGRESS_SA:
        {
            memcpy(entry.mac_addr, *p_mirror_conf->mac_addr, ETHSW_MAC_ADDR_LENGTH);
            p_switch_reg->MIRROR_ISRC_0 = entry.reg_data[0];
            p_switch_reg->MIRROR_ISRC_1 = entry.reg_data[1];

            p_switch_reg->MIRROR_CONTROL_b.ING_SA_MATCH = 1;
            p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN    = 1;
            break;
        }

        /* Match egress port */
        case ETHSW_MIRR_MODE_EGRESS_PORT:
        {
            /* remove portmask bit and translate host port if set */
            p_switch_reg->MIRROR_EG_MAP_b.EMAP = p_mirror_conf->port_map.mask &
                                                 (R_ETHSW_MIRROR_EG_MAP_EMAP_Msk >> R_ETHSW_MIRROR_EG_MAP_EMAP_Pos);

            p_switch_reg->MIRROR_CONTROL_b.EG_MAP_EN = 1;
            p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN = 1;
            break;
        }

        /* Match ingress port */
        case ETHSW_MIRR_MODE_INGRESS_PORT:
        {
            /* remove portmask bit and translate host port if set */
            p_switch_reg->MIRROR_ING_MAP_b.IMAP = p_mirror_conf->port_map.mask &
                                                  (R_ETHSW_MIRROR_ING_MAP_IMAP_Msk >> R_ETHSW_MIRROR_ING_MAP_IMAP_Pos);

            p_switch_reg->MIRROR_CONTROL_b.ING_MAP_EN = 1;
            p_switch_reg->MIRROR_CONTROL_b.MIRROR_EN  = 1;
            break;
        }

        /* Disable */
        case ETHSW_MIRR_MODE_DISABLE:
        default:
        {
            p_switch_reg->MIRROR_CONTROL = 0;
            break;
        }
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_MirrorSet() */

/********************************************************************************************************************//**
 * Sets enable cut-through for the specified port.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_CtEnableSet (ether_switch_ctrl_t * const p_ctrl, ethsw_port_mask_t port_mask)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->MODE_CONFIG_b.CUT_THRU_EN = port_mask.mask &
                                              (R_ETHSW_MODE_CONFIG_CUT_THRU_EN_Msk >>
                                               R_ETHSW_MODE_CONFIG_CUT_THRU_EN_Pos);

    return err;
}                                      /* End of function R_ETHSW_CtEnableSet() */

/********************************************************************************************************************//**
 * Sets the cut-through delay for the specified port,
 * delay Value in 400 ns / 40 ns / 8 ns increments (fequency of the MII PHY interface).
 * This is a programmable value in MII clock cycles that can delay the assertion of the cut-through flag
 * to the forwarding engine in order to hide the variable latency of the forwarding decision.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_CtDelaySet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint32_t ct_delay)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint32_t volatile     * p_ct_delay_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_ct_delay_reg = (uint32_t volatile *) ((uint8_t *) &p_reg_switch->CT_DELAY_P0 + (ETHSW_PORT_OFFSET * port));

    *p_ct_delay_reg = ct_delay & (R_ETHSW_CT_DELAY_P0_CT_DELAY_Msk >> R_ETHSW_CT_DELAY_P0_CT_DELAY_Pos);

    return err;
}                                      /* End of function R_ETHSW_CtDelaySet() */

/********************************************************************************************************************//**
 * Initilize pulse generator.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PulseGeneratorInit (ether_switch_ctrl_t * const p_ctrl, uint32_t time_num)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSS_Type volatile * p_reg_ethss;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_TIME_COUNT > time_num, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_ethss = p_instance_ctrl->p_reg_ethss;

    r_ethsw_reg_protection_disable(p_instance_ctrl->p_reg_ethss);

    p_reg_ethss->PTPMCTRL_b.PTP_PLS_RSTn = 0;

#if defined(BSP_MCU_GROUP_RZN2H)
    p_reg_ethss->PTPMCTRL_b.PTP_MODE0 = time_num & (R_ETHSS_PTPMCTRL_PTP_MODE0_Msk >> R_ETHSS_PTPMCTRL_PTP_MODE0_Pos);
#else
    p_reg_ethss->PTPMCTRL_b.PTP_MODE = time_num & (R_ETHSS_PTPMCTRL_PTP_MODE_Msk >> R_ETHSS_PTPMCTRL_PTP_MODE_Pos);
#endif

    p_reg_ethss->PTPMCTRL_b.PTP_PLS_RSTn = 1;

    r_ethsw_reg_protection_enable(p_instance_ctrl->p_reg_ethss);

    return err;
}                                      /* End of function R_ETHSW_PulseGeneratorInit() */

/********************************************************************************************************************//**
 * Sets pulse generator.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_PulseGeneratorSet (ether_switch_ctrl_t * const p_ctrl, ethsw_ts_pulse_generator_t * p_pulse)
{
    ethsw_instance_ctrl_t     * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t                   err             = FSP_SUCCESS;
    R_ETHSW_PTP_Type volatile * p_reg_ethsw_ptp;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_pulse), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(ETHSW_PULSE_GENERATOR_COUNT > p_pulse->pulse_num, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_ethsw_ptp = p_instance_ctrl->p_reg_ethsw_ptp;

    r_ethsw_reg_protection_disable(p_instance_ctrl->p_reg_ethss);

    /* PTP Timer Pulse Output Disable */
    p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].EN_b.OUTEN = 0;

    if (true == p_pulse->enable)       // enable
    {
        /* Set the Pulse Width of ETHSW_PTPOUTn in the cycle number */
        p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].WTH_b.WIDTH = p_pulse->wide;

        /* PTP Timer Pulse Period */
        p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].PSEC = p_pulse->period_sec;
        p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].PNS  = p_pulse->period_ns;

        /* PTP Timer Pulse Start */
        p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].STSEC = p_pulse->start_sec;
        p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].STNS  = p_pulse->start_ns;

        /* PTP Timer Pulse Output Enable */
        p_reg_ethsw_ptp->SWTM[p_pulse->pulse_num].EN_b.OUTEN = p_pulse->enable;
    }

    r_ethsw_reg_protection_enable(p_instance_ctrl->p_reg_ethss);

    return err;
}                                      /* End of function R_ETHSW_PulseGeneratorSet() */

/*******************************************************************************************************************//**
 * Sets authorization state for given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_PortAuthSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool auth_state)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* set authentication */
    if (false == auth_state)
    {
        p_switch_reg->AUTH_PORT_b[port].AUTH = 0;
    }
    else
    {
        p_switch_reg->AUTH_PORT_b[port].AUTH = 1;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_PortAuhSet() */

/*******************************************************************************************************************//**
 * Sets controlled direction for given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_PortCtrlDirSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool dir_state)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* check given direction value */
    if (false == dir_state)
    {
        /* set port to controll input and output */
        p_switch_reg->AUTH_PORT_b[port].CTRL_BOTH = 0;
    }
    else
    {
        /* set port to control only input */
        p_switch_reg->AUTH_PORT_b[port].CTRL_BOTH = 1;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_PortCtrlDirSet() */

/*******************************************************************************************************************//**
 * Sets EAPOL reception mode for given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_PortEapolSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool eapol_state)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN(ETHSW_PORT_COUNT > port, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* set EAPOL reception mode */
    if (false == eapol_state)
    {
        /* enable reception of EAPOL frames */
        p_switch_reg->AUTH_PORT_b[port].EAPOL_EN = 0;
    }
    else
    {
        /* disable reception of EAPOL frames */
        p_switch_reg->AUTH_PORT_b[port].EAPOL_EN = 1;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_PortEapolSet() */

/*******************************************************************************************************************//**
 * Sest Bridge Protocol Frame (BPDU) forwarding mode.
 * Enabling the BPDU forwarding mode this function will also disable the unchecked dropping of BPDU frames.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_BpduSet (ether_switch_ctrl_t * const p_ctrl, bool bpdu_state)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* set BPDU forwarding mode */
    if (false == bpdu_state)
    {
        /* Disable BPDU forwarding */
        p_switch_reg->MGMT_CONFIG_b.ENABLE = 0;

        /* Enable discarding BPDU frames */
        p_switch_reg->MGMT_CONFIG_b.MGMT_DISC = 1;
    }
    else
    {
        /* Enable BPDU forwarding */
        p_switch_reg->MGMT_CONFIG_b.ENABLE = 1;

        /* Disable discarding BPDU frames */
        p_switch_reg->MGMT_CONFIG_b.DISCARD   = 0;
        p_switch_reg->MGMT_CONFIG_b.MGMT_DISC = 0;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_BpduSet() */

/*******************************************************************************************************************//**
 * Sets the default VLAN for input/output processing for the specified port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanDefaultSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint16_t vlan_id)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_switch_reg->SYSTEM_TAGINFO_b[port].SYSVLANINFO = vlan_id &
                                                       (R_ETHSW_SYSTEM_TAGINFO_SYSVLANINFO_Msk >>
                                                        R_ETHSW_SYSTEM_TAGINFO_SYSVLANINFO_Pos);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_VlanDefaultSet() */

/*******************************************************************************************************************//**
 * Adds a VLAN for a given port to the VLAN domain table.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanPortAdd (ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint16_t vlan_id)
{
    fsp_err_t err = FSP_ERR_INVALID_ARGUMENT;
    uint32_t  index;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* First run: check whether the vlan id entry is already in the table */
    for (index = 0;
         index < (sizeof(p_switch_reg->VLAN_RES_TABLE) / sizeof(p_switch_reg->VLAN_RES_TABLE[0]));
         index++)
    {
        if (vlan_id == p_switch_reg->VLAN_RES_TABLE_b[index].VLANID)
        {
            uint32_t port_mask = p_switch_reg->VLAN_RES_TABLE_b[index].PORTMASK | (1U << port);

            p_switch_reg->VLAN_RES_TABLE_b[index].PORTMASK = port_mask &
                                                             (R_ETHSW_VLAN_RES_TABLE_PORTMASK_Msk >>
                                                              R_ETHSW_VLAN_RES_TABLE_PORTMASK_Pos);

            err = FSP_SUCCESS;
            break;
        }
    }

    if (FSP_SUCCESS != err)
    {
        /* Second run: find free entry and add VLAN id and port */
        for (index = 0;
             index < (sizeof(p_switch_reg->VLAN_RES_TABLE) / sizeof(p_switch_reg->VLAN_RES_TABLE[0]));
             index++)
        {
            /* Default VLANid is all bit set */
            if ((R_ETHSW_VLAN_RES_TABLE_VLANID_Msk >> R_ETHSW_VLAN_RES_TABLE_VLANID_Pos) ==
                p_switch_reg->VLAN_RES_TABLE_b[index].VLANID)
            {
                p_switch_reg->VLAN_RES_TABLE_b[index].VLANID = vlan_id &
                                                               (R_ETHSW_VLAN_RES_TABLE_VLANID_Msk >>
                                                                R_ETHSW_VLAN_RES_TABLE_VLANID_Pos);
                p_switch_reg->VLAN_RES_TABLE_b[index].PORTMASK = (1U << port) &
                                                                 (R_ETHSW_VLAN_RES_TABLE_PORTMASK_Msk >>
                                                                  R_ETHSW_VLAN_RES_TABLE_PORTMASK_Pos);

                err = FSP_SUCCESS;
                break;
            }
        }
    }

    /* If not FSP_SUCCESS, not VLAN not in table and no free entries left */
    return err;
}                                      /* End of function R_ETHSW_VlanPortAdd() */

/*******************************************************************************************************************//**
 * Removes a VLAN for a given port to the VLAN domain table.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanPortRemove (ether_switch_ctrl_t * const p_ctrl, uint32_t port, uint16_t vlan_id)
{
    fsp_err_t err = FSP_ERR_INVALID_ARGUMENT;
    uint32_t  index;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* First run: check whether the vlan id entry is already in the table */
    for (index = 0;
         index < (sizeof(p_switch_reg->VLAN_RES_TABLE) / sizeof(p_switch_reg->VLAN_RES_TABLE[0]));
         index++)
    {
        if (vlan_id == p_switch_reg->VLAN_RES_TABLE_b[index].VLANID)
        {
            /* Remove port from port mask */
            p_switch_reg->VLAN_RES_TABLE_b[index].PORTMASK &= (~(1U << port)) &
                                                              (R_ETHSW_VLAN_RES_TABLE_PORTMASK_Msk >>
                                                               R_ETHSW_VLAN_RES_TABLE_PORTMASK_Pos);

            /* Reset to default if no more ports are mapped
             * so the entry is free again for new VLAN ids
             */
            if (0 == p_switch_reg->VLAN_RES_TABLE_b[index].PORTMASK)
            {
                p_switch_reg->VLAN_RES_TABLE_b[index].PORTMASK = R_ETHSW_VLAN_RES_TABLE_PORTMASK_Msk >>
                                                                 R_ETHSW_VLAN_RES_TABLE_PORTMASK_Pos;
                p_switch_reg->VLAN_RES_TABLE_b[index].VLANID = R_ETHSW_VLAN_RES_TABLE_VLANID_Msk >>
                                                               R_ETHSW_VLAN_RES_TABLE_VLANID_Pos;
            }

            err = FSP_SUCCESS;
            break;
        }
    }

    /* If not FSP_SUCCESS, The VLAN id was not in the table so nothing to remove */
    return err;
}                                      /* End of function R_ETHSW_VlanPortRemove() */

/*******************************************************************************************************************//**
 * Sets the mode of VLAN input operations for the given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanInModeSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, ethsw_vlan_in_mode_t vlan_in_mode)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN(((ETHSW_VLANIN_PASSTHROUGH_OVERRIDE == vlan_in_mode) ||
                        (ETHSW_VLANIN_REPLACE == vlan_in_mode) ||
                        (ETHSW_VLANIN_TAG_ALWAYS == vlan_in_mode) ||
                        (ETHSW_VLANIN_DISABLE == vlan_in_mode)),
                       FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Disable VLAN input processing ? */
    if (ETHSW_VLANIN_DISABLE == vlan_in_mode)
    {
        /* Disable input processing and reset mode */
        p_switch_reg->VLAN_IN_MODE_ENA_b.VLANINMDEN &= ~(1U << port) &
                                                       (R_ETHSW_VLAN_IN_MODE_ENA_VLANINMDEN_Msk >>
                                                        R_ETHSW_VLAN_IN_MODE_ENA_VLANINMDEN_Pos);

        switch (port)
        {
            case 0:
            {
                p_switch_reg->VLAN_IN_MODE_b.P0VLANINMD = 0;
                break;
            }

            case 1:
            {
                p_switch_reg->VLAN_IN_MODE_b.P1VLANINMD = 0;
                break;
            }

            case 2:
            {
                p_switch_reg->VLAN_IN_MODE_b.P2VLANINMD = 0;
                break;
            }

            case 3:
            {
                p_switch_reg->VLAN_IN_MODE_b.P3VLANINMD = 0;
                break;
            }

            default:
            {
                break;
            }
        }
    }
    else
    {
        switch (port)
        {
            case 0:
            {
                p_switch_reg->VLAN_IN_MODE_b.P0VLANINMD = vlan_in_mode &
                                                          (R_ETHSW_VLAN_IN_MODE_P0VLANINMD_Msk >>
                                                           R_ETHSW_VLAN_IN_MODE_P0VLANINMD_Pos);
                break;
            }

            case 1:
            {
                p_switch_reg->VLAN_IN_MODE_b.P1VLANINMD = vlan_in_mode &
                                                          (R_ETHSW_VLAN_IN_MODE_P1VLANINMD_Msk >>
                                                           R_ETHSW_VLAN_IN_MODE_P1VLANINMD_Pos);
                break;
            }

            case 2:
            {
                p_switch_reg->VLAN_IN_MODE_b.P2VLANINMD = vlan_in_mode &
                                                          (R_ETHSW_VLAN_IN_MODE_P2VLANINMD_Msk >>
                                                           R_ETHSW_VLAN_IN_MODE_P2VLANINMD_Pos);
                break;
            }

            case 3:
            {
                p_switch_reg->VLAN_IN_MODE_b.P3VLANINMD = vlan_in_mode &
                                                          (R_ETHSW_VLAN_IN_MODE_P3VLANINMD_Msk >>
                                                           R_ETHSW_VLAN_IN_MODE_P3VLANINMD_Pos);
                break;
            }

            default:
            {
                break;
            }
        }

        p_switch_reg->VLAN_IN_MODE_ENA_b.VLANINMDEN = (p_switch_reg->VLAN_IN_MODE_ENA_b.VLANINMDEN | (1U << port)) &
                                                      (R_ETHSW_VLAN_IN_MODE_ENA_VLANINMDEN_Msk >>
                                                       R_ETHSW_VLAN_IN_MODE_ENA_VLANINMDEN_Pos);
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_VlanInModeSet() */

/*******************************************************************************************************************//**
 * Sets the mode of VLAN output operations for the given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanOutModeSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port,
                                  ethsw_vlan_out_mode_t vlan_out_mode)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN(((ETHSW_VLANOUT_DISABLE == vlan_out_mode) ||
                        (ETHSW_VLANOUT_STRIP == vlan_out_mode) ||
                        (ETHSW_VLANOUT_TAGTHROUGH == vlan_out_mode) ||
                        (ETHSW_VLANOUT_DOMAINTRANSP == vlan_out_mode)),
                       FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    switch (port)
    {
        case 0:
        {
            p_switch_reg->VLAN_OUT_MODE_b.P0VLANOUTMD = vlan_out_mode &
                                                        (R_ETHSW_VLAN_OUT_MODE_P0VLANOUTMD_Msk >>
                                                         R_ETHSW_VLAN_OUT_MODE_P0VLANOUTMD_Pos);
            break;
        }

        case 1:
        {
            p_switch_reg->VLAN_OUT_MODE_b.P1VLANOUTMD = vlan_out_mode &
                                                        (R_ETHSW_VLAN_OUT_MODE_P1VLANOUTMD_Msk >>
                                                         R_ETHSW_VLAN_OUT_MODE_P1VLANOUTMD_Pos);
            break;
        }

        case 2:
        {
            p_switch_reg->VLAN_OUT_MODE_b.P2VLANOUTMD = vlan_out_mode &
                                                        (R_ETHSW_VLAN_OUT_MODE_P2VLANOUTMD_Msk >>
                                                         R_ETHSW_VLAN_OUT_MODE_P2VLANOUTMD_Pos);
            break;
        }

        case 3:
        {
            p_switch_reg->VLAN_OUT_MODE_b.P3VLANOUTMD = vlan_out_mode &
                                                        (R_ETHSW_VLAN_OUT_MODE_P3VLANOUTMD_Msk >>
                                                         R_ETHSW_VLAN_OUT_MODE_P3VLANOUTMD_Pos);
            break;
        }

        default:
        {
            break;
        }
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_VlanOutModeSet() */

/*******************************************************************************************************************//**
 * Set VLAN verification for the given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanVerifySet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (true == enable)
    {
        p_switch_reg->VLAN_VERIFY_b.VLANVERI = (p_switch_reg->VLAN_VERIFY_b.VLANVERI | (1U << port)) &
                                               (R_ETHSW_VLAN_VERIFY_VLANVERI_Msk >> R_ETHSW_VLAN_VERIFY_VLANVERI_Pos);
    }
    else
    {
        p_switch_reg->VLAN_VERIFY_b.VLANVERI &= ~(1U << port) &
                                                (R_ETHSW_VLAN_VERIFY_VLANVERI_Msk >> R_ETHSW_VLAN_VERIFY_VLANVERI_Pos);
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_VlanVerifySet() */

/*******************************************************************************************************************//**
 * Set VLAN discartion of unknown frames for the given port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_VlanDiscardUnknownSet (ether_switch_ctrl_t * const p_ctrl, uint32_t port, bool enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT_ALL > port), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (true == enable)
    {
        p_switch_reg->VLAN_VERIFY_b.VLANDISC = (p_switch_reg->VLAN_VERIFY_b.VLANDISC | (1U << port)) &
                                               (R_ETHSW_VLAN_VERIFY_VLANDISC_Msk >> R_ETHSW_VLAN_VERIFY_VLANDISC_Pos);
    }
    else
    {
        p_switch_reg->VLAN_VERIFY_b.VLANDISC &= ~(1U << port) &
                                                (R_ETHSW_VLAN_VERIFY_VLANDISC_Msk >> R_ETHSW_VLAN_VERIFY_VLANDISC_Pos);
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_VlanDiscardUnknownSet() */

/*******************************************************************************************************************//**
 * Enable or disable the TDMA scheduler.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaEnableSet (ether_switch_ctrl_t * const p_ctrl, ethsw_tdma_enable_t * p_tdma_enable)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_tdma_enable), FSP_ERR_INVALID_POINTER);

    ETHSW_ERROR_RETURN((ETHSW_TIME_COUNT > p_tdma_enable->time_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* disable operation */
    p_switch_reg->TDMA_CONFIG_b.TDMA_ENA        = 0;
    p_switch_reg->TDMA_IRQ_CONTROL_b.TCV_INT_EN = 0;
    p_switch_reg->INT_CONFIG_b.TDMA_INT         = 0;

    p_switch_reg->TDMA_IRQ_STAT_ACK = R_ETHSW_TDMA_IRQ_STAT_ACK_TCV_ACK_Msk | R_ETHSW_TDMA_IRQ_STAT_ACK_CTR1_ACK_Msk;

    if (true == p_tdma_enable->enable)
    {
        /* enable operation */

        gp_ethsw_tdma_callback = p_tdma_enable->p_tdma_callback;

        p_switch_reg->TDMA_START  = p_tdma_enable->tdma_start;
        p_switch_reg->TDMA_MODULO = p_tdma_enable->tdma_modulo;
        p_switch_reg->TDMA_CYCLE  = p_tdma_enable->tdma_cycle;

        p_switch_reg->TDMA_ENA_CTRL_b.PORT_ENA = p_tdma_enable->port_mask.mask &
                                                 (R_ETHSW_TDMA_ENA_CTRL_PORT_ENA_Msk >>
                                                  R_ETHSW_TDMA_ENA_CTRL_PORT_ENA_Pos);

        p_switch_reg->TDMA_CONFIG_b.TIMER_SEL = p_tdma_enable->time_num &
                                                (R_ETHSW_TDMA_CONFIG_TIMER_SEL_Msk >>
                                                 R_ETHSW_TDMA_CONFIG_TIMER_SEL_Pos);

        p_switch_reg->TDMA_IRQ_CONTROL_b.TCV_INT_EN = 1;
        p_switch_reg->INT_CONFIG_b.TDMA_INT         = 1;
        p_switch_reg->INT_CONFIG_b.IRQ_EN           = 1;
        p_switch_reg->TDMA_CONFIG_b.TDMA_ENA        = 1;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TdmaEnableSet() */

/*******************************************************************************************************************//**
 * Sets the TDMA scheduler entry.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaScheduleSet (ether_switch_ctrl_t * const   p_ctrl,
                                   ethsw_tdma_schedule_entry_t * p_tdma_schedule_entry,
                                   uint16_t                      tdma_schedule_entry_count)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

    uint32_t index;
    uint32_t tcv_seq_ctrl;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_tdma_schedule_entry), FSP_ERR_INVALID_POINTER);

    ETHSW_ERROR_RETURN(((0 != tdma_schedule_entry_count) &&
                        ((R_ETHSW_TCV_D_ADDR_ADDR_Msk >> R_ETHSW_TCV_D_ADDR_ADDR_Pos) >=
                         tdma_schedule_entry_count) &&
                        ((R_ETHSW_TCV_SEQ_ADDR_TCV_S_ADDR_Msk >> R_ETHSW_TCV_SEQ_ADDR_TCV_S_ADDR_Pos) >=
                         tdma_schedule_entry_count)),
                       FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    for (index = 0; index < tdma_schedule_entry_count; index++)
    {
        /* sets TCV sequence */
        tcv_seq_ctrl = 0;

        if (0 == index)
        {
            tcv_seq_ctrl |= R_ETHSW_TCV_SEQ_CTRL_START_Msk;
        }

        if (true == p_tdma_schedule_entry[index].interrupt)
        {
            tcv_seq_ctrl |= R_ETHSW_TCV_SEQ_CTRL_INT_Msk;
        }

        tcv_seq_ctrl |= (uint32_t) ((index << R_ETHSW_TCV_SEQ_CTRL_TCV_D_IDX_Pos) & R_ETHSW_TCV_SEQ_CTRL_TCV_D_IDX_Msk);
        tcv_seq_ctrl |=
            (uint32_t) (((uint32_t) p_tdma_schedule_entry[index].gpio_mask << R_ETHSW_TCV_SEQ_CTRL_GPIO_Pos) &
                        R_ETHSW_TCV_SEQ_CTRL_GPIO_Msk);

        p_switch_reg->TCV_SEQ_ADDR = (index << R_ETHSW_TCV_SEQ_ADDR_TCV_S_ADDR_Pos) &
                                     R_ETHSW_TCV_SEQ_ADDR_TCV_S_ADDR_Msk;
        p_switch_reg->TCV_SEQ_CTRL = tcv_seq_ctrl;

        /* sets TCV data */
        p_switch_reg->TCV_D_ADDR   = (index << R_ETHSW_TCV_D_ADDR_ADDR_Pos) & R_ETHSW_TCV_D_ADDR_ADDR_Msk;
        p_switch_reg->TCV_D_OFFSET = p_tdma_schedule_entry[index].time_offset;

        p_switch_reg->TCV_D_CTRL_b.QGATE = p_tdma_schedule_entry[index].gate_state &
                                           (R_ETHSW_TCV_D_CTRL_QGATE_Msk >> R_ETHSW_TCV_D_CTRL_QGATE_Pos);

        p_switch_reg->TCV_D_CTRL_b.PMASK = p_tdma_schedule_entry[index].port_mask.mask &
                                           (R_ETHSW_TCV_D_CTRL_PMASK_Msk >> R_ETHSW_TCV_D_CTRL_PMASK_Pos);
        p_switch_reg->TCV_D_CTRL_b.HOLD_REQ     = p_tdma_schedule_entry[index].hold_req;
        p_switch_reg->TCV_D_CTRL_b.GATE_MODE    = p_tdma_schedule_entry[index].gate_mode;
        p_switch_reg->TCV_D_CTRL_b.TRIGGER_MODE = p_tdma_schedule_entry[index].trigger_mode;
        p_switch_reg->TCV_D_CTRL_b.IN_CT_ENA    = p_tdma_schedule_entry[index].in_ct_ena;
        p_switch_reg->TCV_D_CTRL_b.OUT_CT_ENA   = p_tdma_schedule_entry[index].out_ct_ena;
        p_switch_reg->TCV_D_CTRL_b.RED_PERIOD   = p_tdma_schedule_entry[index].red_period;
        p_switch_reg->TCV_D_CTRL_b.INC_CTR1     = p_tdma_schedule_entry[index].inc_crt1;
        p_switch_reg->TCV_D_CTRL_b.INC_CTR0     = p_tdma_schedule_entry[index].inc_crt0;
    }

    p_switch_reg->TDMA_TCV_START      = 0;
    p_switch_reg->TCV_SEQ_LAST_b.LAST = (uint32_t) (tdma_schedule_entry_count - 1) &
                                        (R_ETHSW_TCV_SEQ_LAST_LAST_Msk >> R_ETHSW_TCV_SEQ_LAST_LAST_Pos);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TdmaScheduleSet() */

/*******************************************************************************************************************//**
 * Sets the operation mode for the specified TDMA_GPIO number.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaGpioModeSet (ether_switch_ctrl_t * const p_ctrl,
                                   uint8_t                     gpio_num,
                                   ethsw_tdma_gpio_mode_t      gpio_mode)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;
    uint32_t                tdma_gpio_mode;

#define ETHSW_TDMA_GPIO_MODE_MASK     (0x3U)
#define ETHSW_TDMA_GPIO_MODE_SHIFT    (2U)

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_GPIO_COUNT > gpio_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    tdma_gpio_mode  = p_switch_reg->TDMA_GPIO_b.GPIO_MODE;
    tdma_gpio_mode &= ~(ETHSW_TDMA_GPIO_MODE_MASK << (gpio_num * ETHSW_TDMA_GPIO_MODE_SHIFT));
    tdma_gpio_mode |= (uint32_t) gpio_mode << (gpio_num * ETHSW_TDMA_GPIO_MODE_SHIFT);

    p_switch_reg->TDMA_GPIO_b.GPIO_MODE = tdma_gpio_mode &
                                          (R_ETHSW_TDMA_GPIO_GPIO_MODE_Msk >> R_ETHSW_TDMA_GPIO_GPIO_MODE_Pos);

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TdmaGpioModeSet() */

/********************************************************************************************************************//**
 * Sets TDMA counter 0
 * The Counter 0 is 32-bit counter that is incremented when the TCV field INC_CTR0 is set to 1.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaCounter0Set (ether_switch_ctrl_t * const p_ctrl, uint32_t tdma_counter0)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->TDMA_CTR0 = tdma_counter0;

    return err;
}                                      /* End of function R_ETHSW_TdmaCounter0Set() */

/********************************************************************************************************************//**
 * Gets TDMA counter 0
 * The Counter 0 is 32-bit counter that is incremented when the TCV field INC_CTR0 is set to 1.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaCounter0Get (ether_switch_ctrl_t * const p_ctrl, uint32_t * p_tdma_counter0)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_tdma_counter0), FSP_ERR_INVALID_POINTER);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    *p_tdma_counter0 = p_reg_switch->TDMA_CTR0;

    return err;
}                                      /* End of function R_ETHSW_TdmaCounter0Get() */

/********************************************************************************************************************//**
 * Sets TDMA counter 1
 * The counter 1 is an 8-bit counter that is incremented when the TCV field INC_CTR1 is set to 1.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaCounter1Set (ether_switch_ctrl_t * const p_ctrl, ethsw_tdma_counter1_t * p_tdma_counter1)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;
    uint32_t                counter1;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_tdma_counter1), FSP_ERR_INVALID_POINTER);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    counter1 = 0;

    if (true == p_tdma_counter1->write_value)
    {
        counter1  = (uint32_t) (p_tdma_counter1->value << R_ETHSW_TDMA_CTR1_VALUE_Pos) & R_ETHSW_TDMA_CTR1_VALUE_Msk;
        counter1 |= (uint32_t) ((uint32_t) (p_tdma_counter1->write_value << R_ETHSW_TDMA_CTR1_WRITE_ENA_Pos) &
                                R_ETHSW_TDMA_CTR1_WRITE_ENA_Msk);
    }

    counter1 |=
        (uint32_t) ((uint32_t) (p_tdma_counter1->max_value << R_ETHSW_TDMA_CTR1_MAX_Pos) & R_ETHSW_TDMA_CTR1_MAX_Msk);
    counter1 |= (uint32_t) ((uint32_t) (p_tdma_counter1->int_value << R_ETHSW_TDMA_CTR1_INT_VALUE_Pos) &
                            R_ETHSW_TDMA_CTR1_INT_VALUE_Msk);

    p_reg_switch->TDMA_CTR1 = counter1;

    p_reg_switch->TDMA_IRQ_CONTROL_b.CTR1_INT_EN = p_tdma_counter1->int_enable;

    return err;
}                                      /* End of function R_ETHSW_TdmaCounter1Set() */

/********************************************************************************************************************//**
 * Gets TDMA counter 1
 * The counter 1 is an 8-bit counter that is incremented when the TCV field INC_CTR1 is set to 1.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaCounter1Get (ether_switch_ctrl_t * const p_ctrl, ethsw_tdma_counter1_t * p_tdma_counter1)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_tdma_counter1), FSP_ERR_INVALID_POINTER);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_tdma_counter1->value      = p_reg_switch->TDMA_CTR1_b.VALUE;
    p_tdma_counter1->max_value  = p_reg_switch->TDMA_CTR1_b.MAX;
    p_tdma_counter1->int_value  = p_reg_switch->TDMA_CTR1_b.INT_VALUE;
    p_tdma_counter1->int_enable = (bool) p_reg_switch->TDMA_IRQ_CONTROL_b.CTR1_INT_EN;

    return err;
}                                      /* End of function R_ETHSW_TdmaCounter1Get() */

/*******************************************************************************************************************//**
 * Clear the TDMA hold request
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TdmaHoldReqClear (ether_switch_ctrl_t * const p_ctrl)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    p_reg_switch->TDMA_CONFIG_b.HOLD_REQ_CLR = 1;

    return err;
}                                      /* End of function R_ETHSW_TdmaHoldReqClear() */

/*******************************************************************************************************************//**
 * Enables or disables the timer with the specified timer number.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeEnableSet (ether_switch_ctrl_t * const p_ctrl, ethsw_time_enable_t * p_time_enable)
{
    volatile uint32_t * p_atime_ctrl;  /* time control for time */
    volatile uint32_t * p_atime_inc;   /* Clock period of the timestamping clock */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_time_enable), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((ETHSW_TIME_COUNT > p_time_enable->time_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Pointer ATIME_CTRL[n] register */
    p_atime_ctrl = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME_CTRL0 +
                                          (ETHSW_PORT_OFFSET_32BYTES * p_time_enable->time_num));

    /* Pointer ATIME_INC[n] register */
    p_atime_inc = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME_INC0 +
                                         (ETHSW_PORT_OFFSET_32BYTES * p_time_enable->time_num));

    if (true == p_time_enable->enable)
    {
        /* enable operation */
        *p_atime_ctrl |= R_ETHSW_ATIME_CTRL1_ENABLE_Msk << R_ETHSW_ATIME_CTRL1_ENABLE_Pos;

        *p_atime_inc &= (uint32_t) (~R_ETHSW_ATIME_INC1_CLK_PERIOD_Msk);
        *p_atime_inc |= (uint32_t) ((uint32_t) (p_time_enable->clock_period << R_ETHSW_ATIME_INC1_CLK_PERIOD_Pos) &
                                    R_ETHSW_ATIME_INC1_CLK_PERIOD_Msk);
    }
    else
    {
        /* disable operation */
        *p_atime_ctrl &= (uint32_t) ~(R_ETHSW_ATIME_CTRL1_ENABLE_Msk << R_ETHSW_ATIME_CTRL1_ENABLE_Pos);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Enable Tx timestamping for an external port.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeTransmitTimestampSet (ether_switch_ctrl_t * const p_ctrl, ethsw_time_transmit_t * p_time_transmit)
{
    uint32_t reg_config;               /* CONFIG */
    uint32_t reg_int_config;           /* INT_CONFIG */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_time_transmit), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    reg_config     = p_switch_reg->TSM_CONFIG; /* CONFIG register */
    reg_int_config = p_switch_reg->INT_CONFIG; /* INT_CONFIG register */

    if (0 != p_time_transmit->port_mask.mask)
    {
        /* enable options */
        reg_config &= (uint32_t) ~R_ETHSW_TSM_CONFIG_IRQ_TX_EN_Msk;
        reg_config |= (uint32_t) ((uint32_t) (p_time_transmit->port_mask.mask << R_ETHSW_TSM_CONFIG_IRQ_TX_EN_Pos) &
                                  R_ETHSW_TSM_CONFIG_IRQ_TX_EN_Msk);
        reg_config |= R_ETHSW_TSM_CONFIG_IRQ_EN_Msk;

        p_switch_reg->TSM_CONFIG = reg_config;

        reg_int_config |= R_ETHSW_INT_CONFIG_IRQ_EN_Msk |
                          R_ETHSW_INT_CONFIG_TSM_INT_Msk;

        p_switch_reg->INT_CONFIG = reg_int_config;

        gp_ethsw_time_callback = p_time_transmit->p_time_callback;
    }
    else
    {
        /* disable options */
        reg_config &= (uint32_t) ~R_ETHSW_TSM_CONFIG_IRQ_TX_EN_Msk;

        p_switch_reg->TSM_CONFIG = reg_config;

        reg_int_config &= (uint32_t) ~R_ETHSW_INT_CONFIG_TSM_INT_Msk;

        p_switch_reg->INT_CONFIG = reg_int_config;

        gp_ethsw_time_callback = 0;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TimeTransmitTimestampSet() */

/*******************************************************************************************************************//**
 * Sets the current time value to the timer with the specified timer number.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeValueSet (ether_switch_ctrl_t * const p_ctrl, ethsw_timestamp_t * p_timestamp)
{
    volatile uint32_t * p_atime;       /* nanoseconds time value */
    volatile uint32_t * p_atime_sec;   /* seconds time value */

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_timestamp), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((ETHSW_TIME_COUNT > p_timestamp->time_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Pointer ATIME[n] register */
    p_atime = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME0 +
                                     (ETHSW_PORT_OFFSET_32BYTES * p_timestamp->time_num));

    /* Pointer ATIME_SEC[n] register */
    p_atime_sec = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME_SEC0 +
                                         (ETHSW_PORT_OFFSET_32BYTES * p_timestamp->time_num));

    *p_atime_sec = p_timestamp->time_sec;
    *p_atime     = p_timestamp->time_nsec;

    return FSP_SUCCESS;
}                                      /* End of R_ETHSW_TimeValueSet() */

/*******************************************************************************************************************//**
 * Gets the current time value to the timer with the specified timer number.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeValueGet (ether_switch_ctrl_t * const p_ctrl, ethsw_timestamp_t * p_timestamp)
{
    volatile uint32_t * p_atime_ctrl;  /* time control for time */
    volatile uint32_t * p_atime;       /* nanoseconds time value */
    volatile uint32_t * p_atime_sec;   /* seconds time value */
    uint64_t            timeout;       /* timeout */
    int32_t             sign_time_nano;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_timestamp), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((ETHSW_TIME_COUNT > p_timestamp->time_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    /* Pointer ATIME_CTRL[n] register */
    p_atime_ctrl = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME_CTRL0 +
                                          (ETHSW_PORT_OFFSET_32BYTES * p_timestamp->time_num));

    /* Pointer ATIME[n] register */
    p_atime = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME0 +
                                     (ETHSW_PORT_OFFSET_32BYTES * p_timestamp->time_num));

    /* Pointer ATIME_SEC[n] register */
    p_atime_sec = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->ATIME_SEC0 +
                                         (ETHSW_PORT_OFFSET_32BYTES * p_timestamp->time_num));

    *p_atime_ctrl |= 1 << R_ETHSW_ATIME_CTRL0_CAPTURE_Pos;

    timeout = ETHSW_TIMEOUT_COUNT;

    while (0 != (*p_atime_ctrl & R_ETHSW_ATIME_CTRL0_CAPTURE_Msk))
    {
        ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
        timeout--;
    }

    p_timestamp->time_sec = *p_atime_sec;
    sign_time_nano        = (int32_t) *p_atime;

    if (sign_time_nano < 0)
    {
        p_timestamp->time_sec -= 1;
        sign_time_nano        += NANO_COUNT_FOR_1SEC;
    }
    else if (sign_time_nano >= NANO_COUNT_FOR_1SEC)
    {
        p_timestamp->time_sec += 1;
        sign_time_nano        -= NANO_COUNT_FOR_1SEC;
    }
    else
    {
        ;
    }

    p_timestamp->time_nsec    = (uint32_t) sign_time_nano;
    p_timestamp->timestamp_id = 0;

    return FSP_SUCCESS;
}                                      /* End of R_ETHSW_TimeValueGet() */

/*******************************************************************************************************************//**
 * Gets the current time value to all timer (timer number 0 and 1).
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_TIMEOUT             Timeout error
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeValueGetAll (ether_switch_ctrl_t * const p_ctrl,
                                   ethsw_timestamp_t         * p_timer0,
                                   ethsw_timestamp_t         * p_timer1)
{
    uint64_t timeout;                  /* timeout */
    uint8_t  corr_inc0;
    uint8_t  corr_inc1;
    uint8_t  clk_period0;
    uint8_t  clk_period1;
    uint8_t  set_clk_period0 = 0;
    uint8_t  set_clk_period1 = 0;

    R_ETHSW_Type volatile * p_switch_reg;
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_timer0), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((NULL != p_timer1), FSP_ERR_INVALID_POINTER);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    corr_inc0   = p_switch_reg->ATIME_INC0_b.CORR_INC;
    clk_period0 = p_switch_reg->ATIME_INC0_b.CLK_PERIOD;

    if (clk_period0 != corr_inc0)
    {
        p_switch_reg->ATIME_INC0_b.CORR_INC = clk_period0 &
                                              (R_ETHSW_ATIME_INC0_CORR_INC_Msk >> R_ETHSW_ATIME_INC0_CORR_INC_Pos);
        set_clk_period0 = 1;
    }

    corr_inc1   = p_switch_reg->ATIME_INC1_b.CORR_INC;
    clk_period1 = p_switch_reg->ATIME_INC1_b.CLK_PERIOD;

    if (clk_period1 != corr_inc1)
    {
        p_switch_reg->ATIME_INC1_b.CORR_INC = clk_period1 &
                                              (R_ETHSW_ATIME_INC0_CORR_INC_Msk >> R_ETHSW_ATIME_INC0_CORR_INC_Pos);
        set_clk_period1 = 1;
    }

    p_switch_reg->ATIME_CTRL0_b.CAPTURE_ALL = 1;

    timeout = ETHSW_TIMEOUT_COUNT;

    while (0 != p_switch_reg->ATIME_CTRL0_b.CAPTURE_ALL)
    {
        ETHSW_ERROR_RETURN((0 != timeout), FSP_ERR_TIMEOUT);
        timeout--;
    }

    p_timer0->time_sec  = p_switch_reg->ATIME_SEC0;
    p_timer0->time_nsec = p_switch_reg->ATIME0;

    p_timer1->time_sec  = p_switch_reg->ATIME_SEC1;
    p_timer1->time_nsec = p_switch_reg->ATIME1;

    p_timer0->timestamp_id = 0;
    p_timer1->timestamp_id = 0;

    p_timer0->time_num = 0;
    p_timer1->time_num = 1;

    if (set_clk_period0)
    {
        p_switch_reg->ATIME_INC0_b.CORR_INC = corr_inc0 &
                                              (R_ETHSW_ATIME_INC0_CORR_INC_Msk >> R_ETHSW_ATIME_INC0_CORR_INC_Pos);
    }

    if (set_clk_period1)
    {
        p_switch_reg->ATIME_INC1_b.CORR_INC = corr_inc1 &
                                              (R_ETHSW_ATIME_INC1_CORR_INC_Msk >> R_ETHSW_ATIME_INC1_CORR_INC_Pos);
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TimeValueGetAll() */

/*******************************************************************************************************************//**
 * Sets the peer delay value for the specified timer number of the specified port number.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimePeerDelaySet (ether_switch_ctrl_t * const p_ctrl,
                                    uint32_t                    port,
                                    ethsw_time_peerdelay_t    * p_peerdelay)
{
    volatile uint32_t * p_peerdelay_reg;

    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((ETHSW_PORT_COUNT > port), FSP_ERR_INVALID_ARGUMENT);

    ETHSW_ERROR_RETURN((NULL != p_peerdelay), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((ETHSW_TIME_COUNT > p_peerdelay->time_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    p_peerdelay_reg = (volatile uint32_t *) ((uint8_t *) &p_switch_reg->PEERDELAY_P0_T0 +
                                             (ETHSW_PORT_OFFSET_16BYTES * port) +
                                             (ETHSW_PORT_OFFSET_4BYTES * p_peerdelay->time_num));

    *p_peerdelay_reg = p_peerdelay->peerdelay;

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TimePeerDelaySet() */

/********************************************************************************************************************//**
 * Sets the offset correction for the specified timer number.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeOffsetSet (ether_switch_ctrl_t * const p_ctrl, ethsw_time_offset_correction_t * p_offset)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_offset), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(ETHSW_TIME_COUNT > p_offset->time_num, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    if (0 == p_offset->time_num)       // timer 0
    {
        p_reg_switch->ATIME_INC0_b.OFFS_CORR_INC = p_offset->offs_inc &
                                                   (R_ETHSW_ATIME_INC0_OFFS_CORR_INC_Msk >>
                                                    R_ETHSW_ATIME_INC0_OFFS_CORR_INC_Pos);

        p_reg_switch->ATIME_OFFS_CORR0 = p_offset->offs_corr;

        p_reg_switch->ATIME_OFFSET0 = p_offset->offset;
    }
    else                               // timer 1
    {
        p_reg_switch->ATIME_INC1_b.OFFS_CORR_INC = p_offset->offs_inc &
                                                   (R_ETHSW_ATIME_INC1_OFFS_CORR_INC_Msk >>
                                                    R_ETHSW_ATIME_INC1_OFFS_CORR_INC_Pos);

        p_reg_switch->ATIME_OFFS_CORR1 = p_offset->offs_corr;

        p_reg_switch->ATIME_OFFSET1 = p_offset->offset;
    }

    return err;
}                                      /* End of function R_ETHSW_TimeOffsetSet() */

/********************************************************************************************************************//**
 * Sets clock rate corection for the specified timer.
 *
 * @retval  FSP_SUCCESS                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 ***********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeRateSet (ether_switch_ctrl_t * const p_ctrl, ethsw_time_rate_correction_t * p_rate)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    fsp_err_t               err             = FSP_SUCCESS;
    R_ETHSW_Type volatile * p_reg_switch;

    uint32_t clock_period;
    uint32_t atime_increment;
    uint32_t atime_correct;
    uint32_t abs_ppb;

#define TS_CLK              125 * 1000 * 1000 /* 125MHz */
#define CLOCK_CORRECTION    1                 /* Number of clocks increasing or decreasing */

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_rate), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN(ETHSW_TIME_COUNT > p_rate->time_num, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((+TS_CLK / 10) >= p_rate->rate, FSP_ERR_INVALID_ARGUMENT);
    ETHSW_ERROR_RETURN((-TS_CLK / 10) <= p_rate->rate, FSP_ERR_INVALID_ARGUMENT);
#endif

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    if (0 == p_rate->rate)
    {
        atime_increment = 0;
        atime_correct   = 0;
    }
    else
    {
        if (0 == p_rate->time_num)     // timer 0
        {
            clock_period = p_reg_switch->ATIME_INC0_b.CLK_PERIOD;
        }
        else                           // timer 1
        {
            clock_period = p_reg_switch->ATIME_INC1_b.CLK_PERIOD;
        }

        if (0 > p_rate->rate)
        {
            atime_increment = clock_period - CLOCK_CORRECTION;

            abs_ppb = (uint32_t) (p_rate->rate * -1);
        }
        else
        {
            atime_increment = clock_period + CLOCK_CORRECTION;

            abs_ppb = (uint32_t) p_rate->rate;
        }

        atime_correct = (uint32_t) ((TS_CLK * CLOCK_CORRECTION) / abs_ppb) - 1;
    }

    if (0 == p_rate->time_num)         // timer 0
    {
        p_reg_switch->ATIME_INC0_b.CORR_INC = atime_increment &
                                              (R_ETHSW_ATIME_INC0_CORR_INC_Msk >> R_ETHSW_ATIME_INC0_CORR_INC_Pos);

        p_reg_switch->ATIME_CORR0 = atime_correct;
    }
    else                               // timer 1
    {
        p_reg_switch->ATIME_INC1_b.CORR_INC = atime_increment &
                                              (R_ETHSW_ATIME_INC1_CORR_INC_Msk >> R_ETHSW_ATIME_INC1_CORR_INC_Pos);

        p_reg_switch->ATIME_CORR1 = atime_correct;
    }

    return err;
}                                      /* End of function R_ETHSW_TimeRateSet() */

/*******************************************************************************************************************//**
 * Sets the domain number for the specified timer.
 *
 * @retval  FSP_SUCCESS                 Command successfully.
 * @retval  FSP_ERR_ASSERTION           Pointer to ETHER control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN            The control block has not been opened.
 * @retval  FSP_ERR_INVALID_POINTER     Pointer to arguments are NULL.
 * @retval  FSP_ERR_INVALID_ARGUMENT    Invalid input parameter.
 **********************************************************************************************************************/
fsp_err_t R_ETHSW_TimeDomainSet (ether_switch_ctrl_t * const p_ctrl, ethsw_time_domain_t * p_domain)
{
    ethsw_instance_ctrl_t * p_instance_ctrl = (ethsw_instance_ctrl_t *) p_ctrl;
    R_ETHSW_Type volatile * p_switch_reg;

#if (ETHSW_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    ETHSW_ERROR_RETURN(ETHSW_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);

    ETHSW_ERROR_RETURN((NULL != p_domain), FSP_ERR_INVALID_POINTER);
    ETHSW_ERROR_RETURN((ETHSW_TIME_COUNT > p_domain->time_num), FSP_ERR_INVALID_ARGUMENT);
#endif

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    if (0 == p_domain->time_num)
    {
        p_switch_reg->PTP_DOMAIN_b.DOMAIN0 = p_domain->domain_num;
    }
    else
    {
        p_switch_reg->PTP_DOMAIN_b.DOMAIN1 = p_domain->domain_num;
    }

    return FSP_SUCCESS;
}                                      /* End of function R_ETHSW_TimeDomainSet() */

/*******************************************************************************************************************//**
 * @} (end addtogroup ETHSW)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Interrupt handler for Ethernet switch.
 * Supports the interrupts for DLR, PHY link change, TSM(Timestamping) and TDMA.
 *
 * @retval      none
 **********************************************************************************************************************/
void ethsw_isr_intr (void)
{
    ether_switch_callback_args_t callback_arg;
    uint32_t          ulInt_Stat_Ack_Val;
    R_ETHSW_Type    * p_reg_switch;
    volatile uint32_t dammy_read;

    IRQn_Type               irq             = R_FSP_CurrentIrqGet();
    ethsw_instance_ctrl_t * p_instance_ctrl =
        (ethsw_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    p_reg_switch = p_instance_ctrl->p_reg_switch;

    ulInt_Stat_Ack_Val = p_reg_switch->INT_STAT_ACK;

    /* Clear IRQ_LINK Interrupt */
    p_reg_switch->INT_STAT_ACK = ulInt_Stat_Ack_Val;

    /* Dammy read for Countermeasure when the CPU clock is 400 or 800MHz */
    dammy_read = p_reg_switch->INT_STAT_ACK;
    dammy_read = dammy_read;

    /* Callback : Interrupt handler */
    if ((0 != (ulInt_Stat_Ack_Val & ETHSW_INT_CONFIG_IRQ_LNK_MASK)) &&
        (ETHSW_PHYLINK_ENABLE == ((ethsw_extend_cfg_t *) p_instance_ctrl->p_switch_cfg->p_extend)->phylink))
    {
        callback_arg.channel = p_instance_ctrl->p_switch_cfg->channel;

        callback_arg.status_link  = p_reg_switch->STATUS_P0_b.PHYLINK;
        callback_arg.status_link |= (uint32_t) (p_reg_switch->STATUS_P1_b.PHYLINK << 1);
        callback_arg.status_link |= (uint32_t) (p_reg_switch->STATUS_P2_b.PHYLINK << 2);

        callback_arg.event     = ETHER_SWITCH_EVENT_LINK_CHANGE;
        callback_arg.p_context = p_instance_ctrl->p_switch_cfg->p_context;

        (*p_instance_ctrl->p_switch_cfg->p_callback)((void *) &callback_arg);
    }

    /* DLR Interrupt */
    if (ETHSW_INT_CONFIG_DLR_INT == (ulInt_Stat_Ack_Val & ETHSW_INT_CONFIG_DLR_INT))
    {
        ethsw_isr_dlr(p_instance_ctrl);
    }

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

    /* EFP Interrupt */
    if (ETHSW_INT_CONFIG_EFP_INT == (ulInt_Stat_Ack_Val & ETHSW_INT_CONFIG_EFP_INT))
    {
        ethsw_isr_efp(p_instance_ctrl);
    }
#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

    /* RX Pattern Matcher Interrupt */
    if (ETHSW_INT_CONFIG_PATTERN_INT == (ulInt_Stat_Ack_Val & ETHSW_INT_CONFIG_PATTERN_INT))
    {
        ethsw_isr_rx_pattern_matcher(p_instance_ctrl);
    }

    /* TDMA Interrupt */
    if (ETHSW_INT_CONFIG_TDMA_INT == (ulInt_Stat_Ack_Val & ETHSW_INT_CONFIG_TDMA_INT))
    {
        ethsw_isr_tdma(p_instance_ctrl);
    }

    /* TSM Interrupt */
    if (ETHSW_INT_CONFIG_TSM_INT == (ulInt_Stat_Ack_Val & ETHSW_INT_CONFIG_TSM_INT))
    {
        ethsw_isr_tsm(p_instance_ctrl);
    }
}

/*******************************************************************************************************************//**
 * Call the callback function registered for the DLR.
 *
 * @param[in]   event           event ID
 * @param[in]   port            source port of DLR event
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_dlr_callback (ethsw_dlr_event_t event, uint32_t port)
{
    if (0 != gp_ethsw_dlr_callback)
    {
        (*gp_ethsw_dlr_callback)(event, port);
    }
}                                      /* End of ethsw_dlr_callback() */

/*******************************************************************************************************************//**
 * Interrupt handler for DLR
 * This function is called by ethsw_isr_intr
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_isr_dlr (ethsw_instance_ctrl_t * p_instance_ctrl)
{
    R_ETHSW_Type      * p_switch_reg;
    ethsw_link_status_t link_state = ETHSW_LINK_STATE_DOWN; /* link state */
    uint32_t            int_ID;                             /* interrupt Flag */
    uint32_t            reg_val;                            /* register content */
    int32_t             cnt;                                /* loop counter */

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    while (p_switch_reg->DLR_IRQ_STAT_ACK)
    {
        reg_val = p_switch_reg->DLR_IRQ_STAT_ACK;
        p_switch_reg->DLR_IRQ_STAT_ACK = reg_val;

        /* scan all status bits */
        for (cnt = 0; cnt < 16; cnt++)
        {
            int_ID = reg_val & (1U << cnt);

            if (int_ID == 0)
            {
                /* bit wasn't set, check next one */
                continue;
            }

            switch (int_ID)
            {
                case ETHSW_TGT_DLR_INT_NEWSTATUS:
                {
                    ethsw_dlr_callback(ETHSW_DLR_EVENT_NEWSTATE, ETHSW_PORT_MGMT);
                    break;
                }

                case ETHSW_TGT_DLR_INT_BEACONTMO_0:
                {
                    ethsw_dlr_callback(ETHSW_DLR_EVENT_BEACONTIMEOUT, 0);
                    break;
                }

                case ETHSW_TGT_DLR_INT_BEACONTMO_1:
                {
                    ethsw_dlr_callback(ETHSW_DLR_EVENT_BEACONTIMEOUT, 1);
                    break;
                }

                case ETHSW_TGT_DLR_INT_SUVICHNG:
                {
                    ethsw_dlr_callback(ETHSW_DLR_EVENT_NEWSUPERVISOR, 0);
                    break;
                }

                case ETHSW_TGT_DLR_INT_LNKCHNG_0:
                {
                    R_ETHSW_LinkStateGet((ether_switch_ctrl_t *) p_instance_ctrl, 0, &link_state);

                    if (ETHSW_LINK_STATE_UP == link_state)
                    {
                        ethsw_dlr_callback(ETHSW_DLR_EVENT_LINKRESTORED, 0);
                    }
                    else
                    {
                        ethsw_dlr_callback(ETHSW_DLR_EVENT_LINKLOST, 0);
                    }

                    break;
                }

                case ETHSW_TGT_DLR_INT_LNKCHNG_1:
                {
                    R_ETHSW_LinkStateGet((ether_switch_ctrl_t *) p_instance_ctrl, 1, &link_state);

                    if (ETHSW_LINK_STATE_UP == link_state)
                    {
                        ethsw_dlr_callback(ETHSW_DLR_EVENT_LINKRESTORED, 1);
                    }
                    else
                    {
                        ethsw_dlr_callback(ETHSW_DLR_EVENT_LINKLOST, 1);
                    }

                    break;
                }

                case ETHSW_TGT_DLR_INT_LOCALLOOP_0:
                {
                    ethsw_dlr_callback(ETHSW_DLR_EVENT_OWNFRAME, 0);
                    break;
                }

                case ETHSW_TGT_DLR_INT_LOCALLOOP_1:
                {
                    ethsw_dlr_callback(ETHSW_DLR_EVENT_OWNFRAME, 1);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
}                                      /* End of ethsw_isr_dlr() */

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

/*******************************************************************************************************************//**
 * Interrupt handler for Extended Frame Parser.
 * This function is called by ethsw_isr_intr.
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_isr_efp (ethsw_instance_ctrl_t * p_instance_ctrl)
{
    R_ETHSW_Type    * p_switch_reg = p_instance_ctrl->p_reg_switch;
    uint32_t          port;
    ethsw_efp_event_t id;
    uint32_t          en_cb = 0;

    if (0 != gp_ethsw_efp_callback)
    {
        volatile uint32_t * pn_qgeis        = &p_switch_reg->P0_QGEIS;
        volatile uint32_t * pn_qseis        = &p_switch_reg->P0_QSEIS;
        volatile uint32_t * pn_qmeis        = &p_switch_reg->P0_QMEIS;
        volatile uint32_t * pn_error_status = &p_switch_reg->P0_ERROR_STATUS;

        for (port = 0; port < ETHSW_PORT_COUNT; port++)
        {
            /* Check Qci stream gate error interrupt */
            if ((*pn_qgeis & R_ETHSW_P0_QGEIS_QGMOIS_Msk) != 0)
            {
                id        = ETHSW_EFP_INT_SRC_GATE_ERROR_DROP;
                *pn_qgeis = *pn_qgeis;
                en_cb     = 1;
                break;
            }

            /* Qci stream filter error interrupt */
            if ((*pn_qseis & R_ETHSW_P0_QSEIS_QSMOIS_Msk) != 0)
            {
                id        = ETHSW_EFP_INT_SRC_SDU_OVER_DROP;
                *pn_qseis = *pn_qseis;
                en_cb     = 1;
                break;
            }

            /* Qci meter error interrupt */
            if ((*pn_qmeis & R_ETHSW_P0_QMEIS_QRFIS_Msk) != 0)
            {
                id        = ETHSW_EFP_INT_SRC_FLOW_METER_DROP;
                *pn_qmeis = *pn_qmeis;
                en_cb     = 1;
                break;
            }

            /* Frame parser error interrupt */
            if ((*pn_error_status & 0x1FU) != 0)
            {
                id               = ETHSW_EFP_INT_SRC_RUNTIME_ERROR;
                *pn_error_status = *pn_error_status;
                en_cb            = 1;
                break;
            }

            pn_qgeis        += ETHSW_QGEIS_OFFSET;
            pn_qseis        += ETHSW_QSEIS_OFFSET;
            pn_qmeis        += ETHSW_QMEIS_OFFSET;
            pn_error_status += ETHSW_ERROR_STATUS_OFFSET;
        }

        if (en_cb == 1)
        {
            (*gp_ethsw_efp_callback)(port, id);
        }
    }
}

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

/*******************************************************************************************************************//**
 * Interrupt handler for Rx pattern matcher
 * This function is called by ethsw_isr_intr
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_isr_rx_pattern_matcher (ethsw_instance_ctrl_t * p_instance_ctrl)
{
    R_ETHSW_Type                * p_switch_reg = p_instance_ctrl->p_reg_switch;
    ethsw_rx_pattern_event_data_t event_data;
    uint32_t pattern_irq_stat_ack;

    pattern_irq_stat_ack = p_switch_reg->PATTERN_IRQ_STAT_ACK;

    p_switch_reg->PATTERN_IRQ_STAT_ACK = pattern_irq_stat_ack;

    if (0 != gp_ethsw_rx_pattern_callback)
    {
        event_data.match_int = (pattern_irq_stat_ack & R_ETHSW_PATTERN_IRQ_STAT_ACK_MATCHINT_Msk) >>
                               R_ETHSW_PATTERN_IRQ_STAT_ACK_MATCHINT_Pos;
        event_data.error_int = (pattern_irq_stat_ack & R_ETHSW_PATTERN_IRQ_STAT_ACK_ERROR_INT_Msk) >>
                               R_ETHSW_PATTERN_IRQ_STAT_ACK_ERROR_INT_Pos;

        (*gp_ethsw_rx_pattern_callback)(ETHSW_RX_PATTERN_MATCHER, &event_data);
    }
}

/*******************************************************************************************************************//**
 * Interrupt handler for TDMA.
 * This function is called by ethsw_isr_intr.
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_isr_tdma (ethsw_instance_ctrl_t * p_instance_ctrl)
{
    ethsw_tdma_callback_data_t callback_data;
    R_ETHSW_Type             * p_switch_reg;
    uint32_t tdma_irq_stat_ack;

    p_switch_reg                    = p_instance_ctrl->p_reg_switch;
    tdma_irq_stat_ack               = p_switch_reg->TDMA_IRQ_STAT_ACK;
    p_switch_reg->TDMA_IRQ_STAT_ACK = tdma_irq_stat_ack;

    if (0 != gp_ethsw_tdma_callback)
    {
        if (0 != (tdma_irq_stat_ack & R_ETHSW_TDMA_IRQ_STAT_ACK_TCV_ACK_Msk))
        {
            callback_data.tcv_s_idx = p_switch_reg->TCV_SEQ_LAST_b.ACTIVE;

            (*gp_ethsw_tdma_callback)(ETHSW_TDMA_CALLBACK_TCV_INT, &callback_data);
        }

        if (0 != (tdma_irq_stat_ack & R_ETHSW_TDMA_IRQ_STAT_ACK_CTR1_ACK_Msk))
        {
            callback_data.tcv_s_idx = 0;

            (*gp_ethsw_tdma_callback)(ETHSW_TDMA_CALLBACK_COUNTER1_INT, &callback_data);
        }
    }
}                                      /* End of ethsw_isr_tdma() */

/*******************************************************************************************************************//**
 * Interrupt handler for TSM(Timestamping)
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_isr_tsm (ethsw_instance_ctrl_t * p_instance_ctrl)
{
    ethsw_timestamp_t timestamp;
    uint32_t          port;
    uint32_t          irq_tx_en;
    uint32_t          port_bit;
    uint32_t          irq_stat_ack;
    R_ETHSW_Type    * p_switch_reg;
    uint32_t          ts_fifo_read_ctrl;
    int32_t           sign_time_nano;

    p_switch_reg = p_instance_ctrl->p_reg_switch;

    irq_stat_ack = p_switch_reg->TSM_IRQ_STAT_ACK;
    p_switch_reg->TSM_IRQ_STAT_ACK = irq_stat_ack;

    irq_tx_en = (irq_stat_ack & R_ETHSW_TSM_IRQ_STAT_ACK_IRQ_TX_Msk) >> R_ETHSW_TSM_IRQ_STAT_ACK_IRQ_TX_Pos;
    for (port = 0; irq_tx_en; port++)
    {
        port_bit = 1U << port;
        if (irq_tx_en & port_bit)
        {
            irq_tx_en &= (~port_bit);

            while (1)
            {
                ts_fifo_read_ctrl = (port << R_ETHSW_TS_FIFO_READ_CTRL_PORT_NUM_Pos) &
                                    R_ETHSW_TS_FIFO_READ_CTRL_PORT_NUM_Msk;
                p_switch_reg->TS_FIFO_READ_CTRL = ts_fifo_read_ctrl;

                ts_fifo_read_ctrl = p_switch_reg->TS_FIFO_READ_CTRL;

                if (0 == (ts_fifo_read_ctrl & R_ETHSW_TS_FIFO_READ_CTRL_TS_VALID_Msk))
                {
                    break;
                }

                timestamp.time_num = (uint8_t) ((ts_fifo_read_ctrl & R_ETHSW_TS_FIFO_READ_CTRL_TS_SEL_Msk) >>
                                                R_ETHSW_TS_FIFO_READ_CTRL_TS_SEL_Pos);

                timestamp.timestamp_id = ((ts_fifo_read_ctrl & R_ETHSW_TS_FIFO_READ_CTRL_TS_ID_Msk) >>
                                          R_ETHSW_TS_FIFO_READ_CTRL_TS_ID_Pos);

                sign_time_nano = (int32_t) p_switch_reg->TS_FIFO_READ_TIMESTAMP;

                if (sign_time_nano < 0)
                {
                    sign_time_nano += NANO_COUNT_FOR_1SEC;
                }
                else if (sign_time_nano >= NANO_COUNT_FOR_1SEC)
                {
                    sign_time_nano -= NANO_COUNT_FOR_1SEC;
                }
                else
                {
                    ;
                }

                timestamp.time_nsec = (uint32_t) sign_time_nano;

                if (gp_ethsw_time_callback)
                {
                    (*gp_ethsw_time_callback)(ETHSW_TIME_CALLBACK_TIMESTAMP, (uint8_t) port, &timestamp);
                }
            }
        }
    }
}                                      /* End of ethsw_isr_tsm() */

/*******************************************************************************************************************//**
 * Enable ICU for Ether Switcht
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_enable_icu (ethsw_instance_ctrl_t * const p_instance_ctrl)
{
    /** Configure the Ethernet Switch interrupt. */
    R_BSP_IrqCfgEnable(p_instance_ctrl->p_switch_cfg->irq, p_instance_ctrl->p_switch_cfg->ipl, p_instance_ctrl);
}                                      /* End of function ethsw_enable_icu() */

/*******************************************************************************************************************//**
 * Disable ICU for Ether Switcht
 *
 * @param[in]   p_instance_ctrl     pointer to control structure
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethsw_disable_icu (ethsw_instance_ctrl_t * const p_instance_ctrl)
{
    /* Get IRQ number for A5PSW_Int interrupt. */
    R_BSP_IrqDisable(p_instance_ctrl->p_switch_cfg->irq);
    R_FSP_IsrContextSet(p_instance_ctrl->p_switch_cfg->irq, NULL);
}                                      /* End of function ether_disable_icu() */

#ifdef  ETHSW_EFP_FEATURE_SUPPORTED

/*******************************************************************************************************************//**
 * Create the CRC value and set it in buffer
 * The CRC value and source bit stream is string consisting of '1' or '0'.
 *
 * @param[in]     bit_str      Pointer to Source bit stream
 * @param[in]     len          Length  of Source bit stream
 * @param[in]     crc_buf      Pointer to CRC set buffer
 *
 * @retval        none
 **********************************************************************************************************************/
static void r_ethsw_make_crc (char * bit_str, uint32_t len, char * crc_buf)
{
    char     crc[7];
    uint32_t i;
    char     do_inv;

    for (i = 0; i < 7; ++i)
    {
        crc[i] = 1;
    }

    for (i = 0; i < len; ++i)
    {
        do_inv = (char) (('1' == bit_str[i]) ^ crc[6]);

        crc[6] = crc[5] ^ do_inv;
        crc[5] = crc[4] ^ do_inv;
        crc[4] = crc[3];
        crc[3] = crc[2];
        crc[2] = crc[1] ^ do_inv;
        crc[1] = crc[0];
        crc[0] = do_inv;
    }

    for (i = 0; i < 7; ++i)
    {
        crc_buf[6 - i] = crc[i] ? '1' : '0';
    }

    crc_buf[7] = 0;
}

/*******************************************************************************************************************//**
 * Calculate the hash (CRC-7)
 *
 * @param[in]   dmaci      DMAC of incoming frame
 * @param[in]   vidi       VID  of incoming frame
 *
 * @retval      calculated hash
 **********************************************************************************************************************/
static uint8_t r_ethsw_calc_hash (ethsw_mac_addr_t * dmaci, ethsw_vlan_vid_t * vidi)
{
    uint8_t  c_mes_bit[ETHSW_EFP_ASI_HASH_SZ];
    char     c_crc[8];
    uint8_t  c_veri_mes_bit[ETHSW_EFP_ASI_HASH_SZ + ETHSW_EFP_ASI_ADDR_SZ];
    uint16_t test_vid = (uint16_t) (((*vidi)[0] << 8) + ((*vidi)[1])) & ETHSW_EFP_ASI_HASH_VID_MASK;
    uint8_t  hash     = 0;
 #ifdef HASH_CULC_DEBUG
    char    vc_crc[8];
    uint8_t hash_key = 0;
 #endif

    /* dmaci */
    for (uint8_t i = 0; i < ETHSW_EFP_ASI_HASH_DA_SZ / 8; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            c_mes_bit[(i * 8) + j] = (uint8_t) ((((*dmaci)[i] >> (7 - j)) & 0x01) + '0');
        }
    }

    /* vidi */
    for (uint8_t i = 0; i < ETHSW_EFP_ASI_HASH_VID_SZ; i++)
    {
        c_mes_bit[ETHSW_EFP_ASI_HASH_DA_SZ +
                  (i)] = (uint8_t) (((test_vid >> (ETHSW_EFP_ASI_HASH_VID_SZ - 1 - i)) & 0x0001) + '0');
    }

    /* padding */
    for (uint8_t i = ETHSW_EFP_ASI_HASH_DA_SZ + ETHSW_EFP_ASI_HASH_VID_SZ; i < ETHSW_EFP_ASI_HASH_SZ; i++)
    {
        c_mes_bit[i] = '0';
    }

    /* calculate crc value */
    r_ethsw_make_crc((char *) c_mes_bit, ETHSW_EFP_ASI_HASH_SZ, c_crc);

    /* verify crc value */
    memcpy(c_veri_mes_bit, c_mes_bit, ETHSW_EFP_ASI_HASH_SZ);
    memcpy(&c_veri_mes_bit[ETHSW_EFP_ASI_HASH_SZ], c_crc, ETHSW_EFP_ASI_ADDR_SZ);
 #ifdef HASH_CULC_DEBUG
    r_ethsw_make_crc((char *) c_veri_mes_bit, (ETHSW_EFP_ASI_HASH_SZ + ETHSW_EFP_ASI_ADDR_SZ), vc_crc);
    for (uint8_t i = 0; i < ETHSW_EFP_ASI_ADDR_SZ; i++)
    {
        if (vc_crc[i] == '1')
        {
            hash_key |= (uint8_t) (1U << (ETHSW_EFP_ASI_ADDR_SZ - 1 - i));
        }
    }
    app_printf("entry [dmaci] %02X %02X %02X %02X %02X %02X [vidi] %02X %02X\n",
               (*dmaci)[0],
               (*dmaci)[1],
               (*dmaci)[2],
               (*dmaci)[3],
               (*dmaci)[4],
               (*dmaci)[5],
               (*vidi)[0],
               (*vidi)[1]);
 #endif

    /* convert to hash key in binary */
    for (uint8_t i = 0; i < ETHSW_EFP_ASI_ADDR_SZ; i++)
    {
        if (c_crc[i] == '1')
        {
            hash |= (uint8_t) (1U << (ETHSW_EFP_ASI_ADDR_SZ - 1 - i));
        }
    }

 #ifdef HASH_CULC_DEBUG
    app_printf("hash = 0x%02X, verified crc = 0x%02X\n", hash, hash_key);
 #endif

    return hash;
}

#endif                                 /* ETHSW_EFP_FEATURE_SUPPORTED */

/*******************************************************************************************************************//**
 * Unlock write access protection for Ethernet subsystem registers
 *
 * @param[in]   p_reg_base      Base address of Ethernet subsystem registers
 *
 * @retval      none
 **********************************************************************************************************************/
static void r_ethsw_reg_protection_disable (void * const p_reg_base)
{
    /* Key code for PRCMD register */
#define ETHSW_PRCMD_UNLOCK1    (0x000000A5U)
#define ETHSW_PRCMD_UNLOCK2    (0x00000001U)
#define ETHSW_PRCMD_UNLOCK3    (0x0000FFFEU)
#define ETHSW_PRCMD_UNLOCK4    (0x00000001U)

    R_ETHSS_Type * p_reg_ethss = (R_ETHSS_Type *) p_reg_base;

    /* Unlock the protection of ethernet subsystem register */
    p_reg_ethss->PRCMD = ETHSW_PRCMD_UNLOCK1;
    p_reg_ethss->PRCMD = ETHSW_PRCMD_UNLOCK2;
    p_reg_ethss->PRCMD = ETHSW_PRCMD_UNLOCK3;
    p_reg_ethss->PRCMD = ETHSW_PRCMD_UNLOCK4;
}                                      /* End of function r_ethsw_reg_protection_disable() */

/*******************************************************************************************************************//**
 * Lock write access protection for Ethernet subsystem registers
 *
 * @param[in]   p_reg_base      Base address of Ethernet subsystem registers
 *
 * @retval      none
 **********************************************************************************************************************/
static void r_ethsw_reg_protection_enable (void * const p_reg_base)
{
    /* Key code for PRCMD register */
#define ETHSW_PRCMD_LOCK    (0x00000000U)

    R_ETHSS_Type * p_reg_ethss = (R_ETHSS_Type *) p_reg_base;

    /* Lock the protection of ethernet access register */
    p_reg_ethss->PRCMD = ETHSW_PRCMD_LOCK;
}                                      /* End of function r_ethsw_reg_protection_enable() */
