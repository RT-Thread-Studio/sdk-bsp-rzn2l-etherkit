/*
                  Linker File for Renesas RZ/N2 FSP
*/

/* The memory information for each device is done in memory regions file.
 * The starting address and length of memory not defined in memory regions file are defined as 0. */
 
/* generated memory regions file - do not edit */
ATCM_START  = 0x00000000;
ATCM_LENGTH = 0x20000;
BTCM_START  = 0x00100000;
BTCM_LENGTH = 0x20000;
SYSTEM_RAM_START  = 0x10000000;
SYSTEM_RAM_LENGTH = 0x180000;
SYSTEM_RAM_MIRROR_START  = 0x30000000;
SYSTEM_RAM_MIRROR_LENGTH = 0x180000;
xSPI0_CS0_SPACE_MIRROR_START  = 0x40000000;
xSPI0_CS0_SPACE_MIRROR_LENGTH = 0x4000000;
xSPI0_CS1_SPACE_MIRROR_START  = 0x44000000;
xSPI0_CS1_SPACE_MIRROR_LENGTH = 0x4000000;
xSPI1_CS0_SPACE_MIRROR_START  = 0x48000000;
xSPI1_CS0_SPACE_MIRROR_LENGTH = 0x4000000;
CS0_SPACE_MIRROR_START  = 0x50000000;
CS0_SPACE_MIRROR_LENGTH = 0x4000000;
CS2_SPACE_MIRROR_START  = 0x54000000;
CS2_SPACE_MIRROR_LENGTH = 0x4000000;
CS3_SPACE_MIRROR_START  = 0x58000000;
CS3_SPACE_MIRROR_LENGTH = 0x4000000;
CS5_SPACE_MIRROR_START  = 0x5C000000;
CS5_SPACE_MIRROR_LENGTH = 0x4000000;
xSPI0_CS0_SPACE_START  = 0x60000000;
xSPI0_CS0_SPACE_LENGTH = 0x4000000;
xSPI0_CS1_SPACE_START  = 0x64000000;
xSPI0_CS1_SPACE_LENGTH = 0x4000000;
xSPI1_CS0_SPACE_START  = 0x68000000;
xSPI1_CS0_SPACE_LENGTH = 0x4000000;
CS0_SPACE_START  = 0x70000000;
CS0_SPACE_LENGTH = 0x4000000;
CS2_SPACE_START  = 0x74000000;
CS2_SPACE_LENGTH = 0x4000000;
CS3_SPACE_START  = 0x78000000;
CS3_SPACE_LENGTH = 0x4000000;
CS5_SPACE_START  = 0x7C000000;
CS5_SPACE_LENGTH = 0x4000000;
CR52_0 = 0;

ATCM_PRV_START = DEFINED(ATCM_START) ? ATCM_START : 0;
ATCM_PRV_LENGTH = DEFINED(ATCM_LENGTH) ? ATCM_LENGTH : 0;
BTCM_PRV_START = DEFINED(BTCM_START) ? BTCM_START : 0;
BTCM_PRV_LENGTH = DEFINED(BTCM_LENGTH) ? BTCM_LENGTH : 0;
SYSTEM_RAM_PRV_START = DEFINED(SYSTEM_RAM_START) ? SYSTEM_RAM_START : 0;
SYSTEM_RAM_PRV_LENGTH = DEFINED(SYSTEM_RAM_LENGTH) ? SYSTEM_RAM_LENGTH : 0;
SYSTEM_RAM_MIRROR_PRV_START = DEFINED(SYSTEM_RAM_MIRROR_START) ? SYSTEM_RAM_MIRROR_START : 0;
SYSTEM_RAM_MIRROR_PRV_LENGTH = DEFINED(SYSTEM_RAM_MIRROR_LENGTH) ? SYSTEM_RAM_MIRROR_LENGTH : 0;
xSPI0_CS0_SPACE_MIRROR_PRV_START = DEFINED(xSPI0_CS0_SPACE_MIRROR_START) ? xSPI0_CS0_SPACE_MIRROR_START : 0;
xSPI0_CS0_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI0_CS0_SPACE_MIRROR_LENGTH) ? xSPI0_CS0_SPACE_MIRROR_LENGTH : 0;
xSPI0_CS1_SPACE_MIRROR_PRV_START = DEFINED(xSPI0_CS1_SPACE_MIRROR_START) ? xSPI0_CS1_SPACE_MIRROR_START : 0;
xSPI0_CS1_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI0_CS1_SPACE_MIRROR_LENGTH) ? xSPI0_CS1_SPACE_MIRROR_LENGTH : 0;
xSPI1_CS0_SPACE_MIRROR_PRV_START = DEFINED(xSPI1_CS0_SPACE_MIRROR_START) ? xSPI1_CS0_SPACE_MIRROR_START : 0;
xSPI1_CS0_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI1_CS0_SPACE_MIRROR_LENGTH) ? xSPI1_CS0_SPACE_MIRROR_LENGTH : 0;
xSPI1_CS1_SPACE_MIRROR_PRV_START = DEFINED(xSPI1_CS1_SPACE_MIRROR_START) ? xSPI1_CS1_SPACE_MIRROR_START : 0;
xSPI1_CS1_SPACE_MIRROR_PRV_LENGTH = DEFINED(xSPI1_CS1_SPACE_MIRROR_LENGTH) ? xSPI1_CS1_SPACE_MIRROR_LENGTH : 0;
CS0_SPACE_MIRROR_PRV_START = DEFINED(CS0_SPACE_MIRROR_START) ? CS0_SPACE_MIRROR_START : 0;
CS0_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS0_SPACE_MIRROR_LENGTH) ? CS0_SPACE_MIRROR_LENGTH : 0;
CS2_SPACE_MIRROR_PRV_START = DEFINED(CS2_SPACE_MIRROR_START) ? CS2_SPACE_MIRROR_START : 0;
CS2_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS2_SPACE_MIRROR_LENGTH) ? CS2_SPACE_MIRROR_LENGTH : 0;
CS3_SPACE_MIRROR_PRV_START = DEFINED(CS3_SPACE_MIRROR_START) ? CS3_SPACE_MIRROR_START : 0;
CS3_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS3_SPACE_MIRROR_LENGTH) ? CS3_SPACE_MIRROR_LENGTH : 0;
CS5_SPACE_MIRROR_PRV_START = DEFINED(CS5_SPACE_MIRROR_START) ? CS5_SPACE_MIRROR_START : 0;
CS5_SPACE_MIRROR_PRV_LENGTH = DEFINED(CS5_SPACE_MIRROR_LENGTH) ? CS5_SPACE_MIRROR_LENGTH : 0;
xSPI0_CS0_SPACE_PRV_START = DEFINED(xSPI0_CS0_SPACE_START) ? xSPI0_CS0_SPACE_START : 0;
xSPI0_CS0_SPACE_PRV_LENGTH = DEFINED(xSPI0_CS0_SPACE_LENGTH) ? xSPI0_CS0_SPACE_LENGTH : 0;
xSPI0_CS1_SPACE_PRV_START = DEFINED(xSPI0_CS1_SPACE_START) ? xSPI0_CS1_SPACE_START : 0;
xSPI0_CS1_SPACE_PRV_LENGTH = DEFINED(xSPI0_CS1_SPACE_LENGTH) ? xSPI0_CS1_SPACE_LENGTH : 0;
xSPI1_CS0_SPACE_PRV_START = DEFINED(xSPI1_CS0_SPACE_START) ? xSPI1_CS0_SPACE_START : 0;
xSPI1_CS0_SPACE_PRV_LENGTH = DEFINED(xSPI1_CS0_SPACE_LENGTH) ? xSPI1_CS0_SPACE_LENGTH : 0;
xSPI1_CS1_SPACE_PRV_START = DEFINED(xSPI1_CS1_SPACE_START) ? xSPI1_CS1_SPACE_START : 0;
xSPI1_CS1_SPACE_PRV_LENGTH = DEFINED(xSPI1_CS1_SPACE_LENGTH) ? xSPI1_CS1_SPACE_LENGTH : 0;
CS0_SPACE_PRV_START = DEFINED(CS0_SPACE_START) ? CS0_SPACE_START : 0;
CS0_SPACE_PRV_LENGTH = DEFINED(CS0_SPACE_LENGTH) ? CS0_SPACE_LENGTH : 0;
CS2_SPACE_PRV_START = DEFINED(CS2_SPACE_START) ? CS2_SPACE_START : 0;
CS2_SPACE_PRV_LENGTH = DEFINED(CS2_SPACE_LENGTH) ? CS2_SPACE_LENGTH : 0;
CS3_SPACE_PRV_START = DEFINED(CS3_SPACE_START) ? CS3_SPACE_START : 0;
CS3_SPACE_PRV_LENGTH = DEFINED(CS3_SPACE_LENGTH) ? CS3_SPACE_LENGTH : 0;
CS5_SPACE_PRV_START = DEFINED(CS5_SPACE_START) ? CS5_SPACE_START : 0;
CS5_SPACE_PRV_LENGTH = DEFINED(CS5_SPACE_LENGTH) ? CS5_SPACE_LENGTH : 0;
PCIE0_MIRROR_PRV_START = DEFINED(PCIE0_MIRROR_START) ? PCIE0_MIRROR_START : 0;
PCIE0_MIRROR_PRV_LENGTH = DEFINED(PCIE0_MIRROR_LENGTH) ? PCIE0_MIRROR_LENGTH : 0;
PCIE1_MIRROR_PRV_START = DEFINED(PCIE1_MIRROR_START) ? PCIE1_MIRROR_START : 0;
PCIE1_MIRROR_PRV_LENGTH = DEFINED(PCIE1_MIRROR_LENGTH) ? PCIE1_MIRROR_LENGTH : 0;
DDR_MIRROR0_PRV_START = DEFINED(DDR_MIRROR0_START) ? DDR_MIRROR0_START : 0;
DDR_MIRROR0_PRV_LENGTH = DEFINED(DDR_MIRROR0_LENGTH) ? DDR_MIRROR0_LENGTH : 0;
DDR_MIRROR1_PRV_START = DEFINED(DDR_MIRROR1_START) ? DDR_MIRROR1_START : 0;
DDR_MIRROR1_PRV_LENGTH = DEFINED(DDR_MIRROR1_LENGTH) ? DDR_MIRROR1_LENGTH : 0;
DDR_MIRROR_PRV_START = DEFINED(DDR_MIRROR_START) ? DDR_MIRROR_START : 0;
DDR_MIRROR_PRV_LENGTH = DEFINED(DDR_MIRROR_LENGTH) ? DDR_MIRROR_LENGTH : 0;
DDR_PRV_START = DEFINED(DDR_START) ? DDR_START : 0;
DDR_PRV_LENGTH = DEFINED(DDR_LENGTH) ? DDR_LENGTH : 0;
PCIE0_PRV_START = DEFINED(PCIE0_START) ? PCIE0_START : 0;
PCIE0_PRV_LENGTH = DEFINED(PCIE0_LENGTH) ? PCIE0_LENGTH : 0;
PCIE1_PRV_START = DEFINED(PCIE1_START) ? PCIE1_START : 0;
PCIE1_PRV_LENGTH = DEFINED(PCIE1_LENGTH) ? PCIE1_LENGTH : 0;
SECONDARY_PRV = DEFINED(SECONDARY) ? SECONDARY : 0;

