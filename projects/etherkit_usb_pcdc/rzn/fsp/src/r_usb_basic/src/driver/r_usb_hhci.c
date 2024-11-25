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
 * File Name    : r_usb_hHci.c
 * Description  : HCI Common Code
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include "r_usb_hhci_local.h"
#include "r_usb_extern.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define USB_HCI_MESS_MAX         (32U) /* Maximum number of HCI message buffers */
/* For RIIC */
#define TUSB320LI_DEV_ADDRESS    (0x47U << 1)

#define USB_VAL_0200             (0x0200U)
#define USB_VAL_X40              (0x40U)
#define USB_VAL_X08              (0x08U)
#define USB_VAL_XC0              (0x000000C0U)
#define USB_VAL_XFFCF            (0xFFFFFFCFU)
#define USB_VAL_X0080            (0x00000080U)
#define USB_VAL_XFF              (0xFFU)
#define USB_VAL_66U              (66U)

#define USB_VAL_LSDA             (0x00000200U)
#define USB_VAL_POCI             (0x00000008U)
#define USB_VAL_PO1              (0x00002000U)
#define USB_VAL_PO0              (0xFFFFDFFFU)
#define USB_VAL_POCI_1           (0x00000100U)

#define USB_VAL_PPS              (0x00000001U)
#define USB_VAL_LPSC             (0x00010000U)

#define USB_VAL_NPS_A            (0xFFFFFDFFU)
#define USB_VAL_NPS_B            (0x00000200U)

#if  USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * External variables and functions
 ***********************************************************************************************************************/
extern void usb_hstd_int_enable(void);
extern void usb_hstd_int_disable(void);

 #ifdef USB_HOST_COMPLIANCE_MODE
extern void usb_hstd_electrical_test_mode(uint16_t product_id, uint16_t port);

 #endif                                /* USB_HOST_COMPLIANCE_MODE */

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/
typedef void (* USB_HCI_TASK_FUNC)(void);

typedef struct
{
    int16_t  sem_id;
    int16_t  tsk_id;
    uint32_t mess_write;
    uint32_t mess_read;
    uint32_t mess_cnt;
    uint32_t data1_buff[USB_HCI_MESS_MAX];
    uint32_t data2_buff[USB_HCI_MESS_MAX];
    uint32_t data3_buff[USB_HCI_MESS_MAX];
    int8_t   mess_buff[USB_HCI_MESS_MAX];
} st_usb_hci_task_info_t;

typedef struct
{
    st_usb_hci_task_info_t task[USB_HCI_TASK_MAX];
} st_usb_hci_manage_info_t;

void usb_hstd_hci_interrupt_handler(void);

/* global */
st_usb_hci_cb_info_t g_usb_hstd_hci_callback;

/***********************************************************************************************************************
 * Static variables and functions
 ***********************************************************************************************************************/
static void                               usb_hstd_hci_free_dev_info(usb_utr_t * ptr, uint32_t devadd);
static st_usb_hci_tr_req_t              * usb_hstd_hci_alloc_transefer_request(void);
static void                               usb_hstd_hci_set_disconnect_flag(uint32_t devadd);
static st_usb_hci_endpoint_descriptor_t * usb_hstd_hci_search_endpoint_descriptor(st_usb_hci_tr_req_t * tr_req);

static void usb_hub_diconnect_delay(void);

 #if 0
static void     usb_hstd_hci_deinit(usb_utr_t * ptr);
static uint16_t usb_hstd_hci_get_pid_status(uint16_t devaddr, uint16_t epnum);
static uint32_t usb_hstd_hci_get_port_current_connect_status(uint16_t rootport);

 #endif

 #ifdef RZA2_USB_OHCI_IP
static void r_usb_port_clean_seq(void);

 #endif

static st_usb_hci_manage_info_t usb_hci_manage_info;
static uint16_t                 usb_hci_tmp_addr;

uint16_t g_usb_msg_check;

/* save register */
uint32_t usb_hcrh_port_status1;
uint32_t usb_hcrh_descriptor_a;
uint32_t usb_hcrh_descriptor_b;
uint32_t usb_portsc1;

BSP_ALIGN_VARIABLE(4) static st_usb_hci_device_t usb_hci_device_info[USB_HCI_DEVICE_NUM] USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(4) st_usb_hci_tr_req_t g_usb_hstd_hci_transfer_request[USB_HCI_TRREQ_NUM] USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(4) static uint32_t usb_hci_setup_buffer[USB_HCI_TRREQ_NUM][USB_HCI_SETUP_DATA_SIZE /
                                                                              sizeof(uint32_t)]
USB_BUFFER_PLACE_IN_SECTION;

