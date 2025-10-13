/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_MODULE_H
#define BSP_MODULE_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Cancels the module stop state.
 *
 * @param      ip       fsp_ip_t enum value for the module to be stopped
 * @param      channel  The channel. Use channel 0 for modules without channels.
 **********************************************************************************************************************/
#define R_BSP_MODULE_START(ip, channel)             {FSP_CRITICAL_SECTION_DEFINE;                                   \
                                                     FSP_CRITICAL_SECTION_ENTER;                                    \
                                                     BSP_MSTP_REG_ ## ip(channel) &= ~BSP_MSTP_BIT_ ## ip(channel); \
                                                     BSP_MSTP_REG_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     BSP_MSTP_DMY_ ## ip(channel);                                  \
                                                     FSP_CRITICAL_SECTION_EXIT;}

/*******************************************************************************************************************//**
 * Enables the module stop state.
 *
 * @param      ip       fsp_ip_t enum value for the module to be stopped
 * @param      channel  The channel. Use channel 0 for modules without channels.
 **********************************************************************************************************************/
#define R_BSP_MODULE_STOP(ip, channel)              {FSP_CRITICAL_SECTION_DEFINE;                                  \
                                                     FSP_CRITICAL_SECTION_ENTER;                                   \
                                                     BSP_MSTP_REG_ ## ip(channel) |= BSP_MSTP_BIT_ ## ip(channel); \
                                                     BSP_MSTP_REG_ ## ip(channel);                                 \
                                                     FSP_CRITICAL_SECTION_EXIT;}

/** @} (end addtogroup BSP_MCU) */

#define BSP_MSTP_REG_FSP_IP_BSC(channel)            R_SYSC_NS->MSTPCRA
#define BSP_MSTP_BIT_FSP_IP_BSC(channel)            (1U)
#define BSP_MSTP_DMY_FSP_IP_BSC(channel)            R_BSC->TOENR

#define BSP_MSTP_REG_FSP_IP_XSPI(channel)           R_SYSC_NS->MSTPCRA
#define BSP_MSTP_BIT_FSP_IP_XSPI(channel)           (1U << (4U + channel))
#define BSP_MSTP_DMY_FSP_IP_XSPI(channel)           (0 >= channel) ? R_XSPI0->WRAPCFG : R_XSPI1->WRAPCFG

#define BSP_MSTP_REG_FSP_IP_SCI(channel)            *((4U >= channel) ? &R_SYSC_NS->MSTPCRA : &R_SYSC_S->MSTPCRG)
#define BSP_MSTP_BIT_FSP_IP_SCI(channel)            ((4U >= channel) ? (1U << (8U + channel)) : (1U))
#define BSP_MSTP_DMY_FSP_IP_SCI(channel)            (0 >= channel) ? R_SCI0->RDR : ((1 >= channel) ? R_SCI1->RDR :         \
                                                                                    ((2 >=                                 \
                                                                                      channel) ? R_SCI2->RDR : ((3 >=      \
                                                                                                                 channel)  \
                                                                                                                ? R_SCI3   \
                                                                                                                ->RDR :    \
                                                                                                                ((4        \
                                                                                                                  >=       \
                                                                                                                  channel) \
                                                                                                                 ?         \
                                                                                                                 R_SCI4    \
                                                                                                                 ->RDR :   \
                                                                                                                 R_SCI5    \
                                                                                                                 ->        \
                                                                                                                 RDR))))

#if BSP_FEATURE_SCIE_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_SCIE(channel)          R_SYSC_NS->MSTPCRA
 #define BSP_MSTP_BIT_FSP_IP_SCIE(channel)          (1U << (16U + channel))
 #define BSP_MSTP_DMY_FSP_IP_SCIE(channel)          (0 >= channel) ? R_SCIE0->RDR : \
    ((1 >= channel) ? R_SCIE1->RDR :                                                \
     ((2 >= channel) ? R_SCIE2->RDR :                                               \
      ((3 >= channel) ? R_SCIE3->RDR :                                              \
       ((4 >= channel) ? R_SCIE4->RDR :                                             \
        ((5 >= channel) ? R_SCIE5->RDR :                                            \
         ((6 >= channel) ? R_SCIE6->RDR :                                           \
          ((7 >= channel) ? R_SCIE7->RDR :                                          \
           ((8 >= channel) ? R_SCIE8->RDR :                                         \
            ((9 >= channel) ? R_SCIE9->RDR :                                        \
             ((10 >= channel) ? R_SCIE10->RDR :                                     \
              R_SCIE11->RDR))))))))))
