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
 * File Name    : r_usb_hEhciTransfer.c
 * Description  : EHCI Transfer Code
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include "r_usb_hhci_local.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define USB_VAL_FF          (0xFFU)
#define USB_VAL_1023U       (1023U)
#define USB_VAL_1000        (0x1000U)
#define USB_VAL_FFFFF000    (0xFFFFF000U)
#define USB_VAL_XFE0        (0xFFFFFFE0U)
#define USB_VAL_FFF         (0x00000FFFU)

#if  USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * External variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/

 #ifdef USB_HOST_COMPLIANCE_MODE
extern uint8_t g_usb_hstd_test_packet_parameter_flag;
 #endif                                /* USB_HOST_COMPLIANCE_MODE */

/***********************************************************************************************************************
 * Static variables and functions
 ***********************************************************************************************************************/
static void usb_hstd_ehci_set_async_qh(st_usb_hci_tr_req_t * p_tr_req, st_usb_ehci_qh_t * p_qh);
static void usb_hstd_ehci_make_cntrol_bulk_interrupt_request(st_usb_hci_tr_req_t * p_tr_req);
static void usb_hstd_ehci_make_isochronous_request(st_usb_hci_tr_req_t * p_tr_req);
static void usb_hstd_ehci_set_qtd(st_usb_ehci_qtd_t * p_qtd,
                                  uint32_t            totalsize,
                                  uint32_t            pid,
                                  uint32_t            toggle,
                                  uint32_t            ioc,
                                  uint32_t            bufferadrs);
static void usb_hstd_ehci_set_periodic_qh(st_usb_hci_tr_req_t * p_tr_req,
                                          st_usb_ehci_qh_t    * p_qh,
                                          uint32_t              pollrate_offset);
static void usb_hstd_ehci_start_periodic_qh(st_usb_ehci_qh_t * p_qh);
static void usb_hstd_ehci_init_itd(st_usb_hci_tr_req_t * p_tr_req,
                                   st_usb_ehci_itd_t   * p_itd,
                                   uint32_t              ioc,
                                   uint32_t              multi,
                                   uint32_t              pollrate_offset);
static void usb_hstd_ehci_start_itd(st_usb_hci_tr_req_t * p_tr_req);
static void usb_hstd_ehci_init_sitd(st_usb_hci_tr_req_t * p_tr_req,
                                    st_usb_ehci_sitd_t  * p_sitd,
                                    uint32_t              pollrate_offset);
static void usb_hstd_ehci_set_sitd(st_usb_ehci_sitd_t * p_sitd,
                                   uint32_t             totalsize,
                                   uint32_t             direction,
                                   uint32_t             ioc,
                                   uint32_t             bufferadrs);
static void usb_hstd_ehci_start_sitd(st_usb_ehci_sitd_t * p_sitd);

/* Start Micro-Frame Mask Table */
static const uint8_t gs_usb_hstd_ehci_s_mask[8] =
{
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80
};

/* Start Micro-Frame Mask Table */
static const uint8_t gs_usb_hstd_ehci_s_mask_pollrate4[4] =
{
    0x11,
    0x22,
    0x44,
    0x88
};

/* Start Micro-Frame Mask Table */
static const uint8_t gs_usb_hstd_ehci_s_mask_pollrate2[2] =
{
    0x55,
    0xAA
};

/* Complte Micro-Frame Mask Table */
static const uint8_t gs_usb_hstd_ehci_c_mask[8] =
{
    0x1C,
    0x38,
    0x70,
    0xE0,
    0x00,
    0x00,
    0x00,
    0x00
};

/***********************************************************************************************************************
 * Function     : Make Transfer Request
 * Declaration  : usb_hstd_ehci_make_transfer_request( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_make_transfer_request (st_usb_hci_tr_req_t * p_tr_req)
{
    uint32_t frame_list_size;

    /* pollrate setting */
    switch (p_tr_req->bit.eptype)
    {
        case USB_EP_CNTRL:
        case USB_EP_BULK:
        {
            p_tr_req->pollrate = 0;
            break;
        }

        case USB_EP_INT:
        case USB_EP_ISO:
        {
            p_tr_req->pollrate = usb_hstd_hci_get_pollrate(p_tr_req) & USB_VAL_3FFF;
            if (0 == p_tr_req->pollrate)
            {
                USB_HCI_PRINTF0_R("usb_hstd_ehci_make_transfer_request Error: pollrate = 0\n");

                return;
            }

            /* When the cycle of the pollrate is larger than that of the frame list size */
            frame_list_size = usb_hstd_ehci_get_frame_list_size();
            if ((p_tr_req->pollrate / 8) > frame_list_size)
            {
                p_tr_req->pollrate = (frame_list_size * 8) & USB_VAL_3FFF;
            }

            break;
        }

        default:
            USB_HCI_PRINTF0_R("usb_hstd_ehci_make_transfer_request Error: Unkown Endpoint-Type\n");

            return;
    }

    if (USB_EP_ISO == p_tr_req->bit.eptype)
    {
        usb_hstd_ehci_make_isochronous_request(p_tr_req);
    }
    else
    {
        usb_hstd_ehci_make_cntrol_bulk_interrupt_request(p_tr_req);
    }
}                                      /* End of function usb_hstd_ehci_make_transfer_request() */

