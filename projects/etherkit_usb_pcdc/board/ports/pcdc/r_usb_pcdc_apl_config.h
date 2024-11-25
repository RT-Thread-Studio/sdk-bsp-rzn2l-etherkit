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
 * Copyright (C) 2014(2016) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_pcdc_apl_config.h
 * Description  : USB Peripheral CDC program configuration file.
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 ***********************************************************************************************************************/

#ifndef R_USB_PCDC_APL_CONFIG_H
#define R_USB_PCDC_APL_CONFIG_H

/******************************************************************************
 User define macro definitions
 ******************************************************************************/
/** [USB module selection setting]
 *  USB_IP0             : Uses USB0 module
 *  USB_IP1             : Uses USB1 module
 */
#define USE_USBIP           (USB_IP0)


/** [USB connect speed setting]
 *  USB_HS              : Support Hi-Speed
 *  USB_FS              : Not suppor Hi-Speed(Full-Speed only)
 */
#define USB_SUPPORT_SPEED   (USB_FS)


/** [Select PCDC Demo sample Application mode]
 *  USB_ECHO              : Loop back(Echo) mode
 *  USB_UART              : USB Serial(VCOM) converter mode
 */
#define OPERATION_MODE  (USB_ECHO)


/** [Select Support Free RTOS]
 *  USB_APL_ENABLE        : Support Free RTOS
 *  USB_APL_DISABLE       : Not support Free RTOS(Support non OS)
 */
#define USB_SUPPORT_RTOS    (USB_APL_DISABLE)

/** [Select USE power saving control]
 *  USB_APL_ENABLE        : Use power saving control
 *  USB_APL_DISABLE       : Not use power saving control
 */
#define USB_SUPPORT_LPW    (USB_APL_DISABLE)

#endif  /* R_USB_PCDC_APL_CONFIG_H */
/******************************************************************************
 End  Of File
 ******************************************************************************/

