//
// CScsiLog.cpp  Implementation of Base class CScsiLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsiLog.cpp  Implementation of Base class CScsiLog
//
#include "CScsi_Log.h"
#include "CScsi_Environmental_Logs.h"
#include "CScsi_Start_Stop_Cycle_Counter_Log.h"
#include "CScsi_Self_Test_Results_Log.h"
#include "CScsi_Protocol_Specific_Port_Log.h"
#include "CScsi_Power_Condition_Transitions_Log.h"
#include "CScsi_Cache_Statistics_Log.h"
#include "CScsi_Error_Counter_Log.h"
#include "CScsi_Supported_LogPages_Log.h"
#include "CScsi_Pending_Defects_Log.h"
#include "CScsi_Non_Medium_Error_Count_Log.h"
#include "CScsi_Logical_Block_Provisioning_Log.h"
#include "CScsi_Informational_Exeptions_Log.h"
#include "CScsi_Format_Status_Log.h"
#include "CScsi_Factory_Log.h"
#include "CScsi_Background_Operation_Log.h"
#include "CScsi_Background_Scan_Log.h"
#include "CScsi_Farm_Log.h"
#include "CScsi_Application_Client_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiLog()
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
CScsiLog::CScsiLog()
	: m_name("SCSI Log")
	, m_ScsiStatus(IN_PROGRESS)
	, m_Page()
	, bufferData()
	, m_LogSize(0)
{
	
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiLog()
//
//! \brief
//!   Description: Class constructor for the CScsiLog
//
//  Entry:
//! \param fileName = the name of the file that is to be read in
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiLog::CScsiLog(const std::string fileName, JSONNODE *masterData)
    : m_name("SCSI Log")
	, m_ScsiStatus(IN_PROGRESS)
	, m_Page()
	, bufferData()
	, m_LogSize(0)
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_LogSize = cCLog->get_Size();
			bufferData = new uint8_t[m_LogSize];								// new a buffer to the point				
#ifdef __linux__ //To make old gcc compilers happy
			memcpy(bufferData, cCLog->get_Buffer(), m_LogSize);
#else
			memcpy_s(bufferData, m_LogSize, cCLog->get_Buffer(), m_LogSize);// copy the buffer data to the class member pBuf
#endif
			m_ScsiStatus = get_Log_Parsed(masterData);							// init the data for getting the log
			m_ScsiStatus = IN_PROGRESS;
		}
		else
		{

			m_ScsiStatus = FAILURE;
		}
	}
	else
	{
		m_ScsiStatus = cCLog->get_Log_Status();
	}
	delete (cCLog);
}

