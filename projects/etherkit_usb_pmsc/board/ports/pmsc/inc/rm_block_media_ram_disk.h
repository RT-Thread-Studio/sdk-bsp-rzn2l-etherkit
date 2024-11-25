/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2014(2020) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_ram_disk.h
 * Description  : Header file for RAM-disk media driver.
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 30.09.2017 1.22 Move RAM DISK macro & global variables.
 *         : 01.03.2020 1.30 RX72N/RX66N is added and uITRON is supported.
 ***********************************************************************************************************************/

#ifndef RAM_DISK_H
#define RAM_DISK_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Fixed width integers. */
#include <stdint.h>
#include "r_usb_basic_cfg.h"

#define STRG_MEDIASIZE                 (64UL * 1024UL) //(64UL * 1024UL) /* Media size */
#define STRG_TOTALSECT                 (STRG_MEDIASIZE / STRG_SECTSIZE)

#if STRG_TOTALSECT < 4096
    #define RAMDISK_FATLENGTH           (0x155ul)     /* FAT12 */
#else   /* STRG_TOTALSECT < 4096 */
    #if STRG_TOTALSECT < 65536
        #define RAMDISK_FATLENGTH       (0x100ul)     /* FAT16 */
    #else   /* STRG_TOTALSECT < 65536 */
        #define RAMDISK_FATLENGTH       (0x80ul)     /* FAT32 */
    #endif  /* STRG_TOTALSECT < 65536 */
#endif  /* STRG_TOTALSECT < 4096 */

#define RAMDISK_FATSIZE   (((STRG_TOTALSECT - 8) / RAMDISK_FATLENGTH) + 1)


/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 ************************************************************************************************************************/

extern const uint8_t g_ram_disk_boot_sector[];
extern const uint8_t g_usb_pmsc_table1[];
extern const uint8_t g_usb_pmsc_tablefat1[];
extern const uint8_t g_usb_pmsc_tablefat2[];
extern const uint8_t g_usb_pmsc_rootdir[];

#endif /* RAM_DISK_H */

/***********************************************************************************************************************
End  Of File
***********************************************************************************************************************/
