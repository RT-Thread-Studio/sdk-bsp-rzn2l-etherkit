/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <stdio.h>
#include "bsp_api.h"

#if (BSP_FEATURE_DDR_SUPPORTED)
 #if (1 == BSP_CFG_DDR_INIT_ENABLE)
  #include "bsp_ddr.h"
  #include "bsp_ddr_fw_param.h"

  #if (1 == BSP_CFG_BOARD_RZN2H_EVB)
   #include "../../../../../board/rzn2h_evb/board_ddr_param.h"
  #elif (1 == BSP_CFG_BOARD_CUSTOM)
   #include "../../../../../board/custom/board_ddr_param.h"
  #endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
  #define ARRAY_SIZE(a)    (sizeof(a) / sizeof(a[0]))
  #define BSP_PRV_DDR_PHYINIT_TIMEOUT    (500000U)

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/
static void bsp_prv_ddr_reg_init(void);
static void bsp_prv_ddr_phyinit_c(void);
static void bsp_prv_ddr_phyinit_d2h_1d(void);
static void bsp_prv_ddr_phyinit_d2h_2d(void);
static void bsp_prv_ddr_phyinit_mc(void);
static void bsp_prv_ddr_phyinit_i(void);
static void bsp_prv_ddr_phyinit_j(void);

  #if (1 == BSP_CFG_DDR_ZERO_INITIALIZED_ENABLE)
static void bsp_prv_ddr_prog_all0(uint64_t start_addr, uint32_t addr_space);

  #endif

static int8_t bsp_prv_dwc_ddrphy_cdd_int(uint8_t val);
static int8_t bsp_prv_dwc_ddrphy_cdd_abs(uint8_t val);

static uint32_t bsp_get_mail(uint8_t mode_32bits);
static void     bsp_prv_dwc_ddrphy_phyinit_userCustom_G_waitDone(uint8_t sel_train);
static void     bsp_prv_ddr_setup_mc(void);
static void     bsp_prv_ddr_update_mc(void);

static void bsp_prv_ddr_phyinit_configuration(void);
static void bsp_prv_ddr_phyinit_pin_swizzling(void);
static void bsp_prv_ddr_phyinit_load_1d_image(void);
static void bsp_prv_ddr_phyinit_exec_1d_image(void);
static void bsp_prv_ddr_phyinit_load_2d_image(void);
static void bsp_prv_ddr_phyinit_exec_2d_image(void);
static void bsp_prv_ddr_phyinit_load_eng_image(void);

const uint32_t param_phyinit_c_size       = ARRAY_SIZE(param_phyinit_c);
const uint32_t param_phyinit_i_size       = ARRAY_SIZE(param_phyinit_i);
const uint32_t param_phyinit_f_1d_0_size  = ARRAY_SIZE(param_phyinit_f_1d_0);
const uint32_t param_phyinit_f_2d_0_size  = ARRAY_SIZE(param_phyinit_f_2d_0);
const uint32_t param_phyinit_swizzle_size = ARRAY_SIZE(param_phyinit_swizzle);
const uint32_t param_mcinit_size          = ARRAY_SIZE(param_mcinit);

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_reg_init
 **********************************************************************************************************************/
static void bsp_prv_ddr_reg_init (void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_RST_N);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_RST_PWROKLN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_RESET);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_AXI0_ARESETN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_AXI1_ARESETN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_AXI2_ARESETN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_AXI3_ARESETN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_AXI4_ARESETN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_MC_PRESETN);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_DDRSS_PHY_PRESETN);

    R_BSP_MODULE_START(FSP_IP_DDRSS, 0);

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);
}

/*******************************************************************************************************************//**
 * Executes bsp_ddr_init
 **********************************************************************************************************************/
