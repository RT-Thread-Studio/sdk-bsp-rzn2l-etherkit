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

/*******************************************************************************
 * File Name    : r_usb_dmaca_rx_target.c
 * Device(s)    : RX
 * Tool-Chain   : Renesas RXC Toolchain v2.01.00
 * OS           : not use
 * H/W Platform : not use
 * Description  : Functions for DMACA driver
 *******************************************************************************/

/*******************************************************************************
 * History      : DD.MM.YYYY Version  Description
 *              : 30.06.2014 1.00     Initial revision
 *              : 27.08.2014 1.01     Not change program due to updating application note only.
 *              : 30.01.2015 1.02     Added RX71M.
 *              : 13.04.2015 1.03     Added RX231.
 *******************************************************************************/

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)

/*******************************************************************************
 * Includes <System Includes>, "Project Includes"
 *******************************************************************************/

/* Definitions for DMACA support */
#include "../hw/inc/r_usb_dmaca_rz_if.h"
#include "../hw/inc/r_usb_dmaca_rz_private.h"

/*******************************************************************************
 * Macro definitions
 *******************************************************************************/
typedef void (* DMACI_Callback)(void);

/*******************************************************************************
 * Private variables and functions
 *******************************************************************************/

/* DMACA DMAC0I */
void r_usb_dmaca_intDMAC0I_isr(void);

/* DMACA DMAC1I */
void r_usb_dmaca_intDMAC1I_isr(void);

/************************************************************************************
 * Outline      : Interrupt DMAC0I handler
 * Function Name: r_usb_dmaca_intDMAC0I_isr
 * Description  : Types of interrupt requests from DMACA channel 0.
 *                If the pointer to function is 'FIT_NO_FUNC' or 'NULL',
 *                write a program under "do something" in this
 *                r_usb_dmaca_intDMAC0I_isr() function.
 * Arguments    : None
 * Return Value : None
 ************************************************************************************/
void r_usb_dmaca_intDMAC0I_isr (void)
{
#if USB_CFG_DMA == USB_CFG_ENABLE
    if (((uint32_t) NULL != (uint32_t) p_USB_DMACI_Handlers[USB_DMACA_CH0]))
    {
        ((DMACI_Callback) p_USB_DMACI_Handlers[USB_DMACA_CH0])();
    }
    else
    {
        /* do something */
    }
#endif                                 /* USB_CFG_DMA == USB_CFG_ENABLE */
}

/************************************************************************************
 * Outline      : Interrupt DMAC1I handler
 * Function Name: r_usb_dmaca_intDMAC1I_isr
 * Description  : Types of interrupt requests from DMACA channel1.
 *                If the pointer to function is 'FIT_NO_FUNC' or 'NULL',
 *                write a program under "do something" in this
 *                r_usb_dmaca_intDMAC1I_isr() function.
 * Arguments    : None
 * Return Value : None
 ************************************************************************************/
void r_usb_dmaca_intDMAC1I_isr (void)
{
#if USB_CFG_DMA == USB_CFG_ENABLE
    if (((uint32_t) NULL != (uint32_t) p_USB_DMACI_Handlers[USB_DMACA_CH1]))
    {
        ((DMACI_Callback) p_USB_DMACI_Handlers[USB_DMACA_CH1])();
    }
    else
    {
        /* do something */
    }
#endif                                 /* USB_CFG_DMA == USB_CFG_ENABLE */
}

#if USB_CFG_DMA == USB_CFG_ENABLE

/*******************************************************************************
 * Exported global variables (to be accessed by other files)
 *******************************************************************************/

/*******************************************************************************
 * Function Name: r_usb_dmaca_channel_valid_check
 * Description  : Checks whether channel is valid or not.
 * Arguments    : channel -
 *                    Which channel to use
 * Return Value : true -
 *                    Channel is valid.
 *                false -
 *                    Channel is invalid.
 *******************************************************************************/
bool r_usb_dmaca_channel_valid_check (uint8_t channel)
{
    bool ret = false;

    switch (channel)
    {
        case USB_DMACA_CH0:
        case USB_DMACA_CH1:
        {
            /* The channel number is valid. */
            ret = true;
            break;
        }

        default:
        {
            /* The channel number is invalid. */
            ret = false;
            break;
        }
    }

    return ret;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_int_disable_set
 * Description  : Disables DMACmI interrupt. (m = 0 to 3, or 74)
 * Arguments    : channel -
 *                    Which channel to use
 * Return Value : USB_DMACA_SUCCESS -
 *                    DMACmI interrupt is disabled successfully.
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_int_disable_set (uint8_t channel)
{
    /* Clear the DMACmI interrupt Enable bit. */
    /* Clear the DMACmI priority level. */

    switch (channel)
    {
        case USB_DMACA_CH0:
        {
 #if USB_CFG_USE_USBIP == USB_CFG_IP0
            R_BSP_IrqDisable(VECTOR_NUMBER_USB_FDMA0);

/* R_INTC_SetPriority(INTC_ID_USB_USBFDMA00, 0); */
 #else
            R_BSP_IrqDisable(VECTOR_NUMBER_USB_FDMA1);

/* R_INTC_SetPriority(INTC_ID_USB_USBFDMA10, 0); */
 #endif
            break;
        }

        case USB_DMACA_CH1:
        {
 #if USB_CFG_USE_USBIP == USB_CFG_IP0
            R_BSP_IrqDisable(VECTOR_NUMBER_USB_FDMA0);

/* R_INTC_SetPriority(INTC_ID_USB_USBFDMA01, 0); */
 #else
            R_BSP_IrqDisable(VECTOR_NUMBER_USB_FDMA1);

            /* R_INTC_SetPriority(INTC_ID_USB_USBFDMA11, 0); */
 #endif
            break;
        }

        default:
        {
            /* The channel number is invalid. */
            return USB_DMACA_ERR_INVALID_CH;
            break;
        }
    }

    return USB_DMACA_SUCCESS;
}

/*******************************************************************************
 * Function Name: r_usb_dmaca_int_enable_set
 * Description  : Enables DMACmI interrupt. (m = 0 to 3, or 74)
 * Arguments    : channel -
 *                    Which channel to use
 *              : priority -
 *                    DMACmI interrupt priority level
 * Return Value : USB_DMACA_SUCCESS -
 *                    DMACmI interrupt is enabled successfully.
 *                USB_DMACA_ERR_INVALID_CH -
 *                    Channel is invalid.
 *******************************************************************************/
usb_dmaca_return_t r_usb_dmaca_int_enable_set (uint8_t channel, uint8_t priority)
{
    /* Set the DMACmI priority level. */
    /* Set the DMACmI interrupt Enable bit. */
    FSP_PARAMETER_NOT_USED(priority);
    switch (channel)
    {
        case USB_DMACA_CH0:
        {
 #if USB_CFG_USE_USBIP == USB_CFG_IP0
 #else
            R_BSP_IrqCfgEnable(VECTOR_NUMBER_USB_FDMA0, 12, NULL);
 #endif
            break;
        }

        case USB_DMACA_CH1:
        {
 #if USB_CFG_USE_USBIP == USB_CFG_IP0
 #else
            R_BSP_IrqCfgEnable(VECTOR_NUMBER_USB_FDMA1, 12, NULL);
 #endif
            break;
        }

        default:
        {
            /* The channel number is invalid. */
            return USB_DMACA_ERR_INVALID_CH;
            break;
        }
    }

    return USB_DMACA_SUCCESS;
}

#endif
#endif                                 /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */
/* End of File */