_RZN_ORDINAL = (0 == SECONDARY_PRV) ? 1 : 2;
HAS_SYSTEM_RAM_MIRROR = DEFINED(SYSTEM_RAM_MIRROR_START) ? 1 : 0;

SYSTEM_RAM_ALIGN_START            = ALIGN(SYSTEM_RAM_PRV_START, 0x00020000);
SYSTEM_RAM_ALIGN_LENGTH           = SYSTEM_RAM_PRV_LENGTH - (SYSTEM_RAM_PRV_LENGTH % 0x00020000);
SYSTEM_RAM_MIRROR_OFFSET          = DEFINED(SYSTEM_RAM_START) ?
                                        (0 == HAS_SYSTEM_RAM_MIRROR) ?
                                            (DEFINED(CR52_0) || DEFINED(CR52_1)) ?
                                                0x00000000 :
                                                0x00200000 :
                                            0x20000000 :
                                        0;

xSPI0_CS0_SPACE_ALIGN_START       = ALIGN(xSPI0_CS0_SPACE_PRV_START, 0x00020000);
xSPI0_CS0_SPACE_ALIGN_LENGTH      = xSPI0_CS0_SPACE_PRV_LENGTH - (xSPI0_CS0_SPACE_PRV_LENGTH % 0x00020000);

RAM_START                         = SYSTEM_RAM_ALIGN_START;
RAM_LENGTH                        = SYSTEM_RAM_ALIGN_LENGTH;
LOADER_START                      = ((1 == _RZN_ORDINAL) && DEFINED(CR52_0)) ? BTCM_PRV_START : SYSTEM_RAM_ALIGN_START;
LOADER_LENGTH                     = ((1 == _RZN_ORDINAL) && DEFINED(CR52_0)) ? BTCM_PRV_LENGTH : SYSTEM_RAM_ALIGN_LENGTH;
ROM_START                         = (1 == _RZN_ORDINAL) ? xSPI0_CS0_SPACE_ALIGN_START : SYSTEM_RAM_ALIGN_START;
ROM_LENGTH                        = (1 == _RZN_ORDINAL) ? xSPI0_CS0_SPACE_ALIGN_LENGTH : SYSTEM_RAM_ALIGN_LENGTH;

FLASH_CONTENTS_OFFSET             = (0 == HAS_SYSTEM_RAM_MIRROR) ? 0x00000050 : 0x0000004C;
LOADER_TEXT_OFFSET                = ((1 == _RZN_ORDINAL) && DEFINED(CR52_0)) ? 0x00002000 : 0x00001000;
TEXT_OFFSET                       = ((1 == _RZN_ORDINAL) && DEFINED(CR52_0)) ? 0x00000100 : 0x00010000;
IMAGE_INFO_OFFSET                 = 0x00000800;

INTVEC_ADDRESS                    = RAM_START;
LOADER_TEXT_ADDRESS               = LOADER_START + LOADER_TEXT_OFFSET;
TEXT_ADDRESS                      = ((1 == _RZN_ORDINAL) || ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1)))) ? RAM_START + TEXT_OFFSET : noncache_flash_contents_end;

