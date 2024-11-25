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
 * File Name    : r_usb_hEhciMemory.c
 * Description  : EHCI Memory Control Code
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

BSP_ALIGN_VARIABLE(4096) static uint32_t gs_usb_hstd_ehci_periodic_frame_list[USB_EHCI_PFL_SIZE]
USB_BUFFER_PLACE_IN_SECTION;                                                                                           /* 4KByte alignment */
BSP_ALIGN_VARIABLE(32) static st_usb_ehci_qh_t gs_usb_hstd_ehci_qh[USB_EHCI_NUM_QH] USB_BUFFER_PLACE_IN_SECTION;       /* 32Byte alignment */
BSP_ALIGN_VARIABLE(32) static st_usb_ehci_qtd_t gs_usb_hstd_ehci_qtd[USB_EHCI_NUM_QTD] USB_BUFFER_PLACE_IN_SECTION;    /* 32Byte alignment */
BSP_ALIGN_VARIABLE(32) static st_usb_ehci_itd_t gs_usb_hstd_ehci_itd[USB_EHCI_NUM_ITD] USB_BUFFER_PLACE_IN_SECTION;    /* 32Byte alignment */
BSP_ALIGN_VARIABLE(32) static st_usb_ehci_sitd_t gs_usb_hstd_ehci_sitd[USB_EHCI_NUM_SITD] USB_BUFFER_PLACE_IN_SECTION; /* 32Byte alignment */

static st_usb_ehci_qtd_t * gsp_usb_hstd_ehci_qtd_top;

/***********************************************************************************************************************
 * Function     : Initialize QH
 * Declaration  : usb_hstd_ehci_init_qh_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_init_qh_memory (void)
{
    usb_hstd_hci_lock_resouce();

    memset(&gs_usb_hstd_ehci_qh[0], 0, sizeof(gs_usb_hstd_ehci_qh));

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ehci_init_qh_memory() */

/***********************************************************************************************************************
 * Function     : Initialize iTD
 * Declaration  : usb_hstd_ehci_init_itd_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_init_itd_memory (void)
{
    usb_hstd_hci_lock_resouce();

    memset(&gs_usb_hstd_ehci_itd[0], 0, sizeof(gs_usb_hstd_ehci_itd));

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ehci_init_itd_memory() */

/***********************************************************************************************************************
 * Function     : Initialize siTD
 * Declaration  : usb_hstd_ehci_init_sitd_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_init_sitd_memory (void)
{
    usb_hstd_hci_lock_resouce();

    memset(&gs_usb_hstd_ehci_sitd[0], 0, sizeof(gs_usb_hstd_ehci_sitd));

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ehci_init_sitd_memory() */

/***********************************************************************************************************************
 * Function     : Initialize qTD
 * Declaration  : usb_hstd_ehci_init_qtd_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_init_qtd_memory (void)
{
    st_usb_ehci_qtd_t * p_qtd;
    uint32_t            n;

    usb_hstd_hci_lock_resouce();

    gsp_usb_hstd_ehci_qtd_top = NULL;
    memset(&gs_usb_hstd_ehci_qtd[0], 0, sizeof(gs_usb_hstd_ehci_qtd));

    usb_hstd_hci_unlock_resouce();

    p_qtd = &gs_usb_hstd_ehci_qtd[0];

    for (n = 0; n < USB_EHCI_NUM_QTD; n++, p_qtd++)
    {
        usb_hstd_ehci_free_qtd(p_qtd);
    }
}                                      /* End of function usb_hstd_ehci_init_qtd_memory() */

/***********************************************************************************************************************
 * Function     : Initialize Periodic Frame List
 * Declaration  : usb_hstd_ehci_init_periodic_frame_list( void )
 * Argument     : void
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_init_periodic_frame_list (void)
{
    uint32_t * p_list;
    uint32_t   n;

    p_list = &gs_usb_hstd_ehci_periodic_frame_list[0];
    for (n = 0; n < USB_EHCI_PFL_SIZE; n++)
    {
        *p_list++ = 0x00000001;
    }
}                                      /* End of function usb_hstd_ehci_init_periodic_frame_list() */

/***********************************************************************************************************************
 * Function     : Get Periodic Frame List Address
 * Declaration  : usb_hstd_ehci_get_periodic_frame_list_addr( void )
 * Argument     : void
 * Return       : uint32_t ; Priodic Frame List Address
 * Note         :
 ***********************************************************************************************************************/
uint32_t usb_hstd_ehci_get_periodic_frame_list_addr (void)
{
    return (uint32_t) &gs_usb_hstd_ehci_periodic_frame_list[0];
}                                      /* End of function usb_hstd_ehci_get_periodic_frame_list_addr() */

/***********************************************************************************************************************
 * Function     : Allocate QH
 * Declaration  : usb_hstd_ehci_alloc_qh(void)
 * Argument     : void
 * Return       : st_usb_ehci_qh_t* ; QH Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_qh_t * usb_hstd_ehci_alloc_qh (void)
{
    st_usb_ehci_qh_t * p_ret_qh = NULL;
    st_usb_ehci_qh_t * p_qh;
    uint32_t           n;

    usb_hstd_hci_lock_resouce();

    p_qh = &gs_usb_hstd_ehci_qh[0];
    for (n = 0; n < USB_EHCI_NUM_QH; n++, p_qh++)
    {
        if (FALSE == p_qh->info.enable)
        {
            p_qh->info.enable                = TRUE;
            p_qh->next_qtd.address           = 0x00000001;
            p_qh->alternate_next_qtd.address = 0x00000001;
            p_qh->qtd_head = NULL;
            p_qh->qtd_end  = NULL;

            p_ret_qh = p_qh;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    if (NULL == p_ret_qh)
    {
        USB_HCI_PRINTF0_R("-----------------------------------------------\n");
        USB_HCI_PRINTF0_R("Memory Allocation Error: usb_hstd_ehci_alloc_qh\n");
        USB_HCI_PRINTF0_R("-----------------------------------------------\n");
    }

    return p_ret_qh;
}                                      /* End of function usb_hstd_ehci_alloc_qh() */

/***********************************************************************************************************************
 * Function     : Allocate iTD
 * Declaration  : usb_hstd_ehci_alloc_itd(void)
 * Argument     : void
 * Return       : st_usb_ehci_itd_t* ; ITD Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_itd_t * usb_hstd_ehci_alloc_itd (void)
{
    st_usb_ehci_itd_t * p_ret_itd = NULL;
    st_usb_ehci_itd_t * p_itd;
    uint32_t            n;

    usb_hstd_hci_lock_resouce();

    p_itd = &gs_usb_hstd_ehci_itd[0];
    for (n = 0; n < USB_EHCI_NUM_ITD; n++, p_itd++)
    {
        if (FALSE == p_itd->info.enable)
        {
            p_itd->info.enable = TRUE;

            p_ret_itd = p_itd;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    if (NULL == p_ret_itd)
    {
        USB_HCI_PRINTF0_R("------------------------------------------------\n");
        USB_HCI_PRINTF0_R("Memory Allocation Error: usb_hstd_ehci_alloc_itd\n");
        USB_HCI_PRINTF0_R("------------------------------------------------\n");
    }

    return p_ret_itd;
}                                      /* End of function usb_hstd_ehci_alloc_itd() */

/***********************************************************************************************************************
 * Function     : Allocate siTD
 * Declaration  : usb_hstd_ehci_alloc_sitd(void)
 * Argument     : void
 * Return       : st_usb_ehci_sitd_t* ; SITD Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_sitd_t * usb_hstd_ehci_alloc_sitd (void)
{
    st_usb_ehci_sitd_t * p_ret_sitd = NULL;
    st_usb_ehci_sitd_t * p_sitd;
    uint32_t             n;

    usb_hstd_hci_lock_resouce();

    p_sitd = &gs_usb_hstd_ehci_sitd[0];
    for (n = 0; n < USB_EHCI_NUM_SITD; n++, p_sitd++)
    {
        if (FALSE == p_sitd->info.enable)
        {
            p_sitd->info.enable = TRUE;

            p_ret_sitd = p_sitd;
            break;
        }
    }

    usb_hstd_hci_unlock_resouce();

    if (NULL == p_ret_sitd)
    {
        USB_HCI_PRINTF0_R("-------------------------------------------------\n");
        USB_HCI_PRINTF0_R("Memory Allocation Error: usb_hstd_ehci_alloc_sitd\n");
        USB_HCI_PRINTF0_R("-------------------------------------------------\n");
    }

    return p_ret_sitd;
}                                      /* End of function usb_hstd_ehci_alloc_sitd() */

/***********************************************************************************************************************
 * Function     : Allocate qTD
 * Declaration  : usb_hstd_ehci_alloc_qtd(void)
 * Argument     : void
 * Return       : st_usb_ehci_qtd_t* ; QTD Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_qtd_t * usb_hstd_ehci_alloc_qtd (void)
{
    st_usb_ehci_qtd_t * p_ret_qtd = NULL;

    usb_hstd_hci_lock_resouce();

    if (NULL != gsp_usb_hstd_ehci_qtd_top)
    {
 #if 0
        p_ret_qtd = (st_usb_ehci_qtd_t *) r_usb_pa_to_va(gsp_usb_hstd_ehci_qtd_top);

        gsp_usb_hstd_ehci_qtd_top = (st_usb_ehci_qtd_t *) r_usb_pa_to_va(
            (uint32_t) gsp_usb_hstd_ehci_qtd_top->next_qtd.address);
 #else
        p_ret_qtd = gsp_usb_hstd_ehci_qtd_top;

        gsp_usb_hstd_ehci_qtd_top = (st_usb_ehci_qtd_t *) gsp_usb_hstd_ehci_qtd_top->next_qtd.address;
 #endif
    }

    usb_hstd_hci_unlock_resouce();

    if (NULL == p_ret_qtd)
    {
        USB_HCI_PRINTF0_R("------------------------------------------------\n");
        USB_HCI_PRINTF0_R("Memory Allocation Error: usb_hstd_ehci_alloc_qtd\n");
        USB_HCI_PRINTF0_R("------------------------------------------------\n");
    }

    return p_ret_qtd;
}                                      /* End of function usb_hstd_ehci_alloc_qtd() */

