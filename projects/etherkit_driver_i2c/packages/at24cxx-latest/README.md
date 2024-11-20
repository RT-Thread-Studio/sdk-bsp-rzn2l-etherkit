# AT24CXX 软件包

## 1 介绍

AT24CXX 软件包提供了at24cxx 系列 EEPROM 基本功能。本文介绍该软件包的基本读写功能，以及 `Finsh/MSH` 测试命令等。
目前已在 at24c02, at24c512验证通过。

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| at24cxx.h | EEPROM 使用头文件 |
| at24cxx.c | EEPROM 使用源代码 |
| SConscript | RT-Thread 默认的构建脚本 |
| README.md | 软件包使用说明 |
| at24cxx_datasheet.pdf | 官方数据手册 |

### 1.2 许可证

AT24CXX 软件包遵循  Apache-2.0 许可，详见 LICENSE 文件。

### 1.3 依赖

依赖 `RT-Thread I2C` 设备驱动框架。

## 2 获取软件包

使用 `at24cxx` 软件包需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers  --->
        [*] at24cxx: eeprom at24cxx driver library  --->
            Version (latest)  --->
```


每个功能的配置说明如下：

- `at24cxx: ee2prom at24cxx driver library`：选择使用 `at24cxx` 软件包；
- `Version`：配置软件包版本，默认最新版本。

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3 使用 at24cxx 软件包

按照前文介绍，获取 `at24cxx` 软件包后，就可以按照 下文提供的 API 使用 ee2prom `at24cxx 与 `Finsh/MSH` 命令进行测试，详细内容如下。

### 3.1 API

#### 3.1.1  初始化 

`at24cxx_device_t at24cxx_init(const char *i2c_bus_name, uint8_t addr)

根据总线名称，自动初始化对应的 AT24CXX 设备，具体参数与返回说明如下表

| 参数    | 描述                      |
| :----- | :----------------------- |
| name   | i2c 设备名称 |
| addr   | 地址         |
| **返回** | **描述** |
| != NULL | 将返回 at24cxx 设备对象 |
| = NULL | 查找失败 |

#### 3.1.2  反初始化

void at24cxx_deinit(aht10_device_t dev)

如果设备不再使用，反初始化将回收 at24cxx 设备的相关资源，具体参数说明如下表

| 参数 | 描述           |
| :--- | :------------- |
| dev  | at24cxx 设备对象 |

#### 3.1.3 读取 

rt_err_t at24cxx_read(struct rt_i2c_bus_device *bus,uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)

通过 `at24cxx` 读取 eeprom ，在AT24CXX里面的指定地址开始读出指定个数的数据，具体参数与返回说明如下表

| 参数     | 描述           |
| :------- | :------------- |
| bus      | at24cxx 设备对象 |
| **返回** | **描述**       |
| != RT_EOK | 读取成功     |
| = RT_EOK| 读取失败       |

#### 3.1.4 写入

rt_err_t at24cxx_write(struct rt_i2c_bus_device *bus,uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)


通过 `at24cxx` 写入，在AT24CXX里面的指定地址开始写入指定个数的数据，具体参数与返回说明如下表

| 参数     | 描述          |
| :------- | :------------ |
| bus      | at24cxx 设备对象 |
| **返回** | **描述**      |
| != RT_EOK  | 写入失败    |
| =RT_EOK    | 写入成功      |

#### 3.1.5 查看设备

rt_err_t at24cxx_check(struct rt_i2c_bus_device *bus)

查看是否存在eeprom 设备，通过在最后一个字节写入标志位，进行判断，具体参数与返回说明如下表

| 参数     | 描述          |
| :------- | :------------ |
| bus      | at24cxx 设备对象 |
| **返回** | **描述**      |
| != RT_EOK  | 设备不存在    |
| =RT_EOK    | 设备存在      |


### 3.2 Finsh/MSH 测试命令

at24cxx 软件包提供了丰富的测试命令，项目只要在 RT-Thread 上开启 Finsh/MSH 功能即可。在做一些基于 `at24cxx` 的应用开发、调试时，这些命令会非常实用，它可以准确的读取指传感器测量的温度与湿度。具体功能可以输入 `at24cxx` ，可以查看完整的命令列表

```
msh />at24cxx
Usage:
at24cxx probe <dev_name>   - probe sensor by given name
at24cxx check              - check device
at24cxx write              - write  data
at24cxx read               - read   data
msh />
```

#### 3.2.1 在指定的 i2c 总线上探测传感器 

当第一次使用 `at24cxx` 命令时，直接输入 `at24cxx probe <dev_name>` ，其中 `<dev_name>` 为指定的 i2c 总线，例如：i2c0。如果有这个设备，就不会提示错误；如果总线上没有这个设备，将会显示提示找不到相关设备，日志如下：

```
msh />at24cxx probe i2c1      #探测成功，没有错误日志
msh />
msh />at24cxx probe i2c88     #探测失败，提示对应的 I2C 设备找不到
[E/aht10] can't find at24cxx device on 'i2c88'
msh />
```

## 4 注意事项

- 请在at24cxx.h中修改EE_TYPE为自己使用的型号(默认为AT25C512) 。
- 请在at24cxx.h中修改EE_TWR为自己使用EEPROM的Write Cycle Time，具体值请查看芯片datasheet(默认为5ms) 。
- 从设备地址为7位地址 0x50, 而不是 0xA0 。
- 跨区域操作，比如 AT24C04的数据空间由P0位决定，当P0为“0”时，将对AT24C04的0 ~ 255空间的数据进行操作，当P0为“1”时，将对AT24C04的256~511空间的数据进行操作，需要注意变更AddrInput的值。

## 5 联系方式

* 维护：[XiaojieFan](https://github.com/XiaojieFan)
* 主页：https://github.com/XiaojieFan/at24cxx

