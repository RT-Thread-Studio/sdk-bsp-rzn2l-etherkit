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
 * File Name    : r_usb_hdriver.c
 * Description  : USB Host Control Driver
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include "r_usb_basic_local.h"
#include "r_usb_hhci.h"
#include "r_usb_extern.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * External variables and functions
 ***********************************************************************************************************************/
#if USB_IP_EHCI_OHCI == 1

 #define USB_DATA_DIR_IN     (0x80U)
 #define USB_DATA_DIR_OUT    (0x00U)

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/
void usb_memcpy(uint32_t addr1, uint32_t addr2, uint32_t size);

st_usb_pipe_t g_usb_hstd_pipe[USB_MAXPIPE + 1U];          /* pipe information */

/***********************************************************************************************************************
 * Static variables and functions
 ***********************************************************************************************************************/
static uint16_t gs_usb_hstd_dev_addr[USB_MAXDEVADDR + 1]; /* DEVADD table */

/***********************************************************************************************************************
 * Description     : Get End point number from pipe number
 * Function Name   : uint16_t usb_hstd_get_epnum(uint16_t pipe_id)
 * Arguments       : uint16_t pipe_id  : Pipe Number
 * Return value    : uint16_t endpoint number
 ***********************************************************************************************************************/
uint16_t usb_hstd_get_epnum (uint16_t pipe_id)
{
    return g_usb_hstd_pipe[pipe_id].epnum;
}                                      /* End of function usb_hstd_get_epnum() */

/***********************************************************************************************************************
 * Description     : Get End point number from pipe number
 * Function Name   : uint16_t usb_hstd_get_epnum(uint16_t pipe_id)
 * Arguments       : uint16_t pipe_id  : Pipe Number
 * Return value    : uint16_t endpoint number
 ***********************************************************************************************************************/
uint8_t usb_hstd_convert_epnum (uint8_t pipe_id)
{
    return (uint8_t) g_usb_pipe_table[0][pipe_id].pipe_cfg & 0x0F;
}                                      /* End of function usb_hstd_get_epnum() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_get_max_packet_size
 * Description     : Fetch MaxPacketSize of the specified pipe.
 * Arguments       : uint16_t pipe_id  : Pipe number.
 * Return value    : uint16_t MaxPacketSize
 ***********************************************************************************************************************/
uint16_t usb_hstd_get_max_packet_size (uint16_t pipe_id)
{
    return g_usb_hstd_pipe[pipe_id].mps;
}                                      /* End of function usb_hstd_get_max_packet_size() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_get_dev_addr
 * Description     : Get device address from pipe number
 * Arguments       : uint16_t pipe_id  : Pipe number.
 * Return value    : uint16_t DEVSEL-bit status
 ***********************************************************************************************************************/
uint16_t usb_hstd_get_dev_addr (uint16_t pipe_id)
{
    return g_usb_hstd_pipe[pipe_id].devaddr;
}                                      /* End of function usb_hstd_get_dev_addr() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_get_pipe_dir
 * Description     : Get PIPE DIR
 * Arguments       : uint16_t pipe_id  : Pipe number.
 * Return value    : uint16_t pipe_id direction.
 ***********************************************************************************************************************/
uint16_t usb_hstd_get_pipe_dir (uint16_t pipe_id)
{
    return g_usb_hstd_pipe[pipe_id].direction;
}                                      /* End of function usb_hstd_get_pipe_dir() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_get_pipe_type
 * Description     : Fetch and return PIPE TYPE.
 * Arguments       : uint16_t pipe_id  : Pipe number.
 * Return value    : uint16_t pipe_id type
 ***********************************************************************************************************************/
uint16_t usb_hstd_get_pipe_type (uint16_t pipe_id)
{
    return g_usb_hstd_pipe[pipe_id].type;
}                                      /* End of function usb_hstd_get_pipe_type() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_dummy_function
 * Description     : dummy function
 * Arguments       : uint16_t data1    : Not used.
 *              : uint16_t data2    : Not used.
 * Return value    : none
 ***********************************************************************************************************************/
 #if 0
void usb_hstd_dummy_function (st_usb_utr_t * p_utr)
{
}                                      /* End of function usb_hstd_dummy_function() */

 #endif

