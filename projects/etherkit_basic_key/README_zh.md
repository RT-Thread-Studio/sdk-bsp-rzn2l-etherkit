# RZ EtherKit 开发板 KEY使用 说明

**中文** | [**English**](./README.md)

## 简介

​	本例程主要功能是通过板载的按键KEY控制RGB-LED 中RGB的亮灭。

## 硬件说明

![img](./figures/wps23.jpg) 

图3-1 key电路原理图

![img](./figures/wps24.jpg) 

图3-2 key引脚示意图

如上图所示， KEY1(LEFT)、KEY2(RIGHT)引脚分别连接单片机P14_2(LEFT)和P16_3(RIGHT)引脚, KEY按键按下为高电平，松开为低电平。

KEY 在开发板中的位置如下图所示：

![img](./figures/wps25.jpg) 

图3-3 按键位置

## 软件说明

本例程的源码位于/projects/etherkit_basic_key。

KEY1(LEFT) 、KEY2(RIGHT)对应的单片机引脚定义如下。

/* 配置 KEY 输入引脚  */

\#define PIN_KEY1     BSP_IO_PORT_14_PIN_2    // P14_2 :  KEY1

\#define PIN_KEY2     BSP_IO_PORT_16_PIN_3    // P16_3 :  KEY2

 

按键输入的源代码位于/projects/etherkit_basic_key/src/hal_entry.c 中。首先为了实验效果清晰可见，板载RGB 蓝色LED 作为KEY1(LEFT) 的状态指示灯，板载RGB 绿色LED 作为KEY2(RIGHT) 的状态指示灯，设置RGB 红灯引脚的模式为输出模式，然后设置按键引脚为输入模式，最后在while 循环中通过rt_pin_read(PIN_KEY) 判断KEY的电平状态，并作50ms的消抖处理，如果成功判断KEY为低电平状态（即按键按下）则打印输出“KEYx pressed!” 并且指示灯亮，否则指示灯熄灭。

```
 unsigned int count = 1;

  /* 设置 RGB 绿灯引脚的模式为输出模式 */

  rt_pin_mode(LED_PIN_B, PIN_MODE_OUTPUT);

  rt_pin_mode(LED_PIN_G, PIN_MODE_OUTPUT);

  /* 设置 KEY 引脚的模式为输入上拉模式 */

  rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);

  rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);

  while (count > 0)

  {

​    /* 读取按键 KEY 的引脚状态 */

​    if (rt_pin_read(PIN_KEY1) == PIN_LOW)

​    {

​      rt_thread_mdelay(50);

​      if (rt_pin_read(PIN_KEY1) == PIN_LOW)

​      {

​        /* 按键已被按下，输出 log，点亮 LED 灯 */

​        LOG_D("KEY1 pressed!");

​        rt_pin_write(LED_PIN_B, PIN_LOW);

​      }

​    }

​    else if(rt_pin_read(PIN_KEY2) == PIN_LOW)

​    {

​      rt_thread_mdelay(50);

​      if (rt_pin_read(PIN_KEY2) == PIN_LOW)

​      {

​        /* 按键已被按下，输出 log，点亮 LED 灯 */

​        LOG_D("KEY2 pressed!");

​        rt_pin_write(LED_PIN_G, PIN_LOW);

​      }

​    }

​    else

​    {

​      /* 按键没被按下，熄灭 LED 灯 */

​      rt_pin_write(LED_PIN_B, PIN_HIGH);

​      rt_pin_write(LED_PIN_G, PIN_HIGH);

​    }

​    rt_thread_mdelay(10);

​    count++;

  }
```

 

## 运行

### 编译&下载

l RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。

l IAR：首先双击mklinks.bat，生成rt-thread 与libraries 文件夹链接；再使用Env 生成IAR 工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

### 运行效果

按下复位按键重启开发板，初始状态下的LED1和LED2出于灭灯状态，当按下KEY1时，LED1(Blue)亮起；当按下KEY2时，LED2(Green)亮起。

![img](./figures/wps26.jpg) 

## 注意事项

​	暂无

## 引用参考

设备与驱动：[PIN 设备](#/rt-thread-version/rt-thread-standard/programming-manual/device/pin/pin)
