/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2024-08-30    Wangyuqiang   first version
 */

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>
#include <board.h>

#include "ecat_def.h"
#include "ecatappl.h"
#include "ecatslv.h"
#include "applInterface.h"
#if (CiA402_SAMPLE_APPLICATION == 1)
#include "cia402appl.h"
#else
#include "sampleappl.h"
#endif

#include "fsp_common_api.h"

static rt_uint8_t ethercat_thread_stack[1024 * 4];
static struct rt_thread ethercat_thread;

static bool app_status = 0;

static void ecat_thread_entry();
static void eoe_app(void);

static void netdev_status_callback(struct netdev *netdev, rt_bool_t up)
{
    if (up)
    {
        eoe_app();
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

static void eoe_app(void)
{
	if(app_status == 0)
	{
		rt_kprintf("==================================================\n");
		rt_kprintf("EtherCAT Slave with EOE Project!\n");
		rt_kprintf("==================================================\n");

		rt_thread_init(&ethercat_thread,
					"ethercat_thread",
					ecat_thread_entry,
					RT_NULL,
					&ethercat_thread_stack[0],
					sizeof(ethercat_thread_stack),
					16, 10);

		rt_thread_startup(&ethercat_thread);

		return;
	}

	return;
}

static void ecat_thread_entry(void *parameter)
{
	fsp_err_t err;

#if defined(BOARD_RZT2L_RSK)
	ethercat_ssc_port_extend_cfg_t *p_ethercat_ssc_port_ext_cfg;
	ether_phy_instance_t *p_ether_phy0;
	ether_phy_instance_t *p_ether_phy1;

	p_ethercat_ssc_port_ext_cfg = (ethercat_ssc_port_extend_cfg_t *)gp_ethercat_ssc_port->p_cfg->p_extend;
	p_ether_phy0 = (ether_phy_instance_t *)p_ethercat_ssc_port_ext_cfg->p_ether_phy_instance[0];
	p_ether_phy1 = (ether_phy_instance_t *)p_ethercat_ssc_port_ext_cfg->p_ether_phy_instance[1];
#endif

	/* TODO: add your own code here */
	/* Initialize EtherCAT SSC Port */
	err = RM_ETHERCAT_SSC_PORT_Open(gp_ethercat_ssc_port->p_ctrl, gp_ethercat_ssc_port->p_cfg);
	if (FSP_SUCCESS != err)
	{
		__BKPT(0); /* Can't continue the stack */
	}

#if defined(BOARD_RZT2L_RSK)
	/* RZ/T2L RSK board needs starting auto negotiation by phy register access */
	R_ETHER_PHY_StartAutoNegotiate(p_ether_phy0->p_ctrl);
	R_ETHER_PHY_StartAutoNegotiate(p_ether_phy1->p_ctrl);
#endif

	/* Initilize the stack */
	MainInit();
#if (CiA402_SAMPLE_APPLICATION == 1)
	/* Initialize axis structures */
	CiA402_Init();
#endif
	/* Create basic mapping */
	APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);
	/* Set stack run flag */
	bRunApplication = TRUE;
	/* Execute the stack */
	while (bRunApplication == TRUE)
	{
		MainLoop();
		rt_thread_mdelay(1);
	}
#if (CiA402_SAMPLE_APPLICATION == 1)
	/* Remove all allocated axes resources */
	CiA402_DeallocateAxis();
#endif
	/* Close SSC Port */
	RM_ETHERCAT_SSC_PORT_Close(gp_ethercat_ssc_port->p_ctrl);
}

void rm_ssc_port_timer_interrupt(timer_callback_args_t *p_args)
{
	rt_interrupt_enter();
	if (TIMER_EVENT_CYCLE_END == p_args->event)
	{
		HW_IncTimer();
	}
	rt_interrupt_leave();
}

void ssc_port_callback(ethercat_ssc_port_callback_args_t *p_args)
{
	rt_interrupt_enter();
	switch (p_args->event)
	{
	case ETHERCAT_SSC_PORT_EVENT_ESC_CAT_INTERRUPT:
	{
		PDI_Isr(); /* PDI ISR */
		break;
	}
	case ETHERCAT_SSC_PORT_EVENT_ESC_SYNC0_INTERRUPT:
	{
		Sync0_Isr(); /* Sync0 ISR */
		break;
	}
	case ETHERCAT_SSC_PORT_EVENT_ESC_SYNC1_INTERRUPT:
	{
		Sync1_Isr(); /* Sync1 ISR */
		break;
	}
	default:
	{
		/* Do Nothing! */
	}
	}
	rt_interrupt_leave();
}
