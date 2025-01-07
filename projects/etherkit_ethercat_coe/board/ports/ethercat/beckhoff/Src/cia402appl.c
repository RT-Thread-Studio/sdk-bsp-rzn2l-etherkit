/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup CiA402appl CiA402 Sample Application
@{
*/

/**
\file cia402appl.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains all ciA402 specific functions

\version 5.13

<br>Changes to version V5.12:<br>
V5.13 CIA402 2: write profile info to 0xF010.x bit15-0 (was bit16-31 before)<br>
V5.13 CIA402 3: change define "CIA402_DEVICE" to "CiA402_SAMPLE_APPLICATION"<br>
<br>Changes to version V5.11:<br>
V5.12 COE3: update entry access right handling<br>
<br>Changes to version V5.10:<br>
V5.11 ECAT11: create application interface function pointer, add eeprom emulation interface functions<br>
<br>Changes to version V5.01:<br>
V5.10 CIA402 1: Update complete access handling for 0xF030<br>
V5.10 ECAT6: Add "USE_DEFAULT_MAIN" to enable or disable the main function<br>
<br>Changes to version V5.0:<br>
V5.01 ESC2: Add missed value swapping<br>
<br>Changes to version V4.40:<br>
V5.0 CIA402 1: Syntax bugfix in dummy motion controller<br>
V5.0 CIA402 2: Handle 0xF030/0xF050 in correlation do PDO assign/mapping objects<br>
V5.0 CIA402 3: Trigger dummy motion controller if valid mode of operation is set.<br>
V5.0 CIA402 4: Change Axes structure handling and resources allocation.<br>
V5.0 ECAT2: Create generic application interface functions. Documentation in Application Note ET9300.<br>
<br>Changes to version V4.30:<br>
V4.40 CoE 6: add AL Status code to Init functions<br>
V4.40 CIA402 2: set motion control trigger depending on "Synchronisation", "mode of operation" and "cycle time"<br>
V4.40 CIA402 1: change behaviour and name of bit12 of the status word (0x6041) (WG CIA402 24.02.2010)<br>
V4.30 : create file (state machine; handling state transition options; input feedback)
*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

/*ECATCHANGE_START(V5.13) CIA402 3*/
/*ECATCHANGE_END(V5.13) CIA402 3*/

#include "applInterface.h"


#include "coeappl.h"

#define _CiA402_
#include "cia402appl.h"
#undef _CiA402_

#include "hal_data.h"

#if (CiA402_SAMPLE_APPLICATION == 1)
/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/
TCiA402Axis       LocalAxes[MAX_AXES];

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    0               Init CiA402 device successful
            ALSTATUSCODE_XX Init CiA402 device failed

 \brief    This function initializes the Axes structures
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 CiA402_Init(void)
{
    UINT16 result = 0;
    UINT16 AxisCnt = 0;
    UINT16 j = 0;
    UINT32 ObjectOffset = 0x800;
    UINT8 PDOOffset = 0x10;

    for(AxisCnt = 0; AxisCnt < MAX_AXES ; AxisCnt++)
    {
        /*Reset Axis buffer*/
        HMEMSET(&LocalAxes[AxisCnt],0,SIZEOF(TCiA402Axis));

        LocalAxes[AxisCnt].bAxisIsActive = FALSE;
        LocalAxes[AxisCnt].bBrakeApplied = TRUE;
        LocalAxes[AxisCnt].bLowLevelPowerApplied = TRUE;
        LocalAxes[AxisCnt].bHighLevelPowerApplied = FALSE;
        LocalAxes[AxisCnt].bAxisFunctionEnabled = FALSE;
        LocalAxes[AxisCnt].bConfigurationAllowed = TRUE;
        LocalAxes[AxisCnt].u16AxisNum = AxisCnt+1;
        LocalAxes[AxisCnt].i16State = STATE_NOT_READY_TO_SWITCH_ON;
        LocalAxes[AxisCnt].u16PendingOptionCode = 0x00;

        LocalAxes[AxisCnt].fCurPosition = 0;
        LocalAxes[AxisCnt].u32CycleTime = 0;


        /***********************************
            init objects
        *************************************/

        /*set default values*/
        HMEMCPY(&LocalAxes[AxisCnt].Objects,&DefCiA402ObjectValues,CIA402_OBJECTS_SIZE);

        /***set Object offset to PDO entries***/

        /*csv/csp RxPDO*/
        for (j = 0; j < LocalAxes[AxisCnt].Objects.sRxPDOMap0.u16SubIndex0; j++)
        {
            LocalAxes[AxisCnt].Objects.sRxPDOMap0.aEntries[j] += AxisCnt* (ObjectOffset << 16);
        }

        /*csp RxPDO*/
        for (j = 0; j < LocalAxes[AxisCnt].Objects.sRxPDOMap1.u16SubIndex0; j++)
        {
            LocalAxes[AxisCnt].Objects.sRxPDOMap1.aEntries[j] += AxisCnt* (ObjectOffset << 16);
        }

        /*csv RxPDO*/
        for (j = 0; j < LocalAxes[AxisCnt].Objects.sRxPDOMap2.u16SubIndex0; j++)
        {
            LocalAxes[AxisCnt].Objects.sRxPDOMap2.aEntries[j] += AxisCnt* (ObjectOffset << 16);
        }


        /*csv/csp TxPDO*/
        for (j = 0; j < LocalAxes[AxisCnt].Objects.sTxPDOMap0.u16SubIndex0; j++)
        {
            LocalAxes[AxisCnt].Objects.sTxPDOMap0.aEntries[j] += AxisCnt* (ObjectOffset << 16);
        }

        /*csp TxPDO*/
        for (j = 0; j < LocalAxes[AxisCnt].Objects.sTxPDOMap1.u16SubIndex0; j++)
        {
            LocalAxes[AxisCnt].Objects.sTxPDOMap1.aEntries[j] += AxisCnt* (ObjectOffset << 16);
        }

        /*csv TxPDO*/
        for (j = 0; j < LocalAxes[AxisCnt].Objects.sTxPDOMap2.u16SubIndex0; j++)
        {
            LocalAxes[AxisCnt].Objects.sTxPDOMap2.aEntries[j] += AxisCnt* (ObjectOffset << 16);
        }



        /***********************************
            init objects dictionary entries
        *************************************/
        LocalAxes[AxisCnt].ObjDic = (TOBJECT *) ALLOCMEM(SIZEOF(DefCiA402AxisObjDic));
        HMEMCPY(LocalAxes[AxisCnt].ObjDic,&DefCiA402AxisObjDic,SIZEOF(DefCiA402AxisObjDic));
        {
        TOBJECT OBJMEM *pDiCEntry = LocalAxes[AxisCnt].ObjDic;

        /*adapt Object index and assign Var pointer*/
        while(pDiCEntry->Index != 0xFFFF)
        {
            BOOL bObjectFound = TRUE;

            switch(pDiCEntry->Index)
            {
            case 0x1600:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.sRxPDOMap0;
                break;
            case 0x1601:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.sRxPDOMap1;
                break;
            case 0x1602:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.sRxPDOMap2;
                break;
            case 0x1A00:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.sTxPDOMap0;
                break;
            case 0x1A01:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.sTxPDOMap1;
                break;
            case 0x1A02:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.sTxPDOMap2;
                break;
            case 0x603F:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objErrorCode;
                break;
            case 0x6040:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objControlWord;
                break;
            case 0x6041:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objStatusWord;
                break;
            case 0x605A:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objQuickStopOptionCode;
                break;
            case 0x605B:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objShutdownOptionCode;
                break;
            case 0x605C:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objDisableOperationOptionCode;
                break;
            case 0x605D:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objHaltOptionCode;
                break;
            case 0x605E:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objFaultReactionCode;
                break;
            case 0x6060:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objModesOfOperation;
                break;
            case 0x6061:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objModesOfOperationDisplay;
                break;
            case 0x6062:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositionDemandValue;
                break;
            case 0x6063:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositionActualInternalValue;
                break;
            case 0x6064:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositionActualValue;
                break;
            case 0x6065:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objFollowingErrorWindow;
                break;
            case 0x6066:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objFollowingErrorTimeOut;
                break;
            case 0x6067:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositionWindow;
                break;
            case 0x606C:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objVelocityActualValue;
                break;
            case 0x6072:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objMaxTorque;
                break;
            case 0x6077:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTorqueActualValue;
                break;
            case 0x607A:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTargetPosition;
                break;
            case 0x607B:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositionRangeLimit;
                break;
            case 0x607C:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objHomeOffset;
                break;
            case 0x607D:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objSoftwarePositionLimit;
                break;
            case 0x6085:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objQuickStopDeclaration;
                break;
            case 0x6091:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objGearRatio;
                break;
            case 0x6099:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objHomingSpeeds;
                break;
            case 0x60B0:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositionOffset;
                break;
            case 0x60B1:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objVelocityOffset;
                break;
            case 0x60B2:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTorqueOffset;
                break;
            case 0x60B8:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTouchProbeFunction;
                break;
            case 0x60B9:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTouchProbeStatus;
                break;
            case 0x60BA:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTouchProbePos1PosValue;
                break;
            case 0x60BB:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.ObjTouchProbePos1NegValue;
                break;
            case 0x60C2:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objInterpolationTimePeriod;
                break;
            case 0x60D0:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTouchProbeSource;
                break;
            case 0x60E0:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objPositiveTorqueLimitValue;
                break;
            case 0x60E1:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objNegativeTorqueLimitValue;
                break;
            case 0x60F4:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objFollowingErrorActualValue;
                break;
            case 0x60FD:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objDigitalInputs;
                break;
            case 0x60FF:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objTargetVelocity;
                break;
            case 0x6402:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objMotorType;
                break;
            case 0x6502:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objSupportedDriveModes;
                break;
            case 0x60FE:
                pDiCEntry->pVarPtr = &LocalAxes[AxisCnt].Objects.objDigitalOutputs;
                break;
            default :
                bObjectFound = FALSE;
            break;
            }//switch(pDiCEntry->Index)

            /*increment object index*/
            if (pDiCEntry->Index >= 0x1400 && pDiCEntry->Index <= 0x1BFF)    //PDO region
            {
            	pDiCEntry->Index += AxisCnt* PDOOffset;
            }
            else
            {
            	pDiCEntry->Index += AxisCnt* (UINT16)ObjectOffset;
            }

            pDiCEntry++;

            FSP_PARAMETER_NOT_USED(bObjectFound);
        }//while(pDiCEntry->Index != 0xFFFF)

        }
    }// for "all active axes"

    return result;
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    CiA402_DeallocateAxis
 \brief    Remove all allocated axes resources
*////////////////////////////////////////////////////////////////////////////////////////
void CiA402_DeallocateAxis(void)
{
    UINT8 cnt = 0;

    for(cnt = 0 ; cnt < MAX_AXES ; cnt++)
    {
    /*Remove object dictionary entries*/
    if(LocalAxes[cnt].ObjDic != NULL)
    {
        TOBJECT OBJMEM *pEntry = LocalAxes[cnt].ObjDic;

        while(pEntry->Index != 0xFFFF)
        {
            COE_RemoveDicEntry(pEntry->Index);

            pEntry++;
        }

        FREEMEM(LocalAxes[cnt].ObjDic);
    }

    nPdOutputSize = 0;
    nPdInputSize = 0;

    }

}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    CiA402-Statemachine
        This function handles the state machine for devices using the CiA402 profile.
        called cyclic from MainLoop()
        All described transition numbers are referring to the document
        "ETG Implementation Guideline for the CiA402 Axis Profile" located on the EtherCAT.org download section

*////////////////////////////////////////////////////////////////////////////////////////
void CiA402_StateMachine(void)
{
    TCiA402Axis *pCiA402Axis;
    UINT16 StatusWord = 0;
    UINT16 ControlWord6040 = 0;
    UINT16 counter = 0;

    for(counter = 0; counter < MAX_AXES;counter++)
    {
        if(!LocalAxes[counter].bAxisIsActive)
        {
            continue;
        }

        pCiA402Axis = &LocalAxes[counter];
        StatusWord = pCiA402Axis->Objects.objStatusWord;
        ControlWord6040 = pCiA402Axis->Objects.objControlWord;

        /*clear statusword state and controlword processed complete bits*/
        StatusWord &= ~(STATUSWORD_STATE_MASK | STATUSWORD_REMOTE);

        /*skip state state transition if the previous transition is pending*/
        if(pCiA402Axis->u16PendingOptionCode!= 0x0)
        {
            return;
        }
        /*skip transition 1 and 2*/
        if(pCiA402Axis->i16State < STATE_READY_TO_SWITCH_ON && nAlStatus == STATE_OP){
            if(pCiA402Axis->i16State == STATE_NOT_READY_TO_SWITCH_ON){
                CiA402_StateTransition1(pCiA402Axis);
				CiA402_StateTransition2(pCiA402Axis);
            }
            if(pCiA402Axis->i16State == STATE_SWITCH_ON_DISABLED){
                CiA402_StateTransition2(pCiA402Axis);
            }

            pCiA402Axis->i16State = STATE_READY_TO_SWITCH_ON;
        }

        switch(pCiA402Axis->i16State)
        {
        case STATE_NOT_READY_TO_SWITCH_ON:
            StatusWord |= (STATUSWORD_STATE_NOTREADYTOSWITCHON);
            if(nAlStatus == STATE_OP)
            {
                if(!CiA402_StateTransition1(pCiA402Axis)){
                // Automatic transition -> Communication shall be activated
                    pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED; // Transition 1
                }
            }
            else
            {
                /*
                CiA402 statemachine shall stay in "STATE_NOT_READY_TO_SWITCH_ON" if EtherCAT state is not OP.
                */
                pCiA402Axis->i16State = STATE_NOT_READY_TO_SWITCH_ON; // stay in current state
            }

            break;
        case STATE_SWITCH_ON_DISABLED:
            StatusWord |= (STATUSWORD_STATE_SWITCHEDONDISABLED);
            if ((ControlWord6040 & CONTROLWORD_COMMAND_SHUTDOWN_MASK) == CONTROLWORD_COMMAND_SHUTDOWN)
            {
                pCiA402Axis->i16State = STATE_READY_TO_SWITCH_ON; // Transition 2
            }
            break;
        case STATE_READY_TO_SWITCH_ON:
            StatusWord |= (STATUSWORD_STATE_READYTOSWITCHON);
            if (((ControlWord6040 & CONTROLWORD_COMMAND_QUICKSTOP_MASK) == CONTROLWORD_COMMAND_QUICKSTOP)
                || ((ControlWord6040 & CONTROLWORD_COMMAND_DISABLEVOLTAGE_MASK) == CONTROLWORD_COMMAND_DISABLEVOLTAGE))
            {
                if(!CiA402_StateTransition7(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED; // Transition 7
                }
            } else
                if (((ControlWord6040 & CONTROLWORD_COMMAND_SWITCHON_MASK) == CONTROLWORD_COMMAND_SWITCHON) ||
                    ((ControlWord6040 & CONTROLWORD_COMMAND_SWITCHON_ENABLEOPERATION_MASK) == CONTROLWORD_COMMAND_SWITCHON_ENABLEOPERATION))
                {
                    if(!CiA402_StateTransition3(pCiA402Axis)){
                        pCiA402Axis->i16State = STATE_SWITCHED_ON;           // Transition 3
                    }
                }
                break;
        case STATE_SWITCHED_ON:
            StatusWord |= (STATUSWORD_STATE_SWITCHEDON);

            if ((ControlWord6040 & CONTROLWORD_COMMAND_SHUTDOWN_MASK) == CONTROLWORD_COMMAND_SHUTDOWN)
            {
                if(!CiA402_StateTransition6(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_READY_TO_SWITCH_ON; // Transition 6
                }
            } else
                if (((ControlWord6040 & CONTROLWORD_COMMAND_QUICKSTOP_MASK) == CONTROLWORD_COMMAND_QUICKSTOP
                    || (ControlWord6040 & CONTROLWORD_COMMAND_DISABLEVOLTAGE_MASK) == CONTROLWORD_COMMAND_DISABLEVOLTAGE))
                {
                    if(!CiA402_StateTransition10(pCiA402Axis)){
                        pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED; // Transition 10
                    }
                } else
                    if ((ControlWord6040 & CONTROLWORD_COMMAND_ENABLEOPERATION_MASK) == CONTROLWORD_COMMAND_ENABLEOPERATION)
                    {
                        if(!CiA402_StateTransition4(pCiA402Axis)){	
                            pCiA402Axis->i16State = STATE_OPERATION_ENABLED;  // Transition 4
                        //The Axis function shall be enabled and all internal set-points cleared.
                        }
                    }
                    break;
        case STATE_OPERATION_ENABLED:
            StatusWord |= (STATUSWORD_STATE_OPERATIONENABLED);

            if ((ControlWord6040 & CONTROLWORD_COMMAND_DISABLEOPERATION_MASK) == CONTROLWORD_COMMAND_DISABLEOPERATION)
            {
                if(pCiA402Axis->Objects.objDisableOperationOptionCode!= DISABLE_DRIVE)
                {
                    /*disable operation pending*/
                    pCiA402Axis->u16PendingOptionCode = 0x605C; //STATE_TRANSITION (STATE_OPERATION_ENABLED,STATE_SWITCHED_ON);
                    return;
                }else{
                    if(!CiA402_StateTransition5(pCiA402Axis)){
                        pCiA402Axis->i16State = STATE_SWITCHED_ON;        // Transition 5
                    }
                }
            } else
                if ((ControlWord6040 & CONTROLWORD_COMMAND_QUICKSTOP_MASK) == CONTROLWORD_COMMAND_QUICKSTOP)
                {
                    if(!CiA402_StateTransition11(pCiA402Axis)){
                        pCiA402Axis->i16State = STATE_QUICK_STOP_ACTIVE;  // Transition 11
                    }
                } else
                    if ((ControlWord6040 & CONTROLWORD_COMMAND_SHUTDOWN_MASK) == CONTROLWORD_COMMAND_SHUTDOWN)
                    {
                        if(pCiA402Axis->Objects.objShutdownOptionCode != DISABLE_DRIVE)
                        {
                            /*shutdown operation required*/
                            pCiA402Axis->u16PendingOptionCode = 0x605B; //STATE_TRANSITION (STATE_OPERATION_ENABLED,STATE_READY_TO_SWITCH_ON);
                            return;
                        }else{
                            if(!CiA402_StateTransition8(pCiA402Axis)){
                                pCiA402Axis->i16State = STATE_READY_TO_SWITCH_ON; // Transition 8
                            }
                        }
                    } else
                        if ((ControlWord6040 & CONTROLWORD_COMMAND_DISABLEVOLTAGE_MASK) == CONTROLWORD_COMMAND_DISABLEVOLTAGE)
                        {
                            if(!CiA402_StateTransition9(pCiA402Axis)){
                                pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED; // Transition 9
                            }
                        }
                        break;
        case STATE_QUICK_STOP_ACTIVE:
            StatusWord |= STATUSWORD_STATE_QUICKSTOPACTIVE;

            if((pCiA402Axis->Objects.objQuickStopOptionCode != DISABLE_DRIVE) &&
                ((pCiA402Axis->Objects.objStatusWord & STATUSWORD_STATE_MASK)!= STATUSWORD_STATE_QUICKSTOPACTIVE))
            {
                /*Only execute quick stop action in state transition 11*/
                pCiA402Axis->u16PendingOptionCode = 0x605A;//STATE_TRANSITION (STATE_OPERATION_ENABLED,STATE_QUICK_STOP_ACTIVE);
                return;
            }

            if ((ControlWord6040 & CONTROLWORD_COMMAND_DISABLEVOLTAGE_MASK) == CONTROLWORD_COMMAND_DISABLEVOLTAGE)
            {
                if(!CiA402_StateTransition12(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED; // Transition 12
                }
            }

            /*NOTE: it is recommend to support transition 16 */
            if ((ControlWord6040 & CONTROLWORD_COMMAND_ENABLEOPERATION_MASK) == CONTROLWORD_COMMAND_ENABLEOPERATION)
            {
                if(!CiA402_StateTransition16(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_OPERATION_ENABLED;  // Transition 16
                    //The Axis function shall be enabled and all internal set-points cleared.
                }
            }
            break;
        case STATE_FAULT_REACTION_ACTIVE:
            StatusWord |= (STATUSWORD_STATE_FAULTREACTIONACTIVE);
            if(pCiA402Axis->Objects.objFaultReactionCode!= DISABLE_DRIVE)
            {
                /*fault reaction pending*/
                pCiA402Axis->u16PendingOptionCode = 0x605E;
                return;
            }else
                if(!CiA402_StateTransition14(pCiA402Axis)){
                // Automatic transition
                pCiA402Axis->i16State = STATE_FAULT;// Transition 14
            }
            break;
        case STATE_FAULT:
            StatusWord |= (STATUSWORD_STATE_FAULT);

            if ((ControlWord6040 & CONTROLWORD_COMMAND_FAULTRESET_MASK) == CONTROLWORD_COMMAND_FAULTRESET)
            {
                if(!CiA402_StateTransition15(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED;// Transition 15
                }
            }
            break;

        default:    //the sate variable is set to in invalid value => rest Axis
            StatusWord = (STATUSWORD_STATE_NOTREADYTOSWITCHON);
            pCiA402Axis->i16State = STATE_NOT_READY_TO_SWITCH_ON;
            break;

        }// switch(current state)



        /*Update operational functions (the low level power supply is always TRUE*/
        switch(pCiA402Axis->i16State)
        {
        case STATE_NOT_READY_TO_SWITCH_ON:
        case STATE_SWITCH_ON_DISABLED:
        case STATE_READY_TO_SWITCH_ON:
            pCiA402Axis->bBrakeApplied = TRUE;
            pCiA402Axis->bHighLevelPowerApplied =  FALSE;
            pCiA402Axis->bAxisFunctionEnabled = FALSE;
            pCiA402Axis->bConfigurationAllowed = TRUE;
            break;
        case STATE_SWITCHED_ON:
            pCiA402Axis->bBrakeApplied = TRUE;
            pCiA402Axis->bHighLevelPowerApplied =  TRUE;
            pCiA402Axis->bAxisFunctionEnabled = FALSE;
            pCiA402Axis->bConfigurationAllowed = TRUE;
            break;
        case STATE_OPERATION_ENABLED:
        case STATE_QUICK_STOP_ACTIVE:
        case STATE_FAULT_REACTION_ACTIVE:
            pCiA402Axis->bBrakeApplied = FALSE;
            pCiA402Axis->bHighLevelPowerApplied =  TRUE;
            pCiA402Axis->bAxisFunctionEnabled = TRUE;
            pCiA402Axis->bConfigurationAllowed = FALSE;
            break;
        case STATE_FAULT:
            pCiA402Axis->bBrakeApplied = TRUE;
            pCiA402Axis->bHighLevelPowerApplied =  FALSE;
            pCiA402Axis->bAxisFunctionEnabled = FALSE;
            pCiA402Axis->bConfigurationAllowed = TRUE;
            break;
        default:
            pCiA402Axis->bBrakeApplied = TRUE;
            pCiA402Axis->bHighLevelPowerApplied =  FALSE;
            pCiA402Axis->bAxisFunctionEnabled = FALSE;
            pCiA402Axis->bConfigurationAllowed = TRUE;
            break;
        }

        if (pCiA402Axis->bHighLevelPowerApplied == TRUE)
        {
            StatusWord |= STATUSWORD_VOLTAGE_ENABLED;
        }
        else
        {
            StatusWord &= ~(UINT16)STATUSWORD_VOLTAGE_ENABLED;
        }

        /*state transition finished set controlword complete bit and update status object 0x6041*/
        pCiA402Axis->Objects.objStatusWord = (StatusWord | STATUSWORD_REMOTE);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return TRUE if moving on predefined ramp is finished

 \brief    CiA402-TransitionAction
 \brief this function shall calculate the desired Axis input values to move on a predefined ramp
 \brief if the ramp is finished return TRUE
*////////////////////////////////////////////////////////////////////////////////////////
BOOL CiA402_TransitionAction(INT16 Characteristic,TCiA402Axis *pCiA402Axis)
{
	FSP_PARAMETER_NOT_USED(pCiA402Axis);
   switch(Characteristic)
   {

   case SLOW_DOWN_RAMP:
#if _WIN32
      #pragma message ("Warning: Implement slowdown ramp")
#else
      #warning "Implement slowdown ramp"
#endif
      return TRUE;
      break;
   case QUICKSTOP_RAMP:
#if _WIN32
      #pragma message ("Warning: Implement quick stop ramp ramp")
#else
      #warning "Implement quick stop ramp ramp"
#endif
      return TRUE;
      break;
   case STOP_ON_CURRENT_LIMIT:
#if _WIN32
      #pragma message ("Warning: Implement slowdown on current limit ramp")
#else
      #warning "Implement slowdown on current limit ramp"
#endif
      return TRUE;
      break;
   case STOP_ON_VOLTAGE_LIMIT:
#if _WIN32
      #pragma message ("Warning: Implement slowdown on voltage limit ramp")
#else
      #warning "Implement slowdown on voltage limit ramp"
#endif
      return TRUE;
      break;
   default:
      break;
   }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    CiA402-Application
 \brief check if a state transition is pending and pass desired ramp-code to CiA402TransitionAction()
 \brief if this functions returns true the state transition is finished.
*////////////////////////////////////////////////////////////////////////////////////////
void CiA402_Application(TCiA402Axis *pCiA402Axis, UINT16 i)
{
    /*clear "Drive follows the command value" flag if the target values from the master overwritten by the local application*/
    if(pCiA402Axis->u16PendingOptionCode != 0 &&
        (pCiA402Axis->Objects.objModesOfOperationDisplay == CYCLIC_SYNC_POSITION_MODE ||
        pCiA402Axis->Objects.objModesOfOperationDisplay == CYCLIC_SYNC_VELOCITY_MODE))
    {
    	pCiA402Axis->Objects.objStatusWord &= ~ STATUSWORD_DRIVE_FOLLOWS_COMMAND;
    }
    else
    {
        pCiA402Axis->Objects.objStatusWord |= STATUSWORD_DRIVE_FOLLOWS_COMMAND;
    }


    switch(pCiA402Axis->u16PendingOptionCode)
    {
    case 0x605A:
        /*state transition 11 is pending analyse shutdown option code (0x605A)*/
        {
        	UINT16 ramp = pCiA402Axis->Objects.objQuickStopOptionCode;


            /*masked and execute specified quick stop ramp characteristic */
            if(pCiA402Axis->Objects.objQuickStopOptionCode > 4 && pCiA402Axis->Objects.objQuickStopOptionCode <9)
            {
                    if (pCiA402Axis->Objects.objQuickStopOptionCode == 5)
                    {
                        ramp = 1;
                    }
                if (pCiA402Axis->Objects.objQuickStopOptionCode == 6)
                {
                    ramp = 2;
                }
                if (pCiA402Axis->Objects.objQuickStopOptionCode == 7)
                {
                    ramp = 3;
                }
                if (pCiA402Axis->Objects.objQuickStopOptionCode == 8)
                {
                    ramp = 4;
                }
            }

            if(CiA402_TransitionAction(ramp,pCiA402Axis))
            {
                /*quick stop ramp is finished complete state transition*/
                pCiA402Axis->u16PendingOptionCode = 0x0;
                if(pCiA402Axis->Objects.objQuickStopOptionCode > 0 && pCiA402Axis->Objects.objQuickStopOptionCode < 5)
                {
                    if(!CiA402_StateTransition12(pCiA402Axis)){
                        pCiA402Axis->i16State = STATE_SWITCH_ON_DISABLED;    //continue state transition 12
                    }
                }
                else if(pCiA402Axis->Objects.objQuickStopOptionCode > 4 && pCiA402Axis->Objects.objQuickStopOptionCode < 9)
                {
                    pCiA402Axis->Objects.objStatusWord |= STATUSWORD_TARGET_REACHED;
                    pCiA402Axis->Objects.objStatusWord &= (UINT16)(~(STATUSWORD_STATE_SWITCHEDONDISABLED));
                }
            }
        }
        break;
    case 0x605B:
        /*state transition 8 is pending analyse shutdown option code (0x605B)*/
        {
            if(CiA402_TransitionAction(pCiA402Axis->Objects.objShutdownOptionCode,pCiA402Axis))
            {
                /*shutdown ramp is finished complete state transition*/
                pCiA402Axis->u16PendingOptionCode = 0x0;
                if(!CiA402_StateTransition8(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_READY_TO_SWITCH_ON;    //continue state transition 8
                }
            }
        }
        break;
    case 0x605C:
        /*state transition 5 is pending analyse Disable operation option code (0x605C)*/
        {
            if(CiA402_TransitionAction(pCiA402Axis->Objects.objDisableOperationOptionCode,pCiA402Axis))
            {
                /*disable operation ramp is finished complete state transition*/
                pCiA402Axis->u16PendingOptionCode = 0x0;
                if(!CiA402_StateTransition5(pCiA402Axis)){
                    pCiA402Axis->i16State = STATE_SWITCHED_ON;    //continue state transition 5
                }
            }
        }
        break;

    case 0x605E:
        /*state transition 14 is pending analyse Fault reaction option code (0x605E)*/
        {
            if(CiA402_TransitionAction(pCiA402Axis->Objects.objFaultReactionCode,pCiA402Axis))
            {
                /*fault reaction ramp is finished complete state transition*/
                pCiA402Axis->u16PendingOptionCode = 0x0;
                if(!CiA402_StateTransition14(pCiA402Axis)){
                    // Automatic transition
                    pCiA402Axis->i16State = STATE_FAULT;// Transition 14
                }
            }
        }
        break;
    default:
        //pending transition code is invalid => values from the master are used
        pCiA402Axis->Objects.objStatusWord |= STATUSWORD_DRIVE_FOLLOWS_COMMAND;
        break;
    }
    if(bDcSyncActive
        && (pCiA402Axis->u32CycleTime != 0)
        && ((pCiA402Axis->Objects.objSupportedDriveModes >> (pCiA402Axis->Objects.objModesOfOperation - 1)) & 0x1)) //Mode of Operation (0x6060) - 1 specifies the Bit within Supported Drive Modes (0x6502)
    {
#if (_DUMMY_ == 1)
        CiA402_DummyMotionControl(pCiA402Axis, i);
#else
        APPL_MOTOR_MotionControl_Main(pCiA402Axis, i);
#endif
    }

}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     index                 index of the requested object.
 \param     subindex                subindex of the requested object.
 \param     dataSize                received data size of the SDO Download
 \param     pObjEntry            handle to the dictionary object returned by
                                     OBJ_GetObjectHandle which was called before
 \param    pData                    Pointer to the buffer where the written data can be copied from
 \param    bCompleteAccess    Indicates if a complete write of all subindices of the
                                     object shall be done or not

 \return    result of the write operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))

 \brief    This function writes "Configured Modules" Object 0xF030
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 Write0xF030( UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess )
{

    UINT16 i = subindex;
    UINT16 maxSubindex = sConfiguredModuleIdentList.u16SubIndex0;
    OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry;
    /* lastSubindex is used for complete access to make loop over the requested entries
       to be read, we initialize this variable with the requested subindex that only
       one loop will be done for a single access */
    UINT8 lastSubindex = subindex;

    if ( bCompleteAccess )
    {
        if ( subindex == 0 )
        {
            /* we change the subindex 0 */
            maxSubindex = (UINT8) pData[0];
        }

        /* we write until the maximum subindex */
        lastSubindex = (UINT8)maxSubindex;
    }
    else
        if (subindex > maxSubindex)
        {
            /* the maximum subindex is reached */
            return ABORTIDX_SUBINDEX_NOT_EXISTING;
        }
    else
    {
        /* we check the write access for single accesses here, a complete write access
           is allowed if at least one entry is writable (in this case the values for the
            read only entries shall be ignored) */
        /* we get the corresponding entry description */
        pEntry = &asEntryDesc0xF030[subindex];

        /* check if we have write access (bits 3-5 (PREOP, SAFEOP, OP) of ObjAccess)
           by comparing with the actual state (bits 1-3 (PREOP, SAFEOP, OP) of AL Status) */
        if (0 == (((UINT8) ((pEntry->ObjAccess & ACCESS_WRITE) >> 2)) & (nAlStatus & STATE_MASK) ))
        {
            /* we don't have write access */
            if ( (pEntry->ObjAccess & ACCESS_WRITE) == 0 )
            {
                /* it is a read only entry */
                return ABORTIDX_READ_ONLY_ENTRY;
            }
            else
            {
                /* we don't have write access in this state */
                return ABORTIDX_IN_THIS_STATE_DATA_CANNOT_BE_READ_OR_STORED;
            }
        }
    }

        /* we use the standard write function */
        for (i = subindex; i <= lastSubindex; i++)
        {
            /* we have to copy the entry */
            if (i == 0)
            {
                /*check if the value for subindex0 is valid */
                if(MAX_AXES < (UINT8) pData[0])
                {
                    return ABORTIDX_VALUE_TOO_GREAT;
                }

                sConfiguredModuleIdentList.u16SubIndex0 =  pData[0];

                /* we increment the destination pointer by 2 because the subindex 0 will be
                    transmitted as UINT16 for a complete access */
                pData++;
            }
            else
            {
                UINT32 CurValue = sConfiguredModuleIdentList.aEntries[(i-1)];
                UINT16 MBXMEM *pVarPtr = (UINT16 MBXMEM *) &sConfiguredModuleIdentList.aEntries[(i-1)];

                pVarPtr[0] = pData[0];
                pVarPtr[1] = pData[1];
                pData += 2;

                /*Check if valid value was written*/
                if((sConfiguredModuleIdentList.aEntries[(i-1)] != (UINT32)CSV_CSP_MODULE_ID)
                && (sConfiguredModuleIdentList.aEntries[(i-1)] != (UINT32)CSP_MODULE_ID)
                && (sConfiguredModuleIdentList.aEntries[(i-1)] != (UINT32)CSV_MODULE_ID)
                && (sConfiguredModuleIdentList.aEntries[(i-1)] != 0))
                {
                    /*write previous value*/
                    sConfiguredModuleIdentList.aEntries[(i-1)] = CurValue;

                    /*reset subindex 0 (if required)*/
                    if(sConfiguredModuleIdentList.aEntries[(i-1)] != 0)
                    {
                        sConfiguredModuleIdentList.u16SubIndex0 = i;
                    }
                    else
                    {
                        /*current entry is 0 => set subindex0 value i-1*/
                        sConfiguredModuleIdentList.u16SubIndex0 = (i-1);
                    }


                    return ABORTIDX_VALUE_EXCEEDED;
                }
           }
        }

        /*Update PDO assign objects and 0xF010 (Module Profile List)*/
        {
        UINT8 cnt = 0;

        /*Update 0xF010.0 */
//        sModuleProfileInfo.u16SubIndex0 = sConfiguredModuleIdentList.u16SubIndex0;
        
        /*Update PDO assign SI0*/
        sRxPDOassign.u16SubIndex0 = sConfiguredModuleIdentList.u16SubIndex0;
        sTxPDOassign.u16SubIndex0 = sConfiguredModuleIdentList.u16SubIndex0;

        for (cnt = 0 ; cnt < sConfiguredModuleIdentList.u16SubIndex0; cnt++)
        {
            /*all Modules have the same profile number*/
/*ECATCHANGE_START(V5.13) CIA402 2*/
//            sModuleProfileInfo.aEntries[cnt] = (DEVICE_PROFILE_TYPE >> 16);
/*ECATCHANGE_END(V5.13) CIA402 2*/

            switch(sConfiguredModuleIdentList.aEntries[cnt])
            {
                case CSV_CSP_MODULE_ID:
                    sRxPDOassign.aEntries[cnt] = (0x1600 +(0x10*cnt));
                    sTxPDOassign.aEntries[cnt] = (0x1A00 +(0x10*cnt));
                break;
                case CSP_MODULE_ID:
                    sRxPDOassign.aEntries[cnt] = (0x1601 +(0x10*cnt));
                    sTxPDOassign.aEntries[cnt] = (0x1A01 +(0x10*cnt));
                break;
                case CSV_MODULE_ID:
                    sRxPDOassign.aEntries[cnt] = (0x1602 +(0x10*cnt));
                    sTxPDOassign.aEntries[cnt] = (0x1A02 +(0x10*cnt));
                break;
                default:
                    sRxPDOassign.aEntries[cnt] = 0;
                    sTxPDOassign.aEntries[cnt] = 0;

//                    sModuleProfileInfo.aEntries[cnt] = 0;
                break;
            }
        }
        }

        FSP_PARAMETER_NOT_USED(dataSize);
		FSP_PARAMETER_NOT_USED(index);
    return 0;
}

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

/** @} */
#endif // #if (CiA402_SAMPLE_APPLICATION == 1)
