/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_CLOCKS_H
#define BSP_CLOCKS_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_clock_cfg.h"
#include "bsp_api.h"

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Startup clock frequency of each system clock. These macros are only helpful if the system clock and dividers have
 * not changed since startup. These macros are not used in FSP modules except for the clock startup code. */

/* xSPI unit0 clock options. */
#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_150_0_MHZ                   (0x01) // xSPI0 base clock 800MHz and xSPI0 clock 150.0MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_133_3_MHZ                   (0x02) // xSPI0 base clock 800MHz and xSPI0 clock 133.3MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_100_0_MHZ                   (0x03) // xSPI0 base clock 800MHz and xSPI0 clock 100.0 MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_50_0_MHZ                    (0x04) // xSPI0 base clock 800MHz and xSPI0 clock 50.0 MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_25_0_MHZ                    (0x05) // xSPI0 base clock 800MHz and xSPI0 clock 25.0 MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV0_12_5_MHZ                    (0x06) // xSPI0 base clock 800MHz and xSPI0 clock 12.5 MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV1_75_0_MHZ                    (0x43) // xSPI0 base clock 600MHz and xSPI0 clock 75.0 MHz.
#define BSP_CLOCKS_XSPI0_CLOCK_DIV1_37_5_MHZ                    (0x44) // xSPI0 base clock 600MHz and xSPI0 clock 37.5 MHz.

/* xSPI unit1 clock options. */
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_150_0_MHZ                   (0x01) // xSPI1 base clock 800MHz and xSPI1 clock 150.0MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_133_3_MHZ                   (0x02) // xSPI1 base clock 800MHz and xSPI1 clock 133.3MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_100_0_MHZ                   (0x03) // xSPI1 base clock 800MHz and xSPI1 clock 100.0 MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_50_0_MHZ                    (0x04) // xSPI1 base clock 800MHz and xSPI1 clock 50.0 MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_25_0_MHZ                    (0x05) // xSPI1 base clock 800MHz and xSPI1 clock 25.0 MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV0_12_5_MHZ                    (0x06) // xSPI1 base clock 800MHz and xSPI1 clock 12.5 MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV1_75_0_MHZ                    (0x43) // xSPI1 base clock 600MHz and xSPI1 clock 75.0 MHz.
#define BSP_CLOCKS_XSPI1_CLOCK_DIV1_37_5_MHZ                    (0x44) // xSPI1 base clock 600MHz and xSPI1 clock 37.5 MHz.

/* CKIO clock options. */
#if (1U == BSP_FEATURE_CGC_CKIO_CLOCK_FREQ_TYPE)
 #define BSP_CLOCKS_CKIO_ICLK_DIV2                              (0)    // CKIO clock 100.0 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 75.0 MHz  (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_CKIO_ICLK_DIV3                              (1)    // CKIO clock 66.7 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 50.0 MHz  (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_CKIO_ICLK_DIV4                              (2)    // CKIO clock 50.0 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 37.5 MHz  (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_CKIO_ICLK_DIV5                              (3)    // CKIO clock 40.0 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 30.0 MHz  (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_CKIO_ICLK_DIV6                              (4)    // CKIO clock 33.3 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 25.0 MHz  (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_CKIO_ICLK_DIV7                              (5)    // CKIO clock 28.6 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 21.4 MHz  (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_CKIO_ICLK_DIV8                              (6)    // CKIO clock 25.0 MHz  (when SCKCR2.DIVSELSUB = 0),
                                                                       // or 18.75 MHz (when SCKCR2.DIVSELSUB = 1).
