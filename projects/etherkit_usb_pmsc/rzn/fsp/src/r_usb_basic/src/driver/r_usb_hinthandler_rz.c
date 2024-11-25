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
 * File Name    : r_usb_hinthandler.c
 * Version      : 1.0
 * Description  : This module solves all the world's problems
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2007 1.00     First Release
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_extern.h"

#if   USB_IP_EHCI_OHCI == 1

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
 #define USB00_WAKEON_INT    0x00000010 /* Interrupt */
 #define USB00_UCOM_INT      0x00000008 /* Interrupt */
 #define USB00_USBH_INTB     0x00000004 /* EHCI interrupt */
 #define USB00_USBH_INTA     0x00000002 /* OHCI interrupt */
 #define USB00_AHB_INT       0x00000001 /* Bus master error interrupt */

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/
extern void usb_hstd_hci_interrupt_handler(void);
extern void R_USB_isr(void);

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
void R_USB_HstdIntInit(void);
void usb_hstd_int_enable(void);
void usb_hstd_int_disable(void);

/***********************************************************************************************************************
 * Function Name: usb_hstd_int_enable
 * Description  : USB host interrupt enable
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usb_hstd_int_enable (void)
{
    usb_cpu_int_enable();
}                                      /* End of function usb_hstd_int_enable() */

/***********************************************************************************************************************
 * Function Name: usb_hstd_int_disable
 * Description  : USB host interrupt disable
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usb_hstd_int_disable (void)
{
    usb_cpu_int_disable();
}                                      /* End of function usb_hstd_int_disable() */

/***********************************************************************************************************************
 * Function Name: R_USB_isr
 * Description  : Interrupt service routine of USB host
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/

void R_USB_isr (void)
{
    uint32_t int_state;

    USB_CFG_MULTIPLEX_INTERRUPT_ENABLE;

    int_state = USB00->INTSTATUS;
    if (USB00_AHB_INT == (int_state & USB00_AHB_INT))
    {
        USB00->INTSTATUS |= USB00_AHB_INT;
        while (1)
        {
            ;
        }
    }
    else if ((USB00_USBH_INTA == (int_state & USB00_USBH_INTA)) || (USB00_USBH_INTB == (int_state & USB00_USBH_INTB))) /* EHCI or OHCI interrupt */
    {
        usb_hstd_hci_interrupt_handler();
    }
    else if (USB00_UCOM_INT == (int_state & USB00_UCOM_INT))
    {
    }
    else if (USB00_WAKEON_INT == (int_state & USB00_WAKEON_INT))
    {
        USB00->INTSTATUS |= USB00_WAKEON_INT;
    }
    else
    {
    }

    USB_CFG_MULTIPLEX_INTERRUPT_DISABLE;
}                                      /* End of function R_USB_isr() */

#endif                                 /* USB_IP_EHCI_OHCI == 1 */

/***********************************************************************************************************************
 * End Of File
 ***********************************************************************************************************************/