#endif

#define BSP_MSTP_REG_FSP_IP_IIC(channel)            *((1U >= channel) ? &R_SYSC_NS->MSTPCRB : &R_SYSC_S->MSTPCRG)
#define BSP_MSTP_BIT_FSP_IP_IIC(channel)            ((1U >= channel) ? (1U << (0U + channel)) : (1U << (1U)))
#define BSP_MSTP_DMY_FSP_IP_IIC(channel)            (0 >= channel) ? R_IIC0->ICCR1 : ((1 >= channel) ? R_IIC1->ICCR1 \
                                                                                      : R_IIC2->ICCR1)

#define BSP_MSTP_REG_FSP_IP_SPI(channel)            *((2U >= channel) ? &R_SYSC_NS->MSTPCRB : &R_SYSC_S->MSTPCRG)
#define BSP_MSTP_BIT_FSP_IP_SPI(channel)            ((2U >= channel) ? (1U << (4U + channel)) : (1U << (2U)))
#define BSP_MSTP_DMY_FSP_IP_SPI(channel)            (0 >= channel) ? R_SPI0->SPCKD : ((1 >= channel) ? R_SPI1->SPCKD : \
                                                                                      ((2 >=                           \
                                                                                        channel) ? R_SPI2->SPCKD :     \
                                                                                       R_SPI3                          \
                                                                                       ->SPCKD))

#define BSP_MSTP_REG_FSP_IP_MTU3(channel)           R_SYSC_NS->MSTPCRC
#define BSP_MSTP_BIT_FSP_IP_MTU3(channel)           (1U)
#define BSP_MSTP_DMY_FSP_IP_MTU3(channel)           R_MTU0->TCR

#if (18 == BSP_FEATURE_GPT_CHANNEL)
 #define BSP_MSTP_REG_FSP_IP_GPT(channel)           *((13U >= channel) ? &R_SYSC_NS->MSTPCRC : &R_SYSC_S->MSTPCRG)
 #define BSP_MSTP_BIT_FSP_IP_GPT(channel)           ((6U >= channel) ? (1U << (1U)) : ((13U >= channel) ? \
                                                                                       (1U << (2U)) : (1U << (3U))))
 #define BSP_MSTP_DMY_FSP_IP_GPT(channel)           (6 >=                                                              \
                                                     channel) ? R_GPT0->GTSTR : ((13 >=                                \
                                                                                  channel) ? R_GPT7->GTSTR : R_GPT14-> \
                                                                                 GTSTR)
#elif (56 == BSP_FEATURE_GPT_CHANNEL)
 #define BSP_MSTP_REG_FSP_IP_GPT(channel)           *((51U >= channel) ? &R_SYSC_NS->MSTPCRC : &R_SYSC_S->MSTPCRG)
 #define BSP_MSTP_BIT_FSP_IP_GPT(channel)           ((4U >= channel) ? (1U << (1U)) :            \
                                                     ((9U >= channel) ? (1U << (2U)) :           \
                                                      ((14U >= channel) ? (1U << (16U)) :        \
                                                       ((19U >= channel) ? (1U << (17U)) :       \
                                                        ((24U >= channel) ? (1U << (18U)) :      \
                                                         ((29U >= channel) ? (1U << (19U)) :     \
                                                          ((34U >= channel) ? (1U << (20U)) :    \
                                                           ((39U >= channel) ? (1U << (21U)) :   \
                                                            ((44U >= channel) ? (1U << (22U)) :  \
                                                             ((51U >= channel) ? (1U << (23U)) : \
                                                              (1U << (3U))))))))))))

 #define BSP_MSTP_DMY_FSP_IP_GPT(channel)           (4 >= channel) ? R_GPT00_0->GTSTR : \
    ((9 >= channel) ? R_GPT01_0->GTSTR :                                                \
     ((14 >= channel) ? R_GPT02_0->GTSTR :                                              \
      ((19 >= channel) ? R_GPT03_0->GTSTR :                                             \
       ((24 >= channel) ? R_GPT04_0->GTSTR :                                            \
        ((29 >= channel) ? R_GPT05_0->GTSTR :                                           \
         ((34 >= channel) ? R_GPT06_0->GTSTR :                                          \
          ((39 >= channel) ? R_GPT07_0->GTSTR :                                         \
           ((44 >= channel) ? R_GPT08_0->GTSTR :                                        \
            ((51 >= channel) ? R_GPT09_0->GTSTR :                                       \
             R_GPT10_0->GTSTR)))))))))

