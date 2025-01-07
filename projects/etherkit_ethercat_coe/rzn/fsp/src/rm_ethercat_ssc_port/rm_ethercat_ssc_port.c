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

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <string.h>
#include "bsp_api.h"
#include "r_ioport.h"
#include "renesashw.h"
#include "rm_ethercat_ssc_port.h"

extern ethercat_ssc_port_instance_t const * gp_ethercat_ssc_port;

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/** "ECAT" in ASCII, used to determine if channel is open. */
#define ETHERCAT_SSC_PORT_OPEN                     (('E' << 24U) | ('C' << 16U) | ('A' << 8U) | ('T' << 0U))

/** Detect type of the interrupt  */
#define ETHERCAT_SSC_PORT_IRQ_TYPE_LEVEL           (0U)     /* active-HIGH level */
#define ETHERCAT_SSC_PORT_IRQ_TYPE_POSEDGE         (1U)     /* rising edge-triggerd */

/* ESC EEPROM status */
#define ETHERCAT_SSC_PORT_EEPROM_MASK_STATE        (0x2800) /* error check mask */
#define ETHERCAT_SSC_PORT_EEPROM_ERROR_BLANK       (0x0800) /* active-HIGH level */
#define ETHERCAT_SSC_PORT_EEPROM_ERROR_I2CBUS      (0x2800) /* rising edge-triggerd */

/* ESC_RESETOUT# PFC setting */
#define ETHERCAT_SSC_PORT_PFC_ESC_RESETOUT         (0x01U << IOPORT_PFC_OFFSET)

/* Key code for PRCMD register */
#define ETHERCAT_SSC_PORT_PRCMD_UNLOCK1            (0x000000A5U)
#define ETHERCAT_SSC_PORT_PRCMD_UNLOCK2            (0x00000001U)
#define ETHERCAT_SSC_PORT_PRCMD_UNLOCK3            (0x0000FFFEU)
#define ETHERCAT_SSC_PORT_PRCMD_UNLOCK4            (0x00000001U)
#define ETHERCAT_SSC_PORT_PRCMD_LOCK               (0x00000000U)

/* Initial value of down counter for timeout detection */
#define ETHERCAT_SSC_PORT_EEPROMLOADING_TIMEOUT    (1000000000)

/* Exceptionally used to support GMAC MDIO for RSK board Rev1.0 or Rev1.0.1 */
/* Set to 1 to support GMAC MDIO */
#ifndef ETHERCAT_SSC_PORT_GMAC_MDIO_SUPPORT
 #define ETHERCAT_SSC_PORT_GMAC_MDIO_SUPPORT       (0U)
#endif

/* Set to 1 to support ETHSW MDIO */
#ifndef ETHERCAT_SSC_PORT_ETHSW_MDIO_SUPPORT
 #define ETHERCAT_SSC_PORT_ETHSW_MDIO_SUPPORT      (0U)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 ***********************************************************************************************************************/
MEM_ADDR ESCMEM * pEsc;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if (RM_ETHERCAT_SSC_PORT_CFG_PARAM_CHECKING_ENABLE)
static fsp_err_t ethercat_ssc_port_open_param_check(ethercat_ssc_port_instance_ctrl_t const * const p_instance_ctrl,
                                                    ethercat_ssc_port_cfg_t const * const           p_cfg);

#endif

static void ethercat_ssc_port_enable_icu(ethercat_ssc_port_instance_ctrl_t * const p_instance_ctrl);
static void ethercat_ssc_port_disable_icu(ethercat_ssc_port_instance_ctrl_t * const p_instance_ctrl);

static void ethercat_ssc_port_reg_protection_disable(void * const p_reg_base);
static void ethercat_ssc_port_reg_protection_enable(void * const p_reg_base);

static fsp_err_t ethercat_ssc_port_loading_eeprom(void * const p_reg_base);

void ethercat_ssc_port_isr_esc_cat(void);
void ethercat_ssc_port_isr_esc_sync0(void);
void ethercat_ssc_port_isr_esc_sync1(void);

__attribute__((weak)) void PDI_Isr(void);

__attribute__((weak)) void Sync0_Isr(void);

__attribute__((weak)) void Sync1_Isr(void);

__attribute__((weak)) void rm_ssc_port_timer_interrupt(timer_callback_args_t * p_args);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
__attribute__((weak)) const ioport_instance_t g_ioport;