/***********************************************************************************************************************
 * Function     : HCI Task
 * Declaration  : void usb_hstd_hci_task(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_hci_task (void)
{
    usb_utr_t              * p_mess;
    usb_utr_t              * ptr;
    uint16_t                 rootport;
    uint16_t                 pipenum;
    usb_hciinfo_t          * hp;
    usb_er_t                 err;
    st_usb_hci_task_info_t * p_task_info;
    uint32_t                 message;
    uint32_t                 data1;
    uint32_t                 data2;
    uint32_t                 data3;
    st_usb_utr_t             dummy;

    memset((void *) &dummy, 0, sizeof(st_usb_utr_t));

    /* receive message */

 #if (BSP_CFG_RTOS == 0)
    err = USB_RCV_MSG(USB_HCI_MBX, (usb_msg_t **) &p_mess);
    if (err != USB_OK)
    {
        /* Nothing to do */
    }
    else
 #else                                 /* #if (BSP_CFG_RTOS == 2) */
    while (1)
    {
        err = USB_TRCV_MSG(USB_HCI_MBX, (usb_msg_t **) &p_mess, (usb_tm_t) 0);
        if (err != USB_OK)
        {
            /* Nothing to do */
        }
        else
 #endif                                /* #if (BSP_CFG_RTOS == 2) */
    {
        p_task_info = &usb_hci_manage_info.task[USB_HCI_TASK];

        /* Taking out of message */
        if (p_task_info->mess_cnt)
        {
            usb_hstd_int_disable();

            message = (uint32_t) p_task_info->mess_buff[p_task_info->mess_read];
            data1   = p_task_info->data1_buff[p_task_info->mess_read];
            data2   = p_task_info->data2_buff[p_task_info->mess_read];
            data3   = p_task_info->data3_buff[p_task_info->mess_read];

            ptr      = p_mess;
            hp       = (usb_hciinfo_t *) p_mess;
            rootport = ptr->keyword;
            pipenum  = ptr->keyword;

            p_task_info->mess_read++;
            if (USB_HCI_MESS_MAX <= p_task_info->mess_read)
            {
                p_task_info->mess_read = 0UL;
            }

            p_task_info->mess_cnt--;

            usb_hstd_int_enable();

            /* Processing of message */
            switch (message)
            {
                case USB_HCI_MESS_EHCI_PORT_CHANGE_DETECT:
                {
                    USB_HCI_PRINTF0("USB_HCI_MESS_EHCI_PORT_CHANGE_DETECT \n");
                    usb_hstd_ehci_int_port_change(ptr);
                    break;
                }

                case USB_HCI_MESS_EHCI_HOST_SYSTEM_ERROR:
                {
                    USB_HCI_PRINTF0_R("USB_HCI_MESS_EHCI_HOST_SYSTEM_ERROR \n");
                    break;
                }

                case USB_HCI_MESS_EHCI_USBINT:
                {
                    USB_HCI_PRINTF0("USB_HCI_MESS_EHCI_USBINT \n");
                    usb_hstd_ehci_int_transfer_end(ptr);
                    break;
                }

                case USB_HCI_MESS_EHCI_USBERRINT:
                {
                    USB_HCI_PRINTF0_R("USB_HCI_MESS_EHCI_USBERRINT \n");
                    usb_hstd_ehci_int_transfer_end(ptr);
                    break;
                }

                case USB_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER:
                {
                    USB_HCI_PRINTF0("USB_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER \n");
                    break;
                }

                case USB_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE:
                {
                    USB_HCI_PRINTF0("USB_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE \n");
                    break;
                }

                case USB_HCI_MESS_OHCI_INT:
                {
                    USB_HCI_PRINTF3("USB_HCI_MESS_OHCI_INT : %08x %08x %08x\n", data1, data2, data3);
                    usb_hstd_OhciMainRoutine(ptr, data1, data2, data3);
                    break;
                }

                default:
                {
                    USB_HCI_PRINTF0_R("Error: HCI transfer task unknown message \n");
                    break;
                }
            }
        }
    }

    FSP_PARAMETER_NOT_USED(rootport);
    FSP_PARAMETER_NOT_USED(hp);
    FSP_PARAMETER_NOT_USED(pipenum);
    FSP_PARAMETER_NOT_USED(dummy);

 #if (BSP_CFG_RTOS != 0)
}
 #endif                                /* #if (BSP_CFG_RTOS == 0) */
}                                      /* End of function usb_hstd_hci_task() */

/***********************************************************************************************************************
 * Function     : Interrupt Routine for HCI
 * Declaration  : void usb_hstd_hci_interrupt_handler(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_hci_interrupt_handler (void)
{
    usb_utr_t * ptr;
 #if 0
    usb_er_t    err;
    IRQn_Type   irq;
    usb_cfg_t * p_cfg;
 #endif

    /* Initial pointer */
    ptr     = &g_usb_cstd_int_msg[0][g_usb_cstd_int_msg_cnt[0]];
    ptr->ip = USB_IP0;

    g_usb_msg_check |= (1 << USB_INT);

    if (USB00->HCINTERRUPTSTATUS_b.RHSC && USB00->HCINTERRUPTENABLE_b.RHSCE)
    {
        usb_hcrh_descriptor_a = USB00->HCRHDESCRIPTORA;
        usb_hcrh_descriptor_b = USB00->HCRHDESCRIPTORB;
        usb_portsc1           = USB00->PORTSC1;
        usb_hcrh_port_status1 = USB00->HCRHPORTSTATUS1;

        if (0 == USB00->HCRHPORTSTATUS1_b.CCS)
        {
 #ifdef RZA2_USB_OHCI_IP
            r_usb_port_clean_seq();
 #endif                                /* RZA2_USB_OHCI_IP */
            /* R_USB_HstdDelayXms((uint16_t)30); */
            usb_hub_diconnect_delay();
        }
        else
        {
        }
    }
    else if (USB00->USBSTS_b.PTCGDET && USB00->USBINTR_b.PTCGIE)
    {
        if (0 == USB00->HCRHPORTSTATUS1_b.CCS)
        {
            /* R_USB_HstdDelayXms((uint16_t)30); */
            usb_hub_diconnect_delay();
        }
        else
        {
        }
    }
    else
    {
    }

    /* Check ohci */
    usb_hstd_ohci_interrupt_handler(ptr);

    /* Check ehci */
    usb_hstd_ehci_interrupt_handler(ptr);

    /* Renewal Message count  */
    g_usb_cstd_int_msg_cnt[0]++;
    if (USB_INTMSGMAX == g_usb_cstd_int_msg_cnt[0])
    {
        g_usb_cstd_int_msg_cnt[0] = 0;
    }

    g_usb_msg_check &= (uint16_t) (~(1 << USB_INT));
}                                      /* End of function usb_hstd_hci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : Message sending from interrupt
 * Declaration  : void usb_hstd_hci_send_message_from_int( int32_t task_no, int8_t message, uint32_t data )
 * Argument     : int32_t task_no ; task no
 *              : int8_t message  ; message type
 *              : void *data      ; data pointer
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_hci_send_message_from_int (usb_utr_t * ptr,
                                         int32_t     task_no,
                                         int8_t      message,
                                         uint32_t    data1,
                                         uint32_t    data2,
                                         uint32_t    data3)
{
    st_usb_hci_task_info_t * p_task_info;

    p_task_info = &usb_hci_manage_info.task[task_no];

    ptr->keyword = (uint16_t) message;

    /* Registration of message */
    p_task_info->mess_buff[p_task_info->mess_write]  = message;
    p_task_info->data1_buff[p_task_info->mess_write] = data1;
    p_task_info->data2_buff[p_task_info->mess_write] = data2;
    p_task_info->data3_buff[p_task_info->mess_write] = data3;
    p_task_info->mess_write++;

    if (USB_HCI_MESS_MAX <= p_task_info->mess_write)
    {
        p_task_info->mess_write = 0UL;
    }

    p_task_info->mess_cnt++;

    if (p_task_info->mess_cnt > USB_HCI_MESS_MAX)
    {
        /* Message buffer overflow  */
        USB_HCI_PRINTF1_R("Error: HCI send message buffer overflow( task_no:%d ) \n", task_no);
    }

    /* Notification of message sending */

/* USB_SND_MSG(USB_HCI_MBX, (usb_msg_t *)NULL); */
 #if (BSP_CFG_RTOS == 0)
    USB_SND_MSG(USB_HCI_MBX, (usb_msg_t *) ptr);
 #else
    USB_ISND_MSG(USB_HCI_MBX, (usb_msg_t *) ptr);
 #endif
}                                      /* End of function usb_hstd_hci_send_message_from_int() */