#endif
#if (1 == BSP_FEATURE_TFU_UNIT)
 #define BSP_MSTP_REG_FSP_IP_TFU(channel)           R_SYSC_NS->MSTPCRC
 #define BSP_MSTP_BIT_FSP_IP_TFU(channel)           (1U << (5U))
 #define BSP_MSTP_DMY_FSP_IP_TFU(channel)           R_TFU->TRGSTS
#elif (2 == BSP_FEATURE_TFU_UNIT)
 #define BSP_MSTP_REG_FSP_IP_TFU(channel)           R_SYSC_NS->MSTPCRC
 #define BSP_MSTP_BIT_FSP_IP_TFU(channel)           ((0 == channel) ? (1U << (5U)) : (1U << (24U)))
 #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)
  #define BSP_MSTP_DMY_FSP_IP_TFU(channel)          R_TFU0->TRGSTS
 #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)
  #define BSP_MSTP_DMY_FSP_IP_TFU(channel)          R_TFU1->TRGSTS
 #endif
#endif

#if (2 == BSP_FEATURE_ADC_UNIT)
 #define BSP_MSTP_REG_FSP_IP_ADC12(channel)         R_SYSC_NS->MSTPCRC
 #define BSP_MSTP_BIT_FSP_IP_ADC12(channel)         (1U << (6U + channel))
 #define BSP_MSTP_DMY_FSP_IP_ADC12(channel)         (0 >= channel) ? R_ADC120->ADCSR : R_ADC121->ADCSR
#elif (3 == BSP_FEATURE_ADC_UNIT)
 #define BSP_MSTP_REG_FSP_IP_ADC12(channel)         R_SYSC_NS->MSTPCRC
 #define BSP_MSTP_BIT_FSP_IP_ADC12(channel)         ((1 >= channel) ? (1U << (6U + channel)) : (1U << (25U)))
 #define BSP_MSTP_DMY_FSP_IP_ADC12(channel)         ((0 >= channel) ? R_ADC120->ADCSR :  \
                                                     ((1 >= channel) ? R_ADC121->ADCSR : \
                                                      R_ADC122->ADCSR))
#endif

#if (2 == BSP_FEATURE_DSMIF_UNIT)
 #define BSP_MSTP_REG_FSP_IP_DSMIF(channel)         R_SYSC_NS->MSTPCRD
 #define BSP_MSTP_BIT_FSP_IP_DSMIF(channel)         (1U << (0U + channel))
 #define BSP_MSTP_DMY_FSP_IP_DSMIF(channel)         (0 >= channel) ? R_DSMIF0->DSSEICR : R_DSMIF1->DSSEICR
#elif (10 == BSP_FEATURE_DSMIF_UNIT)
 #define BSP_MSTP_REG_FSP_IP_DSMIF(channel)         R_SYSC_NS->MSTPCRD
 #define BSP_MSTP_BIT_FSP_IP_DSMIF(channel)         ((1 >= channel) ? (1U << (0U + channel)) : (1U << (14U + channel)))
 #define BSP_MSTP_DMY_FSP_IP_DSMIF(channel)         (0 >= channel) ? R_DSMIF0->DSSEICR : \
    ((1 >= channel) ? R_DSMIF1->DSSEICR :                                                \
     ((2 >= channel) ? R_DSMIF2->DSSEICR :                                               \
      ((3 >= channel) ? R_DSMIF3->DSSEICR :                                              \
       ((4 >= channel) ? R_DSMIF4->DSSEICR :                                             \
        ((5 >= channel) ? R_DSMIF5->DSSEICR :                                            \
         ((6 >= channel) ? R_DSMIF6->DSSEICR :                                           \
          ((7 >= channel) ? R_DSMIF7->DSSEICR :                                          \
           ((8 >= channel) ? R_DSMIF8->DSSEICR :                                         \
            R_DSMIF9->DSSEICR))))))))