/** ETHERCAT SSC PORT API mapping for ETHECAT SSC PORT interface */
const ethercat_ssc_port_api_t g_ethercat_ssc_port_on_ethercat_ssc_port =
{
    .open  = RM_ETHERCAT_SSC_PORT_Open,
    .close = RM_ETHERCAT_SSC_PORT_Close,
};

static UINT16 BaseTime = 0;

/*******************************************************************************************************************//**
 * @addtogroup RM_ETHERCAT_SSC_PORT
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/********************************************************************************************************************//**
 * @brief EtherCAT Slave Controller is initialized with this function.
 * This function includes PHY initialization and ESC EEPROM loading.
 * Afterwards, EtherCAT communication begins.
 * Also EtherCAT interrupts are permitted if the interrupts are used.
 * In order to receive the EtherCAT, EtherCAT Sync0, EtherCAT Sync1 interrupt event,
 * it's necessary to register a callback function.
 * Implements @ref ethercat_ssc_port_api_t::open.
 *
 * @retval  FSP_SUCCESS                             Opened successfully
 * @retval  FSP_ERR_ASSERTION                       A required input pointer is NULL
 * @retval  FSP_ERR_ALREADY_OPEN                    Control block has already been opened
 * @retval  FSP_ERR_INVALID_ARGUMENT                Irq number lower then 0.
 * @retval  FSP_ERR_INVALID_HW_CONDITION            ESC EEPROM loading error
 * @retval  FSP_ERR_TIMEOUT                         ESC EEPROM loading timeout error
 ***********************************************************************************************************************/
fsp_err_t RM_ETHERCAT_SSC_PORT_Open (ethercat_ssc_port_ctrl_t * const      p_ctrl,
                                     ethercat_ssc_port_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl = (ethercat_ssc_port_instance_ctrl_t *) p_ctrl;
    ethercat_ssc_port_extend_cfg_t    * p_extend;

    R_ESC_INI_Type * p_reg_ini   = (R_ESC_INI_Type *) R_ESC_INI_BASE;
    R_ESC_Type     * p_reg       = (R_ESC_Type *) R_ESC_BASE;
    R_ETHSS_Type   * p_reg_ethss = R_ETHSS;

    uint32_t               i;
    uint32_t               ecatdbgc;
    uint8_t                opened_phy[BSP_FEATURE_ESC_MAX_PORTS] = {0};
    uint8_t                opened_timer = 0;
    ether_phy_instance_t * p_ether_phy_instance;
    timer_instance_t     * p_timer_instance;
    bsp_io_region_t        reset_port_region;

#if (RM_ETHERCAT_SSC_PORT_CFG_PARAM_CHECKING_ENABLE)

    /** Check parameters. */
    err = ethercat_ssc_port_open_param_check(p_instance_ctrl, p_cfg); /** check arguments */
    FSP_ERROR_RETURN((FSP_SUCCESS == err), err);
#endif

    /** Make sure this channel exists. */
    p_instance_ctrl->p_reg_ini = (void *) p_reg_ini;
    p_instance_ctrl->p_reg     = (void *) p_reg;

    /* Initialize configuration of Ethernet module. */
    p_instance_ctrl->p_cfg = p_cfg;
    p_extend               = (ethercat_ssc_port_extend_cfg_t *) p_cfg->p_extend;

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);

    /* Powe on ESC  */
    R_BSP_MODULE_START(FSP_IP_ESC, 0);

    /* Power on ETHSS */
    R_BSP_MODULE_START(FSP_IP_ETHSS, 0);

    /* Release ETHSS module */
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_ESC_ETH_SUBSYSTEM);

#if (ETHERCAT_SSC_PORT_GMAC_MDIO_SUPPORT == 1)

    /* Power on GMAC for MDIO */
    R_BSP_MODULE_START(FSP_IP_GMAC, 0);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_GMAC0_PCLKH);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_GMAC0_PCLKM);
#endif

#if (ETHERCAT_SSC_PORT_ETHSW_MDIO_SUPPORT == 1)

    /* Power on ETHSW for MDIO */
    R_BSP_MODULE_START(FSP_IP_ETHSW, 0);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_ETHSW);
