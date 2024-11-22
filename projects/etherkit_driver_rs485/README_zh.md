# RZ EtherKit 开发板 RS485使用 说明

**中文** | [**English**](./README.md)

## 简介

本例程主要介绍了如何在EtherKit上使用RS485设备;

## 硬件说明

![img](./figures/wps41.jpg) 

 

## 软件说明

### FSP配置说明

打开FSP工具 新建Stacks选择r_sci_uart5

### RT-Thread Settings配置

无

### 工程示例说明

工程使用FSP库函数开发；

## 运行

##  编译&下载

RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。

IAR：首先双击mklinks.bat，生成rt-thread与libraries 文件夹链接；再使用Env 生成IAR工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

### 运行效果

串口输出指令rs485_send指令 打开另一个串口的终端查看收到的数据

![image-20241122171605909](./figures/image-20241122171605909.png)

## 注意事项

​	暂无

## 引用参考

 设备与驱动：[UART_V2 设备](#/rt-thread-version/rt-thread-standard/programming-manual/device/uart/uart_v2/uart)
