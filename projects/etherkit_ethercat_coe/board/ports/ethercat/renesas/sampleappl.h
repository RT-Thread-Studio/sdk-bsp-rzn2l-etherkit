/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
 * \addtogroup SampleAppl Sample Application
 * @{
 */

/**
\file sampleappl.h
\author EthercatSSC@beckhoff.com
\brief Sample application specific objects

\version 5.12

<br>Changes to version V5.11:<br>
V5.12 EOE4: handle 16bit only acceess, move ethernet protocol defines and structures to application header files<br>
<br>Changes to version V5.01:<br>
V5.11 COE1: update invalid end entry in the object dictionaries (error with some compilers)<br>
V5.11 ECAT4: enhance SM/Sync monitoring for input/output only slaves<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"
#include "cia402appl.h"
#include "sampleios.h"

#ifndef _SAMPLE_APPL_H_
#define _SAMPLE_APPL_H_


/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
#if (CiA402_SAMPLE_APPLICATION != 1)

/**
 * \addtogroup PdoMappingObjects PDO Mapping Objects
 * Input Counter PDO mapping : 0x1A00<br>
 * Ouput Counter PDO mapping : 0x1601
 * @{
 */
/** \brief 0x1603 (RxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[1]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1603;

/** \brief 0x1A03 (TxPDO) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT32   aEntries[1]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1A03;
/** @}*/


/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * SyncManager 2 : 0x1C12
 * <br>SyncManager 3 : 0x1C13
 * @{
 */
/** \brief 0x1C12 (SyncManager 2 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT16   aEntries[2]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1C12;

/** \brief 0x1C13 (SyncManager 3 assignment) data structure*/
typedef struct OBJ_STRUCT_PACKED_START {
   UINT16   u16SubIndex0; /**< \brief SubIndex 0*/
   UINT16   aEntries[2]; /**< \brief Entry buffer*/
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
/** @}*/

#ifdef _SAMPLE_APPLICATION_
    #define PROTO
#else
    #define PROTO extern
#endif


#ifdef _OBJD_
/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */
/**
 * \brief Entry descriptions of SyncManager assign objects
 *
 * SubIndex0<br>
 * SubIndex1 (for all other entries the same description will be used (because the object code is ARRAY))
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM asPDOAssignEntryDesc[] = {
   {DEFTYPE_UNSIGNED8, 0x08, (ACCESS_READ|ACCESS_WRITE_PREOP)},
   {DEFTYPE_UNSIGNED16, 0x10, (ACCESS_READ|ACCESS_WRITE_PREOP)}};
/** @}*/
#endif


/**
* \addtogroup PdoMappingObjects PDO Mapping Objects
* @{
*/
#ifdef _OBJD_
/**
 * \brief Object 0x1603 (RxPDO) entry descriptions
 *
 * SubIndex 0 <br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1603[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};


/**
 * \brief Object 0x1603 (RxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1603[] = "RxPDO-Map\000\377";
#endif //#ifdef _OBJD_


/**
 * \brief Object 0x1603 (RxPDO) variable to handle object data
 *
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x7010.0 32bit
 */
PROTO TOBJ1603 RxPDOMap
#ifdef _SAMPLE_APPLICATION_
 = {1, {0x70100020}}
#endif
;

#ifdef _OBJD_
/**
 * \brief Object 0x1A03 (TxPDO) entry descriptions
 *
 * SubIndex 0<br>
 * SubIndex 1
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A03[] = {
   {DEFTYPE_UNSIGNED8, 0x8, ACCESS_READ },
   {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ}};


/**
 * \brief Object 0x1A03 (TxPDO) object and entry names
 *
 * In this example no specific entry name is defined ("SubIndex xxx" is used)
 */
OBJCONST UCHAR OBJMEM aName0x1A03[] = "TxPDO-Map\000\377";
#endif //#ifdef _OBJD_

/**
 * \brief Object 0x1A03 (TxPDO) variable to handle object data
 *
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x6000.0 32bit
 */
PROTO TOBJ1A03 TxPDOMap
#ifdef _SAMPLE_APPLICATION_
 = {1, {0x60000020}}
#endif
;
/** @}*/

/**
 * \addtogroup SmAssignObjects SyncManager Assignment Objects
 * @{
 */

#ifdef _OBJD_
/**
 * \brief 0x1C12 (SyncManager 2 assignment) object name
 *
 * No entry names defined because the object code is ARRAY and all entry names are "SubIndex 000"
 */
OBJCONST UCHAR OBJMEM aName0x1C12[] = "RxPDO assign";
#endif //#ifdef _OBJD_

/**
 * \brief 0x1C12 (SyncManager 2 assignment) variable to handle object data
 *
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x1601
 */
PROTO TOBJ1C12 sRxPDOassign
#ifdef _SAMPLE_APPLICATION_
= {0x01, {0x1603}}
#endif
;



#ifdef _OBJD_
/**
 * \brief 0x1C13 (SyncManager 3 assignment) object name
 *
 * No entry names defined because the object code is ARRAY and all entry names are "SubIndex 000"
 */
OBJCONST UCHAR OBJMEM aName0x1C13[] = "TxPDO assign";
#endif //#ifdef _OBJD_


/**
 * \brief 0x1C13 (SyncManager 3 assignment) variable to handle object data
 *
 * SubIndex 0 : 1<br>
 * SubIndex 1 : 0x1A00
 */
PROTO TOBJ1C13 sTxPDOassign
#ifdef _SAMPLE_APPLICATION_
= {0x01, {0x1A03}}
#endif
;
/** @}*/


/**
 * \addtogroup ProcessDataObjects Process data objects
 *
 * Including the following objects:<br>
 * 0x6000 : 32bit input counter (unsigned)<br>
 * 0x7010 : 32bit output counter (unsigned)
 * @{
 */

#ifdef _OBJD_
/**
 * \brief Object 0x6000 (input counter) entry description
 *
 * This object is a variable so only one entry is defined
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x6000 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READ | OBJACCESS_TXPDOMAPPING};


/** \brief Object 0x6000 (input counter) name*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "InputCounter";
#endif //#ifdef _OBJD_


/** \brief Object 0x6000 (input counter) variable*/
PROTO UINT32 InputCounter
#ifdef _SAMPLE_APPLICATION_
= 0x0
#endif
;


#ifdef _OBJD_
/**
 * \brief Object 0x7010 (output counter) entry description
 *
 * This object is a variable so only one entry is defined
 */
OBJCONST TSDOINFOENTRYDESC    OBJMEM EntryDesc0x7010 = {DEFTYPE_UNSIGNED32, 0x20, ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING};


/** \brief Object 0x7010 (output counter) name*/
OBJCONST UCHAR OBJMEM aName0x7010[] = "OutputCounter";
#endif //#ifdef _OBJD_


 /** \brief Object 0x7010 (output counter) variable*/
PROTO UINT32 OutputCounter
#ifdef _SAMPLE_APPLICATION_
= 0x0
#endif
;
/** @}*/

#ifdef _OBJD_
/**
 * \brief Object dictionary related to the Sample Application
 *
 * For every application related object an object description is added to the dictionary
 */
/*if _PIC18 is enabled the object dictionary is fixed defined in coeappl.c*/
TOBJECT    OBJMEM ApplicationObjDic[] = {
   /* Object 0x1603 */
   {NULL,NULL,  0x1603, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1603, aName0x1603, &RxPDOMap, NULL, NULL, 0x0000 },
   /* Object 0x1A00 */
   {NULL,NULL, 0x1A03, {DEFTYPE_PDOMAPPING, 1 | (OBJCODE_REC << 8)}, asEntryDesc0x1A03, aName0x1A03, &TxPDOMap, NULL, NULL, 0x0000 },
    /* Object 0x1C12 */
   {NULL,NULL, 0x1C12, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C12, &sRxPDOassign, NULL, NULL, 0x0000 },
     /* Object 0x1C13 */
   {NULL,NULL, 0x1C13, {DEFTYPE_UNSIGNED16, 2 | (OBJCODE_ARR << 8)}, asPDOAssignEntryDesc, aName0x1C13, &sTxPDOassign, NULL, NULL, 0x0000 },
   /* Object 0x6000 */
   {NULL,NULL, 0x6000, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x6000, aName0x6000, &InputCounter, NULL, NULL, 0x0000 },
   /* Object 0x7010 */
   {NULL,NULL, 0x7010, {DEFTYPE_UNSIGNED32, 0 | (OBJCODE_VAR << 8)}, &EntryDesc0x7010, aName0x7010, &OutputCounter, NULL, NULL, 0x0000 },
   {NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL, NULL, 0x000}};
#endif    //#ifdef _OBJD_

#undef PROTO

#endif // #if (CiA402_SAMPLE_APPLICATION != 1)

#endif //_SAMPLE_APPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Prototype functions
------
-----------------------------------------------------------------------------------------*/

#define PROTO extern

PROTO void APPL_Application(void);
PROTO UINT16 APPL_GetDeviceID(void);
PROTO UINT16 APPL_GetDipSw(void);
PROTO void APPL_SetLed(UINT16 value);

PROTO void   APPL_AckErrorInd(UINT16 stateTrans);
PROTO UINT16 APPL_StartMailboxHandler(void);
PROTO UINT16 APPL_StopMailboxHandler(void);
PROTO UINT16 APPL_StartInputHandler(UINT16 *pIntMask);
PROTO UINT16 APPL_StopInputHandler(void);
PROTO UINT16 APPL_StartOutputHandler(void);
PROTO UINT16 APPL_StopOutputHandler(void);

PROTO UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize);
PROTO void APPL_InputMapping(UINT16* pData);
PROTO void APPL_OutputMapping(UINT16* pData);

#if (CiA402_SAMPLE_APPLICATION == 1)
PROTO void CiA402_Application(TCiA402Axis *pCiA402Axis, UINT16 i);
#endif

#undef PROTO
/** @}*/