#endif
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Unlock write access protection for Ethernet subsystem registers */
    ethercat_ssc_port_reg_protection_disable(p_reg_ethss);

    /* Set PHY address offset */
    p_reg_ini->ECATOFFADR = p_instance_ctrl->p_cfg->address_offset;

    /* Set EEPROM size */
    p_reg_ini->ECATOPMOD = (uint32_t) p_extend->eeprom_size;

    /* Set TXC delay */
    ecatdbgc            = (uint32_t) ((p_extend->txc2 << 4) | (p_extend->txc1 << 2) | (p_extend->txc0 << 0));
    p_reg_ini->ECATDBGC = ecatdbgc;

    /* Lock write access protection for Ethernet subsystem registers */
    ethercat_ssc_port_reg_protection_enable(p_reg_ethss);

    /* ESC Reset */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_RESET);
    R_BSP_ModuleResetEnable(BSP_MODULE_RESET_ESC_BUS);
    R_BSP_ModuleResetEnable(BSP_MODULE_RESET_ESC_IP);

    /* Write Low-output to ESC_RESETOUT# as PHY reset */
    R_BSP_PinAccessEnable();
    reset_port_region = R_BSP_IoRegionGet((bsp_io_port_pin_t) ETHERCAT_SSC_PORT_CFG_ESC_RESET_PORT);
    R_BSP_PinClear(reset_port_region, (bsp_io_port_pin_t) ETHERCAT_SSC_PORT_CFG_ESC_RESET_PORT);

    /* Reset hold time */
    R_BSP_SoftwareDelay(p_instance_ctrl->p_cfg->reset_hold_time, BSP_DELAY_UNITS_MILLISECONDS);

    /* ESC Reset release*/
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_ESC_BUS);
    R_BSP_ModuleResetDisable(BSP_MODULE_RESET_ESC_IP);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_RESET);

    /* Write High-output to ESC_RESETOUT# as release */
    R_BSP_PinSet(reset_port_region, (bsp_io_port_pin_t) ETHERCAT_SSC_PORT_CFG_ESC_RESET_PORT);
    R_BSP_PinAccessDisable();

    /* Re-assign P20_7 to ESC_RESETOUT# */
    R_IOPORT_PinCfg(g_ioport.p_ctrl, ETHERCAT_SSC_PORT_CFG_ESC_RESET_PORT,
                    ((uint32_t) IOPORT_CFG_PORT_PERI | (uint32_t) ETHERCAT_SSC_PORT_PFC_ESC_RESETOUT));

    /* Wait Loading ESC EEPROM */
    err = ethercat_ssc_port_loading_eeprom(p_reg);
    FSP_ERROR_RETURN((FSP_SUCCESS == err), err);

    /* Wait after reset */
    R_BSP_SoftwareDelay(p_instance_ctrl->p_cfg->reset_wait_time, BSP_DELAY_UNITS_MICROSECONDS);

    /* Open Ether-Phy Driver */
    for (i = 0; BSP_FEATURE_ESC_MAX_PORTS > i; i++)
    {
        p_ether_phy_instance = (ether_phy_instance_t *) p_extend->p_ether_phy_instance[i];
        if (NULL != p_ether_phy_instance)
        {
            err = p_ether_phy_instance->p_api->open(p_ether_phy_instance->p_ctrl, p_ether_phy_instance->p_cfg);
        }

        if (FSP_SUCCESS == err)
        {
            opened_phy[i] = 1;
        }
        else
        {
            break;
        }
    }

    /* Open Timer driver */
    if (FSP_SUCCESS == err)
    {
        p_timer_instance = (timer_instance_t *) p_cfg->p_timer_instance;
        err              = p_timer_instance->p_api->open(p_timer_instance->p_ctrl, p_timer_instance->p_cfg);
        if (FSP_SUCCESS == err)
        {
            opened_timer = 1;
            err          = p_timer_instance->p_api->start(p_timer_instance->p_ctrl);
        }
    }

    if (FSP_SUCCESS == err)
    {
        /* Initialize the EtherCAT Slave Interface of the SSC */
        HW_Init();

        /** Enable EtherCAT interrupts */
        ethercat_ssc_port_enable_icu(p_instance_ctrl);
        p_instance_ctrl->open = ETHERCAT_SSC_PORT_OPEN;
    }
    else
    {
        /* Close operation
         * Close Timer driver */
        if (0 != opened_timer)
        {
            p_timer_instance = (timer_instance_t *) p_cfg->p_timer_instance;
            p_timer_instance->p_api->stop(p_timer_instance->p_ctrl);
            p_timer_instance->p_api->close(p_timer_instance->p_ctrl);
        }

        /* Close PHY driver */
        for (i = 0; BSP_FEATURE_ESC_MAX_PORTS > i; i++)
        {
            if (0 != opened_phy[i])
            {
                p_ether_phy_instance = (ether_phy_instance_t *) p_extend->p_ether_phy_instance[i];
                if (NULL != p_ether_phy_instance)
                {
                    p_ether_phy_instance->p_api->close(p_ether_phy_instance->p_ctrl);
                }
            }
        }
    }

    return err;
}                                      /* End of function RM_ETHERCAT_SSC_PORT_Open() */