/***********************************************************************************************************************
 * Function Name   : usb_hstd_set_rootport
 * Description     : Set Port Number of the connected USB Device.
 * Arguments       : uint16_t devaddr          : device address
 *              : uint16_t port             : root port
 * Return value    : none
 ***********************************************************************************************************************/
void usb_hstd_set_rootport (uint16_t devaddr, uint16_t rootport)
{
    gs_usb_hstd_dev_addr[devaddr] = rootport;
}                                      /* End of function usb_hstd_set_rootport() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_get_rootport
 * Description     : Get USB port no. set in the USB register based on the speci-
 *              : fied USB Device address.
 * Arguments       : uint16_t devaddr          : device address
 * Return value    : uint16_t                  : root port number
 ***********************************************************************************************************************/
uint16_t usb_hstd_get_rootport (uint16_t devaddr)
{
    return gs_usb_hstd_dev_addr[devaddr];
}                                      /* End of function usb_hstd_get_rootport() */

/***********************************************************************************************************************
 * Function Name   : usb_hstd_mgr_snd_mbx
 * Description     : Send the message to MGR(Manager) task
 * Arguments       : uint16_t msginfo  : Message info.
 *              : uint16_t dat      : Port no.
 *              : uint16_t res      : Result
 * Return value    : none
 ***********************************************************************************************************************/
 #if 0
