# Etherkit  ADC应用示例说明

**中文** | [**English**](./README.md)

## 简介

本例程主要介绍了如何在EtherKit上使用rtthread的ADC框架完成通过ADC 采集模拟信号并进行数字信号的转换；主要内容如下：

## 硬件说明

![image-20241121103911617](./figures/image-20241121103911617.png)

如上述原理图所示：EtherKit上留有Analog  Input 8通道接口分别连接到单片机的adc0、adc1的通道0、1、2、3；（注意，Analog Input的耐压范围为0~1.8v）；

##  软件说明

## FSP配置说明

第一步：打开FSP导入xml配置文件；（或者直接点击Rtthread Studio 的FSP链接文件）；

第二步：新建r_adc Stack 配置adc设备以及所用通道

![image-20241121103918629](./figures/image-20241121103918629.png)

第三步：保存并点击Generate Project;生成的代码保存到hal_data.c中；

![image-20241121104129821](./figures/image-20241121104129821.png)

本例程的源码位于/projects/etherkit_basic_key。

## env配置

使用env工具打开adc0的外设

![image-20241121103936802](./figures/image-20241121103936802.png)

## 工程实例说明

ADC的源代码位于/projects/etherkit_driver_adc/src/hal_entry.c 中

使用的宏定义如下所示：

![image-20241121103952259](./figures/image-20241121103952259.png)

具体功能为每隔1000ms对ADC0的通道0采集一次模拟电压并进行一次转化；

具体代码如下：

```
static int adc_vol_sample()

{

  rt_adc_device_t adc_dev;

  rt_uint32_t value, vol;

  rt_err_t ret = RT_EOK;

  /* 查找设备 */

  adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);

  if (adc_dev == RT_NULL)

  {

​    rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);

​    return RT_ERROR;

  }

  /* 使能设备 */

  ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);

  /* 读取采样值 */

  value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);

  rt_kprintf("the value is :%d \n", value);

  /* 转换为对应电压值 */

  vol = value * REFER_VOLTAGE / CONVERT_BITS;

  rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);

  /* 关闭通道 */

  ret = rt_adc_disable(adc_dev, ADC_DEV_CHANNEL);

  return ret;

}
```



 

实例中While循环每隔1000ms调用一次adc_vol_sample;

 

## 运行

### 编译&下载

l RT-Thread Studio：在RT-Thread Studio 的包管理器中下载EtherKit 资源包，然后创建新工程，执行编译。

l IAR：首先双击mklinks.bat，生成rt-thread与libraries 文件夹链接；再使用Env 生成IAR工程；最后双击project.eww打开IAR工程，执行编译。

编译完成后，将开发板的Jlink接口与PC 机连接，然后将固件下载至开发板。

### 运行效果

使用adc0 的0通道采集1.8v电压效果如下：

![image-20241125155851070](./figures/image-20241125155851070.png)



## 注意事项

​	R9A07G084M08GBG 芯片的ADC采集电压耐压为1.8v!

## 引用参考

设备与驱动：[ADC 设备](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/adc/adc)

 