/***********************************************************************************************************************
 * Function     : Set Async QH
 * Declaration  : usb_hstd_ehci_set_async_qh(st_usb_hci_tr_req_t *tr_req, st_usb_ehci_qh_t *qh)
 * Argument     : st_usb_hci_tr_req_t *tr_req ; Transfer Request Infomation
 *              : st_usb_ehci_qh_t *qh        ; pointer to QH
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_set_async_qh (st_usb_hci_tr_req_t * p_tr_req, st_usb_ehci_qh_t * p_qh)
{
    union endpoint1_tag endpoint1;
    union endpoint2_tag endpoint2;

    endpoint1.dword = 0;
    endpoint2.dword = 0;

    endpoint1.bit.maximum_packet_length = p_tr_req->bit.mps;        /* MaxPacketSize */
    if (USB_EP_CNTRL == p_tr_req->bit.eptype)
    {
        endpoint1.bit.dtc = 1;                                      /* Data Toggle Control */
    }

    endpoint1.bit.eps            = p_tr_req->devinfo->bit.devspeed; /* Device Speed */
    endpoint1.bit.endpt          = p_tr_req->bit.epnum;             /* Endpoint No */
    endpoint1.bit.device_address = p_tr_req->bit.devadrs;           /* Device Address */

    /* For split transaction */
    if (USB_HCI_DEVSPEED_HS != p_tr_req->devinfo->bit.devspeed)
    {
        if (USB_EP_CNTRL == p_tr_req->bit.eptype)
        {
            endpoint1.bit.c = 1;                                       /* Control Endpoint Flag */
        }

        endpoint2.bit.hub_addr    = p_tr_req->devinfo->bit.hubaddr;    /* Hub Address */
        endpoint2.bit.port_number = p_tr_req->devinfo->bit.hubportnum; /* Port Number */
    }

    endpoint2.bit.mult = 1;                                            /* Multi = 1 */

    /* For reclamation flag */
    if (1 == p_qh->endpoint1.bit.h)
    {
        endpoint1.bit.h = 1;
    }

    p_qh->endpoint2.dword = endpoint2.dword;
    p_qh->endpoint1.dword = endpoint1.dword;

    if (NULL == p_qh->qtd_head)
    {
        /* Case that is called from usb_hstd_EhciInit function */
        p_qh->next_qtd.address           = 1;
        p_qh->alternate_next_qtd.address = 1;
    }
    else
    {
        /* When Next_qTD of QH is not set */
 #if 0
        R_MMU_VAtoPA((uint32_t) p_qh->qtd_head, &p_qh->next_qtd.address);
 #else
        p_qh->next_qtd.address = (uint32_t) p_qh->qtd_head;
 #endif
        p_qh->alternate_next_qtd.address = 1;
    }

    if (1 == p_qh->transfer_info.bit.status_halted)
    {
        /* The Halted bit is released */
        p_qh->transfer_info.bit.status_halted = 0;
    }

    p_qh->info.direction = p_tr_req->bit.direction;

    /* Set Transfer Request Flag */
    p_qh->info.tr_req_flag = TRUE;
}                                      /* End of function usb_hstd_ehci_set_async_qh() */

/***********************************************************************************************************************
 * Function     : Transfer End QH
 * Declaration  : usb_hstd_ehci_transfer_end_qh( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_transfer_end_qh (usb_utr_t * ptr, st_usb_hci_tr_req_t * p_tr_req)
{
    st_usb_ehci_qh_t  * p_qh;
    st_usb_ehci_qtd_t * p_data_qtd;
    uint32_t            remain_size;
    uint16_t            status;
    uint16_t            ret;
 #if 0
    p_qh = (st_usb_ehci_qh_t *) r_usb_pa_to_va((uint32_t) p_tr_req->hci_info);
 #else
    p_qh = (st_usb_ehci_qh_t *) p_tr_req->hci_info;
 #endif

    /* ==================== */
    /*  Calc Transfer Size  */
    /* ==================== */
    if (USB_EP_CNTRL == p_tr_req->bit.eptype)
    {
        /* SETUP is skipped, and the DATA is set */
 #if 0
        p_data_qtd = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) (p_qh->qtd_head));
        p_data_qtd = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) (p_data_qtd->next_qtd.address & USB_VAL_XFE0));
 #else
        p_data_qtd = (st_usb_ehci_qtd_t *) p_qh->qtd_head;
        p_data_qtd = (st_usb_ehci_qtd_t *) (p_data_qtd->next_qtd.address & USB_VAL_XFE0);
 #endif
    }
    else
    {
 #if 0
        p_data_qtd = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) p_qh->qtd_head);
 #else
        p_data_qtd = (st_usb_ehci_qtd_t *) p_qh->qtd_head;
 #endif
    }

    /* The remaining transfer size is calculated */
    p_tr_req->actual_size = p_tr_req->trsize;
    while (1)
    {
        remain_size = p_data_qtd->transfer_info.bit.total_bytes_to_transfer;
        if (p_tr_req->actual_size >= remain_size)
        {
            p_tr_req->actual_size -= remain_size;
        }
        else
        {
            p_tr_req->actual_size = 0;
            USB_HCI_PRINTF0_R("usb_hstd_ehci_transfer_end_qh Error: Transfer sizes that are bigger than Request Sizes\n");
            break;
        }

 #if 0
        if ((int) p_data_qtd == r_usb_pa_to_va((uint32_t) p_qh->qtd_end))
 #else
        if (p_data_qtd == p_qh->qtd_end)
 #endif
        {
            break;
        }

        if (0 == p_data_qtd->next_qtd.bit.t)
        {
 #if 0
            p_data_qtd = (st_usb_ehci_qtd_t *) r_usb_pa_to_va(p_data_qtd->next_qtd.address & USB_VAL_XFE0);
 #else
            p_data_qtd = (st_usb_ehci_qtd_t *) (p_data_qtd->next_qtd.address & USB_VAL_XFE0);
 #endif
        }
        else
        {
            break;
        }
    }

    /* ======================= */
    /*  Check Transfer Result  */
    /* ======================= */
    p_tr_req->status = p_qh->transfer_info.dword;

    /* =========== */
    /*  Clear qTD  */
    /* =========== */
    usb_hstd_ehci_clear_qtd(p_qh->qtd_head);
    p_qh->qtd_head = NULL;

    /* Clear Transfer Request Flag */
    p_qh->info.tr_req_flag = FALSE;

    /* Set Transfer Status */
    status = usb_hstd_ehci_get_transfer_end_status(p_tr_req);

    if (USB_EP_CNTRL == p_tr_req->bit.eptype)
    {
        ret = usb_hstd_ehci_get_transfer_end_status(p_tr_req);
        if (USB_DATA_STALL == ret)
        {
            usb_hstd_ehci_clear_qh(p_qh);
        }
    }

    /* ============== */
    /*  Clear tr_req  */
    /* ============== */
    p_tr_req->bit.enable = FALSE;

    /* =============== */
    /*  Callback Func  */
    /* =============== */
    ptr->keyword = p_tr_req->bit.devadrs;
    g_usb_hstd_hci_callback.tr_end_cb(ptr, p_tr_req->utr_p, p_tr_req->actual_size, status);
}                                      /* End of function usb_hstd_ehci_transfer_end_qh() */

