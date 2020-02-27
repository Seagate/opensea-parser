//
// CScsi_Temperature_Log.cpp  Implementation of CScsi Temperature Log class
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
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
		m_Page = (sTempLogPageStruct *)buffer;				// set a buffer to the point to the log page info
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
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("Temperature Log Page \n");
	printf("\tParameter Code =   0x%04" PRIx16"  \n", m_Page->paramCode);
#endif
	snprintf((char*)myStr.c_str(), BASIC, "Parameter Code 0x%04" PRIx16"", m_Page->paramCode);
	JSONNODE *paramInfo = json_new(JSON_NODE);
	json_set_name(paramInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Page->paramLength);
	json_push_back(paramInfo, json_new_a("Parameter Length", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Page->paramControlByte);
	json_push_back(paramInfo, json_new_a("Control Byte", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "%" PRIu8"", m_Page->temp);
	json_push_back(paramInfo, json_new_a("Temperature", (char*)myStr.c_str()));

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
			if ((param + (2* tempSize) + sizeof(sLogPageStruct)) > m_pDataSize && param + tempSize < m_PageLength || param > UINT32_MAX)
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