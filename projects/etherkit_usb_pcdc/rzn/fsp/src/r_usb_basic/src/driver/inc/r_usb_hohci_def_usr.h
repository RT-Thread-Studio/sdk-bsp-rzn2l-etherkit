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
 * File Name    : r_usb_hohci_def_usr.h
 * Version      : 1.0
 * Description  : OHCI User Define Header
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USB_HOHCIDEFUSR_H
#define R_USB_HOHCIDEFUSR_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* The maximum number of Endpoint memory */
#define USB_OHCI_NUM_ENDPOINT            ((USB_HCI_DEVICE_NUM) *(USB_OHCI_DEVICE_ENDPOINT_MAX) +4)

/* The maximum number of Endpoint Descriptor memory */
#define USB_OHCI_NUM_ED                  (USB_OHCI_ED_INTERRUPT_32ms + USB_OHCI_NUM_ENDPOINT)

/* The maximum number of Transfer Descriptor memory */
#define USB_OHCI_NUM_TD                  (64) // (0x100000 / USB_OHCI_MAXTDSIZE)

#if defined(BSP_MCU_GROUP_RZN2L)

/* The maximum number of OHCI Isochronous Device */
 #define USB_OHCI_ISO_MAXDEVICE          (4U)

/* The maximum number of OHCI Isochronous Packet Size */
 #define USB_OHCI_ISO_MAX_PACKET_SIZE    (1U)
#else

/* The maximum number of OHCI Isochronous Device */
 #define USB_OHCI_ISO_MAXDEVICE          (4U)

/* The maximum number of OHCI Isochronous Packet Size */
 #define USB_OHCI_ISO_MAX_PACKET_SIZE    (1023U)
#endif

/* Time-out setting (msec) */
#define USB_OHCI_TIMEOUT                 (3000U)

#endif                                 /* R_USB_HOHCIDEFUSR_H */
