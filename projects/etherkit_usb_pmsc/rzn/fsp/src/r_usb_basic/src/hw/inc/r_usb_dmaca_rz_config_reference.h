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
 * File Name     : r_usb_dmaca_rx_config.h
 * Description   : Configuration file for DMACA driver
 ********************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 30.06.2014 1.00    Initial revision
 *         : 27.08.2014 1.01    Not change program due to updating application note only.
 *         : 30.01.2015 1.02    Added RX71M.
 *         : 13.04.2015 1.03    Added RX231 and RX230.
 *******************************************************************************/
#ifndef USB_DMACA_RX_CONFIG_H
#define USB_DMACA_RX_CONFIG_H

/*
 * SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING
 *  0 : Compiles out parameter checking.
 *  1 : Includes parameter checking.
 * Default value is set to BSP_CFG_PARAM_CHECKING_ENABLE to re-use the system default setting
 */
#define USB_DMACA_CFG_PARAM_CHECKING_ENABLE    BSP_CFG_PARAM_CHECKING_ENABLE

/*
 * SPECIFY WHETHER THE DTC DRIVER IS USED WITH DMACA DRIVER
 *  0 : DTC driver is not used with DMACA driver.
 *  1 : DTC driver is used with DMACA driver.
 */
#define USB_DMACA_CFG_USE_DTC_FIT_MODULE       (0)

#endif                                 /* USB_DMACA_RX_CONFIG_H */