/***********************************************************************************************************************
 * Function     : Allocate Device-Information-Memory
 * Declaration  : st_usb_hci_device_t *usb_hstd_hci_alloc_dev_info(void)
 * Argument     : void
 * Return       : st_usb_hci_device_t * ; Device-Information-Memory pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_hci_device_t * usb_hstd_hci_alloc_dev_info (void)
{
    st_usb_hci_device_t * p_ret = NULL;
    st_usb_hci_device_t * p_devinfo;
    uint32_t              i;

    usb_hstd_hci_lock_resouce();

    p_devinfo = &usb_hci_device_info[0];
    for (i = 0; i < USB_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (FALSE == p_devinfo->bit.enable)
        {
            memset(p_devinfo, 0, sizeof(st_usb_hci_device_t));
            p_devinfo->bit.enable = TRUE;
            p_ret                 = p_devinfo;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    if (NULL == p_ret)
    {
        USB_HCI_PRINTF0_R("--------------------------------------------------\n");
        USB_HCI_PRINTF0_R("Memory Allocation Error: usb_hstd_hci_alloc_dev_info\n");
        USB_HCI_PRINTF0_R("--------------------------------------------------\n");
    }

    return p_ret;
}                                      /* End of function usb_hstd_hci_alloc_dev_info() */

/***********************************************************************************************************************
 * Function     : Free Device-Information-Memory
 * Declaration  : void usb_hstd_hci_free_dev_info(uint32_t devadd)
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         : When the device is HUB, the disconnection process of the down port is executed.
 ***********************************************************************************************************************/
void usb_hstd_hci_free_dev_info (usb_utr_t * ptr, uint32_t devadd)
{
    st_usb_hci_device_t * p_devinfo;
    uint32_t              n;
    uint32_t              i;

    p_devinfo = &usb_hci_device_info[0];
    for (i = 0; i < USB_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (p_devinfo->bit.enable == TRUE)
        {
            if (p_devinfo->bit.devnum == devadd)
            {
                /* When the device is HUB, the disconnection process of the down port is executed.  */
                for (n = 0; n < USB_HCI_HUBPORT_NUM; n++)
                {
                    if (p_devinfo->hubdevinfo.devnum[n] != 0)
                    {
                        r_usb_hstd_hci_disconnect(ptr, p_devinfo->hubdevinfo.devnum[n]);
                    }
                }

                usb_hstd_hci_lock_resouce();
                p_devinfo->bit.enable = FALSE; /* clear device info */
                usb_hstd_hci_unlock_resouce();
            }
            else
            {
                /* The HUB registered information is deleted for the device connected with HUB. */
                for (n = 0; n < USB_HCI_HUBPORT_NUM; n++)
                {
                    if (p_devinfo->hubdevinfo.devnum[n] == devadd)
                    {
                        p_devinfo->hubdevinfo.devnum[n]   = 0;
                        p_devinfo->hubdevinfo.devspeed[n] = 0;
                    }
                }
            }
        }
    }
}                                      /* End of function usb_hstd_hci_free_dev_info() */

/***********************************************************************************************************************
 * Function     : Getting of Pollrate
 * Declaration  : uint32_t usb_hstd_hci_get_pollrate(st_usb_hci_tr_req_t *tr_req)
 * Argument     : st_usb_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : uint32_t               ; pollrate( uFrame unit )
 * Note         :
 ***********************************************************************************************************************/
