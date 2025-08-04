/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef BSP_DDR_FW_PARAM_H
#define BSP_DDR_FW_PARAM_H

/***********************************************************************************************************************
 * Includes <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <stdio.h>
#include "bsp_api.h"

#if (BSP_FEATURE_DDR_SUPPORTED)
 #if (1 == BSP_CFG_DDR_INIT_ENABLE)
  #include "bsp_ddr_fw_param.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern const uint32_t phyinit_fw_1d[];
extern const uint32_t phyinit_fw_2d[];

 #endif
#endif

#endif
