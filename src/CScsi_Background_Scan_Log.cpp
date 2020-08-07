//
// CScsi_Background_Scan_Log.cpp  Definition of Background Scan Log Page
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Background_Scan_Log.cpp  Definition of Background Scan Log Page
//
#include "CScsi_Background_Scan_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiScanLog()
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
CScsiScanLog::CScsiScanLog()
	: pData()
	, m_ScanName("Background Scan Log")
	, m_ScanStatus(IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_ScanParam()
	, m_defect()
    , m_ParamHeader()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ScanName.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiScanLog()
//
//! \brief
//!   Description: Class constructor for the Cache Statistics
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiScanLog::CScsiScanLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
	: pData(NULL)
	, m_ScanName("Background Scan Log")
	, m_ScanStatus(IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_ScanParam()
	, m_defect()
    , m_ParamHeader()
{
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ScanName.c_str());
	}
    pData = new uint8_t[pageLength];								// new a buffer to the point				
#ifndef _WIN64
    memcpy(pData, buffer, pageLength);
#else
    memcpy_s(pData, bufferSize, buffer, pageLength);// copy the buffer data to the class member pBuf
#endif
	if (pData != NULL)
	{
		m_ScanStatus = IN_PROGRESS;
	}
	else
	{
		m_ScanStatus = FAILURE;
	}

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiScanLog
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
CScsiScanLog::~CScsiScanLog()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Scan_Status_Description
//
//! \brief
//!   Description: parser out the description of the status 
//
//  Entry:
//! \param description - string to give the status description
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiScanLog::get_Scan_Status_Description(std::string *scan)
{
	switch (m_ScanParam->status)
	{
		case 0x00:
		{
			snprintf((char*)scan->c_str(), BASIC, "No background scans active");
			break;
		}
		case 0x01:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan is active");
			break;
		}
		case 0x02:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background prescan is active");
			break;
		}
		case 0x03:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan halted due to fatal error");
			break;
		}
		case 0x04:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan halted due to a vendor specific pattern of errors");
			break;
		}
		case 0x05:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan halted due to medium formatted without Plist");
			break;
		}
		case 0x06:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan halted - vendor specific cause");
			break;
		}
		case 0x07:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan halted due to temperature out of allowed range");
			break;
		}
		case 0x08:
		{
			snprintf((char*)scan->c_str(), BASIC, "Background medium scan halted, waiting for Background Medium Interval timer expiration");
			break;
		}
		default:
		{
			snprintf((char*)scan->c_str(), BASIC, "Reserved");
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//! \fn process_Scan_Status_Data
//
//! \brief
//!   Description: parser out the data for status headers
//
//  Entry:
//! \param eventData - Json node that parsed scan status will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiScanLog::process_Scan_Status_Data(JSONNODE *scanData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
#if defined _DEBUG
	printf("Background Scan Status Description \n");
#endif
	byte_Swap_16(&m_ScanParam->paramCode);
	snprintf((char*)myStr.c_str(), BASIC, "Background Scan Status");
	JSONNODE *statusInfo = json_new(JSON_NODE);
	json_set_name(statusInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_ScanParam->paramCode);
	json_push_back(statusInfo, json_new_a("Background Scan Status Parameter Code", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_ScanParam->paramControlByte);
	json_push_back(statusInfo, json_new_a("Background Scan Status Control Byte ", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_ScanParam->paramLength);
	json_push_back(statusInfo, json_new_a("Background Scan Status Length ", (char*)myStr.c_str()));

	byte_Swap_32(&m_ScanParam->timeStamp);
	json_push_back(statusInfo, json_new_i("SMART Time Stamp", static_cast<uint32_t>(m_ScanParam->timeStamp)));
	get_Scan_Status_Description(&myStr);
	json_push_back(statusInfo, json_new_i((char*)myStr.c_str(), static_cast<uint32_t>(m_ScanParam->status)));
	byte_Swap_16(&m_ScanParam->scansPerformed);
	json_push_back(statusInfo, json_new_i("Number of Background Scans Performed", static_cast<uint32_t>(m_ScanParam->scansPerformed)));
	byte_Swap_16(&m_ScanParam->mediumProgress);
	json_push_back(statusInfo, json_new_i("Background Medium Scan Progress", static_cast<uint32_t>(m_ScanParam->mediumProgress)));
	byte_Swap_16(&m_ScanParam->mediumPerformed);
	json_push_back(statusInfo, json_new_i("Number of Background Medium Scans Performed", static_cast<uint32_t>(m_ScanParam->mediumPerformed)));


	json_push_back(scanData, statusInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Scan_Defect_Status_Description
//
//! \brief
//!   Description: parser out the description of the defect status 
//
//  Entry:
//! \param description - string to give the status description
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiScanLog::get_Scan_Defect_Status_Description(std::string *defect)
{
	switch (M_GETBITRANGE(m_defect->status,7,4))
	{
		case 0x01:
		{
			snprintf((char*)defect->c_str(), BASIC, "The LBA has not yet been reassigned");
			break;
		}
		case 0x02:
		{
			snprintf((char*)defect->c_str(), BASIC, "Performed Automatic Read Reassignment for the LBA");
			break;
		}
		case 0x03:
		{
			snprintf((char*)defect->c_str(), BASIC, "Reserved");
			break;
		}
		case 0x04:
		{
			snprintf((char*)defect->c_str(), BASIC, "Attempt to Perform Automatic Read Reassignment Failed");
			break;
		}
		case 0x05:
		{
			snprintf((char*)defect->c_str(), BASIC, "Error Corrected, Device rewriting the LBA without Performing Reassign Operation.");
			break;
		}
		case 0x06:
		{
			snprintf((char*)defect->c_str(), BASIC, "Application Client caused automatic write reassignment for the LBA or LBPRZ");
			break;
		}
		case 0x07:
		{
			snprintf((char*)defect->c_str(), BASIC, "Application Client,  Reassign by Command - Invalid Data");
			break;
		}
		case 0x08:
		{
			snprintf((char*)defect->c_str(), BASIC, "Application Client, Reassign Command Failed");
			break;
		}
		default:
		{
			snprintf((char*)defect->c_str(), BASIC, "Reserved");
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//! \fn process_Defect_Data
//
//! \brief
//!   Description: parser out the data for each defect
//
//  Entry:
//! \param eventData - Json node that parsed defect will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiScanLog::process_Defect_Data(JSONNODE *defectData)
{
	std::string myStr = "";
	myStr.resize(BASIC);
	std::string headerStr = "";
	headerStr.resize(BASIC);
#if defined _DEBUG
	printf("Background Scan Defect Description \n");
#endif
	byte_Swap_16(&m_defect->paramCode);
	snprintf((char*)myStr.c_str(), BASIC, "Background Scan Defect Location 0x%" PRIX16"", m_defect->paramCode);;
	JSONNODE *defectInfo = json_new(JSON_NODE);
	json_set_name(defectInfo, (char*)myStr.c_str());

	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_defect->paramCode);
	json_push_back(defectInfo, json_new_a("Background Scan Defect Parameter Code", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_defect->paramControlByte);
	json_push_back(defectInfo, json_new_a("Background Scan Defect Control Byte ", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_defect->paramLength);
	json_push_back(defectInfo, json_new_a("Background Scan Defect Length ", (char*)myStr.c_str()));

	byte_Swap_32(&m_defect->powerOnMinutes);
	json_push_back(defectInfo, json_new_i("Power On Minutes", static_cast<uint32_t>(m_defect->powerOnMinutes)));

    //Nayana: Need not print the Scan status here as already printed in process_Scan_Status_Data
	//get_Scan_Status_Description(&myStr);
	//json_push_back(defectInfo, json_new_i((char*)myStr.c_str(), static_cast<uint32_t>(m_ScanParam->status)));

	get_Scan_Defect_Status_Description(&headerStr);
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", (uint8_t)M_GETBITRANGE(m_defect->status,7, 4));
	json_push_back(defectInfo, json_new_a((char*)headerStr.c_str(), (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", (uint8_t)M_GETBITRANGE(m_defect->status,3, 0));
	json_push_back(defectInfo, json_new_a("Sense Key", (char*)myStr.c_str()));


	//json_push_back(defectInfo, json_new_i("Additional Sense Code", static_cast<uint32_t>(m_defect->senseCode)));
	//json_push_back(defectInfo, json_new_i("Additional Sense Code Qualifier", static_cast<uint32_t>(m_defect->codeQualifier)));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_defect->senseCode);
    json_push_back(defectInfo, json_new_a("Additional Sense Code", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_defect->codeQualifier);
    json_push_back(defectInfo, json_new_a("Additional Sense Code Qualifier", (char*)myStr.c_str()));

	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_defect->vendorSpecific);
	json_push_back(defectInfo, json_new_a("Vendor Specific", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_defect->vendorSpecific1);
	json_push_back(defectInfo, json_new_a("Vendor Specific 1", (char*)myStr.c_str()));
	snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_defect->vendorSpecific2);
	json_push_back(defectInfo, json_new_a("Vendor Specific 2", (char*)myStr.c_str()));
	byte_Swap_64(&m_defect->LBA);
    snprintf((char*)myStr.c_str(), BASIC, "%" PRIu64"", m_defect->LBA);
    json_push_back(defectInfo, json_new_a("Logical Block Address", (char*)myStr.c_str()));
	
	json_push_back(defectData, defectInfo);
}

//-----------------------------------------------------------------------------
//! \fn process_other_param_data
//
//! \brief
//!   Description: parser out the data for other params
//
//  Entry:
//! \param eventData - Json node that parsed data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiScanLog::process_other_param_data(JSONNODE *scanData, size_t offset)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string headerStr = "";
    headerStr.resize(BASIC);
#if defined _DEBUG
    printf("Background Scan Defect Description \n");
#endif
    byte_Swap_16(&m_ParamHeader->paramCode);
    snprintf((char*)myStr.c_str(), BASIC, "Background Scan Defect Location 0x%04" PRIX16"", m_ParamHeader->paramCode);
    JSONNODE *defectInfo = json_new(JSON_NODE);
    json_set_name(defectInfo, (char*)myStr.c_str());

    snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_ParamHeader->paramCode);
    json_push_back(defectInfo, json_new_a("Background Scan Defect Parameter Code", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_ParamHeader->paramControlByte);
    json_push_back(defectInfo, json_new_a("Background Scan Defect Control Byte ", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_ParamHeader->paramLength);
    json_push_back(defectInfo, json_new_a("Background Scan Defect Length ", (char*)myStr.c_str()));
    JSONNODE *myArray = json_new(JSON_ARRAY);
    json_set_name(myArray, ("Background Data"));
    // add the size of the param header
    for (uint16_t loop = 4; loop < m_ParamHeader->paramLength; loop++)
    {
        json_push_back(myArray, json_new_i("Background data", pData[offset + loop]));
    }
    json_push_back(defectInfo, myArray);

    json_push_back(scanData, defectInfo);
}

//-----------------------------------------------------------------------------
//
//! \fn get_Scan_Data
//
//! \brief
//!   Description: parser out the data for the Background Scan Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiScanLog::get_Scan_Data(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	if (pData != NULL)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Background Scan Log - 15h");
        m_ScanParam = (sScanStatusParams *)&pData[0];
		process_Scan_Status_Data(pageInfo);
		for (size_t offset = ((size_t)m_ScanParam->paramLength + 4); offset < (((size_t)m_PageLength) - sizeof(sScanFindingsParams)); )
		{
			if (offset < m_bufferLength && offset < UINT16_MAX)
			{
                uint16_t paramCode = *(reinterpret_cast<uint16_t*>(&pData[offset]));
                byte_Swap_16(&paramCode);
                if (paramCode >= 0x0001 && paramCode <= 0x0800)
                {
                    m_defect = (sScanFindingsParams *)&pData[offset];
                    process_Defect_Data(pageInfo);
                    //offset += sizeof(sScanFindingsParams);
                    offset += m_defect->paramLength + 4;
                }
                else //if (paramCode >= 0x8000) //TODO: Nayana to check with Tim how to skip ssd part here
                {
                    m_ParamHeader = (sBackgroundScanParamHeader*)&pData[offset];                   
                    process_other_param_data(pageInfo,offset);
                    offset += m_ParamHeader->paramLength + 4;
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