#elif (2U == BSP_FEATURE_CGC_CKIO_CLOCK_FREQ_TYPE)
 #define BSP_CLOCKS_CKIO_125_0_MHZ                              (0)    // CKIO clock 125.0 MHz.
 #define BSP_CLOCKS_CKIO_83_3_MHZ                               (1)    // CKIO clock 83.3 MHz.
 #define BSP_CLOCKS_CKIO_62_5_MHZ                               (2)    // CKIO clock 62.5 MHz.
 #define BSP_CLOCKS_CKIO_50_0_MHZ                               (3)    // CKIO clock 50.0 MHz.
 #define BSP_CLOCKS_CKIO_41_7_MHZ                               (4)    // CKIO clock 41.7 MHz.
 #define BSP_CLOCKS_CKIO_35_7_MHZ                               (5)    // CKIO clock 35.7 MHz.
 #define BSP_CLOCKS_CKIO_31_25_MHZ                              (6)    // CKIO clock 31.25 MHz.
#endif

/* CANFD clock options. */
#define BSP_CLOCKS_CANFD_CLOCK_80_MHZ                           (0)    // CANFD clock 80 MHz.
#define BSP_CLOCKS_CANFD_CLOCK_40_MHZ                           (1)    // CANFD clock 40 MHz.
#define BSP_CLOCKS_CANFD_CLOCK_PCLKM                            (1)    // CANFD clock source PCLKM.

/* ENCOUT clock options. */
#define BSP_CLOCKS_ENCO_CLOCK_80_MHZ                            (1)    // ENCOUT clock 80 MHz.
#define BSP_CLOCKS_ENCO_CLOCK_20_MHZ                            (0)    // ENCOUT clock 20 MHz.

/* Ethernet PHY reference clock (ETHn_REFCLK : n = 0 to 2) options. */
#define BSP_CLOCKS_PHYSEL_PLL1_DIV                              (0)    // PLL1 devider clock.
#define BSP_CLOCKS_PHYSEL_MAINOSC_DIV                           (1)    // Main clock oscillator.

/* Alternative clock options when main clock abnormal oscillation is detected. */
#define BSP_CLOCKS_CLMASEL_LOCO                                 (0)    // LOCO clock.
#define BSP_CLOCKS_CLMASEL_PLL                                  (1)    // PLL clock.

/* LCDC clock options. */
#define BSP_CLOCKS_LCDCDIVSEL_DIV_2                             (0x00) // Divide LCDC source clock by 2.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_4                             (0x01) // Divide LCDC source clock by 4.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_6                             (0x02) // Divide LCDC source clock by 6.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_8                             (0x03) // Divide LCDC source clock by 8.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_10                            (0x04) // Divide LCDC source clock by 10.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_12                            (0x05) // Divide LCDC source clock by 12.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_14                            (0x06) // Divide LCDC source clock by 14.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_16                            (0x07) // Divide LCDC source clock by 16.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_18                            (0x08) // Divide LCDC source clock by 18.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_20                            (0x09) // Divide LCDC source clock by 20.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_22                            (0x0A) // Divide LCDC source clock by 22.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_24                            (0x0B) // Divide LCDC source clock by 24.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_26                            (0x0C) // Divide LCDC source clock by 26.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_28                            (0x0D) // Divide LCDC source clock by 28.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_30                            (0x0E) // Divide LCDC source clock by 30.
#define BSP_CLOCKS_LCDCDIVSEL_DIV_32                            (0x0F) // Divide LCDC source clock by 32.

/* SPI clock options. */
#if (1U == BSP_FEATURE_CGC_PCLKSPI_CLOCK_FREQ_TYPE)
 #define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SPI0 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SPI0 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SPI1 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SPI1 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SPI2 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SPI2 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SPI3 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SPI3 asynchronous serial clock 96.0 MHz.

#elif (2U == BSP_FEATURE_CGC_PCLKSPI_CLOCK_FREQ_TYPE)
 #define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SPI0 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SPI0 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SPI0 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI0_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SPI0 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SPI1 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SPI1 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SPI1 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI1_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SPI1 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SPI2 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SPI2 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SPI2 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI2_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SPI2 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SPI3 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SPI3 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SPI3 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SPI3_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SPI3 asynchronous serial clock 100.0 MHz.
#endif

/* SCI clock options. */
#if (1U == BSP_FEATURE_CGC_PCLKSCI_CLOCK_FREQ_TYPE)
 #define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SCI0 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SCI0 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SCI1 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SCI1 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SCI2 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SCI2 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SCI3 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SCI3 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SCI4 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SCI4 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0)    // SCI5 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (1)    // SCI5 asynchronous serial clock 96.0 MHz.
