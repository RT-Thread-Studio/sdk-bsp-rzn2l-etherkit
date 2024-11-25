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
 * File Name    : r_usb_hOhciMemory.c
 * Description  : OHCI Memory Control Code
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include "../driver/inc/r_usb_hhci_local.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define USB_VAL_BANDWIDTH    (0x2778)

/***********************************************************************************************************************
 * External variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Static variables and functions
 ***********************************************************************************************************************/
#if  USB_IP_EHCI_OHCI == 1

BSP_ALIGN_VARIABLE(256) st_usb_ohci_hcca_block_t gs_usb_hstd_ohci_hcca USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(16) st_usb_ohci_hcd_transfer_descriptor_t gs_usb_hstd_ohci_td_memory[USB_OHCI_NUM_TD]
USB_BUFFER_PLACE_IN_SECTION;                                                                      /* 32 bit-boundary Area */
BSP_ALIGN_VARIABLE(16) st_usb_ohci_hcd_endpoint_descriptor_t gs_usb_hstd_ohci_ed_memory[USB_OHCI_NUM_ED]
USB_BUFFER_PLACE_IN_SECTION;                                                                      /* 16 bit-boundary Area */
BSP_ALIGN_VARIABLE(16) uint8_t gs_usb_hstd_ohci_iso_buffer[USB_OHCI_ISO_MAXDEVICE][USB_OHCI_ISO_MAX_FRAME][
    USB_OHCI_ISO_MAX_PACKET_SIZE] USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(16) st_usb_ohci_request_t gs_usb_hstd_ohci_request_array[USB_OHCI_NUM_REQUEST] /* request setup to TD */
USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(16) st_usb_ohci_hcd_device_data_t gs_usb_hstd_ohci_device_data_memory[USB_OHCI_NUM_DEVICEDATA]
USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(16) st_usb_ohci_hcd_endpoint_t gs_usb_hstd_ohci_endpoint_memory[USB_OHCI_NUM_ENDPOINT]
USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(16) st_usb_ohci_hcd_endpoint_p_t gs_usb_hstd_ohci_enable_endpoint[USB_OHCI_MAXDEVICE][
    USB_OHCI_MAXENDPOINT][2] USB_BUFFER_PLACE_IN_SECTION;
BSP_ALIGN_VARIABLE(16) st_usb_ohci_iso_info_t gs_usb_hstd_ohci_iso_endpoint[USB_OHCI_ISO_MAXDEVICE]
USB_BUFFER_PLACE_IN_SECTION;

static uint32_t gs_usb_hstd_ohci_remain_dd = USB_OHCI_NUM_DEVICEDATA; /* Remaining DeviceData */
static uint32_t gs_usb_hstd_ohci_remain_ep = USB_OHCI_NUM_ENDPOINT;   /* Remaining Endpoint */
static uint32_t gs_usb_hstd_ohci_remain_ed = USB_OHCI_NUM_ED;         /* Remaining EndpointDescriptor */
static uint32_t gs_usb_hstd_ohci_remain_td = USB_OHCI_NUM_TD;         /* Remaining TransferDescriptor */

/***********************************************************************************************************************
 * Function     : OHCI Getting of HCCA Address
 * Declaration  : st_usb_ohci_hcca_block_p_t usb_hstd_ohci_get_hcca_address( void )
 * Argument     : void
 * Return       : st_usb_ohci_hcca_block_p_t ; HCCA Address pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_hcca_block_p_t usb_hstd_ohci_get_hcca_address (void)
{
    return &gs_usb_hstd_ohci_hcca;
}                                      /* End of function usb_hstd_ohci_get_hcca_address() */

/***********************************************************************************************************************
 * Function     : OHCI Initialize Request-Memory
 * Declaration  : void usb_hstd_ohci_init_request_memory( void )
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_init_request_memory (void)
{
    usb_hstd_hci_lock_resouce();

    memset(&gs_usb_hstd_ohci_request_array[0], 0, sizeof(gs_usb_hstd_ohci_request_array));

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ohci_init_request_memory() */

/***********************************************************************************************************************
 * Function     : OHCI Initialize Device Data Memory
 * Declaration  : void usb_hstd_ohci_init_device_data( void )
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_init_device_data (void)
{
    usb_hstd_hci_lock_resouce();

    /*  Clear all DeviceData */
    memset(&gs_usb_hstd_ohci_device_data_memory[0], 0, sizeof(gs_usb_hstd_ohci_device_data_memory));

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ohci_init_device_data() */

