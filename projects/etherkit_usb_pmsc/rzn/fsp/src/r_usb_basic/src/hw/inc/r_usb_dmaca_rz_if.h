/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics Corporation and/or its affiliates and may only
 * be used with products of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.
 * Renesas products are sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for
 * the selection and use of Renesas products and Renesas assumes no liability.  No license, express or implied, to any
 * intellectual property right is granted by Renesas.  This software is protected under all applicable laws, including
 * copyright laws. Renesas reserves the right to change or discontinue this software and/or this documentation.
 * THE SOFTWARE AND DOCUMENTATION IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND
 * TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY,
 * INCLUDING WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE
 * SOFTWARE OR DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR
 * DOCUMENTATION (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER,
 * INCLUDING, WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY
 * LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/*******************************************************************************
 * File Name    : r_usb_dmaca_rx_if.h
 * Description  : Functions for DMACA driver
 *******************************************************************************/

/*******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 30.06.2014 1.00    Initial revision
 *         : 27.08.2014 1.01    Not change program due to updating application note only.
 *         : 30.01.2015 1.02    Added RX71M.
 *         : 13.04.2015 1.03    Added RX231 and RX230.
 *         : 30.09.2016 1.04    Added RX65N.
 *         :                    Added transfer_count and vlock_size to usb_dmaca_stat_t.
 *******************************************************************************/
#ifndef USB_DMACA_RX_IF_H
#define USB_DMACA_RX_IF_H

/*******************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *******************************************************************************/

/* Includes board and MCU related header files. */
#include "../../driver/inc/r_usb_typedef.h"

/* Configuration for this package. */
#include "../../hw/inc/r_usb_dmaca_rz_config.h"
#include "../../hw/inc/r_usb_dmac.h"
#include <stdbool.h>

#if USB_CFG_DMA == USB_CFG_ENABLE

/*******************************************************************************
 * Macro definitions
 *******************************************************************************/

/* Version Number of API */
 #define USB_DMACA_VERSION_MAJOR         (1)
 #define USB_DMACA_VERSION_MINOR         (0)

/* Channel Status Register .b00:EN bit mask*/
 #define USB_DMACA_CHSTAT_EN_MASK        (0x1 << 0)

/* Channel Status Register .b01:RQST bit mask*/
 #define USB_DMACA_CHSTAT_RQST_MASK      (0x1 << 1)

/* Channel Status Register .b02:TACT bit mask*/
 #define USB_DMACA_CHSTAT_TACT_MASK      (0x1 << 2)

/* Channel Status Register .b03:SUS bit mask*/
 #define USB_DMACA_CHSTAT_SUS_MASK       (0x1 << 3)

/* Channel Status Register .b04:ER bit mask*/
 #define USB_DMACA_CHSTAT_ER_MASK        (0x1 << 4)

/* Channel Status Register .b05:END bit mask*/
 #define USB_DMACA_CHSTAT_END_MASK       (0x1 << 5)

/* Channel Status Register .b06:TC bit mask*/
 #define USB_DMACA_CHSTAT_TC_MASK        (0x1 << 6)

/* Channel Status Register .b07:SR bit mask*/
 #define USB_DMACA_CHSTAT_SR_MASK        (0x1 << 7)

/* Channel Status Register .b08:DL bit mask*/
 #define USB_DMACA_CHSTAT_DL_MASK        (0x1 << 8)

/* Channel Status Register .b09:DW bit mask*/
 #define USB_DMACA_CHSTAT_DW_MASK        (0x1 << 9)

/* Channel Status Register .b10:DER bit mask*/
 #define USB_DMACA_CHSTAT_DER_MASK       (0x1 << 10)

/* Channel Status Register .b11:MODE bit mask*/
 #define USB_DMACA_CHSTAT_MODE_MASK      (0x1 << 11)

/* Channel Status Register .b16:INTM bit mask*/
 #define USB_DMACA_CHSTAT_INTM_MASK      (0x1 << 16)

/* Channel Status Register .b17:DMARQM bit mask*/
 #define USB_DMACA_CHSTAT_DMARQM_MASK    (0x1 << 17)

/* Channel Status Register .b18:SWPRQ bit mask*/
 #define USB_DMACA_CHSTAT_SWPRQ_MASK     (0x1 << 18)

/* Channel Status Register .b31-24:DNUM bit mask*/
 #define USB_DMACA_CHSTAT_DNUM_MASK      (0xff << 24)

/* Channel Configuration Register .b00:SEL bit mask*/
 #define USB_DMACA_CHCFG_SEL_MASK        (0x1 << 0)

/* Channel Configuration Register .b03:REQD bit mask*/
 #define USB_DMACA_CHCFG_REQD_MASK       (0x1 << 3)

/* Channel Configuration Register .b15-12:SDS[3:0] bit mask*/
 #define USB_DMACA_CHCFG_SDS_MASK        (0xf << 12)

/* Channel Configuration Register .b19-16:DDS[3:0] bit mask*/
 #define USB_DMACA_CHCFG_DDS_MASK        (0xf << 16)

/* Channel Configuration Register .b20:SAD bit mask*/
 #define USB_DMACA_CHCFG_SAD_MASK        (0x1 << 20)

/* Channel Configuration Register .b21:DAD bit mask*/
 #define USB_DMACA_CHCFG_DAD_MASK        (0x1 << 21)

/* Channel Configuration Register .b23:WONLY bit mask*/
 #define USB_DMACA_CONFIG_WONLY_MASK     (0x1 << 23)

/* Channel Configuration Register .b24:DEM bit mask*/
 #define USB_DMACA_CHCFG_DEM_MASK        (0x1 << 24)

/* Channel Configuration Register .b25:TCM bit mask*/
 #define USB_DMACA_CHCFG_TCM_MASK        (0x1 << 25)

/* Channel Configuration Register .b26:DIM bit mask*/
 #define USB_DMACA_CHCFG_DIM_MASK        (0x1 << 26)

/* Channel Configuration Register .b27:SBE bit mask*/
 #define USB_DMACA_CONFIG_SBE_MASK       (0x1 << 27)

/* Channel Configuration Register .b28:RSEL bit mask*/
 #define USB_DMACA_CONFIG_RSEL_MASK      (0x1 << 28)

/* Channel Configuration Register .b29:RSW bit mask*/
 #define USB_DMACA_CONFIG_RSW_MASK       (0x1 << 29)

/* Channel Configuration Register .b30:REN bit mask*/
 #define USB_DMACA_CONFIG_REN_MASK       (0x1 << 30)

/* Channel Configuration Register .b31:DMS bit mask*/
 #define USB_DMACA_CONFIG_DMS_MASK       (0x1 << 31)

/* Channel Configuration Register .b14-12:SDS[2:0] bit set*/
typedef enum e_usb_dmaca_chcfg_src_size
{
    USB_DMACA_CONFIG_SRC_SIZE_BYTE  = (0x0 << 12),
    USB_DMACA_CONFIG_SRC_SIZE_WORD  = (0x1 << 12),
    USB_DMACA_CONFIG_SRC_SIZE_LWORD = (0x2 << 12)
} usb_dmaca_chcfg_src_size_t;

/* Channel Configuration Register .b18-16:DDS[2:0] bit set*/
typedef enum e_usb_dmaca_chcfg_des_size
{
    USB_DMACA_CONFIG_DES_SIZE_BYTE  = (0x0 << 16),
    USB_DMACA_CONFIG_DES_SIZE_WORD  = (0x1 << 16),
    USB_DMACA_CONFIG_DES_SIZE_LWORD = (0x2 << 16)
} usb_dmaca_chcfg_des_size_t;

/* DMA Control Register .b01:LVINT bit set */
typedef enum e_usb_dmaca_int_detect_select
{
    USB_DMACA_INT_DETECT_LEVEL = (0x1 << 1)
} usb_dmaca_int_detect_select_t;

/* CHSTAT Register b01:RQST clear (CHCTRL Register b04:CLRRQ)  */
 #define USB_DMACA_CHSTAT_RQST_CLEAR       (0x1 << 4)

/* CHSTAT Register clear (CHCTRL Register b03:SWRST)  */
 #define USB_DMACA_CHSTAT_CLEAR            (0x1 << 3)

/* CHSTAT Register b05:END clear (CHCTRL Register b05:CLREND)  */
 #define USB_DMACA_CHSTAT_END_CLEAR        (0x1 << 5)

/* CHSTAT Register b06:TC clear (CHCTRL Register b06:CLRTC)  */
 #define USB_DMACA_CHSTAT_TC_CLEAR         (0x1 << 6)

/* USBFDMA clear (CHCTRL Register b07:CLRDER)  */
 #define USB_DMACA_CHSTAT_USBFDMA_CLEAR    (0x1 << 7)

/* CHCTRL Register b19-08 */
 #define USB_DMACA_CHCTRL_CLEAR            ((0x1 << 8) | (0x1 << 9) | (0x1 << 12) | (0x1 << 14) | (0x1 << 16) | \
                                            (0x1 << 17) | (0x1 << 18) | (0x1 << 19))

