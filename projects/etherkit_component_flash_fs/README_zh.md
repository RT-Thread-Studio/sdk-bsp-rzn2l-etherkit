# Filesystem 例程

**中文** | [**English**](./README.md)

## 简介

FAL (Flash Abstraction Layer) Flash 抽象层，是对 Flash 及基于 Flash 的分区进行管理、操作的抽象层，对上层统一了 Flash 及 分区操作的 API (框架图如下所示)，并具有以下特性：

- 支持静态可配置的分区表，并可关联多个 Flash 设备；
- 分区表支持 **自动装载** 。避免在多固件项目，分区表被多次定义的问题；
- 代码精简，对操作系统 **无依赖** ，可运行于裸机平台，比如对资源有一定要求的 Bootloader；
- 统一的操作接口。保证了文件系统、OTA、NVM（例如：[EasyFlash](https://github.com/armink-rtt-pkgs/EasyFlash)） 等对 Flash 有一定依赖的组件，底层 Flash 驱动的可重用性；
- 自带基于 Finsh/MSH 的测试命令，可以通过 Shell 按字节寻址的方式操作（读写擦） Flash 或分区，方便开发者进行调试、测试；

![FAL framework](figures/fal_framework.png)

本示例中将使用 EtherKit 板载 GD Flash 结合 RT-Thread FAL 组件，使用 littlefs 构建文件系统。

## 硬件说明

![image-20250421131242258](figures/image-20250421131242258.png)

## 软件说明

本例程的文件系统初始化源码位于 ../board/ports/filesystem/drv_filesystem.c 中：

```c
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author                Notes
 * 2025-02-07     newflydd@gmail.com    the first version
 */
#define DBG_TAG "drv.fs"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include <rtthread.h>
#include <fal.h>
#include <dfs_fs.h>

int initFileSystem()
{
    fal_init();

    // register onchip flash tail area as littlefs
    struct rt_device* flashDev = fal_mtd_nor_device_create("param");
    if (RT_NULL == flashDev)
    {
        LOG_W("create fal device failed");
        return RT_ERROR;
    }

    if (RT_EOK != dfs_mount("param", "/", "lfs", 0, RT_NULL))
    {
        LOG_W("mount lfs failed once, try to format it");
        if (RT_EOK != dfs_mkfs("lfs", "param"))
        {
            LOG_W("mkfs lfs failed");
            return RT_ERROR;
        }
        LOG_I("mkfs lfs success");

        if (RT_EOK != dfs_mount("param", "/", "lfs", 0, RT_NULL))
        {
            LOG_W("mount lfs failed");
            return RT_ERROR;
        }
    }
    LOG_I("mount lfs success");
    return RT_EOK;
}
INIT_ENV_EXPORT(initFileSystem);
```

## 编译&下载

* RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。

* IAR：首先双击mklinks.bat，生成rt-thread 与libraries 文件夹链接；再使用Env 生成IAR 工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

## 运行效果

按下复位按键重启开发板，观察开发板终端日志。

![image-20250421131424928](figures/image-20250421131424928.png)

执行以下指令开始 FAL 读写测试：

```c
> fal probe param param
> fal bench 4096 yes  
```

