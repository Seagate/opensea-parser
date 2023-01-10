//
// CScsi_Supported_LogPages_Log.cpp Definition for parsing the supported log pages 
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Supported_LogPages_Log..cpp Definition for parsing the supported log pages 
//
#include "CScsi_Supported_LogPages_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiSupportedLog()
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
CScsiSupportedLog::CScsiSupportedLog()
	: pData()
	, m_SupportedName("Supported Log Pages Logs")
	, m_SupportedStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength(0)
	, m_SubPage(0)
	, m_Page(0)
	, m_ShowSubPage(false)
    , m_ShowSupportedPagesOnce(true)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_SupportedName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiSupportedLog()
//
//! \brief
//!   Description: Class constructor for the supported log pages
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//! \param subPage - set to true for using the subpages log
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiSupportedLog::CScsiSupportedLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength, bool subPage)
	: pData(buffer)
	, m_SupportedName("Supported Log Pages Logs")
	, m_SupportedStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_SubPage(0)
	, m_Page(0)
	, m_ShowSubPage(subPage)
    , m_ShowSupportedPagesOnce(true)
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_SupportedName.c_str());
	}
	if (buffer != NULL)
	{
		m_SupportedStatus = IN_PROGRESS;
	}
	else
	{
		m_SupportedStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiSupportedLog
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
CScsiSupportedLog::~CScsiSupportedLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Supported_And_Subpage_Description
//
//! \brief
//!   Description: parser out the data for supported and subpage
//
//  Entry:
//! \param description - string to give supported page decription
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiSupportedLog::get_Supported_And_Subpage_Description(std::string *description)
{
	switch (m_Page)
	{
		case SUPPORTED_LOG_PAGES:
		{
            if (m_ShowSupportedPagesOnce)
            {
                if (m_ShowSubPage && m_SubPage == 0xFF)
                {
                    description->assign("Supported Log Pages and Subpages");
                }
                else
                {
                    description->assign("Supported Log Pages");
                }
                m_ShowSupportedPagesOnce = false;
            }
			break;
		}
		case WRITE_ERROR_COUNTER:
		{
            description->assign("Write Error Counter");
			break;
		}
		case READ_ERROR_COUNTER:
		{
            description->assign("Read Error Counter");
			break;
		}
		case VERIFY_ERROR_COUNTER:
		{
            description->assign("Verify Error Counter");
			break;
		}
		case NON_MEDIUM_ERROR:
		{
            description->assign("Non-Medium Error");
			break;
		}
		case FORMAT_STATUS:
		{
            description->assign("Format Status");
			break;
		}
		case LOGICAL_BLOCK_PROVISIONING:
		{
            description->assign("Logical Block Provisioning");
			break;
		}
		case ENVIRONMENTAL:
		{
			if (m_ShowSubPage && m_SubPage == 0x00)
			{
                description->assign("Temperature");
			}
			else if (m_ShowSubPage && m_SubPage == 0x01)
			{
                description->assign("Environmental Reporting");
			}
			else if (m_ShowSubPage && m_SubPage == 0x02)
			{
                description->assign("Environmental Limits");
			}
			else
			{
                description->assign("Environmental");
			}
			break;
		}
		case START_STOP_CYCLE_COUNTER:
		{
            description->assign("Start-Stop Cycle Counter");
			break;
		}
		case APPLICATION_CLIENT:
		{
            description->assign("Application Client");
			break;
		}
		case SELF_TEST_RESULTS:
		{
            description->assign("Self-Test Results");
			break;
		}
		case SOLID_STATE_MEDIA:
		{
            description->assign("Solid State Media");
			break;
		}
        case ZONED_DEVICE_STATISTICS:
        {
            description->assign("Zoned Device Statistics");
            break;
        }
		case BACKGROUND_SCAN:
		{
			if (m_ShowSubPage && m_SubPage == 0x02)
			{
                description->assign("Background Operation");
			}
			else
			{
                description->assign("Background Scan");
			}
			break;
		}
		case PROTOCOL_SPECIFIC_PORT:
		{
            description->assign("SAS Protocol Log Page");
			break;
		}
		case POWER_CONDITION_TRANSITIONS:
		{
            description->assign("Protocol Specific Port");
			break;
		}
		case INFORMATIONAL_EXCEPTIONS:
		{
            description->assign("Informational Exceptions");
			break;
		}
		case CACHE_STATISTICS:
		{
            description->assign("Cache Statistics");
			break;
		}
		case FACTORY_LOG:
		{
            description->assign("Factory Log");
			break;
		}
		default:
		{
            std::ostringstream temp;
            temp << "Unknown 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Page);
            description->assign(temp.str());
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn process_Supported_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param SupportData - Json node that parsed Supported pages
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiSupportedLog::process_Supported_Data(JSONNODE *SupportData)
{
	std::string myStr;
#if defined _DEBUG
	printf("Supported Log Pages \n");
#endif
    std::ostringstream temp;
    temp << "Page 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Page);
    if (m_ShowSubPage)
    {
         temp << " SubPage 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_SubPage);
    }
    get_Supported_And_Subpage_Description(&myStr);
	json_push_back(SupportData, json_new_a(temp.str().c_str(), myStr.c_str()));
}
//-----------------------------------------------------------------------------
//
//! \fn get_Supported_Log_Data
//
//! \brief
//!   Description: parser out the data for tLog
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiSupportedLog::get_Supported_Log_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Supported Logs");

		for (size_t offset = 0; offset < static_cast<size_t>(m_PageLength); )
		{
			if (offset < m_bufferLength )
			{
				m_Page = static_cast<uint8_t>(pData[offset]);
				offset++;
				if (m_ShowSubPage && (offset +1 ) < m_bufferLength)
				{
					m_SubPage = static_cast<uint8_t>(pData[offset]);
					offset++;
				}
                if ((m_Page != 0 || m_SubPage != 0) )
                {
                    process_Supported_Data(pageInfo);
                }
                else if(m_ShowSupportedPagesOnce)
                { 
                    process_Supported_Data(pageInfo);
                }

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