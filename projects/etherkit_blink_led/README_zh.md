# RZ EtherKit 开发板 RGB使用 说明

**中文** | [**English**](./README.md)

## 简介

本例程作为SDK 的第一个例程，也是最简单的例程，类似于程序员接触的第一个程序Hello World一样简洁。它的主要功能是让板载的RGB-LED 进行周期性闪烁。

## 硬件说明

![img](./figures/wps31.jpg) 

图2.1 LED电路原理图

![img](./figures/wps32.jpg) 

图2.2 LED引脚示意图

​	如上图所示，EtherKit提供三个用户LED，分别为LED0（RED）、LED1（BLUE）、LED2（GREEN），其中LED_RED对应引脚P14_3。单片机引脚输出低电平即可点亮LED ，输出高电平则会熄灭LED。

​	LED在开发板中的位置如下图所示：

![img](./figures/wps33.jpg) 

图2-3 LED位置

## 软件说明

​	本例程的源码位于/projects/etherkit_blink_led.

​	RGB-LED对应的单片机引脚定义及RGB变换源码可以通过查阅 src/hal_data.c中。

```
/* 配置 LED 灯引脚 */

#define LED_PIN_R   BSP_IO_PORT_14_PIN_3 /* Onboard RED LED pins */

#define LED_PIN_B   BSP_IO_PORT_14_PIN_0 /* Onboard BLUE LED pins */

#define LED_PIN_G   BSP_IO_PORT_14_PIN_1 /* Onboard GREEN LED pins */

  do

  {

​    /* 获得组编号 */

​    group_current = count % group_num;

 

​    /* 控制 RGB 灯 */

​    rt_pin_write(LED_PIN_R, _blink_tab[group_current][0]);

​    rt_pin_write(LED_PIN_B, _blink_tab[group_current][1]);

​    rt_pin_write(LED_PIN_G, _blink_tab[group_current][2]);

 

​    /* 输出 LOG 信息 */

​    LOG_D("group: %d | red led [%-3.3s] | | blue led [%-3.3s] | | green led [%-3.3s]",

​      group_current,

​      _blink_tab[group_current][0] == LED_ON ? "ON" : "OFF",

​      _blink_tab[group_current][1] == LED_ON ? "ON" : "OFF",

​      _blink_tab[group_current][2] == LED_ON ? "ON" : "OFF");

 

​    count++;

 

​    /* 延时一段时间 */

​    rt_thread_mdelay(500);

  }while(count > 0);
```



## 运行

## 编译&下载

l RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。

l IAR：首先双击mklinks.bat，生成rt-thread 与libraries 文件夹链接；再使用Env 生成IAR 工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

## 运行效果

按下复位按键重启开发板，观察开发板上RBG-LED的实际效果。正常运行后，RGB 会周期性变化，如下图所示：

![img](./figures/wps34.jpg) 

图2-4 RGB-LED演示

此时也可以在PC 端使用终端工具打开开发板的默认配置的串口，设置波特率为115200N。开发板的运行日志信息即可实时输出出来。

[D/main] group: 0 | red led [OFF] | | blue led [OFF] | | green led [OFF]

[D/main] group: 1 | red led [ON ] | | blue led [OFF] | | green led [OFF]

[D/main] group: 2 | red led [OFF] | | blue led [ON ] | | green led [OFF]

[D/main] group: 3 | red led [OFF] | | blue led [OFF] | | green led [ON ]

[D/main] group: 4 | red led [ON ] | | blue led [OFF] | | green led [ON ]

[D/main] group: 5 | red led [ON ] | | blue led [ON ] | | green led [OFF]

[D/main] group: 6 | red led [OFF] | | blue led [ON ] | | green led [ON ]

[D/main] group: 7 | red led [ON ] | | blue led [ON ] | | green led [ON ]

## 注意事项

​	暂无

## 引用参考

 设备与驱动：[PIN 设备](#/rt-thread-version/rt-thread-standard/programming-manual/device/pin/pin)
