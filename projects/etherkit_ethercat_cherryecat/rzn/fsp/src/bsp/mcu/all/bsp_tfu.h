/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RENESAS_TFU
#define RENESAS_TFU

/***********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/* Mathematical Functions includes. */
#ifdef __cplusplus
 #include <cmath>
#else
 #include <math.h>
#endif

/** Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define R_TFU_HYPOT_SCALING_FACTOR          0.607252935F
#define R_TFU_HYPOT_SCALING_FACTOR_FIXED    326016448
#define FIXED_POINT_FRACTIONAL_BITS         29U

#ifdef __GNUC__                        /* and (arm)clang */
 #if (__STDC_VERSION__ < 199901L) && defined(__STRICT_ANSI__) && !defined(__cplusplus)

/* No form of inline is available, it happens only when -std=c89, gnu89 and
 * above are OK */
  #warning \
    "-std=c89 doesn't support type checking on TFU. Please use -std=gnu89 or higher for example -std=c99"
 #else
  #ifdef __GNUC_GNU_INLINE__

/* gnu89 semantics of inline and extern inline are essentially the exact
 * opposite of those in C99 */
   #define BSP_TFU_INLINE    extern inline __attribute__((always_inline))
  #else                                /* __GNUC_STDC_INLINE__ */
   #define BSP_TFU_INLINE    static inline __attribute__((always_inline))
  #endif
 #endif
#elif __ICCARM__
 #define BSP_TFU_INLINE
#else
 #error "Compiler not supported!"
#endif

#if BSP_CFG_USE_TFU_MATHLIB
 #define sinf(x)                     __sinf(x)
 #define cosf(x)                     __cosf(x)
 #define atan2f(y, x)                __atan2f(y, x)
 #define hypotf(x, y)                __hypotf(x, y)
 #define atan2hypotf(y, x, a, h)     __atan2hypotf(y, x, a, h)
 #define sincosf(a, s, c)            __sincosf(a, s, c)
 #define sinfx(x)                    __sinfx(x)
 #define cosfx(x)                    __cosfx(x)
 #define atan2fx(y, x)               __atan2fx(y, x)
 #define hypotfx(x, y)               __hypotfx(x, y)
 #define atan2hypotfx(y, x, a, h)    __atan2hypotfx(y, x, a, h)
 #define sincosfx(a, s, c)           __sincosfx(a, s, c)
#endif

#if BSP_CFG_TFU_DATA_SAVE_RESTORE_ENABLE
 #define BSP_CFG_TFU_SAVE_ENABLE       BSP_TFU_SAVE_ENABLE
 #define BSP_CFG_TFU_RESTORE_ENABLE    BSP_TFU_RESTORE_ENABLE
#else
 #define BSP_CFG_TFU_SAVE_ENABLE
 #define BSP_CFG_TFU_RESTORE_ENABLE
#endif

#if 2 == BSP_FEATURE_TFU_VERSION
 #if (1 == BSP_FEATURE_TFU_UNIT)
  #define BSP_TFU_DTSR0_ADDRESS     (&R_TFU->DTSR0)
  #define BSP_TFU_DTSR1_ADDRESS     (&R_TFU->DTSR1)
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)
   #define BSP_TFU_DTSR0_ADDRESS    (&R_TFU0->DTSR0)
   #define BSP_TFU_DTSR1_ADDRESS    (&R_TFU0->DTSR1)
  #else
   #define BSP_TFU_DTSR0_ADDRESS    (&R_TFU1->DTSR0)
   #define BSP_TFU_DTSR1_ADDRESS    (&R_TFU1->DTSR1)
  #endif
 #endif

 #define BSP_TFU_SAVE_ENABLE        uint32_t dtsr0 = *(volatile uint32_t *) BSP_TFU_DTSR0_ADDRESS; \
    uint32_t dtsr1 = *(volatile uint32_t *) BSP_TFU_DTSR1_ADDRESS;                                 \
    __asm volatile (                                                                               \
        "PUSH {%[DTSR0]}      \n"                                                                  \
        "PUSH {%[DTSR1]}      \n"                                                                  \
        ::[DTSR0] "r" (dtsr0), [DTSR1] "r" (dtsr1) : "memory");

 #define BSP_TFU_RESTORE_ENABLE     __asm volatile (            \
        "POP {%[DTSR1]}      \n"                                \
        "POP {%[DTSR0]}      \n"                                \
        :[DTSR1] "=r" (dtsr1), [DTSR0] "=r" (dtsr0)::"memory"); \
    *(volatile uint32_t *) BSP_TFU_DTSR0_ADDRESS = dtsr0;       \
    *(volatile uint32_t *) BSP_TFU_DTSR1_ADDRESS = dtsr1;
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
#if (1 == BSP_LP64_SUPPORT)
extern float g_bsp_tfu_convert_result;
#endif