/***********************************************************************************************************************
 * Function     : Transfer End ITD
 * Declaration  : usb_hstd_ehci_transfer_end_itd( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_transfer_end_itd (usb_utr_t * ptr, st_usb_hci_tr_req_t * p_tr_req)
{
    u_usb_ehci_transfer_info_t * p_status;
    st_usb_ehci_itd_t          * p_itd;
    uint16_t  status;
    uint16_t  i;
    uint8_t * p_data_buf;
    uint8_t * tmp_itd_buffer;
 #if 0
    p_itd = (st_usb_ehci_itd_t *) r_usb_pa_to_va((uint32_t) p_tr_req->hci_info);
 #else
    p_itd = (st_usb_ehci_itd_t *) p_tr_req->hci_info;
 #endif

    /* ==================== */
    /*  Calc Transfer Size  */
    /* ==================== */
    if (p_tr_req->bit.direction == USB_HCI_DIRECTION_IN)
    {
        /* Set Distination Address */
 #if 0
        p_data_buf = (uint8_t *) (r_usb_pa_to_va((uint32_t) p_tr_req->databuf));
 #else
        p_data_buf = (uint8_t *) p_tr_req->databuf;
 #endif
        for (i = 0; i < 8; i++)
        {
            /* Check data is set */
            if (p_itd->transaction[i].bit.length > 0)
            {
                /* copy IN data */
 #if 0
                tmp_itd_buffer = (uint8_t *) r_usb_pa_to_va(&p_itd->tmp_buffer[i][0]);
 #else
                tmp_itd_buffer = &p_itd->tmp_buffer[i][0];
 #endif
                memcpy(p_data_buf, tmp_itd_buffer, p_itd->transaction[i].bit.length);

                /* Update Distination Address */
                p_data_buf += p_itd->transaction[i].bit.length;

                /* Set the Reception Size */
                p_tr_req->actual_size += p_itd->transaction[i].bit.length;
            }
        }
    }
    else
    {
        /* Set the Transfer Request Size */
        p_tr_req->actual_size = p_tr_req->trsize;
    }

    /* ======================= */
    /*  Check Transfer Result  */
    /* ======================= */
    p_tr_req->status                       = 0;
    p_status                               = (u_usb_ehci_transfer_info_t *) &p_tr_req->status;
    p_status->bit.status_active            = p_itd->transaction[p_itd->next_trend_uframe].bit.status_active;
    p_status->bit.status_data_buffer_error = p_itd->transaction[p_itd->next_trend_uframe].bit.status_data_buffer_error;
    p_status->bit.status_babble_detected   = p_itd->transaction[p_itd->next_trend_uframe].bit.status_babble_detected;
    p_status->bit.status_transaction_error = p_itd->transaction[p_itd->next_trend_uframe].bit.status_transaction_error;

    /* Clear Transfer Request Flag */
    p_itd->info.tr_req_flag = FALSE;

    /* Set Transfer Status */
    status = usb_hstd_ehci_get_transfer_end_status(p_tr_req);

    /* ============== */
    /*  Clear tr_req  */
    /* ============== */
    p_tr_req->bit.enable = FALSE;

    /* =========== */
    /*  Call Func  */
    /* =========== */
    g_usb_hstd_hci_callback.tr_end_cb(ptr, p_tr_req->utr_p, p_tr_req->actual_size, status);
}                                      /* End of function usb_hstd_ehci_transfer_end_itd() */

/***********************************************************************************************************************
 * Function     : Transfer End SITD
 * Declaration  : usb_hstd_ehci_transfer_end_sitd( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_transfer_end_sitd (usb_utr_t * ptr, st_usb_hci_tr_req_t * p_tr_req)
{
    st_usb_ehci_sitd_t * p_sitd;
    uint32_t             remain_size;
    uint16_t             status;
 #if 0
    p_sitd = (st_usb_ehci_sitd_t *) r_usb_pa_to_va((uint32_t) p_tr_req->hci_info);
 #else
    p_sitd = (st_usb_ehci_sitd_t *) p_tr_req->hci_info;
 #endif

    /* ==================== */
    /*  Calc Transfer Size  */
    /* ==================== */
    /* The remaining transfer size is calculated */
    p_tr_req->actual_size = p_tr_req->trsize;
    remain_size           = p_sitd->state.bit.total_bytes_to_transfer;
    if (p_tr_req->actual_size < remain_size)
    {
        p_tr_req->actual_size = 0;
        USB_HCI_PRINTF0_R("usb_hstd_ehci_transfer_end_sitd Error: Transfer sizes that are bigger than Request Sizes\n");
    }
    else
    {
        p_tr_req->actual_size -= remain_size;
    }

    /* ======================= */
    /*  Check Transfer Result  */
    /* ======================= */
    p_tr_req->status = p_sitd->state.dword;

    /* Clear Transfer Request Flag */
    p_sitd->info.tr_req_flag = FALSE;

    /* Set Transfer Status */
    status = usb_hstd_ehci_get_transfer_end_status(p_tr_req);

    /* ============== */
    /*  Clear tr_req  */
    /* ============== */
    p_tr_req->bit.enable = FALSE;

    /* =========== */
    /*  Call Func  */
    /* =========== */
    g_usb_hstd_hci_callback.tr_end_cb(ptr, p_tr_req->utr_p, p_tr_req->actual_size, status);
}                                      /* End of function usb_hstd_ehci_transfer_end_sitd() */

/* Static Function */

