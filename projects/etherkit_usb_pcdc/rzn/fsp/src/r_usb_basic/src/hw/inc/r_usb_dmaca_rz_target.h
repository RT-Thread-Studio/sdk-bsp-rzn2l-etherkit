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
 * File Name    : r_usb_dmaca_rx_target.h
 * Description  : Functions for DMACA driver
 *******************************************************************************/

/*******************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 30.06.2014 1.00    Initial revision
 *         : 27.08.2014 1.01    Not change program due to updating application note only.
 *         : 30.01.2015 1.02    Added RX71M.
 *         : 13.04.2015 1.03    Added RX231.
 *         : 30.09.2016 1.04    Added #define USB_DMACA_INVALID_LOWER_BIT_MASK.
 *         :                    Comment out #define USB_DMACA_NUM_INTERRUPT_SRC
 *         :                    because this FIT module not use it.
 *******************************************************************************/
#ifndef USB_DMACA_RX_TARGET_H
#define USB_DMACA_RX_TARGET_H

/*******************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *******************************************************************************/

/*******************************************************************************
 * Macro definitions
 *******************************************************************************/

/* Number of activation sources */
/* #define USB_DMACA_NUM_INTERRUPT_SRC         (194) */

/* Maximum value of 16bit count value (normal transfer mode) */
#define USB_DMACA_MAX_16BITS_COUNT_VAL_A    (65535)

/* Definition of number of DMACA channel */
#define USB_DMACA_NUM_CHANNELS              (2)

/* Channel Configuration Register .b15:SDS[3] bit mask */
#define USB_DMACA_INVALID_SRC_SKIP_MASK     (0x1 << 15)

/* Channel Configuration Register .b14-12:SDS[2:0] bit mask */
#define USB_DMACA_INVALID_SRC_SIZE_MASK     (0x7 << 12)

/* Channel Configuration Register .b19:DDS[3] bit mask */
#define USB_DMACA_INVALID_DES_SKIP_MASK     (0x1 << 19)

/* Channel Configuration Register .b18-16:DDS[2:0] bit mask */
#define USB_DMACA_INVALID_DES_SIZE_MASK     (0x7 << 16)

/* CHITVL Register.b15-00:ITVL[15:00]  Channel Interval mask */
#define USB_DMACA_INVALID_INTERVAL_MAX      0xffffU

/* IP base address */

// #define USB_BASE_ADDR               ((uint32_t*)&USB01 + ((uint32_t*)&USB11-(uint32_t*)&USB01)*USB_CFG_USE_USBIP)
#define USB_BASE_ADDR                       (uint32_t *) &R_USBF

/* DMAC register base address 1 :  N0SA - CRLA */
#define USB_DMACA_BASE_ADDR1                ((uint32_t *) &USB01.N0SA_1.LONG - (uint32_t *) &USB01.N0SA_0.LONG)

/* DMAC register base address 2 :  SCNT - DSKP */
#define USB_DMACA_BASE_ADDR2                ((uint32_t *) &USB01.SCNT_1.LONG - (uint32_t *) &USB01.SCNT_0.LONG)

/* DMAC register offset address */
/* N0SA register offset address */
#define USB_DMACA_N0SA_OFST                 ((uint32_t *) &USB01.N0SA_0.LONG - (uint32_t *) &USB01)

/* N0DA register offset address */
#define USB_DMACA_N0DA_OFST                 ((uint32_t *) &USB01.N0DA_0.LONG - (uint32_t *) &USB01)

/* N0TB register offset address */
#define USB_DMACA_N0TB_OFST                 ((uint32_t *) &USB01.N0TB_0.LONG - (uint32_t *) &USB01)

/* N1SA register offset address */
#define USB_DMACA_N1SA_OFST                 ((uint32_t *) &USB01.N1SA_0.LONG - (uint32_t *) &USB01)

/* N1DA register offset address */
#define USB_DMACA_N1DA_OFST                 ((uint32_t *) &USB01.N1DA_0.LONG - (uint32_t *) &USB01)

/* N1TB register offset address */
#define USB_DMACA_N1TB_OFST                 ((uint32_t *) &USB01.N1TB_0.LONG - (uint32_t *) &USB01)

/* CRSA register offset address */
#define USB_DMACA_CRSA_OFST                 ((uint32_t *) &USB01.CRSA_0.LONG - (uint32_t *) &USB01)

/* CRDA register offset address */
#define USB_DMACA_CRDA_OFST                 ((uint32_t *) &USB01.CRDA_0.LONG - (uint32_t *) &USB01)

/* CRTB register offset address */
#define USB_DMACA_CRTB_OFST                 ((uint32_t *) &USB01.CRTB_0.LONG - (uint32_t *) &USB01)

