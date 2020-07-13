//
// CScsi_Supported_LogPages_Log.cpp Definition for parsing the supported log pages 
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
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
                    snprintf((char*)description->c_str(), BASIC, "Supported Log Pages and Subpages");
                }
                else
                {
                    snprintf((char*)description->c_str(), BASIC, "Supported Log Pages");
                }
                m_ShowSupportedPagesOnce = false;
            }
			break;
		}
		case WRITE_ERROR_COUNTER:
		{
			snprintf((char*)description->c_str(), BASIC, "Write Error Counter");
			break;
		}
		case READ_ERROR_COUNTER:
		{
			snprintf((char*)description->c_str(), BASIC, "Read Error Counter");
			break;
		}
		case VERIFY_ERROR_COUNTER:
		{
			snprintf((char*)description->c_str(), BASIC, "Verify Error Counter");
			break;
		}
		case NON_MEDIUM_ERROR:
		{
			snprintf((char*)description->c_str(), BASIC, "Non-Medium Error");
			break;
		}
		case FORMAT_STATUS:
		{
			snprintf((char*)description->c_str(), BASIC, "Format Status");
			break;
		}
		case LOGICAL_BLOCK_PROVISIONING:
		{
			snprintf((char*)description->c_str(), BASIC, "Logical Block Provisioning");
			break;
		}
		case ENVIRONMENTAL:
		{
			if (m_ShowSubPage && m_SubPage == 0x00)
			{
				snprintf((char*)description->c_str(), BASIC, "Temperature");
			}
			else if (m_ShowSubPage && m_SubPage == 0x01)
			{
				snprintf((char*)description->c_str(), BASIC, "Environmental Reporting");
			}
			else if (m_ShowSubPage && m_SubPage == 0x02)
			{
				snprintf((char*)description->c_str(), BASIC, "Environmental Limits");
			}
			else
			{
				snprintf((char*)description->c_str(), BASIC, "Environmental");
			}
			break;
		}
		case START_STOP_CYCLE_COUNTER:
		{
			snprintf((char*)description->c_str(), BASIC, "Start-Stop Cycle Counter");
			break;
		}
		case APPLICATION_CLIENT:
		{
			snprintf((char*)description->c_str(), BASIC, "Application Client");
			break;
		}
		case SELF_TEST_RESULTS:
		{
			snprintf((char*)description->c_str(), BASIC, "Self-Test Results");
			break;
		}
		case SOLID_STATE_MEDIA:
		{
			snprintf((char*)description->c_str(), BASIC, "Solid State Media");
			break;
		}
		case BACKGROUND_SCAN:
		{
			if (m_ShowSubPage && m_SubPage == 0x02)
			{
				snprintf((char*)description->c_str(), BASIC, "Background Operation");
			}
			else
			{
				snprintf((char*)description->c_str(), BASIC, "Background Scan");
			}
			break;
		}
		case PROTOCOL_SPECIFIC_PORT:
		{
			snprintf((char*)description->c_str(), BASIC, "SAS Protocol Log Page");
			break;
		}
		case POWER_CONDITION_TRANSITIONS:
		{
			snprintf((char*)description->c_str(), BASIC, "Protocol Specific Port");
			break;
		}
		case INFORMATIONAL_EXCEPTIONS:
		{
			snprintf((char*)description->c_str(), BASIC, "Informational Exceptions");
			break;
		}
		case CACHE_STATISTICS:
		{
			snprintf((char*)description->c_str(), BASIC, "Cache Statistics");
			break;
		}
		case FACTORY_LOG:
		{
			snprintf((char*)description->c_str(), BASIC, "Factory Log");
			break;
		}
		default:
		{
			snprintf((char*)description->c_str(), BASIC, "Unknown 0x%02" PRIx8"", m_Page);
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
	std::string myStr = "";
	myStr.resize(BASIC);
	std::string myHeader = "";
	myHeader.resize(BASIC);
#if defined_DEBUG
	printf("Supported Log Pages \n");
#endif

    if (m_ShowSubPage)
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Page 0x%02" PRIx8" SubPage 0x%02" PRIx8"", m_Page, m_SubPage);
    }
    else
    {
        snprintf((char*)myHeader.c_str(), BASIC, "Page 0x%02" PRIx8"", m_Page);
    }
    get_Supported_And_Subpage_Description(&myStr);
	json_push_back(SupportData, json_new_a((char*)myHeader.c_str(), (char*)myStr.c_str()));
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
	std::string myStr = "";
	myStr.resize(BASIC);
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		myStr = "Supported Logs";
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, (char*)myStr.c_str());

		for (size_t offset = 0; offset < (size_t) m_PageLength; )
		{
			if (offset < m_bufferLength )
			{
				m_Page = (uint8_t)pData[offset];
				offset++;
				if (m_ShowSubPage && (offset +1 ) < m_bufferLength)
				{
					m_SubPage = (uint8_t)pData[offset];
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