/***********************************************************************************************************************
 * Function     : Make Transfer Request
 * Declaration  : usb_hstd_ehci_make_cntrol_bulk_interrupt_request( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_make_cntrol_bulk_interrupt_request (st_usb_hci_tr_req_t * p_tr_req)
{
    uint32_t            new_qh_flag = FALSE;
    st_usb_ehci_qh_t  * p_qh;
    st_usb_ehci_qtd_t * p_tmp_qtd;
    st_usb_ehci_qtd_t * p_pre_qtd;
    st_usb_ehci_qtd_t * p_qtd_head = 0;
    st_usb_ehci_qtd_t * p_qtd_end  = 0;
    uint32_t            data_pid;
    uint32_t            setup_pid;
    uint32_t            data_ioc;
    uint32_t            data_toggle;
    uint32_t            remain_size;
    uint32_t            bufaddress;
    uint32_t            ioc_flag = 0;
    uint32_t            tr_size;
    uint32_t            pollrate_offset;
    uint32_t            max_period;
 #ifdef USB_HOST_COMPLIANCE_MODE
    st_usb_ehci_qtd_t * p_qtd_head_tmp;
    st_usb_ehci_qtd_t * p_qtd_end_tmp;
 #endif                                /* USB_HOST_COMPLIANCE_MODE */

    if (USB_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
    {
        data_pid  = USB_EHCI_PID_OUT;  /* Data PID : OUT */
        setup_pid = USB_EHCI_PID_IN;   /* Setup PID: IN */
    }
    else
    {
        data_pid  = USB_EHCI_PID_IN;   /* Data PID : IN */
        setup_pid = USB_EHCI_PID_OUT;  /* Setup PID: OUT */
    }

    /*-----------*/
    /*  Setup TD */
    /*-----------*/
    if (USB_EP_CNTRL == p_tr_req->bit.eptype)
    {
        p_tmp_qtd = usb_hstd_ehci_alloc_qtd();
        if (NULL == p_tmp_qtd)
        {
            p_tr_req->bit.enable = FALSE;                       /* clear tr_req */
            return;
        }

        usb_hstd_ehci_set_qtd(p_tmp_qtd,                        /* qTD */
                              USB_HCI_SETUP_DATA_SIZE,          /* Total Size */
                              USB_EHCI_PID_SETUP,               /* PID */
                              0,                                /* Toggle */
                              0,                                /* Interrupt On Complete */
                              (uint32_t) &p_tr_req->setupbuf[0] /* Buffer Address */
                              );

        p_pre_qtd  = p_tmp_qtd;
        p_qtd_head = p_tmp_qtd;
    }
    else
    {
        p_pre_qtd = NULL;
    }

    /*---------*/
    /* Data TD */
    /*---------*/
    if (USB_EP_CNTRL == p_tr_req->bit.eptype) /* Control */
    {
        data_ioc    = 0;
        data_toggle = 1;
    }
    else                               /* Bulk or Interrupt */
    {
        data_ioc    = 1;
        data_toggle = 0;
    }

    remain_size = p_tr_req->trsize;
    bufaddress  = p_tr_req->databuf;
    while (0 != remain_size)
    {
        p_tmp_qtd = usb_hstd_ehci_alloc_qtd();
        if (NULL == p_tmp_qtd)
        {
            usb_hstd_ehci_clear_qtd(p_qtd_head); /* clear qtd */
            p_tr_req->bit.enable = FALSE;        /* clear tr_req */
            return;
        }

        /* The maximum forwarding size of one QTD is decided */
        tr_size  = USB_EHCI_MAXQTDSIZE - (bufaddress & USB_VAL_FFF);
        tr_size -= tr_size % p_tr_req->bit.mps;

        /* Is it the last forwarding? */
        if (remain_size <= tr_size)
        {
            tr_size  = remain_size;
            ioc_flag = data_ioc;
        }

        usb_hstd_ehci_set_qtd(p_tmp_qtd,   /* qTD */
                              tr_size,     /* Total Size */
                              data_pid,    /* PID */
                              data_toggle, /* Toggle */
                              ioc_flag,    /* Interrupt On Complete */
                              bufaddress   /* Buffer Address */
                              );

        remain_size -= tr_size;
        bufaddress  += tr_size;

        if (NULL != p_pre_qtd)
        {
            usb_hstd_ehci_link_qtd(p_pre_qtd, p_tmp_qtd);
        }
        else
        {
            p_qtd_head = p_tmp_qtd;
        }

        p_pre_qtd = p_tmp_qtd;
        p_qtd_end = p_tmp_qtd;
    }

    /*-----------*/
    /* Status TD */
    /*-----------*/
    if (USB_EP_CNTRL == p_tr_req->bit.eptype)
    {
        p_tmp_qtd = usb_hstd_ehci_alloc_qtd();
        if (NULL == p_tmp_qtd)
        {
            usb_hstd_ehci_clear_qtd(p_qtd_head); /* clear qtd */
            p_tr_req->bit.enable = FALSE;        /* clear tr_req */
            return;
        }

        usb_hstd_ehci_set_qtd(p_tmp_qtd,         /* qTD */
                              0,                 /* Total Size */
                              setup_pid,         /* PID */
                              1,                 /* Toggle */
                              1,                 /* Interrupt On Complete */
                              0                  /* Buffer Address */
                              );
        usb_hstd_ehci_link_qtd(p_pre_qtd, p_tmp_qtd);
        p_qtd_end = p_tmp_qtd;
    }

    /*  QH search */
    p_qh = usb_hstd_ehci_search_qh(p_tr_req->bit.devadrs,
                                   p_tr_req->bit.epnum,
                                   p_tr_req->bit.direction,
                                   p_tr_req->bit.eptype);
    if (NULL == p_qh)
    {
        p_qh = usb_hstd_ehci_alloc_qh();
        if (NULL != p_qh)
        {
            new_qh_flag = TRUE;
        }
        else
        {
            usb_hstd_ehci_clear_qtd(p_qtd_head); /* clear qtd */
            p_tr_req->bit.enable = FALSE;        /* clear tr_req */
            return;
        }
    }

    /* QH registration */
 #if 0
    R_MMU_VAtoPA((uint32_t) p_qh, &p_tr_req->hci_info);
    R_MMU_VAtoPA((uint32_t) p_qtd_head, &p_qh->qtd_head);
    R_MMU_VAtoPA((uint32_t) p_qtd_end, &p_qh->qtd_end);
 #else
    p_tr_req->hci_info = p_qh;
    p_qh->qtd_head     = p_qtd_head;
    p_qh->qtd_end      = p_qtd_end;
 #endif

 #ifdef USB_HOST_COMPLIANCE_MODE
    if (g_usb_hstd_test_packet_parameter_flag)
    {
  #if 0
        p_qtd_head_tmp = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) (p_qh->qtd_head));
        p_qtd_head_tmp->transfer_info.bit.status_active = 0;
        p_qtd_head_tmp->next_qtd.pointer->transfer_info.bit.status_active = 0;

        p_qtd_end_tmp = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) (p_qh->qtd_end));
        p_qtd_end_tmp->transfer_info.bit.status_active = 0;
  #else
        p_qtd_head_tmp = (st_usb_ehci_qtd_t *) p_qh->qtd_head;
        p_qtd_head_tmp->transfer_info.bit.status_active = 0;
        p_qtd_head_tmp->next_qtd.pointer->transfer_info.bit.status_active = 0;

        p_qtd_end_tmp = (st_usb_ehci_qtd_t *) p_qh->qtd_end;
        p_qtd_end_tmp->transfer_info.bit.status_active = 0;
  #endif
    }
 #endif                                     /* USB_HOST_COMPLIANCE_MODE */

    if (USB_EP_INT == p_tr_req->bit.eptype) /* Interrupt */
    {
        if (TRUE == new_qh_flag)
        {
            if (USB_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
            {
                max_period = 1;
            }
            else
            {
                /* The maximum term of 5 frames is set for the split transaction of the interrupt forwarding */
                max_period = 5;
            }

            /* Scheduling score update of Periodic Transfer */
            pollrate_offset = usb_hstd_ehci_update_periodic_scheduling_score(max_period, p_tr_req->pollrate);
        }
        else
        {
            /* An existing setting is used */
            pollrate_offset = p_qh->info.pollrate_offset;
        }

        usb_hstd_ehci_set_periodic_qh(p_tr_req, p_qh, pollrate_offset);

        if (TRUE == new_qh_flag)
        {
            /*  Link to Periodic Schedule */
            usb_hstd_ehci_add_periodic(&p_qh->queue_head_horizontal_link,
                                       USB_EHCI_TYP_QH,
                                       p_qh->info.pollrate,
                                       p_qh->info.pollrate_offset);
        }

        /* Transfer Start */
        usb_hstd_ehci_start_periodic_qh(p_qh);
    }
    else                               /* Control or Bulk */
    {
        usb_hstd_ehci_set_async_qh(p_tr_req, p_qh);
        if (TRUE == new_qh_flag)
        {
            /*  Link to Async Schedule */
            usb_hstd_ehci_add_async(p_qh);
        }
    }

 #ifdef USB_HOST_COMPLIANCE_MODE
    if (g_usb_hstd_test_packet_parameter_flag)
    {
  #if 0
        p_qtd_head_tmp = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) (p_qh->qtd_head));
        p_qtd_end_tmp  = (st_usb_ehci_qtd_t *) r_usb_pa_to_va((uint32_t) (p_qh->qtd_end));
  #else
        p_qtd_head_tmp = (st_usb_ehci_qtd_t *) p_qh->qtd_head;
        p_qtd_end_tmp  = (st_usb_ehci_qtd_t *) p_qh->qtd_end;
  #endif

        p_qtd_head_tmp->transfer_info.bit.status_active = 1;                   /* Setup stage */
        r_usb_hstd_hci_wait_time(15000);
        p_qtd_head_tmp->next_qtd.pointer->transfer_info.bit.status_active = 1; /* Data stage(IN) */
        p_qtd_end_tmp->transfer_info.bit.status_active = 1;                    /* Status stage */
        r_usb_hstd_hci_wait_time(15000);
    }
 #endif /* USB_HOST_COMPLIANCE_MODE */
} /* End of function usb_hstd_ehci_make_cntrol_bulk_interrupt_request() */

