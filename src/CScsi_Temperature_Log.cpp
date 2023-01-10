//
// CScsi_Temperature_Log.cpp  Implementation of CScsi Temperature Log class
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Temperature_Log.cpp Implementation of CScsi Temperature Log class
//
#include "CScsi_Temperature_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiTemperatureLog()
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
CScsiTemperatureLog::CScsiTemperatureLog()
	: pData()
	, m_pDataSize(0)
	, m_Page()
	, m_TempName("Temperature Log")
	, m_TempStatus(IN_PROGRESS)
	, m_PageLength(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_TempName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiTemperatureLog()
//
//! \brief
//!   Description: Class constructor for the CScsiTemperatureLog
//
//  Entry:
//! \param fileName = the name of the file that is to be read in
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiTemperatureLog::CScsiTemperatureLog(uint8_t * buffer, size_t bufferSize)
	:pData(buffer)
	, m_pDataSize(bufferSize)
	, m_Page()
	, m_TempName("Temperature Log")
	, m_TempStatus(IN_PROGRESS)
	, m_PageLength(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_TempName.c_str());
	}
	if (buffer != NULL)
	{
		pData = buffer;
		m_Page = reinterpret_cast<sTempLogPageStruct *>(buffer);				// set a buffer to the point to the log page info
		m_TempStatus = SUCCESS;
	}
	else
	{
		m_TempStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiTemperatureLog
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
CScsiTemperatureLog::~CScsiTemperatureLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Temp
//
//! \brief
//!   Description: parser out the data for the temperature
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
void CScsiTemperatureLog::get_Temp(JSONNODE *tempData)
{
#if defined _DEBUG
	printf("Temperature Log Page \n");
	printf("\tParameter Code =   0x%04" PRIx16"  \n", m_Page->paramCode);
#endif
    std::ostringstream temp;
    temp << "Parameter Code 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_Page->paramCode;
	JSONNODE *paramInfo = json_new(JSON_NODE);
	json_set_name(paramInfo, temp.str().c_str());

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Page->paramLength);
	json_push_back(paramInfo, json_new_a("Parameter Length", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Page->paramControlByte);
	json_push_back(paramInfo, json_new_a("Control Byte", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << std::dec << static_cast<uint16_t>(m_Page->temp);
	json_push_back(paramInfo, json_new_a("Temperature", temp.str().c_str()));

	json_push_back(tempData, paramInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Temp
//
//! \brief
//!   Description: parser out the data for the temperature
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiTemperatureLog::get_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;

	size_t tempSize = sizeof(sTempLogPageStruct);
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Temperature Log Page - Dh");
		for (size_t param = 0; param < m_PageLength; param += tempSize)
		{
			byte_Swap_16(&m_Page->paramCode);
			get_Temp(pageInfo);
			// Check to make sure we still have engough data to increment the m_Page
			if (((param + (2* tempSize) + sizeof(sLogPageStruct)) > m_pDataSize && param + tempSize < m_PageLength) || param > UINT32_MAX)
			{
				json_push_back(masterData, pageInfo);
				return BAD_PARAMETER;
			}
			else if (param + tempSize < m_PageLength)
			{
				m_Page++;
			}
			else
			{
				break;
			}
		}

		json_push_back(masterData, pageInfo);
		retStatus = SUCCESS;
	}
	else
	{
		retStatus = FAILURE;
	}
	return retStatus;
}