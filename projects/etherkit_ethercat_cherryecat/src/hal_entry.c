/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2024-03-11     Wangyuqiang   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "hal_data.h"
#include "ec_master.h"

#define LED_PIN_0    BSP_IO_PORT_14_PIN_3 /* Onboard LED pins */
#define LED_PIN_1    BSP_IO_PORT_14_PIN_0 /* Onboard LED pins */
#define LED_PIN_2    BSP_IO_PORT_14_PIN_1 /* Onboard LED pins */

#define MOTOR_MODE_CSV_CSP  0
#define MOTOR_MODE_CSP      1
#define MOTOR_MODE_CSV      2

volatile uint8_t motor_mode = MOTOR_MODE_CSP;

ec_master_t g_ec_master;
uint8_t cherryecat_eepromdata[2048];

static ec_pdo_entry_info_t coe402_1601[] =
{
    { 0x6040, 0x00, 0x10 }, // Control Word
    { 0x6072, 0x00, 0x10 }, // Max Torque
    { 0x607A, 0x00, 0x20 }, // Target Position
    { 0x60B0, 0x00, 0x20 }, // Position Offset
    { 0x60B1, 0x00, 0x20 }, // Velocity Offset
    { 0x60B2, 0x00, 0x10 }, // Torque Offset
    { 0x60B8, 0x00, 0x10 }, // Touch Probe Function
    { 0x60E0, 0x00, 0x10 }, // Positive Torque Limit Value
    { 0x60E0, 0x00, 0x10 }, // Negative Torque Limit Value
    { 0x60FE, 0x01, 0x20 }, // Digital Outputs
};

static ec_pdo_entry_info_t coe402_1a01[] =
{
    { 0x603F, 0x00, 0x10 }, // Error Code
    { 0x6041, 0x00, 0x10 }, // Status Word
    { 0x6064, 0x00, 0x20 }, // Actual Position
    { 0x606C, 0x00, 0x20 }, // Actual Velocity
    { 0x6077, 0x00, 0x10 }, // Actual Torque
    { 0x60B9, 0x00, 0x10 }, // Touch Probe Status
    { 0x60BA, 0x00, 0x20 }, // Touch Probe Position 1 Positive Value
    { 0x60BC, 0x00, 0x20 }, // Touch Probe Position 2 Positive Value
    { 0x60F4, 0x00, 0x20 }, // Following Error Actual Value
    { 0x60FD, 0x00, 0x20 }, // Digital Inputs
};

static ec_pdo_info_t cia402_rxpdos[] =
{
    { 0x1601, 10, &coe402_1601[0] },
};

static ec_pdo_info_t cia402_txpdos[] =
{
    { 0x1a01, 10, &coe402_1a01[0] },
};

static ec_sync_info_t cia402_syncs[] =
{
    { 2, EC_DIR_OUTPUT, 1, cia402_rxpdos },
    { 3, EC_DIR_INPUT, 1, cia402_txpdos },
};

int ec_start(int argc, const char **argv)
{
    static ec_slave_config_t slave_cia402_config;

    if (g_ec_master.slave_count == 0)
    {
        printf("No slave found, please check the connection\r\n");
        return -1;
    }

    slave_cia402_config.dc_assign_activate = 0x300;

    slave_cia402_config.dc_sync[0].cycle_time = atoi(argv[1]) * 1000;
    slave_cia402_config.dc_sync[0].shift_time = 500000;
    slave_cia402_config.dc_sync[1].cycle_time = 0;
    slave_cia402_config.dc_sync[1].shift_time = 0;

    slave_cia402_config.sync = cia402_syncs;
    slave_cia402_config.sync_count = sizeof(cia402_syncs) / sizeof(ec_sync_info_t);

    for (uint32_t i = 0; i < g_ec_master.slave_count; i++)
    {
        if (g_ec_master.slaves[i].sii.vendor_id != 0x00000766)
        {
            EC_LOG_ERR("Unsupported slave found: vendor_id=0x%08x\n", g_ec_master.slaves[i].sii.vendor_id);
            return -1;
        }

        g_ec_master.slaves[i].config = &slave_cia402_config;
    }

    ec_master_start(&g_ec_master, atoi(argv[1]));

    return 0;
}
MSH_CMD_EXPORT(ec_start, ethercat start);

int ec_stop(int argc, const char **argv)
{
    ec_master_stop(&g_ec_master);
    return 0;
}
MSH_CMD_EXPORT(ec_stop, ethercat stop);

void hal_entry(void)
{
    rt_kprintf("\nHello RT-Thread!\n");
    rt_kprintf("==================================================\n");
    rt_kprintf("This example project is an ethernet routine!\n");
    rt_kprintf("==================================================\n");

    ec_master_cmd_init(&g_ec_master);
    ec_master_init(&g_ec_master, 0);

    while (1)
    {
        rt_pin_write(LED_PIN_0, PIN_HIGH);
        rt_pin_write(LED_PIN_1, PIN_HIGH);
        rt_pin_write(LED_PIN_2, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(LED_PIN_0, PIN_LOW);
        rt_pin_write(LED_PIN_1, PIN_LOW);
        rt_pin_write(LED_PIN_2, PIN_LOW);
        rt_thread_mdelay(1000);
    }
}