void usb_hstd_mgr_snd_mbx (uint16_t msginfo, uint16_t dat, uint16_t res)
{
    st_usb_utr_t * p_blf;
    usb_er_t       err;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
    if (USB_OK == err)
    {
        p_blf->msginfo = msginfo;
        p_blf->keyword = dat;
        p_blf->result  = res;

        /* Send message */
        err = USB_SND_MSG(USB_MGR_MBX, (usb_msg_t *) p_blf);
        if (USB_OK != err)
        {
            USB_PRINTF1("### hMgrSndMbx snd_msg error (%ld)\n", err);
            err = USB_REL_BLK(USB_MGR_MPL, p_blf);
            if (USB_OK != err)
            {
                USB_PRINTF1("### hMgrSndMbx rel_blk error (%ld)\n", err);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrSndMbx pget_blk error (%ld)\n", err);
    }
}                                      /* End of function usb_hstd_mgr_snd_mbx() */

/***********************************************************************************************************************
 * Function Name   : void usb_hstd_transfer_end
 * Description     : End of data transfer (IN/OUT)
 * Arguments       : uint16_t pipe_id         ; pipe Number
 * Return value    : usb_er_t  error code  : USB_OK/USB_ERROR.
 ***********************************************************************************************************************/
usb_er_t usb_hstd_transfer_end (uint16_t pipe_id)
{
    uint16_t dev_addr;
    uint16_t ep_num;
    uint16_t pipe_type;
    uint16_t pipe_dir;

    if (pipe_id > USB_MAXPIPE)
    {
        USB_PRINTF1("### R_USB_HstdTransferEnd PipeErr %d\n", pipe_id);

        return USB_ERROR;
    }

    dev_addr = usb_hstd_get_dev_addr(pipe_id);
    if ((USB_DEVICE_0 == dev_addr) && (USB_PIPE0 != pipe_id))
    {
        USB_PRINTF1("### R_USB_HstdTransferEnd AddrErr %d\n", pipe_id);

        return USB_ERROR;
    }

    ep_num    = usb_hstd_get_epnum(pipe_id);
    pipe_type = usb_hstd_get_pipe_type(pipe_id);
    pipe_dir  = usb_hstd_get_pipe_dir(pipe_id);

    if (USB_PIPE0 == pipe_id)
    {
    }
    else
    {
        r_usb_hstd_hci_cancel_transfer_request(dev_addr, ep_num, pipe_type, pipe_dir);
    }

    return USB_OK;
}                                      /* End of function usb_hstd_transfer_end() */

 #endif
 #if (USB_UT_MODE == 0)
  #if USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * Function Name   : usb_hstd_transfer_start
 * Description     : Request HCD to transfer data. HCD will transfer the data
 *                based on the transfer information stored in utrmsg.
 * Arguments       : st_usb_utr_t *utr  : USB system internal structure.
 * Return value    : usb_er_t  error code  : USB_OK/USB_ERROR.
 ***********************************************************************************************************************/
usb_er_t usb_hstd_transfer_start (st_usb_utr_t * p_utr)
{
    usb_er_t err;
    uint16_t pipe_id;
    uint16_t dev_addr;
    uint16_t ep_num;
    uint16_t pipe_type;
    uint16_t pipe_dir;
    uint16_t mps;

    pipe_id = p_utr->keyword;

    if (USB_PIPE0 == pipe_id)
    {
        /* dev_addr = (uint16_t)(p_utr->p_setup->devaddr); */
        dev_addr = p_utr->p_setup[4];  /* << USB_DEVADDRBIT); */
    }
    else
    {
        dev_addr = usb_hstd_get_dev_addr(pipe_id);
    }

    if ((USB_DEVICE_0 == dev_addr) && (USB_PIPE0 != pipe_id))
    {
        USB_PRINTF1("### R_USB_HstdTransferStart not configured %x\n", dev_addr);

        return USB_ERROR;
    }

    if (USB_PIPE0 == pipe_id)
    {
        err = r_usb_hstd_hci_make_transfer_request(p_utr,                                 /* utr */
                                                   p_utr->p_setup,                        /* setup */
                                                   p_utr->p_setup[4],                     /* devaddr << USB_DEVADDRBIT), */
                                                   0,                                     /* epnum */
                                                   USB_EP_CNTRL,                          /* eptype */
                                                   (p_utr->p_setup[0] & USB_DEV_TO_HOST), /* epdir */
                                                   p_utr->tranlen,                        /* tranlen */
                                                   (uint32_t) p_utr->p_tranadr,           /* tranadr */
                                                   0                                      /* mps */
                                                   );
    }
    else
    {
        dev_addr  = usb_hstd_get_dev_addr(pipe_id);
        ep_num    = usb_hstd_get_epnum(pipe_id);
        pipe_type = usb_hstd_get_pipe_type(pipe_id);
        pipe_dir  = usb_hstd_get_pipe_dir(pipe_id);
        mps       = usb_hstd_get_max_packet_size(pipe_id);

        err = r_usb_hstd_hci_make_transfer_request(p_utr,                       /* utr */
                                                   (void *) USB_NULL,           /* setup */
                                                   dev_addr,                    /* devaddr */
                                                   ep_num,                      /* epnum */
                                                   pipe_type,                   /* eptype */
                                                   pipe_dir,                    /* epdir */
                                                   p_utr->tranlen,              /* tranlen */
                                                   (uint32_t) p_utr->p_tranadr, /* tranadr */
                                                   mps                          /* mps */
                                                   );
    }

    return err;
}                                      /* End of function usb_hstd_transfer_start() */

  #endif /* USB_IP_EHCI_OHCI == 1 */
 #endif  /* (USB_UT_MODE == 0) */

/***********************************************************************************************************************
 * Function Name   : void usb_hstd_AttachCb
 * Description     : Attach / Detach Callback Function
 * Arguments       : uint16_t rootport         : rootport number
 * Return value    : void
 ***********************************************************************************************************************/
void usb_hstd_a_or_detach_cb (usb_utr_t * p_utr)
{
    uint16_t connect_inf;
    uint16_t dev_addr;

    /* dev_addr = r_usb_hstd_hci_get_device_address_of_rootpoot(rootport); */
    dev_addr = r_usb_hstd_hci_get_device_address_of_rootpoot(p_utr->ip);
    if (USB_HCI_NODEVICE != dev_addr)  /* Decide USB Line state (ATTACH) */
    {
        connect_inf = USB_ATTACH;
    }
    else                               /* Decide USB Line state (DETACH) */
    {
        connect_inf = USB_DETACH;
        usb_hstd_set_rootport(USB_DEVICEADDR, 0x0000);
        USB_PRINTF0("*** Device address 1 clear.\n");
    }

    usb_hstd_mgr_snd_mbx(p_utr, (uint16_t) USB_MSG_MGR_AORDETACH, p_utr->keyword, connect_inf);
}                                      /* End of function usb_hstd_a_or_detach_cb() */

/***********************************************************************************************************************
 * Function Name   : void usb_hstd_port_reset_cb
 * Description     : Port Reset Callback Function
 * Arguments       : uint16_t rootport     : rootport number
 * Return value    : void
 ***********************************************************************************************************************/
void usb_hstd_port_reset_cb (usb_utr_t * p_utr)
{
    uint16_t dev_addr;
    uint16_t connect_inf = 0;
    uint32_t speed;
    uint16_t rootport = 0;

    /* support speed check */
    dev_addr = r_usb_hstd_hci_get_device_address_of_rootpoot(p_utr->ip);
    if (USB_HCI_NODEVICE == dev_addr)
    {
        connect_inf = USB_NOCONNECT;
        FSP_PARAMETER_NOT_USED(connect_inf);

        return;
    }

    speed = r_usb_hstd_hci_get_device_speed(dev_addr);

    switch (speed)
    {
        case USB_HCI_DEVSPEED_HS:
        {
            connect_inf = USB_HSCONNECT;
            break;
        }

        case USB_HCI_DEVSPEED_FS:
        {
            connect_inf = USB_FSCONNECT;
            break;
        }

        case USB_HCI_DEVSPEED_LS:
        {
            connect_inf = USB_LSCONNECT;
            break;
        }

        default:
        {
            connect_inf = USB_NOCONNECT;
            break;
        }
    }

    usb_hstd_mgr_snd_mbx(p_utr, (uint16_t) USB_MSG_MGR_STATUSRESULT, rootport, connect_inf);
}                                      /* End of function usb_hstd_port_reset_cb() */

/***********************************************************************************************************************
 * Function Name   : void usb_hstd_over_current_cb
 * Description     : Over Current Callback Function
 * Arguments       : uint16_t rootport     : rootport number
 * Return value    : void
 ***********************************************************************************************************************/
void usb_hstd_over_current_cb (void * p_utr)
{
    uint16_t rootport = 0;
    usb_hstd_mgr_snd_mbx(p_utr, (uint16_t) USB_MSG_MGR_OVERCURRENT, rootport, 0U);
}                                      /* End of function usb_hstd_over_current_cb() */

/***********************************************************************************************************************
 * Function Name   : void usb_hstd_transfer_end_cb
 * Description     : Transfer End Callback Function
 * Arguments       : void     *p_utr : UTR Pointer
 *                uint16_t status : Transfer Status
 * Return value    : void
 ***********************************************************************************************************************/
void usb_hstd_transfer_end_cb (usb_utr_t * ptr, void * p_utr, uint32_t actual_size, uint16_t status)
{
    st_usb_utr_t * p_mess;
    uint16_t       devadr = ptr->keyword;
    uint16_t       data2  = 0;

    FSP_PARAMETER_NOT_USED(ptr);

    p_mess = (st_usb_utr_t *) p_utr;

    if (1 == g_data_read_flag)
    {
        if ((p_mess->keyword != USB_PIPE0) && (g_usb_hstd_pipe[p_mess->keyword].direction == USB_DATA_DIR_IN))      /* API USB_Read */
        {
            usb_memcpy(g_data_buf_addr[p_mess->ip][devadr], (uint32_t) p_mess->p_tranadr, p_mess->tranlen);
        }
        else if ((p_mess->keyword == USB_PIPE0) && (g_usb_hstd_pipe[p_mess->keyword].direction == USB_DATA_DIR_IN)) /* API Controll transfer */
        {
            if (0 != g_data_buf_addr[p_mess->ip][p_mess->keyword])
            {
                usb_memcpy(g_data_buf_addr[p_mess->ip][p_mess->keyword], (uint32_t) p_mess->p_tranadr, p_mess->tranlen);
            }
        }
        else
        {
        }

        g_data_read_flag = 0;
    }

    if ((USB_DATA_SHT == status) || (USB_DATA_OK == status))
    {
        p_mess->tranlen -= actual_size;
    }

    /* Status setting */
    p_mess->status = status;

    /* Callback */
    p_mess->complete(p_mess, devadr, data2);
}                                      /* End of function usb_hstd_transfer_end_cb() */

void usb_memcpy (uint32_t addr1, uint32_t addr2, uint32_t size)
{
    memcpy((void *) addr1, (void *) addr2, size);
}

#endif                                 /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