#endif

#define BSP_MSTP_REG_FSP_IP_CMT(channel)            R_SYSC_NS->MSTPCRD
#define BSP_MSTP_BIT_FSP_IP_CMT(channel)            (1U << (2U + channel))
#define BSP_MSTP_DMY_FSP_IP_CMT(channel)            (0 >=                                                                \
                                                     channel) ? R_CMT->UNT[0].CMSTR0 : ((1 >=                            \
                                                                                         channel) ? R_CMT->UNT[1].CMSTR0 \
                                                                                        : R_CMT->UNT[2].CMSTR0)

#define BSP_MSTP_REG_FSP_IP_CMTW(channel)           R_SYSC_NS->MSTPCRD
#define BSP_MSTP_BIT_FSP_IP_CMTW(channel)           (1U << (5U + channel))
#define BSP_MSTP_DMY_FSP_IP_CMTW(channel)           (0 >= channel) ? R_CMTW0->CMWSTR : R_CMTW1->CMWSTR

#define BSP_MSTP_REG_FSP_IP_TSU(channel)            R_SYSC_NS->MSTPCRD
#define BSP_MSTP_BIT_FSP_IP_TSU(channel)            (1U << (7U))
#if (1 == BSP_FEATURE_TSU_VERSION)
 #define BSP_MSTP_DMY_FSP_IP_TSU(channel)           R_TSU->TSUSS
#else
 #define BSP_MSTP_DMY_FSP_IP_TSU(channel)           R_TSU_B0->SSR
#endif

#define BSP_MSTP_REG_FSP_IP_DOC(channel)            R_SYSC_NS->MSTPCRD
#define BSP_MSTP_BIT_FSP_IP_DOC(channel)            (1U << (8U))
#define BSP_MSTP_DMY_FSP_IP_DOC(channel)            R_DOC->DOCR

#define BSP_MSTP_REG_FSP_IP_CRC(channel)            *((0U == channel) ? &R_SYSC_NS->MSTPCRD : &R_SYSC_S->MSTPCRG)
#define BSP_MSTP_BIT_FSP_IP_CRC(channel)            ((0U == channel) ? (1U << (9U)) : (1U << (4U)))
#define BSP_MSTP_DMY_FSP_IP_CRC(channel)            (0 >= channel) ? R_CRC0->CRCDIR : R_CRC1->CRCDIR

#define BSP_MSTP_REG_FSP_IP_CANFD(channel)          R_SYSC_NS->MSTPCRD
#define BSP_MSTP_BIT_FSP_IP_CANFD(channel)          (1U << (10U))
#define BSP_MSTP_DMY_FSP_IP_CANFD(channel)          R_CANFD->CFDGIPV

#define BSP_MSTP_REG_FSP_IP_CKIO(channel)           R_SYSC_NS->MSTPCRD
#define BSP_MSTP_BIT_FSP_IP_CKIO(channel)           (1U << (11U))
#define BSP_MSTP_DMY_FSP_IP_CKIO(channel)           R_BSC->TOENR

#if (1 == BSP_FEATURE_GMAC_UNIT)
 #define BSP_MSTP_REG_FSP_IP_GMAC(channel)          R_SYSC_NS->MSTPCRE
 #define BSP_MSTP_BIT_FSP_IP_GMAC(channel)          (1U)
 #define BSP_MSTP_DMY_FSP_IP_GMAC(channel)          R_GMAC->MAC_Configuration
#elif (3 == BSP_FEATURE_GMAC_UNIT)
 #define BSP_MSTP_REG_FSP_IP_GMAC(channel)          R_SYSC_NS->MSTPCRE
 #define BSP_MSTP_BIT_FSP_IP_GMAC(channel)          ((0U == channel) ? (1U) : (1U << (15U + channel)))
 #define BSP_MSTP_DMY_FSP_IP_GMAC(channel)          ((0 >= channel) ? R_GMAC0->MAC_Configuration :  \
                                                     ((1 >= channel) ? R_GMAC1->MAC_Configuration : \
                                                      R_GMAC2->MAC_Configuration))
#endif

#define BSP_MSTP_REG_FSP_IP_ETHSW(channel)          R_SYSC_NS->MSTPCRE
#define BSP_MSTP_BIT_FSP_IP_ETHSW(channel)          (1U << (1U))
#define BSP_MSTP_DMY_FSP_IP_ETHSW(channel)          R_ETHSW->REVISION

