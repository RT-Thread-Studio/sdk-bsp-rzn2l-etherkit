/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics Corporation and/or its affiliates and may only
 * be used with products of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.
 * Renesas products are sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for
 * the selection and use of Renesas products and Renesas assumes no liability.  No license, express or implied, to any
 * intellectual property right is granted by Renesas.  This software is protected under all applicable laws, including
 * copyright laws. Renesas reserves the right to change or discontinue this software and/or this documentation.
 * THE SOFTWARE AND DOCUMENTATION IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND
 * TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY,
 * INCLUDING WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE
 * SOFTWARE OR DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR
 * DOCUMENTATION (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER,
 * INCLUDING, WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY
 * LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#ifndef ETHERCAT_RENESASHW_H
#define ETHERCAT_RENESASHW_H

/*******************************************************************************
 * Includes <System Includes> , "Project Includes"
 *******************************************************************************/
#include <string.h>

/*******************************************************************************
 * Typedef definitions
 *******************************************************************************/

/**
 * BOOL: Should be adapted to the boolean type of the microcontroller */
#ifndef BOOL
 #define BOOL        unsigned char
#endif

/**
 * UINT8: Should be adapted to the unsigned8 type of the microcontroller  */
#ifndef UINT8
 #define UINT8       unsigned char
#endif

/**
 * UINT16: Should be adapted to the unsigned16 type of the microcontroller  */
#ifndef UINT16
 #define UINT16      unsigned short
#endif

/**
 * UINT32: Should be adapted to the unsigned32 type of the microcontroller  */
#ifndef UINT32
 #define UINT32      unsigned long
#endif

/**
 * UINT64: Should be adapted to the unsigned64 type of the microcontroller  */
#ifndef UINT64
 #define UINT64      unsigned long long
#endif

/**
 * USHORT: Should be adapted to the unsigned16 type of the microcontroller */
#ifndef USHORT
 #define USHORT      unsigned short
#endif

/**
 * INT8: Should be adapted to the integer8 type of the microcontroller */
#ifndef INT8
 #define INT8        char
#endif

/**
 * INT16: Should be adapted to the integer16 type of the microcontroller  */
#ifndef INT16
 #define INT16       short
#endif

/**
 * INT32: Should be adapted to the integer32 type of the microcontroller */
#ifndef INT32
 #define INT32       long
#endif

/**
 * INT64: Should be adapted to the integer64 type of the microcontroller */
#ifndef INT64
 #define INT64       long long
#endif

/**
 * CHAR: Should be adapted to the character type of the microcontroller */
#ifndef CHAR
 #define CHAR        char
#endif

/**
 * UCHAR: Should be adapted to the unsigned character type of the microcontroller */
#ifndef UCHAR
 #define UCHAR       unsigned char
#endif

/**
 * MEM_ADDR: Type to access local memory addresses */
#ifndef MEM_ADDR
 #define MEM_ADDR    UINT8
#endif

/**
 * ESCMEM: Should be defined to select the memory type of the ESC memory (e.g. near, far or huge),
 * if the microcontroller does not support different memory types, ESCMEM shall be defined to nothing */
#ifndef ESCMEM
 #define ESCMEM
#endif

/*******************************************************************************
 * Macro definitions
 *******************************************************************************/
#define ESC_MEM_ADDR                  UINT8
#define ESC_MEM_SHIFT                 0

#define ESC_REG_ERROR_LED_OVERRIDE    (0x0139)
#define ESC_LED_OVERRIDE_OFF          (0x00)
#define ESC_LED_OFF                   (0x10)
#define ESC_LED_BLINKING              (0x1D)
#define ESC_LED_SINGLE_FLASH          (0x11)
#define ESC_LED_DOUBLE_FLASH          (0x12)
#define ESC_LED_FLICKERING            (0x1E)
#define ESC_LED_ON                    (0x1F)

#ifndef TIMER_INT_HEADER
 #define    TIMER_INT_HEADER
#endif

#define     ESC_RD    0x02             /* read access to ESC */
#define     ESC_WR    0x04             /* write access to ESC */

/*******************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 *******************************************************************************/

extern MEM_ADDR ESCMEM * pEsc;         // pointer to the ESC

UINT16 HW_GetTimer();
void   HW_ClearTimer();

#define ECAT_TIMER_INC_P_MS    1000    // the timer unit 1ms

void HW_SetLed(BOOL RunLed, BOOL ErrLed);

UINT16 HW_Init(void);
void   HW_Release(void);

#define     HW_GetALEventRegister()                     ((((UINT16 ESCMEM *) pEsc)[((ESC_AL_EVENT_OFFSET) >> 1)]))
#define     HW_GetALEventRegister_Isr    HW_GetALEventRegister

#define     HW_EscRead(pData, Address, Len)             ESCMEMCPY((MEM_ADDR *) (pData),                             \
                                                                  &((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >>     \
                                                                                                   ESC_MEM_SHIFT)], \
                                                                  (Len))