/***********************************************************************************************************************
 * Function     : Make Isochronous Transfer Request
 * Declaration  : usb_hstd_ehci_make_isochronous_request( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_make_isochronous_request (st_usb_hci_tr_req_t * p_tr_req)
{
    st_usb_ehci_itd_t  * p_itd;
    st_usb_ehci_sitd_t * p_sitd;
    uint32_t             new_flag = FALSE;
    uint32_t             pollrate_offset;
    uint32_t             max_period;

    /* 0 length packet is unsupported */
    if (0 == p_tr_req->trsize)
    {
        return;
    }

    if (USB_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
    {
        /* Unsupported size */
        if (p_tr_req->trsize > (USB_EHCI_ITD_DATA_SIZE * (8U / p_tr_req->pollrate)))
        {
            return;
        }

        p_itd = usb_hstd_ehci_search_itd(p_tr_req->bit.devadrs, p_tr_req->bit.epnum, p_tr_req->bit.direction);
        if (NULL == p_itd)
        {
            new_flag = TRUE;
            p_itd    = usb_hstd_ehci_alloc_itd();
            if (NULL == p_itd)
            {
                p_tr_req->bit.enable = FALSE;

                return;
            }
        }

 #if 0
        R_MMU_VAtoPA((uint32_t) p_itd, &p_tr_req->hci_info);
 #else
        p_tr_req->hci_info = p_itd;
 #endif

        if (TRUE == new_flag)
        {
            max_period = 1;

            /* Update Periodic Scheduling Score */
            pollrate_offset = usb_hstd_ehci_update_periodic_scheduling_score(max_period, p_tr_req->pollrate);

            /* Initialize ITD */
            usb_hstd_ehci_init_itd(p_tr_req,       /* Transfer Request Infomation */
                                   p_itd,          /* pointer to ITD */
                                   0,              /* Interrupt On Complete */
                                   1,              /* Multi */
                                   pollrate_offset /* polling rate offset */
                                   );

            /* Link to Periodic Schedule */
            while ((USB00->FRINDEX & 0x00000007) != 0x00000007)
            {
                ;                      /* uFrame Sync */
            }

            usb_hstd_ehci_add_periodic(&p_itd->next_link,
                                       USB_EHCI_TYP_ITD,
                                       p_itd->info.pollrate,
                                       p_itd->info.pollrate_offset);
        }

        /* Transfer Start */
        usb_hstd_ehci_start_itd(p_tr_req);
    }
    else
    {
        /* Full-speed isochronous( Split transaction ) */
        p_sitd = usb_hstd_ehci_search_sitd(p_tr_req->bit.devadrs, p_tr_req->bit.epnum, p_tr_req->bit.direction);
        if (NULL == p_sitd)
        {
            new_flag = TRUE;
            p_sitd   = usb_hstd_ehci_alloc_sitd();
            if (NULL == p_sitd)
            {
                p_tr_req->bit.enable = FALSE;

                return;
            }
        }

 #if 0
        R_MMU_VAtoPA((uint32_t) p_sitd, &p_tr_req->hci_info);
 #else
        p_tr_req->hci_info = p_sitd;
 #endif

        if (TRUE == new_flag)
        {
            /* The number of micro frames necessary for transfer is calculated */
            max_period = (p_tr_req->bit.mps + USB_EHCI_SITD_DATA_SIZE - 1) / USB_EHCI_SITD_DATA_SIZE;
            if (USB_HCI_DIRECTION_IN == p_tr_req->bit.direction)
            {
                /* The IN transfer adds 2 micro frame.  */
                max_period += 2;
            }

            /* Update Periodic Scheduling Score */
            pollrate_offset = usb_hstd_ehci_update_periodic_scheduling_score(max_period, p_tr_req->pollrate);

            /* Initialize SITD */
            usb_hstd_ehci_init_sitd(p_tr_req, p_sitd, pollrate_offset);

            /* Link to Periodic Schedule */
            usb_hstd_ehci_add_periodic(&p_sitd->next_link,
                                       USB_EHCI_TYP_SITD,
                                       p_sitd->info.pollrate,
                                       p_sitd->info.pollrate_offset);
        }

        usb_hstd_ehci_set_sitd(p_sitd,                  /* siTD */
                               p_tr_req->trsize,        /* Total Size */
                               p_tr_req->bit.direction, /* direction */
                               1,                       /* Interrupt On Complete */
                               p_tr_req->databuf        /* Buffer */
                               );

        /* Transfer Setting */
        usb_hstd_ehci_start_sitd(p_sitd);
    }
}                                      /* End of function usb_hstd_ehci_make_isochronous_request() */

