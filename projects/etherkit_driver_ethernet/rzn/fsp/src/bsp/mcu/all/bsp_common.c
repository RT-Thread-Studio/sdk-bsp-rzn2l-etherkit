/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "bsp_api.h"

#if (2 == BSP_CFG_RTOS)                // FreeRTOS
 #include "FreeRTOS.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if defined(__ICCARM__)
 #define WEAK_ERROR_ATTRIBUTE
 #define WEAK_INIT_ATTRIBUTE
 #pragma weak fsp_error_log                            = fsp_error_log_internal
 #pragma weak bsp_init                                 = bsp_init_internal
#elif defined(__GNUC__)

 #define WEAK_ERROR_ATTRIBUTE    __attribute__((weak, alias("fsp_error_log_internal")))

 #define WEAK_INIT_ATTRIBUTE     __attribute__((weak, alias("bsp_init_internal")))
#endif

#define FSP_SECTION_VERSION      ".version"

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/* System clock frequency information for CKIO */
#if (1 == BSP_FEATURE_CGC_SCKCR_TYPE)
const uint32_t g_bsp_system_clock_select_ckio[][2] =
{
    {BSP_PRV_CKIO_FREQ_100_MHZ,       BSP_PRV_CKIO_FREQ_75_MHZ       }, // CKIO = 000b
    {BSP_PRV_CKIO_FREQ_66_7_MHZ,      BSP_PRV_CKIO_FREQ_50_MHZ       }, // CKIO = 001b
    {BSP_PRV_CKIO_FREQ_50_MHZ,        BSP_PRV_CKIO_FREQ_37_5_MHZ     }, // CKIO = 010b
    {BSP_PRV_CKIO_FREQ_40_MHZ,        BSP_PRV_CKIO_FREQ_30_MHZ       }, // CKIO = 011b
    {BSP_PRV_CKIO_FREQ_33_3_MHZ,      BSP_PRV_CKIO_FREQ_25_MHZ       }, // CKIO = 100b
    {BSP_PRV_CKIO_FREQ_28_6_MHZ,      BSP_PRV_CKIO_FREQ_21_4_MHZ     }, // CKIO = 101b
    {BSP_PRV_CKIO_FREQ_25_MHZ,        BSP_PRV_CKIO_FREQ_18_75_MHZ    }, // CKIO = 110b
    {BSP_PRV_CKIO_FREQ_NOT_SUPPORTED, BSP_PRV_CKIO_FREQ_NOT_SUPPORTED}, // CKIO = 111b
};
#elif (2 == BSP_FEATURE_CGC_SCKCR_TYPE)
const uint32_t g_bsp_system_clock_select_ckio[] =
{
    BSP_PRV_CKIO_FREQ_125_MHZ,         // CKIO = 000b
    BSP_PRV_CKIO_FREQ_83_3_MHZ,        // CKIO = 001b
    BSP_PRV_CKIO_FREQ_62_5_MHZ,        // CKIO = 010b
    BSP_PRV_CKIO_FREQ_50_MHZ,          // CKIO = 011b
    BSP_PRV_CKIO_FREQ_41_7_MHZ,        // CKIO = 100b
    BSP_PRV_CKIO_FREQ_35_7_MHZ,        // CKIO = 101b
    BSP_PRV_CKIO_FREQ_31_25_MHZ,       // CKIO = 110b
    BSP_PRV_CKIO_FREQ_NOT_SUPPORTED,   // CKIO = 111b
};
#endif

/* System clock frequency information for XSPI_CLK */
const uint32_t g_bsp_system_clock_select_xspi_clk[][2] =
{
    {BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED, BSP_PRV_XSPI_CLK_FREQ_150_MHZ      }, // FSELXSPIn = 000b
    {BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED, BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED}, // FSELXSPIn = 001b
    {BSP_PRV_XSPI_CLK_FREQ_133_3_MHZ,     BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED}, // FSELXSPIn = 010b
    {BSP_PRV_XSPI_CLK_FREQ_100_MHZ,       BSP_PRV_XSPI_CLK_FREQ_75_MHZ       }, // FSELXSPIn = 011b
    {BSP_PRV_XSPI_CLK_FREQ_50_MHZ,        BSP_PRV_XSPI_CLK_FREQ_37_5_MHZ     }, // FSELXSPIn = 100b
    {BSP_PRV_XSPI_CLK_FREQ_25_MHZ,        BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED}, // FSELXSPIn = 101b
    {BSP_PRV_XSPI_CLK_FREQ_12_5_MHZ,      BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED}, // FSELXSPIn = 110b
    {BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED, BSP_PRV_XSPI_CLK_FREQ_NOT_SUPPORTED}, // FSELXSPIn = 111b
};