/********************************************************************************************************************//**
 * Disables interrupts. Removes power and releases hardware lock. Implements @ref ethercat_ssc_port_api_t::close.
 *
 * @retval  FSP_SUCCESS                                 Channel successfully closed.
 * @retval  FSP_ERR_ASSERTION                           Pointer to the control block is NULL.
 * @retval  FSP_ERR_NOT_OPEN                            The control block has not been opened
 *
 ***********************************************************************************************************************/
fsp_err_t RM_ETHERCAT_SSC_PORT_Close (ethercat_ssc_port_ctrl_t * const p_ctrl)
{
    fsp_err_t                           err = FSP_SUCCESS;
    uint32_t                            i;
    ether_phy_instance_t              * p_ether_phy_instance;
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl = (ethercat_ssc_port_instance_ctrl_t *) p_ctrl;
    timer_instance_t                  * p_timer_instance;

#if (RM_ETHERCAT_SSC_PORT_CFG_PARAM_CHECKING_ENABLE)
    FSP_ASSERT(p_instance_ctrl);
    FSP_ERROR_RETURN(ETHERCAT_SSC_PORT_OPEN == p_instance_ctrl->open, FSP_ERR_NOT_OPEN);
#endif

    ethercat_ssc_port_cfg_t        * p_cfg    = (ethercat_ssc_port_cfg_t *) p_instance_ctrl->p_cfg;
    ethercat_ssc_port_extend_cfg_t * p_extend = (ethercat_ssc_port_extend_cfg_t *) p_cfg->p_extend;

    /* Disable EtherCAT interrupt. */
    ethercat_ssc_port_disable_icu(p_instance_ctrl);

    /* Release the EtherCAT Slave Interface of the SSC */
    HW_Release();

    /* Close Timer driver */
    p_timer_instance = (timer_instance_t *) p_cfg->p_timer_instance;
    p_timer_instance->p_api->stop(p_timer_instance->p_ctrl);
    p_timer_instance->p_api->close(p_timer_instance->p_ctrl);

    /* Close PHY driver */
    for (i = 0; BSP_FEATURE_ESC_MAX_PORTS > i; i++)
    {
        p_ether_phy_instance = (ether_phy_instance_t *) p_extend->p_ether_phy_instance[i];
        if (NULL != p_ether_phy_instance)
        {
            p_ether_phy_instance->p_api->close(p_ether_phy_instance->p_ctrl);
        }
    }

    /** Clear configure block parameters. */
    p_instance_ctrl->p_cfg = NULL;

    /** Mark the channel not open so other APIs cannot use it. */
    p_instance_ctrl->open = 0U;

    return err;
}                                      /* End of function RM_ETHERCAT_SSC_PORT_Close() */

/*******************************************************************************************************************//**
 * @} (end addtogroup ETHERCAT_SSC_PORT)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/

#if (RM_ETHERCAT_SSC_PORT_CFG_PARAM_CHECKING_ENABLE)

/*******************************************************************************************************************//**
 * @brief Parameter error check function for open.
 *
 * @param[in] p_instance_ctrl   Pointer to the control block
 * @param[in] p_cfg             Pointer to the configuration structure
 *
 * @retval  FSP_SUCCESS                  No parameter error found
 * @retval  FSP_ERR_ASSERTION            A required input pointer is NULL
 * @retval  FSP_ERR_ALREADY_OPEN         Control block has already been opened
 * @retval  FSP_ERR_INVALID_ARGUMENT     Irq number lower then 0.
 **********************************************************************************************************************/