/***********************************************************************************************************************
 * Function     : Set Async qTD
 * Declaration  : usb_hstd_ehci_set_qtd( st_usb_ehci_qtd_t *qtd, uint32_t totalsize, uint32_t pid, uint32_t toggle, uint32_t ioc, uint32_t bufferadrs )
 * Argument     : st_usb_ehci_qtd_t *qtd     ; pointer to qTD
 *              : uint32_t totalsize    ; total transfer size
 *              : uint32_t pid          ; PID
 *              : uint32_t toggle       ; toggle
 *              : uint32_t ioc          ; Interrupt On Complete
 *              : uint32_t bufferadrs   ; buffer address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_set_qtd (st_usb_ehci_qtd_t * p_qtd,
                                   uint32_t            totalsize,
                                   uint32_t            pid,
                                   uint32_t            toggle,
                                   uint32_t            ioc,
                                   uint32_t            bufferadrs)
{
    memset(p_qtd, 0, sizeof(st_usb_ehci_qtd_t));

    p_qtd->next_qtd.bit.t           = 1;
    p_qtd->alternate_next_qtd.bit.t = 1;

    p_qtd->transfer_info.bit.total_bytes_to_transfer = totalsize & USB_VAL_7FFF;
    p_qtd->transfer_info.bit.pid_code                = pid & 0x00000003U;
    p_qtd->transfer_info.bit.data_toggle             = toggle & 0x00000001U;
    p_qtd->transfer_info.bit.ioc           = ioc & 0x00000001U;;
    p_qtd->transfer_info.bit.cerr          = 3; /* Initial value of error count(3) */
    p_qtd->transfer_info.bit.status_active = 1;

    if (0 != bufferadrs)
    {
 #if 0
        R_MMU_VAtoPA(bufferadrs, &p_qtd->buffer[0].address);
 #else
        p_qtd->buffer[0].address = bufferadrs;
 #endif
        p_qtd->buffer[1].address = (p_qtd->buffer[0].address + USB_VAL_1000) & USB_VAL_FFFFF000;
        p_qtd->buffer[2].address = (p_qtd->buffer[1].address + USB_VAL_1000);
        p_qtd->buffer[3].address = (p_qtd->buffer[2].address + USB_VAL_1000);
        p_qtd->buffer[4].address = (p_qtd->buffer[3].address + USB_VAL_1000);
    }
}                                      /* End of function usb_hstd_ehci_set_qtd() */

/***********************************************************************************************************************
 * Function     : Set Periodic QH
 * Declaration  : usb_hstd_ehci_set_periodic_qh(st_usb_hci_tr_req_t *tr_req, st_usb_ehci_qh_t *qh, uint32_t pollrate_offset)
 * Argument     : st_usb_hci_tr_req_t *tr_req   ; Transfer Request Infomation
 *              : st_usb_ehci_qh_t *qh          ; pointer to QH
 *              : uint32_t pollrate_offset ; polling rate offset
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_set_periodic_qh (st_usb_hci_tr_req_t * p_tr_req,
                                           st_usb_ehci_qh_t    * p_qh,
                                           uint32_t              pollrate_offset)
{
    uint32_t offset_us;

    offset_us = pollrate_offset % 8;

    p_qh->endpoint1.bit.maximum_packet_length = p_tr_req->bit.mps;                                 /* MaxPacketSize */
    p_qh->endpoint1.bit.eps            = p_tr_req->devinfo->bit.devspeed;                          /* Device Speed */
    p_qh->endpoint1.bit.endpt          = p_tr_req->bit.epnum;                                      /* Endpoint Number */
    p_qh->endpoint1.bit.device_address = p_tr_req->bit.devadrs;                                    /* Device Address */

    /* For high band width */
    p_qh->endpoint2.bit.mult = (uint32_t) (((p_tr_req->bit.mps >> 10) & 0x3) + 1) & 0x00000003U;   /* Multi */
    if (8 <= p_tr_req->pollrate)
    {
        p_qh->endpoint2.bit.mframe_smask = gs_usb_hstd_ehci_s_mask[offset_us];                     /* Start mask */
    }
    else if (4 == p_tr_req->pollrate)
    {
        p_qh->endpoint2.bit.mframe_smask = gs_usb_hstd_ehci_s_mask_pollrate4[pollrate_offset % 4]; /* Start mask */
    }
    else if (2 == p_tr_req->pollrate)
    {
        p_qh->endpoint2.bit.mframe_smask = gs_usb_hstd_ehci_s_mask_pollrate2[pollrate_offset % 2]; /* Start mask */
    }
    else
    {
        p_qh->endpoint2.bit.mframe_smask = USB_VAL_FF;                                             /* Start mask */
    }

    /* For split transaction */
    if (USB_HCI_DEVSPEED_HS != p_tr_req->devinfo->bit.devspeed)
    {
        p_qh->endpoint2.bit.hub_addr     = p_tr_req->devinfo->bit.hubaddr;     /* Hub Address */
        p_qh->endpoint2.bit.port_number  = p_tr_req->devinfo->bit.hubportnum;  /* Port Number */
        p_qh->endpoint2.bit.mframe_cmask = gs_usb_hstd_ehci_c_mask[offset_us]; /* Complte Mask */
    }

    p_qh->info.direction         = p_tr_req->bit.direction;
    p_qh->info.pollrate          = p_tr_req->pollrate;
    p_qh->info.pollrate_offset   = pollrate_offset & USB_VAL_3FFF;
    p_qh->transfer_info.bit.cerr = 3;  /* Initial value of error count(3) */

    p_qh->alternate_next_qtd.address = 0x00000001;
} /* End of function usb_hstd_ehci_set_periodic_qh() */

/***********************************************************************************************************************
 * Function     : Start Periodic QH
 * Declaration  : void usb_hstd_ehci_start_periodic_qh( st_usb_ehci_qh_t *qh )
 * Argument     : st_usb_ehci_qh_t *qh     ; pointer to QH
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_start_periodic_qh (st_usb_ehci_qh_t * p_qh)
{
    /* Set Transfer Request Flag */
    p_qh->info.tr_req_flag = TRUE;
 #if 0
    R_MMU_VAtoPA((uint32_t) p_qh->qtd_head, &p_qh->next_qtd.address);
 #else
    p_qh->next_qtd.address = (uint32_t) p_qh->qtd_head;
 #endif
}                                      /* End of function usb_hstd_ehci_start_periodic_qh() */

