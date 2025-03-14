//
// CScsi_Background_Scan_Log.cpp  Definition of Background Scan Log Page
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
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
	: v_Data()
	, m_ScanName("Background Scan Log")
	, m_ScanStatus(eReturnValues::IN_PROGRESS)
	, m_PageLength(0)
	, m_bufferLength()
	, m_ScanParam()
	, m_defect()
    , m_ParamHeader()
{
	if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
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
	: v_Data(&buffer[0], &buffer[bufferSize])
	, m_ScanName("Background Scan Log")
	, m_ScanStatus(eReturnValues::IN_PROGRESS)
	, m_PageLength(pageLength)
	, m_bufferLength(bufferSize)
	, m_ScanParam()
	, m_defect()
    , m_ParamHeader()
{
	if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("%s \n", m_ScanName.c_str());
	}
	/*
    pData = new uint8_t[pageLength];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
    memcpy(pData, buffer, pageLength);
#else
    memcpy_s(pData, bufferSize, buffer, pageLength);// copy the buffer data to the class member pBuf
#endif
*/
	if (v_Data.size() != 0)
	{
		m_ScanStatus = eReturnValues::IN_PROGRESS;
	}
	else
	{
		m_ScanStatus = eReturnValues::FAILURE;
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
	/*
    if (pData != M_NULLPTR)
    {
        delete[] pData;
        pData = M_NULLPTR;
    }
	*/
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
			scan->assign("No background scans active");
			break;
		}
		case 0x01:
		{
            scan->assign("Background medium scan is active");
			break;
		}
		case 0x02:
		{
            scan->assign("Background prescan is active");
			break;
		}
		case 0x03:
		{
            scan->assign("Background medium scan halted due to fatal error");
			break;
		}
		case 0x04:
		{
            scan->assign("Background medium scan halted due to a vendor specific pattern of errors");
			break;
		}
		case 0x05:
		{
            scan->assign("Background medium scan halted due to medium formatted without Plist");
			break;
		}
		case 0x06:
		{
            scan->assign("Background medium scan halted - vendor specific cause");
			break;
		}
		case 0x07:
		{
            scan->assign("Background medium scan halted due to temperature out of allowed range");
			break;
		}
		case 0x08:
		{
            scan->assign("Background medium scan halted, waiting for Background Medium Interval timer expiration");
			break;
		}
		default:
		{
            scan->assign("Reserved");
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
	std::string myStr;
#if defined _DEBUG
	printf("Background Scan Status Description \n");
#endif
	byte_Swap_16(&m_ScanParam->paramCode);
	JSONNODE *statusInfo = json_new(JSON_NODE);
	json_set_name(statusInfo, "Background Scan Status");

    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_ScanParam->paramCode;
	json_push_back(statusInfo, json_new_a("Background Scan Status Parameter Code", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_ScanParam->paramControlByte);
	json_push_back(statusInfo, json_new_a("Background Scan Status Control Byte ", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_ScanParam->paramLength);
	json_push_back(statusInfo, json_new_a("Background Scan Status Length ", temp.str().c_str()));

	byte_Swap_32(&m_ScanParam->timeStamp);
	json_push_back(statusInfo, json_new_i("SMART Time Stamp", static_cast<uint32_t>(m_ScanParam->timeStamp)));
	get_Scan_Status_Description(&myStr);
	json_push_back(statusInfo, json_new_i(myStr.c_str(), static_cast<uint32_t>(m_ScanParam->status)));
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
            defect->assign("The LBA has not yet been reassigned");
			break;
		}
		case 0x02:
		{
            defect->assign("Performed Automatic Read Reassignment for the LBA");
			break;
		}
		case 0x03:
		{
            defect->assign("Reserved");
			break;
		}
		case 0x04:
		{
            defect->assign("Attempt to Perform Automatic Read Reassignment Failed");
			break;
		}
		case 0x05:
		{
            defect->assign("Error Corrected, Device rewriting the LBA without Performing Reassign Operation.");
			break;
		}
		case 0x06:
		{
            defect->assign("Application Client caused automatic write reassignment for the LBA or LBPRZ");
			break;
		}
		case 0x07:
		{
            defect->assign("Application Client,  Reassign by Command - Invalid Data");
			break;
		}
		case 0x08:
		{
            defect->assign("Application Client, Reassign Command Failed");
			break;
		}
		default:
		{
            defect->assign("Reserved");
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
	std::string headerStr = "";
    std::ostringstream temp;
#if defined _DEBUG
	printf("Background Scan Defect Description \n");
#endif
	byte_Swap_16(&m_defect->paramCode);
    temp << "Background Scan Defect Location 0x" << std::hex << m_defect->paramCode;
	JSONNODE *defectInfo = json_new(JSON_NODE);
	json_set_name(defectInfo, temp.str().c_str());

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_defect->paramCode;
	json_push_back(defectInfo, json_new_a("Background Scan Defect Parameter Code", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_defect->paramControlByte);
	json_push_back(defectInfo, json_new_a("Background Scan Defect Control Byte ", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_defect->paramLength);
	json_push_back(defectInfo, json_new_a("Background Scan Defect Length ", temp.str().c_str()));

	byte_Swap_32(&m_defect->powerOnMinutes);
	json_push_back(defectInfo, json_new_i("Power On Minutes", static_cast<uint32_t>(m_defect->powerOnMinutes)));

	get_Scan_Defect_Status_Description(&headerStr);
    temp.str("");temp.clear();
    
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(M_GETBITRANGE(m_defect->status,7, 4));
	json_push_back(defectInfo, json_new_a(headerStr.c_str(), temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(M_GETBITRANGE(m_defect->status,3, 0));
	json_push_back(defectInfo, json_new_a("Sense Key", temp.str().c_str()));

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_defect->senseCode);
    json_push_back(defectInfo, json_new_a("Additional Sense Code", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_defect->codeQualifier);
    json_push_back(defectInfo, json_new_a("Additional Sense Code Qualifier", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_defect->vendorSpecific;
	json_push_back(defectInfo, json_new_a("Vendor Specific", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_defect->vendorSpecific1;
	json_push_back(defectInfo, json_new_a("Vendor Specific 1", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_defect->vendorSpecific2);
	json_push_back(defectInfo, json_new_a("Vendor Specific 2", temp.str().c_str()));
	byte_Swap_64(&m_defect->LBA);
    temp.str("");temp.clear();
    temp << "0x" << std::dec <<  m_defect->LBA;
    json_push_back(defectInfo, json_new_a("Logical Block Address", temp.str().c_str()));
	
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
#if defined _DEBUG
    printf("Background Scan Defect Description \n");
#endif
    byte_Swap_16(&m_ParamHeader->paramCode);
    std::ostringstream temp;
    temp << "Background Scan Defect Location 0x" << std::hex << std::uppercase << std::setfill('0') << m_ParamHeader->paramCode;
    JSONNODE *defectInfo = json_new(JSON_NODE);
    json_set_name(defectInfo, temp.str().c_str());
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_ParamHeader->paramCode;
    json_push_back(defectInfo, json_new_a("Background Scan Defect Parameter Code", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_ParamHeader->paramControlByte);
    json_push_back(defectInfo, json_new_a("Background Scan Defect Control Byte ", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_ParamHeader->paramLength);
    json_push_back(defectInfo, json_new_a("Background Scan Defect Length ", temp.str().c_str()));
    JSONNODE *myArray = json_new(JSON_ARRAY);
    json_set_name(myArray, ("Background Data"));
    // add the size of the param header
    for (uint16_t loop = 4; loop < m_ParamHeader->paramLength; loop++)
    {
        json_push_back(myArray, json_new_i("Background data", v_Data.at(offset + loop)));
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
	eReturnValues retStatus = eReturnValues::IN_PROGRESS;
	if (v_Data.size() != 0)
	{
		JSONNODE *pageInfo = json_new(JSON_NODE);
		json_set_name(pageInfo, "Background Scan Log - 15h");
        m_ScanParam = reinterpret_cast<sScanStatusParams*>(&v_Data.at(0));
		process_Scan_Status_Data(pageInfo);
        if ((m_ScanParam->paramLength + 4) < m_PageLength)
        {
            for (size_t offset = (static_cast<size_t>(m_ScanParam->paramLength) + PARAMSIZE); offset < ((static_cast<size_t>(m_PageLength)) - sizeof(sScanFindingsParams)); )
            {
                if (offset < m_bufferLength && offset < UINT16_MAX)
                {
                    uint16_t paramCode = *(reinterpret_cast<uint16_t*>(&v_Data.at(offset)));
                    byte_Swap_16(&paramCode);
                    if (paramCode >= 0x0001 && paramCode <= 0x0800)
                    {
                        m_defect = reinterpret_cast<sScanFindingsParams*>(&v_Data.at(offset));
                        process_Defect_Data(pageInfo);
                        offset += static_cast<size_t>(m_defect->paramLength) + PARAMSIZE;
                    }
                    else 
                    {
                        m_ParamHeader = reinterpret_cast<sBackgroundScanParamHeader*>(&v_Data.at(offset));
                        process_other_param_data(pageInfo, offset);
                        offset += static_cast<size_t>(m_ParamHeader->paramLength) + PARAMSIZE;
                    }
                }
                else
                {
                    json_push_back(masterData, pageInfo);
                    return eReturnValues::BAD_PARAMETER;
                }

            }
        }

		json_push_back(masterData, pageInfo);
		retStatus = eReturnValues::SUCCESS;
	}
	else
	{
		retStatus = eReturnValues::MEMORY_FAILURE;
	}
	return retStatus;
}
