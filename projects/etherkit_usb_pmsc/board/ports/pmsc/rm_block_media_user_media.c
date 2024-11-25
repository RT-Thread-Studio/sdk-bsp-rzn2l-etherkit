/* ${REA_DISCLAIMER_PLACEHOLDER} */

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "hal_data.h"
#include "r_usb_pmsc_driver.h"
#include "inc/rm_block_media_ram_disk.h"
#include "rm_block_media_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* "MESD" in ASCII. */
#define RM_BLOCK_MEDIA_SDMMC_OPEN                (0x4D454D53)

#define RM_BLOCK_MEDIA_SDMMC_PRV_SD_R1_ERRORS    (0xFDF98008U)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
//void rm_block_media_sdmmc_callback(sdmmc_callback_args_t * p_args);
fsp_err_t RM_BLOCK_MEDIA_RAM_Open (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_cfg_t const * const p_cfg);
fsp_err_t RM_BLOCK_MEDIA_RAM_MediaInit (rm_block_media_ctrl_t * const p_ctrl);
fsp_err_t RM_BLOCK_MEDIA_RAM_Read (rm_block_media_ctrl_t * const p_ctrl,
                                     uint8_t * const               p_dest_address,
                                     uint32_t const                block_address,
                                     uint32_t const                num_blocks);
fsp_err_t RM_BLOCK_MEDIA_RAM_Write (rm_block_media_ctrl_t * const p_ctrl,
                                      uint8_t const * const         p_src_address,
                                      uint32_t const                block_address,
                                      uint32_t const                num_blocks);
fsp_err_t RM_BLOCK_MEDIA_RAM_Erase (rm_block_media_ctrl_t * const p_ctrl,
                                      uint32_t const                block_address,
                                      uint32_t const                num_blocks);
fsp_err_t RM_BLOCK_MEDIA_RAM_StatusGet (rm_block_media_ctrl_t * const   p_api_ctrl,
                                          rm_block_media_status_t * const p_status);
fsp_err_t RM_BLOCK_MEDIA_RAM_InfoGet (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_info_t * const p_info);
fsp_err_t RM_BLOCK_MEDIA_RAM_Close (rm_block_media_ctrl_t * const p_ctrl);

extern volatile bool g_blockmedia_complete_event;

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static uint8_t g_usb_pmsc_ram_disk[STRG_MEDIASIZE] USB_BUFFER_PLACE_IN_SECTION;  //yama

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

const rm_block_media_api_t g_rm_block_media_on_user_media =
{
    .open       = RM_BLOCK_MEDIA_RAM_Open,
    .mediaInit  = RM_BLOCK_MEDIA_RAM_MediaInit,
    .read       = RM_BLOCK_MEDIA_RAM_Read,
    .write      = RM_BLOCK_MEDIA_RAM_Write,
    .erase      = RM_BLOCK_MEDIA_RAM_Erase,
    .infoGet    = RM_BLOCK_MEDIA_RAM_InfoGet,
    .statusGet  = RM_BLOCK_MEDIA_RAM_StatusGet,
    .close      = RM_BLOCK_MEDIA_RAM_Close,
};

/*******************************************************************************************************************//**
 * @addtogroup RM_BLOCK_MEDIA_SDMMC
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Opens the module.
 *
 * Implements @ref rm_block_media_api_t::open().
 *
 * @retval     FSP_SUCCESS                     Module is available and is now open.
 * @retval     FSP_ERR_ASSERTION               An input parameter is invalid.
 * @retval     FSP_ERR_ALREADY_OPEN            Module has already been opened with this instance of the control
 *                                             structure.
 *
 * @return See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes.
 *         This function calls:
 *             * @ref sdmmc_api_t::open
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Open (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_cfg_t const * const p_cfg)
{
    uint32_t adr;
    uint32_t adr2;

    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);

    adr  = (uint32_t)g_usb_pmsc_ram_disk;
    adr2 = (uint32_t)g_usb_pmsc_ram_disk;
    memcpy((void *)adr, (void *)&g_ram_disk_boot_sector, STRG_SECTSIZE);

    adr += (STRG_SECTSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_table1, STRG_SECTSIZE);

    adr += (STRG_SECTSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_tablefat1, (STRG_SECTSIZE * RAMDISK_FATSIZE));

    adr += (STRG_SECTSIZE * RAMDISK_FATSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_tablefat2, (STRG_SECTSIZE * RAMDISK_FATSIZE));

    adr += (STRG_SECTSIZE * RAMDISK_FATSIZE);
    memcpy((void *)adr, (void *)&g_usb_pmsc_rootdir, (STRG_SECTSIZE * 16ul));

    adr += (STRG_SECTSIZE * 16ul);
    memset((void *)adr, 0, (STRG_MEDIASIZE - (adr - adr2)));


    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Initializes the SD or eMMC device.  This procedure requires several sequential commands.  This function blocks
 * until all identification and configuration commands are complete.
 *
 * Implements @ref rm_block_media_api_t::mediaInit().
 *
 * @retval     FSP_SUCCESS               Module is initialized and ready to access the memory device.
 * @retval     FSP_ERR_ASSERTION         An input parameter is invalid.
 * @retval     FSP_ERR_NOT_OPEN          Module is not open.
 *
 * @return See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes.
 *         This function calls:
 *             * @ref sdmmc_api_t::mediaInit
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_MediaInit (rm_block_media_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Reads data from an SD or eMMC device.  Up to 0x10000 sectors can be read at a time. Implements
 * @ref rm_block_media_api_t::read().
 *
 * This function blocks until the data is read into the destination buffer.
 *
 * @retval     FSP_SUCCESS                   Data read successfully.
 * @retval     FSP_ERR_ASSERTION             An input parameter is invalid.
 * @retval     FSP_ERR_NOT_OPEN              Module is not open.
 * @retval     FSP_ERR_NOT_INITIALIZED       Module has not been initialized.
 *
 * @return See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes.
 *         This function calls:
 *             * @ref sdmmc_api_t::read
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Read (rm_block_media_ctrl_t * const p_ctrl,
                                     uint8_t * const               p_dest_address,
                                     uint32_t const                block_address,
                                     uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    /* Copy 1 block from specified ram disk block address to read_buffer. */