#define BSP_MSTP_REG_FSP_IP_ESC(channel)            R_SYSC_NS->MSTPCRE
#define BSP_MSTP_BIT_FSP_IP_ESC(channel)            (1U << (2U))
#define BSP_MSTP_DMY_FSP_IP_ESC(channel)            R_ESC->TYPE

#define BSP_MSTP_REG_FSP_IP_ETHSS(channel)          R_SYSC_NS->MSTPCRE
#define BSP_MSTP_BIT_FSP_IP_ETHSS(channel)          (1U << (3U))
#define BSP_MSTP_DMY_FSP_IP_ETHSS(channel)          R_ETHSS->PRCMD

#define BSP_MSTP_REG_FSP_IP_USBHS(channel)          R_SYSC_NS->MSTPCRE
#define BSP_MSTP_BIT_FSP_IP_USBHS(channel)          (1U << (8U))
#define BSP_MSTP_DMY_FSP_IP_USBHS(channel)          R_USBHC->HCREVISION

#if BSP_FEATURE_BSP_TRACE_CLOCK_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_TRACECLOCK(channel)    R_SYSC_S->MSTPCRF
 #define BSP_MSTP_BIT_FSP_IP_TRACECLOCK(channel)    (1U << (0U))
 #define BSP_MSTP_DMY_FSP_IP_TRACECLOCK(channel) /* No dummy read is required after module stop. */
#endif

#define BSP_MSTP_REG_FSP_IP_RTC(channel)            R_SYSC_S->MSTPCRG
#define BSP_MSTP_BIT_FSP_IP_RTC(channel)            (1U << (5U))
#define BSP_MSTP_DMY_FSP_IP_RTC(channel) /* After module stop release, the waiting process is performed by the driver, \
                                          * so this macro does not define a register for dummy read. */

#if (4 == BSP_FEATURE_CGC_CLMA_UNIT)
 #define BSP_MSTP_REG_FSP_IP_CLMA(channel)          R_SYSC_S->MSTPCRG
 #define BSP_MSTP_BIT_FSP_IP_CLMA(channel)          ((2U >= channel) ? \
                                                     (1U << (9U + channel)) : (1U << (8U)))
 #define BSP_MSTP_DMY_FSP_IP_CLMA(channel)          (0 >=                                                               \
                                                     channel) ? R_CLMA0->CTL0 : ((1 >=                                  \
                                                                                  channel) ? R_CLMA1->CTL0 : ((2 >=     \
                                                                                                               channel) \
                                                                                                              ?         \
                                                                                                              R_CLMA2-> \
                                                                                                              CTL0 :    \
                                                                                                              R_CLMA3-> \
                                                                                                              CTL0))
#elif (7 == BSP_FEATURE_CGC_CLMA_UNIT)
 #define BSP_MSTP_REG_FSP_IP_CLMA(channel)          R_SYSC_S->MSTPCRG
 #define BSP_MSTP_BIT_FSP_IP_CLMA(channel)          ((5U >= channel) ? \
                                                     (1U << (9U + channel)) : (1U << (8U)))
 #define BSP_MSTP_DMY_FSP_IP_CLMA(channel)          ((0 >= channel) ? R_CLMA0->CTL0 :      \
                                                     ((1 >= channel) ? R_CLMA1->CTL0 :     \
                                                      ((2 >= channel) ? R_CLMA2->CTL0 :    \
                                                       ((3 >= channel) ? R_CLMA3->CTL0 :   \
                                                        ((4 >= channel) ? R_CLMA4->CTL0 :  \
                                                         ((5 >= channel) ? R_CLMA5->CTL0 : \
                                                          R_CLMA6->CTL0))))))
#endif

#if BSP_FEATURE_BSP_SHOSTIF_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_SHOSTIF(channel)       R_SYSC_S->MSTPCRI
 #define BSP_MSTP_BIT_FSP_IP_SHOSTIF(channel)       (1U << (1U))
 #define BSP_MSTP_DMY_FSP_IP_SHOSTIF(channel)       R_SHOSTIF->CTRLR0
#endif

