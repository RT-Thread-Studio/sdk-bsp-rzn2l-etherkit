# EtherKit USB-PMSC例程

**中文** | [**English**](./README.md)

## 简介

本例程展示了通过USB方式实现U盘的示例。USB-PMSC（USB Peripheral Mass Storage Class）是一种通用串行总线（USB）设备类，用于实现基于USB的存储设备功能。

## 硬件说明

![image-20241126111451833](figures/image-20241126111451833.png)

EtherKit提供一个USB-Device外设，位于开发板的位置如下所示：

![image-20241126111503789](figures/image-20241126111503789.png)

## FSP配置说明

* 使用fsp打开工程下的configuration.xml文件，并添加usb_pmcs stack；

![image-20241126111522109](figures/image-20241126111522109.png)

* 添加g_rm_block_media0；

![image-20241126111550254](figures/image-20241126111550254.png)

* 选择g_basic0_usb，设置其中断回调函数为usb_apl_callback：

![image-20241126111602558](figures/image-20241126111602558.png)

* 下面配置USB引脚，找到USB_HS并使能：

![image-20241126111620891](figures/image-20241126111620891.png)

## 构建配置

进入工程找到指定路径下的文件：.\rzn\SConscript，替换该文件为如下内容：

```c
Import('RTT_ROOT')
Import('rtconfig')
from building import *
from gcc import *

cwd = GetCurrentDir()
src = []
group = []
CPPPATH = []

if rtconfig.PLATFORM in ['iccarm'] + GetGCCLikePLATFORM():
    if rtconfig.PLATFORM == 'iccarm' or GetOption('target') != 'mdk5':
        src += Glob('./fsp/src/bsp/mcu/all/*.c')
        src += Glob('./fsp/src/bsp/mcu/all/cr/*.c')
        src += Glob('./fsp/src/bsp/mcu/r*/*.c')
        src += Glob('./fsp/src/bsp/cmsis/Device/RENESAS/Source/*.c')
        src += Glob('./fsp/src/bsp/cmsis/Device/RENESAS/Source/cr/*.c')
        src += Glob('./fsp/src/r_*/*.c')
        src += Glob('./fsp/src/r_usb_basic/src/driver/*.c')
        src += Glob('./fsp/src/r_usb_basic/src/hw/*.c')
        src += Glob('./fsp/src/r_usb_pmsc/src/*.c')
        CPPPATH = [ cwd + '/arm/CMSIS_5/CMSIS/Core_R/Include',
                            cwd + '/fsp/inc',
                            cwd + '/fsp/src/inc',
                            cwd + '/fsp/inc/api',
                            cwd + '/fsp/inc/instances',
                            cwd + '/fsp/src/r_usb_basic/src/driver/inc',
                            cwd + '/fsp/src/r_usb_basic/src/hw/inc',
                            cwd + '/fsp/src/r_usb_pmsc/src/inc',]

group = DefineGroup('rzn', src, depend = [''], CPPPATH = CPPPATH)
Return('group')
```

使用studio开发的话需要右键工程点击 **同步scons配置至项目**；如果是使用IAR开发请在当前工程下右键打开env，执行：scons –target=iar 重新生成配置。

## RT-Thread Settings配置

USB示例目前使用的是freertos接口驱动，因此我们还需要使能Freertos兼容层软件包；

![image-20241126111643871](figures/image-20241126111643871.png)

## 编译&下载

* RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。
* IAR：首先双击mklinks.bat，生成rt-thread与libraries 文件夹链接；再使用Env 生成IAR工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

## 运行效果

生成FSP配置之后进行编译下载，将代码烧录到开发板即可自动启动该示例，同时在文件管理器中可以发现多出了一个U盘设备；

![image-20241126111718060](figures/image-20241126111718060.png)

![image-20241126111722825](figures/image-20241126111722825.png)