/* Define starting addresses and length for data_noncache, DMAC link mode data, CPU-shared non-cache, and CPU-specific non-cache areas. */
DATA_NONCACHE_OFFSET              = DEFINED(SYSTEM_RAM_START) ? 0x00048000 : 0;
DMAC_LINK_MODE_OFFSET             = DEFINED(SYSTEM_RAM_START) ? 0x00044000 : 0;
SHARED_NONCACHE_BUFFER_OFFSET     = DEFINED(SYSTEM_RAM_START) ? 0x00040000 : 0;
NONCACHE_BUFFER_OFFSET            = DEFINED(SYSTEM_RAM_START) ? 0x00020000 : 0;

SYSTEM_RAM_END_ADDRESS            = SYSTEM_RAM_ALIGN_START + SYSTEM_RAM_ALIGN_LENGTH;
SYSTEM_RAM_MIRROR_END_ADDRESS     = SYSTEM_RAM_ALIGN_START + SYSTEM_RAM_ALIGN_LENGTH + SYSTEM_RAM_MIRROR_OFFSET;

DATA_NONCACHE_START               = (1 == _RZN_ORDINAL) ?
                                        SYSTEM_RAM_MIRROR_END_ADDRESS - DATA_NONCACHE_OFFSET :
                                        ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                            __ThreadStackLimit :
                                            _mdata_noncache + SYSTEM_RAM_MIRROR_OFFSET;
DMAC_LINK_MODE_START              = (1 == _RZN_ORDINAL) ?
                                        SYSTEM_RAM_MIRROR_END_ADDRESS - DMAC_LINK_MODE_OFFSET :
                                        ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                            _data_noncache_end :
                                            _mdmac_link_mode + SYSTEM_RAM_MIRROR_OFFSET;
NONCACHE_BUFFER_START             = (1 == _RZN_ORDINAL) ?
                                        SYSTEM_RAM_MIRROR_END_ADDRESS - NONCACHE_BUFFER_OFFSET :
                                        ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                            _dmac_link_mode_end :
                                            _mncbuffer + SYSTEM_RAM_MIRROR_OFFSET;
SHARED_NONCACHE_BUFFER_START      = SYSTEM_RAM_MIRROR_END_ADDRESS - SHARED_NONCACHE_BUFFER_OFFSET;

data_noncache_size                = _data_noncache_end - _data_noncache_start;
dmac_link_mode_size               = _dmac_link_mode_end - _dmac_link_mode_start;
sncbuffer_size                    = _sncbuffer_end - _sncbuffer_start;
ncbuffer_size                     = _ncbuffer_end - _ncbuffer_start;

LCDC_FRAME_BUFFER_OFFSET          = (DEFINED(CR52_0) || DEFINED(CR52_1)) ?
                                        DEFINED(DDR_MIRROR1_START) ?
                                            0x04000000 :
                                            0 :
                                        DEFINED(DDR_MIRROR_START) ?
                                            0x04000000 :
                                            0;
LCDC_FRAME_BUFFER_START           = (DEFINED(CR52_0) || DEFINED(CR52_1)) ?
                                        DDR_MIRROR1_PRV_START + DDR_MIRROR1_PRV_LENGTH - LCDC_FRAME_BUFFER_OFFSET :
                                        DDR_MIRROR_PRV_START + DDR_MIRROR_PRV_LENGTH - LCDC_FRAME_BUFFER_OFFSET;
LCDC_FRAME_BUFFER_LENGTH          = LCDC_FRAME_BUFFER_OFFSET;

