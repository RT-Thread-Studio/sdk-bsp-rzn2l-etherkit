/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_SLAVE_STOP_H
#define BSP_SLAVE_STOP_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define BSP_SLAVE_STOP_SSTPCR0_SELECT    (0x04000000U)
#define BSP_SLAVE_STOP_SSTPCR1_SELECT    (0x04010000U)
#define BSP_SLAVE_STOP_SSTPCR4_SELECT    (0x00000000U)
#define BSP_SLAVE_STOP_SSTPCR5_SELECT    (0x00010000U)
#define BSP_SLAVE_STOP_SSTPCR6_SELECT    (0x00020000U)
#define BSP_SLAVE_STOP_SSTPCR7_SELECT    (0x00030000U)

#define BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(req_pos, ack_pos)    (((req_pos) << 8U) | ((ack_pos) << 0U))

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/
typedef enum e_bsp_bus_slave
{
    BSP_BUS_SLAVE_DDRSS_A0_IF = (BSP_SLAVE_STOP_SSTPCR0_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(0, 1)),
    BSP_BUS_SLAVE_DDRSS_A1_IF = (BSP_SLAVE_STOP_SSTPCR0_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(4, 5)),
    BSP_BUS_SLAVE_DDRSS_R2_IF = (BSP_SLAVE_STOP_SSTPCR4_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(0, 1)),
    BSP_BUS_SLAVE_DDRSS_R3_IF = (BSP_SLAVE_STOP_SSTPCR4_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(4, 5)),
    BSP_BUS_SLAVE_DDRSS_A4_IF = (BSP_SLAVE_STOP_SSTPCR0_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(8, 9)),
    BSP_BUS_SLAVE_PCIE0       = (BSP_SLAVE_STOP_SSTPCR1_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(0, 1)),
    BSP_BUS_SLAVE_PCIE1       = (BSP_SLAVE_STOP_SSTPCR1_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(4, 5)),
    BSP_BUS_SLAVE_SDHI0       = (BSP_SLAVE_STOP_SSTPCR1_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(16, 17)),
    BSP_BUS_SLAVE_SDHI1       = (BSP_SLAVE_STOP_SSTPCR1_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(20, 21)),
    BSP_BUS_SLAVE_GIC600      = (BSP_SLAVE_STOP_SSTPCR5_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(16, 17)),
    BSP_BUS_SLAVE_XSPI0       = (BSP_SLAVE_STOP_SSTPCR6_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(0, 1)),
    BSP_BUS_SLAVE_XSPI1       = (BSP_SLAVE_STOP_SSTPCR6_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(4, 5)),
    BSP_BUS_SLAVE_CR520_AXIS  = (BSP_SLAVE_STOP_SSTPCR7_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(0, 1)),
    BSP_BUS_SLAVE_CR521_AXIS  = (BSP_SLAVE_STOP_SSTPCR7_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(4, 5)),
    BSP_BUS_SLAVE_DDRAPB      = (BSP_SLAVE_STOP_SSTPCR4_SELECT | BSP_SLAVE_STOP_SSTPCR_REQ_ACK_POS(8, 8)),
} bsp_bus_slave_t;

/** @} (end addtogroup BSP_MCU) */

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif
