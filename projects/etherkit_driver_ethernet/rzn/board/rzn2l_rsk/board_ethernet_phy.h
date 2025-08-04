/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @ingroup BOARD_RZN2_RSK
 * @defgroup BOARD_RZN2_RSK_ETHERNET_PHY Board Ethernet Phy
 * @brief Ethernet Phy information for this board.
 *
 * This is code specific to the RZN2_RSK board.
 *
 * @{
 **********************************************************************************************************************/

#ifndef BSP_ETHERNET_PHY_H
#define BSP_ETHERNET_PHY_H

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define ETHER_PHY_CFG_TARGET_VSC8541_ENABLE    (1)
#define ETHER_PHY_LSI_TYPE_KIT_COMPONENT       ETHER_PHY_LSI_TYPE_VSC8541

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif

/** @} (end defgroup BOARD_RZN2_RSK_ETHERNET_PHY) */