void bsp_ddr_init (void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_SYSTEM);

    R_BSP_SlaveStopRelease(BSP_BUS_SLAVE_DDRSS_A0_IF);
    R_BSP_SlaveStopRelease(BSP_BUS_SLAVE_DDRSS_A1_IF);
    R_BSP_SlaveStopRelease(BSP_BUS_SLAVE_DDRSS_R2_IF);
    R_BSP_SlaveStopRelease(BSP_BUS_SLAVE_DDRSS_R3_IF);
    R_BSP_SlaveStopRelease(BSP_BUS_SLAVE_DDRSS_A4_IF);
    R_SSC->SSTPCR4_b.DDRAPB = 0;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_SYSTEM);

    bsp_prv_ddr_reg_init();

    bsp_prv_ddr_setup_mc();

    bsp_prv_ddr_phyinit_c();
    bsp_prv_ddr_phyinit_d2h_1d();
    bsp_prv_ddr_phyinit_d2h_2d();

    bsp_prv_ddr_phyinit_mc();

    bsp_prv_ddr_phyinit_i();
    bsp_prv_ddr_phyinit_j();

  #if (1 == BSP_CFG_DDR_ZERO_INITIALIZED_ENABLE)
    uint32_t val;
    uint32_t addr_space, end_addr17;

    val = R_DDRSS->DDR_MEMC_DENALI_CTL_312_b.CS_MAP;
    if (val == 0x01)
    {
        end_addr17 = R_DDRSS->DDR_MEMC_DENALI_CTL_304_b.CS_VAL_UPPER_0;
    }
    else
    {
        end_addr17 = R_DDRSS->DDR_MEMC_DENALI_CTL_306_b.CS_VAL_UPPER_1;
    }

    if (end_addr17 == 0xdfff)
    {
        addr_space = 33;
    }
    else if (end_addr17 == 0x6fff)
    {
        addr_space = 32;
    }
    else if (end_addr17 == 0x37ff)
    {
        addr_space = 31;
    }
    else if (end_addr17 == 0x1bff)
    {
        addr_space = 30;
    }
    else if (end_addr17 == 0x0dff)
    {
        addr_space = 29;
    }
    else
    {
        addr_space = 33;
    }
    bsp_prv_ddr_prog_all0(BPS_DDR_BASE_ADDR, addr_space);
  #endif

    bsp_prv_ddr_update_mc();
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_c
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_c (void)
{
    bsp_prv_ddr_phyinit_configuration();
    bsp_prv_ddr_phyinit_pin_swizzling();
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_d2h_1d
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_d2h_1d (void)
{
    bsp_prv_ddr_phyinit_load_1d_image();
    bsp_prv_ddr_phyinit_exec_1d_image();
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_d2h_2d
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_d2h_2d (void)
{
    bsp_prv_ddr_phyinit_load_2d_image();
    bsp_prv_ddr_phyinit_exec_2d_image();
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_i
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_i (void)
{
    bsp_prv_ddr_phyinit_load_eng_image();
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_j
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_j (void)
{
    R_DDRSS->DDR_MEMC_DENALI_CTL_00_b.start = 1;
    FSP_HARDWARE_REGISTER_WAIT((R_DDRSS->DDR_MEMC_DENALI_CTL_335_b.INT_STATUS_INIT & 0x02), 0x02);
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_mc
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_mc (void)
{
    uint32_t val, num_rank, num_byte, tctrl_delay, bl, x, tx_dqs_dly;

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);

    val      = R_DDRSS->DDR_MEMC_DENALI_CTL_312_b.CS_MAP;
    num_rank = (val == 3) ? 2 : 1;

    val = R_DDRSS->DDR_MEMC_DENALI_CTL_313_b.MEM_DP_REDUCTION;

    num_byte    = (val == 1) ? 2 : 4;
    val         = bsp_dwc_ddrphy_apb_rd(0x02002e);
    tctrl_delay = ((val >> 1) + (val & 1)) + 8;

    val = R_DDRSS->DDR_MEMC_DENALI_CTL_64_b.BSTLEN;
    bl  = (1 << val);

    x = 0;
    if (num_byte > 0)
    {
        val = bsp_dwc_ddrphy_apb_rd(0x010020);
        x   = (val > x) ? val : x;
    }

    if (num_byte > 1)
    {
        val = bsp_dwc_ddrphy_apb_rd(0x011020);
        x   = (val > x) ? val : x;
    }

    if (num_byte > 2)
    {
        val = bsp_dwc_ddrphy_apb_rd(0x012020); x = (val > x) ? val : x;
    }

    if (num_byte > 3)
    {
        val = bsp_dwc_ddrphy_apb_rd(0x013020); x = (val > x) ? val : x;
    }

    val = 16 + tctrl_delay + (2 * x);
    R_DDRSS->DDR_MEMC_DENALI_CTL_732_b.TDFI_PHY_RDLAT_F0 = (val & 0x7F);

    x = 0;
    if ((num_rank > 0) && (num_byte > 0))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0100d0);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0101d0);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 0) && (num_byte > 1))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0110d0);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0111d0);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 0) && (num_byte > 2))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0120d0);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0121d0);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 0) && (num_byte > 3))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0130d0);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0131d0);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 1) && (num_byte > 0))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0100d1);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0101d1);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 1) && (num_byte > 1))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0110d1);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0111d1);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 1) && (num_byte > 2))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0120d1);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0121d1);
        x   = (val > x) ? val : x;
    }

    if ((num_rank > 1) && (num_byte > 3))
    {
        val = bsp_dwc_ddrphy_apb_rd(0x0130d1);
        x   = (val > x) ? val : x;
        val = bsp_dwc_ddrphy_apb_rd(0x0131d1);
        x   = (val > x) ? val : x;
    }

    tx_dqs_dly = ((x >> 6) & 0xf) + (((x >> 4) & 0x01) + ((x >> 3) & 0x1));
    val        = tctrl_delay + (6 + (bl / 2)) + tx_dqs_dly;
    R_DDRSS->DDR_MEMC_DENALI_CTL_761_b.TDFI_WRDATA = (val & 0xff);

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);
}

  #if (1 == BSP_CFG_DDR_ZERO_INITIALIZED_ENABLE)

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_prog_all0
 *
 * @param[in] start_addr    First argument
 * @param[in] addr_space    Second argument
 **********************************************************************************************************************/