/***********************************************************************************************************************
 * Function     : Initialize ITD
 * Declaration  : usb_hstd_ehci_init_itd(st_usb_hci_tr_req_t *tr_req, st_usb_ehci_itd_t *itd, uint32_t ioc, uint32_t multi, uint32_t pollrate_offset)
 * Argument     : st_usb_hci_tr_req_t *tr_req            ; Transfer Request Infomation
 *              : st_usb_ehci_itd_t   *itd               ; pointer to ITD
 *              : uint32_t       ioc                ; Interrupt On Complete
 *              : uint32_t       multi              ; Multi
 *              : uint32_t       pollrate_offset    ; polling rate offset
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_init_itd (st_usb_hci_tr_req_t * p_tr_req,
                                    st_usb_ehci_itd_t   * p_itd,
                                    uint32_t              ioc,
                                    uint32_t              multi,
                                    uint32_t              pollrate_offset)
{
    uint8_t  n;
    uint8_t  pg;
    uint32_t tmp_bufferadrs;
    uint32_t address;

    pg = 0;
 #if 0
    tmp_bufferadrs = (uint32_t) r_usb_pa_to_va(&p_itd->tmp_buffer[0][0]);
    R_MMU_VAtoPA((uint32_t) (tmp_bufferadrs & USB_VAL_FFFFF000), &p_itd->buffer[pg].address);
 #else
    tmp_bufferadrs            = (uint32_t) &p_itd->tmp_buffer[0][0];
    p_itd->buffer[pg].address = (tmp_bufferadrs & USB_VAL_FFFFF000);
 #endif

    for (n = 0; n < 8; n++)
    {
 #if 0
        R_MMU_VAtoPA((uint32_t) (tmp_bufferadrs & USB_VAL_FFF), p_itd->transaction[n].bit.offset); /* Offset */
 #else
        p_itd->transaction[n].bit.offset = (tmp_bufferadrs & USB_VAL_FFF);                         /* Offset */
 #endif
        p_itd->transaction[n].bit.pg  = pg & 0x07U;                                                /* PG */
        p_itd->transaction[n].bit.ioc = ioc & 0x00000001U;                                         /* Interrupt On Completion */

        address         = tmp_bufferadrs & USB_VAL_FFFFF000;
        tmp_bufferadrs += USB_EHCI_ITD_DATA_SIZE;
        if (address != (tmp_bufferadrs & USB_VAL_FFFFF000))
        {
            pg++;
 #if 0
            R_MMU_VAtoPA((uint32_t) (tmp_bufferadrs & USB_VAL_FFFFF000), &p_itd->buffer[pg].address);
 #else
            p_itd->buffer[pg].address = (tmp_bufferadrs & USB_VAL_FFFFF000);
 #endif
        }
    }

    p_itd->buffer[0].bit0.endpt                 = p_tr_req->bit.epnum;     /* endpoint no */
    p_itd->buffer[0].bit0.device_address        = p_tr_req->bit.devadrs;   /* device address */
    p_itd->buffer[1].bit1.direction             = p_tr_req->bit.direction; /* direction */
    p_itd->buffer[1].bit1.maximum_packet_length = p_tr_req->bit.mps;       /* max packet size */
    p_itd->buffer[2].bit2.multi                 = multi & 0x00000003U;     /* multi */

    p_itd->info.direction       = p_tr_req->bit.direction;
    p_itd->info.pollrate        = p_tr_req->pollrate;
    p_itd->info.pollrate_offset = pollrate_offset & USB_VAL_3FFF;
    p_itd->next_setup_uframe    = pollrate_offset;
    p_itd->next_active_uframe   = pollrate_offset;
    p_itd->next_trend_uframe    = pollrate_offset;
}                                      /* End of function usb_hstd_ehci_init_itd() */

/***********************************************************************************************************************
 * Function     : Start ITD
 * Declaration  : void usb_hstd_ehci_start_itd( st_usb_hci_tr_req_t *tr_req )
 * Argument     : st_usb_hci_tr_req_t    *tr_req     ; Transfer Request Infomation
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_start_itd (st_usb_hci_tr_req_t * p_tr_req)
{
    st_usb_ehci_itd_t * p_itd;
    uint32_t            i;
    uint32_t            trsize;
    uint32_t            remain;
    uint8_t           * p_databuf;
    uint8_t           * tmp_itd_duffer;
 #if 0
    p_itd = (st_usb_ehci_itd_t *) r_usb_pa_to_va((uint32_t) p_tr_req->hci_info);
 #else
    p_itd = (st_usb_ehci_itd_t *) p_tr_req->hci_info;
 #endif

    /* Transfer Setting */
    remain = p_tr_req->trsize;
    for (i = 0; i < (8 / p_itd->info.pollrate); i++)
    {
        /* Set TransferSize */
        if (remain > p_tr_req->bit.mps)
        {
            /* MaxPacketSize */
            trsize = p_tr_req->bit.mps;
        }
        else
        {
            /* FinalSize */
            trsize = remain;
        }

        /* Update remain size */
        remain -= trsize;

        /* Set Source Address */
 #if 0
        p_databuf = (uint8_t *) r_usb_pa_to_va(((p_tr_req->databuf) + (p_tr_req->bit.mps * i)));
 #else
        p_databuf = (uint8_t *) (p_tr_req->databuf + (p_tr_req->bit.mps * i));
 #endif

        /* Copy OUT data */
        if (USB_HCI_DIRECTION_OUT == p_itd->info.direction)
        {
 #if 0
            tmp_itd_duffer = (uint8_t *) r_usb_pa_to_va(&p_itd->tmp_buffer[p_itd->next_setup_uframe][0]);
 #else
            tmp_itd_duffer = &p_itd->tmp_buffer[p_itd->next_setup_uframe][0];
 #endif
            memcpy(tmp_itd_duffer, p_databuf, trsize);
        }

        /* Set Transaction */
        p_itd->transaction[p_itd->next_setup_uframe].bit.length = trsize & USB_VAL_FFF;
        p_itd->transaction[p_itd->next_setup_uframe].bit.status_data_buffer_error = 0;
        p_itd->transaction[p_itd->next_setup_uframe].bit.status_babble_detected   = 0;
        p_itd->transaction[p_itd->next_setup_uframe].bit.status_transaction_error = 0;
        p_itd->transaction[p_itd->next_setup_uframe].bit.status_active            = 0;

        /* Data End */
        if (0 == remain)
        {
            /* Set Interrupt On Completion */
            p_itd->transaction[p_itd->next_setup_uframe].bit.ioc = 1;
            break;
        }

        /* Update Transaction index */
        p_itd->next_setup_uframe += p_itd->info.pollrate;
    }

    /* Reset Counter */
    p_itd->next_setup_uframe = p_itd->info.pollrate_offset;

    /* Set Transfer Request Flag */
    p_itd->info.tr_req_flag = TRUE;

    /* Setting Active Flag */
    for (i = 0; i < (8 / p_itd->info.pollrate); i++)
    {
        /* Check data is set */
        if (p_itd->transaction[p_itd->next_active_uframe].bit.length > 0)
        {
            /* set active flag */
            p_itd->transaction[p_itd->next_active_uframe].bit.status_active = 1;

            /* update transaction end flame */
            p_itd->next_trend_uframe = p_itd->next_active_uframe;
        }

        /* Update Transaction index */
        p_itd->next_active_uframe += p_itd->info.pollrate;
    }

    /* Reset Counter */
    p_itd->next_active_uframe = p_itd->info.pollrate_offset;
}                                      /* End of function usb_hstd_ehci_start_itd() */

