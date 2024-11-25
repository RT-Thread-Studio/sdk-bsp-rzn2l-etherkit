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

#include <dfs_fs.h>

#define LED_PIN_0    BSP_IO_PORT_14_PIN_3 /* Onboard LED pins */
#define LED_PIN_1    BSP_IO_PORT_14_PIN_0 /* Onboard LED pins */
#define LED_PIN_2    BSP_IO_PORT_14_PIN_1 /* Onboard LED pins */

void hal_entry(void)
{
    rt_kprintf("\nHello RT-Thread!\n");
    rt_kprintf("==================================================\n");
    rt_kprintf("This example project is an ethernet routine!\n");
    rt_kprintf("==================================================\n");

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

/* Initialize the filesystem */
static int filesystem_init(void)
{
    // ramdisk as root
    if(rt_device_find("ramdisk0"))
    {
        // format the ramdisk
        extern void mkfs(const char *fs_name, const char *device_name);
        mkfs("elm", "ramdisk0");

        /* mount ramdisk0 as root directory */
        if (dfs_mount("ramdisk0", "/", "elm", 0, RT_NULL) == 0)
        {
            rt_kprintf("ramdisk mounted on \"/\".");

            /* now you can create dir dynamically. */
            mkdir("/download", 0x777);
        }
        else
        {
            rt_kprintf("root folder creation failed!\n");
        }
        return RT_EOK;
    }
    rt_kprintf("cannot find ramdisk device");
    return -RT_ERROR;
}
INIT_ENV_EXPORT(filesystem_init);

static int ramdisk_device_init(void)
{
    extern rt_err_t ramdisk_init(const char *dev_name, rt_uint8_t* disk_addr, rt_size_t block_size, rt_size_t num_block);
    ramdisk_init("ramdisk0", NULL, 128, 1024);
    return RT_EOK;
}
INIT_DEVICE_EXPORT(ramdisk_device_init);