static void bsp_prv_ddr_prog_all0 (uint64_t start_addr, uint32_t addr_space)
{
    uint32_t bak_lp_auto_entry_en;

    R_DDRSS->DDR_MEMC_DENALI_CTL_232_b.ECC_DISABLE_W_UC_ERR = 1;

    bak_lp_auto_entry_en = R_DDRSS->DDR_MEMC_DENALI_CTL_160_b.LP_AUTO_ENTRY_EN;

    R_DDRSS->DDR_MEMC_DENALI_CTL_160_b.LP_AUTO_ENTRY_EN = 0x0;

    R_DDRSS->DDR_MEMC_DENALI_CTL_221_b.BIST_START_ADDRESS   = (start_addr & 0xffffffff);
    R_DDRSS->DDR_MEMC_DENALI_CTL_222_b.BIST_START_ADDRESS32 = ((start_addr >> 32) & 0x00000001);
    R_DDRSS->DDR_MEMC_DENALI_CTL_219_b.ADDR_SPACE           = (addr_space & 0x3F);
    R_DDRSS->DDR_MEMC_DENALI_CTL_220_b.BIST_DATA_CHECK      = 1;
    R_DDRSS->DDR_MEMC_DENALI_CTL_225_b.BIST_TEST_MODE       = 0b100;
    R_DDRSS->DDR_MEMC_DENALI_CTL_226_b.BIST_DATA_PATTERN0   = 0x00000000;
    R_DDRSS->DDR_MEMC_DENALI_CTL_227_b.BIST_DATA_PATTERN1   = 0x00000000;
    R_DDRSS->DDR_MEMC_DENALI_CTL_228_b.BIST_DATA_PATTERN2   = 0x00000000;
    R_DDRSS->DDR_MEMC_DENALI_CTL_229_b.BIST_DATA_PATTERN3   = 0x00000000;

    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);

    R_DDRSS->DDR_MEMC_DENALI_CTL_219_b.BIST_GO = 1;
    FSP_HARDWARE_REGISTER_WAIT((R_DDRSS->DDR_MEMC_DENALI_CTL_334_b.INT_STATUS_BIST & 0x01), 0x01);

    R_DDRSS->DDR_MEMC_DENALI_CTL_219_b.BIST_GO      = 0;
    R_DDRSS->DDR_MEMC_DENALI_CTL_343_b.INT_ACK_BIST = 1;
    R_DDRSS->DDR_MEMC_DENALI_CTL_338_b.INT_ACK_ECC  = 0x000001CF;

    FSP_HARDWARE_REGISTER_WAIT(R_DDRSS->DDR_MEMC_DENALI_CTL_334_b.INT_STATUS_BIST, 0);
    FSP_HARDWARE_REGISTER_WAIT(R_DDRSS->DDR_MEMC_DENALI_CTL_329_b.INT_STATUS_ECC, 0);

    R_DDRSS->DDR_MEMC_DENALI_CTL_160_b.LP_AUTO_ENTRY_EN     = (bak_lp_auto_entry_en & 0xf);
    R_DDRSS->DDR_MEMC_DENALI_CTL_232_b.ECC_DISABLE_W_UC_ERR = 0;

    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
}

  #endif

