/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2024-03-11    Wangyuqiang   first version
 */

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>
#include <board.h>
#include <factory_test.h>
#include <arpa/inet.h>
#include <netdev.h>
#define LED_PIN_0    BSP_IO_PORT_14_PIN_3 /* Onboard LED pins */
#define LED_PIN_1    BSP_IO_PORT_14_PIN_0 /* Onboard LED pins */
#define LED_PIN_2    BSP_IO_PORT_14_PIN_1 /* Onboard LED pins */

#define KEY_1        BSP_IO_PORT_14_PIN_2 /* Onboard KEY2 pins */
#define KEY_2        BSP_IO_PORT_16_PIN_3 /* Onboard KEY3 pins */
#define KEY_RESET    BSP_IO_PORT_16_PIN_0 /* Onboard KEY4 pins */

#define DIGITAL_IN0  BSP_IO_PORT_04_PIN_1
#define DIGITAL_IN1  BSP_IO_PORT_16_PIN_7
#define DIGITAL_IN2   BSP_IO_PORT_17_PIN_3
#define DIGITAL_IN3   BSP_IO_PORT_18_PIN_6        

#define DIGITAL_OUT0  BSP_IO_PORT_12_PIN_4          

#define DIGITAL_OUT1   BSP_IO_PORT_22_PIN_3             

#define DIGITAL_OUT2   BSP_IO_PORT_22_PIN_2         

#define DIGITAL_OUT3   BSP_IO_PORT_17_PIN_4            
static char ip_addr[]="180.163.146.100";
static int rtc_sample(void);
void key1_entry(external_irq_callback_args_t * p_args);
void key2_entry(external_irq_callback_args_t * p_args);
rt_uint8_t eth_test();
void test_init(void);
void  test_board_entry(void);
void led_entry();
rt_uint8_t flag_eth,flag_board=0;
void hal_entry(void)
{
    rt_kprintf("\nHello RT-Thread!\n");
    rt_kprintf("==================================================\n");
    rt_kprintf("\nHello RT-Thread!\n");
    rt_kprintf("==================================================\n");
    adc_init();
    test_init();
    c_struct.led_flag=0;
    rt_thread_mdelay(2000);
    while (1)
    {
        if(flag_eth==1)
        {
            eth_test();

        }
        if(flag_board==1)
        {
            test_board_entry();
        }
        if(c_struct.led_flag==1)
        {
            led_entry();
        }
        rt_thread_mdelay(500);
    }
}
void input0_entry(void *args)
{
    rt_uint8_t pin_value;
    //将参数指针转化为pin_value
    pin_value = *(rt_uint8_t *)args;
    rt_kprintf("input%d_entry\n",pin_value);
}
void test_init(void)
{
      rt_pin_write(LED_PIN_2, PIN_HIGH);
      rt_pin_write(DIGITAL_OUT0, PIN_LOW);
      rt_pin_write(DIGITAL_OUT1, PIN_LOW);
      rt_pin_write(DIGITAL_OUT2, PIN_LOW);
      rt_pin_write(DIGITAL_OUT3, PIN_LOW);
      R_SCI_UART_Open(&g_uart5_ctrl, &g_uart5_cfg);
      rt_pin_attach_irq(KEY_1, PIN_IRQ_MODE_RISING_FALLING, key1_entry,NULL);
      rt_pin_irq_enable(KEY_1, PIN_IRQ_ENABLE);
      rt_pin_attach_irq(KEY_2, PIN_IRQ_MODE_RISING_FALLING, key2_entry,NULL);
      rt_pin_irq_enable(KEY_2, PIN_IRQ_ENABLE);
}
///**
// * @brief  KEY1 按键中断服务函数 key按下触发ETH发送ping www.baidu.com
// * @param args
// * @return
// */
void key1_entry(external_irq_callback_args_t * p_args)
{
    flag_board=1;
    if(rt_pin_read(LED_PIN_1))
    {
        rt_pin_write(LED_PIN_1, PIN_LOW);
    }
    else {
        //同时蓝灯LED1常亮
        rt_pin_write(LED_PIN_1, PIN_HIGH);
    }
}
void led_entry()
{
    if(rt_pin_read(LED_PIN_0))
    {
        rt_pin_write(LED_PIN_0, PIN_LOW);
    }
    else {
        //同时蓝灯LED1常亮
        rt_pin_write(LED_PIN_0, PIN_HIGH);
    }
}
//   KEY2 按键中断服务函数  key2按下测试CAN通信与RS485是否正常