uint32_t usb_hstd_hci_get_pollrate (st_usb_hci_tr_req_t * p_tr_req)
{
    st_usb_hci_endpoint_descriptor_t * p_ep_desc;
    uint32_t ep_interbal;
    uint32_t pollrate = 0;

    p_ep_desc = usb_hstd_hci_search_endpoint_descriptor(p_tr_req);
    if (NULL != p_ep_desc)
    {
        ep_interbal = p_ep_desc->bInterval;
        switch (p_tr_req->bit.eptype)
        {
            case USB_EP_INT:
            {
                if (USB_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
                {
                    pollrate = 1;
                    for (ep_interbal--; ep_interbal != 0; ep_interbal--)
                    {
                        pollrate *= 2;
                    }
                }
                else
                {
                    pollrate = ep_interbal * 8;
                }

                break;
            }

            case USB_EP_ISO:
            {
                if (USB_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
                {
                    pollrate = 1;
                    for (ep_interbal--; ep_interbal != 0; ep_interbal--)
                    {
                        pollrate *= 2;
                    }
                }
                else
                {
                    if (USB_VAL_0200 == p_tr_req->devinfo->devicedesc.bcd_usb)
                    {
                        pollrate = 1;
                        for (ep_interbal--; ep_interbal != 0; ep_interbal--)
                        {
                            pollrate *= 2;
                        }

                        pollrate *= 8;
                    }
                    else
                    {
                        pollrate = 8;
                    }
                }

                break;
            }

            default:
            {
                break;
            }
        }
    }

    return pollrate;
}                                      /* End of function usb_hstd_hci_get_pollrate() */

/***********************************************************************************************************************
 * Function     : HCI Lock resource
 * Declaration  : void usb_hstd_hci_lock_resouce(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_hci_lock_resouce (void)
{
}                                      /* End of function usb_hstd_hci_lock_resouce() */

/***********************************************************************************************************************
 * Function     : HCI Lock resource
 * Declaration  : void usb_hstd_hci_unlock_resouce( void )
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_hci_unlock_resouce (void)
{
}                                      /* End of function usb_hstd_hci_unlock_resouce() */

/***********************************************************************************************************************
 * Function     : Device Disconnect Flag Set
 * Declaration  : void usb_hstd_hci_set_disconnect_flag(uint32_t devadd)
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_hci_set_disconnect_flag (uint32_t devadd)
{
    st_usb_hci_device_t * p_devinfo;
    uint32_t              n;

    p_devinfo = r_usb_hstd_hci_get_device_infomation((uint16_t) devadd);

    /* Disconnect Request Flag Set */
    p_devinfo->bit.disconnect_req = TRUE;

    /* Hub Port Device Disconnect */
    for (n = 0; n < USB_HCI_HUBPORT_NUM; n++)
    {
        if (p_devinfo->hubdevinfo.devnum[n] != 0)
        {
            usb_hstd_hci_set_disconnect_flag(p_devinfo->hubdevinfo.devnum[n]);
        }
    }
}                                      /* End of function usb_hstd_hci_set_disconnect_flag() */

/***********************************************************************************************************************
 * Function     : Searching of endpoint descriptor address
 * Declaration  : st_usb_hci_endpoint_descriptor_t* usb_hstd_hci_search_endpoint_descriptor(st_usb_hci_tr_req_t *tr_req)
 * Argument     : st_usb_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : st_usb_hci_endpoint_descriptor_t * ; endpoint descriptor pointer
 * Note         :
 ***********************************************************************************************************************/
static st_usb_hci_endpoint_descriptor_t * usb_hstd_hci_search_endpoint_descriptor (st_usb_hci_tr_req_t * p_tr_req)
{
    st_usb_hci_endpoint_descriptor_t * p_epdesc;
    st_usb_hci_descriptor_header_t   * p_desc_head;
    st_usb_hci_device_t              * p_devinfo;
    uint32_t n;
    uint8_t  ep_direction;

    n         = 0;
    p_devinfo = p_tr_req->devinfo;

    if (USB_HCI_DIRECTION_IN == p_tr_req->bit.direction)
    {
        ep_direction = USB_EP_IN;
    }
    else
    {
        ep_direction = USB_EP_OUT;
    }

    while (n < USB_HCI_CONFIGDESC_BUFFSIZE)
    {
        p_desc_head = (st_usb_hci_descriptor_header_t *) ((uint8_t *) &p_devinfo->rawconfigdesc[0] + n);
        if (USB_DT_ENDPOINT == p_desc_head->bdescriptor_type)
        {
            p_epdesc = (st_usb_hci_endpoint_descriptor_t *) p_desc_head;
            if (((p_epdesc->bmAttributes & USB_EP_TRNSMASK) == p_tr_req->bit.eptype) &&
                ((p_epdesc->bEndpointAddress & USB_EP_DIRMASK) == ep_direction) &&
                ((p_epdesc->bEndpointAddress & USB_EP_NUMMASK) == p_tr_req->bit.epnum))
            {
                return p_epdesc;
            }
        }

        n += p_desc_head->blength;
    }

    return NULL;
}                                      /* End of function st_usb_hci_endpoint_descriptor_t() */

/***********************************************************************************************************************
 * Function     : Allocate Transfer-Request-Memory
 * Declaration  : st_usb_hci_tr_req_t* usb_hstd_hci_alloc_transefer_request( void )
 * Argument     : void
 * Return       : st_usb_hci_tr_req_t * ; Transfer-Request-Memory pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_hci_tr_req_t * usb_hstd_hci_alloc_transefer_request (void)
{
    st_usb_hci_tr_req_t * p_ret = NULL;
    st_usb_hci_tr_req_t * p_tr_req;
    uint32_t              i;

    usb_hstd_hci_lock_resouce();

    p_tr_req = &g_usb_hstd_hci_transfer_request[0];
    for (i = 0; i < USB_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (p_tr_req->bit.enable == FALSE)
        {
            memset(p_tr_req, 0, sizeof(st_usb_hci_tr_req_t));

            p_tr_req->bit.enable = TRUE;
 #if 0
            R_MMU_VAtoPA((uint32_t) &usb_hci_setup_buffer[i][0], (uint32_t *) &p_tr_req->setupbuf);
 #else
            p_tr_req->setupbuf = &usb_hci_setup_buffer[i][0];
 #endif

            p_ret = p_tr_req;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    if (NULL == p_ret)
    {
        USB_HCI_PRINTF0_R("-----------------------------------------------------------\n");
        USB_HCI_PRINTF0_R("Memory Allocation Error: usb_hstd_hci_alloc_transefer_request\n");
        USB_HCI_PRINTF0_R("-----------------------------------------------------------\n");
    }

    return p_ret;
}                                      /* End of function usb_hstd_hci_alloc_transefer_request() */

/* API function */

/***********************************************************************************************************************
 * Function     : Initialize HCI
 * Declaration  : int32_t r_usb_hstd_hci_init(st_usb_hci_cb_info_t *cb_info)
 * Argument     : st_usb_hci_cb_info_t *cb_info ; callback function infomation
 * Return       : uint32_t                  ; status
 * Note         :
 ***********************************************************************************************************************/
int32_t r_usb_hstd_hci_init (st_usb_hci_cb_info_t * p_cb_info)
{
    uint32_t err;

    if (NULL == p_cb_info)
    {
        return USB_ERROR;
    }

    if ((NULL == p_cb_info->attach_cb) ||
        (NULL == p_cb_info->detach_cb) ||
        (NULL == p_cb_info->over_current_cb) ||
        (NULL == p_cb_info->tr_end_cb))
    {
        return USB_ERROR;
    }

    /* set callback function infomation */
    g_usb_hstd_hci_callback = *p_cb_info;

    /* HCI management memory initialization */
    memset(&usb_hci_manage_info, 0, sizeof(usb_hci_manage_info));

    usb_hstd_hci_lock_resouce();

    /* Initialize Transfer-Request Array */
    memset(&g_usb_hstd_hci_transfer_request[0], 0, sizeof(g_usb_hstd_hci_transfer_request));

    /* Initialize Device-Info Array */
    memset(&usb_hci_device_info[0], 0, sizeof(usb_hci_device_info));

    usb_hstd_hci_unlock_resouce();

    /* init ehci */
    err = usb_hstd_ehci_init();
    if (USB_OK != err)
    {
        return USB_ERROR;
    }

    /* init ohci */
    err = usb_hstd_ohci_init();
    if (USB_OK != err)
    {
        return USB_ERROR;
    }

    return USB_OK;
}                                      /* End of function r_usb_hstd_hci_init() */

/***********************************************************************************************************************
 * Function     : Making of Transfer Request
 * Declaration  : void usb_hstd_HciMakeTransferRequest(st_usb_hci_tr_req_t *tr_req)
 * Argument     : st_usb_hci_tr_req_t *tr_req ; transfer request infomation
 * Return       : uint16_t
 * Note         :
 ***********************************************************************************************************************/
uint16_t r_usb_hstd_hci_make_transfer_request (void   * p_utr,
                                               void   * p_setup,
                                               uint32_t devaddr,
                                               uint32_t epnum,
                                               uint32_t eptype,
                                               uint32_t epdir,
                                               uint32_t tranlen,
                                               uint32_t tranadr,
                                               uint32_t mps)
{
    st_usb_hci_tr_req_t * p_tr_req;
    uint8_t             * p_dst;
    uint8_t             * p_src;
    uint32_t              i;

    usb_utr_t * ptr = 0;               /* 0.9 do later */

    p_tr_req = usb_hstd_hci_alloc_transefer_request();
    if (NULL == p_tr_req)
    {
        return USB_ERROR;
    }

    /* Device Address */
    p_tr_req->bit.devadrs = devaddr & USB_VAL_7FU32;

    /* Get Device Information */
    if (0 == devaddr)
    {
        devaddr = usb_hci_tmp_addr;
    }

    p_tr_req->devinfo = r_usb_hstd_hci_get_device_infomation((uint16_t) devaddr);

    /* Check Devinfo Address */
    if (NULL == p_tr_req->devinfo)
    {
        p_tr_req->bit.enable = FALSE;
        USB_HCI_PRINTF0_R("-------------\n");
        USB_HCI_PRINTF0_R(" Devinfo NULL\n");
        USB_HCI_PRINTF0_R("-------------\n");

        return USB_ERROR;
    }

    /* Check Connection */
    if (TRUE == p_tr_req->devinfo->bit.disconnect_req)
    {
        p_tr_req->bit.enable = FALSE;
        USB_HCI_PRINTF0_R("-------------\n");
        USB_HCI_PRINTF0_R(" Disconnected\n");
        USB_HCI_PRINTF0_R("-------------\n");

        return USB_ERROR;
    }

    /* Endpoint Type */
    p_tr_req->bit.eptype = eptype & 0x00000003U;

    /* Direction */
    if (USB_EP_OUT == epdir)
    {
        p_tr_req->bit.direction = USB_HCI_DIRECTION_OUT;
    }
    else
    {
        p_tr_req->bit.direction = USB_HCI_DIRECTION_IN;
    }

    p_tr_req->bit.epnum = epnum & 0x0000000FU;            /* Endpoint Number */
    p_tr_req->trsize    = tranlen;                        /* Transfer Size */
 #if 0
    R_MMU_VAtoPA((uint32_t) tranadr, &p_tr_req->databuf); /* Transfer Data Buffer */
 #else
    p_tr_req->databuf = tranadr;                          /* Transfer Data Buffer */
 #endif
    p_tr_req->utr_p = p_utr;                              /* Set UTR Pointer */

    /* Control Transfer */
    if (USB_EP_CNTRL == eptype)
    {
        /* Setup Buffer */
        /* Because the data format is different, location is converted. */
 #if 0
        p_dst = (uint8_t *) r_usb_pa_to_va((uint32_t) p_tr_req->setupbuf);
        p_src = (uint8_t *) r_usb_pa_to_va((uint32_t) p_setup);
 #else
        p_dst = (uint8_t *) p_tr_req->setupbuf;
        p_src = (uint8_t *) p_setup;
 #endif
        for (i = 0; i < USB_HCI_SETUP_DATA_SIZE; i++)
        {
            *p_dst++ = *p_src++;
        }

        /* MaxPacket Size */
        if (0 == p_tr_req->devinfo->devicedesc.bmax_packet_size_0)
        {
            if (USB_HCI_DEVSPEED_LS == p_tr_req->devinfo->bit.devspeed)
            {
                p_tr_req->bit.mps = USB_VAL_X08;
            }
            else
            {
                p_tr_req->bit.mps = USB_VAL_X40;
            }
        }
        else
        {
            p_tr_req->bit.mps = p_tr_req->devinfo->devicedesc.bmax_packet_size_0;
        }
    }
    /* For Bulk, Interrupt and  Isochronous Transfer */
    else if ((eptype == USB_EP_ISO) || (eptype == USB_EP_BULK) || (eptype == USB_EP_INT))
    {
        p_tr_req->bit.mps = mps & USB_VAL_7FF;
    }
    else
    {
    }

    switch (p_tr_req->devinfo->bit.hctype)
    {
        case USB_HCI_HCTYPE_EHCI:
        {
            usb_hstd_ehci_make_transfer_request(p_tr_req);
            break;
        }

        case USB_HCI_HCTYPE_OHCI:
        {
            usb_hstd_ohci_make_transfer_request(ptr, p_tr_req);
            break;
        }

        default:
        {
            break;
        }
    }

    return USB_OK;
}                                      /* End of function r_usb_hstd_hci_make_transfer_request() */

/***********************************************************************************************************************
 * Function     : Message sending of transfer request
 * Declaration  : void usb_hstd_HciSendCancelTransferRequest(uint32_t devadrs, uint32_t epnum, uint32_t direction)
 * Argument     : uint32_t devadrs   ; device address
 *              : uint32_t epnum     ; endpoint no
 *              : uint32_t direction ; direction
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_cancel_transfer_request (usb_utr_t * ptr,
                                             uint32_t    devaddr,
                                             uint32_t    epnum,
                                             uint32_t    eptype,
                                             uint32_t    epdir)
{
    st_usb_hci_tr_req_t * p_tr_req;
    uint32_t              i;

    p_tr_req = &g_usb_hstd_hci_transfer_request[0];
    for (i = 0; i < USB_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (p_tr_req->bit.enable == TRUE)
        {
            if (epdir == USB_EP_OUT)
            {
                epdir = USB_HCI_DIRECTION_OUT;
            }
            else
            {
                epdir = USB_HCI_DIRECTION_IN;
            }

            if ((p_tr_req->bit.devadrs == devaddr) && (p_tr_req->bit.epnum == epnum) &&
                (p_tr_req->bit.direction == epdir) && (p_tr_req->bit.eptype == eptype))
            {
                if (NULL != p_tr_req->hci_info)
                {
                    p_tr_req->devinfo = r_usb_hstd_hci_get_device_infomation((uint16_t) devaddr);
                    if (USB_HCI_HCTYPE_EHCI == p_tr_req->devinfo->bit.hctype)
                    {
                        usb_hstd_ehci_cancel_transfer_request(p_tr_req);
                    }
                    else if (USB_HCI_HCTYPE_OHCI == p_tr_req->devinfo->bit.hctype)
                    {
                        usb_hstd_ohci_cancel_transfer_request(ptr, p_tr_req);
                    }
                    else
                    {
                    }
                }

                return;
            }
        }
    }
}                                      /* End of function r_usb_hstd_hci_cancel_transfer_request() */

/***********************************************************************************************************************
 * Function     : Device Disconnect
 * Declaration  : void r_usb_hstd_hci_disconnect(uint32_t devadd)
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_disconnect (usb_utr_t * ptr, uint32_t devadd)
{
    st_usb_hci_device_t * p_devinfo;

    p_devinfo = r_usb_hstd_hci_get_device_infomation((uint16_t) devadd);
    if (NULL != p_devinfo)
    {
        /* Disconnect Request Flag Set */
        usb_hstd_hci_set_disconnect_flag(devadd);

        switch (p_devinfo->bit.hctype)
        {
            case USB_HCI_HCTYPE_EHCI:
            {
                usb_hstd_ehci_disconnect(devadd);
                usb_hstd_hci_free_dev_info(ptr, devadd);
                break;
            }

            case USB_HCI_HCTYPE_OHCI:
            {
                USB00->HCCONTROL = USB00->HCCONTROL | (USB_VAL_XC0 & USB_VAL_XFFCF);

                usb_hstd_ohci_disconnect(ptr, 0);
                usb_hstd_hci_free_dev_info(ptr, 0);

                usb_hstd_ohci_disconnect(ptr, devadd);
                usb_hstd_hci_free_dev_info(ptr, devadd);

                USB00->HCCONTROL = (USB00->HCCONTROL & ~USB_VAL_XC0) | USB_VAL_X0080;
                break;
            }

            default:
            {
                break;
            }
        }
    }
}                                      /* End of function r_usb_hstd_hci_disconnect() */