/***********************************************************************************************************************
 * Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Inline Functions
 **********************************************************************************************************************/

#if (1 == BSP_LP64_SUPPORT)

/*******************************************************************************************************************//**
 * Convert unsigned int type to float type.
 * @param[in]    target_value  Value before conversion.
 *
 * @retval Value after conversion.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float bsp_prv_conversion_uint_to_float (uint32_t target_value)
{
    __asm volatile (
        "ADR  x9,  g_bsp_tfu_convert_result    \n"
        "MOV  x10, %0                          \n"
        "FMOV s0, w10                          \n"
        "STR  s0, [x9]                         \n"
        ::"r" (target_value) : "memory");

    return g_bsp_tfu_convert_result;
}

#endif

#if 1 == BSP_FEATURE_TFU_VERSION

/*******************************************************************************************************************//**
 * Calculates sine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 *
 * @retval Sine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __sinf (float angle)
{
    /* Set the angle to R_TFU->SCDT1 */
    R_TFU->SCDT1 = angle;

    /* Read sin from R_TFU->SCDT1 */
    return R_TFU->SCDT1;
}

/*******************************************************************************************************************//**
 * Calculates cosine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 *
 * @retval Cosine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __cosf (float angle)
{
    /* Set the angle to R_TFU->SCDT1 */
    R_TFU->SCDT1 = angle;

    /* Read cos from R_TFU->SCDT1 */
    return R_TFU->SCDT0;
}

/*******************************************************************************************************************//**
 * Calculates sine and cosine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 * @param[out]   sin    Sine value of an angle.
 * @param[out]   cos    Cosine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE void __sincosf (float angle, float * sin, float * cos)
{
    /* Set the angle to R_TFU->SCDT1 */
    R_TFU->SCDT1 = angle;

    /* Read sin from R_TFU->SCDT1 */
    *sin = R_TFU->SCDT1;

    /* Read cos from R_TFU->SCDT0 */
    *cos = R_TFU->SCDT0;
}

/*******************************************************************************************************************//**
 * Calculates the arc tangent based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-Axis cordinate value.
 * @param[in]    x_cord  X-Axis cordinate value.
 *
 * @retval Arc tangent for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __atan2f (float y_cord, float x_cord)
{
    /* Set X-cordinate to R_TFU->ATDT0 */
    R_TFU->ATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->ATDT1 */
    R_TFU->ATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->ATDT1 */
    return R_TFU->ATDT1;
}

/*******************************************************************************************************************//**
 * Calculates the hypotenuse based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-cordinate value.
 * @param[in]    x_cord  X-cordinate value.
 *
 * @retval Hypotenuse for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __hypotf (float x_cord, float y_cord)
{
    /* Set X-coordinate to R_TFU->ATDT0 */
    R_TFU->ATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->ATDT1 */
    R_TFU->ATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->ATDT0 */
    return R_TFU->ATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
}

