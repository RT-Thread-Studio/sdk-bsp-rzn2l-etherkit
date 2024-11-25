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
#include "hal_data.h"
#include "r_usb_basic.h"
#include "r_usb_basic_cfg.h"
#include "inc/r_usb_pcdc_apl.h"
#include "r_usb_pcdc_api.h"
#include "r_usb_pcdc_apl_config.h"
#include "r_usb_typedef.h"
#include "r_usb_pcdc_cfg.h"

#include <rtthread.h>

#if USB_CFG_DTC == USB_CFG_ENABLE
#include "r_dtc_rx_if.h"
#endif /* USB_CFG_DTC == USB_CFG_ENABLE */

#if USB_CFG_DMA == USB_CFG_ENABLE
// #include "r_dmaca_rx_if.h"
#endif /* USB_CFG_DMA == USB_CFG_ENABLE */

#if (BSP_CFG_RTOS == 2)
#include "inc/r_usb_rtos_apl.h"
#include "r_usb_cstd_rtos.h"
#endif /* (BSP_CFG_RTOS == 2) */

#ifdef USB_CFG_PCDC_USE

#if OPERATION_MODE == USB_ECHO
/******************************************************************************
 Macro definitions
 ***************************************************************************#***/

#define FSP_VECTOR_USB_D0FIFO ((IRQn_Type)1)

#define FSP_VECTOR_USB_D1FIFO ((IRQn_Type)2)

#define FSP_VECTOR_USB_USBI ((IRQn_Type)3)
#define FSP_VECTOR_USB_USBI_IPL (1U)

#define FSP_VECTOR_USB_USBR ((IRQn_Type)4)
#define FSP_VECTOR_USB_USBR_IPL (2U)

#define FSP_VECTOR_USB_USBIR ((IRQn_Type)5)
#define FSP_VECTOR_USB_USBIR_IPL (3U)

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static uint8_t g_buf[DATA_LEN];
static uint8_t g_buf1[DATA_LEN];
static usb_pcdc_linecoding_t g_line_coding;

extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];

// static  usb_event_info_t    g_event_info[6];

const usb_descriptor_t g_usb_descriptor =
    {
        g_apl_device,               /* Pointer to the device descriptor */
        g_apl_configuration,        /* Pointer to the configuration descriptor for Full-speed */
        g_apl_hs_configuration,     /* Pointer to the configuration descriptor for Hi-speed */
        g_apl_qualifier_descriptor, /* Pointer to the qualifier descriptor */
        g_apl_string_table,         /* Pointer to the string descriptor table */
        NUM_STRING_DESCRIPTOR};

/******************************************************************************
Exported global functions (to be accessed by other files)
 ******************************************************************************/
extern const usb_api_t g_usb_on_usb;

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
void usb_apl_callback(usb_event_info_t *p_api_event, usb_hdl_t cur_task, usb_onoff_t usb_state)
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
void usb_pcdc_example(void)
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
#endif

#if (BSP_CFG_RTOS == 2)
    usb_event_info_t *p_mess;
    usb_event_info_t event_info;
#else /* (BSP_CFG_RTOS == 2) */
    usb_status_t event;
    usb_event_info_t event_info;
#endif
    usb_info_t info;

    g_usb_on_usb.open(&g_basic0_ctrl, &g_basic0_cfg);

    /* Loop back between PC(TerminalSoft) and USB MCU */
    while (1)
    {
#if (BSP_CFG_RTOS == 2)
        USB_APL_RCV_MSG(USB_APL_MBX, (usb_msg_t **)&p_mess);
        event_info = *p_mess;
        /* Analyzing the received message */
        switch (event_info.event)
#else  /* (BSP_CFG_RTOS == 2) */
        R_USB_EventGet(&event_info, &event);
        switch (event)
#endif /* (BSP_CFG_RTOS == 2) */
        {
        case USB_STATUS_CONFIGURED:
            g_usb_on_usb.read(&g_basic0_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
            g_usb_on_usb.read(&g_basic0_ctrl, g_buf1, DATA_LEN, USB_CLASS_PCDC2);
            break;
        case USB_STATUS_WRITE_COMPLETE:
            if (USB_CFG_PCDC_BULK_IN == event_info.pipe)
            {

                g_usb_on_usb.read(&g_basic0_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
            }
            else if (USB_CFG_PCDC_BULK_IN2 == event_info.pipe)
            {
                g_usb_on_usb.read(&g_basic0_ctrl, g_buf1, DATA_LEN, USB_CLASS_PCDC2);
            }

            break;

        case USB_STATUS_READ_COMPLETE:
            if (USB_CFG_PCDC_BULK_OUT == event_info.pipe)
            {
                g_usb_on_usb.write(&g_basic0_ctrl, g_buf, event_info.data_size, USB_CLASS_PCDC);
            }
            else if (USB_CFG_PCDC_BULK_OUT2 == event_info.pipe)
            {
                g_usb_on_usb.write(&g_basic0_ctrl, g_buf1, event_info.data_size, USB_CLASS_PCDC2);
            }
            break;

        case USB_STATUS_REQUEST: /* Receive Class Request */
            if (USB_PCDC_SET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
            {
                R_USB_InfoGet(&g_basic0_ctrl, &info, event_info.device_address);
                R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t *)&g_line_coding, LINE_CODING_LENGTH);
            }
            else if (USB_PCDC_GET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
            {
                R_USB_InfoGet(&g_basic0_ctrl, &info, event_info.device_address);
                R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t *)&g_line_coding, LINE_CODING_LENGTH);
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (event_info.setup.request_type & USB_BREQUEST))
            {
                /* ACK all other status requests */
                R_USB_InfoGet(&g_basic0_ctrl, &info, event_info.device_address);
                R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
            }
            else
            {
                R_USB_InfoGet(&g_basic0_ctrl, &info, event_info.device_address);
                R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_STALL);
            }
            break;

        case USB_STATUS_SUSPEND:
        case USB_STATUS_DETACH:
            break;

        default:
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
usb_er_t usb_apl_rec_msg(uint8_t id, usb_msg_t **mess, usb_tm_t tm)
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
    *mess = NULL;

    /** Receive message from queue specified by *(mbx_table[id]) **/
    err = xQueueReceive(handle, (void *)mess, (portMAX_DELAY));
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
usb_er_t usb_apl_snd_msg(uint8_t id, usb_msg_t *mess)
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
#endif /* #if (BSP_CFG_RTOS == 2) */
#endif /* OPERATION_MODE == USB_ECHO */
#endif /* USB_CFG_PCDC_USE */

/******************************************************************************
 End  Of File
 ******************************************************************************/