/***********************************************************************************************************************
 * Function     : Getting of device address of rootpoot
 * Declaration  : uint16_t r_usb_hstd_hci_get_device_address_of_rootpoot(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint16_t          ; device address
 * Note         :
 ***********************************************************************************************************************/
uint16_t r_usb_hstd_hci_get_device_address_of_rootpoot (uint16_t rootport)
{
    uint16_t devaddr;

    devaddr = usb_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USB_HCI_NODEVICE == devaddr)
    {
        devaddr = usb_hstd_ohci_get_device_address_of_rootpoot(rootport);
    }

    return devaddr;
}                                      /* End of function r_usb_hstd_hci_get_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : The device address of the route port is set.
 * Declaration  : uint16_t r_usb_hstd_hci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t devaddr)
 * Argument     : uint16_t rootport ; rootport no
 *              : uint16_t devaddr  ; device address
 * Return       : uint16_t          ; device address
 * Note         :
 ***********************************************************************************************************************/
uint16_t r_usb_hstd_hci_set_device_address_of_rootpoot (uint16_t rootport, uint16_t devaddr)
{
    uint16_t ret_devaddr;

    usb_hci_tmp_addr = devaddr;

    ret_devaddr = usb_hstd_ehci_set_device_address_of_rootpoot(rootport, devaddr);
    if (USB_HCI_NODEVICE == ret_devaddr)
    {
        ret_devaddr = usb_hstd_ohci_set_device_address_of_rootpoot(rootport, devaddr);
    }

    return ret_devaddr;
}                                      /* End of function r_usb_hstd_hci_set_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : Getting of device speed
 * Declaration  : uint32_t r_usb_hstd_hci_get_device_speed(uint16_t devaddr)
 * Argument     : uint16_t devaddr  ; device address
 * Return       : uint16_t          ; device speed
 * Note         :
 ***********************************************************************************************************************/