#if BSP_FEATURE_BSP_PHOSTIF_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_PHOSTIF(channel)       R_SYSC_S->MSTPCRI
 #define BSP_MSTP_BIT_FSP_IP_PHOSTIF(channel)       (1U << (0U))
 #define BSP_MSTP_DMY_FSP_IP_PHOSTIF(channel)       R_PHOSTIF->HIFBCC
#endif

#if (2U == BSP_FEATURE_BSP_AFMT_UNIT)
 #define BSP_MSTP_REG_FSP_IP_AFMT(channel)          *((0U == channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_AFMT(channel)          (1U << (0U))
 #define BSP_MSTP_DMY_FSP_IP_AFMT(channel)          (0 == channel) ? R_AFMT0->ENC1RXDATA0 : R_AFMT1->ENC1RXDATA0
#elif (14U == BSP_FEATURE_BSP_AFMT_UNIT)
 #define BSP_MSTP_REG_FSP_IP_AFMT(channel)          *((8U >= channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_AFMT(channel)          ((8U >= channel) ? \
                                                     (1U << (channel * 4U)) : (1U << ((channel - 8U) * 4U)))
 #define BSP_MSTP_DMY_FSP_IP_AFMT(channel)          (0 >= channel) ? R_AFMT0->ENC1RXDATA0 : \
    ((1 >= channel) ? R_AFMT1->ENC1RXDATA0 :                                                \
     ((2 >= channel) ? R_AFMT2->ENC1RXDATA0 :                                               \
      ((3 >= channel) ? R_AFMT3->ENC1RXDATA0 :                                              \
       ((4 >= channel) ? R_AFMT4->ENC1RXDATA0 :                                             \
        ((5 >= channel) ? R_AFMT5->ENC1RXDATA0 :                                            \
         ((6 >= channel) ? R_AFMT6->ENC1RXDATA0 :                                           \
          ((7 >= channel) ? R_AFMT7->ENC1RXDATA0 :                                          \
           ((8 >= channel) ? R_AFMT8->ENC1RXDATA0 :                                         \
            ((9 >= channel) ? R_AFMT9->ENC1RXDATA0 :                                        \
             ((10 >= channel) ? R_AFMT10->ENC1RXDATA0 :                                     \
              ((11 >= channel) ? R_AFMT11->ENC1RXDATA0 :                                    \
               ((12 >= channel) ? R_AFMT12->ENC1RXDATA0 :                                   \
                ((13 >= channel) ? R_AFMT13->ENC1RXDATA0 :                                  \
                 ((14 >= channel) ? R_AFMT14->ENC1RXDATA0 :                                 \
                  R_AFMT15->ENC1RXDATA0))))))))))))))
#endif

#if (2 == BSP_FEATURE_BSP_HDSL_UNIT)
 #define BSP_MSTP_REG_FSP_IP_HDSL(channel)          *((0U == channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_HDSL(channel)          (1U << (1U))
 #define BSP_MSTP_DMY_FSP_IP_HDSL(channel)          (0 == channel) ? R_HDSLD0->SYS_CTRL : R_HDSLD1->SYS_CTRL
#elif (14 == BSP_FEATURE_BSP_HDSL_UNIT)
 #define BSP_MSTP_REG_FSP_IP_HDSL(channel)          *((8U >= channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_HDSL(channel)          ((8U >= channel) ?                      \
                                                     (1U << ((channel * 4U) + 1U)) : (1U << \
                                                                                      (((channel - 8U) * 4U) + 1U)))
 #define BSP_MSTP_DMY_FSP_IP_HDSL(channel)          (0 >= channel) ? R_HDSLD0->SYS_CTRL : \
    ((1 >= channel) ? R_HDSLD1->SYS_CTRL :                                                \
     ((2 >= channel) ? R_HDSLD2->SYS_CTRL :                                               \
      ((3 >= channel) ? R_HDSLD3->SYS_CTRL :                                              \
       ((4 >= channel) ? R_HDSLD4->SYS_CTRL :                                             \
        ((5 >= channel) ? R_HDSLD5->SYS_CTRL :                                            \
         ((6 >= channel) ? R_HDSLD6->SYS_CTRL :                                           \
          ((7 >= channel) ? R_HDSLD7->SYS_CTRL :                                          \
           ((8 >= channel) ? R_HDSLD8->SYS_CTRL :                                         \
            ((9 >= channel) ? R_HDSLD9->SYS_CTRL :                                        \
             ((10 >= channel) ? R_HDSLD10->SYS_CTRL :                                     \
              ((11 >= channel) ? R_HDSLD11->SYS_CTRL :                                    \
               ((12 >= channel) ? R_HDSLD12->SYS_CTRL :                                   \
                ((13 >= channel) ? R_HDSLD13->SYS_CTRL :                                  \
                 ((14 >= channel) ? R_HDSLD14->SYS_CTRL :                                 \
                  R_HDSLD15->SYS_CTRL))))))))))))))
