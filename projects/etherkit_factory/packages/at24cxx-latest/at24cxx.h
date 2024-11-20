/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-13     XiaojieFan   the first version
 * 2019-12-04     RenMing      Use PAGE WRITE instead of BYTE WRITE and input address can be selected
 */

#ifndef __AT24CXX_H__
#define __AT24CXX_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define AT24C01     0
#define AT24C02     1
#define AT24C04     2
#define AT24C08     3
#define AT24C16     4
#define AT24C32     5
#define AT24C64     6
#define AT24C128    7
#define AT24C256    8
#define AT24C512    9
#define AT24CTYPE   10   // Number of supported types

#define EE_TWR      4

#ifndef EE_TYPE
#define EE_TYPE     AT24C16
#endif

struct at24cxx_device
{
    struct rt_i2c_bus_device *i2c;
    rt_mutex_t lock;
    uint8_t AddrInput;
};
typedef struct at24cxx_device *at24cxx_device_t;

extern at24cxx_device_t at24cxx_init(const char *i2c_bus_name, uint8_t AddrInput);
extern rt_err_t at24cxx_read(at24cxx_device_t dev, uint32_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead);
extern rt_err_t at24cxx_write(at24cxx_device_t dev, uint32_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite);
extern rt_err_t at24cxx_page_read(at24cxx_device_t dev, uint32_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead);
extern rt_err_t at24cxx_page_write(at24cxx_device_t dev, uint32_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite);
rt_uint8_t at24cxx();
#endif