/* System clock frequency information for SPI_CLK */
#if (1 == BSP_FEATURE_CGC_PCLKSPI_CLOCK_FREQ_TYPE)
const uint32_t g_bsp_system_clock_select_spi_clk[] =
{
    BSP_PRV_PCLKSPI_FREQ_75_MHZ,       // SPInASYNCSEL = 0b
    BSP_PRV_PCLKSPI_FREQ_96_MHZ,       // SPInASYNCSEL = 1b
};
#elif (2 == BSP_FEATURE_CGC_PCLKSCI_CLOCK_FREQ_TYPE)
const uint32_t g_bsp_system_clock_select_spi_clk[] =
{
    BSP_PRV_PCLKSPI_FREQ_75_MHZ,       // SPInASYNCSEL = 00b
    BSP_PRV_PCLKSPI_FREQ_80_MHZ,       // SPInASYNCSEL = 01b
    BSP_PRV_PCLKSPI_FREQ_96_MHZ,       // SPInASYNCSEL = 10b
    BSP_PRV_PCLKSPI_FREQ_100_MHZ,      // SPInASYNCSEL = 11b
};
#endif

/* System clock frequency information for SCI_CLK */
#if (1 == BSP_FEATURE_CGC_PCLKSCI_CLOCK_FREQ_TYPE)
const uint32_t g_bsp_system_clock_select_sci_clk[] =
{
    BSP_PRV_PCLKSCI_FREQ_75_MHZ,       // SCInASYNCSEL = 0b
    BSP_PRV_PCLKSCI_FREQ_96_MHZ,       // SCInASYNCSEL = 1b
};
#elif (2 == BSP_FEATURE_CGC_PCLKSCI_CLOCK_FREQ_TYPE)
const uint32_t g_bsp_system_clock_select_sci_clk[] =
{
    BSP_PRV_PCLKSCI_FREQ_75_MHZ,       // SCInASYNCSEL = 00b
    BSP_PRV_PCLKSCI_FREQ_80_MHZ,       // SCInASYNCSEL = 01b
    BSP_PRV_PCLKSCI_FREQ_96_MHZ,       // SCInASYNCSEL = 10b
    BSP_PRV_PCLKSCI_FREQ_100_MHZ,      // SCInASYNCSEL = 11b
};
#endif

/***********************************************************************************************************************
 * Private global variables and functions
 **********************************************************************************************************************/

/* FSP pack version structure. */
static BSP_DONT_REMOVE const fsp_pack_version_t g_fsp_version BSP_PLACE_IN_SECTION (FSP_SECTION_VERSION) =
{
    .version_id_b =
    {
        .minor = FSP_VERSION_MINOR,
        .major = FSP_VERSION_MAJOR,
        .build = FSP_VERSION_BUILD,
        .patch = FSP_VERSION_PATCH
    }
};

/* Public FSP version name. */
static BSP_DONT_REMOVE const uint8_t g_fsp_version_string[] BSP_PLACE_IN_SECTION(FSP_SECTION_VERSION) =
    FSP_VERSION_STRING;

/* Unique FSP version ID. */
static BSP_DONT_REMOVE const uint8_t g_fsp_version_build_string[] BSP_PLACE_IN_SECTION(FSP_SECTION_VERSION) =
    FSP_VERSION_BUILD_STRING;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/** Prototype of initialization function called before main.  This prototype sets the weak association of this
 * function to an internal example implementation. If this function is defined in the application code, the
 * application code version is used. */