#define     HW_EscReadIsr                HW_EscRead
#define     HW_EscReadDWord(DWordValue, Address)        ((DWordValue) = (UINT32) (((UINT32 *) pEsc)[(Address >> 2)]))
#define     HW_EscReadDWordIsr(DWordValue, Address)     ((DWordValue) = (UINT32) (((UINT32 *) pEsc)[(Address >> 2)]))
#define     HW_EscReadWord(WordValue, Address)          ((WordValue) = (((UINT16 *) pEsc)[((Address) >> 1)])) // the esc pointer is handled as a word pointer so the address must be devided by 2
#define     HW_EscReadWordIsr(WordValue, Address)       HW_EscReadWord(WordValue, Address)                    // the esc pointer is handled as a word pointer so the address must be devided by 2
#define     HW_EscReadByte(ByteValue, Address)          ((ByteValue) = (((UINT8 *) pEsc)[(Address)]))
#define     HW_EscReadByteIsr(ByteValue, Address)       HW_EscReadByte(ByteValue, Address)

/* ECATCHANGE_START(V5.01) HW2*/
#define     HW_EscReadMbxMem(pData, Address, Len)       ESCMBXMEMCPY((MEM_ADDR *) (pData),                             \
                                                                     &((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >>     \
                                                                                                      ESC_MEM_SHIFT)], \
                                                                     (Len))

#define     HW_EscWrite(pData, Address,                                                                             \
                        Len)                            ESCMEMCPY(&((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >>     \
                                                                                                   ESC_MEM_SHIFT)], \
                                                                  (MEM_ADDR *) (pData),                             \
                                                                  (Len))

/* ECATCHANGE_END(V5.01) HW2*/
#define     HW_EscWriteIsr    HW_EscWrite
#define     HW_EscWriteDWord(DWordValue, Address)       ((((UINT32 *) pEsc)[(Address >> 2)]) = (DWordValue))
#define     HW_EscWriteDWordIsr(DWordValue, Address)    ((((UINT32 *) pEsc)[(Address >> 2)]) = (DWordValue))
#define     HW_EscWriteWord(WordValue, Address)         ((((UINT16 *) pEsc)[((Address) >> 1)]) = (WordValue)) // the esc pointer is handled as a word pointer so the address must be devided by 2
#define     HW_EscWriteWordIsr(WordValue, Address)      HW_EscWriteWord(WordValue, Address)                   // the esc pointer is handled as a word pointer so the address must be devided by 2
#define     HW_EscWriteByte(ByteValue, Address)         ((((UINT8 *) pEsc)[(Address)]) = (ByteValue))
#define     HW_EscWriteByteIsr(ByteValue, Address)      HW_EscWriteByte(ByteValue, Address)
#define     HW_EscWriteMbxMem(pData, Address,                                                                          \
                              Len)                      ESCMBXMEMCPY(&((ESC_MEM_ADDR ESCMEM *) pEsc)[((Address) >>     \
                                                                                                      ESC_MEM_SHIFT)], \
                                                                     (MEM_ADDR *) (pData),                             \
                                                                     (Len))

void DISABLE_ESC_INT();
void ENABLE_ESC_INT();
void HW_IncTimer(void);

#endif                                 // ETHERCAT_RENESASHW_H
