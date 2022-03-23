//
// CAta_Power_Conditions_Log.cpp   Implementation of Base class CAtaLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CAta_Power_Conditions_Log.cpp  Implementation of Base class CAta_Power_Conditions_Log
//
#include "CAta_Power_Conditions_Log.h"

/*#if !defined BASIC
#define	BASIC (80)
#endif*/

using namespace opensea_parser;
#define OFFSET_IDLE_A      0
#define OFFSET_IDLE_B     64
#define OFFSET_IDLE_C    128
#define OFFSET_STANDBY_Y 384
#define OFFSET_STANDBY_Z 448

//-----------------------------------------------------------------------------
//
//! \fn   CAtaPowerConditionsLog()
//
//! \brief
//!   Description: Default Class constructor for the CAtaPowerConditionsLog
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaPowerConditionsLog::CAtaPowerConditionsLog()
    : m_powerConditionLog(NULL)
    , m_powerFlags(NULL)
    , Buffer()
    , m_status(IN_PROGRESS)
    , m_myJSON()
    , m_idleAPowerConditions(NULL), m_idleBPowerConditions(NULL), m_idleCPowerConditions(NULL)
    , m_standbyYPowerConditions(NULL), m_standbyZPowerConditions(NULL)
    , m_conditionFlags(NULL)
    , conditionFlags()
{

}
//-----------------------------------------------------------------------------
//
//! \fn   CAtaPowerConditionsLog()
//
//! \brief
//!   Description: Default Class constructor for the CAtaPowerConditionsLog that reads in the file
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaPowerConditionsLog::CAtaPowerConditionsLog(std::string filename)
    : m_powerConditionLog(NULL)
    , m_powerFlags(NULL)
    , Buffer()
    , m_status(IN_PROGRESS)
    , m_myJSON()
    , m_idleAPowerConditions(NULL), m_idleBPowerConditions(NULL), m_idleCPowerConditions(NULL)
    , m_standbyYPowerConditions(NULL), m_standbyZPowerConditions(NULL)
    , m_conditionFlags(NULL)
    , conditionFlags()
{

    CLog *cCLog;
    cCLog = new CLog(filename);
    if (cCLog->get_Log_Status() == SUCCESS)
    {
        if (cCLog->get_Buffer() != NULL)
        {
            size_t logSize = cCLog->get_Size();
            m_powerConditionLog = new uint8_t[logSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
            memcpy(m_powerConditionLog, cCLog->get_Buffer(), logSize);
#else
            memcpy_s(m_powerConditionLog, logSize, cCLog->get_Buffer(), logSize);// copy the buffer data to the class member pBuf
#endif
            sLogPageStruct *idCheck;
            idCheck = (sLogPageStruct *)&m_powerConditionLog[0];
            byte_Swap_16(&idCheck->pageLength);
            if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
            {
                byte_Swap_16(&idCheck->pageLength);  // now that we know it's not scsi we need to flip the bytes back
                m_conditionFlags = &conditionFlags;
                get_Power_Condition_Log();
                m_status = IN_PROGRESS;
            }
            else
            {
                m_status = BAD_PARAMETER;
            }
        }
        else
        {

            m_status = FAILURE;
        }
    }
    else
    {
        m_status = cCLog->get_Log_Status();
    }
    delete (cCLog);
}

//-----------------------------------------------------------------------------
//
//! \fn   CAtaPowerConditionsLog()
//
//! \brief
//!   Description: Default Class constructor for the CAtaPowerConditionsLog prints out data
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaPowerConditionsLog::CAtaPowerConditionsLog(tDataPtr pData, JSONNODE *masterData)
    : m_powerConditionLog(NULL)
    , m_powerFlags(NULL)
    , Buffer()
    , m_status(IN_PROGRESS)
    , m_myJSON(masterData)
    , m_idleAPowerConditions(NULL), m_idleBPowerConditions(NULL), m_idleCPowerConditions(NULL)
    , m_standbyYPowerConditions(NULL), m_standbyZPowerConditions(NULL)
    , m_conditionFlags(NULL)
    , conditionFlags()
{
    m_powerConditionLog = new uint8_t[pData.DataLen];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
    memcpy(m_powerConditionLog, static_cast<uint8_t*>(pData.pData), pData.DataLen);
#else
    memcpy_s(m_powerConditionLog, pData.DataLen, static_cast<uint8_t*>(pData.pData), pData.DataLen);// copy the buffer data to the class member pBuf
#endif

    if (m_powerConditionLog != NULL)
    {
        m_conditionFlags = &conditionFlags;
        get_Power_Condition_Log();
        printPowerConditionLog(masterData);
        m_status = SUCCESS;
    }
    else
    {
        m_status = FAILURE;
    }

}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAtaPowerConditionsLog()
//
//! \brief
//!   Description:  Class deconstructor for for the CAtaPowerConditionsLog
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaPowerConditionsLog::~CAtaPowerConditionsLog()
{
    if (m_powerConditionLog != NULL)
    {
        delete[] m_powerConditionLog;
    }
}

//-----------------------------------------------------------------------------
//
//! \fn   get_Power_Condition_Log()
//
//! \brief
//!   Description:  get the power condition data.  \n
//! All data will be added to the json object that is passed in.                   \n
//
//  Entry:
//!
//
//  Exit:
//!   \return SUCCESS 
//
//---------------------------------------------------------------------------
eReturnValues CAtaPowerConditionsLog::get_Power_Condition_Log()
{

    //get the idle_a power condition descriptor
    m_idleAPowerConditions = (sPowerLogDescriptor*)&m_powerConditionLog[OFFSET_IDLE_A];

    //get the idle_b power condition descriptor
    m_idleBPowerConditions = (sPowerLogDescriptor*)&m_powerConditionLog[OFFSET_IDLE_B];

    //get the idle_c power condition descriptor
    m_idleCPowerConditions = (sPowerLogDescriptor*)&m_powerConditionLog[OFFSET_IDLE_C];

    //get the standby_y power condition descriptor
    m_standbyYPowerConditions = (sPowerLogDescriptor*)&m_powerConditionLog[OFFSET_STANDBY_Y];

    //get the standby_z power condition descriptor
    m_standbyZPowerConditions = (sPowerLogDescriptor*)&m_powerConditionLog[OFFSET_STANDBY_Z];

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn   get_Power_Condition_Flags()
//
//! \brieff
//!   Description:  get the power condition flag data.  \n
//! All data will be added to the json object that is passed in.                   \n
//
//  Entry:
//!   \param readFlags - Power Condition Flags data.
//
//  Exit:
//!   \return SUCCESS 
//
//---------------------------------------------------------------------------
eReturnValues CAtaPowerConditionsLog::get_Power_Condition_Flags(uint8_t readFlags)
{
    eReturnValues ret = NOT_SUPPORTED;
    m_conditionFlags->powerSupportBit = false;
    m_conditionFlags->powerSaveableBit = false;
    m_conditionFlags->powerChangeableBit = false;
    m_conditionFlags->defaultTimerBit = false;
    m_conditionFlags->savedTimmerBit = false;
    m_conditionFlags->currentTimmerBit = false;
    m_conditionFlags->holdPowerConditionBit = false;
    m_conditionFlags->reserved = false;

    if (readFlags & BIT7)
    {
        m_conditionFlags->powerSupportBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT6)
    {
        m_conditionFlags->powerSaveableBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT5)
    {
        m_conditionFlags->powerChangeableBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT4)
    {
        m_conditionFlags->defaultTimerBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT3)
    {
        m_conditionFlags->savedTimmerBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT2)
    {
        m_conditionFlags->currentTimmerBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT1)
    {
        m_conditionFlags->holdPowerConditionBit = true;
        ret = SUCCESS;
    }
    if (readFlags & BIT0)
    {
        m_conditionFlags->reserved = true;
    }

    return ret;
}
//-----------------------------------------------------------------------------
//
//! \fn   printPowerConditionLog()
//
//! \brieff
//!   Description:  print out the power condition log information
//
//  Entry:
//!   \param masterData - opointer to the json data that will be printed or passed on
//
//  Exit:
//!   \return SUCCESS 
//
//---------------------------------------------------------------------------
eReturnValues CAtaPowerConditionsLog::printPowerConditionLog(JSONNODE *masterData)
{
    m_myJSON = json_new(JSON_NODE);

    JSONNODE *idleAConditionInfo = json_new(JSON_NODE);
    json_set_name(m_myJSON, "SATA Power Condition Log");
    json_set_name(idleAConditionInfo, "Idle_a Power Condition Log");
    printPowerLogDescriptor(idleAConditionInfo, m_idleAPowerConditions);
    json_push_back(m_myJSON, idleAConditionInfo);

    JSONNODE *idleBConditionInfo = json_new(JSON_NODE);
    json_set_name(idleBConditionInfo, "Idle_b Power Condition Log");
    printPowerLogDescriptor(idleBConditionInfo, m_idleBPowerConditions);
    json_push_back(m_myJSON, idleBConditionInfo);

    JSONNODE *idleCConditionInfo = json_new(JSON_NODE);
    json_set_name(idleCConditionInfo, "Idle_c Power Condition Log");
    printPowerLogDescriptor(idleCConditionInfo, m_idleCPowerConditions);
    json_push_back(m_myJSON, idleCConditionInfo);

    JSONNODE *standByYConditionInfo = json_new(JSON_NODE);
    json_set_name(standByYConditionInfo, "Standby_y Power Condition Log");
    printPowerLogDescriptor(standByYConditionInfo, m_standbyYPowerConditions);
    json_push_back(m_myJSON, standByYConditionInfo);

    JSONNODE *standByZConditionInfo = json_new(JSON_NODE);
    json_set_name(standByZConditionInfo, "Standby_z Power Condition Log");
    printPowerLogDescriptor(standByZConditionInfo, m_standbyZPowerConditions);
    json_push_back(m_myJSON, standByZConditionInfo);
    json_push_back(masterData, m_myJSON);

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn   printPowerLogDescriptor()
//
//! \brieff
//!   Description:  print out the power condition log descriptor from the power condition flags information
//
//  Entry:
//!   \param masterData - pointer to the json data that will be printed or passed on
//!   \param logDescriptor - pointer to the power condition log offset
//
//  Exit:
//!   \return SUCCESS 
//
//---------------------------------------------------------------------------
eReturnValues CAtaPowerConditionsLog::printPowerLogDescriptor(JSONNODE *masterData, sPowerLogDescriptor *logDescriptor)
{
    get_Power_Condition_Flags(logDescriptor->bitFlags);
    if (logDescriptor != NULL)
    {
#if defined _DEBUG
        printf("\nReserved \n");
        printf("Power Condition Flags\n");
        printf("     Power Condition Supported                 : %" PRIu8" \n", m_conditionFlags->powerSupportBit);
        printf("     Power Condition Saveable                  : %" PRIu8" \n", m_conditionFlags->powerSaveableBit);
        printf("     Power Condition Changeable                : %" PRIu8" \n", m_conditionFlags->powerChangeableBit);
        printf("     Default Timer Enable                      : %" PRIu8" \n", m_conditionFlags->defaultTimerBit);
        printf("     Saved Timer Enable                        : %" PRIu8" \n", m_conditionFlags->savedTimmerBit);
        printf("     Current Timer Enable                      : %" PRIu8" \n", m_conditionFlags->currentTimmerBit);
        printf("     Hold Power Condition NOT Supported        : %" PRIu8" \n", m_conditionFlags->holdPowerConditionBit);
        printf("     Reseved                                   : %" PRIu8" \n", m_conditionFlags->reserved);
        printf("Reserved \n");
        printf("Default Timer Setting Field                    : %" PRIu32" \n", logDescriptor->defaultTimerSetting);
        printf("Saved Timer Setting Field                      : %" PRIu32" \n", logDescriptor->savedTimerSetting);
        printf("Current Timer Setting Field                    : %" PRIu32" \n", logDescriptor->currentTimerSetting);
        printf("Norminal Recovery Timer to PM0 : Active Filed  : %" PRIu32" \n", logDescriptor->normalRecoveryTime);
        printf("Minimum Timer Setting Field                    : %" PRIu32" \n", logDescriptor->minimumTimerSetting);
        printf("Maximum Timer Setting Field                    : %" PRIu32" \n", logDescriptor->maximumTimerSetting);
        printf("Reserved\n");

#endif

		//json_push_back(masterData, json_new_a("Reserved :", "Reserved"));
        std::ostringstream temp;
        temp << std::hex << logDescriptor->bitFlags;
		json_push_back(masterData, json_new_a("Power Condition Flags", temp.str().c_str()));
		printPowerConditionFlag(masterData);
		//json_push_back(masterData, json_new_a("Reserved :", "Reserved"));
		json_push_back(masterData, json_new_i("Default Timer Setting Field", logDescriptor->defaultTimerSetting));
		json_push_back(masterData, json_new_i("Saved Timer Setting Field", logDescriptor->savedTimerSetting));
		json_push_back(masterData, json_new_i("Current Timer Setting Field", logDescriptor->currentTimerSetting));
		json_push_back(masterData, json_new_i("Norminal Recovery Timer to PM0  Active Filed", logDescriptor->normalRecoveryTime));
		json_push_back(masterData, json_new_i("Minimum Timer Setting Field", logDescriptor->minimumTimerSetting));
		json_push_back(masterData, json_new_i("Maximum Timer Setting Field", logDescriptor->maximumTimerSetting));
		json_push_back(masterData, json_new_a("Reserved :", "Reserved"));
	}
	else
	{
		printf("\tNo Data Found \n");
		return NOT_SUPPORTED;
	}
	
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//
//! \fn   printPowerConditionFlag()
//
//! \brieff
//!   Description:  print out the boolean from power condition flags information
//
//  Entry:
//!   \param masterData - pointer to the json data that will be printed or passed on
//
//  Exit:
// 
//---------------------------------------------------------------------------
void CAtaPowerConditionsLog::printPowerConditionFlag(JSONNODE *masterData)
{

    if (m_conditionFlags != NULL)
    {

#if defined _DEBUG
        printf("Power Condition Supported            = %d   \n", m_conditionFlags->powerSupportBit);
        printf("Power Condition Saveable             = %d   \n", m_conditionFlags->powerSaveableBit);
        printf("Power Condition Changeable           = %d   \n", m_conditionFlags->powerChangeableBit);
        printf("Default Timer Enable                 = %d   \n", m_conditionFlags->defaultTimerBit);
        printf("Saved Timer Enable                   = %d   \n", m_conditionFlags->savedTimmerBit);
        printf("Current Timer Enable                 = %d   \n", m_conditionFlags->currentTimmerBit);
        printf("Hold Power Condition NOT Supported   = %d   \n", m_conditionFlags->holdPowerConditionBit);
        printf("Hold Power Condition NOT Supported   = %d   \n", m_conditionFlags->reserved);


#endif
        json_push_back(masterData, json_new_b("Power Condition Supported          ", m_conditionFlags->powerSupportBit));
        json_push_back(masterData, json_new_b("Power Condition Saveable           ", m_conditionFlags->powerSaveableBit));
        json_push_back(masterData, json_new_b("Power Condition Changeable         ", m_conditionFlags->powerChangeableBit));
        json_push_back(masterData, json_new_b("Default Timer Enable               ", m_conditionFlags->defaultTimerBit));
        json_push_back(masterData, json_new_b("Saved Timer Enable                 ", m_conditionFlags->savedTimmerBit));
        json_push_back(masterData, json_new_b("Current Timer Enable               ", m_conditionFlags->currentTimmerBit));
        json_push_back(masterData, json_new_b("Hold Power Condition NOT Supported ", m_conditionFlags->holdPowerConditionBit));
        json_push_back(masterData, json_new_b("Reserve                            ", m_conditionFlags->reserved));
    }

}