/* CHSTAT register offset address */
#define USB_DMACA_CHSTAT_OFST               ((uint32_t *) &USB01.CHSTAT_0.LONG - (uint32_t *) &USB01)

/* CHCTRL register offset address */
#define USB_DMACA_CHCTRL_OFST               ((uint32_t *) &USB01.CHCTRL_0.LONG - (uint32_t *) &USB01)

/* CHCFG register offset address */
#define USB_DMACA_CHCFG_OFST                ((uint32_t *) &USB01.CHCFG_0.LONG - (uint32_t *) &USB01)

/* CHITVL register offset address */
#define USB_DMACA_CHITVL_OFST               ((uint32_t *) &USB01.CHITVL_0.LONG - (uint32_t *) &USB01)

/* CHEXT register offset address */
#define USB_DMACA_CHEXT_OFST                ((uint32_t *) &USB01.CHEXT_0.LONG - (uint32_t *) &USB01)

/* NXLA register offset address */
#define USB_DMACA_NXLA_OFST                 ((uint32_t *) &USB01.NXLA_0.LONG - (uint32_t *) &USB01)

/* CRLA register offset address */
#define USB_DMACA_CRLA_OFST                 ((uint32_t *) &USB01.CRLA_0.LONG - (uint32_t *) &USB01)

/* SCNT register offset address */
#define USB_DMACA_SCNT_OFST                 ((uint32_t *) &USB01.SCNT_0.LONG - (uint32_t *) &USB01)

/* SSKP register offset address */
#define USB_DMACA_SSKP_OFST                 ((uint32_t *) &USB01.SSKP_0.LONG - (uint32_t *) &USB01)

/* DCNT register offset address */
#define USB_DMACA_DCNT_OFST                 ((uint32_t *) &USB01.DCNT_0.LONG - (uint32_t *) &USB01)

/* DSKP register offset address */
#define USB_DMACA_DSKP_OFST                 ((uint32_t *) &USB01.DSKP_0.LONG - (uint32_t *) &USB01)

/* DCTRL register offset address */
#define USB_DMACA_DCTRL_OFST                ((uint32_t *) &USB01.DCTRL.LONG - (uint32_t *) &USB01)

/* DSCITVL register offset address */
#define USB_DMACA_DSCITVL_OFST              ((uint32_t *) &USB01.DSCITVL.LONG - (uint32_t *) &USB01)

/* DST_EN register offset address */
#define USB_DMACA_DST_EN_OFST               ((uint32_t *) &USB01.DST_EN.LONG - (uint32_t *) &USB01)

/* DST_ER register offset address */
#define USB_DMACA_DST_ER_OFST               ((uint32_t *) &USB01.DST_ER.LONG - (uint32_t *) &USB01)

/* DST_END register offset address */
#define USB_DMACA_DST_END_OFST              ((uint32_t *) &USB01.DST_END.LONG - (uint32_t *) &USB01)

/* DST_TC register offset address */
#define USB_DMACA_DST_TC_OFST               ((uint32_t *) &USB01.DST_TC.LONG - (uint32_t *) &USB01)

/* DST_SUS register offset address */
#define USB_DMACA_DST_SUS_OFST              ((uint32_t *) &USB01.DST_SUS.LONG - (uint32_t *) &USB01)

/* DMAC register access macro */
/* N0SA register */

// #define USB_DMACA_N0SA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_N0SA_OFST))
#define USB_DMACA_NOSA(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].N[0].SA_b.SAWD))

/* N0DA register */

// #define USB_DMACA_N0DA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_N0DA_OFST))
#define USB_DMACA_N0DA(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].N[0].DA_b.DA))

/* N0TB register */

// #define USB_DMACA_N0TB(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_N0TB_OFST))
#define USB_DMACA_N0TB(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].N[0].TB_b.TB))

/* N1SA register */

// #define USB_DMACA_N1SA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_N1SA_OFST))
#define USB_DMACA_N1SA(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].N[1].SA_b.SAWD))

/* N1DA register */

// #define USB_DMACA_N1DA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_N1DA_OFST))
#define USB_DMACA_N1DA(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].N[1].DA_b.DA))

/* N1TB register */

// #define USB_DMACA_N1TB(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_N1TB_OFST))
#define USB_DMACA_N1TB(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].N[1].TB_b.TB))

/* CRSA register */

// #define USB_DMACA_CRSA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CRSA_OFST))
#define USB_DMACA_CRSA(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].CRSA))

/* CRDA register */

// #define USB_DMACA_CRDA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CRDA_OFST))
#define USB_DMACA_CRDA(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].CRDA))

/* CRTB register */