uint32_t r_usb_hstd_hci_get_device_speed (uint16_t devaddr)
{
    st_usb_hci_device_t * devinfo;
    uint32_t              i;

    devinfo = &usb_hci_device_info[0];
    for (i = 0; i < USB_HCI_DEVICE_NUM; i++, devinfo++)
    {
        if (TRUE == devinfo->bit.enable)
        {
            if (devinfo->bit.devnum == devaddr)
            {
                return devinfo->bit.devspeed;
            }
        }
    }

    return USB_VAL_XFF;                /* Error */
} /* End of function r_usb_hstd_hci_get_device_speed() */

/***********************************************************************************************************************
 * Function     : Getting of device information
 * Declaration  : st_usb_hci_device_t *r_usb_hstd_hci_get_device_infomation(uint16_t devaddr)
 * Argument     : uint16_t devaddr  ; device address
 * Return       : st_usb_hci_device_t *  ; device information
 * Note         :
 ***********************************************************************************************************************/
st_usb_hci_device_t * r_usb_hstd_hci_get_device_infomation (uint16_t devaddr)
{
    st_usb_hci_device_t * p_devinfo;
    uint32_t              i;

    p_devinfo = &usb_hci_device_info[0];
    for (i = 0; i < USB_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (TRUE == p_devinfo->bit.enable)
        {
            if (p_devinfo->bit.devnum == devaddr)
            {
                return p_devinfo;
            }
        }
    }

    return NULL;                       /* Error */
} /* End of function r_usb_hstd_hci_get_device_infomation() */

/***********************************************************************************************************************
 * Function     : Port Reset
 * Declaration  : void usb_hstd_HciPortReset(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_port_reset (usb_utr_t * p_utr)
{
    uint16_t devaddr;

    devaddr = usb_hstd_ehci_get_device_address_of_rootpoot(p_utr->ip);
    if (USB_HCI_NODEVICE != devaddr)
    {
        usb_hstd_ehci_port_reset(p_utr->ip);
        g_usb_hstd_hci_callback.port_reset_cb(p_utr);
    }
    else
    {
        devaddr = usb_hstd_ohci_get_device_address_of_rootpoot(p_utr->ip);
        if (USB_HCI_NODEVICE != devaddr)
        {
            usb_hstd_ohci_port_reset(p_utr->ip);
            g_usb_hstd_hci_callback.port_reset_cb(p_utr);
        }
    }
}                                      /* End of function r_usb_hstd_hci_port_reset() */

/***********************************************************************************************************************
 * Function     : Port Suspend
 * Declaration  : void r_usb_hstd_hci_port_suspend(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_port_suspend (uint16_t rootport)
{
    uint16_t devaddr;

    devaddr = usb_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USB_HCI_NODEVICE != devaddr)
    {
        usb_hstd_ehci_port_suspend(rootport);
    }
    else
    {
        devaddr = usb_hstd_ohci_get_device_address_of_rootpoot(rootport);
        if (USB_HCI_NODEVICE != devaddr)
        {
            usb_hstd_ohci_port_suspend(rootport);
        }
    }
}                                      /* End of function r_usb_hstd_hci_port_suspend() */

/***********************************************************************************************************************
 * Function     : Port Resume
 * Declaration  : void r_usb_hstd_hci_port_resume(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_port_resume (uint16_t rootport)
{
    uint16_t devaddr;

    devaddr = usb_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USB_HCI_NODEVICE != devaddr)
    {
        usb_hstd_ehci_port_resume(rootport);
    }
    else
    {
        devaddr = usb_hstd_ohci_get_device_address_of_rootpoot(rootport);
        if (USB_HCI_NODEVICE != devaddr)
        {
            usb_hstd_ohci_port_resume(rootport);
        }
    }
}                                      /* End of function r_usb_hstd_hci_port_resume() */

