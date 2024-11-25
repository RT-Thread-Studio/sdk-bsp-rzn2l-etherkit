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
 * File Name    : r_usb_hdriverapi.c
 * Description  : USB Host Control Driver API. USB Host transfer level commands.
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 31.08.2015 1.00    First Release
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include "../driver/inc/r_usb_basic_local.h"
#include "../driver/inc/r_usb_extern.h"

#include "../driver/inc/r_usb_hhci.h"

#if (BSP_CFG_RTOS == 2)
 #include "../driver/inc/r_usb_cstd_rtos.h"
#endif

#if USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * External variables and functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Constant macro definitions
 ***********************************************************************************************************************/
 #define USB_DRV_MAXIMUM_ACCESS_PRV    (1)

/***********************************************************************************************************************
 * Static variables and functions
 ***********************************************************************************************************************/

/* Variable */
static st_usb_hci_cb_info_t cb =
{
    &usb_hstd_a_or_detach_cb,
    &usb_hstd_a_or_detach_cb,
    &usb_hstd_over_current_cb,
    &usb_hstd_port_reset_cb,
    &usb_hstd_transfer_end_cb
};

uint32_t g_usb_sem;

 #if (BSP_CFG_RTOS == 2)
  #if defined(USB_CFG_HMSC_USE)
extern SemaphoreHandle_t SemaphoreHandleRead;
  #endif                               /*  #if defined(USB_CFG_HMSC_USE) */
 #endif                                /* #if (BSP_CFG_RTOS == 2) */

/***********************************************************************************************************************
 * Renesas USB Host Driver API functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdTransferStart
 * Description     : Request HCD to transfer data. HCD will transfer the data
 *                based on the transfer information stored in ptr.
 * Arguments       : st_usb_utr_t *utrmsg    : USB system internal structure.
 * Return          : usb_er_t  error code : USB_OK/USB_ERROR/USB_ERROR.
 ***********************************************************************************************************************/
usb_er_t R_USB_HstdTransferStart (st_usb_utr_t * p_utr)
{
    usb_er_t err;

    if ((p_utr->keyword == 0) && (g_usb_hstd_mgr_mode[p_utr->keyword] == USB_DEFAULT))
    {
        return USB_ERROR;
    }

    err = usb_hstd_transfer_start(p_utr);

    return err;
}                                      /* End of function R_USB_HstdTransferStart() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdTransferEnd
 * Description     : Request HCD to force termination of data transfer.
 * Arguments       : uint16_t pipe_id      : PipeID
 * Return          : usb_er_t error code   : USB_OK etc
 ***********************************************************************************************************************/
 #if 0
usb_er_t R_USB_HstdTransferEnd (uint16_t pipe_id)
{
    usb_er_t err;

    err = usb_hstd_transfer_end(pipe_id);

    return err;
}                                      /* End of function R_USB_HstdTransferEnd() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdDriverRegistration
 * Description     : The HDCD information registered in the class driver structure
 *              : is registered in HCD.
 * Arguments       : st_usb_hcdreg_t *p_callback    : class driver structure
 * Return          : none
 ***********************************************************************************************************************/
