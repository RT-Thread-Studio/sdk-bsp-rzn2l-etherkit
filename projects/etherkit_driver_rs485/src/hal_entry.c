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

/* 配置 KEY 输入引脚  */
#define PIN_KEY1        BSP_IO_PORT_14_PIN_2      // P14_2 :  KEY1
#define PIN_KEY2        BSP_IO_PORT_16_PIN_3      // P16_3 :  KEY2
#define RS485_OUT       rt_pin_write(BSP_IO_PORT_22_PIN_0,PIN_HIGH)
#define RS485_IN        rt_pin_write(BSP_IO_PORT_22_PIN_0,PIN_LOW)
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
     R_SCI_UART_Open(&g_uart5_ctrl, &g_uart5_cfg);
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
        rt_thread_mdelay(100);
        count++;
    }
}
// /*发送数据函数*/
static void RS485_Send_Example( uint8_t ch )
{
    /*串口写入函数*/
    R_SCI_UART_Write(&g_uart5_ctrl, (uint8_t*) &ch, 1);
}

/*RS485_1中断回调函数*/
void rs485_callback(uart_callback_args_t * p_args)
{
    rt_interrupt_enter();

    switch(p_args->event)
    {
        /*接收数据时将数据打印出来*/
        case UART_EVENT_RX_CHAR:
          {
            rt_kprintf("%d\n", p_args->data);
            break;
          }
        default:
            break;
    }
    rt_interrupt_leave();
}

int rs485_send_test(void)
{
   static uint8_t i;

   for(i =1; i <= 10; i++)
   {
       /*发送数据*/
       RS485_Send_Example(i);
       rt_thread_delay(1000);
   }
   return 0;
}
MSH_CMD_EXPORT(rs485_send_test, rs485_send_test)
