#ifndef __TEST__
#define __TEST__
#include <rtthread.h>
#include <rtdevice.h>
#define SAMPLE_UART_NAME       "uart5"      /* 串口设备名称 */
#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     5           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */
 #define CAN_DEV_NAME            "canfd0"      /* CAN 设备名称 */
 #define CAN_DEV_NAME_1            "canfd1"      /* CAN 设备名称 */
#define DEV_ADC                 "adc0"
#define DEV_ADC1                 "adc1"
/***
 * LED CODE  PIN
 * 
 */
#define ETH_LED_PIN_0    BSP_IO_PORT_20_PIN_2 /* Onboard LED pins */
#define ETH_LED_PIN_1    BSP_IO_PORT_22_PIN_3 /* Onboard LED pins */
#define ETH_LED_PIN_2    BSP_IO_PORT_20_PIN_1 /* Onboard LED pins */
#define ETH_LED_PIN_3    BSP_IO_PORT_20_PIN_4 /* Onboard LED pins */
#define ETH_LED_PIN_4    BSP_IO_PORT_03_PIN_0 /* Onboard LED pins */
#define ETH_LED_PIN_5    BSP_IO_PORT_22_PIN_1 /* Onboard LED pins */
#define ETH_LED_PIN_6    BSP_IO_PORT_04_PIN_4 /* Onboard LED pins */
#define ETH_LED_PIN_7    BSP_IO_PORT_05_PIN_0 /* Onboard LED pins */

#define DEV_ADC_CHANNEL_0         0
#define DEV_ADC_CHANNEL_1         1
#define REFER_VOLTAGE           180
#define CONVERT_BITS            (1 << 12)
struct commom_struct
{
/* 串口接收消息结构 */
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
rt_adc_device_t dev_adc_0;

rt_adc_device_t dev_adc_1;

/* 串口设备句柄 */
 rt_device_t serial;
/* 消息队列控制块 */
 struct rt_messagequeue rx_mq;

 struct rt_semaphore rx_sem;     /* 用于接收消息的信号量 */
   rt_device_t can_dev;            /* CAN 设备句柄 */
   rt_device_t can1_dev;
   rt_thread_t serial_thread;
   rt_uint8_t  serial_flag;
   rt_uint8_t  can_flag;
   rt_uint8_t  can_flag1;
   rt_uint8_t  adc_flag;
   rt_uint8_t  led_flag;
   rt_thread_t can_thread;
   rt_thread_t can1_thread;
};
extern struct commom_struct c_struct;
rt_uint8_t adc_channel_test(rt_adc_device_t dev,rt_uint8_t channel);
void adc_init(void);
 int uart_dma_sample();
 int can_sample_receive();
 int can1_sample_receive();
 int can_sample_send();
 void RS485_Send_Example( uint8_t ch );
#endif