void bsp_init(void * p_args) WEAK_INIT_ATTRIBUTE;

void bsp_init_internal(void * p_args); /// Default initialization function

#if ((1 == BSP_CFG_ERROR_LOG) || (1 == BSP_CFG_ASSERT))

/** Prototype of function called before errors are returned in FSP code if BSP_CFG_ERROR_LOG is set to 1.  This
 * prototype sets the weak association of this function to an internal example implementation. */

void fsp_error_log(fsp_err_t err, const char * file, int32_t line) WEAK_ERROR_ATTRIBUTE;

void fsp_error_log_internal(fsp_err_t err, const char * file, int32_t line); /// Default error logger function

#endif

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Get the FSP version based on compile time macros.
 *
 * @param[out] p_version        Memory address to return version information to.
 *
 * @retval FSP_SUCCESS          Version information stored.
 * @retval FSP_ERR_ASSERTION    The parameter p_version is NULL.
 **********************************************************************************************************************/
fsp_err_t R_FSP_VersionGet (fsp_pack_version_t * const p_version)
{
#if BSP_CFG_PARAM_CHECKING_ENABLE

    /** Verify parameters are valid */
    FSP_ASSERT(NULL != p_version);
#endif

    *p_version = g_fsp_version;

    return FSP_SUCCESS;
}

#if ((1 == BSP_CFG_ERROR_LOG) || (1 == BSP_CFG_ASSERT))

/*******************************************************************************************************************//**
 * Default error logger function, used only if fsp_error_log is not defined in the user application.
 *
 * @param[in]  err     The error code encountered.
 * @param[in]  file    The file name in which the error code was encountered.
 * @param[in]  line    The line number at which the error code was encountered.
 **********************************************************************************************************************/
void fsp_error_log_internal (fsp_err_t err, const char * file, int32_t line)
{
    /** Do nothing. Do not generate any 'unused' warnings. */
    FSP_PARAMETER_NOT_USED(err);
    FSP_PARAMETER_NOT_USED(file);
    FSP_PARAMETER_NOT_USED(line);
}

#endif

/** @} (end addtogroup BSP_MCU) */

/*******************************************************************************************************************//**
 * Default initialization function, used only if bsp_init is not defined in the user application.
 **********************************************************************************************************************/
void bsp_init_internal (void * p_args)
{
    /* Do nothing. */
    FSP_PARAMETER_NOT_USED(p_args);
}

#if defined(__ARMCC_VERSION)

/*******************************************************************************************************************//**
 * Default implementation of assert for AC6.
 **********************************************************************************************************************/
__attribute__((weak, noreturn))
void __aeabi_assert (const char * expr, const char * file, int line) {
    FSP_PARAMETER_NOT_USED(expr);
    FSP_PARAMETER_NOT_USED(file);
    FSP_PARAMETER_NOT_USED(line);
    __BKPT(0);
    while (1)
    {
        /* Do nothing. */
    }
}

#endif

/*******************************************************************************************************************//**
 * Allocate memory dynamically to heap.
 *
 * @param[in]   size    Size of memory allocated.
 *
 * @return      Returns a void pointer to the allocated area. If no memory is allocated, returns NULL.
 **********************************************************************************************************************/
void * bsp_prv_malloc (size_t size)
{
#if (2 == BSP_CFG_RTOS) && configSUPPORT_DYNAMIC_ALLOCATION

    /* Use FreeRTOS heap */
    return pvPortMalloc(size);
#else

    /* If RTOS dynamic allocation is disabled or RTOS is not in use, allocate heap data to the main heap. */
    return malloc(size);
#endif
}

/*******************************************************************************************************************//**
 * Free dynamically allocated memory on heap.
 *
 * @param[in]   ptr     Pointer to memory to be released
 **********************************************************************************************************************/
void bsp_prv_free (void * ptr)
{
#if (2 == BSP_CFG_RTOS) && configSUPPORT_DYNAMIC_ALLOCATION

    /* Use FreeRTOS heap */
    vPortFree(ptr);
#else

    /* If RTOS dynamic allocation is disabled or RTOS is not in use, use free(). */
    free(ptr);
#endif
}
