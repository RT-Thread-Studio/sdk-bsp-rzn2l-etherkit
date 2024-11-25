/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2014(2018) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/*******************************************************************************
 * File Name    : r_usb_pcdc_echo_apl.c
 * Description  : USB Peripheral Communications Devices Class Sample Code
 *******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 31.03.2018 1.23 Using Pin setting API.
 *                           Support CDC Class Request.
 *                           Num String descriptor is added.
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic.h"
#include "r_usb_basic_cfg.h"
#include "inc/r_usb_pmsc_apl.h"
#include "r_usb_pmsc_api.h"
#include "rm_block_media_api.h"
#include "bsp_pin_cfg.h"
#include "r_usb_typedef.h"
#include "common_data.h"
#include "hal_data.h"

#if (BSP_CFG_RTOS == 2)
#include <rtthread.h>
#include "inc/r_usb_rtos_apl.h"
#include "r_usb_cstd_rtos.h"
#endif /* (BSP_CFG_RTOS == 2) */

#ifdef USB_CFG_PMSC_USE

/******************************************************************************
 Macro definitions
 ***************************************************************************#***/

#define FSP_VECTOR_USB_D0FIFO          ((IRQn_Type)1)

#define FSP_VECTOR_USB_D1FIFO          ((IRQn_Type)2)

#define FSP_VECTOR_USB_USBI            ((IRQn_Type)3)
#define FSP_VECTOR_USB_USBI_IPL        (1U)

#define FSP_VECTOR_USB_USBR            ((IRQn_Type)4)
#define FSP_VECTOR_USB_USBR_IPL        (2U)

#define FSP_VECTOR_USB_USBIR           ((IRQn_Type)5)
#define FSP_VECTOR_USB_USBIR_IPL       (3U)


/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];

const usb_descriptor_t g_usb_descriptor =
{
    g_apl_device,
    g_apl_configuration,
    g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
    g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
    g_apl_string_table,
    NUM_STRING_DESCRIPTOR
};

extern void r_usb_pmsc_block_media_event_callback(rm_block_media_callback_args_t *p_args);
extern const rm_block_media_api_t g_rm_block_media_on_user_media;

rm_block_media_cfg_t g_rm_block_media0_cfg =
{ .p_extend = NULL, .p_callback = r_usb_pmsc_block_media_event_callback, .p_context = NULL, };

rm_block_media_instance_t g_rm_block_media0 =
{ .p_api = &g_rm_block_media_on_user_media, .p_ctrl = NULL, .p_cfg = &g_rm_block_media0_cfg, };

/******************************************************************************
 Renesas Peripheral Communications Devices Class Sample Code functions
 ******************************************************************************/
#if (BSP_CFG_RTOS == 2)
/************* semaphore initialization *****************/
static QueueHandle_t g_apl_mbx_hdl;
/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/
QueueHandle_t *g_apl_mbx_table[] =
{
    &g_apl_mbx_hdl,
};
/******************************************************************************
 Function Name   : usb_apl_callback
 Description     : Callback function for Application program
 Arguments       : usb_ctrl_t *p_ctrl   : Control structure for USB API.
                   usb_hdl_t  cur_task  : Task Handle
                   uint8_t    usb_state : USB_ON(USB_STATUS_REQUEST) / USB_OFF
 Return value    : none
 ******************************************************************************/
void usb_apl_callback (usb_event_info_t *p_api_event, usb_hdl_t cur_task, usb_onoff_t usb_state)
{
    (void)usb_state;
    (void)cur_task;

    rt_interrupt_enter();
    USB_APL_SND_MSG(USB_APL_MBX, (usb_msg_t *)p_api_event);
    rt_interrupt_leave();
} /* End of function usb_apl_callback */
#endif /* (BSP_CFG_RTOS == 2) */

/******************************************************************************
 Function Name   : usb_main
 Description     : Peripheral CDC application main process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_msc_example (void)
{
#if (BSP_CFG_RTOS == 2)
    g_apl_mbx_hdl = xQueueCreate(QUEUE_SIZE, sizeof(void *));
    if (NULL == g_apl_mbx_hdl)
    {
        while(1)
        {
            /* Failed! The mailbox for the application can not be created. */
        }
    }
    usb_event_info_t  *p_mess;
    usb_event_info_t   usb_event;
#else /* (BSP_CFG_RTOS == 2) */
    usb_status_t       event;
    usb_event_info_t   event_info;
#endif /* (BSP_CFG_RTOS == 2) */
    g_usb_on_usb.open(&g_basic0_ctrl, &g_basic0_cfg);
    /* Loop back between PC(TerminalSoft) and USB MCU */
    while (1)
    {
#if (BSP_CFG_RTOS == 2)
        USB_APL_RCV_MSG(USB_APL_MBX, (usb_msg_t **)&p_mess);
        usb_event = *p_mess;
        /* Analyzing the received message */
        switch (usb_event.event)
#else   /* (BSP_CFG_RTOS == 2) */
        R_USB_EventGet(&event_info, &event);

        switch (event)
#endif  /* (BSP_CFG_RTOS == 2) */
        {
            case USB_STATUS_CONFIGURED :
            break;
            case USB_STATUS_SUSPEND :
            case USB_STATUS_DETACH :
            break;
            default :
            break;
        }
    }
} /* End of function usb_main() */

#if (BSP_CFG_RTOS == 2)
/******************************************************************************
 Function Name   : usb_apl_rec_msg
 Description     : Receive a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
                 : usb_msg_t** mess  : Message pointer
                 : usb_tm_t tm       : Timeout Value
 Return          : uint16_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_apl_rec_msg (uint8_t id, usb_msg_t** mess, usb_tm_t tm)
{
    BaseType_t err;
    QueueHandle_t handle;
    usb_er_t result;

    (void)tm;

    if (NULL == mess)
    {
        return USB_APL_ERROR;
    }

    handle = (*(g_apl_mbx_table[id]));
    *mess  = NULL;

    /** Receive message from queue specified by *(mbx_table[id]) **/
    err = xQueueReceive(handle, (void *)mess,(portMAX_DELAY));
    if ((pdTRUE == err) && (NULL != (*mess)))
    {
        result = USB_APL_OK;
    }
    else
    {
        result = USB_APL_ERROR;
    }

    return result;
}
/******************************************************************************
 End of function usb_apl_rec_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_apl_snd_msg
 Description     : Send a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
                 : usb_msg_t* mess   : Message pointer
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_apl_snd_msg (uint8_t id, usb_msg_t* mess)
{
    BaseType_t err;
    QueueHandle_t handle;
    usb_er_t result;

    if (NULL == mess)
    {
        return USB_APL_ERROR;
    }

    handle = (*(g_apl_mbx_table[id]));

    /** Send message to queue specified by *(mbx_table[id]) **/
    err = xQueueSend(handle, (const void *)&mess, (TickType_t)(0));
    if (pdTRUE == err)
    {
        result = USB_APL_OK;
    }
    else
    {
        result = USB_APL_ERROR;
    }

    return result;
}
/******************************************************************************
 End of function usb_apl_snd_msg
 ******************************************************************************/
#endif /* (BSP_CFG_RTOS == 2) */
#endif /* USB_CFG_PMSC_USE */

/******************************************************************************
 End  Of File
 ******************************************************************************/

