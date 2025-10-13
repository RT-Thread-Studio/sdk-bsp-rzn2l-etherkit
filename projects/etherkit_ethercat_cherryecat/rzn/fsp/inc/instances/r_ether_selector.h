/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*******************************************************************************************************************//**
 * @addtogroup ETHER_SELECTOR
 * @{
 **********************************************************************************************************************/

#ifndef R_ETHER_SELECTOR_H
#define R_ETHER_SELECTOR_H

#include "bsp_api.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_ether_selector_cfg.h"
#include "r_ether_selector_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** ETHER SELECTOR control block. DO NOT INITIALIZE.  Initialization occurs when @ref ether_selector_api_t::open is called. */
typedef struct st_ether_selector_instance_ctrl
{
    uint32_t open;                      ///< Used to determine if the channel is configured

    /* Configuration of Ethernet Selector module. */
    ether_selector_cfg_t const * p_cfg; ///< Pointer to initial configurations.

    /* Interface for Ethernet Selector */
    R_ETHSS_Type * p_reg;               ///< Pointer to Ethernet Subsystem peripheral registers.
} ether_selector_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const ether_selector_api_t g_ether_selector_on_ether_selector;

/** @endcond */

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Public Function Prototypes
 **********************************************************************************************************************/
fsp_err_t R_ETHER_SELECTOR_Open(ether_selector_ctrl_t * const p_ctrl, ether_selector_cfg_t const * const p_cfg);

fsp_err_t R_ETHER_SELECTOR_Close(ether_selector_ctrl_t * const p_ctrl);

fsp_err_t R_ETHER_SELECTOR_ConverterSet(ether_selector_ctrl_t * const p_ctrl,
                                        ether_selector_speed_t        speed,
                                        ether_selector_duplex_t       duplex);

/*******************************************************************************************************************//**
 * @} (end addtogroup ETHER_SELECTOR)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // R_ETHER_SELECTOR_H
