//
// CScsi_Application_Client_Log.cpp  Definition of Application Client Log where clients store information
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Application_Client_Log.cpp  Definition of Application Client Log where clients store information
//
#include "CScsi_Application_Client_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiApplicationLog()
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
CScsiApplicationLog::CScsiApplicationLog()
	: pData()
	, m_ApplicationName("Application Client Log")
	, m_ApplicationStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_App()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ApplicationName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiApplicationLog()
//
//! \brief
//!   Description: Class constructor for the Application Client
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiApplicationLog::CScsiApplicationLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(buffer)
	, m_ApplicationName("Application Client Log")
	, m_ApplicationStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_App(0)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ApplicationName.c_str());
	}
	if (buffer != NULL)
	{
		m_ApplicationStatus = IN_PROGRESS;
	}
	else
	{
		m_ApplicationStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiApplicationLog
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
CScsiApplicationLog::~CScsiApplicationLog()
{

}

//-----------------------------------------------------------------------------
//
//! \fn process_Client_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed client data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiApplicationLog::process_Client_Data(JSONNODE *clientData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined( _DEBUG)
	printf("Cache Event Description \n");
#endif
	byte_Swap_16(&m_App->paramCode);
	//get_Cache_Parameter_Code_Description(&myStr);
	snprintf((char*)myStr.c_str(), BASIC, "Cache Statistics Description %" PRId16"", m_App->paramCode);
	JSONNODE *clientInfo = json_new(JSON_NODE);
	json_set_name(clientInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_App->paramCode);
	json_push_back(clientInfo, json_new_a("Cache Statistics Parameter Code", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_App->paramControlByte);
	json_push_back(clientInfo, json_new_a("Cache Statistics Control Byte ", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_App->paramLength);
	json_push_back(clientInfo, json_new_a("Cache Statistics Length ", (char*)myStr.c_str()));


	json_push_back(clientData, clientInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Client_Data
//
//! \brief
//!   Description: parser out the data for the Application Client Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiApplicationLog::get_Client_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Application Client Log");

		for (size_t offset = 0; offset < m_PageLength; )
		{
			if (offset+sizeof(sApplicationParams) < m_bufferLength && offset < UINT16_MAX)
			{
				m_App = (sApplicationParams *)&pData[offset];
				offset += sizeof(sApplicationParams);
				
				process_Client_Data(pageInfo);
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