/***********************************************************************************************************************
 * Function     : HUB information setting when new device is attached.
 * Declaration  : void r_usb_hstd_hci_set_hub_info(uint32_t hubaddr, uint32_t hubportnum, uint32_t devaddr, uint32_t devspeed)
 * Argument     : uint32_t hubaddr    ; HUB device address
 *              : uint32_t hubportnum ; HUB port no
 *              : uint32_t devaddr    ; new device address
 *              : uint32_t devspeed   ; new device speed
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_set_hub_info (uint32_t hubaddr, uint32_t hubportnum, uint32_t devaddr, uint32_t devspeed)
{
    st_usb_hci_device_t * p_new_devinfo;
    st_usb_hci_device_t * p_hub_devinfo;

    p_new_devinfo = usb_hstd_hci_alloc_dev_info();
    if (NULL != p_new_devinfo)
    {
        /* Getting of device information on connected HUB */
        p_hub_devinfo = r_usb_hstd_hci_get_device_infomation((uint16_t) hubaddr);
        if (NULL != p_hub_devinfo)
        {
            p_new_devinfo->bit.devnum   = devaddr & USB_VAL_7FU32;
            p_new_devinfo->bit.devspeed = devspeed & 0x00000003U;
            p_new_devinfo->bit.hctype   = p_hub_devinfo->bit.hctype;

            /* The split transaction is checked. */
            if (TRUE == p_hub_devinfo->bit.sprit_flag)
            {
                p_new_devinfo->bit.sprit_flag = TRUE;
                p_new_devinfo->bit.hubaddr    = p_hub_devinfo->bit.hubaddr;
                p_new_devinfo->bit.hubportnum = p_hub_devinfo->bit.hubportnum;
            }
            else
            {
                if ((USB_HCI_DEVSPEED_HS != devspeed) && (USB_HCI_DEVSPEED_HS == p_hub_devinfo->bit.devspeed))
                {
                    /* The split transaction is set. */
                    p_new_devinfo->bit.sprit_flag = TRUE;
                }
                else
                {
                    p_new_devinfo->bit.sprit_flag = FALSE;
                }

                p_new_devinfo->bit.hubaddr    = hubaddr & USB_VAL_7FU32;
                p_new_devinfo->bit.hubportnum = hubportnum & USB_VAL_7FU32;
            }

            /* Device information on HCI is succeeded. */
            p_new_devinfo->hci_device = p_hub_devinfo->hci_device;
        }
    }
}                                      /* End of function r_usb_hstd_hci_set_hub_info() */

/***********************************************************************************************************************
 * Function     : The device address of the HUB port is set.
 * Declaration  : void r_usb_hstd_hci_set_device_address_of_hub_port(uint16_t hubaddr, uint16_t devaddr)
 * Argument     : uint16_t hubaddr ; HUB device address
 *              : uint16_t devaddr ; HUB down port device address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void r_usb_hstd_hci_set_device_address_of_hub_port (uint16_t hubaddr, uint16_t devaddr)
{
    st_usb_hci_device_t * p_hub_devinfo;
    st_usb_hci_device_t * p_devinfo;
    uint32_t              cnt;

    /* Getting of HUB device information */
    p_hub_devinfo = r_usb_hstd_hci_get_device_infomation(hubaddr);

    /* Getting of HUB downport device information */
    p_devinfo = r_usb_hstd_hci_get_device_infomation(devaddr);

    if ((p_hub_devinfo != NULL) && (p_devinfo != NULL))
    {
        for (cnt = 0; cnt < USB_HCI_HUBPORT_NUM; cnt++)
        {
            if (0 == p_hub_devinfo->hubdevinfo.devnum[cnt])
            {
                p_hub_devinfo->hubdevinfo.devnum[cnt]   = (uint8_t) devaddr;
                p_hub_devinfo->hubdevinfo.devspeed[cnt] = (uint8_t) p_devinfo->bit.devspeed;
                usb_hci_tmp_addr = devaddr;
                break;
            }
        }
    }
    else
    {
        /* Error */
        USB_HCI_PRINTF2_R("Error: unknown hubaddr( %d ) or devaddr( %d )\n", hubaddr, devaddr);
    }
}                                      /* End of function r_usb_hstd_hci_set_device_address_of_hub_port() */

/*********************/
/* utility function  */
/*********************/
 #ifdef USB_HOST_COMPLIANCE_MODE
void r_usb_hstd_hci_electrical_test (uint16_t rootport, uint8_t mode)
{
    switch (mode)
    {
        case 0:                        /* Test SE0_NAK */
        {
            USB00->PORTSC1_b.PTTST = 3;
            while (1)
            {
                ;                      /* This loops infinitely until it's reset. */
            }

            break;
        }

        case 1:                        /* Test J */
        {
            USB00->PORTSC1_b.PTTST = 1;
            while (1)
            {
                ;                      /* This loops infinitely until it's reset. */
            }

            break;
        }

        case 2:                        /* Test K */
        {
            USB00->PORTSC1_b.PTTST = 2;
            while (1)
            {
                ;                      /* This loops infinitely until it's reset. */
            }

            break;
        }

        case 3:                        /* Signal Quality */
        {
            USB00->PORTSC1_b.PTTST = 4;
            while (1)
            {
                ;                      /* This loops infinitely until it's reset. */
            }

            break;
        }

        case 4:                              /* Suspend/Resume */
        {
            r_usb_hstd_hci_wait_time(15000); /* wait 15sec */
            USB00->PORTSC1_b.SUSPEND = 1;    /* Suspend */
            r_usb_hstd_hci_wait_time(15000); /* wait 15sec */
            USB00->PORTSC1_b.FRCPTRSM = 1;   /* Resume start */
            r_usb_hstd_hci_wait_time(20);    /* wait 20ms */
            USB00->PORTSC1_b.FRCPTRSM = 0;   /* Resume end */
            break;
        }

        case 5:                              /* Packet Parameters */
        {
            usb_hstd_electrical_test_mode(0x0108, rootport);
            break;
        }

        case 6:                        /* Chirp Timing */
        {
            USB00->PORTSC1_b.SUSPEND = 1;
            if (1 == USB00->PORTSC1_b.CCSTS)
            {
                USB00->USBINTR = 0x00000000;     /* Disable interrupt */
                r_usb_hstd_hci_port_reset(rootport);
                r_usb_hstd_hci_wait_time(15000); /* wait 15sec */
            }

            if (1 == USB00->PORTSC1_b.CCSTS)
            {
                r_usb_hstd_hci_port_reset(rootport);
            }

            USB00->USBINTR = 0x00000007; /* PortChange, USBError, USBInt */
            break;
        }

        default:
        {
            break;
        }
    }
}                                      /* End of function r_usb_hstd_hci_electrical_test() */

 #endif /* USB_HOST_COMPLIANCE_MODE */