#endif

#if (2 == BSP_FEATURE_BSP_BISS_UNIT)
 #define BSP_MSTP_REG_FSP_IP_BISS(channel)          *((0U == channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_BISS(channel)          (1U << (2U))
 #define BSP_MSTP_DMY_FSP_IP_BISS(channel)          (0 == channel) ? R_BISS0->SCDATA[0].L : R_BISS1->SCDATA[0].L
#elif (14 == BSP_FEATURE_BSP_BISS_UNIT)
 #define BSP_MSTP_REG_FSP_IP_BISS(channel)          *((0U == channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_BISS(channel)          ((8U >= channel) ?                      \
                                                     (1U << ((channel * 4U) + 2U)) : (1U << \
                                                                                      (((channel - 8U) * 4U) + 2U)))

 #define BSP_MSTP_DMY_FSP_IP_BISS(channel)          (0 == channel) ? R_BISS0->SCDATAL1 : \
    ((1 >= channel) ? R_BISS1->SCDATAL1 :                                                \
     ((2 >= channel) ? R_BISS2->SCDATAL1 :                                               \
      ((3 >= channel) ? R_BISS3->SCDATAL1 :                                              \
       ((4 >= channel) ? R_BISS4->SCDATAL1 :                                             \
        ((5 >= channel) ? R_BISS5->SCDATAL1 :                                            \
         ((6 >= channel) ? R_BISS6->SCDATAL1 :                                           \
          ((7 >= channel) ? R_BISS7->SCDATAL1 :                                          \
           ((8 >= channel) ? R_BISS8->SCDATAL1 :                                         \
            ((9 >= channel) ? R_BISS9->SCDATAL1 :                                        \
             ((10 >= channel) ? R_BISS10->SCDATAL1 :                                     \
              ((11 >= channel) ? R_BISS11->SCDATAL1 :                                    \
               ((12 >= channel) ? R_BISS12->SCDATAL1 :                                   \
                ((13 >= channel) ? R_BISS13->SCDATAL1 :                                  \
                 ((14 >= channel) ? R_BISS14->SCDATAL1 :                                 \
                  R_BISS5->SCDATAL1))))))))))))))
#endif

#if (2 == BSP_FEATURE_BSP_ENDAT_UNIT)
 #define BSP_MSTP_REG_FSP_IP_ENDAT(channel)         *((0U == channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_ENDAT(channel)         (1U << (3U))
 #define BSP_MSTP_DMY_FSP_IP_ENDAT(channel)         (0 == channel) ? R_ENDAT0->SEND : R_ENDAT1->SEND
#elif (14 == BSP_FEATURE_BSP_ENDAT_UNIT)
 #define BSP_MSTP_REG_FSP_IP_ENDAT(channel)         *((0U == channel) ? &R_SYSC_NS->MSTPCRJ : &R_SYSC_NS->MSTPCRK)
 #define BSP_MSTP_BIT_FSP_IP_ENDAT(channel)         ((8U >= channel) ?                      \
                                                     (1U << ((channel * 4U) + 3U)) : (1U << \
                                                                                      (((channel - 8U) * 4U) + 3U)))
 #define BSP_MSTP_DMY_FSP_IP_ENDAT(channel)         (0 >= channel) ? R_ENDAT0->SEND : \
    ((1 >= channel) ? R_ENDAT1->SEND :                                                \
     ((2 >= channel) ? R_ENDAT2->SEND :                                               \
      ((3 >= channel) ? R_ENDAT3->SEND :                                              \
       ((4 >= channel) ? R_ENDAT4->SEND :                                             \
        ((5 >= channel) ? R_ENDAT5->SEND :                                            \
         ((6 >= channel) ? R_ENDAT6->SEND :                                           \
          ((7 >= channel) ? R_ENDAT7->SEND :                                          \
           ((8 >= channel) ? R_ENDAT8->SEND :                                         \
            ((9 >= channel) ? R_ENDAT9->SEND :                                        \
             ((10 >= channel) ? R_ENDAT10->SEND :                                     \
              ((11 >= channel) ? R_ENDAT11->SEND :                                    \
               ((12 >= channel) ? R_ENDAT12->SEND :                                   \
                ((13 >= channel) ? R_ENDAT13->SEND :                                  \
                 ((14 >= channel) ? R_ENDAT14->SEND :                                 \
                  R_ENDAT15->SEND))))))))))))))