MEMORY
{
    ATCM : ORIGIN = ATCM_PRV_START, LENGTH = ATCM_PRV_LENGTH
    BTCM : ORIGIN = BTCM_PRV_START, LENGTH = BTCM_PRV_LENGTH
    SYSTEM_RAM : ORIGIN = SYSTEM_RAM_ALIGN_START, LENGTH = SYSTEM_RAM_ALIGN_LENGTH
    SYSTEM_RAM_MIRROR : ORIGIN = SYSTEM_RAM_MIRROR_PRV_START, LENGTH = SYSTEM_RAM_MIRROR_PRV_LENGTH
    xSPI0_CS0_SPACE_MIRROR : ORIGIN = xSPI0_CS0_SPACE_MIRROR_PRV_START, LENGTH = xSPI0_CS0_SPACE_MIRROR_PRV_LENGTH
    xSPI0_CS1_SPACE_MIRROR : ORIGIN = xSPI0_CS1_SPACE_MIRROR_PRV_START, LENGTH = xSPI0_CS1_SPACE_MIRROR_PRV_LENGTH
    xSPI1_CS0_SPACE_MIRROR : ORIGIN = xSPI1_CS0_SPACE_MIRROR_PRV_START, LENGTH = xSPI1_CS0_SPACE_MIRROR_PRV_LENGTH
    xSPI1_CS1_SPACE_MIRROR : ORIGIN = xSPI1_CS1_SPACE_MIRROR_PRV_START, LENGTH = xSPI1_CS1_SPACE_MIRROR_PRV_LENGTH
    CS0_SPACE_MIRROR : ORIGIN = CS0_SPACE_MIRROR_PRV_START, LENGTH = CS0_SPACE_MIRROR_PRV_LENGTH
    CS2_SPACE_MIRROR : ORIGIN = CS2_SPACE_MIRROR_PRV_START, LENGTH = CS2_SPACE_MIRROR_PRV_LENGTH
    CS3_SPACE_MIRROR : ORIGIN = CS3_SPACE_MIRROR_PRV_START, LENGTH = CS3_SPACE_MIRROR_PRV_LENGTH
    CS5_SPACE_MIRROR : ORIGIN = CS5_SPACE_MIRROR_PRV_START, LENGTH = CS5_SPACE_MIRROR_PRV_LENGTH
    xSPI0_CS0_SPACE : ORIGIN = xSPI0_CS0_SPACE_ALIGN_START, LENGTH = xSPI0_CS0_SPACE_ALIGN_LENGTH
    xSPI0_CS1_SPACE : ORIGIN = xSPI0_CS1_SPACE_PRV_START, LENGTH = xSPI0_CS1_SPACE_PRV_LENGTH
    xSPI1_CS0_SPACE : ORIGIN = xSPI1_CS0_SPACE_PRV_START, LENGTH = xSPI1_CS0_SPACE_PRV_LENGTH
    xSPI1_CS1_SPACE : ORIGIN = xSPI1_CS1_SPACE_PRV_START, LENGTH = xSPI1_CS1_SPACE_PRV_LENGTH
    CS0_SPACE : ORIGIN = CS0_SPACE_PRV_START, LENGTH = CS0_SPACE_PRV_LENGTH
    CS2_SPACE : ORIGIN = CS2_SPACE_PRV_START, LENGTH = CS2_SPACE_PRV_LENGTH
    CS3_SPACE : ORIGIN = CS3_SPACE_PRV_START, LENGTH = CS3_SPACE_PRV_LENGTH
    CS5_SPACE : ORIGIN = CS5_SPACE_PRV_START, LENGTH = CS5_SPACE_PRV_LENGTH
    PCIE0_MIRROR : ORIGIN = PCIE0_MIRROR_PRV_START, LENGTH = PCIE0_MIRROR_PRV_LENGTH
    PCIE1_MIRROR : ORIGIN = PCIE1_MIRROR_PRV_START, LENGTH = PCIE1_MIRROR_PRV_LENGTH
    DDR_MIRROR0 : ORIGIN = DDR_MIRROR0_PRV_START, LENGTH = DDR_MIRROR0_PRV_LENGTH
    DDR_MIRROR1 : ORIGIN = DDR_MIRROR1_PRV_START, LENGTH = DDR_MIRROR1_PRV_LENGTH
    DDR_MIRROR : ORIGIN = DDR_MIRROR_PRV_START, LENGTH = DDR_MIRROR_PRV_LENGTH
    DDR : ORIGIN = DDR_PRV_START, LENGTH = DDR_PRV_LENGTH
    PCIE0 : ORIGIN = PCIE0_PRV_START, LENGTH = PCIE0_PRV_LENGTH
    PCIE1 : ORIGIN = PCIE1_PRV_START, LENGTH = PCIE1_PRV_LENGTH
    RAM_TEXT : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
    RAM_DATA : ORIGIN = RAM_START, LENGTH = RAM_LENGTH
    LOADER_TEXT_STACK : ORIGIN = LOADER_START, LENGTH = LOADER_LENGTH
    LOADER_DATA_STACK : ORIGIN = LOADER_START, LENGTH = LOADER_LENGTH
    FLASH_CONTENTS : ORIGIN = ROM_START, LENGTH = ROM_LENGTH
    NONCACHE : ORIGIN = SYSTEM_RAM_ALIGN_START + SYSTEM_RAM_MIRROR_OFFSET, LENGTH = SYSTEM_RAM_ALIGN_LENGTH
    LCDC_FRAME_BUFFER : ORIGIN = LCDC_FRAME_BUFFER_START, LENGTH = LCDC_FRAME_BUFFER_LENGTH
}