//-----------------------------------------------------------------------------
//
//! \fn CScsiLog::~CScsiLog()
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
CScsiLog::~CScsiLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Parsed
//
//! \brief
//!   Description: Class to parse teh log page format and then call the correct class to parse out the data
//
//  Entry:
//! \param masterData - Master Jyson node for holding all the parsed data
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiLog::get_Log_Parsed(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (bufferData != NULL)
	{
		sLogPageStruct myStr;
		sLogPageStruct* m_Page = &myStr;

		m_Page = (sLogPageStruct *)bufferData;				// set a buffer to the point to the log page info
		byte_Swap_16(&m_Page->pageLength);
		if (IsScsiLogPage(m_Page->pageLength , M_GETBITRANGE(m_Page->pageCode, 5, 0)) == true)
		{
			uint8_t pageCode = 0;
			switch (M_GETBITRANGE(m_Page->pageCode, 5, 0))
			{
			case SUPPORTED_LOG_PAGES:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Supported Log Pages Found" << std::endl;
				}
				CScsiSupportedLog *cSupport;
				if (m_Page->subPage == 00)
				{
					cSupport = new CScsiSupportedLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength, false);
				}
				else
				{
					cSupport = new CScsiSupportedLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength, true);
				}
				retStatus = cSupport->parse_Supported_Log_Pages_Log(masterData);
				delete(cSupport);
			}
			break;
			case WRITE_ERROR_COUNTER:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Write Error Counter Log Pages Found" << std::endl;
				}
				CScsiErrorCounterLog *cWriteError;
				cWriteError = new CScsiErrorCounterLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength, m_Page->pageCode);
				retStatus = cWriteError->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cWriteError->parse_Error_Counter_Log(masterData);
				}
				delete(cWriteError);
			}
			break;
			case READ_ERROR_COUNTER:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Read Error Counter Log Pages Found" << std::endl;
				}
				CScsiErrorCounterLog *cReadError;
				cReadError = new CScsiErrorCounterLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength, m_Page->pageCode);
				retStatus = cReadError->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cReadError->parse_Error_Counter_Log(masterData);
				}
				delete(cReadError);
			}
			break;
			case VERIFY_ERROR_COUNTER:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Verify Error Counter Log Pages Found" << std::endl;
				}
				CScsiErrorCounterLog *cVerifyError;
				cVerifyError = new CScsiErrorCounterLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength, m_Page->pageCode);
				retStatus = cVerifyError->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cVerifyError->parse_Error_Counter_Log(masterData);
				}
				delete(cVerifyError);
			}
			break;
			case NON_MEDIUM_ERROR:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Non-Medium Error Log Pages Found" << std::endl;
				}
				CScsiNonMediumErrorCountLog *cNonMedium;
				cNonMedium = new CScsiNonMediumErrorCountLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cNonMedium->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cNonMedium->parse_Non_Medium_Error_Count_Log(masterData);
				}
				delete(cNonMedium);
			}
			break;
			case FORMAT_STATUS:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Format Status Log Pages Found" << std::endl;
				}

				CScsiFormatStatusLog *cFormat;
				cFormat = new CScsiFormatStatusLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cFormat->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cFormat->parse_Format_Status_Log(masterData);
				}
				delete(cFormat);
			}
			break;
			case LOGICAL_BLOCK_PROVISIONING:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Logical Block Provisioning Log Pages Found" << std::endl;
				}
				CScsiLBAProvisionLog *cLBA;
				cLBA = new CScsiLBAProvisionLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cLBA->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cLBA->parse_LBA_Provision_Log(masterData);
				}
				delete(cLBA);
			}
			break;
			case ENVIRONMENTAL:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Environmental Log Found" << std::endl;
				}
				CScsiEnvironmentLog *cEPA;
				cEPA = new CScsiEnvironmentLog((uint8_t *)bufferData, m_LogSize, m_Page->subPage, masterData);
				retStatus = cEPA->get_Log_Status();
				delete (cEPA);
			}
			break;
			case START_STOP_CYCLE_COUNTER:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Start Stop Cycle Log Found" << std::endl;
				}
				CScsiStartStop *cSS;
				cSS = new CScsiStartStop((uint8_t *)&bufferData[4], m_LogSize, masterData);
				retStatus = cSS->get_Log_Status();
				delete(cSS);
			}
			break;
			case APPLICATION_CLIENT:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Application Client Log Pages Found" << std::endl;
				}
				CScsiApplicationLog *cApplicationClient;
				cApplicationClient = new CScsiApplicationLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cApplicationClient->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cApplicationClient->parse_Application_Client_Log(masterData);
				}
				delete(cApplicationClient);
			}
			break;
			case SELF_TEST_RESULTS:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Self Test Log Found" << std::endl;
				}
				CScsi_DST_Results *cSelfTest;
				cSelfTest = new CScsi_DST_Results((uint8_t *)&bufferData[4], m_LogSize, masterData);
				retStatus = cSelfTest->get_Log_Status();
				delete(cSelfTest);
			}
			break;
			case SOLID_STATE_MEDIA:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Solid State Media Log Pages Found" << std::endl;
				}
				std::cout << "not supported at this time" << std::endl;
				retStatus = SUCCESS;
			}
			break;
			case BACKGROUND_SCAN:
			{
				if (m_Page->subPage == 0x00)        // Background Scan
				{
					CScsiScanLog *cScan;
					cScan = new CScsiScanLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
					retStatus = cScan->get_Log_Status();
					if (retStatus == IN_PROGRESS)
					{
						retStatus = cScan->parse_Background_Scan_Log(masterData);
					}
					delete(cScan);
				}
				else if (m_Page->subPage == 0x01)   // Pending Defects log
				{
					CScsiPendingDefectsLog *cPlist;
					cPlist = new CScsiPendingDefectsLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
					retStatus = cPlist->get_Log_Status();
					if (retStatus == IN_PROGRESS)
					{
						retStatus = cPlist->parse_Supported_Log_Pages_Log(masterData);
					}
					delete(cPlist);
				}
				else if (m_Page->subPage == 0x02)   // Background Operation
				{
					CScsiOperationLog *cOperation;
					cOperation = new CScsiOperationLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
					retStatus = cOperation->get_Log_Status();
					if (retStatus == IN_PROGRESS)
					{
						retStatus = cOperation->parse_Background_Operationss_Log(masterData);
					}
					delete(cOperation);
				}
			}
			break;
			case PROTOCOL_SPECIFIC_PORT:
			{

				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Protocol Specific Port Log Pages Found" << std::endl;
				}
				CScsiProtocolPortLog * cPSP;
				cPSP = new CScsiProtocolPortLog((uint8_t *)&bufferData[4], m_LogSize);
				cPSP->set_PSP_Page_Length(m_Page->pageLength);
				retStatus = cPSP->parse_Protocol_Port_Log(masterData);
				delete (cPSP);
			}
			break;
			case POWER_CONDITION_TRANSITIONS:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Power Condition Transitions Log Pages Found" << std::endl;
				}
				CScsiPowerConditiontLog *cPower;
				cPower = new CScsiPowerConditiontLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cPower->parse_Power_Condition_Transitions_Log(masterData);
				delete(cPower);
			}
			break;
			case INFORMATIONAL_EXCEPTIONS:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Informational Exceptions Log Pages Found" << std::endl;
				}
				CScsiInformationalExeptionsLog *cInfo;
				cInfo = new CScsiInformationalExeptionsLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cInfo->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cInfo->get_Informational_Exceptions(masterData);
				}
				delete(cInfo);
			}
			break;
			case CACHE_STATISTICS:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Cache Statistics Log Pages Found" << std::endl;
				}
				CScsiCacheLog *cCache;
				cCache = new CScsiCacheLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cCache->parse_Cache_Statistics_Log(masterData);
				delete (cCache);
			}
			break;
			case SEAGATE_FARM_LOG:
			{
				CSCSI_Farm_Log *pCFarm;
				pCFarm = new CSCSI_Farm_Log((uint8_t *)bufferData, m_LogSize);
				if (pCFarm->get_Log_Status() == SUCCESS)
				{
					retStatus = pCFarm->ParseFarmLog();
					if (retStatus == SUCCESS)
					{
						pCFarm->print_All_Pages(masterData);
					}
				}
				delete(pCFarm);
			}
			break;
			case FACTORY_LOG:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Factory Log Pages Found" << std::endl;
				}
				CScsiFactoryLog *cFactory;
				cFactory = new CScsiFactoryLog((uint8_t *)&bufferData[4], m_LogSize, m_Page->pageLength);
				retStatus = cFactory->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					retStatus = cFactory->parse_Factory_Log(masterData);
				}
				delete(cFactory);
			}
			break;
			default:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Not Found" << std::endl;
				}
				std::cout << "not supported" << std::endl;
				retStatus = SUCCESS;
			}
			break;
			}
		}
		else
		{
			if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
			{
				std::cout << "Invalid Length" << std::endl;
			}
			std::cout << "Invalid Length    check --LogType" << std::endl;
			retStatus = static_cast<eReturnValues>(INVALID_LENGTH);
		}
	}
	else
	{
		retStatus = FAILURE;
	}
	return retStatus;
}