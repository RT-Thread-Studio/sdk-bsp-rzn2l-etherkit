/***********************************************************************************************************************
 * Copyright [2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include <string.h>

#include "bsp_api.h"
#include "eoe_vether_feature.h"

#if EOE_VETHER_FEATURE_TCPIP_STACK_PLUS_TCP
#include "common_data.h"
#endif
#if EOE_VETHER_FEATURE_TCPIP_STACK_LWIP
#include "hal_data.h"
#endif

#include "applInterface.h"

#include <rtthread.h>

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#ifndef ETHER_GMAC_INTERRUPT_FACTOR_RECEPTION
#define ETHER_GMAC_INTERRUPT_FACTOR_RECEPTION    (0x000000C0)
#endif
/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef struct
{
    #define  EoE_Fragment_Max_Byte_Size MAX_MBX_SIZE
	uint8_t  pData[EoE_Fragment_Max_Byte_Size];
	uint16_t length;
    rt_event_t recv_event;
} vether_ctrl_t;

#define EOE_VETHER_FLAG 1 << 5

/***********************************************************************************************************************
 * Extern global variables
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static vether_ctrl_t g_eoe_vether_ctrl = {0};

const ether_api_t g_ether_on_eoe_vether =
{
    .open            = EOE_VETHER_Open,
    .close           = EOE_VETHER_Close,
    .read            = EOE_VETHER_Read,
    .bufferRelease   = NULL,
    .rxBufferUpdate  = NULL,
    .write           = EOE_VETHER_Write,
    .linkProcess     = EOE_VETHER_LinkProcess,
    .wakeOnLANEnable = NULL,
    .txStatusGet     = NULL,
    .callbackSet     = NULL
};

const ether_instance_t g_eoe_vether =
{ .p_ctrl = &g_ether0_ctrl, .p_cfg = &g_ether0_cfg, .p_api = &g_ether_on_eoe_vether };

/*********************************************************************************************************************
* Function Name: eoe_vether_frame_received_callback
* Description  : The function is called by the SSC if a new Ethernet frame is received via EoE.
*                The function passes the received Ether frame to the TCP/IP stack.
* Arguments    : pData - pointer to the received frame 
*                length - length of the received frame
* Return Value : none
***********************************************************************************************************************/
void eoe_vether_frame_received_callback (uint16_t *pData, uint16_t length)
{
    gmac_instance_ctrl_t * p_instance_ctrl = g_eoe_vether.p_ctrl;

    ether_callback_args_t  callback_arg;

    if (length > EoE_Fragment_Max_Byte_Size)
    {
        length = EoE_Fragment_Max_Byte_Size;
    }
    rt_memcpy(g_eoe_vether_ctrl.pData, pData, length);
	g_eoe_vether_ctrl.length = length;
    
    rt_event_send(g_eoe_vether_ctrl.recv_event, EOE_VETHER_FLAG);

    if (NULL != p_instance_ctrl->p_gmac_cfg->p_callback)
	{
		callback_arg.event        = ETHER_EVENT_SBD_INTERRUPT;
		callback_arg.status_ether = ETHER_GMAC_INTERRUPT_FACTOR_RECEPTION;
		(*p_instance_ctrl->p_gmac_cfg->p_callback)((void *) &callback_arg);
	}
}

/*********************************************************************************************************************
* Function Name: EOE_VETHER_Read
* Description  : Read packet
*
* Arguments    : p_ctrl           Pointer to control structure.
*                p_buffer         Pointer to where to store read data
*                length_bytes     Number of bytes in buffer
* Return Value : fsp_err_t        Error code
***********************************************************************************************************************/
fsp_err_t EOE_VETHER_Read(ether_ctrl_t * const p_ctrl, void * const p_buffer, uint32_t * const length_bytes)
{
	(void)p_ctrl;
	rt_uint32_t e;

	fsp_err_t err = FSP_ERR_ETHER_ERROR_NO_DATA;

	if(rt_event_recv(g_eoe_vether_ctrl.recv_event, EOE_VETHER_FLAG, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      RT_WAITING_FOREVER, &e) == RT_EOK)
	{
		if(g_eoe_vether_ctrl.length != 0)
		{ /* copy received data to buffer */
			rt_memcpy(p_buffer, g_eoe_vether_ctrl.pData, g_eoe_vether_ctrl.length);
			*length_bytes = (uint32_t)g_eoe_vether_ctrl.length;
			err = FSP_SUCCESS;
		}
	}
	return err;
}

/*********************************************************************************************************************
* Function Name: EOE_VETHER_Write
* Description  : Write packet
*
* Arguments    : p_ctrl           Pointer to control structure.
*                p_buffer         Pointer to where to store read data
*                farem_length     Number of bytes in buffer
* Return Value : fsp_err_t        Error code
***********************************************************************************************************************/
fsp_err_t EOE_VETHER_Write(ether_ctrl_t * const p_ctrl, void * const p_buffer, uint32_t const frame_length)
{
	(void)p_ctrl;

	fsp_err_t err = FSP_SUCCESS;
	EOE_SendFrameRequest((uint16_t *)p_buffer, (uint16_t)frame_length);
	return err;
}

/*********************************************************************************************************************
* Function Name: EOE_VETHER_Open
* Description  : Open driver
*
* Arguments    : p_ctrl           Pointer to control structure.
*                p_cfg            Pointer to configuration structure.
* Return Value : fsp_err_t        Error code
***********************************************************************************************************************/
fsp_err_t EOE_VETHER_Open (ether_ctrl_t * const p_ctrl, ether_cfg_t const * const p_cfg)
{
    gmac_instance_ctrl_t * p_instance_ctrl = (gmac_instance_ctrl_t *) p_ctrl;
    p_instance_ctrl->p_gmac_cfg = p_cfg;

	g_eoe_vether_ctrl.recv_event = rt_event_create("eoe_vether", RT_IPC_FLAG_FIFO);

	fsp_err_t err = FSP_SUCCESS;
	return err;
}

/*********************************************************************************************************************
* Function Name: EOE_VETHER_Close
* Description  : Close driver
*
* Arguments    : p_ctrl           Pointer to control structure.
* Return Value : fsp_err_t        Error code
***********************************************************************************************************************/
fsp_err_t EOE_VETHER_Close (ether_ctrl_t * const p_ctrl)
{
	(void)p_ctrl;

	fsp_err_t err = FSP_SUCCESS;
	return err;
}

/*********************************************************************************************************************
* Function Name: EOE_VETHER_LinkProcess
* Description  : Process link
*
* Arguments    : p_ctrl           Pointer to control structure.
* Return Value : fsp_err_t        Error code
***********************************************************************************************************************/
fsp_err_t EOE_VETHER_LinkProcess (ether_ctrl_t * const p_ctrl)
{
	(void)p_ctrl;

	fsp_err_t err = FSP_SUCCESS;
	return err;
}

/*********************************************************************************************************************
* Function Name: EOE_VETHER_GetLinkStatus
* Description  : Get link status
*
* Arguments    : p_ctrl           Pointer to control structure.
* Return Value : fsp_err_t        Error code
***********************************************************************************************************************/
fsp_err_t EOE_VETHER_GetLinkStatus (ether_ctrl_t * const p_ctrl, uint8_t port, gmac_link_status_t * p_status)
{
    (void)p_ctrl;
    (void)port;

    *p_status = GMAC_LINK_STATUS_READY;
    fsp_err_t err = FSP_SUCCESS;
    return err;
}


