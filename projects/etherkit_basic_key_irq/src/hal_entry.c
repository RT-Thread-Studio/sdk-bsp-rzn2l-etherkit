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

#define DBG_TAG "irq"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 配置 LED 灯引脚 */
#define LED_PIN_B    BSP_IO_PORT_14_PIN_0 /* Onboard BLUE LED pins */
#define LED_PIN_G    BSP_IO_PORT_14_PIN_1 /* Onboard GREEN LED pins */

/* 配置 KEY 输入引脚  */
#define PIN_KEY1        BSP_IO_PORT_14_PIN_2      // P14_2 :  KEY1
#define PIN_KEY2        BSP_IO_PORT_16_PIN_3      // P16_3 :  KEY2

static void irq_callback_blue_led(void *args)
{
    LOG_D("IRQ:%d triggered!", args);
}

static void irq_callback_green_led(void *args)
{
    LOG_D("IRQ:%d triggered!", args);
}

void hal_entry(void)
{
    rt_kprintf("\nHello RT-Thread!\n");
    rt_kprintf("==================================================\n");
    rt_kprintf("This example project is an basic key irq routine!\n");
    rt_kprintf("==================================================\n");

    unsigned int count = 1;

    /* 设置 RGB 绿灯引脚的模式为输出模式 */
    rt_pin_mode(LED_PIN_B, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_PIN_G, PIN_MODE_OUTPUT);
    /* 设置 KEY 引脚的模式为输入上拉模式 */
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);

    /* init */
    rt_err_t err = rt_pin_attach_irq(PIN_KEY1, PIN_IRQ_MODE_RISING, irq_callback_blue_led, (void *)1);
    if (RT_EOK != err)
    {
        rt_kprintf("\n attach irq failed. \n");
    }
    err = rt_pin_attach_irq(PIN_KEY2, PIN_IRQ_MODE_RISING, irq_callback_green_led, (void *)2);
    if (RT_EOK != err)
    {
        rt_kprintf("\n attach irq failed. \n");
    }

    err = rt_pin_irq_enable(PIN_KEY1, PIN_IRQ_ENABLE);
    if (RT_EOK != err)
    {
        rt_kprintf("\n enable irq failed. \n");
    }
    err = rt_pin_irq_enable(PIN_KEY2, PIN_IRQ_ENABLE);
    if (RT_EOK != err)
    {
        rt_kprintf("\n enable irq failed. \n");
    }

    while (count > 0)
    {
        /* 读取按键 KEY 的引脚状态 */
        if (rt_pin_read(PIN_KEY1) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_KEY1) == PIN_LOW)
            {
                /* 按键已被按下，输出 log，点亮 LED 灯 */
                rt_pin_write(LED_PIN_B, PIN_LOW);
            }
        }
        else if(rt_pin_read(PIN_KEY2) == PIN_LOW)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_KEY2) == PIN_LOW)
            {
                /* 按键已被按下，输出 log，点亮 LED 灯 */
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