/*******************************************************************************************************************//**
 * Calculates the arc tangent and hypotenuse based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-cordinate value.
 * @param[in]    x_cord  X-cordinate value.
 * @param[out]   atan2   Arc tangent for given values.
 * @param[out]   hypot   Hypotenuse for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE void __atan2hypotf (float y_cord, float x_cord, float * atan2, float * hypot)
{
    /* Set X-coordinate to R_TFU->ATDT0 */
    R_TFU->ATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->ATDT1 */
    R_TFU->ATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->ATDT1 */
    *atan2 = R_TFU->ATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->ATDT0 */
    *hypot = R_TFU->ATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
}

#else

/*******************************************************************************************************************//**
 * Calculates sine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 *
 * @retval Sine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __sinf (float angle)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU->FPSCDT1 = angle;

    /* Read sin from R_TFU->FPSCDT1 */
    return R_TFU->FPSCDT1;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU0->FPSCDT1 = angle;

    /* Read sin from R_TFU->FPSCDT1 */
    return R_TFU0->FPSCDT1;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU1->FPSCDT1 = angle;

    /* Read sin from R_TFU->FPSCDT1 */
    return R_TFU1->FPSCDT1;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates cosine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 *
 * @retval Cosine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __cosf (float angle)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU->FPSCDT1 = angle;

    /* Read cos from R_TFU->FPSCDT0 */
    return R_TFU->FPSCDT0;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU0->FPSCDT1 = angle;

    /* Read cos from R_TFU->FPSCDT0 */
    return R_TFU0->FPSCDT0;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU1->FPSCDT1 = angle;

    /* Read cos from R_TFU->FPSCDT0 */
    return R_TFU1->FPSCDT0;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates sine and cosine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 * @param[out]   sin    Sine value of an angle.
 * @param[out]   cos    Cosine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE void __sincosf (float angle, float * sin, float * cos)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU->FPSCDT1 = angle;

    /* Read sin from R_TFU->FPSCDT1 */
    *sin = R_TFU->FPSCDT1;

    /* Read sin from R_TFU->FPSCDT0 */
    *cos = R_TFU->FPSCDT0;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU0->FPSCDT1 = angle;

    /* Read sin from R_TFU->FPSCDT1 */
    *sin = R_TFU0->FPSCDT1;

    /* Read sin from R_TFU->FPSCDT0 */
    *cos = R_TFU0->FPSCDT0;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FPSCDT1 */
    R_TFU1->FPSCDT1 = angle;

    /* Read sin from R_TFU->FPSCDT1 */
    *sin = R_TFU1->FPSCDT1;

    /* Read sin from R_TFU->FPSCDT0 */
    *cos = R_TFU1->FPSCDT0;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates the arc tangent based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-Axis cordinate value.
 * @param[in]    x_cord  X-Axis cordinate value.
 *
 * @retval Arc tangent for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __atan2f (float y_cord, float x_cord)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set X-cordinate to R_TFU->FPATDT0 */
    R_TFU->FPATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->FPATDT1 */
    R_TFU->FPATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FPATDT1 */
    return R_TFU->FPATDT1;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-cordinate to R_TFU->FPATDT0 */
    R_TFU0->FPATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->FPATDT1 */
    R_TFU0->FPATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FPATDT1 */
    return R_TFU0->FPATDT1;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-cordinate to R_TFU->FPATDT0 */
    R_TFU1->FPATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->FPATDT1 */
    R_TFU1->FPATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FPATDT1 */
    return R_TFU1->FPATDT1;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates the hypotenuse based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-cordinate value.
 * @param[in]    x_cord  X-cordinate value.
 *
 * @retval Hypotenuse for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE float __hypotf (float x_cord, float y_cord)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set X-coordinate to R_TFU->FPATDT0 */
    R_TFU->FPATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FPATDT1 */
    R_TFU->FPATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FPATDT0 */
    return R_TFU->FPATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FPATDT0 */
    R_TFU0->FPATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FPATDT1 */
    R_TFU0->FPATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FPATDT0 */
    return R_TFU0->FPATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FPATDT0 */
    R_TFU1->FPATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FPATDT1 */
    R_TFU1->FPATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FPATDT0 */
    return R_TFU1->FPATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates the arc tangent and hypotenuse based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-cordinate value.
 * @param[in]    x_cord  X-cordinate value.
 * @param[out]   atan2   Arc tangent for given values.
 * @param[out]   hypot   Hypotenuse for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE void __atan2hypotf (float y_cord, float x_cord, float * atan2, float * hypot)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set X-coordinate to R_TFU->FPATDT0 */
    R_TFU->FPATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FPATDT1 */
    R_TFU->FPATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FPATDT1 */
    *atan2 = R_TFU->FPATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FPATDT0 */
    *hypot = R_TFU->FPATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FPATDT0 */
    R_TFU0->FPATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FPATDT1 */
    R_TFU0->FPATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FPATDT1 */
    *atan2 = R_TFU0->FPATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FPATDT0 */
    *hypot = R_TFU0->FPATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FPATDT0 */
    R_TFU1->FPATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FPATDT1 */
    R_TFU1->FPATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FPATDT1 */
    *atan2 = R_TFU1->FPATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FPATDT0 */
    *hypot = R_TFU1->FPATDT0 * R_TFU_HYPOT_SCALING_FACTOR;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates sine of the given angle.
 * @param[in]    angle  The value of an angle.
 *
 * @retval Sine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE uint32_t __sinfx (uint32_t angle)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT1 */
    return R_TFU->FXSCDT1;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU0->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT1 */
    return R_TFU0->FXSCDT1;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU1->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT1 */
    return R_TFU1->FXSCDT1;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates cosine of the given angle.
 * @param[in]    angle  The value of an angle in radian.
 *
 * @retval Cosine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE uint32_t __cosfx (uint32_t angle)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT0 */
    return R_TFU->FXSCDT0;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU0->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT0 */
    return R_TFU0->FXSCDT0;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU1->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT0 */
    return R_TFU1->FXSCDT0;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates sine and cosine of the given angle.
 * @param[in]    angle  The value of an angle.
 * @param[out]   sin    Sine value of an angle.
 * @param[out]   cos    Cosine value of an angle.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE void __sincosfx (uint32_t angle, uint32_t * sin, uint32_t * cos)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT1 */
    *sin = R_TFU->FXSCDT1;

    /* Read sin from R_TFU->FXSCDT0 */
    *cos = R_TFU->FXSCDT0;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU0->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT1 */
    *sin = R_TFU0->FXSCDT1;

    /* Read sin from R_TFU->FXSCDT0 */
    *cos = R_TFU0->FXSCDT0;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set the angle to R_TFU->FXSCDT1 */
    R_TFU1->FXSCDT1 = angle;

    /* Read sin from R_TFU->FXSCDT1 */
    *sin = R_TFU1->FXSCDT1;

    /* Read sin from R_TFU->FXSCDT0 */
    *cos = R_TFU1->FXSCDT0;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates the arc tangent based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-Axis cordinate value.
 * @param[in]    x_cord  X-Axis cordinate value.
 *
 * @retval Arc tangent for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE uint32_t __atan2fx (uint32_t y_cord, uint32_t x_cord)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set X-cordinate to R_TFU->FXATDT0 */
    R_TFU->FXATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->FXATDT1 */
    R_TFU->FXATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FXATDT1 */
    return R_TFU->FXATDT1;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-cordinate to R_TFU->FXATDT0 */
    R_TFU0->FXATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->FXATDT1 */
    R_TFU0->FXATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FXATDT1 */
    return R_TFU0->FXATDT1;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-cordinate to R_TFU->FXATDT0 */
    R_TFU1->FXATDT0 = x_cord;

    /* set Y-cordinate to R_TFU->FXATDT1 */
    R_TFU1->FXATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FXATDT1 */
    return R_TFU1->FXATDT1;
  #endif
 #endif
}