#endif

#if BSP_FEATURE_BSP_ENCOUT_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_ENCOUT(channel)        R_SYSC_NS->MSTPCRL
 #define BSP_MSTP_BIT_FSP_IP_ENCOUT(channel)        (1U << (0U))
 #define BSP_MSTP_DMY_FSP_IP_ENCOUT(channel)        R_ENCOUT->CTL
#endif

#if BSP_FEATURE_BSP_DDRSS_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_DDRSS(channel)         R_SYSC_NS->MSTPCRM
 #define BSP_MSTP_BIT_FSP_IP_DDRSS(channel)         (1U << (0U))
 #define BSP_MSTP_DMY_FSP_IP_DDRSS(channel)         R_DDRSS->DDR_MEMC_DENALI_CTL_00
#endif

#if BSP_FEATURE_BSP_LCDC_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_LCDC(channel)          R_SYSC_NS->MSTPCRM
 #define BSP_MSTP_BIT_FSP_IP_LCDC(channel)          (1U << (4U))
 #define BSP_MSTP_DMY_FSP_IP_LCDC(channel) /* After module stop release, the waiting process is performed by the driver, \
                                            * so this macro does not define a register for dummy read. */
#endif

#if BSP_FEATURE_BSP_PCIE_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_PCIE(channel)          R_SYSC_NS->MSTPCRM
 #define BSP_MSTP_BIT_FSP_IP_PCIE(channel)          (1U << (8U))
 #define BSP_MSTP_DMY_FSP_IP_PCIE(channel)          R_PCIE_LNK->PCIE_LINKMODE
#endif

#if BSP_FEATURE_BSP_SDHI_SUPPORTED
 #define BSP_MSTP_REG_FSP_IP_SDHI(channel)          R_SYSC_NS->MSTPCRM
 #define BSP_MSTP_BIT_FSP_IP_SDHI(channel)          (1U << (12U + channel))
 #define BSP_MSTP_DMY_FSP_IP_SDHI(channel)          (0 == channel) ? R_SDHI0->SD_CMD : R_SDHI1->SD_CMD
#endif

#if BSP_FEATURE_BSP_MSTP_CR52_CPU1_HAS_MSTPCRH
 #define BSP_MSTP_REG_FSP_IP_CPU1(channel)          R_SYSC_S->MSTPCRH
 #define BSP_MSTP_BIT_FSP_IP_CPU1(channel)          (1U << (1U))
 #define BSP_MSTP_DMY_FSP_IP_CPU1(channel) /* No dummy read is required after module stop. */
#endif

#if BSP_FEATURE_BSP_MSTP_CR52_HAS_MSTPCRN
 #define BSP_MSTP_REG_FSP_IP_CR52(channel)          R_SYSC_S->MSTPCRN
 #define BSP_MSTP_BIT_FSP_IP_CR52(channel)          (1U << (channel))
 #define BSP_MSTP_DMY_FSP_IP_CR52(channel)          (0 == channel) ? R_TCMAW->CPU0WAIT : \
    R_TCMAW->CPU1WAIT
#endif

#if BSP_FEATURE_BSP_MSTP_CA55_HAS_MSTPCRN
 #define BSP_MSTP_REG_FSP_IP_CA55(channel)          R_SYSC_S->MSTPCRN
 #define BSP_MSTP_BIT_FSP_IP_CA55(channel)          (1U << (channel + 2U))
 #define BSP_MSTP_DMY_FSP_IP_CA55(channel)          (0 == channel) ? R_CA55->RVBA[0].L : \
    ((1 >= channel) ? R_CA55->RVBA[1].L :                                                \
     ((2 >= channel) ? R_CA55->RVBA[2].L :                                               \
      R_CA55->RVBA[3].L))
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
