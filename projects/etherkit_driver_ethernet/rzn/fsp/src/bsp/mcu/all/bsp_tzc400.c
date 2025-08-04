/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"
#if (1 == BSP_FEATURE_TZC400_SUPPORTED)
 #include "bsp_tzc400.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if BSP_FEATURE_TZC400_SUPPORTED
 #if (1 == _RZN_ORDINAL)
  #define BSP_TZC_400_BASEADRESS                               (0x81100000)
  #define BSP_TZC_400_MODULE_OFFSET                            (uint32_t) (0x1000)
  #define BSP_TZC_400_REGION_ACTION_OFFSET                     (uint32_t) (0x004)
  #define BSP_TZC_400_REGION_SPEC_DISABLEULATION_CTL_OFFSET    (uint32_t) (0x00C)
  #define BSP_TZC_400_REGION_GATEKEEPER_OFFSET                 (uint32_t) (0x008)
  #define BSP_TZC_400_REGION_ATTR_N_OFFSET(n)         (uint32_t) (0x110 + (0x20 * n))
  #define BSP_TZC_400_REGION_ID_ACCESS_N_OFFSET(n)    (uint32_t) (0x114 + (0x20 * n))
  #define BSP_TZC_400_NUMBER_SHIFT_NSAID_WR_EN                 16U
  #define BSP_TZC_400_NUMBER_SHIFT_NSAID_RD_EN                 0U
  #define BSP_TZC_400_MAX_MODULE                               9U
  #define BSP_TZC_400_MAX_REGION_SUPPORT                       1U
  #define BSP_TZC_400_MAX_FILTER                               4U

  #define BSP_TZC400_REGION(module, region)                                    \
    {                                                                          \
        BSP_CFG_TZC400_ ## module ## _REGION_ID_ACCESS_NSAID_RD_EN_ ## region, \
        BSP_CFG_TZC400_ ## module ## _REGION_ID_ACCESS_NSAID_WR_EN_ ## region, \
        BSP_CFG_TZC400_ ## module ## _REGION_ATTR_S_RD_EN_ ## region,          \
        BSP_CFG_TZC400_ ## module ## _REGION_ATTR_S_WR_EN_ ## region,          \
        BSP_CFG_TZC400_ ## module ## _REGION_ATTR_FILTER_UNIT_ ## region,      \
    }
  #define BSP_TZC400_MODULE(module)                       \
    {                                                     \
        BSP_CFG_TZC400_ ## module ## _GATEKEEPER,         \
        BSP_CFG_TZC400_ ## module ## _WRITE_SPEC_DISABLE, \
        BSP_CFG_TZC400_ ## module ## _READ_SPEC_DISABLE,  \
        {                                                 \
            BSP_TZC400_REGION(module, 0),                 \
        }                                                 \
    }
 #endif
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
#if BSP_FEATURE_TZC400_SUPPORTED
 #if (1 == _RZN_ORDINAL)
typedef struct st_bsp_tzc400_region_cfg_data
{
    uint16_t id_access_rd_en;
    uint16_t id_access_wr_en;
    uint8_t  attr_rd_en;
    uint8_t  attr_wr_en;
    uint8_t  attr_filter;
} bsp_tzc400_region_cfg_data_t;

typedef struct st_bsp_tzc400_cfg_data
{
    uint32_t gatekeeper;
    uint8_t  write_spec_disable;
    uint8_t  read_spec_disable;

    bsp_tzc400_region_cfg_data_t region[BSP_TZC_400_MAX_REGION_SUPPORT];
} bsp_tzc400_cfg_data_t;
 #endif
#endif

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
#if BSP_FEATURE_TZC400_SUPPORTED
 #if (1 == _RZN_ORDINAL)
static const bsp_tzc400_cfg_data_t g_bsp_tzc_module_cfg_data[] =
{
    BSP_TZC400_MODULE(0),
    BSP_TZC400_MODULE(1),
    BSP_TZC400_MODULE(2),
    BSP_TZC400_MODULE(3),
    BSP_TZC400_MODULE(4),
    BSP_TZC400_MODULE(5),
    BSP_TZC400_MODULE(6),
    BSP_TZC400_MODULE(7),
    BSP_TZC400_MODULE(8),
};

/*******************************************************************************************************************//**
 * Configure TrustedZone 400.
 * @param[in]    None
 * @param[out]   None
 * @note         This function supports configurable the registers of TZC-400.
 * @retval       None
 **********************************************************************************************************************/
void bsp_tzc_400_cfg (void)
{
    /* Initialize local variable */
    uint8_t             tzc400module;
    uintptr_t           basetzc400;
    volatile uint32_t * p_reg;

    /* Loop all modules of TZC400 */
    for (tzc400module = 0; tzc400module < BSP_TZC_400_MAX_MODULE; tzc400module++)
    {
        /* Get base address of each module */
        basetzc400 = (uintptr_t) (BSP_TZC_400_BASEADRESS + (BSP_TZC_400_MODULE_OFFSET * tzc400module));

        /* Setting for GATE_KEEPER */
        p_reg  = (uint32_t *) (basetzc400 + BSP_TZC_400_REGION_GATEKEEPER_OFFSET);
        *p_reg = g_bsp_tzc_module_cfg_data[tzc400module].gatekeeper;

        /* Setting for SPECULATION_CTRL */
        p_reg  = (uint32_t *) (basetzc400 + BSP_TZC_400_REGION_SPEC_DISABLEULATION_CTL_OFFSET);
        *p_reg = (uint32_t) ((g_bsp_tzc_module_cfg_data[tzc400module].write_spec_disable << 1) |
                             (g_bsp_tzc_module_cfg_data[tzc400module].read_spec_disable << 0));

        /* Setting attribute for region 0 only as BSP_TZC_400_MAX_REGION_SUPPORT */
        p_reg  = (uint32_t *) (basetzc400 + BSP_TZC_400_REGION_ATTR_N_OFFSET(0));
        *p_reg = (uint32_t) ((g_bsp_tzc_module_cfg_data[tzc400module].region[0].attr_rd_en << 30) |
                             (g_bsp_tzc_module_cfg_data[tzc400module].region[0].attr_wr_en << 31) |
                             (g_bsp_tzc_module_cfg_data[tzc400module].region[0].attr_filter));

        /* Setting id access for region 0 only as BSP_TZC_400_MAX_REGION_SUPPORT */
        p_reg  = (uint32_t *) (basetzc400 + BSP_TZC_400_REGION_ID_ACCESS_N_OFFSET(0));
        *p_reg =
            (uint32_t) ((g_bsp_tzc_module_cfg_data[tzc400module].region[0].id_access_rd_en <<
                         BSP_TZC_400_NUMBER_SHIFT_NSAID_RD_EN) |
                        (g_bsp_tzc_module_cfg_data[tzc400module].region[0].id_access_wr_en <<
                         BSP_TZC_400_NUMBER_SHIFT_NSAID_WR_EN));
    }
}

 #endif
#endif