/***********************************************************************************************************************
 * Function     : Free qTD
 * Declaration  : usb_hstd_ehci_free_qtd( st_usb_ehci_qtd_t *free_qtd )
 * Argument     : st_usb_ehci_qtd_t *free_qtd ; QTD Address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_free_qtd (st_usb_ehci_qtd_t * p_free_qtd)
{
    usb_hstd_hci_lock_resouce();
 #if 0
    R_MMU_VAtoPA(gsp_usb_hstd_ehci_qtd_top, &p_free_qtd->next_qtd.address);
    gsp_usb_hstd_ehci_qtd_top = (st_usb_ehci_qtd_t *) r_usb_pa_to_va(p_free_qtd);
 #else
    p_free_qtd->next_qtd.address = (uint32_t) gsp_usb_hstd_ehci_qtd_top;
    gsp_usb_hstd_ehci_qtd_top    = p_free_qtd;
 #endif

    usb_hstd_hci_unlock_resouce();
}                                      /* End of function usb_hstd_ehci_free_qtd() */

/***********************************************************************************************************************
 * Function     : Search QH
 * Declaration  : usb_hstd_ehci_search_qh( uint32_t devadd, uint32_t epnum, uint32_t direction, uint32_t eptype )
 * Argument     : uint32_t devadd       ; Device Address
 *              : uint32_t epnum        ; Endpoint Number
 *              : uint32_t direction    ; Direction
 *              : uint32_t eptype       ; Endpoint Type
 * Return       : st_usb_ehci_qh_t*          ; QH Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_qh_t * usb_hstd_ehci_search_qh (uint32_t devadd, uint32_t epnum, uint32_t direction, uint32_t eptype)
{
    st_usb_ehci_qh_t * p_ret_qh = NULL;
    st_usb_ehci_qh_t * p_qh;
    uint32_t           n;

    p_qh = &gs_usb_hstd_ehci_qh[1];
    for (n = 1; n < USB_EHCI_NUM_QH; n++, p_qh++)
    {
        if (TRUE == p_qh->info.enable)
        {
            if ((p_qh->endpoint1.bit.device_address == devadd) && (p_qh->endpoint1.bit.endpt == epnum))
            {
                if ((USB_EP_CNTRL == eptype) || (p_qh->info.direction == direction))
                {
                    p_ret_qh = p_qh;
                }
            }
        }
    }

    return p_ret_qh;
}                                      /* End of function usb_hstd_ehci_search_qh() */

/***********************************************************************************************************************
 * Function     : Search ITD
 * Declaration  : usb_hstd_ehci_search_itd( uint32_t devadd, uint32_t epnum, uint32_t direction )
 * Argument     : uint32_t devadd       ; Device Address
 *              : uint32_t epnum        ; Endpoint Number
 *              : uint32_t direction    ; Direction
 * Return       : st_usb_ehci_itd_t*         ; ITD Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_itd_t * usb_hstd_ehci_search_itd (uint32_t devadd, uint32_t epnum, uint32_t direction)
{
    st_usb_ehci_itd_t * p_ret_itd = NULL;
    st_usb_ehci_itd_t * p_itd;
    uint32_t            n;

    p_itd = &gs_usb_hstd_ehci_itd[0];
    for (n = 0; n < USB_EHCI_NUM_ITD; n++, p_itd++)
    {
        if (TRUE == p_itd->info.enable)
        {
            if ((p_itd->buffer[0].bit0.device_address == devadd) &&
                (p_itd->buffer[0].bit0.endpt == epnum) &&
                (p_itd->buffer[1].bit1.direction == direction))
            {
                p_ret_itd = p_itd;
            }
        }
    }

    return p_ret_itd;
}                                      /* End of function usb_hstd_ehci_search_itd() */

