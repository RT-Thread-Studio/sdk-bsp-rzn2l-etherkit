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
 * File Name    : r_usb_hEhciExtern.c
 * Description  : EHCI Extern Header
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

#ifndef R_USB_HEHCIEXTERN_H
#define R_USB_HEHCIEXTERN_H

/*======================================================================== */
/*                      Prototypes                                         */
/*======================================================================== */
/* function */
/* r_usb_hEhciMain.c */
uint32_t usb_hstd_ehci_init(void);
void     usb_hstd_ehci_deinit(void);

uint32_t usb_hstd_ehci_port_reset(uint32_t portnum);
void     usb_hstd_ehci_disconnect(uint32_t devadd);
uint32_t usb_hstd_ehci_get_frame_list_size(void);
void     usb_hstd_ehci_wait_micro_frame(uint32_t uframe_no);
void     usb_hstd_ehci_add_async(st_usb_ehci_qh_t * qh);
void     usb_hstd_ehci_add_periodic(u_usb_ehci_flep_t * new_element,
                                    uint32_t            type,
                                    uint32_t            pollrate,
                                    uint32_t            pollrate_offset);
void     usb_hstd_ehci_link_qtd(st_usb_ehci_qtd_t * qtd1st, st_usb_ehci_qtd_t * qtd2nd);
void     usb_hstd_ehci_clear_qtd(st_usb_ehci_qtd_t * qtd_head);
void     usb_hstd_ehci_inactive_qtd(st_usb_ehci_qtd_t * qtd_head);
void     usb_hstd_ehci_unlink_qh_for_async(st_usb_ehci_qh_t * qh);
void     usb_hstd_ehci_unlink_qh_for_periodic(st_usb_ehci_qh_t * qh);
void     usb_hstd_ehci_unlink_itd(st_usb_ehci_itd_t * itd);
void     usb_hstd_ehci_unlink_sitd(st_usb_ehci_sitd_t * sitd);
uint32_t usb_hstd_ehci_update_periodic_scheduling_score(uint32_t max_period, uint32_t pollrate);
void     usb_hstd_ehci_int_transfer_end(usb_utr_t * ptr);
void     usb_hstd_ehci_int_port_change(usb_utr_t * ptr);
uint32_t usb_hstd_ehci_get_port_current_connect_status(uint16_t rootport);
void     usb_hstd_ehci_clear_enumalation_wait_flag(void);
void     usb_hstd_ehci_interrupt_handler(usb_utr_t * ptr);
uint16_t usb_hstd_ehci_get_device_address_of_rootpoot(uint16_t rootport);
uint16_t usb_hstd_ehci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t deviceaddr);
uint16_t usb_hstd_ehci_get_transfer_end_status(st_usb_hci_tr_req_t * tr_req);
uint16_t usb_hstd_ehci_get_pid_status(st_usb_hci_tr_req_t * tr_req);
void     usb_hstd_ehci_port_suspend(uint16_t port);
void     usb_hstd_ehci_port_resume(uint16_t port);
void     usb_hstd_ehci_cancel_transfer_request(st_usb_hci_tr_req_t * tr_req);

/* r_usb_hEhciMemory.c */
void                 usb_hstd_ehci_init_qh_memory(void);
void                 usb_hstd_ehci_init_itd_memory(void);
void                 usb_hstd_ehci_init_sitd_memory(void);
void                 usb_hstd_ehci_init_qtd_memory(void);
void                 usb_hstd_ehci_init_periodic_frame_list(void);
uint32_t             usb_hstd_ehci_get_periodic_frame_list_addr(void);
st_usb_ehci_qh_t   * usb_hstd_ehci_alloc_qh(void);
st_usb_ehci_itd_t  * usb_hstd_ehci_alloc_itd(void);
st_usb_ehci_sitd_t * usb_hstd_ehci_alloc_sitd(void);
st_usb_ehci_qtd_t  * usb_hstd_ehci_alloc_qtd(void);
void                 usb_hstd_ehci_free_qtd(st_usb_ehci_qtd_t * free_qtd);
st_usb_ehci_qh_t   * usb_hstd_ehci_search_qh(uint32_t devadd, uint32_t epnum, uint32_t direction, uint32_t eptype);
st_usb_ehci_itd_t  * usb_hstd_ehci_search_itd(uint32_t devadd, uint32_t epnum, uint32_t direction);
st_usb_ehci_sitd_t * usb_hstd_ehci_search_sitd(uint32_t devadd, uint32_t epnum, uint32_t direction);
void                 usb_hstd_ehci_clear_device_qh(uint32_t devadd);
void                 usb_hstd_ehci_clear_qh(st_usb_ehci_qh_t * qh);
void                 usb_hstd_ehci_clear_device_itd(uint32_t devadd);
void                 usb_hstd_ehci_clear_device_sitd(uint32_t devadd);

/* r_usb_hEhciTransfer.c */
void usb_hstd_ehci_make_transfer_request(st_usb_hci_tr_req_t * p_tr_req);

void usb_hstd_ehci_transfer_end_qh(usb_utr_t * ptr, st_usb_hci_tr_req_t * p_tr_req);
void usb_hstd_ehci_transfer_end_itd(usb_utr_t * ptr, st_usb_hci_tr_req_t * p_tr_req);
void usb_hstd_ehci_transfer_end_sitd(usb_utr_t * ptr, st_usb_hci_tr_req_t * p_tr_req);

#endif                                 /* R_USB_HEHCIEXTERN_H */