/* Definition of DMAC channel */
 #define USB_DMACA_CH0                     (0)
 #define USB_DMACA_CH1                     (1)

/*******************************************************************************
 * Typedef definitions
 *******************************************************************************/

/* Enumerate list that can be selected as DMAC activation source */
/* enum enum_ir is included from iodefine.h.  */
typedef enum enum_ir usb_dmaca_activation_source_t;

typedef enum e_usb_dmaca_command
{
    USB_DMACA_CMD_ENABLE = 0,          /* Enables DMA transfer. */
    USB_DMACA_CMD_RESUME,              /* Resumes DMA transfer. */
    USB_DMACA_CMD_DISABLE,             /* Enables DMA transfer. */
    USB_DMACA_CMD_SOFT_REQ_CLR,        /* Clears DMACA Software request flag. */
    USB_DMACA_CMD_STATUS_GET,          /* Gets the current status of DMACA. */
    USB_DMACA_CMD_SOFT_REQ,
    USB_DMACA_CMD_CH_STATUS_CLR,
    USB_DMACA_CMD_REG_SET_FIX,
    USB_DMACA_CMD_STATUS_CLR
} usb_dmaca_command_t;

typedef enum e_usb_dmaca_return
{
    USB_DMACA_SUCCESS_OTHER_CH_BUSY = 0, /* Other DMAC channels are locked, */
    /*  so that cannot set to module stop state. */
    USB_DMACA_SUCCESS,

    USB_DMACA_ERR_INVALID_CH,            /* Channel is invalid. */
    USB_DMACA_ERR_INVALID_ARG,           /* Parameters are invalid. */
    USB_DMACA_ERR_INVALID_HANDLER_ADDR,  /* Invalid function address is set, */
    /*  and any previous function has been unregistered. */
    USB_DMACA_ERR_INVALID_COMMAND,       /* Command is invalid. */
    USB_DMACA_ERR_NULL_PTR,              /* Argument pointers are NULL. */
    USB_DMACA_ERR_BUSY,                  /* Resource has been locked by other process. */
    USB_DMACA_ERR_INTERNAL               /* DMACA Driver internal error */
} usb_dmaca_return_t;