//    memcpy(p_dest_address, (void *)(USB_MEDIA_ADDRESS + (block_address * STRG_SECTSIZE)), (STRG_SECTSIZE * num_blocks));
    memcpy(p_dest_address, (void *)(((uint32_t)g_usb_pmsc_ram_disk) + (block_address * STRG_SECTSIZE)), (STRG_SECTSIZE * num_blocks));  //yama

    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Writes data to an SD or eMMC device. Up to 0x10000 sectors can be written at a time. Implements
 * @ref rm_block_media_api_t::write().
 *
 * This function blocks until the write operation completes.
 *
 * @retval     FSP_SUCCESS                   Write finished successfully.
 * @retval     FSP_ERR_ASSERTION             An input parameter is invalid.
 * @retval     FSP_ERR_NOT_OPEN              Module is not open.
 * @retval     FSP_ERR_NOT_INITIALIZED       Module has not been initialized.
 *
 * @return See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes.
 *         This function calls:
 *             * @ref sdmmc_api_t::write
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Write (rm_block_media_ctrl_t * const p_ctrl,
                                      uint8_t const * const         p_src_address,
                                      uint32_t const                block_address,
                                      uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    /* Copy 1 block from write_buffer to appropriate block address in ram disk. */
//    memcpy((void *)(USB_MEDIA_ADDRESS + (block_address * STRG_SECTSIZE)), p_src_address, (STRG_SECTSIZE * num_blocks));
    memcpy((void *)(((uint32_t)g_usb_pmsc_ram_disk) + (block_address * STRG_SECTSIZE)), p_src_address, (STRG_SECTSIZE * num_blocks));  //yama

    g_blockmedia_complete_event = true;
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Erases sectors of an SD card or eMMC device.  Implements @ref rm_block_media_api_t::erase().
 *
 * This function blocks until erase is complete.
 *
 * @retval     FSP_SUCCESS                   Erase operation requested.
 * @retval     FSP_ERR_ASSERTION             An input parameter is invalid.
 * @retval     FSP_ERR_NOT_OPEN              Module is not open.
 * @retval     FSP_ERR_NOT_INITIALIZED       Module has not been initialized.
 *
 * @return See @ref RENESAS_ERROR_CODES or functions called by this function for other possible return codes.
 *         This function calls:
 *             * @ref sdmmc_api_t::erase
 *             * @ref sdmmc_api_t::statusGet
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Erase (rm_block_media_ctrl_t * const p_ctrl,
                                      uint32_t const                block_address,
                                      uint32_t const                num_blocks)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);
    FSP_PARAMETER_NOT_USED(block_address);
    FSP_PARAMETER_NOT_USED(num_blocks);

    g_blockmedia_complete_event = true;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Provides driver status.  Implements @ref rm_block_media_api_t::statusGet().
 *
 * @retval     FSP_SUCCESS                   Status stored in p_status.
 * @retval     FSP_ERR_ASSERTION             NULL pointer.
 * @retval     FSP_ERR_NOT_OPEN              Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_StatusGet (rm_block_media_ctrl_t * const   p_api_ctrl,
                                          rm_block_media_status_t * const p_status)
{

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_status);

    p_status->media_inserted = true;
    p_status->initialized    = true;
    p_status->busy           = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Retrieves module information.  Implements @ref rm_block_media_api_t::infoGet().
 *
 * @retval     FSP_SUCCESS                   Erase operation requested.
 * @retval     FSP_ERR_ASSERTION             An input parameter is invalid.
 * @retval     FSP_ERR_NOT_OPEN              Module is not open.
 * @retval     FSP_ERR_NOT_INITIALIZED       Module has not been initialized.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_InfoGet (rm_block_media_ctrl_t * const p_ctrl, rm_block_media_info_t * const p_info)
{
    rm_block_media_info_t * p_instance_ctrl = (rm_block_media_info_t *) p_ctrl;

    FSP_PARAMETER_NOT_USED(p_instance_ctrl);

    p_info->sector_size_bytes = STRG_SECTSIZE;
//    p_info->num_sectors       = p_instance_ctrl->num_sectors;
    p_info->num_sectors       = STRG_TOTALSECT;  //yama
    p_info->reentrant         = false;
    p_info->write_protected   = false;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * Closes an open SD/MMC device.  Implements @ref rm_block_media_api_t::close().
 *
 * @retval     FSP_SUCCESS                   Successful close.
 * @retval     FSP_ERR_ASSERTION             An input parameter is invalid.
 * @retval     FSP_ERR_NOT_OPEN              Module is not open.
 **********************************************************************************************************************/
fsp_err_t RM_BLOCK_MEDIA_RAM_Close (rm_block_media_ctrl_t * const p_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_ctrl);

    return FSP_SUCCESS;
}