/*******************************************************************************************************************//**
 * Calculates the hypotenuse based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-cordinate value.
 * @param[in]    x_cord  X-cordinate value.
 *
 * @retval Hypotenuse for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE int32_t __hypotfx (uint32_t x_cord, uint32_t y_cord)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set X-coordinate to R_TFU->FXATDT0 */
    R_TFU->FXATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FXATDT1 */
    R_TFU->FXATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FXATDT0 */
    int64_t calc_hypotValue = (int64_t) R_TFU->FXATDT0 * R_TFU_HYPOT_SCALING_FACTOR_FIXED;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FXATDT0 */
    R_TFU0->FXATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FXATDT1 */
    R_TFU0->FXATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FXATDT0 */
    int64_t calc_hypotValue = (int64_t) R_TFU0->FXATDT0 * R_TFU_HYPOT_SCALING_FACTOR_FIXED;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FXATDT0 */
    R_TFU1->FXATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FXATDT1 */
    R_TFU1->FXATDT1 = y_cord;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FXATDT0 */
    int64_t calc_hypotValue = (int64_t) R_TFU1->FXATDT0 * R_TFU_HYPOT_SCALING_FACTOR_FIXED;
  #endif
 #endif

    return (int32_t) (calc_hypotValue >> FIXED_POINT_FRACTIONAL_BITS);
}