void key2_entry(external_irq_callback_args_t * p_args)
{
    flag_eth=1;
    RS485_Send_Example(flag_eth);
    if(rt_pin_read(LED_PIN_1))
    {
        rt_pin_write(LED_PIN_1, PIN_LOW);
    }
    else{
        //同时蓝灯LED1常亮
        rt_pin_write(LED_PIN_1, PIN_HIGH);
    }
}
void  test_board_entry(void)
{
         flag_board=0;
         rt_kprintf("=====================RTCTEST=============================\n");
         if(rtc_sample()!=RT_EOK)
         {
            rt_kprintf("RTC Test Failed\r\n");
            goto error_print;
         }
          rt_kprintf("=====================RTCTESTEND=============================\n");
         rt_kprintf("=====================ADCTEST=============================\n");
         rt_kprintf("=====================ADC0-CHANNEL0 Start Test!=============================\n");
         adc_channel_test(c_struct.dev_adc_0,0);
         if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC0-CHANNEL0 Test Failed!\n");
            goto error_print;
         }

         rt_kprintf("=====================ADC0-CHANNEL0 END Test!=============================\n");
        rt_kprintf("=====================ADC0-CHANNEL1 Start Test!=============================\n");
        adc_channel_test(c_struct.dev_adc_0,1);
         if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC0-CHANNEL1 Test Failed!\n");
         goto error_print;
         }
         rt_kprintf("=====================ADC0-CHANNEL1 END Test!=============================\n");
         rt_kprintf("=====================ADC0-CHANNEL2 Start Test!=============================\n");
         adc_channel_test(c_struct.dev_adc_0,2);
         if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC0-CHANNEL2 Test Failed!\n");
            goto error_print;
         }
         rt_kprintf("=====================ADC0-CHANNEL2 END Test!=============================\n");
         rt_kprintf("=====================ADC0-CHANNEL3 Start Test!=============================\n");
         adc_channel_test(c_struct.dev_adc_0,3);
        if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC0-CHANNEL3 Test Failed!\n");
            goto error_print;
         }
         rt_kprintf("=====================ADC0-CHANNEL3 END Test!=============================\n");

         rt_kprintf("=====================ADC1-CHANNEL0 Start Test!=============================\n");
         adc_channel_test(c_struct.dev_adc_1,0);
         if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC1-CHANNEL0 Test Failed!\n");
            goto error_print;
         }
         rt_kprintf("=========================ADC1-CHANNEL0 END Test!=========================\n");
         rt_kprintf("=====================ADC1-CHANNEL1 Start Test!=============================\n");
        adc_channel_test(c_struct.dev_adc_1,1);
         if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC1-CHANNEL1 Test Failed!\n");
            goto error_print;
         }
         rt_kprintf("=====================ADC1-CHANNEL1 END Test!=============================\n");
         rt_kprintf("=====================ADC1-CHANNEL2 Start Test!=============================\n");
         adc_channel_test(c_struct.dev_adc_1,2);
         if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC1-CHANNEL2 Test Failed!\n");
            goto error_print;
         }
         rt_kprintf("=====================ADC1-CHANNEL2 END Test!=============================\n");
         rt_kprintf("=====================ADC1-CHANNEL3 Start Test!=============================\n");
         adc_channel_test(c_struct.dev_adc_1,3);
        if (c_struct.adc_flag==0)
         {
            /* code */
            rt_kprintf("ADC1-CHANNEL3 Test Failed!\n");
            goto error_print;
         }
         rt_kprintf("=====================ADC1-CHANNEL3 END Test!=============================\n");


        rt_kprintf("=====================EEROMTEST=============================\n");
        if(at24cxx()!=RT_EOK)
         {
              rt_kprintf("EEPROM Test Failed!\n");
              goto error_print;

         }
       rt_kprintf("=====================EEROMTESTEND=============================\n");
       rt_kprintf("=====================DIGITAL_TEST=============================\n");
       if(rt_pin_read(DIGITAL_IN0))
       {
           rt_kprintf("DIGITAL_IN0/OUT0 Test Failed!\n");
           goto error_print;
       }
       if(rt_pin_read(DIGITAL_IN1))
       {
           rt_kprintf("DIGITAL_IN1/OUT1 Test Failed!\n");
           goto error_print;
       }
       if(rt_pin_read(DIGITAL_IN2))
       {
           rt_kprintf("DIGITAL_IN2/OUT2 Test Failed!\n");
           goto error_print;
       }
       if(rt_pin_read(DIGITAL_IN3))
       {
           rt_kprintf("DIGITAL_IN3/OUT3 Test Failed!\n");
           goto error_print;
       }
       rt_kprintf("=====================DIGITAL_TESTEND=============================\n");
       rt_kprintf("=====================ETHTEST=============================\n");
       if(eth_test()!=RT_EOK)
       {
             rt_kprintf("ETH Test Failed!\n");
             goto error_print;
       }
       rt_kprintf("=====================ETHTESTEND=============================\n");
       rt_thread_delay(1000);
       rt_kprintf("=====================RS485TEST=============================\n");
       rt_thread_delay(1000);
       if(c_struct.serial_flag!=1)
       {
           rt_kprintf("RS485 Test Failed!\n");
           goto error_print;
       }
       R_SCI_UART_Close(&g_uart5_ctrl);
       rt_kprintf("=====================RS485TESTEND=============================\n");
       rt_kprintf("=====================CANTEST=============================\n");
       can_sample_send();
       rt_thread_delay(1000);
       can1_sample_receive();
       rt_thread_delay(1000);
       if(c_struct.can_flag1!=1)
       {
         rt_kprintf("CAN Test Failed!\n");
         goto error_print;
        };
        rt_kprintf("=====================CANTESTSUCCESS=============================\n");
        rt_kprintf("=====================CANTESTEND=============================\n");
        rt_pin_write(LED_PIN_2, PIN_LOW);
        return RT_EOK;
        error_print:
           printf("TEST ERROR.\n");
           c_struct.led_flag=1;
    
}
rt_uint8_t eth_test()
{
    flag_eth=0;
    rt_kprintf("Start ping!!\r\n");

    struct netdev *netdev=netdev_get_by_name("e0");
    if(netdev==RT_NULL)
    {
        rt_kprintf("erron\r\n");
    }
    char * target_name=ip4addr_ntoa(&(netdev->gw));
    netdev = netdev_get_first_by_flags(NETDEV_FLAG_LINK_UP);
    if(netdev_cmd_ping(target_name, RT_NULL, 2, 0)==RT_EOK)
    {
    //同时蓝灯LED1常亮
    rt_pin_write(LED_PIN_1, PIN_LOW);
    return RT_EOK;
    }
    else{
    //同时红等LED1常亮
    rt_pin_write(LED_PIN_1, PIN_HIGH);
    return RT_ERROR;
   }
}
#define RTC_NAME       "rtc"

static int rtc_sample(void)
{
    rt_err_t ret = RT_EOK;
    time_t now;
    rt_device_t device = RT_NULL;

    device = rt_device_find(RTC_NAME);
    if (!device)
    {
      rt_kprintf("find %s failed!\n", RTC_NAME);
      return RT_ERROR;
    }

    if(rt_device_open(device, 0) != RT_EOK)
    {
      rt_kprintf("open %s failed!\n", RTC_NAME);
      return RT_ERROR;
    }

    ret = set_date(2024, 9, 14);
    rt_kprintf("set RTC date to 2024-9-14\n");
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }
    ret = set_time(15, 03, 50);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC time failed\n");
        return ret;
    }
    rt_thread_mdelay(1000);
    rt_uint8_t err=get_timestamp(&now);
    rt_kprintf("now: %.*s", 25, ctime(&now));

    return ret;
}