/*******************************************************************************************************************//**
 * Executes bsp_prv_dwc_ddrphy_phyinit_userCustom_G_waitDone
 *
 * @param[in] sel_train     First argument
 **********************************************************************************************************************/
void bsp_prv_dwc_ddrphy_phyinit_userCustom_G_waitDone (uint8_t sel_train)
{
    volatile uint32_t train_done = 0;
    volatile uint32_t mail       = 0;
    volatile uint32_t data       = 0;
    volatile uint32_t timeout    = BSP_PRV_DDR_PHYINIT_TIMEOUT;

    while (train_done == 0)
    {
        FSP_ASSERT_NOT_RETURN_VALUE(0 != timeout);

        data = bsp_dwc_ddrphy_apb_rd(0x0d0004);

        if ((data & 0x1) == 0)
        {
            mail = bsp_get_mail(0);
            if ((mail == 0xff) || (mail == 0x07))
            {
                train_done = 1;
            }
        }

        timeout--;
    }

    FSP_PARAMETER_NOT_USED(sel_train);
}

/*******************************************************************************************************************//**
 * Executes bsp_get_mail
 *
 * @param[in] mode_32bits   First argument
 *
 * @retval                  Returns the value of uint32_t
 **********************************************************************************************************************/
uint32_t bsp_get_mail (uint8_t mode_32bits)
{
    uint32_t mail = 0;

    FSP_HARDWARE_REGISTER_WAIT((bsp_dwc_ddrphy_apb_rd(0x0d0004) & 0x1), 0);

    mail = bsp_dwc_ddrphy_apb_rd(0x0d0032);

    if (mode_32bits != 0)
    {
        mail = (bsp_dwc_ddrphy_apb_rd(0x0d0034) << 16) | mail;
    }

    bsp_dwc_ddrphy_apb_wr(0x0d0031, 0x0000);

    FSP_HARDWARE_REGISTER_WAIT((bsp_dwc_ddrphy_apb_rd(0x0d0004) & 0x1), 1);

    bsp_dwc_ddrphy_apb_wr(0x0d0031, 0x0001);

    return mail;
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_dwc_ddrphy_cdd_int
 *
 * @param[in] val           First argument
 *
 * @return                  Returns the value of uint8_t
 **********************************************************************************************************************/
static int8_t bsp_prv_dwc_ddrphy_cdd_int (uint8_t val)
{
    return (int8_t) ((((val >> 7) & 0x1) == 1) ? -((0x7f ^ ((val >> 0) & 0x7f)) + 1) : ((val >> 0) & 0x7f));
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_dwc_ddrphy_cdd_abs
 *
 * @param[in] val           First argument
 *
 * @return                  Returns the value of uint8_t
 **********************************************************************************************************************/
static int8_t bsp_prv_dwc_ddrphy_cdd_abs (uint8_t val)
{
    return (uint8_t) ((((val >> 7) & 0x1) == 1) ? ((0x7f ^ ((val >> 0) & 0x7f)) + 1) : ((val >> 0) & 0x7f));
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_setup_mc
 **********************************************************************************************************************/
void bsp_prv_ddr_setup_mc (void)
{
    uint32_t i;

    for (i = 0; i < param_mcinit_size; i++)
    {
        bsp_ddrtop_mc_apb_wr(param_mcinit[i][0], param_mcinit[i][1]);
    }
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_configuration
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_configuration (void)
{
    uint32_t i;

    for (i = 0; i < param_phyinit_c_size; i++)
    {
        bsp_dwc_ddrphy_apb_wr(param_phyinit_c[i][0], param_phyinit_c[i][1]);
    }
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_pin_swizzling
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_pin_swizzling (void)
{
    uint32_t i;

    for (i = 0; i < param_phyinit_swizzle_size; i++)
    {
        bsp_dwc_ddrphy_apb_wr(param_phyinit_swizzle[i][0], param_phyinit_swizzle[i][1]);
    }
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_load_1d_image
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_load_1d_image (void)
{
    uint32_t i;

    bsp_dwc_ddrphy_apb_wr(0x020060, 0x2);
    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);

    for (i = 0x0; i <= 0x3fff; i++)
    {
        bsp_dwc_ddrphy_apb_wr((uint32_t) (0x50000 + i), (uint32_t) (phyinit_fw_1d[i]));
    }

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);

    for (i = 0; i < param_phyinit_f_1d_0_size; i++)
    {
        bsp_dwc_ddrphy_apb_wr(param_phyinit_f_1d_0[i][0], param_phyinit_f_1d_0[i][1]);
    }
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_exec_1d_image
 **********************************************************************************************************************/
static void bsp_prv_ddr_phyinit_exec_1d_image (void)
{
    uint8_t  sel_train;
    uint32_t val;

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);
    val       = bsp_dwc_ddrphy_apb_rd(0x01005f);
    sel_train = ((val & 0x700) == 0x100) ? 0 : 2;
    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x9);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x1);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x0);
    bsp_prv_dwc_ddrphy_phyinit_userCustom_G_waitDone(sel_train);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x1);

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);

    uint32_t num_rank;
    val      = R_DDRSS->DDR_MEMC_DENALI_CTL_312_b.CS_MAP;
    num_rank = (val == 3) ? 2 : 1;

    int8_t val0;
    int8_t val1;
    int8_t cdd_rr;
    int8_t cdd_rw_abs;
    int8_t cdd_ww;
    int8_t cdd_ww_abs;

    if (num_rank > 1)
    {
        val    = bsp_dwc_ddrphy_apb_rd(0x54013);
        val0   = bsp_prv_dwc_ddrphy_cdd_int((val >> 0) & 0xff);
        val1   = bsp_prv_dwc_ddrphy_cdd_int((val >> 8) & 0xff);
        cdd_rr = val0;
        cdd_rr = (val1 > cdd_rr) ? val1 : cdd_rr;

        val        = bsp_dwc_ddrphy_apb_rd(0x54014);
        val0       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 0) & 0xff);
        val1       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 8) & 0xff);
        cdd_rw_abs = val0;
        cdd_rw_abs = (val1 > cdd_rw_abs) ? val1 : cdd_rw_abs;

        val        = bsp_dwc_ddrphy_apb_rd(0x54015);
        val0       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 0) & 0xff);
        val1       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 8) & 0xff);
        cdd_rw_abs = (val0 > cdd_rw_abs) ? val0 : cdd_rw_abs;
        cdd_rw_abs = (val1 > cdd_rw_abs) ? val1 : cdd_rw_abs;

        val        = bsp_dwc_ddrphy_apb_rd(0x54018);
        val0       = bsp_prv_dwc_ddrphy_cdd_int((val >> 0) & 0xff);
        val1       = bsp_prv_dwc_ddrphy_cdd_int((val >> 8) & 0xff);
        cdd_ww     = val0;
        cdd_ww     = (val1 > cdd_ww) ? val1 : cdd_ww;
        val0       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 0) & 0xff);
        val1       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 8) & 0xff);
        cdd_ww_abs = val0;
        cdd_ww_abs = (val1 > cdd_ww_abs) ? val1 : cdd_ww_abs;

        val    = bsp_dwc_ddrphy_apb_rd(0x5402c);
        val1   = bsp_prv_dwc_ddrphy_cdd_int((val >> 8) & 0xff);
        cdd_rr = (val1 > cdd_rr) ? val1 : cdd_rr;

        val        = bsp_dwc_ddrphy_apb_rd(0x5402d);
        val0       = bsp_prv_dwc_ddrphy_cdd_int((val >> 0) & 0xff);
        cdd_rr     = (val0 > cdd_rr) ? val0 : cdd_rr;
        val1       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 8) & 0xff);
        cdd_rw_abs = (val1 > cdd_rw_abs) ? val1 : cdd_rw_abs;

        val        = bsp_dwc_ddrphy_apb_rd(0x5402e);
        val0       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 0) & 0xff);
        val1       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 8) & 0xff);
        cdd_rw_abs = (val0 > cdd_rw_abs) ? val0 : cdd_rw_abs;
        cdd_rw_abs = (val1 > cdd_rw_abs) ? val1 : cdd_rw_abs;

        val        = bsp_dwc_ddrphy_apb_rd(0x5402f);
        val0       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 0) & 0xff);
        cdd_rw_abs = (val0 > cdd_rw_abs) ? val0 : cdd_rw_abs;

        val        = bsp_dwc_ddrphy_apb_rd(0x54031);
        val1       = bsp_prv_dwc_ddrphy_cdd_int((val >> 8) & 0xff);
        cdd_ww     = (val1 > cdd_ww) ? val1 : cdd_ww;
        val1       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 8) & 0xff);
        cdd_ww_abs = (val1 > cdd_ww_abs) ? val1 : cdd_ww_abs;

        val        = bsp_dwc_ddrphy_apb_rd(0x54032);
        val0       = bsp_prv_dwc_ddrphy_cdd_int((val >> 0) & 0xff);
        cdd_ww     = (val0 > cdd_ww) ? val0 : cdd_ww;
        val0       = bsp_prv_dwc_ddrphy_cdd_abs((val >> 0) & 0xff);
        cdd_ww_abs = (val0 > cdd_ww_abs) ? val0 : cdd_ww_abs;

        if (cdd_rr > 0)
        {
            val  = R_DDRSS->DDR_MEMC_DENALI_CTL_376_b.R2R_DIFFCS_DLY_F0;
            val += (uint32_t) ((uint8_t) cdd_rr);
            R_DDRSS->DDR_MEMC_DENALI_CTL_376_b.R2R_DIFFCS_DLY_F0 = (val & 0x1F);
        }

        if (cdd_ww > 0)
        {
            val  = R_DDRSS->DDR_MEMC_DENALI_CTL_377_b.W2W_DIFFCS_DLY_F0;
            val += (uint32_t) ((uint8_t) cdd_ww);
            R_DDRSS->DDR_MEMC_DENALI_CTL_377_b.W2W_DIFFCS_DLY_F0 = (val & 0x1F);
        }

        val  = R_DDRSS->DDR_MEMC_DENALI_CTL_376_b.R2W_DIFFCS_DLY_F0;
        val += (uint32_t) ((uint8_t) cdd_rw_abs);
        R_DDRSS->DDR_MEMC_DENALI_CTL_376_b.R2W_DIFFCS_DLY_F0 = (val & 0x1F);

        val  = R_DDRSS->DDR_MEMC_DENALI_CTL_377_b.W2R_DIFFCS_DLY_F0;
        val += (uint32_t) ((uint8_t) cdd_ww_abs);
        R_DDRSS->DDR_MEMC_DENALI_CTL_377_b.W2R_DIFFCS_DLY_F0 = (val & 0x1F);
    }

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_load_2d_image
 **********************************************************************************************************************/