/***********************************************************************************************************************
 * Function     : Initialize SITD
 * Declaration  : usb_hstd_ehci_init_sitd( st_usb_hci_tr_req_t *tr_req, st_usb_ehci_sitd_t *sitd, uint32_t pollrate_offset )
 * Argument     : st_usb_hci_tr_req_t *tr_req     ; Transfer Request Infomation
 *              : st_usb_ehci_sitd_t *sitd        ; pointer to siTD
 *              : uint32_t pollrate_offset   ; polling rate offset
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_init_sitd (st_usb_hci_tr_req_t * p_tr_req, st_usb_ehci_sitd_t * sitd,
                                     uint32_t pollrate_offset)
{
    uint32_t bit_vector;
    uint32_t tmp_size;
    uint32_t tr_count;

    sitd->endpoint1.bit.direction      = p_tr_req->bit.direction;           /* direction */
    sitd->endpoint1.bit.port_number    = p_tr_req->devinfo->bit.hubportnum; /* port number */
    sitd->endpoint1.bit.hub_addr       = p_tr_req->devinfo->bit.hubaddr;    /* hub address */
    sitd->endpoint1.bit.endpt          = p_tr_req->bit.epnum;               /* endpoint number */
    sitd->endpoint1.bit.device_address = p_tr_req->bit.devadrs;             /* device address */

    /* set bit vector */
    tmp_size   = p_tr_req->bit.mps;
    bit_vector = 1;
    tr_count   = 1;
    while (tmp_size)
    {
        if (tmp_size < USB_EHCI_SITD_DATA_SIZE)
        {
            tmp_size = 0;
        }
        else
        {
            tmp_size   -= USB_EHCI_SITD_DATA_SIZE;
            bit_vector |= (uint32_t) (1 << tr_count);
            tr_count++;
        }
    }

    if (USB_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
    {
        bit_vector = bit_vector << pollrate_offset;
        sitd->endpoint2.bit.mframe_cmask = 0;                    /* c-mask : 0000 0000 */
        sitd->endpoint2.bit.mframe_smask = (uint8_t) bit_vector; /* s-mask */
    }
    else
    {
        bit_vector = bit_vector << (2 + pollrate_offset);
        sitd->endpoint2.bit.mframe_cmask = (uint8_t) bit_vector;                     /* c-mask */
        sitd->endpoint2.bit.mframe_smask = gs_usb_hstd_ehci_s_mask[pollrate_offset]; /* s-mask */
    }

    sitd->back.bit.t = 1;                                                            /* back pointer */

    sitd->info.pollrate        = p_tr_req->pollrate;
    sitd->info.pollrate_offset = pollrate_offset & USB_VAL_3FFF;
    sitd->info.direction       = p_tr_req->bit.direction;
    sitd->mps = p_tr_req->bit.mps;
}                                      /* End of function usb_hstd_ehci_init_sitd() */

/***********************************************************************************************************************
 * Function     : Set siTD
 * Declaration  : usb_hstd_ehci_set_sitd( st_usb_ehci_sitd_t *sitd, uint32_t totalsize uint32_t direction, uint32_t ioc, uint32_t bufferadrs )
 * Argument     : st_usb_ehci_sitd_t *sitd   ; pointer to siTD
 *              : uint32_t totalsize    ; total transfer size
 *              : uint32_t direction    ; direction
 *              : uint32_t ioc          ; Interrupt On Complete
 *              : uint32_t bufferadrs   ; buffer address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_set_sitd (st_usb_ehci_sitd_t * p_sitd,
                                    uint32_t             totalsize,
                                    uint32_t             direction,
                                    uint32_t             ioc,
                                    uint32_t             bufferadrs)
{
    uint32_t tmp_size;
    uint32_t tr_count;

    if (totalsize > USB_VAL_1023U)
    {
        USB_HCI_PRINTF0_R(
            "usb_hstd_ehci_set_sitd Error: Transfer Request Size over than USB_VAL_1023U, force set USB_VAL_1023U\n");
        totalsize = USB_VAL_1023U;
    }

    /* set bit vector */
    tmp_size = totalsize;
    tr_count = 1;
    while (tmp_size)
    {
        if (tmp_size < USB_EHCI_SITD_DATA_SIZE)
        {
            tmp_size = 0;
        }
        else
        {
            tmp_size -= USB_EHCI_SITD_DATA_SIZE;
            tr_count++;
        }
    }

    p_sitd->state.dword                       = 0;                       /* state clear */
    p_sitd->state.bit.ioc                     = ioc & 0x00000001U;       /* interrupt on completion */
    p_sitd->state.bit.page_select             = 0;                       /* page select */
    p_sitd->state.bit.total_bytes_to_transfer = totalsize & USB_VAL_3FF; /* total size */
 #if 0
    R_MMU_VAtoPA(bufferadrs, &p_sitd->buffer[0].address);
    R_MMU_VAtoPA((uint32_t) ((bufferadrs & USB_VAL_FFFFF000) + USB_VAL_1000), &p_sitd->buffer[1].address);
 #else
    p_sitd->buffer[0].address = bufferadrs;
    p_sitd->buffer[1].address = ((bufferadrs & USB_VAL_FFFFF000) + USB_VAL_1000);
 #endif

    if (USB_HCI_DIRECTION_OUT == direction)
    {
        if (totalsize <= USB_EHCI_SITD_DATA_SIZE)
        {
            p_sitd->buffer[1].bit1.tp = USB_EHCI_TP_ALL;         /* TP = 0(ALL) */
        }
        else
        {
            p_sitd->buffer[1].bit1.tp = USB_EHCI_TP_BEGIN;       /* TP = 1(BEGIN) */
        }

        p_sitd->buffer[1].bit1.t_count = tr_count & 0x00000007U; /* T-Count */
    }
} /* End of function usb_hstd_ehci_set_sitd() */

/***********************************************************************************************************************
 * Function     : Start SITD
 * Declaration  : void usb_hstd_ehci_start_sitd( st_usb_ehci_sitd_t *sitd )
 * Argument     : st_usb_hci_tr_req_t *tr_req     ; Transfer Request Infomation
 *              : st_usb_ehci_sitd_t *sitd        ; pointer to siTD
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
static void usb_hstd_ehci_start_sitd (st_usb_ehci_sitd_t * p_sitd)
{
    p_sitd->info.tr_req_flag        = TRUE; /* Set Transfer Request Flag */
    p_sitd->state.bit.status_active = 1;    /* Set Active Flag */
} /* End of function usb_hstd_ehci_start_sitd() */

#endif /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