/* waittimer_function for usbhstd */
void r_usb_hstd_hci_wait_time (uint32_t ms)
{
    uint32_t prv = USB00->HCFMNUMBER;

    while (ms)
    {
        if ((USB00->HCFMNUMBER - prv) != 0)
        {
            ms--;
            prv = USB00->HCFMNUMBER;
        }
    }
}                                      /* End of function r_usb_hstd_hci_wait_time() */

/***********************************************************************************************************************
 * Function Name: usb_hub_diconnect_delay
 * Description  :
 * Arguments    : none
 * Return Value : none
 ***********************************************************************************************************************/
void usb_hub_diconnect_delay (void)
{
    uint16_t                 i              = 0;
    static volatile uint16_t usb_delay_flag = 0;

    while (i < USB_MAXDEVADDR)
    {
        if (USB_IFCLS_HUB == g_usb_hstd_device_drv[0][i].ifclass)
        {
            usb_delay_flag++;
            break;
        }

        i++;
    }

    if (2 <= usb_delay_flag)
    {
        R_USB_HstdDelayXms((uint16_t) USB_VAL_66U);
        usb_delay_flag = 0;
    }
}

 #ifdef RZA2_USB_OHCI_IP
void r_usb_port_clean_seq (void)
{
    if (0 == USB00->HCRHDESCRIPTORA_b.NPS)
    {
        if ((1 == USB00->HCRHDESCRIPTORA_b.PSM) && (1 == USB00->HCRHDESCRIPTORB_b.PPCM))
        {
            /* A */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_LSDA;   /* LSDA = 1 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_POCI;   /* POCI = 1 */
            USB00->PORTSC1         |= USB_VAL_PO1;    /* PortOwner = 1 */
            USB00->PORTSC1         &= USB_VAL_PO0;    /* PortOwner = 0 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_POCI_1; /* POCI = 1 */
        }
        else
        {
            /* B */
            USB00->HCRHSTATUS      |= USB_VAL_PPS;  /* PPS  = 1 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_POCI; /* POCI = 1 */
            USB00->PORTSC1         |= USB_VAL_PO1;  /* PortOwner = 1 */
            USB00->PORTSC1         &= USB_VAL_PO0;  /* PortOwner = 0 */
            USB00->HCRHSTATUS      |= USB_VAL_LPSC; /* LPSC = 1 */
        }
    }
    else
    {
        if ((1 == USB00->HCRHDESCRIPTORA_b.PSM) && (1 == USB00->HCRHDESCRIPTORB_b.PPCM))
        {
            /* C */
            USB00->HCRHDESCRIPTORA &= USB_VAL_NPS_A;  /* NPS  = 0 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_LSDA;   /* LSDA = 1 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_POCI;   /* POCI = 1 */
            USB00->PORTSC1         |= USB_VAL_PO1;    /* PortOwner = 1 */
            USB00->PORTSC1         &= USB_VAL_PO0;    /* PortOwner = 0 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_POCI_1; /* POCI = 1 */
            USB00->HCRHDESCRIPTORA |= USB_VAL_NPS_B;  /* NPS  = 0 */
        }
        else
        {
            /* D */
            USB00->HCRHDESCRIPTORA &= USB_VAL_NPS_A; /* NPS  = 0 */
            USB00->HCRHSTATUS      |= USB_VAL_PPS;   /* PPS  = 1 */
            USB00->HCRHPORTSTATUS1 |= USB_VAL_POCI;  /* POCI = 1 */
            USB00->PORTSC1         |= USB_VAL_PO1;   /* PortOwner = 1 */
            USB00->PORTSC1         &= USB_VAL_PO0;   /* PortOwner = 0 */
            USB00->HCRHSTATUS      |= USB_VAL_LPSC;  /* LPSC = 1 */
            USB00->HCRHDESCRIPTORA |= USB_VAL_NPS_B; /* NPS  = 0 */
        }
    }
}

 #endif                                /* RZA2_USB_OHCI_IP */

 #if 0

/***********************************************************************************************************************
 * Function     : Deinitialize HCI
 * Declaration  : void usb_hstd_hci_deinit(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_hci_deinit (usb_utr_t * ptr)
{
    st_usb_hci_device_t * p_devinfo;
    uint8_t               i;

    /* All transfers are stopped, and all devices are disconnect. */
    p_devinfo = &usb_hci_device_info[0];
    for (i = 0; i < USB_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (FALSE == p_devinfo->bit.enable)
        {
            r_usb_hstd_hci_disconnect(ptr, p_devinfo->bit.devnum);
        }
    }

    usb_hstd_ehci_deinit();            /* EHCI H/W reset */
    usb_hstd_ohci_deinit();            /* OHCI H/W reset */

    memset(&usb_hci_manage_info, 0, sizeof(usb_hci_manage_info));
} /* End of function usb_hstd_hci_deinit() */

/***********************************************************************************************************************
 * Function     : Getting of PID status
 * Declaration  : uint16_t usb_hstd_hci_get_pid_status(uint16_t devaddr, uint16_t epnum)
 * Argument     : uint16_t devaddr ; device address
 * Note         :
 ***********************************************************************************************************************/
static uint16_t usb_hstd_hci_get_pid_status (uint16_t devaddr, uint16_t epnum)
{
    st_usb_hci_tr_req_t * p_tr_req;
    uint32_t              i;
    uint16_t              sts;

    p_tr_req = &g_usb_hstd_hci_transfer_request[0];
    for (i = 0; i < USB_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (TRUE == p_tr_req->bit.enable)
        {
            if ((p_tr_req->bit.devadrs == devaddr) && (p_tr_req->bit.epnum == epnum))
            {
                if (USB_HCI_HCTYPE_EHCI == p_tr_req->devinfo->bit.hctype)
                {
                    sts = usb_hstd_ehci_get_pid_status(p_tr_req);

                    return sts;
                }
                else if (p_tr_req->devinfo->bit.hctype == USB_HCI_HCTYPE_OHCI)
                {
                    sts = usb_hstd_ohci_get_pid_status(p_tr_req);

                    return sts;
                }
                else
                {
                    return sts;
                }

                break;
            }
        }
    }

    return USB_PID_NAK;
}                                      /* End of function usb_hstd_hci_get_pid_status() */

/***********************************************************************************************************************
 * Function     :
 * Declaration  : uint32_t usb_hstd_hci_get_port_current_connect_status(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint32_t          ; Current Connect Status( 1:Connected, 0:Not Connect );
 * Note         :
 ***********************************************************************************************************************/
static uint32_t usb_hstd_hci_get_port_current_connect_status (uint16_t rootport)
{
    return usb_hstd_ehci_get_port_current_connect_status(rootport);
}                                      /* End of function usb_hstd_hci_get_port_current_connect_status() */

 #endif

#endif /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