static fsp_err_t ethercat_ssc_port_open_param_check (ethercat_ssc_port_instance_ctrl_t const * const p_instance_ctrl,
                                                     ethercat_ssc_port_cfg_t const * const           p_cfg)
{
    FSP_ASSERT(NULL != p_instance_ctrl);
    FSP_ASSERT(NULL != p_cfg);
    FSP_ASSERT(NULL != p_cfg->p_extend);

    /* Check timer driver instance */
    FSP_ASSERT(NULL != p_cfg->p_timer_instance);

    /* Check IRQ number */
    FSP_ERROR_RETURN((0 <= p_cfg->common_irq), FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN((0 <= p_cfg->sync0_irq), FSP_ERR_INVALID_ARGUMENT);
    FSP_ERROR_RETURN((0 <= p_cfg->sync1_irq), FSP_ERR_INVALID_ARGUMENT);

    FSP_ERROR_RETURN((ETHERCAT_SSC_PORT_OPEN != p_instance_ctrl->open), FSP_ERR_ALREADY_OPEN);

    return FSP_SUCCESS;
}

#endif

/*******************************************************************************************************************//**
 * Function Name: ethercat_ssc_port_isr_esc_cat
 * Description  : Interrupt handler for EtherCAT interrupts.
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void ethercat_ssc_port_isr_esc_cat (void)
{
    ethercat_ssc_port_callback_args_t callback_arg;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl =
        (ethercat_ssc_port_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* SSC ESC CAT Interrupt handler */
    PDI_Isr();

    /* Callback : Interrupt handler */
    if (NULL != p_instance_ctrl->p_cfg->p_callback)
    {
        callback_arg.event     = ETHERCAT_SSC_PORT_EVENT_ESC_CAT_INTERRUPT;
        callback_arg.p_context = p_instance_ctrl->p_cfg->p_context;

        (*p_instance_ctrl->p_cfg->p_callback)((void *) &callback_arg);
    }
}                                      /* End of function ethercat_ssc_port_isr_esc_cat() */

/*******************************************************************************************************************//**
 * Function Name: ethercat_ssc_port_isr_esc_sync0
 * Description  : Interrupt handler for EtherCAT Sync0 interrupts.
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void ethercat_ssc_port_isr_esc_sync0 (void)
{
    ethercat_ssc_port_callback_args_t callback_arg;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl =
        (ethercat_ssc_port_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* SSC ESC SYNC0 Interrupt handler */
    Sync0_Isr();

    /* Callback : Interrupt handler */
    if (NULL != p_instance_ctrl->p_cfg->p_callback)
    {
        callback_arg.event     = ETHERCAT_SSC_PORT_EVENT_ESC_SYNC0_INTERRUPT;
        callback_arg.p_context = p_instance_ctrl->p_cfg->p_context;

        (*p_instance_ctrl->p_cfg->p_callback)((void *) &callback_arg);
    }
}                                      /* End of function ethercat_ssc_port_isr_esc_sync0() */

/*******************************************************************************************************************//**
 * Function Name: ethercat_ssc_port_isr_esc_sync1
 * Description  : Interrupt handler for EtherCAT Sync1 interrupts.
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void ethercat_ssc_port_isr_esc_sync1 (void)
{
    ethercat_ssc_port_callback_args_t callback_arg;

    IRQn_Type irq = R_FSP_CurrentIrqGet();
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl =
        (ethercat_ssc_port_instance_ctrl_t *) R_FSP_IsrContextGet(irq);

    /* SSC ESC SYNC01 Interrupt handler */
    Sync1_Isr();

    /* Callback : Interrupt handler */
    if (NULL != p_instance_ctrl->p_cfg->p_callback)
    {
        callback_arg.event     = ETHERCAT_SSC_PORT_EVENT_ESC_SYNC1_INTERRUPT;
        callback_arg.p_context = p_instance_ctrl->p_cfg->p_context;

        (*p_instance_ctrl->p_cfg->p_callback)((void *) &callback_arg);
    }
}                                      /* End of function ethercat_ssc_port_isr_esc_sync1() */

