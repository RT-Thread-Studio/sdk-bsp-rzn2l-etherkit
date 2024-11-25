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

/***********************************************************************************************************************
 * File Name    : r_usb_basic_local.h
 * Version      : 1.0
 * Description  : Header file USB basic
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef USB_BASIC_LOCAL_H
#define USB_BASIC_LOCAL_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_usb_basic_if.h"

#if (BSP_CFG_RTOS == 2)
 #include "r_usb_rtos_config.h"
 #include "r_usb_cstd_rtos.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* HUB down port */
#define USB_HUBDOWNPORT    (4U)        /* HUB downport (MAX15) */

/* H/W function type */
#define USB_BIT0           ((uint16_t) 0x0001)
#define USB_BIT1           ((uint16_t) 0x0002)
#define USB_BIT2           ((uint16_t) 0x0004)
#define USB_BIT3           ((uint16_t) 0x0008)
#define USB_BIT4           ((uint16_t) 0x0010)
#define USB_BIT5           ((uint16_t) 0x0020)
#define USB_BIT6           ((uint16_t) 0x0040)
#define USB_BIT7           ((uint16_t) 0x0080)
#define USB_BIT8           ((uint16_t) 0x0100)
#define USB_BIT9           ((uint16_t) 0x0200)
#define USB_BIT10          ((uint16_t) 0x0400)
#define USB_BIT11          ((uint16_t) 0x0800)
#define USB_BIT12          ((uint16_t) 0x1000)
#define USB_BIT13          ((uint16_t) 0x2000)
#define USB_BIT14          ((uint16_t) 0x4000)
#define USB_BIT15          ((uint16_t) 0x8000)
#define USB_BITSET(x)                 ((uint16_t) ((uint16_t) 1 << (x)))

#define USB_RTOS_SND_MSG(id, mess)    usb_hstd_rtos_snd_msg((uint8_t) id, (usb_msg_t *) mess)
#define USB_RTOS_RCV_MSG(id, mess)    usb_hstd_rtos_rec_msg((uint8_t) id, (usb_msg_t **) mess, 0)

/* HCI Task */
#define USB_HCI_TSK     (USB_TID_0)    /* Task ID */
#define USB_HCI_MBX     (USB_HCI_TSK)  /* Mailbox ID */
#define USB_HCI_MPL     (USB_HCI_TSK)  /* Memorypool ID */
#define USB_HCI_MSG     (USB_HCI_TSK)  /* Message ID */

/* Host Manager Task */
#define USB_MGR_TSK     (USB_TID_1)    /* Task ID */
#define USB_MGR_MBX     (USB_MGR_TSK)  /* Mailbox ID */
#define USB_MGR_MPL     (USB_MGR_TSK)  /* Memorypool ID */
#define USB_MGR_MSG     (USB_MGR_TSK)  /* Message ID */

/* Application MailBox */
#define USB_INT         (USB_TID_10)   /* Mailbox ID */

/* Maximum Task ID +1 */
#define USB_IDMAX       (11U)

/* Maximum Priority number +1 */
#define USB_PRIMAX      (8U)

/* Maximum block */
#define USB_BLKMAX      (20U)

/* Maximum priority table */
#define USB_TABLEMAX    (USB_BLKMAX)

/* Output debugging message in a console of IDE. */

// #define USB_DEBUG_OUTPUT

