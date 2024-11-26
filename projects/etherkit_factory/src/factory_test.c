/**
 * 
 * 测试程序:测试内容为 
 * 1、spiflash;
 * 2、iic0->eerom;
 * 3、canfd0;
 * 4、rs485;
 * 5、uart0\uart5;
 * 6、adc\dac；
 * 7、gpio->digial_input\output;
 * 8、analog_input;
 * 9、eth0;
 * 10、key1 \key2;
 * 11、led0,led1,led2;
 */
/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2024-011-8    liminghui   first version
 */

#include <rtthread.h>
#include "hal_data.h"
#include <rtdevice.h>
#include <board.h>
#include <factory_test.h>
#include <netdev_ipaddr.h>
#include <netdev.h>

 struct  commom_struct c_struct;
 static struct rt_semaphore rx_sem;     /* 用于接收消息的信号量 */
 static struct rt_semaphore rx_sem1;     /* 用于接收消息的信号量 */
 static rt_device_t can_dev;            /* CAN 设备句柄 */

 /* 接收数据回调函数 */
 static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
 {
     /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
     rt_sem_release(&rx_sem);
     c_struct.can_flag=1;
     return RT_EOK;
 }
 /* 接收数据回调函数 */
 static rt_err_t can1_rx_call(rt_device_t dev, rt_size_t size)
 {
     /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
     rt_sem_release(&rx_sem1);
     c_struct.can_flag1=1;
     return RT_EOK;
 }
 static void can_rx_thread(void *parameter)
 {
     struct rt_can_msg rxmsg = {0};

     /* 设置接收回调函数 */
     rt_device_set_rx_indicate(c_struct.can_dev, can_rx_call);

     while (1)
     {
         /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
         rxmsg.hdr_index = -1;
         /* 阻塞等待接收信号量 */
         rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
         /* 从 CAN 读取一帧数据 */
         rt_device_read(c_struct.can_dev, 0, &rxmsg, sizeof(rxmsg));
         /* 打印数据 ID 及内容 */
         rt_kprintf("ID:%x\tmessege:%s\n", rxmsg.id, rxmsg.data);
     }
 }
 static void can1_rx_thread(void *parameter)
  {
      struct rt_can_msg rxmsg = {0};

      /* 设置接收回调函数 */
      rt_device_set_rx_indicate(c_struct.can1_dev, can1_rx_call);

      while (1)
      {
          /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
          rxmsg.hdr_index = -1;
          /* 阻塞等待接收信号量 */
          rt_sem_take(&rx_sem1, RT_WAITING_FOREVER);
          /* 从 CAN 读取一帧数据 */
          rt_device_read(c_struct.can1_dev, 0, &rxmsg, sizeof(rxmsg));
          /* 打印数据 ID 及内容 */
          rt_kprintf("ID:%x\tmessege:%s\n", rxmsg.id, rxmsg.data);
      }
  }
 static void can_tx_thread(void *parameter)
 {
     struct rt_can_msg msg = {0};
     rt_size_t  size;
     msg.id = 0x78;              /* ID 为 0x78 */
     msg.ide = RT_CAN_STDID;     /* 标准格式 */
     msg.rtr = RT_CAN_DTR;       /* 数据帧 */
     msg.len = 8;                /* 数据长度为 8 */
     /* 待发送的 8 字节数据 */
     rt_memset(msg.data,0,sizeof(msg.data));
     int number = 0;
     while(1)
     {
         /* 发送一帧 CAN 数据 */
         rt_sprintf((char *)msg.data,"rtt:%d",number++);
         size = rt_device_write(can_dev, 0, &msg, sizeof(msg));
         if (size == 0)
         {
             rt_kprintf("can dev write data failed!\n");
         }
         rt_thread_delay(1000);
     }
 }

 int can_sample_receive()
 {
     rt_err_t res;
     char can_name[RT_NAME_MAX];
     rt_strncpy(can_name, CAN_DEV_NAME, RT_NAME_MAX);
     c_struct.can_flag=0;
     /* 查找 CAN 设备 */
     c_struct.can_dev = rt_device_find(can_name);
     if (!c_struct.can_dev)
     {
         rt_kprintf("find %s failed!\n", can_name);
         return RT_ERROR;
     }
     /* 初始化 CAN 接收信号量 */
     rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
     /* 以中断接收及发送方式打开 CAN 设备 */
     res = rt_device_open(c_struct.can_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
     RT_ASSERT(res == RT_EOK);
     /* 创建数据接收线程 */
     c_struct.can_thread = rt_thread_create("can_rx", can_rx_thread, RT_NULL, 1024, 25, 10);
     if (c_struct.can_thread != RT_NULL)
     {
         rt_thread_startup(c_struct.can_thread);
     }
     else
     {
         rt_kprintf("create can_rx thread failed!\n");
     }
     return res;
 }
 int can_sample_send()
 {
     rt_err_t res;
     rt_thread_t thread;
     char can_name[RT_NAME_MAX];
     rt_strncpy(can_name, CAN_DEV_NAME, RT_NAME_MAX);
     /* 查找 CAN 设备 */
     can_dev = rt_device_find(can_name);
     if (!can_dev)
     {
         rt_kprintf("find %s failed!\n", can_name);
         return RT_ERROR;
     }

     /* 以中断接收及发送方式打开 CAN 设备 */
     res = rt_device_open(can_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
     RT_ASSERT(res == RT_EOK);
     /* 创建数据接收线程 */
     thread = rt_thread_create("can_tx", can_tx_thread, RT_NULL, 1024, 25, 10);
     if (thread != RT_NULL)
     {
         rt_thread_startup(thread);
     }
     else
     {
         rt_kprintf("create can_rx thread failed!\n");
     }

     return res;
 }
 int can1_sample_receive()
 {
     rt_err_t res;
     char can_name[RT_NAME_MAX];
     rt_strncpy(can_name, CAN_DEV_NAME_1, RT_NAME_MAX);
     c_struct.can_flag1=0;
     /* 查找 CAN 设备 */
     c_struct.can1_dev = rt_device_find(can_name);
     if (!c_struct.can1_dev)
     {
         rt_kprintf("find %s failed!\n", can_name);
         return RT_ERROR;
     }
     /* 初始化 CAN 接收信号量 */
     rt_sem_init(&rx_sem1, "rx_sem1", 0, RT_IPC_FLAG_FIFO);
     /* 以中断接收及发送方式打开 CAN 设备 */
     res = rt_device_open(c_struct.can1_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
     RT_ASSERT(res == RT_EOK);
     /* 创建数据接收线程 */
     c_struct.can1_thread= rt_thread_create("can_rx1", can1_rx_thread, RT_NULL, 1024, 25, 10);
     if (c_struct.can1_thread != RT_NULL)
     {
         rt_thread_startup(c_struct.can1_thread);
     }
     else
     {
         rt_kprintf("create can_rx thread failed!\n");
     }
     return res;
 }
 void adc_init(void)
{
    c_struct.dev_adc_0 = (rt_adc_device_t)rt_device_find(DEV_ADC);
    c_struct.dev_adc_1 = (rt_adc_device_t)rt_device_find(DEV_ADC1);
    rt_uint32_t vol = 2048;
    if(c_struct.dev_adc_0 == RT_NULL&&c_struct.dev_adc_1 == RT_NULL)
    {
        rt_kprintf("no adc device named %s\n", DEV_ADC);
    }
}
rt_uint8_t adc_channel_test(rt_adc_device_t dev,rt_uint8_t channel)
{
       rt_uint32_t vol, value = 2048;
       c_struct.adc_flag=0;
       int count=0;
       rt_adc_enable(dev, channel);
       for (int i = 0; i < 8; i++)
       {
        value = rt_adc_read(dev, channel);
         if(value <10)
         {
              count++;
         }
        vol = value * REFER_VOLTAGE / CONVERT_BITS;
        rt_kprintf("vlo%d\r\n",value);
        rt_kprintf("the adc voltage is :%d.%02d \n", vol / 100, vol % 100);
        rt_thread_mdelay(200);
       }
       if(count >7)
           c_struct.adc_flag=1;
    rt_adc_disable(dev, channel);
}

void RS485_Send_Example( uint8_t ch )
 {
     /*串口写入函数*/
     R_SCI_UART_Write(&g_uart5_ctrl, (uint8_t*) &ch, 1);
 }

/*RS485_1中断回调函数*/
void rs485_callback(uart_callback_args_t * p_args)
{
    rt_interrupt_enter();
    c_struct.serial_flag=1;
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