/*******************************************************************************************************************//**
 * Enable ICU
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethercat_ssc_port_enable_icu (ethercat_ssc_port_instance_ctrl_t * const p_instance_ctrl)
{
    /* Set Detect type */
    R_BSP_IrqDetectTypeSet(p_instance_ctrl->p_cfg->common_irq, ETHERCAT_SSC_PORT_IRQ_TYPE_LEVEL);

    /* Configure the EtherCAT interrupt. */
    R_BSP_IrqCfgEnable(p_instance_ctrl->p_cfg->common_irq, p_instance_ctrl->p_cfg->common_ipl, p_instance_ctrl);

    /* Set Detect type */
    R_BSP_IrqDetectTypeSet(p_instance_ctrl->p_cfg->sync0_irq, ETHERCAT_SSC_PORT_IRQ_TYPE_POSEDGE);

    /* Configure the EtherCAT SYNC0 interrupt. */
    R_BSP_IrqCfgEnable(p_instance_ctrl->p_cfg->sync0_irq, p_instance_ctrl->p_cfg->sync0_ipl, p_instance_ctrl);

    /* Set Detect type */
    R_BSP_IrqDetectTypeSet(p_instance_ctrl->p_cfg->sync1_irq, ETHERCAT_SSC_PORT_IRQ_TYPE_POSEDGE);

    /* Configure the EtherCAT SYNC1 interrupt. */
    R_BSP_IrqCfgEnable(p_instance_ctrl->p_cfg->sync1_irq, p_instance_ctrl->p_cfg->sync1_ipl, p_instance_ctrl);
}                                      /* End of function ethercat_ssc_port_enable_icu() */

/*******************************************************************************************************************//**
 * Disable ICU
 *
 * @param[in] p_instance_ctrl   Pointer to the control block for the channel
 *
 * @retval      none
 **********************************************************************************************************************/
static void ethercat_ssc_port_disable_icu (ethercat_ssc_port_instance_ctrl_t * const p_instance_ctrl)
{
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->common_irq);
    R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->common_irq, NULL);

    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->sync0_irq);
    R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->sync0_irq, NULL);

    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->sync1_irq);
    R_FSP_IsrContextSet(p_instance_ctrl->p_cfg->sync1_irq, NULL);
}                                      /* End of function ethercat_ssc_port_disable_icu() */

/*******************************************************************************************************************//**
 * Unlock write access protection for Ethernet subsystem registers
 *
 **********************************************************************************************************************/
static void ethercat_ssc_port_reg_protection_disable (void * const p_reg_base)
{
    R_ETHSS_Type * p_reg_ethss = (R_ETHSS_Type *) p_reg_base;

    /* Unlock the protection of ethernet subsystem register */
    p_reg_ethss->PRCMD = ETHERCAT_SSC_PORT_PRCMD_UNLOCK1;
    p_reg_ethss->PRCMD = ETHERCAT_SSC_PORT_PRCMD_UNLOCK2;
    p_reg_ethss->PRCMD = ETHERCAT_SSC_PORT_PRCMD_UNLOCK3;
    p_reg_ethss->PRCMD = ETHERCAT_SSC_PORT_PRCMD_UNLOCK4;
}

/*******************************************************************************************************************//**
 * Lock write access protection for Ethernet subsystem registers
 *
 **********************************************************************************************************************/
static void ethercat_ssc_port_reg_protection_enable (void * const p_reg_base)
{
    R_ETHSS_Type * p_reg_ethss = (R_ETHSS_Type *) p_reg_base;

    /* Lock the protection of ethernet access register */
    p_reg_ethss->PRCMD = ETHERCAT_SSC_PORT_PRCMD_LOCK;
}

/*******************************************************************************************************************//**
 * Loding ESC EEPROM
 * EtherCAT communication is possible even if the EEPROM is blank, so no error code is returned.
 *
 * @param[in] p_reg_base   Pointer to the ESC register base address
 *
 * @retval  FSP_SUCCESS                             EEPROM loading successfully or Invalid data in EEPROM including blank
 * @retval  FSP_ERR_INVALID_HW_CONDITION            The error of I2C bus which connectors to EEPROM
 * @retval  FSP_ERR_TIMEOUT                         EEPROM loading timeout error
 **********************************************************************************************************************/
static fsp_err_t ethercat_ssc_port_loading_eeprom (void * const p_reg_base)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t  timeout;
    uint16_t  tmp16;

    R_ESC_Type * p_reg = (R_ESC_Type *) p_reg_base;

    timeout = ETHERCAT_SSC_PORT_EEPROMLOADING_TIMEOUT;

    while (1)
    {
        if (1 == p_reg->ESC_DL_STATUS_b.PDIOPE)
        {
            err = FSP_SUCCESS;
            break;                                          /* Loading successful, PDI operations */
        }

        tmp16 = p_reg->EEP_CONT_STAT & ETHERCAT_SSC_PORT_EEPROM_MASK_STATE;
        if (ETHERCAT_SSC_PORT_EEPROM_ERROR_BLANK == tmp16)  /* ACMDERR,CKSUMERR = 01 */
        {
            err = FSP_SUCCESS;
            break;                                          /* EEPROM is loaded, but it is blank */
        }

        if (ETHERCAT_SSC_PORT_EEPROM_ERROR_I2CBUS == tmp16) /* ACMDERR,CKSUMERR = 11 */
        {
            err = FSP_ERR_INVALID_HW_CONDITION;
            break;                                          /* I2C bus error */
        }

        if (0 == timeout)                                   /* Timeout */
        {
            err = FSP_ERR_TIMEOUT;
            break;                                          /* Timeout error */
        }

        timeout--;
    }

    return err;
}