/***********************************************************************************************************************
 * Function     : OHCI Clear List
 * Declaration  : void usb_hstd_ohci_clear_list( void )
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_clear_list (void)
{
    usb_hstd_hci_lock_resouce();

    /*  Clear all Endpoints */
    memset(&gs_usb_hstd_ohci_endpoint_memory[0], 0, sizeof(gs_usb_hstd_ohci_endpoint_memory));

    /*  Clear all EndpointDescriptors */
    memset(&gs_usb_hstd_ohci_ed_memory[0], 0, sizeof(gs_usb_hstd_ohci_ed_memory));

    /*  Clear all TransferDescriptors */
    memset(&gs_usb_hstd_ohci_td_memory[0], 0, sizeof(gs_usb_hstd_ohci_td_memory));

    /*  Clear gs_usb_hstd_ohci_enable_endpoint[][] */
    memset(&gs_usb_hstd_ohci_enable_endpoint[0], 0, sizeof(gs_usb_hstd_ohci_enable_endpoint));

    gs_usb_hstd_ohci_remain_ep = USB_OHCI_NUM_ENDPOINT;
    gs_usb_hstd_ohci_remain_ed = USB_OHCI_NUM_ED;
    gs_usb_hstd_ohci_remain_td = USB_OHCI_NUM_TD;

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ohci_clear_list() */

/***********************************************************************************************************************
 * Function     : OHCI Create Transfer Request
 * Declaration  : st_usb_ohci_request_p_t usb_hstd_ohci_alloc_transfer_request( void )
 * Argument     : void
 * Return       : st_usb_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_request_p_t usb_hstd_ohci_alloc_transfer_request (void)
{
    st_usb_ohci_request_p_t ohci_req;
    st_usb_ohci_request_p_t ret_req = NULL;
    uint32_t                n;

    usb_hstd_hci_lock_resouce();

    /* Allocate Request */
    ohci_req = &gs_usb_hstd_ohci_request_array[0];
    for (n = 0; n < USB_OHCI_NUM_REQUEST; n++, ohci_req++)
    {
        if (FALSE == ohci_req->using_flag)
        {
            memset(ohci_req, 0, sizeof(st_usb_ohci_request_t));
            ohci_req->using_flag = TRUE;
            ret_req              = ohci_req;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    return ret_req;
}                                      /* End of function usb_hstd_ohci_alloc_transfer_request() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate Device Data Memory
 * Declaration  : st_usb_ohci_hcd_device_data_p_t usb_hstd_ohci_alloc_device_data( void )
 * Argument     : void
 * Return       : st_usb_ohci_hcd_device_data_p_t ; Device Data Pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_hcd_device_data_p_t usb_hstd_ohci_alloc_device_data (void)
{
    st_usb_ohci_hcd_device_data_p_t device_data;
    st_usb_ohci_hcd_device_data_p_t ret_device_data = NULL;
    uint32_t n;
    uint32_t m;

    usb_hstd_hci_lock_resouce();

    /* Allocate device_data */
    device_data = &gs_usb_hstd_ohci_device_data_memory[0];
    for (n = 0; n < USB_OHCI_NUM_DEVICEDATA; n++, device_data++)
    {
        if (FALSE == device_data->using_flag)
        {
            /* 0 clear */
            memset(device_data, 0, sizeof(st_usb_ohci_hcd_device_data_t));
            ret_device_data = device_data;

            /* Initialize Device Data */
            device_data->using_flag          = TRUE;
            device_data->available_bandwidth = USB_VAL_BANDWIDTH;

            for (m = 0; m < USB_OHCI_MAXROOTPORTS; m++)
            {
                device_data->root_device[m] = USB_HCI_NODEVICE;
            }

            gs_usb_hstd_ohci_remain_dd--;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    return ret_device_data;
}                                      /* End of function usb_hstd_ohci_alloc_device_data() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate Endpoint Memory
 * Declaration  : st_usb_ohci_hcd_endpoint_p_t usb_hstd_ohci_alloc_endpoint( void )
 * Argument     : void
 * Return       : st_usb_ohci_hcd_endpoint_p_t ; Endpoint Pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_hcd_endpoint_p_t usb_hstd_ohci_alloc_endpoint (void)
{
    st_usb_ohci_hcd_endpoint_p_t endpoint;
    st_usb_ohci_hcd_endpoint_p_t ret_endpoint = NULL;
    uint32_t n;

    usb_hstd_hci_lock_resouce();

    /*  Allocate endpoint */
    endpoint = &gs_usb_hstd_ohci_endpoint_memory[0];
    for (n = 0; n < USB_OHCI_NUM_ENDPOINT; n++, endpoint++)
    {
        if (FALSE == endpoint->using_flag)
        {
            ret_endpoint = endpoint;

            /* Initialize Endpoint */
            memset(endpoint, 0, sizeof(st_usb_ohci_hcd_endpoint_t));
            endpoint->using_flag = TRUE;
            gs_usb_hstd_ohci_remain_ep--;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    return ret_endpoint;
}                                      /* End of function usb_hstd_ohci_alloc_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate EndpointDescriptor Memory
 * Declaration  : st_usb_ohci_hcd_endpoint_descriptor_p_t usb_hstd_ohci_allocate_endpoint_descriptor( void )
 * Argument     : void
 * Return       : st_usb_ohci_hcd_endpoint_descriptor_p_t ; Endpoint Descriptor Pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_hcd_endpoint_descriptor_p_t usb_hstd_ohci_allocate_endpoint_descriptor (void)
{
    st_usb_ohci_hcd_endpoint_descriptor_p_t ed;
    st_usb_ohci_hcd_endpoint_descriptor_p_t ret_ed = NULL;
    uint32_t i;

    usb_hstd_hci_lock_resouce();

    /* Allocate EndpointDescriptor */
    ed = &gs_usb_hstd_ohci_ed_memory[0];
    for (i = 0; i < USB_OHCI_NUM_ED; i++, ed++)
    {
        if (FALSE == ed->using_flag)
        {
            ret_ed = ed;

            /* Initialize EndpointDescriptor */
            memset(ed, 0, sizeof(st_usb_ohci_hcd_endpoint_descriptor_t));
            ed->using_flag = TRUE;
            gs_usb_hstd_ohci_remain_ed--;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    return ret_ed;
}                                      /* End of function usb_hstd_ohci_allocate_endpoint_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate TransferDescriptor Memory
 * Declaration  : st_usb_ohci_hcd_transfer_descriptor_p_t usb_hstd_ohci_allocate_transfer_descriptor( void )
 * Argument     : void
 * Return       : st_usb_ohci_hcd_transfer_descriptor_p_t ; Transfer Descriptor Pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_hcd_transfer_descriptor_p_t usb_hstd_ohci_allocate_transfer_descriptor (void)
{
    st_usb_ohci_hcd_transfer_descriptor_p_t td;
    st_usb_ohci_hcd_transfer_descriptor_p_t ret_td = NULL;
    uint32_t i;

    usb_hstd_hci_lock_resouce();

    /* Allocate TransferDescriptor */
    td = &gs_usb_hstd_ohci_td_memory[0];
    for (i = 0; i < USB_OHCI_NUM_TD; i++, td++)
    {
        if (FALSE == td->using_flag)
        {
            ret_td = td;

            /* Initialize TransferDescriptor */
            memset(td, 0, sizeof(st_usb_ohci_hcd_transfer_descriptor_t));
            td->using_flag = TRUE;
            gs_usb_hstd_ohci_remain_td--;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    return ret_td;
}                                      /* End of function usb_hstd_ohci_allocate_transfer_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate Iso Endpoint
 * Declaration  : st_usb_ohci_iso_info_p_t usb_hstd_ohci_alloc_endpoint_iso( void )
 * Argument     : void
 * Return       : st_usb_ohci_iso_info_p_t ; Iso Endpoint pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_iso_info_p_t usb_hstd_ohci_alloc_endpoint_iso (void)
{
    st_usb_ohci_iso_info_p_t ret_iso_info = NULL;
    st_usb_ohci_iso_info_p_t iso_info;
    uint32_t                 n;
    uint32_t                 i;

    usb_hstd_hci_lock_resouce();

    iso_info = &gs_usb_hstd_ohci_iso_endpoint[0];
    for (n = 0; n < USB_OHCI_ISO_MAXDEVICE; n++, iso_info++)
    {
        if (FALSE == iso_info->using_flag)
        {
            memset(iso_info, 0, sizeof(st_usb_ohci_iso_info_t));
            for (i = 0; i < USB_OHCI_ISO_MAX_FRAME; i++)
            {
 #if 0
                iso_info->transfer_info[i].buffer = (uint8_t *) usb_hstd_ohci_physical_address_of(
                    &gs_usb_hstd_ohci_iso_buffer[n][i][0]);
 #else

// iso_info->transfer_info[i].buffer = (uint8_t *) &gs_usb_hstd_ohci_iso_buffer[n][i][0];
                iso_info->transfer_info[i].buffer = &gs_usb_hstd_ohci_iso_buffer[n][i][0];
 #endif
            }

            iso_info->using_flag = TRUE;
            ret_iso_info         = iso_info;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    return ret_iso_info;
}                                      /* End of function usb_hstd_ohci_alloc_endpoint_iso() */

/***********************************************************************************************************************
 * Function     : OHCI Free Transfer Request
 * Declaration  : usb_hstd_ohci_free_transfer_request(st_usb_ohci_request_p_t ohci_req)
 * Argument     : st_usb_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_free_transfer_request (st_usb_ohci_request_p_t ohci_req)
{
    usb_hstd_hci_lock_resouce();

    memset(ohci_req, 0, sizeof(st_usb_ohci_request_t));

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ohci_free_transfer_request() */

/***********************************************************************************************************************
 * Function     : OHCI Free TransferDescriptor Memory
 * Declaration  : void usb_hstd_ohci_free_transfer_descriptor( st_usb_ohci_hcd_transfer_descriptor_p_t td )
 * Argument     : st_usb_ohci_hcd_transfer_descriptor_p_t ; Transfer Descriptor Pointer
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_free_transfer_descriptor (st_usb_ohci_hcd_transfer_descriptor_p_t td)
{
    st_usb_ohci_list_entry_p_t tmp_hcd_list_f_link;
    st_usb_ohci_list_entry_p_t tmp_hcd_list_b_link;
    st_usb_ohci_request_p_t    tmp_usb_drequest;

    if (NULL == td)
    {
        return;
    }

    usb_hstd_hci_lock_resouce();
 #if 0
    tmp_usb_drequest    = (st_usb_ohci_request_p_t) r_usb_pa_to_va((uint32_t) td->usb_drequest);
    tmp_hcd_list_f_link = (st_usb_ohci_list_entry_p_t) r_usb_pa_to_va((uint32_t) tmp_usb_drequest->hcd_list.f_link);
    tmp_hcd_list_b_link = (st_usb_ohci_list_entry_p_t) r_usb_pa_to_va((uint32_t) tmp_usb_drequest->hcd_list.b_link);
 #else
    tmp_usb_drequest    = (st_usb_ohci_request_p_t) td->usb_drequest;
    tmp_hcd_list_f_link = (st_usb_ohci_list_entry_p_t) tmp_usb_drequest->hcd_list.f_link;
    tmp_hcd_list_b_link = (st_usb_ohci_list_entry_p_t) tmp_usb_drequest->hcd_list.b_link;
 #endif

    if (tmp_usb_drequest->hcd_list.f_link == &td->request_list)
    {
        /* Removing TD is located in head of the HcdList */
        tmp_hcd_list_f_link = tmp_hcd_list_f_link->f_link;
    }
    else if (tmp_usb_drequest->hcd_list.b_link == &td->request_list)
    {
        /* Removing TD is located in tail of the HcdList */
        tmp_hcd_list_b_link = tmp_hcd_list_b_link->b_link;
    }
    else
    {
    }

    /* clear */

    /* td = r_usb_pa_to_va((uint32_t)td); */
 #if 0
    td = (st_usb_ohci_hcd_transfer_descriptor_p_t) r_usb_pa_to_va((uint32_t) td);
 #endif

    /* memset(td, 0, sizeof(st_usb_ohci_hcd_transfer_descriptor_t)); */
    memset(td, (int) NULL, sizeof(st_usb_ohci_hcd_transfer_descriptor_t));

    gs_usb_hstd_ohci_remain_td++;

    usb_hstd_hci_unlock_resouce();

    FSP_PARAMETER_NOT_USED(tmp_hcd_list_f_link);
    FSP_PARAMETER_NOT_USED(tmp_hcd_list_b_link);
}                                      /* End of function usb_hstd_ohci_free_transfer_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Free Endpoint
 * Declaration  : void usb_hstd_ohci_free_endpoint( st_usb_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : st_usb_ohci_hcd_endpoint_p_t ; endpoint pointer
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_free_endpoint (st_usb_ohci_hcd_endpoint_p_t endpoint)
{
 #if 1
    if ((NULL != endpoint) && (NULL != endpoint->hcd_head_p) && (endpoint->hcd_tail_p == endpoint->hcd_head_p))
    {
        /* Remove NullTD (if remained) */
        usb_hstd_ohci_free_transfer_descriptor(endpoint->hcd_head_p);
    }

 #else
    struct usb_ohci_hcd_transfer_descriptor * volatile tp;
    struct usb_ohci_hcd_transfer_descriptor * volatile tp_next;

    tp = endpoint->hcd_head_p;
    if (0 == tp->hc_td.next_td)
    {
        tp_next = tp->hc_td.next_td;
    }
    else
    {
        tp_next = tp->hc_td.next_td - 0x20;
    }

    while (NULL != tp)
    {
        usb_hstd_ohci_free_transfer_descriptor(tp);
        tp = tp_next;
        if (0 == tp->hc_td.next_td)
        {
            tp_next = tp->hc_td.next_td;
        }
        else
        {
            tp_next = tp->hc_td.next_td - 0x20;
        }
    }
 #endif

    usb_hstd_hci_lock_resouce();

    if ((NULL != endpoint) && (NULL != endpoint->iso_info))
    {
        endpoint->iso_info->using_flag = FALSE;
    }

    if ((NULL != endpoint) && (TRUE == endpoint->using_flag))
    {
        /* memset(endpoint, 0, sizeof(st_usb_ohci_hcd_endpoint_t)); */
        memset(endpoint, (int) NULL, sizeof(st_usb_ohci_hcd_endpoint_t));

        gs_usb_hstd_ohci_remain_ep++;
    }

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ohci_free_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Free Endpoint Descriptor
 * Declaration  : void usb_hstd_ohci_free_endpoint_descriptor( st_usb_ohci_hcd_endpoint_descriptor_p_t ed )
 * Argument     : st_usb_ohci_hcd_endpoint_descriptor_p_t ed ; Endpoint Descriptor Pointer
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_free_endpoint_descriptor (st_usb_ohci_hcd_endpoint_descriptor_p_t ed)
{
    /* usb_hstd_ohci_free_endpoint(ed->endpoint); */
 #if 0
    usb_hstd_ohci_free_endpoint((st_usb_ohci_hcd_endpoint_p_t) r_usb_pa_to_va(ed->endpoint));
 #else
    usb_hstd_ohci_free_endpoint(ed->endpoint);
 #endif

    usb_hstd_hci_lock_resouce();

    /* memset(ed, 0 , sizeof(st_usb_ohci_hcd_endpoint_descriptor_t)); */
    memset(ed, (int) NULL, sizeof(st_usb_ohci_hcd_endpoint_descriptor_t));

    gs_usb_hstd_ohci_remain_ed++;
    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ohci_free_endpoint_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Scan Endpoint info.
 * Declaration  : st_usb_ohci_hcd_endpoint_p_t usb_hstd_ohci_scan_endpoint( uint8_t device_address, uint8_t endpoint_number, uint8_t direction )
 * Argument     : uint8_t device_address  ; device address
 *              : uint8_t endpoint_number ; endpoint no
 *              : uint8_t direction       ; direction
 * Return       : st_usb_ohci_hcd_endpoint_p_t ; endpoint pointer
 * Note         :
 ***********************************************************************************************************************/
st_usb_ohci_hcd_endpoint_p_t usb_hstd_ohci_scan_endpoint (uint8_t device_address,
                                                          uint8_t endpoint_number,
                                                          uint8_t direction)
{
    return gs_usb_hstd_ohci_enable_endpoint[device_address][endpoint_number][direction];
}                                      /* End of function usb_hstd_ohci_scan_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Store Endpoint info.
 * Declaration  : void usb_hstd_ohci_store_endpoint( uint8_t device_address, uint8_t endpoint_number, uint8_t direction, st_usb_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : uint8_t device_address  ; device address
 *              : uint8_t endpoint_number ; endpoint no
 *              : uint8_t direction       ; direction
 *              : st_usb_ohci_hcd_endpoint_p_t ; endpoint pointer
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ohci_store_endpoint (uint8_t                      device_address,
                                   uint8_t                      endpoint_number,
                                   uint8_t                      direction,
                                   st_usb_ohci_hcd_endpoint_p_t endpoint)
{
    gs_usb_hstd_ohci_enable_endpoint[device_address][endpoint_number][direction] = endpoint;
}                                      /* End of function usb_hstd_ohci_store_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Scan Containing Record( ED/TD )
 * Declaration  : uint32_t usb_hstd_ohci_scan_containing_record( st_usb_ohci_list_entry_p_t list, uint8_t type, uint8_t field )
 * Argument     : st_usb_ohci_list_entry_p_t list ; Scanning list pointer
 *              : uint8_t type               ; list type
 *              : uint8_t field              ; list field
 * Return       : uint32_t                   ; scaned record address
 * Note         :
 ***********************************************************************************************************************/
uint32_t usb_hstd_ohci_scan_containing_record (st_usb_ohci_list_entry_p_t list, uint8_t type, uint8_t field)
{
    st_usb_ohci_hcd_endpoint_descriptor_p_t ed;
    st_usb_ohci_hcd_transfer_descriptor_p_t td;
    uint32_t i;

    if (NULL == list)
    {
        return 0;
    }

    if (USB_OHCI_CR_TRANSFER_DESCRIPTOR == type)
    {
        if (USB_OHCI_CR_REQUESTLIST == field)
        {
            td = &gs_usb_hstd_ohci_td_memory[0];
            for (i = 0; i < USB_OHCI_NUM_TD; i++, td++)
            {
                if ((TRUE == td->using_flag) && (&td->request_list == list))
                {
                    return (uint32_t) td;
                }
            }
        }
    }
    else if (USB_OHCI_CR_ENDPOINT_DESCRIPTOR == type)
    {
        if (USB_OHCI_CR_LINK == field)
        {
            ed = &gs_usb_hstd_ohci_ed_memory[0];
            for (i = 0; i < USB_OHCI_NUM_ED; i++, ed++)
            {
                if ((TRUE == ed->using_flag) && (&ed->link == list))
                {
                    return (uint32_t) ed;
                }
            }
        }
        else if (USB_OHCI_CR_PAUSEDLINK == field)
        {
            ed = &gs_usb_hstd_ohci_ed_memory[0];
            for (i = 0; i < USB_OHCI_NUM_ED; i++, ed++)
            {
                if ((TRUE == ed->using_flag) && (&ed->paused_link == list))
                {
                    return (uint32_t) ed;
                }
            }
        }
        else
        {
        }
    }
    else
    {
    }

    return 0;
}                                      /* End of function usb_hstd_ohci_scan_containing_record() */

/***********************************************************************************************************************
 * Function     : OHCI Check remaining TDs
 * Declaration  : uint32_t usb_hstd_ohci_check_remained_td( void )
 * Argument     : void
 * Return       : uint32_t ; check result( TRUE:remaining, FALSE:not remaining )
 * Note         :
 ***********************************************************************************************************************/
uint32_t usb_hstd_ohci_check_remained_td (void)
{
    if (gs_usb_hstd_ohci_remain_td > 4)
    {
        return TRUE;
    }

    return FALSE;
}                                      /* End of function usb_hstd_ohci_check_remained_td() */

/***********************************************************************************************************************
 * Function     : OHCI Check remaining EDs
 * Declaration  : uint32_t usb_hstd_ohci_check_remained_ed( void )
 * Argument     : void
 * Return       : uint32_t ; check result( TRUE:remaining, FALSE:not remaining )
 * Note         :
 ***********************************************************************************************************************/
uint32_t usb_hstd_ohci_check_remained_ed (void)
{
    if (gs_usb_hstd_ohci_remain_ed > 1)
    {
        return TRUE;
    }

    return FALSE;
}                                      /* End of function usb_hstd_ohci_check_remained_ed() */

/***********************************************************************************************************************
 * Function     : OHCI Check remaining EPs
 * Declaration  : uint32_t usb_hstd_ohci_check_remained_ed( void )
 * Argument     : void
 * Return       : uint32_t ; check result( TRUE:remaining, FALSE:not remaining )
 * Note         :
 ***********************************************************************************************************************/
uint32_t usb_hstd_ohci_check_remained_ep (void)
{
    if (gs_usb_hstd_ohci_remain_ep > 1)
    {
        return TRUE;
    }

    return FALSE;
}                                      /* End of function usb_hstd_ohci_check_remained_ep() */

/***********************************************************************************************************************
 * Function     : OHCI ED descriptor 0 clear
 * Declaration  : void usb_hstd_ohci_release_ed( uint32_t )
 * Argument     : ED address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
 #if 0
void usb_hstd_ohci_release_ed (uint32_t hcd_ed_tmp)
{
    memset(hcd_ed_tmp, 0, sizeof(st_usb_ohci_hcd_endpoint_descriptor_t));
    gs_usb_hstd_ohci_remain_ed++;
}

 #endif
#endif                                 /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
