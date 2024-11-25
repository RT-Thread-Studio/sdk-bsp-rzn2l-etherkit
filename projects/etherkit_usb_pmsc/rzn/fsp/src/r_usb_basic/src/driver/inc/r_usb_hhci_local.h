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
 * File Name    : r_usb_hhci_local.h
 * Version      : 1.0
 * Description  : HCI Local Common Header
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#ifndef R_USB_HHCILOCAL_H
#define R_USB_HHCILOCAL_H

#include <stdio.h>
#include <string.h>

// #include "iodefine.h"
#include "r_usb_basic.h"
#include "r_usb_hhci.h"
#include "r_usb_basic_define.h"
#include "r_usb_hehci_def_usr.h"
#include "r_usb_hehci_typedef.h"
#include "r_usb_hehci_extern.h"
#include "r_usb_hohci_def_usr.h"
#include "r_usb_hohci_typedef.h"
#include "r_usb_hohci_extern.h"

// #include "r_mmu_lld_rza2m.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

// #define USB_DEBUG_HCI

#ifndef NULL
 #define NULL     (0U)
#endif
#ifndef FALSE
 #define FALSE    (0U)
#endif
#ifndef TRUE
 #define TRUE     (1U)
#endif

#define USB_HCI_NONCACHE_ADDR(x)    (x)
#define USB_HCI_CACHE_ADDR(x)       (x | 0x02000000)

#define USB_PID          (0x0003U)     /* b1-0: Response PID */
#define USB_PID_STALL    (0x0002U)     /* STALL */
#define USB_PID_BUF      (0x0001U)     /* BUF */
#define USB_PID_NAK      (0x0000U)     /* NAK */

//= =======================================================================
// Definitions for HCI Common
//= =======================================================================

/* Type of HCI task */
typedef enum
{
    USB_HCI_TASK,
    USB_HCI_TASK_MAX
} e_usb_hci_task_enum_t;

/* Type of HCI task message */
typedef enum
{
    USB_HCI_MESS_EHCI_USBINT,                     /* EHCI Interrupt on complte message */
    USB_HCI_MESS_EHCI_USBERRINT,                  /* EHCI Error Interrupt message */
    USB_HCI_MESS_EHCI_PORT_CHANGE_DETECT,         /* EHCI Port Change Detect message */
    USB_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER,        /* EHCI Frame List Rollover message */
    USB_HCI_MESS_EHCI_HOST_SYSTEM_ERROR,          /* EHCI Host System Error message */
    USB_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE, /* EHCI Interrupt on Asnc Advance message */
    USB_HCI_MESS_OHCI_INT,                        /* OHCI Interrupt message */
    USB_HCI_MESS_OHCI_CONTROL_ROOTHUB,            /* OHCI Control Roothub message */
    USB_HCI_MESS_PORT_RESET_REQUEST,              /* Port Reset Request message */
    USB_HCI_MESS_MAKE_TRANSFER_REQUEST,           /* Transfer Request message */
    USB_HCI_MESS_DISCONNECT_REQUEST,              /* Disconnect Request message */
    USB_HCI_MESS_CANCEL_TRANSFER_REQUEST_S,       /* Cancel Transfer Request for Sytem task */
    USB_HCI_MESS_CANCEL_TRANSFER_REQUEST_T,       /* Cancel Transfer Request for Transfer task */
    USB_HCI_MESS_MAX
} e_usb_hci_mess_enum_t;

#ifdef USB_DEBUG_HCI
 #define USB_HCI_PRINTF0(FORM)               printf("\033[36m"FORM "\033[0m")
 #define USB_HCI_PRINTF1(FORM, x)            printf("\033[36m"FORM "\033[0m", x)
 #define USB_HCI_PRINTF2(FORM, x, y)         printf("\033[36m"FORM "\033[0m", x, y)
 #define USB_HCI_PRINTF3(FORM, x, y, z)      printf("\033[36m"FORM "\033[0m", x, y, z)
 #define USB_HCI_PRINTF0_R(FORM)             printf("\033[31m"FORM "\033[0m")
 #define USB_HCI_PRINTF1_R(FORM, x)          printf("\033[31m"FORM "\033[0m", x)
 #define USB_HCI_PRINTF2_R(FORM, x, y)       printf("\033[31m"FORM "\033[0m", x, y)
 #define USB_HCI_PRINTF3_R(FORM, x, y, z)    printf("\033[31m"FORM "\033[0m", x, y, z)
 #define USB_HCI_PRINTF0_G(FORM)             printf("\033[32m"FORM "\033[0m")
 #define USB_HCI_PRINTF1_G(FORM, x)          printf("\033[32m"FORM "\033[0m", x)
 #define USB_HCI_PRINTF2_G(FORM, x, y)       printf("\033[32m"FORM "\033[0m", x, y)
 #define USB_HCI_PRINTF3_G(FORM, x, y, z)    printf("\033[32m"FORM "\033[0m", x, y, z)
 #define USB_HCI_PRINTF0_Y(FORM)             printf("\033[33m"FORM "\033[0m")
 #define USB_HCI_PRINTF1_Y(FORM, x)          printf("\033[33m"FORM "\033[0m", x)
 #define USB_HCI_PRINTF2_Y(FORM, x, y)       printf("\033[33m"FORM "\033[0m", x, y)
 #define USB_HCI_PRINTF3_Y(FORM, x, y, z)    printf("\033[33m"FORM "\033[0m", x, y, z)
#else
 #define USB_HCI_PRINTF0(FORM)
 #define USB_HCI_PRINTF1(FORM, x)
 #define USB_HCI_PRINTF2(FORM, x, y)
 #define USB_HCI_PRINTF3(FORM, x, y, z)
 #define USB_HCI_PRINTF0_R(FORM)
 #define USB_HCI_PRINTF1_R(FORM, x)
 #define USB_HCI_PRINTF2_R(FORM, x, y)
 #define USB_HCI_PRINTF3_R(FORM, x, y, z)
 #define USB_HCI_PRINTF0_G(FORM)
 #define USB_HCI_PRINTF1_G(FORM, x)
 #define USB_HCI_PRINTF2_G(FORM, x, y)
 #define USB_HCI_PRINTF3_G(FORM, x, y, z)
 #define USB_HCI_PRINTF0_Y(FORM)
 #define USB_HCI_PRINTF1_Y(FORM, x)
 #define USB_HCI_PRINTF2_Y(FORM, x, y)
 #define USB_HCI_PRINTF3_Y(FORM, x, y, z)
#endif

#define USB_VAL_7FU8     (0x7FU)
#define USB_VAL_7FU32    (0x0000007FU)
#define USB_VAL_7FF      (0x000007FFU)
#define USB_VAL_7FFF     (0x00007FFFU)
#define USB_VAL_3FF      (0x000003FFU)
#define USB_VAL_3FFF     (0x00003FFFU)
#define USB_VAL_FFF      (0x00000FFFU)

/***************************************(*******)*************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef struct
{
    uint8_t blength;
    uint8_t bdescriptor_type;
} st_usb_hci_descriptor_header_t;

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern st_usb_hci_tr_req_t  g_usb_hstd_hci_transfer_request[USB_HCI_TRREQ_NUM];
extern st_usb_hci_cb_info_t g_usb_hstd_hci_callback;

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
void usb_hstd_hci_send_message_from_int(usb_utr_t * ptr,
                                        int32_t     task_no,
                                        int8_t      message,
                                        uint32_t    data1,
                                        uint32_t    data2,
                                        uint32_t    data3);
st_usb_hci_device_t * usb_hstd_hci_alloc_dev_info(void);
uint32_t              usb_hstd_hci_get_pollrate(st_usb_hci_tr_req_t * tr_req);
void                  usb_hstd_hci_lock_resouce(void);
void                  usb_hstd_hci_unlock_resouce(void);
void                  R_MMU_VAtoPA(uint32_t vaddress, uint32_t * paddress);
uint32_t              r_usb_pa_to_va(uint32_t paddr);

#endif                                 /* R_USB_HHCILOCAL_H */
