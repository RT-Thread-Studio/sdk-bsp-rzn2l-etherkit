/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-17    Jianjia Ma   first version
 */

#ifndef __DRV_RAMDISK_H_
#define __DRV_RAMDISK_H_

#include <rtthread.h>
#include "rtdevice.h"
#include <drv_common.h>

/* ramdisk */
struct ramdisk_device
{
    struct rt_device                parent;     /**< RT-Thread device struct */
    struct rt_device_blk_geometry   geometry;   /**< sector size, sector count */

    uint8_t* disk;                              /**< ramdisk start address */
    rt_size_t size;                             /**< size of the ramdisk */
    uint8_t is_allocated;                       /**< whether the disk buffer is allocated by us or user*/
};

#endif /*__DRV_RAMDISK_H_ */
