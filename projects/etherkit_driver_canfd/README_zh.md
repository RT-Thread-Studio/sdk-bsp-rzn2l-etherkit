# RZ EtherKit 开发板 CANFD使用说明

**中文** | [**English**](./README.md)

## 简介

本例程主要介绍了如何在EtherKit上使用canfd设备;



## 硬件说明

![img](./figures/wps35.jpg) 

![img](./figures/wps36.jpg) 

 

## 软件说明

### FSP配置说明

![img](./figures/wps37.jpg) 

![img](./figures/wps38.jpg) 

### RT-Thread Settings配置

![img](./figures/wps39.jpg) 

### 工程示例说明

工程通过canfd0发送报文canfd1接受报文并将其使用串口打印

发送代码示例：

```
int can0_sample_send(int argc, char *argv[])

{

  rt_err_t res;

  rt_thread_t thread;

  char can_name[RT_NAME_MAX];

  if (argc == 2)

  {

​    rt_strncpy(can_name, argv[1], RT_NAME_MAX);

  }

  else

  {

​    rt_strncpy(can_name, CAN0_DEV_NAME, RT_NAME_MAX);

  }

  /* 查找 CAN 设备 */

  can0_dev = rt_device_find(can_name);

  if (!can0_dev)

  {

​    rt_kprintf("find %s failed!\n", can_name);

​    return RT_ERROR;

  }

  /* 以中断接收及发送方式打开 CAN 设备 */

  res = rt_device_open(can0_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);

  RT_ASSERT(res == RT_EOK);

  /* 创建数据接收线程 */

  thread = rt_thread_create("can0_tx", can0_tx_thread, RT_NULL, 1024, 25, 10);

  if (thread != RT_NULL)

  {

​    rt_thread_startup(thread);

  }

  else

  {

​    rt_kprintf("create can_rx thread failed!\n");

  }

  return res;

}

/* 导出到 msh 命令列表中 */

MSH_CMD_EXPORT(can0_sample_send, can device sample);

Canfd1接受示例

int can1_sample_receive(int argc, char *argv[])

{

  rt_err_t res;

  rt_thread_t thread;

  char can_name[RT_NAME_MAX];

  if (argc == 2)

  {

​    rt_strncpy(can_name, argv[1], RT_NAME_MAX);

  }

  else

  {

​    rt_strncpy(can_name, CAN1_DEV_NAME, RT_NAME_MAX);

  }

  /* 查找 CAN 设备 */

  can1_dev = rt_device_find(can_name);

  if (!can1_dev)

  {

​    rt_kprintf("find %s failed!\n", can_name);

​    return RT_ERROR;

  }

  /* 初始化 CAN 接收信号量 */

  rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

 

  /* 以中断接收及发送方式打开 CAN 设备 */

  res = rt_device_open(can1_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);

  RT_ASSERT(res == RT_EOK);

  /* 创建数据接收线程 */

  thread = rt_thread_create("can1_rx", can1_rx_thread, RT_NULL, 1024, 25, 10);

  if (thread != RT_NULL)

  {

​    rt_thread_startup(thread);

  }

  else

  {

​    rt_kprintf("create can_rx thread failed!\n");

  }

  return res;

}
```



## 运行

### 编译&下载

 RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。

IAR：首先双击mklinks.bat，生成rt-thread与libraries 文件夹链接；再使用Env 生成IAR工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

### 运行效果

使用串口分别发送can0_sample_send,can1_sample_receive命令

![image-20241125160014899](./figures/image-20241125160014899.png)

## 注意事项

​	暂无

##  引用参考

 设备与驱动：[CAN设备](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/can/can)