/*******************************************************************************************************************//**
 * Timer ISR
 *
 * @note    This is a weak function. It can be overridden by an application specific implementation if desired.
 **********************************************************************************************************************/

__attribute__((weak)) void rm_ssc_port_timer_interrupt (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        HW_IncTimer();
    }
}

/*******************************************************************************************************************//**
 * PDI ISR
 *
 * @note    This is a weak function. It can be overriden by the SSC.
 **********************************************************************************************************************/
__attribute__((weak)) void PDI_Isr (void)
{
}

/*******************************************************************************************************************//**
 * Sync0 ISR
 *
 * @note    This is a weak function. It can be overriden by the SSC.
 **********************************************************************************************************************/
__attribute__((weak)) void Sync0_Isr (void)
{
}

/*******************************************************************************************************************//**
 * Sync1 ISR
 *
 * @note    This is a weak function. It can be overriden by the SSC.
 **********************************************************************************************************************/
__attribute__((weak)) void Sync1_Isr (void)
{
}

/*******************************************************************************************************************//**
 * Get the value of the timer.
 *  This function gets the value of the timer.
 *  The timer ticks per ms shall be set in "ECAT_TIMER_INC_P_MS"
 *
 * @retval  Current timer value
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
UINT16 HW_GetTimer ()
{
    return BaseTime;
}

/*******************************************************************************************************************//**
 * Clear the timer value
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
void HW_ClearTimer ()
{
    BaseTime = 0;
}

/*******************************************************************************************************************//**
 * Increment the timer value
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
void HW_IncTimer ()
{
    BaseTime = (UINT16) (BaseTime + ECAT_TIMER_INC_P_MS);
}

/*******************************************************************************************************************//**
 * Initialize the EtherCAT Slave Interface.
 *
 * @retval  0 if initialization was successful
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
UINT16 HW_Init (void)
{
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl = gp_ethercat_ssc_port->p_ctrl;

    /* set esc register base address */
    pEsc = (MEM_ADDR ESCMEM *) p_instance_ctrl->p_reg;

    return 0;
}

/*******************************************************************************************************************//**
 * Release the EtherCAT Slave Interface.
 *  This function is called when EterCAT slave stack is disabled.
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
void HW_Release (void)
{
}

/*******************************************************************************************************************//**
 * RUN and ERR LED control
 *
 * @param[in] RunLed RUN LED control
 * @param[in] ErrLed ERR LED control
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
void HW_SetLed (BOOL RunLed, BOOL ErrLed)
{
    UINT8 errled;

    (void) RunLed;

    if (ErrLed)
    {
        errled = ESC_LED_ON;
    }
    else
    {
        errled = ESC_LED_OFF;
    }

    HW_EscWriteByte(errled, ESC_REG_ERROR_LED_OVERRIDE);
}

/*******************************************************************************************************************//**
 * Disable EtherCAT interrupts
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
void DISABLE_ESC_INT ()
{
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl = gp_ethercat_ssc_port->p_ctrl;

    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->sync0_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->sync1_irq);
    R_BSP_IrqDisable(p_instance_ctrl->p_cfg->common_irq);
}

/*******************************************************************************************************************//**
 * Enable EtherCAT interrupts
 *
 * @note    This function is the Hardware Access Interface function could be called from the SSC.
 **********************************************************************************************************************/
void ENABLE_ESC_INT ()
{
    ethercat_ssc_port_instance_ctrl_t * p_instance_ctrl = gp_ethercat_ssc_port->p_ctrl;

    R_BSP_IrqEnableNoClear(p_instance_ctrl->p_cfg->sync0_irq);
    R_BSP_IrqEnableNoClear(p_instance_ctrl->p_cfg->sync1_irq);
    R_BSP_IrqEnableNoClear(p_instance_ctrl->p_cfg->common_irq);
}
