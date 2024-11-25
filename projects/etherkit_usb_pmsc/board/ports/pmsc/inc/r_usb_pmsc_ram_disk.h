/* ${REA_DISCLAIMER_PLACEHOLDER} */

#ifndef RAM_DISK_H
#define RAM_DISK_H

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/

/* Fixed width integers. */
#include <stdint.h>

/* RAM disk configuration */
#include "r_usb_basic_cfg.h"

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
//#define USB_MEDIA_ADDRESS      (0x30000000UL) //(0x20008000UL)  /* Media Head Address */
//#define STRG_MEDIASIZE         (64UL * 1024UL) /* Media size */
//#define STRG_TOTALSECT         (STRG_MEDIASIZE / STRG_SECTSIZE)

/* #define RAMDISK_MEDIATYPE              (0xF8u)    Fixed media
 * #define RAMDISK_MEDIATYPE              (0xF0u)    Removable media */
#define RAMDISK_MEDIATYPE      (0xF8U)

#define RAMDISK_SIGNATURE      (0xAA55U)

#define RAMDISK_CLSTSIZE       (0x01U)
#define RAMDISK_FATNUM         (0x02U)

/*
 * If the number of data areas of clusters is smaller
 * than that of value 4085(4096-11), it is FAT12.
 * If the number of data areas of clusters is smaller
 * than that of value 65525(65536-11), it is FAT16.
 * Otherwise it is FAT32.
 */

#if STRG_TOTALSECT < 4096
 #define RAMDISK_FATLENGTH     (0x155UL) /* FAT12 */
#else                                    /* STRG_TOTALSECT < 4096 */
 #if STRG_TOTALSECT < 65536
  #define RAMDISK_FATLENGTH    (0x100UL) /* FAT16 */
 #else                                   /* STRG_TOTALSECT < 65536 */
  #define RAMDISK_FATLENGTH    (0x80UL)  /* FAT32 */
 #endif                                  /* STRG_TOTALSECT < 65536 */
#endif  /* STRG_TOTALSECT < 4096 */

#define RAMDISK_FATSIZE        (((STRG_TOTALSECT - 8) / RAMDISK_FATLENGTH) + 1)

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 ************************************************************************************************************************/

//extern const usb_media_driver_t g_ram_mediadriver;
extern const uint8_t g_ram_disk_boot_sector[];
extern const uint8_t g_usb_pmsc_table1[];
extern const uint8_t g_usb_pmsc_rootdir[];

#endif                                 /* RAM_DISK_H */
