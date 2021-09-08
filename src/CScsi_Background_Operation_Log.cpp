//
// CScsi_Background_Operation_Log.cpp  Definition of Background Operation log page reports parameters that are specific to background operations.
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Background_Operation_Log.cpp  Definition of Background Operation log page reports parameters that are specific to background operations.
//
#include "CScsi_Background_Operation_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiOperationLog()
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
CScsiOperationLog::CScsiOperationLog()
	: pData()
	, m_OperationName("Background Operation Log")
	, m_OperationsStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_Operation()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_OperationName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiOperationLog()
//
//! \brief
//!   Description: Class constructor for the Background opteration log
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiOperationLog::CScsiOperationLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(NULL)
	, m_OperationName("Background Operation Log")
	, m_OperationsStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_Operation()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_OperationName.c_str());
	}
    pData = new uint8_t[pageLength];								// new a buffer to the point				
#ifndef _WIN64
    memcpy(pData, buffer, pageLength);
#else
    memcpy_s(pData, pageLength, buffer, pageLength);// copy the buffer data to the class member pBuf
#endif
	if (pData != NULL)
	{
		m_OperationsStatus = IN_PROGRESS;
	}
	else
	{
		m_OperationsStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiOperationLog
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
CScsiOperationLog::~CScsiOperationLog()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Background_Operations_status
//
//! \brief
//!   Description: parser out the data for operation status Information
//
//  Entry:
//! \param status - string to give the Operations status on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiOperationLog::get_Background_Operations_status(std::string *status)
{
	switch (m_Operation->bo_Status)
	{
		case 0x00:
		{
			snprintf((char*)status->c_str(), BASIC, "No indication");
			break;
		}
		case 0x01:
		{
			snprintf((char*)status->c_str(), BASIC, "No advanced background operation being performed");
			break;
		}
		case 0x02:
		{
			snprintf((char*)status->c_str(), BASIC, "Host initiated advanced background operation being performed");
			break;
		}
		case 0x03:
		{
			snprintf((char*)status->c_str(), BASIC, "Device initiated advanced background operation being performed");
			break;
		}
		default:
		{
			snprintf((char*)status->c_str(), BASIC, "reserved");
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn process_Background_Operations_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed Operations data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiOperationLog::process_Background_Operations_Data(JSONNODE *operationData, uint32_t offset)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined _DEBUG
	printf("Cache Event Description \n");
#endif
	byte_Swap_16(&m_Operation->paramCode);
	if (g_dataformat == PREPYTHON_DATA)
	{
		JSONNODE* data = json_new(JSON_NODE);

		json_push_back(data, json_new_a("name", "background_operation_status"));
		JSONNODE* label = json_new(JSON_NODE);
		json_set_name(label, "labels");
		snprintf((char*)myStr.c_str(), BASIC, "scsi-log-page:0x%" PRIx8",%" PRIx8":0x%" PRIx16":%" PRIu32"", 0x15, 0x02, m_Operation->paramCode, offset);
		json_push_back(label, json_new_a("metric_source", (char*)myStr.c_str()));
		get_Background_Operations_status(&myStr);
		json_push_back(label, json_new_a("operation", (char*)myStr.c_str()));
		json_push_back(label, json_new_a("units", "status"));
		json_push_back(data, label);
		json_push_back(data, json_new_i("value", m_Operation->bo_Status));
		json_push_back(operationData, data);

	}
	else
	{
		
		snprintf((char*)myStr.c_str(), BASIC, "Background Operation Description %" PRId16"", m_Operation->paramCode);
		JSONNODE* operationInfo = json_new(JSON_NODE);
		json_set_name(operationInfo, (char*)myStr.c_str());

		snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_Operation->paramCode);
		json_push_back(operationInfo, json_new_a("Background Operation Parameter Code", (char*)myStr.c_str()));

		snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Operation->paramControlByte);
		json_push_back(operationInfo, json_new_a("Background Operation Control Byte ", (char*)myStr.c_str()));
		snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Operation->paramLength);
		json_push_back(operationInfo, json_new_a("Background Operation Length ", (char*)myStr.c_str()));

		get_Background_Operations_status(&myStr);
		json_push_back(operationInfo, json_new_i((char*)myStr.c_str(), static_cast<uint32_t>(m_Operation->bo_Status)));

		json_push_back(operationData, operationInfo);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Background_Operations_Data
//
//! \brief
//!   Description: parser out the data for the Background operation log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiOperationLog::get_Background_Operations_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Background Operation Log - 15h");

		for (uint32_t offset = 0; offset < m_PageLength; )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
				m_Operation = (sOperationParams *)&pData[offset];
				process_Background_Operations_Data(pageInfo,offset);
				offset += sizeof(sOperationParams);
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