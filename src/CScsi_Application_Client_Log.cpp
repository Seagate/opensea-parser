//
// CScsi_Application_Client_Log.cpp  Definition of Application Client Log where clients store information
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
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
	, m_App(0)
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
void CScsiApplicationLog::process_Client_Data(JSONNODE *appData, uint32_t offset)
{
#define MSGSIZE 128
#define STRMSGSIZE 1201
	std::string myStr = "";
	myStr.resize(BASIC);
	if (g_dataformat == PREPYTHON_DATA)
	{
		json_push_back(appData, json_new_a("name", "application client"));
		JSONNODE* label = json_new(JSON_NODE);
		json_set_name(label, "labels");


		snprintf((char*)myStr.c_str(), BASIC, "scsi-log-page:0x%" PRIx8",%" PRIx8":0x%" PRIx16":%" PRIu32"", APPLICATION_CLIENT, 0, m_App->paramCode,offset);
		json_push_back(label, json_new_a("metric_source", (char*)myStr.c_str()));


		char* innerMsg = (char*)calloc(MSGSIZE, sizeof(char));
		if (innerMsg)
		{
			memset(innerMsg, 0, MSGSIZE);
		}
		char* innerStr = (char*)calloc(STRMSGSIZE, sizeof(char));
		if (innerStr)
		{
			memset(innerStr, 0, STRMSGSIZE);
		}

		uint32_t offset = 0;

		snprintf(innerStr, 13, "bytearray([");   //bytearray([%" PRIu8"
		for (uint32_t outer = 0; outer < APP_CLIENT_DATA_LEN - 1; outer++)
		{
			if (outer + 1 == APP_CLIENT_DATA_LEN - 1)
			{
				snprintf(innerMsg, 8, "%" PRIu8"", m_App->data[offset]);
			}
			else
			{
				snprintf(innerMsg, 8, "%" PRIu8",", (uint8_t)m_App->data[offset]);
			}
			if (innerMsg && innerStr)
				strncat(innerStr, innerMsg, MSGSIZE);
			offset++;
		}
		if (innerStr)
		{
			strncat(innerStr, "])", 3);
			//printf(" %s \n", innerStr);
			if (innerMsg && innerStr)
				strncat(innerStr, innerMsg, MSGSIZE);
			offset++;
		}
		//strncat(innerStr, "])", 3);
		json_push_back(label, json_new_a("raw_value", innerStr));

		safe_Free(innerMsg);  //free the string
		safe_Free(innerStr);  // free the string

		json_push_back(appData, label);
		json_push_back(appData, json_new_f("value", -1));

	}
	else
	{
#if defined _DEBUG
		printf("Application Client Description \n");
#endif
		byte_Swap_16(&m_App->paramCode);
		//get_Cache_Parameter_Code_Description(&myStr);
		snprintf((char*)myStr.c_str(), BASIC, "Application Client Log 0x%04" PRIx16"", m_App->paramCode);
		JSONNODE* appInfo = json_new(JSON_NODE);
		json_set_name(appInfo, (char*)myStr.c_str());

		snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_App->paramCode);
		json_push_back(appInfo, json_new_a("Application Client Parameter Code", (char*)myStr.c_str()));

		snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_App->paramControlByte);
		json_push_back(appInfo, json_new_a("Application Client Control Byte ", (char*)myStr.c_str()));
		snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_App->paramLength);
		json_push_back(appInfo, json_new_a("Application Client Length ", (char*)myStr.c_str()));

		// format to show the buffer data.
		if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
		{
			uint32_t lineNumber = 0;
			char* innerMsg = (char*)calloc(MSGSIZE, sizeof(char));
			char* innerStr = (char*)calloc(60, sizeof(char));
			uint32_t offset = 0;


			for (uint32_t outer = 0; outer < APP_CLIENT_DATA_LEN - 1; )
			{
				snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIX32 "", lineNumber);
				sprintf(innerMsg, "%02" PRIX8 "", m_App->data[offset]);
				// inner loop for creating a single ling of the buffer data
				for (uint32_t inner = 1; inner < 16 && offset < APP_CLIENT_DATA_LEN - 1; inner++)
				{
					sprintf(innerStr, "%02" PRIX8"", m_App->data[offset]);
					if (inner % 4 == 0)
					{
						strncat(innerMsg, " ", 1);
					}
					strncat(innerMsg, innerStr, 2);
					offset++;
				}
				// push the line to the json node
				json_push_back(appInfo, json_new_a((char*)myStr.c_str(), innerMsg));
				outer = offset;
				lineNumber = outer;
			}
			safe_Free(innerMsg);  //free the string
			safe_Free(innerStr);  // free the string
		}
		json_push_back(appData, appInfo);
	}
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
		json_set_name(pageInfo, "Application Client Log - Fh");
        uint16_t l_NumberOfPartitions = 0;

		for (uint32_t offset = 0; ((offset < m_PageLength) && (l_NumberOfPartitions <= MAX_PARTITION));)
		{
			if (offset+sizeof(sApplicationParams) < m_bufferLength && offset < UINT16_MAX)
			{
                l_NumberOfPartitions++;
                m_App = new sApplicationParams (&pData[offset]);
                offset += APP_CLIENT_DATA_LEN + 4;
				
				process_Client_Data(pageInfo,offset);
                delete m_App;
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
eReturnValues CScsiApplicationLog::get_PrePython_Client_Data(JSONNODE* masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (g_dataformat == PREPYTHON_DATA)
	{
		if (pData != NULL)
		{
			uint16_t l_NumberOfPartitions = 0;

			for (uint32_t offset = 0; ((offset < m_PageLength) && (l_NumberOfPartitions <= MAX_PARTITION));)
			{
				JSONNODE* pageInfo = json_new(JSON_NODE);
				if (offset + sizeof(sApplicationParams) < m_bufferLength && offset < UINT16_MAX)
				{
					l_NumberOfPartitions++;
					m_App = new sApplicationParams(&pData[offset]);
					byte_Swap_16(&m_App->paramCode);
					

					process_Client_Data(pageInfo,offset);
					offset += APP_CLIENT_DATA_LEN + 4;
					delete m_App;
				}
				else
				{
					return BAD_PARAMETER;
				}
				json_push_back(masterData, pageInfo);
			}

			retStatus = SUCCESS;
		}
		else
		{
			retStatus = MEMORY_FAILURE;
		}
	}
	return retStatus;
}