#elif (2U == BSP_FEATURE_CGC_PCLKSCI_CLOCK_FREQ_TYPE)
 #define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SCI0 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SCI0 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SCI0 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI0_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SCI0 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SCI1 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SCI1 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SCI1 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI1_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SCI1 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SCI2 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SCI2 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SCI2 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI2_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SCI2 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SCI3 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SCI3 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SCI3 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI3_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SCI3 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SCI4 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SCI4 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SCI4 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI4_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SCI4 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ       (0x00) // SCI5 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ       (0x01) // SCI5 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ       (0x02) // SCI5 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCI5_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ      (0x03) // SCI5 asynchronous serial clock 100.0 MHz.
#endif

/* SCIE clock options. */
#if (1U == BSP_FEATURE_SCIE_SUPPORTED)
 #define BSP_CLOCKS_SCIE0_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE0 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE0_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE0 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE0_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE0 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE0_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE0 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE1_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE1 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE1_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE1 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE1_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE1 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE1_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE1 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE2_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE2 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE2_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE2 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE2_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE2 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE2_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE2 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE3_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE3 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE3_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE3 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE3_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE3 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE3_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE3 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE4_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE4 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE4_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE4 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE4_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE4 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE4_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE4 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE5_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE5 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE5_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE5 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE5_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE5 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE5_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE5 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE6_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE6 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE6_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE6 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE6_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE6 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE6_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE6 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE7_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE7 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE7_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE7 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE7_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE7 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE7_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE7 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE8_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE8 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE8_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE8 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE8_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE8 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE8_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE8 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE9_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ      (0x00) // SCIE9 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE9_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ      (0x01) // SCIE9 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE9_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ      (0x02) // SCIE9 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE9_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ     (0x03) // SCIE9 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE10_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ     (0x00) // SCIE10 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE10_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ     (0x01) // SCIE10 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE10_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ     (0x02) // SCIE10 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE10_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ    (0x03) // SCIE10 asynchronous serial clock 100.0 MHz.
 #define BSP_CLOCKS_SCIE11_ASYNCHRONOUS_SERIAL_CLOCK_75_MHZ     (0x00) // SCIE11 asynchronous serial clock 75.0 MHz.
 #define BSP_CLOCKS_SCIE11_ASYNCHRONOUS_SERIAL_CLOCK_80_MHZ     (0x01) // SCIE11 asynchronous serial clock 80.0 MHz.
 #define BSP_CLOCKS_SCIE11_ASYNCHRONOUS_SERIAL_CLOCK_96_MHZ     (0x02) // SCIE11 asynchronous serial clock 96.0 MHz.
 #define BSP_CLOCKS_SCIE11_ASYNCHRONOUS_SERIAL_CLOCK_100_MHZ    (0x03) // SCIE11 asynchronous serial clock 100.0 MHz.
#endif

#if (1U == BSP_FEATURE_CGC_SCKCR_TYPE)

/* CR52 CPU0 clock options. */
 #define BSP_CLOCKS_FSELCPU0_ICLK_MUL2    (1) // CPU0 clock 400 MHz (when SCKCR2.DIVSELSUB = 0),
                                              // or 300 MHz (when SCKCR2.DIVSELSUB = 1).
 #define BSP_CLOCKS_FSELCPU0_ICLK_MUL1    (0) // CPU0 clock 200 MHz (when SCKCR2.DIVSELSUB = 0),
                                              // or 150 MHz (when SCKCR2.DIVSELSUB = 1).

/* Peripheral module base clock options. */
 #define BSP_CLOCKS_DIVSELSUB_0           (0) // ICLK:200MHz, PCLKH:200MHz, PCLKM:100MHz,
                                              // PCLKL:50MHz, PCLKADC:25MHz, PCLKGPTL:400MHz.
 #define BSP_CLOCKS_DIVSELSUB_1           (1) // ICLK:150MHz, PCLKH:150MHz, PCLKM:75 MHz,
                                              // PCLKL:37.5MHz, PCLKADC:18.75MHz, PCLKGPTL:300MHz.