// #define USB_DMACA_CRTB(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CRTB_OFST))
#define USB_DMACA_CRTB(usb_dmaca_channel)      ((R_USBF->CHa[usb_dmaca_channel].CRTB))

/* CHSTAT register */

// #define USB_DMACA_CHSTAT(usb_dmaca_channel) (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CHSTAT_OFST))
#define USB_DMACA_CHSTAT(usb_dmaca_channel)    ((R_USBF->CHa[usb_dmaca_channel].CHSTAT))

/* CHCTRL register */

// #define USB_DMACA_CHCTRL(usb_dmaca_channel) (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CHCTRL_OFST))
#define USB_DMACA_CHCTRL(usb_dmaca_channel)    (R_USBF->CHa[usb_dmaca_channel].CHCTRL)

/* CHCFG register */

// #define USB_DMACA_CHCFG(usb_dmaca_channel)  (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CHCFG_OFST))
#define USB_DMACA_CHCFG(usb_dmaca_channel)     (R_USBF->CHa[usb_dmaca_channel].CHCFG)

/* CHITVL register */

// #define USB_DMACA_CHITVL(usb_dmaca_channel) (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CHITVL_OFST))
#define USB_DMACA_CHITVL(usb_dmaca_channel)    (R_USBF->CHa[usb_dmaca_channel].CHITVL)

/* CHEXT register */

// #define USB_DMACA_CHEXT(usb_dmaca_channel)  (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CHEXT_OFST))
#define USB_DMACA_CHEXT(usb_dmaca_channel)     (R_USBF->CHa[usb_dmaca_channel].CHEXT)

/* NXLA register */

// #define USB_DMACA_NXLA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_NXLA_OFST))
#define USB_DMACA_NXLA(usb_dmaca_channel)      (R_USBF->CHa[usb_dmaca_channel].NXLA)

/* CRLA register */

// #define USB_DMACA_CRLA(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR1*usb_dmaca_channel + USB_DMACA_CRLA_OFST))
#define USB_DMACA_CRLA(usb_dmaca_channel)      (R_USBF->CHa[usb_dmaca_channel].CRLA)

/* SCNT register */

// #define USB_DMACA_SCNT(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR2*usb_dmaca_channel + USB_DMACA_SCNT_OFST))
#define USB_DMACA_SCNT(usb_dmaca_channel)      (R_USBF->CHb[usb_dmaca_channel].SCNT)

/* SSKP register */

// #define USB_DMACA_SSKP(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR2*usb_dmaca_channel + USB_DMACA_SSKP_OFST))
#define USB_DMACA_SSKP(usb_dmaca_channel)      (R_USBF->CHb[usb_dmaca_channel].SSKP)

/* DCNT register */

// #define USB_DMACA_DCNT(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR2*usb_dmaca_channel + USB_DMACA_DCNT_OFST))
#define USB_DMACA_DCNT(usb_dmaca_channel)      (R_USBF->CHb[usb_dmaca_channel].DCNT)

/* DSKP register */

// #define USB_DMACA_DSKP(usb_dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_BASE_ADDR2*usb_dmaca_channel + USB_DMACA_DSKP_OFST))
#define USB_DMACA_DSKP(usb_dmaca_channel)      (R_USBF->CHb[usb_dmaca_channel].DSKP)

/* DCTRL register */

// #define USB_DMACA_DCTRL                 (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DCTRL_OFST))
#define USB_DMACA_DCTRL      (R_USBF->DCTRL)

/* DSCITVL register */

// #define USB_DMACA_DSCITVL               (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DSCITVL_OFST))
#define USB_DMACA_DSCITVL    (R_USBF->DSCITVL)

/* DST_EN register */

// #define USB_DMACA_DST_EN                (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DST_EN_OFST))
#define USB_DMACA_DST_EN     (R_USBF->DSTAT_EN)

/* DST_ER register */

// #define USB_DMACA_DST_ER                (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DST_ER_OFST))
#define USB_DMACA_DST_ER     (R_USBF->DSTAT_ER)

/* DST_END register */

// #define USB_DMACA_DST_END               (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DST_END_OFST))
#define USB_DMACA_DST_END    (R_USBF->DSTAT_END)

/* DST_TC register */

// #define USB_DMACA_DST_TC                (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DST_TC_OFST))
#define USB_DMACA_DST_TC     (R_USBF->DSTAT_TC)

/* DST_SUS register */

// #define USB_DMACA_DST_SUS               (*(volatile uint32_t *)(USB_BASE_ADDR + USB_DMACA_DST_SUS_OFST))
#define USB_DMACA_DST_SUS    (R_USBF->DSTAT_SUS)

#endif                                 /* USB_DMACA_RX_TARGET_H */

/* End of File */
