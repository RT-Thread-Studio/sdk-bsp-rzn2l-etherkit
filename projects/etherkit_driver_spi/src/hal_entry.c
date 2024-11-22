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
#include "hal_data.h"
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "key"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
#define LED_PIN_B    BSP_IO_PORT_14_PIN_0 /* Onboard BLUE LED pins */
#define LED_PIN_G    BSP_IO_PORT_14_PIN_1 /* Onboard GREEN LED pins */
void spi_loop_test(void);
/* 配置 KEY 输入引脚  */
#define PIN_KEY1        BSP_IO_PORT_14_PIN_2      // P14_2 :  KEY1
#define PIN_KEY2        BSP_IO_PORT_16_PIN_3      // P16_3 :  KEY2

void hal_entry(void)
{
    rt_kprintf("\nHello RT-Thread!\n");
    rt_kprintf("==================================================\n");
    rt_kprintf("This example project is an basic key routine!\n");
    rt_kprintf("==================================================\n");

    unsigned int count = 1;

    /* 设置 RGB 绿灯引脚的模式为输出模式 */
    rt_pin_mode(LED_PIN_B, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_PIN_G, PIN_MODE_OUTPUT);
    /* 设置 KEY 引脚的模式为输入上拉模式 */
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    spi_loop_test();
    while (count > 0)
    {
        /* 读取按键 KEY 的引脚状态 */
        if (rt_pin_read(PIN_KEY1) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_KEY1) == PIN_LOW)
            {
                /* 按键已被按下，输出 log，点亮 LED 灯 */
                LOG_D("KEY1 pressed!");
                rt_pin_write(LED_PIN_B, PIN_LOW);
            }
        }
        else if(rt_pin_read(PIN_KEY2) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_KEY2) == PIN_LOW)
            {
                /* 按键已被按下，输出 log，点亮 LED 灯 */
                LOG_D("KEY2 pressed!");
                rt_pin_write(LED_PIN_G, PIN_LOW);
            }
        }
        else
        {
            /* 按键没被按下，熄灭 LED 灯 */
            rt_pin_write(LED_PIN_B, PIN_HIGH);
            rt_pin_write(LED_PIN_G, PIN_HIGH);
        }
        rt_thread_mdelay(10);
        count++;
    }
}
void spi_loop_test(void)
{
#define TEXT_NUMBER_SIZE 1024
#define SPI_BUS_NAME "sci3s"
#define SPI_NAME "spi30"

    static uint8_t sendbuf[TEXT_NUMBER_SIZE] = {0};
    static uint8_t readbuf[TEXT_NUMBER_SIZE] = {0};

    for (int i = 0; i < sizeof(readbuf); i++)
    {
        sendbuf[i] = i;
    }

    static struct rt_spi_device *spi_dev = RT_NULL;
    struct rt_spi_configuration cfg;

    rt_hw_sci_spi_device_attach(SPI_BUS_NAME, SPI_NAME, NULL);

    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB | RT_SPI_NO_CS;
    cfg.max_hz = 1 * 1000 * 1000;

    spi_dev = (struct rt_spi_device *)rt_device_find(SPI_NAME);
    if (RT_NULL == spi_dev)
    {
        rt_kprintf("spi sample run failed! can't find %s device!\n", SPI_NAME);
        return;
    }
    rt_spi_configure(spi_dev, &cfg);

    rt_kprintf("%s send:\n", SPI_NAME);
    for (int i = 0; i < sizeof(sendbuf); i++)
    {
        rt_kprintf("%02x ", sendbuf[i]);
    }

    rt_spi_transfer(spi_dev, sendbuf, readbuf, sizeof(sendbuf));
    rt_kprintf("\n\n%s rcv:\n", SPI_NAME);

    for (int i = 0; i < sizeof(readbuf); i++)
    {
        if (readbuf[i] != sendbuf[i])
        {
            rt_kprintf("SPI test fail!!!\n");
            break;
        }
        else
            rt_kprintf("%02x ", readbuf[i]);
    }

    rt_kprintf("\n\n");
    rt_kprintf("SPI test end\n");
}