#elif (2U == BSP_FEATURE_CGC_SCKCR_TYPE)

/* CR52 CPU0 clock options. */
 #define BSP_CLOCKS_CR52CPU0_500_MHZ           (0)    // CPU0 clock 500 MHz.
 #define BSP_CLOCKS_CR52CPU0_1000_MHZ          (1)    // CPU0 clock 1000 MHz.

/* CR52 CPU1 clock options. */
 #define BSP_CLOCKS_CR52CPU1_500_MHZ           (0)    // CPU1 clock 500 MHz.
 #define BSP_CLOCKS_CR52CPU1_1000_MHZ          (1)    // CPU1 clock 1000 MHz.

/* CA55 core0 clock options. */
 #define BSP_CLOCKS_CA55CORE0_600_MHZ          (0)    // CA55 core0 clock 600 MHz.
 #define BSP_CLOCKS_CA55CORE0_1200_MHZ         (1)    // CA55 core0 clock 1200 MHz.

/* CA55 core1 clock options. */
 #define BSP_CLOCKS_CA55CORE1_600_MHZ          (0)    // CA55 core1 clock 600 MHz.
 #define BSP_CLOCKS_CA55CORE1_1200_MHZ         (1)    // CA55 core1 clock 1200 MHz.

/* CA55 core2 clock options. */
 #define BSP_CLOCKS_CA55CORE2_600_MHZ          (0)    // CA55 core2 clock 600 MHz.
 #define BSP_CLOCKS_CA55CORE2_1200_MHZ         (1)    // CA55 core2 clock 1200 MHz.

/* CA55 core3 clock options. */
 #define BSP_CLOCKS_CA55CORE3_600_MHZ          (0)    // CA55 core3 clock 600 MHz.
 #define BSP_CLOCKS_CA55CORE3_1200_MHZ         (1)    // CA55 core3 clock 1200 MHz.

/* CA55 SCU CLOCK options. */
 #define BSP_CLOCKS_CA55SCLK_500_MHZ           (0)    // CA55 SCU clock 500 MHz.
 #define BSP_CLOCKS_CA55SCLK_1000_MHZ          (1)    // CA55 SCU clock 1000 MHz.
#endif

/* LOCO enable options. */
#define BSP_CLOCKS_LOCO_DISABLE                (0)    // LOCO Stop
#define BSP_CLOCKS_LOCO_ENABLE                 (1)    // LOCO Run

/* PLL0 enable options. */
#define BSP_CLOCKS_PLL0_STANDBY                (0)    // PLL0 is standby state.
#define BSP_CLOCKS_PLL0_NORMAL                 (1)    // PLL0 is normal state.

/* PLL1 enable options. */
#define BSP_CLOCKS_PLL1_INITIAL                (0xFF) // Initial (This value should not be reflected in the register)
#define BSP_CLOCKS_PLL1_STANDBY                (0)    // PLL1 is standby state.
#define BSP_CLOCKS_PLL1_NORMAL                 (1)    // PLL1 is normal state.

/* PLL2 enable options. */
#define BSP_CLOCKS_PLL2_STANDBY                (0)    // PLL2 is standby state.
#define BSP_CLOCKS_PLL2_NORMAL                 (1)    // PLL2 is normal state.

/* PLL3 enable options. */
#define BSP_CLOCKS_PLL3_STANDBY                (0)    // PLL3 is standby state.
#define BSP_CLOCKS_PLL3_NORMAL                 (1)    // PLL3 is normal state.

/* PLL0 SSC options. */
#define BSP_CLOCKS_PLL0SSCEN_DISABLE           (0)    // PLL0 SSC function is enabled
#define BSP_CLOCKS_PLL0SSCEN_ENABLE            (1)    // PLL0 SSC function is disabled

/* PLL2 SSC options. */
#define BSP_CLOCKS_PLL2SSCEN_DISABLE           (0)    // PLL2 SSC function is enabled
#define BSP_CLOCKS_PLL2SSCEN_ENABLE            (1)    // PLL2 SSC function is disabled