/***********************************************************************************************************************
 * Function     : Search SITD
 * Declaration  : usb_hstd_ehci_search_sitd( uint32_t devadd, uint32_t epnum, uint32_t direction )
 * Argument     : uint32_t devadd       ; Device Address
 *              : uint32_t epnum        ; Endpoint Number
 *              : uint32_t direction    ; Direction
 * Return       : st_usb_ehci_sitd_t*        ; SITD Address
 * Note         :
 ***********************************************************************************************************************/
st_usb_ehci_sitd_t * usb_hstd_ehci_search_sitd (uint32_t devadd, uint32_t epnum, uint32_t direction)
{
    st_usb_ehci_sitd_t * p_ret_sitd = NULL;
    st_usb_ehci_sitd_t * p_sitd;
    uint32_t             n;

    p_sitd = &gs_usb_hstd_ehci_sitd[0];
    for (n = 0; n < USB_EHCI_NUM_SITD; n++, p_sitd++)
    {
        if (TRUE == p_sitd->info.enable)
        {
            if ((p_sitd->endpoint1.bit.device_address == devadd) &&
                (p_sitd->endpoint1.bit.endpt == epnum) &&
                (p_sitd->endpoint1.bit.direction == direction))
            {
                p_ret_sitd = p_sitd;
            }
        }
    }

    return p_ret_sitd;
}                                      /* End of function usb_hstd_ehci_search_sitd() */

/***********************************************************************************************************************
 * Function     : Clear QH for disconnected device
 * Declaration  : usb_hstd_ehci_clear_device_qh( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_clear_device_qh (uint32_t devadd)
{
    st_usb_ehci_qh_t * p_qh;
    uint32_t           n;

    p_qh = &gs_usb_hstd_ehci_qh[1];
    for (n = 1; n < USB_EHCI_NUM_QH; n++, p_qh++)
    {
        if (TRUE == p_qh->info.enable)
        {
            if (p_qh->endpoint1.bit.device_address == devadd)
            {
                usb_hstd_ehci_clear_qh(p_qh);
            }
        }
    }
}                                      /* End of function usb_hstd_ehci_clear_device_qh() */

/***********************************************************************************************************************
 * Function     : Clear QH
 * Declaration  : void usb_hstd_ehci_clear_qh( st_usb_ehci_qh_t *qh )
 * Argument     : st_usb_ehci_qh_t *qh ; QH Address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_clear_qh (st_usb_ehci_qh_t * p_qh)
{
    if (NULL != p_qh)
    {
        if (TRUE == p_qh->info.enable)
        {
            /* Unlink qh */
            if (0 != p_qh->info.pollrate)
            {
                /* Interrupt */
                usb_hstd_ehci_unlink_qh_for_periodic(p_qh);
            }
            else
            {
                /* Control/Bulk */
                usb_hstd_ehci_unlink_qh_for_async(p_qh);
            }

            /* Clear qh( enable = FALSE ) */
            memset(p_qh, 0, sizeof(st_usb_ehci_qh_t));
        }
    }
}                                      /* End of function usb_hstd_ehci_clear_qh() */

/***********************************************************************************************************************
 * Function     : Clear ITD for disconnected device
 * Declaration  : usb_hstd_ehci_clear_device_itd( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_clear_device_itd (uint32_t devadd)
{
    st_usb_ehci_itd_t * p_itd;
    uint32_t            n;

    p_itd = &gs_usb_hstd_ehci_itd[0];
    for (n = 0; n < USB_EHCI_NUM_ITD; n++, p_itd++)
    {
        if (TRUE == p_itd->info.enable)
        {
            if (p_itd->buffer[0].bit0.device_address == devadd)
            {
                /* Unlink itd */
                usb_hstd_ehci_unlink_itd(p_itd);

                /* Clear itd( enable = FALSE ) */
                memset(p_itd, 0, sizeof(st_usb_ehci_itd_t));
            }
        }
    }
}                                      /* End of function usb_hstd_ehci_clear_device_itd() */

/***********************************************************************************************************************
 * Function     : Clear SITD for disconnected device
 * Declaration  : usb_hstd_ehci_clear_device_sitd( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
 ***********************************************************************************************************************/
void usb_hstd_ehci_clear_device_sitd (uint32_t devadd)
{
    st_usb_ehci_sitd_t * p_sitd;
    uint32_t             n;

    p_sitd = &gs_usb_hstd_ehci_sitd[0];
    for (n = 0; n < USB_EHCI_NUM_SITD; n++, p_sitd++)
    {
        if (TRUE == p_sitd->info.enable)
        {
            if (p_sitd->endpoint1.bit.device_address == devadd)
            {
                /* Unlink sitd */
                usb_hstd_ehci_unlink_sitd(p_sitd);

                /* Clear sitd( enable = FALSE ) */
                memset(p_sitd, 0, sizeof(st_usb_ehci_sitd_t));
            }
        }
    }
}                                      /* End of function usb_hstd_ehci_clear_device_sitd() */

#endif /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
