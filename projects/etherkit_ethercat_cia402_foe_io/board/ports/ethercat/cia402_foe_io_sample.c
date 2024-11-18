/***********************************************************************************************************************
 * Copyright [2020-2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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

#include <rtthread.h>
#include <board.h>

#include "hal_data.h"
#include "ecat_def.h"
#include "ecatappl.h"
#include "ecatslv.h"
#include "applInterface.h"
#include "r_fw_up_rz_if.h"
#if (CiA402_SAMPLE_APPLICATION == 1)
#include "cia402appl.h"
#else
#include "sampleappl.h"
#endif
#include "renesashw.h"
#include <stdio.h>

void R_BSP_WarmStart(bsp_warm_start_event_t event) BSP_PLACE_IN_SECTION(".warm_start");

#if defined(BOARD_RZT2L_RSK)
extern fsp_err_t R_ETHER_PHY_StartAutoNegotiate (ether_phy_ctrl_t * const p_ctrl);
#endif
void handle_error(fsp_err_t err);

static bool app_status = 0;
static void foe_sample (void);

/*******************************************************************************************************************//**
 * @brief  EtherCAT Slave Stack example application
 *
 * The EtherCAT Slave Stack Code is provided by SSC tool.
 *
 **********************************************************************************************************************/
static void netdev_status_callback(struct netdev *netdev, rt_bool_t up)
{
    if (up)
    {
        foe_sample();
    }
    else
    {
        return;
    }
}

void netdev_monitor_init(void *param)
{
    struct netdev *netdev = netdev_get_by_name("e0");
    if (netdev == RT_NULL)
    {
        rt_kprintf("Failed to get network device.\n");
    }

    netdev_set_status_callback(netdev, netdev_status_callback);
}
INIT_APP_EXPORT(netdev_monitor_init);

static void foe_sample (void)
{
	if(app_status == 0)
	{
	fsp_err_t err;

    /* Open the QSPI instance */
    err = R_XSPI_QSPI_Open(&g_qspi0_ctrl, &g_qspi0_cfg);
	handle_error(err);

#if defined(BOARD_RZT2M_RSK) && (BSP_CFG_CORE_CR52 == 1)
    /* Open the shared memory driver. */
    err = R_SHARED_MEMORY_Open(&g_shared_memory0_ctrl, &g_shared_memory0_cfg);
    handle_error(err);
#endif

#if defined(BOARD_RZT2L_RSK)
	ethercat_ssc_port_extend_cfg_t * p_ethercat_ssc_port_ext_cfg;
	ether_phy_instance_t * p_ether_phy0;
	ether_phy_instance_t * p_ether_phy1;

	p_ethercat_ssc_port_ext_cfg = (ethercat_ssc_port_extend_cfg_t *)gp_ethercat_ssc_port->p_cfg->p_extend;
	p_ether_phy0 = (ether_phy_instance_t *)p_ethercat_ssc_port_ext_cfg->p_ether_phy_instance[0];
	p_ether_phy1 = (ether_phy_instance_t *)p_ethercat_ssc_port_ext_cfg->p_ether_phy_instance[1];
#endif

	/* Initialize EtherCAT SSC Port */
	err = RM_ETHERCAT_SSC_PORT_Open(gp_ethercat_ssc_port->p_ctrl, gp_ethercat_ssc_port->p_cfg);
	handle_error(err);

#if defined(BOARD_RZT2L_RSK)
	/* RZ/T2L RSK board needs starting auto negotiation by phy register access */
	R_ETHER_PHY_StartAutoNegotiate(p_ether_phy0->p_ctrl);
	R_ETHER_PHY_StartAutoNegotiate(p_ether_phy1->p_ctrl);
#endif

	/* Enable interrupt */
	__asm volatile ("cpsie i");

	/* Print that the EtherCAT Sample starts */
#if defined(BOARD_RZT2M_RSK)
#if (BANK == 0)
	char start_messege[] = "RZ/T2M EtherCAT sample program starts on BANK0.\r\n";
#elif (BANK == 1)
    char start_messege[] = "RZ/T2M EtherCAT sample program starts on BANK1.\r\n";
#endif
#endif
#if defined(BOARD_RZT2L_RSK)
#if (BANK == 0)
    char start_messege[] = "RZ/T2L EtherCAT sample program starts on BANK0.\r\n";
#elif (BANK == 1)
    char start_messege[] = "RZ/T2L EtherCAT sample program starts on BANK1.\r\n";
#endif
#endif
#if defined(BOARD_RZN2L_RSK)
#if (BANK == 0)
    char start_messege[] = "RZ/N2L EtherCAT sample program starts on BANK0.\r\n";
#elif (BANK == 1)
    char start_messege[] = "RZ/N2L EtherCAT sample program starts on BANK1.\r\n";
#endif
#endif
    /* Send massage to PC by UART communication. */
    rt_kprintf("%s",start_messege);

	/* Initilize the stack */
	MainInit();
#if (CiA402_SAMPLE_APPLICATION == 1)
	/* Initialize axis structures */
	CiA402_Init();
#endif

	/* Create basic mapping */
	APPL_GenerateMapping(&nPdInputSize,&nPdOutputSize);
	/* Set stack run flag */
	bRunApplication = TRUE;

	/* Execute the stack */
	while(bRunApplication == TRUE)
	{
		MainLoop();
	}
#if (CiA402_SAMPLE_APPLICATION == 1)
	/* Remove all allocated axes resources */
	CiA402_DeallocateAxis();
#endif
	/* Close SSC Port */
	RM_ETHERCAT_SSC_PORT_Close(gp_ethercat_ssc_port->p_ctrl);
	return;
	}

	return;
}

void handle_error(fsp_err_t err)
{
	FSP_PARAMETER_NOT_USED(err);
}