usb_er_t R_USB_HstdDriverRegistration (st_usb_hcdreg_t * p_callback)
{
    st_usb_hcdreg_t * p_driver;

    if (g_usb_hstd_device_num <= USB_MAXDEVADDR)
    {
        p_driver = &g_usb_hstd_device_drv[g_usb_hstd_device_num];

        p_driver->rootport = USB_NOPORT;                   /* Root port */
        p_driver->devaddr  = USB_NODEVICE;                 /* Device address */
        p_driver->devstate = USB_DETACHED;                 /* Device state */
        if (USB_NULL == p_callback)
        {
            p_driver->ifclass    = USB_NULL;               /* Interface Class */
            p_driver->p_tpl      = USB_NULL;               /* Target peripheral list */
            p_driver->classcheck = USB_NULL;               /* Driver check */
            p_driver->devconfig  = USB_NULL;               /* Device configured */
            p_driver->devdetach  = USB_NULL;               /* Device detach */
            p_driver->devsuspend = USB_NULL;               /* Device suspend */
            p_driver->devresume  = USB_NULL;               /* Device resume */
        }
        else
        {
            p_driver->ifclass    = p_callback->ifclass;    /* Interface Class */
            p_driver->p_tpl      = p_callback->p_tpl;      /* Target peripheral list */
            p_driver->classcheck = p_callback->classcheck; /* Driver check */
            p_driver->devconfig  = p_callback->devconfig;  /* Device configured */
            p_driver->devdetach  = p_callback->devdetach;  /* Device detach */
            p_driver->devsuspend = p_callback->devsuspend; /* Device suspend */
            p_driver->devresume  = p_callback->devresume;  /* Device resume */
        }

        g_usb_hstd_device_num++;

        USB_PRINTF1("*** Registration driver 0x%02x\n", p_driver->ifclass);

        return USB_OK;
    }
    else
    {
        USB_PRINTF0("### Registration device driver over\n");

        return USB_ERROR;
    }
}                                      /* End of function R_USB_HstdDriverRegistration() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdReturnEnuMGR
 * Description     : Continuous enumeration is requested to MGR task (API for nonOS)
 * Arguments       : uint16_t cls_result   : class check result
 * Return          : none
 ***********************************************************************************************************************/
void R_USB_HstdReturnEnuMGR (uint16_t cls_result)
{
    g_usb_hstd_check_enu_result = cls_result;
    usb_hstd_mgr_snd_mbx(USB_MSG_MGR_SUBMITRESULT, USB_PIPE0, USB_CTRL_END);
}                                      /* End of function R_USB_HstdReturnEnuMGR() */

 #endif

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdChangeDeviceState
 * Description     : Request to change the status of the connected USB Device
 * Arguments       : usb_utr_cb_t complete    : callback function pointer
 *              : uint16_t msginfo         : Message Info
 *              : uint16_t member          : Device address/pipe number
 * Return          : usb_er_t                 : USB_OK etc
 ***********************************************************************************************************************/