/*******************************************************************************************************************//**
 * Calculates the arc tangent and hypotenuse based on given X-cordinate and Y-cordinate values.
 * @param[in]    y_cord  Y-cordinate value.
 * @param[in]    x_cord  X-cordinate value.
 * @param[out]   atan2   Arc tangent for given values.
 * @param[out]   hypot   Hypotenuse for given values.
 **********************************************************************************************************************/
 #if __ICCARM__
  #pragma inline = forced
 #endif
BSP_TFU_INLINE void __atan2hypotfx (uint32_t y_cord, uint32_t x_cord, uint32_t * atan2, int32_t * hypot)
{
 #if (1 == BSP_FEATURE_TFU_UNIT)

    /* Set X-coordinate to R_TFU->FXATDT0 */
    R_TFU->FXATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FXATDT1 */
    R_TFU->FXATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FXATDT1 */
    *atan2 = R_TFU->FXATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FXATDT0 */
    int64_t calc_hypotValue = (int64_t) R_TFU->FXATDT0 * R_TFU_HYPOT_SCALING_FACTOR_FIXED;
 #else
  #if (0 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FXATDT0 */
    R_TFU0->FXATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FXATDT1 */
    R_TFU0->FXATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FXATDT1 */
    *atan2 = R_TFU0->FXATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FXATDT0 */
    int64_t calc_hypotValue = (int64_t) R_TFU0->FXATDT0 * R_TFU_HYPOT_SCALING_FACTOR_FIXED;
  #elif (1 == BSP_FEATURE_TFU_UNIT_NUMBER)

    /* Set X-coordinate to R_TFU->FXATDT0 */
    R_TFU1->FXATDT0 = x_cord;

    /* set Y-coordinate to R_TFU->FXATDT1 */
    R_TFU1->FXATDT1 = y_cord;

    /* Read arctan(y/x) from R_TFU->FXATDT1 */
    *atan2 = R_TFU1->FXATDT1;

    /* Read sqrt (x_cord2 + y_cord2) from R_TFU->FXATDT0 */
    int64_t calc_hypotValue = (int64_t) R_TFU1->FXATDT0 * R_TFU_HYPOT_SCALING_FACTOR_FIXED;
  #endif
 #endif
    *hypot = (int32_t) (calc_hypotValue >> FIXED_POINT_FRACTIONAL_BITS);
}

#endif

/** @} (end addtogroup BSP_MCU) */

/** Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 /* RENESAS_TFU */