#ifdef USB_DEBUG_OUTPUT
 #include <stdlib.h>
 #include <stdio.h>

 #define USB_PRINTF0(FORM)                                    (printf(FORM))
 #define USB_PRINTF1(FORM, x1)                                (printf(FORM, x1))
 #define USB_PRINTF2(FORM, x1, x2)                            (printf(FORM, x1, x2))
 #define USB_PRINTF3(FORM, x1, x2, x3)                        (printf(FORM, x1, x2, x3))
 #define USB_PRINTF4(FORM, x1, x2, x3, x4)                    (printf(FORM, x1, x2, x3, x4))
 #define USB_PRINTF5(FORM, x1, x2, x3, x4, x5)                (printf(FORM, x1, x2, x3, x4, x5))
 #define USB_PRINTF6(FORM, x1, x2, x3, x4, x5, x6)            (printf(FORM, x1, x2, x3, x4, x5, x6))
 #define USB_PRINTF7(FORM, x1, x2, x3, x4, x5, x6, x7)        (printf(FORM, x1, x2, x3, x4, x5, x6, x7))
 #define USB_PRINTF8(FORM, x1, x2, x3, x4, x5, x6, x7, x8)    (printf(FORM, x1, x2, x3, x4, x5, x6, x7, x8))
#else
 #define USB_PRINTF0(FORM)
 #define USB_PRINTF1(FORM, x1)
 #define USB_PRINTF2(FORM, x1, x2)
 #define USB_PRINTF3(FORM, x1, x2, x3)
 #define USB_PRINTF4(FORM, x1, x2, x3, x4)
 #define USB_PRINTF5(FORM, x1, x2, x3, x4, x5)
 #define USB_PRINTF6(FORM, x1, x2, x3, x4, x5, x6)
 #define USB_PRINTF7(FORM, x1, x2, x3, x4, x5, x6, x7)
 #define USB_PRINTF8(FORM, x1, x2, x3, x4, x5, x6, x7, x8)
#endif

/***************************************(*******)*************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* PIPE Information Structure */
typedef struct
{
    uint8_t  ifnum;                    /* Interface Number */
    uint8_t  epnum;                    /* Endpoint Number */
    uint8_t  direction;                /* Endpoint Direction */
    uint8_t  type;                     /* Endpoint Type */
    uint16_t mps;                      /* Endpoint Max Packet Size */
    uint16_t devaddr;                  /* Device Address */
} st_usb_pipe_t;

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/* r_usb_hDriver.c */
extern st_usb_pipe_t g_usb_hstd_pipe[USB_MAXPIPE + 1U]; /* pipe information */
extern uint16_t      g_usb_msg_check;

#if (BSP_CFG_RTOS == 2)

/* RTOS semaphore */
extern uint32_t g_usb_sem;
#endif

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/* r_usb_cScheduler.c */
usb_er_t usb_hstd_rec_msg(uint8_t id, usb_msg_t ** pp_mess);
usb_er_t usb_hstd_snd_msg(uint8_t id, usb_msg_t * p_mess);
usb_er_t usb_hstd_isnd_msg(uint8_t id, usb_msg_t * p_mess);
usb_er_t usb_hstd_wai_msg(uint8_t id, usb_msg_t * p_mess, uint16_t times);
void     usb_hstd_wait_scheduler(void);
usb_er_t usb_hstd_pget_blk(uint8_t id, st_usb_utr_t ** pp_blk);
usb_er_t usb_hstd_rel_blk(uint8_t id, st_usb_utr_t * p_blk);
void     usb_hstd_sche_init(void);

/* r_usb_hDriver.c */
uint16_t usb_hstd_get_pipe_type(uint16_t pipe_id);
void     usb_hstd_set_rootport(uint16_t devaddr, uint16_t rootport);
uint16_t usb_hstd_get_rootport(uint16_t devaddr);
void     usb_hstd_a_or_detach_cb(usb_utr_t * p_utr);
void     usb_hstd_port_reset_cb(usb_utr_t * p_utr);
void     usb_hstd_over_current_cb(void * p_utr);
void     usb_hstd_transfer_end_cb(usb_utr_t * ptr, void * p_utr, uint32_t actual_size, uint16_t status);
uint8_t  usb_hstd_convert_epnum(uint8_t pipe_id);

/* r_usb_hManager.c */

uint16_t usb_hstd_clear_feature(uint16_t addr, uint16_t epnum, usb_utr_cb_t complete);
usb_er_t usb_hstd_clear_stall(uint16_t pipe, usb_utr_cb_t complete);

#endif                                 /* USB_BASIC_LOCAL_H */
