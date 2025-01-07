/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2024-09-11     Wangyuqiang   first version
 */

#include <rtthread.h>
#include <board.h>

#include "hal_data.h"
#include "ecat_def.h"
#include "ecatappl.h"
#include "ecatslv.h"
#include "applInterface.h"
#if (CiA402_SAMPLE_APPLICATION == 1)
#include "cia402appl.h"
#else
#include "sampleappl.h"
#endif
#include "renesashw.h"
#include <stdio.h>

static rt_uint8_t ethercat_thread_stack[1024 * 4];
static struct rt_thread ethercat_thread;

static void ecat_thread_entry ();

void phy_rtl8211f_initial(ether_phy_instance_ctrl_t *phydev)
{
#define RTL_8211F_PAGE_SELECT 0x1F
#define RTL_8211F_EEELCR_ADDR 0x11
#define RTL_8211F_LED_PAGE 0xD04
#define RTL_8211F_LCR_ADDR 0x10

    uint32_t val1, val2 = 0;

    /* switch to led page */
    R_ETHER_PHY_Write(phydev, RTL_8211F_PAGE_SELECT, RTL_8211F_LED_PAGE);

    /* set led1(green) Link 10/100/1000M, and set led2(yellow) Link 10/100/1000M+Active */
    R_ETHER_PHY_Read(phydev, RTL_8211F_LCR_ADDR, &val1);
    val1 |= (1 << 5);
    val1 |= (1 << 8);
    val1 &= (~(1 << 9));
    val1 |= (1 << 10);
    val1 |= (1 << 11);
    R_ETHER_PHY_Write(phydev, RTL_8211F_LCR_ADDR, val1);

    /* set led1(green) EEE LED function disabled so it can keep on when linked */
    R_ETHER_PHY_Read(phydev, RTL_8211F_EEELCR_ADDR, &val2);
    val2 &= (~(1 << 2));
    R_ETHER_PHY_Write(phydev, RTL_8211F_EEELCR_ADDR, val2);

    /* switch back to page0 */
    R_ETHER_PHY_Write(phydev, RTL_8211F_PAGE_SELECT, 0xa42);

    rt_thread_mdelay(100);

    return;
}

static int coe_app(void)
{
    rt_kprintf("==================================================\n");
    rt_kprintf("EtherCAT Slave with CoE Project!\n");
    rt_kprintf("==================================================\n");

    rt_err_t ret = rt_thread_init(&ethercat_thread,
                "ethercat_thread",
                ecat_thread_entry,
                RT_NULL,
                &ethercat_thread_stack[0],
                sizeof(ethercat_thread_stack),
                16, 10);

    if(ret == RT_EOK)
    {
        rt_thread_startup(&ethercat_thread);
    }

    return 0;
}
INIT_APP_EXPORT(coe_app);

static void ecat_thread_entry ()
{
    /* Initialize EtherCAT SSC Port */
    RM_ETHERCAT_SSC_PORT_Open(gp_ethercat_ssc_port->p_ctrl, gp_ethercat_ssc_port->p_cfg);

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
        rt_thread_mdelay(1);
    }
#if (CiA402_SAMPLE_APPLICATION == 1)
    /* Remove all allocated axes resources */
    CiA402_DeallocateAxis();
#endif
    /* Close SSC Port */
    RM_ETHERCAT_SSC_PORT_Close(gp_ethercat_ssc_port->p_ctrl);

    return;
}
