//
// CScsi_Power_Condition_Transitions_Log.h  Definition of Power Condition Transistions Log Page for SAS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Power_Condition_Transitions_Log.h  Definition of Power Condition Transistions Log Page for SAS
//
#include "CScsi_Power_Condition_Transitions_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiPowerConditiontLog()
//
//! \brief
//!   Description: Default Class constructor 
//
//  Entry:
// \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiPowerConditiontLog::CScsiPowerConditiontLog()
	: pData()
	, m_PowerName("Power Condition Transitions Log")
	, m_PowerStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_PowerParam(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PowerName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiPowerConditiontLog()
//
//! \brief
//!   Description: Class constructor 
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiPowerConditiontLog::CScsiPowerConditiontLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(buffer)
	, m_PowerName("Power Condition Transitions Log")
	, m_PowerStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_PowerParam(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_PowerName.c_str());
	}
	if (buffer != NULL)
	{
		m_PowerStatus = IN_PROGRESS;
	}
	else
	{
		m_PowerStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiPowerConditiontLog
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiPowerConditiontLog::~CScsiPowerConditiontLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Power_Mode_Type
//
//! \brief
//!   Description: parser out the data for Power Mode Transition Type Information
//
//  Entry:
//! \param power - string to give the Power Mode depending on what the code is
//! \param code - parameter code holds the value to the discription
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
bool CScsiPowerConditiontLog::get_Power_Mode_Type(std::string *power, uint16_t code)
{
    bool typeFound = false;
	switch (code)
	{
		case ACTIVE:
		{
			snprintf((char*)power->c_str(), BASIC, "Accumulated transitions to active");
            typeFound = true;
			break;
		}
		case IDLE_A:
		{
			snprintf((char*)power->c_str(), BASIC, "Accumulated transitions to idle_a");
            typeFound = true;
			break;
		}
		case IDLE_B:
		{
			snprintf((char*)power->c_str(), BASIC, "Accumulated transitions to idle_b");
            typeFound = true;
			break;
		}
		case IDLE_C:
		{
			snprintf((char*)power->c_str(), BASIC, "Accumulated transitions to idle_c");
            typeFound = true;
			break;
		}
		case STANDZ:
		{
			snprintf((char*)power->c_str(), BASIC, "Accumulated transitions to standby_z");
            typeFound = true;
			break;
		}
		case STANDY:
		{
			snprintf((char*)power->c_str(), BASIC, "Accumulated transitions to standby_y");
            typeFound = true;
			break;
		}
		default:
		{
			snprintf((char*)power->c_str(), BASIC, "Vendor Specific Power Mode Transition Type");
			break;
		}
	}
    return typeFound;
}
//-----------------------------------------------------------------------------
//
//! \fn process_Power_Information
//
//! \brief
//!   Description: parser out the data for Power parameters Information
//
//  Entry:
//! \param ) - Json node that the information will be added to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiPowerConditiontLog::process_List_Information(JSONNODE *powerData)
{
    bool typeFound = false;
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("Power Condition Transitions Log Page\n");
#endif
	byte_Swap_16(&m_PowerParam->paramCode);
    byte_Swap_32(&m_PowerParam->paramValue);
    typeFound = get_Power_Mode_Type(&myStr, m_PowerParam->paramCode);
	
    if (m_PowerParam->paramValue != 0)
    {
        JSONNODE *powerInfo = json_new(JSON_NODE);
        json_set_name(powerInfo, (char*)myStr.c_str());
        snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_PowerParam->paramCode);
        json_push_back(powerInfo, json_new_a("Power Condition Type", (char*)myStr.c_str()));
        if (!typeFound)
        {
            snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_PowerParam->paramControlByte);
            json_push_back(powerInfo, json_new_a("Control Byte", (char*)myStr.c_str()));
            snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_PowerParam->paramLength);
            json_push_back(powerInfo, json_new_a("Length", (char*)myStr.c_str()));
        }
        json_push_back(powerInfo, json_new_i("Power Value", m_PowerParam->paramValue));
        json_push_back(powerData, powerInfo);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data
//
//! \brief
//!   Description: parser out the data for the Power conditions tranistions Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiPowerConditiontLog::get_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Power Conditions Tranistions Log - 1Ah");

		for (size_t offset = 0; offset < m_PageLength; offset += sizeof(sPowerParams))
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
				m_PowerParam = (sPowerParams *)&pData[offset];
				// process the power information
				process_List_Information(pageInfo);			
			}
			else
			{
				json_push_back(masterData, pageInfo);
				return BAD_PARAMETER;
			}
		}

		json_push_back(masterData, pageInfo);
		retStatus = SUCCESS;
	}
	else
	{
		retStatus = MEMORY_FAILURE;
	}
	return retStatus;
}