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
#include <sstream>
#include <iomanip>

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
void CScsiApplicationLog::process_Client_Data(JSONNODE *appData)
{
#if defined _DEBUG
	printf("Application Client Description \n");
#endif
	byte_Swap_16(&m_App->paramCode);
	//get_Cache_Parameter_Code_Description(&myStr);
    std::ostringstream temp;
    temp << "Application Client Log 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_App->paramCode;
	JSONNODE *appInfo = json_new(JSON_NODE);
	json_set_name(appInfo, temp.str().c_str());
    
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_App->paramCode;
    json_push_back(appInfo, json_new_a("Application Client Parameter Code", temp.str().c_str()));

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_App->paramControlByte);
    json_push_back(appInfo, json_new_a("Application Client Control Byte ", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_App->paramLength);
    json_push_back(appInfo, json_new_a("Application Client Length ", temp.str().c_str()));
    
    // format to show the buffer data.
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        uint32_t lineNumber = 0;
        uint32_t offset = 0;


        for (uint32_t outer = 0; outer < APP_CLIENT_DATA_LEN - 1; )
        {
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << lineNumber;

            std::ostringstream innerMsg;
            innerMsg << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_App->data[offset]);
            // inner loop for creating a single ling of the buffer data
            for (uint32_t inner = 1; inner < 16 && offset < APP_CLIENT_DATA_LEN - 1; inner++)
            {
                innerMsg << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_App->data[offset]);
                if (inner % 4 == 0)
                {
                    innerMsg << " ";
                }
                offset++;
            }
            // push the line to the json node
            json_push_back(appInfo, json_new_a(temp.str().c_str(), innerMsg.str().c_str()));
            outer = offset;
            lineNumber = outer;
        }
    }
	json_push_back(appData, appInfo);
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
				
				process_Client_Data(pageInfo);
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