/* CLMA error mask options. */
#define BSP_CLOCKS_CLMA0_ERROR_MASK            (0)    // CLMA0 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA0_ERROR_NOT_MASK        (1)    // CLMA0 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA1_ERROR_MASK            (0)    // CLMA1 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA1_ERROR_NOT_MASK        (1)    // CLMA1 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA2_ERROR_MASK            (0)    // CLMA0 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA2_ERROR_NOT_MASK        (1)    // CLMA0 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA3_ERROR_MASK            (0)    // CLMA3 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA3_ERROR_NOT_MASK        (1)    // CLMA3 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA4_ERROR_MASK            (0)    // CLMA4 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA4_ERROR_NOT_MASK        (1)    // CLMA4 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA5_ERROR_MASK            (0)    // CLMA5 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA5_ERROR_NOT_MASK        (1)    // CLMA5 error is transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA6_ERROR_MASK            (0)    // CLMA6 error is not transferred to POE3 and POEG.
#define BSP_CLOCKS_CLMA6_ERROR_NOT_MASK        (1)    // CLMA6 error is transferred to POE3 and POEG.

/* CLMA enable options. */
#define BSP_CLOCKS_CLMA0_DISABLE               (0)    // Disable CLMA0 operation.
#define BSP_CLOCKS_CLMA0_ENABLE                (1)    // Enable CLMA0 operation.
#define BSP_CLOCKS_CLMA1_DISABLE               (0)    // Disable CLMA1 operation.
#define BSP_CLOCKS_CLMA1_ENABLE                (1)    // Enable CLMA1 operation.
#define BSP_CLOCKS_CLMA2_DISABLE               (0)    // Disable CLMA2 operation.
#define BSP_CLOCKS_CLMA2_ENABLE                (1)    // Enable CLMA2 operation.
#define BSP_CLOCKS_CLMA3_DISABLE               (0)    // Disable CLMA3 operation.
#define BSP_CLOCKS_CLMA3_ENABLE                (1)    // Enable CLMA3 operation.
#define BSP_CLOCKS_CLMA4_DISABLE               (0)    // Disable CLMA4 operation.
#define BSP_CLOCKS_CLMA4_ENABLE                (1)    // Enable CLMA4 operation.
#define BSP_CLOCKS_CLMA5_DISABLE               (0)    // Disable CLMA5 operation.
#define BSP_CLOCKS_CLMA5_ENABLE                (1)    // Enable CLMA5 operation.
#define BSP_CLOCKS_CLMA6_DISABLE               (0)    // Disable CLMA6 operation.
#define BSP_CLOCKS_CLMA6_ENABLE                (1)    // Enable CLMA6 operation.

#if (1U == BSP_FEATURE_CGC_SCKCR_TYPE)

