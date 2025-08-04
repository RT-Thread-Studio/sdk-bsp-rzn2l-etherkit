/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"

#if !(BSP_CFG_RAM_EXECUTION) && (BSP_CFG_USE_LOADER_PARAMETER)

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
 #define BSP_LOADER_PARAM_MAX    (19)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/* Parameter Information for the Loader. */
BSP_DONT_REMOVE const uint32_t g_bsp_loader_param[BSP_LOADER_PARAM_MAX] BSP_PLACE_IN_SECTION(BSP_SECTION_LOADER_PARAM) =
{
    BSP_CFG_CACHE_FLG,
    BSP_CFG_CS0BCR_V_WRAPCFG_V,
    BSP_CFG_CS0WCR_V_COMCFG_V,
    BSP_CFG_DUMMY0_BMCFG_V,
    BSP_CFG_BSC_FLG_xSPI_FLG,
    BSP_CFG_LDR_ADDR_NML,
    BSP_CFG_LDR_SIZE_NML,
    BSP_CFG_DEST_ADDR_NML,
    BSP_CFG_DUMMY1,
    BSP_CFG_DUMMY2,
    BSP_CFG_DUMMY3_CSSCTL_V,
    BSP_CFG_DUMMY4_LIOCFGCS0_V,
    BSP_CFG_DUMMY5,
    BSP_CFG_DUMMY6,
    BSP_CFG_DUMMY7,
    BSP_CFG_DUMMY8,
    BSP_CFG_DUMMY9,
    BSP_CFG_DUMMY10_ACCESS_SPEED,
    BSP_CFG_CHECK_SUM
};

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
#endif