void bsp_prv_ddr_phyinit_load_2d_image (void)
{
    uint32_t i;

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);
    for (i = 0x0; i <= 0x3fff; i++)
    {
        bsp_dwc_ddrphy_apb_wr((uint32_t) (0x50000 + i), (uint32_t) (phyinit_fw_2d[i]));
    }

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);

    for (i = 0; i < param_phyinit_f_2d_0_size; i++)
    {
        bsp_dwc_ddrphy_apb_wr(param_phyinit_f_2d_0[i][0], param_phyinit_f_2d_0[i][1]);
    }
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_exec_2d_image
 **********************************************************************************************************************/
void bsp_prv_ddr_phyinit_exec_2d_image (void)
{
    uint8_t  sel_train;
    uint32_t val;

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);
    val       = bsp_dwc_ddrphy_apb_rd(0x01005f);
    sel_train = ((val & 0x700) == 0x100) ? 1 : 3;
    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);
    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x9);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x1);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x0);
    bsp_prv_dwc_ddrphy_phyinit_userCustom_G_waitDone(sel_train);
    bsp_dwc_ddrphy_apb_wr(0x0d0099, 0x1);

    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x0);
    bsp_dwc_ddrphy_apb_wr(0x0d0000, 0x1);
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_phyinit_load_eng_image
 **********************************************************************************************************************/
void bsp_prv_ddr_phyinit_load_eng_image (void)
{
    uint32_t i;

    for (i = 0; i < param_phyinit_i_size; i++)
    {
        bsp_dwc_ddrphy_apb_wr(param_phyinit_i[i][0], param_phyinit_i[i][1]);
    }
}

/*******************************************************************************************************************//**
 * Executes bsp_prv_ddr_update_mc
 **********************************************************************************************************************/
void bsp_prv_ddr_update_mc (void)
{
    uint32_t val;

    R_DDRSS->DDR_MEMC_DENALI_CTL_327_b.INT_MASK_MASTER = 0;

    val = R_DDRSS->DDR_MEMC_DENALI_CTL_167_b.PCPCS_PD_EN;
    if (val == 1)
    {
        R_DDRSS->DDR_MEMC_DENALI_CTL_158_b.LPI_WAKEUP_EN = 0x1E;
    }
    else
    {
        R_DDRSS->DDR_MEMC_DENALI_CTL_158_b.LPI_WAKEUP_EN = 0x1F;
    }
}

 #endif
#endif