/* Create a mask of valid bits in SCKCR. */
 #define BSP_PRV_SCKCR_FSELXSPI0_MASK          (7U << 0)
 #define BSP_PRV_SCKCR_DIVSELXSPI0_MASK        (1U << 6)
 #define BSP_PRV_SCKCR_FSELXSPI1_MASK          (7U << 8)
 #define BSP_PRV_SCKCR_DIVSELXSPI1_MASK        (1U << 14)
 #define BSP_PRV_SCKCR_CKIO_MASK               (7U << 16)
 #define BSP_PRV_SCKCR_FSELCANFD_MASK          (1U << 20)
 #define BSP_PRV_SCKCR_PHYSEL_MASK             (1U << 21)
 #define BSP_PRV_SCKCR_CLMASEL_MASK            (1U << 22)
 #define BSP_PRV_SCKCR_SPI0ASYNCSEL_MASK       (1U << 24)
 #define BSP_PRV_SCKCR_SPI1ASYNCSEL_MASK       (1U << 25)
 #define BSP_PRV_SCKCR_SPI2ASYNCSEL_MASK       (1U << 26)
 #define BSP_PRV_SCKCR_SCI0ASYNCSEL_MASK       (1U << 27)
 #define BSP_PRV_SCKCR_SCI1ASYNCSEL_MASK       (1U << 28)
 #define BSP_PRV_SCKCR_SCI2ASYNCSEL_MASK       (1U << 29)
 #define BSP_PRV_SCKCR_SCI3ASYNCSEL_MASK       (1U << 30)
 #define BSP_PRV_SCKCR_SCI4ASYNCSEL_MASK       (1U << 31)
 #define BSP_PRV_SCKCR_MASK                    (((((((((((((((BSP_PRV_SCKCR_FSELXSPI0_MASK |    \
                                                              BSP_PRV_SCKCR_DIVSELXSPI0_MASK) | \
                                                             BSP_PRV_SCKCR_FSELXSPI1_MASK) |    \
                                                            BSP_PRV_SCKCR_DIVSELXSPI1_MASK) |   \
                                                           BSP_PRV_SCKCR_CKIO_MASK) |           \
                                                          BSP_PRV_SCKCR_FSELCANFD_MASK) |       \
                                                         BSP_PRV_SCKCR_PHYSEL_MASK) |           \
                                                        BSP_PRV_SCKCR_CLMASEL_MASK) |           \
                                                       BSP_PRV_SCKCR_SPI0ASYNCSEL_MASK) |       \
                                                      BSP_PRV_SCKCR_SPI1ASYNCSEL_MASK) |        \
                                                     BSP_PRV_SCKCR_SPI2ASYNCSEL_MASK) |         \
                                                    BSP_PRV_SCKCR_SCI0ASYNCSEL_MASK) |          \
                                                   BSP_PRV_SCKCR_SCI1ASYNCSEL_MASK) |           \
                                                  BSP_PRV_SCKCR_SCI2ASYNCSEL_MASK) |            \
                                                 BSP_PRV_SCKCR_SCI3ASYNCSEL_MASK) |             \
                                                BSP_PRV_SCKCR_SCI4ASYNCSEL_MASK)

 #define BSP_PRV_SCKCR_DIVSELXSPI_MASK         (BSP_PRV_SCKCR_DIVSELXSPI0_MASK | \
                                                BSP_PRV_SCKCR_DIVSELXSPI1_MASK)

/* Create a mask of valid bits in SCKCR2. */
 #define BSP_PRV_SCKCR2_FSELCPU0_MASK          (3U << 0)
 #define BSP_PRV_SCKCR2_RESERVED_BIT4_MASK     (1U << 4)
 #define BSP_PRV_SCKCR2_DIVSELSUB_MASK         (1U << 5)
 #define BSP_PRV_SCKCR2_SPI3ASYNCSEL_MASK      (1U << 24)
 #define BSP_PRV_SCKCR2_SCI5ASYNCSEL_MASK      (1U << 25)
 #define BSP_PRV_SCKCR2_MASK                   ((((BSP_PRV_SCKCR2_FSELCPU0_MASK |       \
                                                   BSP_PRV_SCKCR2_RESERVED_BIT4_MASK) | \
                                                  BSP_PRV_SCKCR2_DIVSELSUB_MASK) |      \
                                                 BSP_PRV_SCKCR2_SPI3ASYNCSEL_MASK) |    \
                                                BSP_PRV_SCKCR2_SCI5ASYNCSEL_MASK)

#elif (2U == BSP_FEATURE_CGC_SCKCR_TYPE)

/* Create a mask of valid bits in SCKCR. */
 #define BSP_PRV_SCKCR_FSELXSPI0_MASK          (7U << 0)
 #define BSP_PRV_SCKCR_DIVSELXSPI0_MASK        (1U << 6)
 #define BSP_PRV_SCKCR_FSELXSPI1_MASK          (7U << 8)
 #define BSP_PRV_SCKCR_DIVSELXSPI1_MASK        (1U << 14)
 #define BSP_PRV_SCKCR_CKIO_MASK               (7U << 16)
 #define BSP_PRV_SCKCR_FSELCANFD_MASK          (1U << 20)
 #define BSP_PRV_SCKCR_PHYSEL_MASK             (1U << 21)
 #define BSP_PRV_SCKCR_CLMASEL_MASK            (1U << 22)

 #define BSP_PRV_SCKCR_MASK                    (((((((BSP_PRV_SCKCR_FSELXSPI0_MASK |    \
                                                      BSP_PRV_SCKCR_DIVSELXSPI0_MASK) | \
                                                     BSP_PRV_SCKCR_FSELXSPI1_MASK) |    \
                                                    BSP_PRV_SCKCR_DIVSELXSPI1_MASK) |   \
                                                   BSP_PRV_SCKCR_CKIO_MASK) |           \
                                                  BSP_PRV_SCKCR_FSELCANFD_MASK) |       \
                                                 BSP_PRV_SCKCR_PHYSEL_MASK) |           \
                                                BSP_PRV_SCKCR_CLMASEL_MASK)

 #define BSP_PRV_SCKCR_DIVSELXSPI_MASK         (BSP_PRV_SCKCR_DIVSELXSPI0_MASK | \
                                                BSP_PRV_SCKCR_DIVSELXSPI1_MASK)

/* Create a mask of valid bits in SCKCR2. */
 #define BSP_PRV_SCKCR2_CR52CPU0_MASK          (3U << 0)
 #define BSP_PRV_SCKCR2_CR52CPU1_MASK          (3U << 2)
 #define BSP_PRV_SCKCR2_CA55CORE0_MASK         (1U << 8)
 #define BSP_PRV_SCKCR2_CA55CORE1_MASK         (1U << 9)
 #define BSP_PRV_SCKCR2_CA55CORE2_MASK         (1U << 10)
 #define BSP_PRV_SCKCR2_CA55CORE3_MASK         (1U << 11)
 #define BSP_PRV_SCKCR2_CA55SCLK_MASK          (1U << 12)
 #define BSP_PRV_SCKCR2_SPI3ASYNCSEL_MASK      (3U << 16)
 #define BSP_PRV_SCKCR2_SCI5ASYNCSEL_MASK      (3U << 18)
 #define BSP_PRV_SCKCR2_MASK                   ((((((((BSP_PRV_SCKCR2_CR52CPU0_MASK |  \
                                                       BSP_PRV_SCKCR2_CR52CPU1_MASK) | \
                                                      BSP_PRV_SCKCR2_CA55CORE0_MASK) | \
                                                     BSP_PRV_SCKCR2_CA55CORE1_MASK) |  \
                                                    BSP_PRV_SCKCR2_CA55CORE2_MASK) |   \
                                                   BSP_PRV_SCKCR2_CA55CORE3_MASK) |    \
                                                  BSP_PRV_SCKCR2_CA55SCLK_MASK) |      \
                                                 BSP_PRV_SCKCR2_SPI3ASYNCSEL_MASK) |   \
                                                BSP_PRV_SCKCR2_SCI5ASYNCSEL_MASK)

/* Create a mask of valid bits in SCKCR3. */
 #define BSP_PRV_SCKCR3_SPI0ASYNCSEL_MASK      (3U << 0)
 #define BSP_PRV_SCKCR3_SPI1ASYNCSEL_MASK      (3U << 2)
 #define BSP_PRV_SCKCR3_SPI2ASYNCSEL_MASK      (3U << 4)
 #define BSP_PRV_SCKCR3_SCI0ASYNCSEL_MASK      (3U << 6)
 #define BSP_PRV_SCKCR3_SCI1ASYNCSEL_MASK      (3U << 8)
 #define BSP_PRV_SCKCR3_SCI2ASYNCSEL_MASK      (3U << 10)
 #define BSP_PRV_SCKCR3_SCI3ASYNCSEL_MASK      (3U << 12)
 #define BSP_PRV_SCKCR3_SCI4ASYNCSEL_MASK      (3U << 14)
 #define BSP_PRV_SCKCR3_LCDCDIVSEL_MASK        (15U << 20)
 #define BSP_PRV_SCKCR3_MASK                   (((((((((BSP_PRV_SCKCR3_SPI0ASYNCSEL_MASK) | \
                                                       BSP_PRV_SCKCR3_SPI1ASYNCSEL_MASK) |  \
                                                      BSP_PRV_SCKCR3_SPI2ASYNCSEL_MASK) |   \
                                                     BSP_PRV_SCKCR3_SCI0ASYNCSEL_MASK) |    \
                                                    BSP_PRV_SCKCR3_SCI1ASYNCSEL_MASK) |     \
                                                   BSP_PRV_SCKCR3_SCI2ASYNCSEL_MASK) |      \
                                                  BSP_PRV_SCKCR3_SCI3ASYNCSEL_MASK) |       \
                                                 BSP_PRV_SCKCR3_SCI4ASYNCSEL_MASK) |        \
                                                BSP_PRV_SCKCR3_LCDCDIVSEL_MASK)

