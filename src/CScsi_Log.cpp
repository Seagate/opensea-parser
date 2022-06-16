//
// CScsiLog.cpp  Implementation of Base class CScsiLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
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
#include "CScsi_Solid_State_Drive_Log.h"
#include "CScsi_Zoned_Device_Statistics_Log.h"

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
    : bufferData(NULL)
    , m_LogSize(0)
    , m_name("SCSI Log")
    , m_ScsiStatus(IN_PROGRESS)
    , m_Page()
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
    : bufferData(NULL)
    , m_LogSize(0)
    , m_name("SCSI Log")
    , m_ScsiStatus(IN_PROGRESS)
	, m_Page()

{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == SUCCESS)
    {
        if (cCLog->get_Buffer() != NULL)
        {
            m_LogSize = cCLog->get_Size();
            bufferData = new uint8_t[m_LogSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
            memcpy(bufferData, cCLog->get_Buffer(), m_LogSize);
#else
            memcpy_s(bufferData, m_LogSize, cCLog->get_Buffer(), m_LogSize);// copy the buffer data to the class member pBuf
#endif
            m_ScsiStatus = get_Log_Parsed(masterData);							// init the data for getting the log
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
    if (bufferData != NULL)
    {
        delete[] bufferData;
    }
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
		sLogPageStruct* lpStruct = &myStr;

		lpStruct = reinterpret_cast<sLogPageStruct *>(bufferData);				// set a buffer to the point to the log page info
		byte_Swap_16(&lpStruct->pageLength);
		if (IsScsiLogPage(lpStruct->pageLength , M_GETBITRANGE(lpStruct->pageCode, 5, 0)) == true)
		{
			switch (M_GETBITRANGE(lpStruct->pageCode, 5, 0))
			{
			case SUPPORTED_LOG_PAGES:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Supported Log Pages Found" << std::endl;
				}
				CScsiSupportedLog *cSupport;
				if (lpStruct->subPage == 00)
				{
					cSupport = new CScsiSupportedLog(&bufferData[4], m_LogSize, lpStruct->pageLength, false);
				}
				else
				{
					cSupport = new CScsiSupportedLog(&bufferData[4], m_LogSize, lpStruct->pageLength, true);
				}
				retStatus = cSupport->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cSupport->parse_Supported_Log_Pages_Log(masterData);
					}
					catch (...)
					{
						retStatus = cSupport->get_Log_Status();
						delete(cSupport);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
				}
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
				cWriteError = new CScsiErrorCounterLog(&bufferData[4], m_LogSize, lpStruct->pageLength, lpStruct->pageCode);
				retStatus = cWriteError->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cWriteError->parse_Error_Counter_Log(masterData);
					}
					catch (...)
					{
						retStatus = cWriteError->get_Log_Status();
						delete(cWriteError);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cReadError = new CScsiErrorCounterLog(&bufferData[4], m_LogSize, lpStruct->pageLength, lpStruct->pageCode);
				retStatus = cReadError->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cReadError->parse_Error_Counter_Log(masterData);
					}
					catch (...)
					{
						retStatus = cReadError->get_Log_Status();
						delete(cReadError);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cVerifyError = new CScsiErrorCounterLog(&bufferData[4], m_LogSize, lpStruct->pageLength, lpStruct->pageCode);
				retStatus = cVerifyError->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cVerifyError->parse_Error_Counter_Log(masterData);
					}
					catch (...)
					{
						retStatus = cVerifyError->get_Log_Status();
						delete(cVerifyError);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}	
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
				cNonMedium = new CScsiNonMediumErrorCountLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cNonMedium->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cNonMedium->parse_Non_Medium_Error_Count_Log(masterData);
					}
					catch (...)
					{
						retStatus = cNonMedium->get_Log_Status();
						delete(cNonMedium);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cFormat = new CScsiFormatStatusLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cFormat->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cFormat->parse_Format_Status_Log(masterData);
					}
					catch (...)
					{
						retStatus = cFormat->get_Log_Status();
						delete(cFormat);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cLBA = new CScsiLBAProvisionLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cLBA->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cLBA->parse_LBA_Provision_Log(masterData);
					}
					catch (...)
					{
						retStatus = cLBA->get_Log_Status();
						delete(cLBA);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cEPA = new CScsiEnvironmentLog(bufferData, m_LogSize, lpStruct->subPage, masterData);
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
				cSS = new CScsiStartStop(&bufferData[4], lpStruct->pageLength, masterData);
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
				cApplicationClient = new CScsiApplicationLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cApplicationClient->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cApplicationClient->parse_Application_Client_Log(masterData);
					}
					catch (...)
					{
						retStatus = cApplicationClient->get_Log_Status();
						delete(cApplicationClient);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cSelfTest = new CScsi_DST_Results(&bufferData[4], m_LogSize, masterData);
				retStatus = cSelfTest->get_Log_Status();
				delete(cSelfTest);
			}
			break;
            case SOLID_STATE_MEDIA:
            {
                if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
                {
                    std::cout << "Solid State Drive Log Pages Found" << std::endl;
                }
                CScsiSolidStateDriveLog *cSSD;
                cSSD = new CScsiSolidStateDriveLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
                retStatus = cSSD->get_Solid_State_Drive_Log_Status();
                if (retStatus == IN_PROGRESS)
                {
					try
					{
						retStatus = cSSD->parse_Solid_State_Drive_Log(masterData);
					}
					catch (...)
					{
						retStatus = cSSD->get_Solid_State_Drive_Log_Status();
						delete(cSSD);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}   
                }
                delete(cSSD);
            }
            break;
            case ZONED_DEVICE_STATISTICS:
            {
                if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
                {
                    std::cout << "Zoned Device Statistics Log Pages Found" << std::endl;
                }
                CScsiZonedDeviceStatisticsLog *cZDS;
                cZDS = new CScsiZonedDeviceStatisticsLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
                retStatus = cZDS->get_Zoned_Device_Statistics_Log_Status();
                if (retStatus == IN_PROGRESS)
                {
					try
					{
						retStatus = cZDS->parse_Zoned_Device_Statistics_Log(masterData);
					}
					catch (...)
					{
						retStatus = cZDS->get_Zoned_Device_Statistics_Log_Status();
						delete(cZDS);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}  
                }
                delete(cZDS);
            }
            break;
			case BACKGROUND_SCAN:
			{
				if (lpStruct->subPage == 0x00)        // Background Scan
				{
					CScsiScanLog *cScan;
					cScan = new CScsiScanLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
					retStatus = cScan->get_Log_Status();
					if (retStatus == IN_PROGRESS)
					{
						try
						{
							retStatus = cScan->parse_Background_Scan_Log(masterData);
						}
						catch (...)
						{
							retStatus = cScan->get_Log_Status();
							delete(cScan);
							if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
							{
								return PARSE_FAILURE;
							}
							else
							{
								return retStatus;
							}
						}	
					}
					delete(cScan);
				}
				else if (lpStruct->subPage == 0x01)   // Pending Defects log
				{
					CScsiPendingDefectsLog *cPlist;
					cPlist = new CScsiPendingDefectsLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
					retStatus = cPlist->get_Log_Status();
					if (retStatus == IN_PROGRESS)
					{
						try
						{
							retStatus = cPlist->parse_Plist_Log(masterData);
						}
						catch (...)
						{
							retStatus = cPlist->get_Log_Status();
							delete(cPlist);
							if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
							{
								return PARSE_FAILURE;
							}
							else
							{
								return retStatus;
							}
						}
					}
					delete(cPlist);
				}
				else if (lpStruct->subPage == 0x02)   // Background Operation
				{
					CScsiOperationLog *cOperation;
					cOperation = new CScsiOperationLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
					retStatus = cOperation->get_Log_Status();
					if (retStatus == IN_PROGRESS)
					{
						try
						{
							retStatus = cOperation->parse_Background_Operationss_Log(masterData);
						}
						catch (...)
						{
							retStatus = cOperation->get_Log_Status();
							delete(cOperation);
							if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
							{
								return PARSE_FAILURE;
							}
							else
							{
								return retStatus;
							}
						}
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
				cPSP = new CScsiProtocolPortLog(&bufferData[4], m_LogSize);
				//uint16_t l_pageLength = *(reinterpret_cast<uint16_t*>(&bufferData[2]));
				retStatus = cPSP->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						cPSP->set_PSP_Page_Length_NoSwap(lpStruct->pageLength);  //TODO: noswap vs swap
						retStatus = cPSP->parse_Protocol_Port_Log(masterData);
					}
					catch (...)
					{
						retStatus = cPSP->get_Log_Status();
						delete(cPSP);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
				}
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
				cPower = new CScsiPowerConditiontLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cPower->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cPower->parse_Power_Condition_Transitions_Log(masterData);
					}
					catch (...)
					{
						retStatus = cPower->get_Log_Status();
						delete(cPower);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
				}
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
				cInfo = new CScsiInformationalExeptionsLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cInfo->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cInfo->get_Informational_Exceptions(masterData);
					}
					catch (...)
					{
						retStatus = cInfo->get_Log_Status();
						delete(cInfo);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				cCache = new CScsiCacheLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				if (cCache->get_Log_Status() == SUCCESS)
				{
					try
					{
						retStatus = cCache->parse_Cache_Statistics_Log(masterData);
					}
					catch (...)
					{
						retStatus = cCache->get_Log_Status();
						delete(cCache);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
				}
				delete (cCache);
			}
			break;
			case SEAGATE_SPECIFIC_LOG:
			{
				if (lpStruct->subPage == FARM_LOG_PAGE)   // Farm Log
				{
					CSCSI_Farm_Log* pCFarm;
					pCFarm = new CSCSI_Farm_Log(bufferData, lpStruct->pageLength + 4, false);
					if (pCFarm->get_Log_Status() == SUCCESS)
					{
						try
						{
							pCFarm->print_Page_One_Node(masterData);
							retStatus = pCFarm->get_Log_Status();
						}
						catch (...)
						{
							retStatus = pCFarm->get_Log_Status();
							delete(pCFarm);
							if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
							{
								return PARSE_FAILURE;
							}
							else
							{
								return retStatus;
							}
						}
					}
					delete(pCFarm);
				}
				else if (lpStruct->subPage == FARM_FACTORY_LOG_PAGE)   // Farm Log
				{
					CSCSI_Farm_Log* pCFarm;
					pCFarm = new CSCSI_Farm_Log(bufferData, lpStruct->pageLength + 4, false);
					if (pCFarm->get_Log_Status() == SUCCESS)
					{
						try
						{
							pCFarm->print_Page_One_Node(masterData);
							retStatus = pCFarm->get_Log_Status();
						}
						catch (...)
						{
							retStatus = pCFarm->get_Log_Status();
							delete(pCFarm);
							if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
							{
								return PARSE_FAILURE;
							}
							else
							{
								return retStatus;
							}
						}
					}
					delete(pCFarm);
				}
				else if (lpStruct->subPage >= FARM_TIME_SERIES_0 && lpStruct->subPage <= FARM_TEMP_TRIGGER_LOG_PAGE)   // FARM log when Temperature exceeds 70 c
				{
					CSCSI_Farm_Log* pCFarm;
					pCFarm = new CSCSI_Farm_Log(bufferData, lpStruct->pageLength + 4, true);  // issue with the log bufer size
					if (pCFarm->get_Log_Status() == SUCCESS)
					{
						retStatus = pCFarm->get_Log_Status();
						if (retStatus == SUCCESS)
						{
							try
							{
								pCFarm->print_Page_One_Node(masterData);
								retStatus = pCFarm->get_Log_Status();
							}
							catch (...)
							{
								retStatus = pCFarm->get_Log_Status();
								delete(pCFarm);
								if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
								{
									return PARSE_FAILURE;
								}
								else
								{
									return retStatus;
								}
							}
						}
					}
					delete(pCFarm);   // SAS Log Page 0x3D subpage 0x10 - 0xC7
				}
			}
			break;
			case FACTORY_LOG:
			{
				if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
				{
					std::cout << "Factory Log Pages Found" << std::endl;
				}
				CScsiFactoryLog *cFactory;
				cFactory = new CScsiFactoryLog(&bufferData[4], m_LogSize, lpStruct->pageLength);
				retStatus = cFactory->get_Log_Status();
				if (retStatus == IN_PROGRESS)
				{
					try
					{
						retStatus = cFactory->parse_Factory_Log(masterData);
					}
					catch (...)
					{
						retStatus = cFactory->get_Log_Status();
						delete(cFactory);
						if (retStatus == SUCCESS || retStatus == IN_PROGRESS)
						{
							return PARSE_FAILURE;
						}
						else
						{
							return retStatus;
						}
					}
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
				retStatus = static_cast<eReturnValues>(NOT_SUPPORTED);
			}
			break;
			}
		}
		else
		{
			if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
			{
				std::cout << "Not Found" << std::endl;
			}
			std::cout << "not supported    check --logType" << std::endl;
			retStatus = static_cast<eReturnValues>(NOT_SUPPORTED);
		}
	}
	else
	{
		retStatus = FAILURE;
	}
	return retStatus;
}