SECTIONS
{
    LOADER_PARAM_ADDRESS = (1 == _RZN_ORDINAL) ? xSPI0_CS0_SPACE_ALIGN_START : _loader_text_start;
    .loader_param LOADER_PARAM_ADDRESS : AT (LOADER_PARAM_ADDRESS)
    {
        KEEP(*(.loader_param))
    } > FLASH_CONTENTS

    FLASH_CONTENTS_ADDRESS = (1 == _RZN_ORDINAL) ? LOADER_PARAM_ADDRESS + FLASH_CONTENTS_OFFSET : _loader_text_start;
    .flash_contents FLASH_CONTENTS_ADDRESS : AT (FLASH_CONTENTS_ADDRESS)
    {
        _mloader_text = .;
        . = (1 == _RZN_ORDINAL) ? . + (_loader_text_end - _loader_text_start) : .;
        . = ALIGN(8);
        _mloader_data = .;
        . = (1 == _RZN_ORDINAL) ? . + (_loader_data_end - _loader_data_start) : .;
        . = ALIGN(8);
        _mfvector = .;
        . = (1 == _RZN_ORDINAL) ? . + (_fvector_end - _fvector_start) : .;
        . = ALIGN(8);
        _mtext = .;
        . = (1 == _RZN_ORDINAL) ? . + (__text_end - __text_start) +
                                      (_rvectors_end - _rvectors_start) +
                                      (__extab_end - __extab_start) +
                                      (__exidx_end - __exidx_start) : .;
        . = ALIGN(8);
        _mdata = .;
        . = (1 == _RZN_ORDINAL) ? . + (__data_end - __data_start) +
                                      (__got_end - __got_start) : .;
        flash_contents_end = .;
    } > FLASH_CONTENTS

    NONCACHE_FLASH_CONTENTS_ADDRESS = (1 == _RZN_ORDINAL) ? flash_contents_end : RAM_START + TEXT_OFFSET;
    .noncache_flash_contents NONCACHE_FLASH_CONTENTS_ADDRESS : AT (NONCACHE_FLASH_CONTENTS_ADDRESS)
    {
        . = (0 != data_noncache_size) ? ALIGN(8) : .;
        _mdata_noncache = .;
        . = . + data_noncache_size;
        . = (0 != dmac_link_mode_size) ? ALIGN(8) : .;
        _mdmac_link_mode = .;
        . = . + dmac_link_mode_size;
        . = (0 != sncbuffer_size) ? ALIGN(32) : .;
        _msncbuffer = .;
        . = . + sncbuffer_size;
        . = (0 != ncbuffer_size) ? ALIGN(32) : .;
        _mncbuffer = .;
        . = . + ncbuffer_size;
        noncache_flash_contents_end = .;
    } > FLASH_CONTENTS

    LOADER_TEXT_IMAGE                 = (1 == _RZN_ORDINAL) ? _mloader_text : _loader_text_start;
    .loader_text LOADER_TEXT_ADDRESS : AT (LOADER_TEXT_IMAGE)
    {
        _loader_text_start = .;
        *(.loader_text)
        */fsp/src/bsp/cmsis/Device/RENESAS/Source/*.o(.text*)
        */fsp/src/bsp/mcu/all/*/bsp_irq_core.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_clocks.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_irq.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_semaphore.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_register_protection.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_cache.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_tzc400.o(.text*)
        */fsp/src/bsp/mcu/all/bsp_address_expander.o(.text*)
        */fsp/src/r_ioport/r_ioport.o(.text*)
        KEEP(*(.warm_start))
        KEEP(*(.loader_user_data*))
        . = ALIGN(0x200);
        _loader_text_end = .;
    } > LOADER_TEXT_STACK

    LOADER_DATA_IMAGE = (1 == _RZN_ORDINAL) ? _mloader_data : _loader_data_start;
    .loader_data _loader_text_end : AT (LOADER_DATA_IMAGE)
    {
        _loader_data_start = .;
        __loader_data_start = .;
        */fsp/src/bsp/cmsis/Device/RENESAS/Source/*.o(.data*)
        */fsp/src/bsp/cmsis/Device/RENESAS/Source/*/system_core.o(.rodata*)
        */fsp/src/bsp/mcu/all/*/bsp_irq_core.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_clocks.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_irq.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_semaphore.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_register_protection.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_register_protection.o(.rodata*)
        */fsp/src/bsp/mcu/all/bsp_cache.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_tzc400.o(.data*)
        */fsp/src/bsp/mcu/all/bsp_tzc400.o(.rodata*)
        */fsp/src/bsp/mcu/all/bsp_address_expander.o(.data*)
        */fsp/src/r_ioport/r_ioport.o(.data*)
        . = ALIGN(8);
        __loader_data_end = .;
        __loader_bss_start = .;
        */fsp/src/bsp/cmsis/Device/RENESAS/Source/*.o(.bss*)
        */fsp/src/bsp/mcu/all/*/bsp_irq_core.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_clocks.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_irq.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_semaphore.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_register_protection.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_cache.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_tzc400.o(.bss*)
        */fsp/src/bsp/mcu/all/bsp_address_expander.o(.bss*)
        */fsp/src/r_ioport/r_ioport.o(.bss*)
        KEEP(*(.ttbr))
        */fsp/src/bsp/cmsis/Device/RENESAS/Source/*.o(COMMON)
        */fsp/src/bsp/mcu/all/*/bsp_irq_core.o(COMMON)
        */fsp/src/bsp/mcu/all/bsp_clocks.o(COMMON)
        */fsp/src/bsp/mcu/all/bsp_irq.o(COMMON)
        */fsp/src/bsp/mcu/all/bsp_semaphore.o(COMMON)
        */fsp/src/bsp/mcu/all/bsp_register_protection.o(.COMMON)
        */fsp/src/bsp/mcu/all/bsp_cache.o(COMMON)
        */fsp/src/bsp/mcu/all/bsp_tzc400.o(COMMON)
        */fsp/src/bsp/mcu/all/bsp_address_expander.o(COMMON)
        */fsp/src/r_ioport/r_ioport.o(.COMMON)
        . = ALIGN(8);
        __loader_bss_end = . ;
        */fsp/src/bsp/cmsis/Device/RENESAS/Source/*.o(.software_reset) /* Do not initialize */
        _loader_data_end = .;
    } > LOADER_DATA_STACK

    INTVEC_IMAGE = (1 == _RZN_ORDINAL) ? _mfvector : _fvector_start;
    .intvec INTVEC_ADDRESS : AT (INTVEC_IMAGE)
    {
        _fvector_start = .;
        KEEP(*(.intvec))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x080))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x100))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x180))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x200))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x280))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x300))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x380))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x400))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x480))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x500))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x580))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x600))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x680))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x700))
        . = ALIGN(0x80);
        KEEP(*(.intvec_0x780))
        _fvector_end = .;
    } > RAM_TEXT

    IMAGE_INFO_ADDRESS = RAM_START + IMAGE_INFO_OFFSET;
    .image_info IMAGE_INFO_ADDRESS : AT (IMAGE_INFO_ADDRESS)
    {
        _image_info_start = .;
        KEEP(*(.image_info))
        _image_info_end = .;
    } > RAM_DATA

    TEXT_IMAGE = (1 == _RZN_ORDINAL) ? _mtext : _text_start;
    .text TEXT_ADDRESS : AT (TEXT_IMAGE)
    {
        _text_start = .;
        __text_start = .;

        *(.text*)

        KEEP(*(.reset_handler))

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        _ctor_end = .;

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        _dtor_end = .;

        /* section information for utest */
        . = ALIGN(4);
        __rt_utest_tc_tab_start = .;
        KEEP(*(UtestTcTab))
        __rt_utest_tc_tab_end = .;

        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        /* section information for initial. */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;

        /* new GCC version uses .init_array */
        PROVIDE(__ctors_start__ = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE(__ctors_end__ = .);

        . = ALIGN(4);
        KEEP(*(FalPartTable))

        KEEP(*(.eh_frame*))

        __text_end = .;
    } > RAM_TEXT
    .rvectors :
    {
        _rvectors_start = .;
        KEEP(*(.rvectors))
        _rvectors_end = .;
    } > RAM_TEXT
    .ARM.extab :
    {
        __extab_start = .;
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        __extab_end = .;
    } > RAM_TEXT
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
        _text_end = .;
    } > RAM_TEXT

    DATA_IMAGE = (1 == _RZN_ORDINAL) ? _mdata : _data_start;
    .data _text_end :  AT (DATA_IMAGE)
    {
        _data_start = .;
        __data_start = .;

        *(vtable)
        *(.data.*)
        *(.data)

        *(.rodata*)
        _erodata = .;

        . = ALIGN(8);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(8);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(8);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(8);

        __data_end = .;
    } > RAM_DATA
    .got :
    {
        __got_start = .;
        *(.got)
        *(.got.plt)
        __got_end = .;

        /* All data end */
        _data_end = .;
    } > RAM_DATA
    .bss :
    {
        . = ALIGN(8);
        __bss_start__ = .;
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = .;
        _ebss = .;
        _end = .;
    } > RAM_DATA
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
    } > RAM_DATA
    .aarch64_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __AArch64StackBase = .;
        /* Place the Thread stacks here. */
        KEEP(*(.aarch64_stack*))
        __AArch64StackLimit = .;
    } > RAM_DATA
    .thread_stack (NOLOAD):
    {
        . = ALIGN(8);
        __ThreadStackBase = .;
        /* Place the Thread stacks here. */
        KEEP(*(.stack*))
        __ThreadStackLimit = .;
    } > RAM_DATA
    .sys_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __SysStackBase = .;
        /* Place the sys_stack here. */
        KEEP(*(.sys_stack))
        __SysStackLimit = .;
    } > LOADER_DATA_STACK
    .svc_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __SvcStackBase = .;
        /* Place the svc_stack here. */
        KEEP(*(.svc_stack))
        __SvcStackLimit = .;
    } > LOADER_DATA_STACK
    .irq_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __IrqStackBase = .;
        /* Place the irq_stack here. */
        KEEP(*(.irq_stack))
        __IrqStackLimit = .;
    } > LOADER_DATA_STACK
    .fiq_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __FiqStackBase = .;
        /* Place the fiq_stack here. */
        KEEP(*(.fiq_stack))
        __FiqStackLimit = .;
    } > LOADER_DATA_STACK
    .und_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __UndStackBase = .;
        /* Place the und_stack here. */
        KEEP(*(.und_stack))
        __UndStackLimit = .;
    } > LOADER_DATA_STACK
    .abt_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __AbtStackBase = .;
        /* Place the abt_stack here. */
        KEEP(*(.abt_stack))
        __AbtStackLimit = .;
    } > LOADER_DATA_STACK

    DATA_NONCACHE_INIT_START = (1 == _RZN_ORDINAL) ?
                                   _mdata_noncache :
                                   ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                       __ThreadStackLimit :
                                       _mdata_noncache;
    .data_noncache DATA_NONCACHE_START : AT (DATA_NONCACHE_INIT_START)
    {
        . = ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ? ALIGN(64) : ALIGN(8);
        _data_noncache_start = .;
        KEEP(*(.data_noncache*))
        _data_noncache_end = .;
    } > NONCACHE

    DMAC_LINK_MODE_INIT_START = (1 == _RZN_ORDINAL) ?
                                    _mdmac_link_mode :
                                    ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                        _data_noncache_end :
                                        _mdmac_link_mode;
    .dmac_link_mode DMAC_LINK_MODE_START : AT (DMAC_LINK_MODE_INIT_START)
    {
        . = ALIGN(8);
        _dmac_link_mode_start = .;
        KEEP(*(.dmac_link_mode*))
        _dmac_link_mode_end = .;
    } > NONCACHE

    SHARED_NONCACHE_BUFFER_INIT_START = (1 == _RZN_ORDINAL) ?
                                            _msncbuffer :
                                            ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                                _ncbuffer_end :
                                                _msncbuffer;
    .shared_noncache_buffer SHARED_NONCACHE_BUFFER_START : AT (SHARED_NONCACHE_BUFFER_INIT_START)
    {
        . = ALIGN(32);
        _sncbuffer_start = .;
        KEEP(*(.shared_noncache_buffer*))
        _sncbuffer_end = .;
    } > NONCACHE

    NONCACHE_BUFFER_INIT_START = (1 == _RZN_ORDINAL) ?
                                     _mncbuffer :
                                     ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                         _dmac_link_mode_end :
                                         _mncbuffer;
    .noncache_buffer NONCACHE_BUFFER_START : AT (NONCACHE_BUFFER_INIT_START)
    {
        . = ALIGN(32);
        _ncbuffer_start = .;
        KEEP(*(.noncache_buffer*))
        _ncbuffer_end = .;
    } > NONCACHE

    .lcdc_frame_buffer (NOLOAD) :
    {
        . = ALIGN(512);
        _lcdc_frame_buffer_start = .;
        KEEP(*(.lcdc_frame_buffer*))
        _lcdc_frame_buffer_end = .;
    } > LCDC_FRAME_BUFFER

    SECONDARY_START = ((1 == _RZN_ORDINAL) && DEFINED(CR52_0)) ? SYSTEM_RAM_ALIGN_START : ALIGN(__ThreadStackLimit, 0x00020000);
    SECONDARY_IMAGE = (1 == _RZN_ORDINAL) ? ALIGN(noncache_flash_contents_end, 0x00020000) : SECONDARY_START;
    .secondary SECONDARY_START : AT (SECONDARY_IMAGE)
    {
        . = ALIGN(0x20000);
        _secondary_start = .;
        KEEP(*(.secondary))
        _secondary_end = .;
    } > SYSTEM_RAM
}