typedef struct st_usb_dmaca_stat
{
    bool     act_stat;
    bool     end_stat;
    bool     er_stat;
    uint32_t transfer_count;           /* Transfer Count */
} usb_dmaca_stat_t;

/* Transfer data configuration */
typedef struct st_usb_dmaca_transfer_data_cfg
{
    usb_dmaca_chcfg_src_size_t      src_size;     /* Channel Configuration Register .b14-12:SDS[2:0] bit set*/
    usb_dmaca_chcfg_des_size_t      des_size;     /* Channel Configuration Register .b18-16:DDS[2:0] bit set*/
    usb_dmaca_chcfg_fifo_select_t   use_fifo;     /* Channel Configuration Register .b00:SEL bit set */
    usb_dmaca_chcfg_dmareq_select_t req_dir;      /* Channel Configuration Register .b03:REQD bit set */
    usb_dmaca_chcfg_src_dir_t       src_adr_dir;  /* Channel Configuration Register .b20:SAD bit set */
    usb_dmaca_chcfg_des_dir_t       des_adr_dir;  /* Channel Configuration Register .b21:DAD bit set */
    usb_dmaca_chcfg_int_msk_t       dmaint_msk;   /* Channel Configuration Register .b24:DEM bit set */
    usb_dmaca_chcfg_tc_msk_t        dmatc_msk;    /* Channel Configuration Register .b25:TCM bit set */
    usb_dmaca_ch_interval_t         ch_interval;  /* Channel Interval Register set */
    usb_dmaca_dctrl_priority_t      trx_priority; /* DMA Control Register .b00:PR bit set */
    void   * p_src_addr;                          /* Start Address of Source */
    void   * p_des_addr;                          /* Start Address of Destination */
    uint32_t transfer_count;                      /* Transfer Count */
    uint8_t  rsv[2];
} usb_dmaca_transfer_data_cfg_t;

/*******************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_create(uint8_t channel, usb_dmaca_transfer_data_cfg_t * p_data_cfg);
usb_dmaca_return_t r_usb_dmaca_control(uint8_t channel, usb_dmaca_command_t command, usb_dmaca_stat_t * p_stat);
usb_dmaca_return_t r_usb_dmaca_int_enable(uint8_t channel, uint8_t priority);
usb_dmaca_return_t r_usb_dmaca_int_disable(uint8_t channel);
uint32_t           r_usb_dmaca_getversion(void);
void               r_usb_dmaca_init(void);
usb_dmaca_return_t r_usb_dmaca_int_callback(uint8_t channel, void * callback);

extern usb_dma_int_t g_usb_cstd_dma_int;
#endif
#endif                                 /* USB_DMACA_RX_IF_H */

/* End of File */