/* Create a mask of valid bits in SCKCR4. */
 #define BSP_PRV_SCKCR4_SCIE0ASYNCSEL_MASK     (3U << 0)
 #define BSP_PRV_SCKCR4_SCIE1ASYNCSEL_MASK     (3U << 2)
 #define BSP_PRV_SCKCR4_SCIE2ASYNCSEL_MASK     (3U << 4)
 #define BSP_PRV_SCKCR4_SCIE3ASYNCSEL_MASK     (3U << 6)
 #define BSP_PRV_SCKCR4_SCIE4ASYNCSEL_MASK     (3U << 8)
 #define BSP_PRV_SCKCR4_SCIE5ASYNCSEL_MASK     (3U << 10)
 #define BSP_PRV_SCKCR4_SCIE6ASYNCSEL_MASK     (3U << 12)
 #define BSP_PRV_SCKCR4_SCIE7ASYNCSEL_MASK     (3U << 14)
 #define BSP_PRV_SCKCR4_SCIE8ASYNCSEL_MASK     (3U << 16)
 #define BSP_PRV_SCKCR4_SCIE9ASYNCSEL_MASK     (3U << 18)
 #define BSP_PRV_SCKCR4_SCIE10ASYNCSEL_MASK    (3U << 20)
 #define BSP_PRV_SCKCR4_SCIE11ASYNCSEL_MASK    (3U << 22)
 #define BSP_PRV_SCKCR4_ENCOUTCLK_MASK         (1U << 24)
 #define BSP_PRV_SCKCR4_MASK                   (((((((((((((BSP_PRV_SCKCR4_SCIE0ASYNCSEL_MASK) | \
                                                           BSP_PRV_SCKCR4_SCIE1ASYNCSEL_MASK) |  \
                                                          BSP_PRV_SCKCR4_SCIE2ASYNCSEL_MASK) |   \
                                                         BSP_PRV_SCKCR4_SCIE3ASYNCSEL_MASK) |    \
                                                        BSP_PRV_SCKCR4_SCIE4ASYNCSEL_MASK) |     \
                                                       BSP_PRV_SCKCR4_SCIE5ASYNCSEL_MASK) |      \
                                                      BSP_PRV_SCKCR4_SCIE6ASYNCSEL_MASK) |       \
                                                     BSP_PRV_SCKCR4_SCIE7ASYNCSEL_MASK) |        \
                                                    BSP_PRV_SCKCR4_SCIE8ASYNCSEL_MASK) |         \
                                                   BSP_PRV_SCKCR4_SCIE9ASYNCSEL_MASK) |          \
                                                  BSP_PRV_SCKCR4_SCIE10ASYNCSEL_MASK) |          \
                                                 BSP_PRV_SCKCR4_SCIE11ASYNCSEL_MASK) |           \
                                                BSP_PRV_SCKCR4_ENCOUTCLK_MASK)

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

/* Public functions defined in bsp.h */
void bsp_clock_init(void);             // Used internally by BSP

/* Used internally by CGC */

void bsp_prv_clock_set(uint32_t sckcr, uint32_t sckcr2, uint32_t sckcr3, uint32_t sckcr4);

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