__ddsc_xSPI0_CS0_SPACE_START = ORIGIN(xSPI0_CS0_SPACE);
__ddsc_xSPI0_CS0_SPACE_END   = (1 == _RZN_ORDINAL) ? noncache_flash_contents_end : __ddsc_xSPI0_CS0_SPACE_START;
__ddsc_ATCM_START            = ORIGIN(ATCM);
__ddsc_ATCM_END              = (1 == _RZN_ORDINAL) && DEFINED(CR52_0) ? __ThreadStackLimit : __ddsc_ATCM_START;
__ddsc_BTCM_START            = ORIGIN(BTCM);
__ddsc_BTCM_END              = (1 == _RZN_ORDINAL) && DEFINED(CR52_0) ? __AbtStackLimit : __ddsc_BTCM_START;
__ddsc_SYSTEM_RAM_START      = ORIGIN(SYSTEM_RAM);
__ddsc_SYSTEM_RAM_END         = (1 == _RZN_ORDINAL) ?
                                    DEFINED(CR52_0) ?
                                        __ddsc_SYSTEM_RAM_START :
                                        __ThreadStackLimit :
                                    ((0 == HAS_SYSTEM_RAM_MIRROR) && (DEFINED(CR52_0) || DEFINED(CR52_1))) ?
                                        _ncbuffer_end :
                                        __ThreadStackLimit;
__ddsc_SECONDARY_START        = (1 == _RZN_ORDINAL) ? 0 : 1;
__ddsc_SECONDARY_END          = 1;