usb_er_t R_USB_HstdChangeDeviceState (usb_utr_t * ptr, usb_cb_t complete, uint16_t msginfo, uint16_t member)
{
    usb_utr_t * p_blf;
    usb_er_t    err;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
    if (USB_OK == err)
    {
        USB_PRINTF2("*** member%d : msginfo=%d ***\n", member, msginfo);
        p_blf->msginfo  = msginfo;
        p_blf->keyword  = member;
        p_blf->complete = (usb_cb_t) complete;
        p_blf->ip       = ptr->ip;
        p_blf->ipp      = ptr->ipp;

        /* Send message */
        err = USB_SND_MSG(USB_MGR_MBX, (usb_msg_t *) p_blf);
        if (USB_OK != err)
        {
            USB_PRINTF1("### hMgrChangeDeviceState snd_msg error (%ld)\n", err);
            err = USB_REL_BLK(USB_MGR_MPL, p_blf);
            if (USB_OK != err)
            {
                USB_PRINTF1("### hMgrChangeDeviceState rel_blk error (%ld)\n", err);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
    }

    return err;
}                                      /* End of function R_USB_HstdChangeDeviceState() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdSetPipe
 * Description     : SetPipe
 * Argument        : uint16_t **pp_table : Data table pointer.
 *                  pp_table[0] : Device Descriptor
 *                  pp_table[1] : Configuration Descriptor
 *                  pp_table[2] : Interface Descriptor
 *                  pp_table[3] : Class check result
 *                  pp_table[4] : HUB device speed
 *                  pp_table[5] : Root port
 *                  pp_table[6] : Device speed
 *                  pp_table[7] : Device address
 * Return          : usb_er_t                   : USB_OK etc
 ***********************************************************************************************************************/
usb_er_t R_USB_HstdSetPipe (uint16_t ** pp_table)
{
    uint8_t       * p_condesc;
    uint8_t       * p_desc_offset;
    uint8_t       * p_descriptor;
    uint16_t        total_len;
    uint16_t        index;
    st_usb_pipe_t * p_pipe;
    uint16_t        pipe_id = 0;
    uint8_t         ifnum   = 0;

    /* g_usb_hstd_pipe[0] is Default Control Pipe */
    p_pipe = &g_usb_hstd_pipe[1];

    /* Done ClassCheck */
    *pp_table[3] = USB_OK;

    p_condesc = (uint8_t *) (pp_table[1]);

    /* Configuration Descriptor + Interface Descriptor + Endpoint Descriptor */
    total_len = (uint16_t) ((p_condesc[USB_CON_W_TOTAL_LENGTH_H] << 8U) |
                            (p_condesc[USB_CON_W_TOTAL_LENGTH_L]));
    index = USB_DEV_B_LENGTH;

    p_desc_offset = p_condesc;

    while (index < total_len)
    {
        /* index will always point to the beginning of the Descriptor */
        p_descriptor = &p_desc_offset[index];

        /* Check Descriptor Type */
        switch (p_descriptor[USB_DEV_B_DESCRIPTOR_TYPE])
        {
            case USB_DT_INTERFACE:
            {
                ifnum = p_descriptor[USB_IF_B_INTERFACENUMBER];
                break;
            }

            case USB_DT_ENDPOINT:
            {
                /* Search Empty Pipe */
                while (0 != p_pipe->epnum)
                {
                    p_pipe++;
                    pipe_id++;
                    if (pipe_id > USB_MAXPIPE)
                    {
                        USB_PRINTF0("PIPE_FULL ERROR\n");

                        return USB_ERROR;
                    }
                }

                /* Get value from Endpoint Descriptor */
                p_pipe->epnum     = (p_descriptor[USB_EP_B_ENDPOINTADDRESS] & USB_EP_NUMMASK);
                p_pipe->direction = (p_descriptor[USB_EP_B_ENDPOINTADDRESS] & USB_EP_DIRMASK);
                p_pipe->type      = (p_descriptor[USB_EP_B_ATTRIBUTES] & USB_EP_TRNSMASK);
                p_pipe->mps       = (uint16_t) ((p_descriptor[USB_EP_B_MAXPACKETSIZE_H] << 8U) |
                                                (p_descriptor[USB_EP_B_MAXPACKETSIZE_L]));

                /* Get value from Attach Info */
                p_pipe->devaddr = *pp_table[7];
                p_pipe->ifnum   = ifnum;
                break;
            }

            default:
            {
                break;
            }
        }

        /* Next Descriptor */
        /* index += (uint16_t)(p_desc_offset[index]); */
        index = (uint16_t) (index + p_desc_offset[index]);
    }

    return USB_OK;
}                                      /* End of function R_USB_HstdSetPipe() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdGetPipeID
 * Description     : Return PipeID from PipeTable
 * Argument        : uint16_t devaddr  : Device Address
 *                uint8_t type      : Endpoint Type
 *                uint8_t direction : Endpoint Direction
 *                uint8_t ifnum     : Interface Number
 * Return          : uint16_t : PipeID
 ***********************************************************************************************************************/
 #if 0
uint16_t R_USB_HstdGetPipeID (uint16_t devaddr, uint8_t type, uint8_t direction, uint8_t ifnum)
{
    uint16_t pipe_id;

    for (pipe_id = 0; pipe_id < USB_MAXPIPE; pipe_id++)
    {
        if (g_usb_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            if (g_usb_hstd_pipe[pipe_id].type == type)
            {
                if (g_usb_hstd_pipe[pipe_id].direction == direction)
                {
                    if (0xFF == ifnum)
                    {
                        return pipe_id;
                    }
                    else
                    {
                        if (g_usb_hstd_pipe[pipe_id].ifnum == ifnum)
                        {
                            return pipe_id;
                        }
                    }
                }
            }
        }
    }

    return 0;
}                                      /* End of function R_USB_HstdGetPipeID() */

 #else
uint16_t R_USB_HstdGetPipeID (uint16_t devaddr, uint8_t epnum)
{
    uint16_t pipe_id;

    for (pipe_id = 1; pipe_id < USB_MAXPIPE; pipe_id++)
    {
        if (g_usb_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            if (g_usb_hstd_pipe[pipe_id].epnum == epnum)
            {
                return pipe_id;
            }
        }
    }

    return 0;
}                                      /* End of function R_USB_HstdGetPipeID() */

 #endif

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdClearPipe
 * Description     : ClearPipe
 * Argument        : uint16_t devaddr : Device Address
 * Return          : usb_er_t                   : USB_OK etc
 ***********************************************************************************************************************/
usb_er_t R_USB_HstdClearPipe (uint16_t devaddr)
{
    uint16_t pipe_id;

    for (pipe_id = 1; pipe_id < USB_MAXPIPE; pipe_id++)
    {
        if (g_usb_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            g_usb_hstd_pipe[pipe_id].ifnum     = 0;
            g_usb_hstd_pipe[pipe_id].epnum     = 0;
            g_usb_hstd_pipe[pipe_id].direction = 0;
            g_usb_hstd_pipe[pipe_id].type      = 0;
            g_usb_hstd_pipe[pipe_id].mps       = 0;
            g_usb_hstd_pipe[pipe_id].devaddr   = 0;
        }
    }

    return USB_OK;
}                                      /* End of function R_USB_HstdClearPipe() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdMgrOpen
 * Description     : Initialize Manager(MGR) task
 * Arguments       : none
 * Return          : none
 ***********************************************************************************************************************/
usb_er_t R_USB_HstdMgrOpen (usb_utr_t * ptr)
{
    usb_er_t       err = USB_OK;
    usb_hcdreg_t * p_driver;
    uint16_t       i;

    /* Scheduler init */
    usb_hstd_sche_init();
 #if (BSP_CFG_RTOS == 2)
  #if defined(USB_CFG_HMSC_USE)

    /* create access control semaphore */
    vSemaphoreCreateBinary(SemaphoreHandleRead);
  #endif                               /* #if defined(USB_CFG_HMSC_USE) */
 #endif                                /* #if (BSP_CFG_RTOS == 2) */

    /* Manager Mode */
    g_usb_hstd_mgr_mode[USB_PORT0] = USB_DETACHED;
    g_usb_hstd_device_speed[0]     = USB_NOCONNECT;
    g_usb_hstd_device_addr[0]      = USB_DEVICE_0; /* Device address */
    g_usb_hstd_device_num[0]       = 0;            /* Device driver number */

    for (i = 0; i < (USB_MAXDEVADDR + 1U); i++)
    {
        p_driver = &g_usb_hstd_device_drv[0][i];

        p_driver->rootport = USB_NOPORT;               /* Root port */
        p_driver->devaddr  = USB_NODEVICE;             /* Device address */
        p_driver->devstate = USB_DETACHED;             /* Device state */
        p_driver->ifclass  = (uint16_t) USB_IFCLS_NOT; /* Interface Class : NO class */
    }

    R_USB_CstdSetTaskPri(USB_HCI_TSK, USB_PRI_1);
    R_USB_CstdSetTaskPri(USB_MGR_TSK, USB_PRI_2);

    /* EHCI/OHCI init */
    r_usb_hstd_hci_init(&cb);

    /* HUB Open */
 #if USB_CFG_HUB == USB_CFG_ENABLE
    usb_hhub_registration(ptr, USB_NULL);
    R_USB_CstdSetTaskPri(USB_HUB_TSK, USB_PRI_3);
 #else
    FSP_PARAMETER_NOT_USED(ptr);
 #endif                                /* USB_CFG_HUB == USB_CFG_ENABLE */
    return err;
} /* End of function R_USB_HstdMgrOpen() */

/***********************************************************************************************************************
 * Function Name   : r_usb_hstd_mgr_task
 * Description     : Call MGR(Manager) task (API for nonOS)
 * Argument        : none
 * Return          : none
 ***********************************************************************************************************************/
void r_usb_hstd_mgr_task (void)
{
 #if (BSP_CFG_RTOS == 2)
    st_usb_utr_t * p_mess;
    while (1)
    {
        USB_RTOS_RCV_MSG(USB_MGR_MBX, (usb_msg_t **) &p_mess);
        usb_hstd_mgr_task(0);
        g_usb_msg_check &= (uint16_t) ~(1 << USB_MGR_TSK);
        g_usb_msg_check &= (uint16_t) ~(1 << USB_HCI_TSK);
    }
 #else                                 /* (BSP_CFG_RTOS == 2) */
    usb_hstd_mgr_task(0);
 #endif /* (BSP_CFG_RTOS == 2) */
}                                      /* End of function r_usb_hstd_mgr_task() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdDelayXms
 * Description     : Software Wait Timer
 * Arguments       : uint32_t ms   : msec
 * Return          : none
 ***********************************************************************************************************************/
void R_USB_HstdDelayXms (uint32_t ms)
{
    /* r_usb_hstd_hci_wait_time(ms); */
    R_BSP_SoftwareDelay(ms, BSP_DELAY_UNITS_MILLISECONDS);
}                                      /* End of function R_USB_HstdDelayXms() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdMgrClose
 * Description     :
 * Arguments       : none
 * Return          : none
 ***********************************************************************************************************************/
void R_USB_HstdMgrClose (void)
{
 #if (BSP_CFG_RTOS == 2)

    /* create access control semaphore */
    vSemaphoreDelete(&g_usb_sem);
 #endif
}                                      /* End of function R_USB_HstdMgrClose() */

 #if 0

/***********************************************************************************************************************
 * Function Name   : R_USB_Init
 * Description     :
 * Arguments       : none
 * Return          : none
 ***********************************************************************************************************************/
void R_USB_Init (void)
{
    usb_module_init();                            /* Start USB Port0 module */

    R_USB_HstdMgrOpen();                          /* USB0 MGR Open */

    R_USB_CstdSetTaskPri(USB_HUB_TSK, USB_PRI_3); /* USB0 HUB Open */
    R_USB_HstdDriverRegistration(USB_NULL);

  #if (BSP_CFG_RTOS == 2)
    r_usb_rtos_configration();                    /* USB0 FreeRTOS Setting */
  #endif /* (BSP_CFG_RTOS == 2) */
    R_BSP_IrqCfg(284, 1, (void *) &R_USB_isr);
    R_BSP_IrqEnable(284);
    g_usb_msg_check = 0;
}                                      /* End of function R_USB_Init() */

 #endif

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdGetMaxPacketSize
 * Description     : Get max packet size
 * Arguments       : uint32_t devadr : device address
 * Return          : max packet size
 ***********************************************************************************************************************/
uint16_t R_USB_HstdGetMaxPacketSize (uint16_t devadr)
{
    return usb_hstd_get_max_packet_size(devadr);
}                                      /* End of function R_USB_HstdGetMaxPacketSize() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdGetDevAddr
 * Description     : Get device address
 * Arguments       : uint32_t devadr : device address
 * Return          : device address
 ***********************************************************************************************************************/
uint16_t R_USB_HstdGetDevAddr (uint16_t devadr)
{
    return usb_hstd_get_dev_addr(devadr);
}                                      /* End of function R_USB_HstdGetDevAddr() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdConvertEndpointNum
 * Description     : Convert Endpoint Number
 * Arguments       : uint32_t pipe : Pipe ID
 * Return          : Endpoint number
 ***********************************************************************************************************************/
uint8_t R_USB_HstdConvertEndpointNum (uint8_t pipe)
{
    return usb_hstd_convert_epnum(pipe);
}                                      /* End of function R_USB_HstdConvertEndpointNum() */

/***********************************************************************************************************************
 * Function Name   : R_USB_HstdGetPipeType
 * Description     : Get Pipe Type
 * Arguments       : uint32_t pipe : Pipe ID
 * Return          : Pipe Type
 ***********************************************************************************************************************/
uint8_t R_USB_HstdGetPipeType (uint8_t pipe)
{
    return (uint8_t) usb_hstd_get_pipe_type(pipe);
}                                      /* End of function R_USB_HstdGetPipeType() */

#endif /* USB_IP_EHCI_OHCI == 0 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
