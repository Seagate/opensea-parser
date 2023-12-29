//
// CAta_NCQ_Command_Error_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CAta_NCQ_Command_Error_Log.h"


using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CAta_NCQ_Command_Error_Log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_NCQ_Command_Error_Log::CAta_NCQ_Command_Error_Log()
    : m_name("ATA NCQ Command Error Log")
    , m_status(IN_PROGRESS)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CAta_NCQ_Command_Error_Log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_NCQ_Command_Error_Log::CAta_NCQ_Command_Error_Log(const std::string & fileName)
    : m_name("ATA NCQ Command Error Log")
    , m_status(IN_PROGRESS)
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			size_t logSize = cCLog->get_Size();
			uint8_t *pBuf = new uint8_t[logSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
			memcpy(pBuf, cCLog->get_Buffer(), logSize);
#else
			memcpy_s(pBuf, logSize, cCLog->get_Buffer(), logSize);// copy the buffer data to the class member pBuf
#endif
			sLogPageStruct *idCheck;
			idCheck = reinterpret_cast<sLogPageStruct*>(&pBuf[0]);
			byte_Swap_16(&idCheck->pageLength);
			if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
			{
				byte_Swap_16(&idCheck->pageLength);  // now that we know it's not scsi we need to flip the bytes back
                for (size_t offset = 0; offset <= logSize;)
                {
                    sNCQError* pNCQError = new sNCQError;
                    pNCQError = reinterpret_cast<sNCQError*>(&pBuf[offset]);
                    vNCQFrame.push_back(*pNCQError);
                    offset += sizeof(sNCQError);
                    delete pNCQError;

                }
				m_status = IN_PROGRESS;
			}
			else
			{
				m_status = BAD_PARAMETER;
			}
            delete [] pBuf;
		}
		else
		{

			m_status = FAILURE;
		}
	}
	else
	{
		m_status = cCLog->get_Log_Status();
	}
	delete (cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_NCQ_Command_Error_Log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_NCQ_Command_Error_Log::CAta_NCQ_Command_Error_Log(uint8_t *buffer, size_t length)
    : m_name("ATA NCQ Command Error Log")
    , m_status(IN_PROGRESS)
{	
    
    if (buffer != NULL)
    {
        sNCQError* pNCQError = new sNCQError;
        for (size_t offset = 0; (offset + sizeof(sNCQError)) <= length;)
        {
            pNCQError = reinterpret_cast<sNCQError*>(&buffer[offset]);
            vNCQFrame.push_back(*pNCQError);
            offset += sizeof(sNCQError);
            pNCQError = NULL;
        }
        delete pNCQError;
        m_status = IN_PROGRESS;
    }
    else
    {
        m_status = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn ~CAta_NCQ_Command_Error_Log()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_NCQ_Command_Error_Log::~CAta_NCQ_Command_Error_Log()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Bit_Name_Info
//
//! \brief
//!   Description: parse the bit name and info 
//
//  Entry:
//! \param NCQInfo - json node to the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_NCQ_Command_Error_Log::get_Bit_Name_Info(JSONNODE *NCQInfo , sNCQError *ncqError)
{
    bool validNCQ = true;
    bool idleCmd = false;
    uint8_t ncqTag = M_GETBITRANGE(ncqError->NCQbit, 4, 0);
    if (ncqError->NCQbit & BIT7)  // if bit7 is set then it is NOT valid ncq command
    {
        validNCQ = false;
        
    }
    opensea_parser::set_Json_Bool(NCQInfo, "Valid NCQ tag Command", validNCQ);

    if (ncqError->NCQbit & BIT6)
    {
        idleCmd = true;
    }
    opensea_parser::set_Json_Bool(NCQInfo, "Idle Immediate Command Received for unload", idleCmd);

    json_push_back(NCQInfo, json_new_i("NCQ Tag", static_cast<uint32_t>(ncqTag)));

    return validNCQ;
}

//-----------------------------------------------------------------------------
//
//! \fn create_LBA
//
//! \brief
//!   Description: creates the lba 
//
//  Entry:
//
//  Exit:
//!   \return uint64_t
//
//---------------------------------------------------------------------------
uint64_t CAta_NCQ_Command_Error_Log::create_LBA(sNCQError* ncqError)
{
    uint64_t lba = 0;
    lba = M_BytesTo8ByteValue(0, 0, ncqError->lba6, ncqError->lba5, ncqError->lba4, ncqError->lba3, ncqError->lba2, ncqError->lba1);
    return lba;
}
//-----------------------------------------------------------------------------
//
//! \fn get_NCQ_Command_Error_Log
//
//! \brief
//!   Description: parse out the NCQ command error data from the log
//
//  Entry:
//! \param masterData - json node to the all the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
eReturnValues CAta_NCQ_Command_Error_Log::get_NCQ_Command_Error_Log(JSONNODE *masterData)
{
    JSONNODE *NCQInfo = json_new(JSON_ARRAY);
    json_set_name(NCQInfo, "NCQ Command Error Log");
    
    if (vNCQFrame.size() != 0)
    {
        for (uint32_t index = 0; index < vNCQFrame.size(); ++index)
        {
            JSONNODE* tier1Info = json_new(JSON_NODE);
            json_set_name(tier1Info, "Error Log");
            get_Bit_Name_Info(tier1Info, &vNCQFrame.at(index));
            json_push_back(tier1Info, json_new_i("Status", static_cast<uint32_t>(vNCQFrame.at(index).status)));
            json_push_back(tier1Info, json_new_i("Error", static_cast<uint32_t>(vNCQFrame.at(index).error)));
            json_push_back(tier1Info, json_new_i("Device", static_cast<uint32_t>(vNCQFrame.at(index).device)));
            json_push_back(tier1Info, json_new_i("Count", static_cast<uint32_t>(vNCQFrame.at(index).count)));
            opensea_parser::set_json_64bit(tier1Info, "LBA", create_LBA(&vNCQFrame.at(index)), false);
            json_push_back(tier1Info, json_new_i("Sense Key", static_cast<uint32_t>(vNCQFrame.at(index).senseKey)));
            json_push_back(tier1Info, json_new_i("Sense Code Field", static_cast<uint32_t>(vNCQFrame.at(index).senseCodeField)));
            json_push_back(tier1Info, json_new_i("Sense code Qualifeir", static_cast<uint32_t>(vNCQFrame.at(index).senseCodeQualifier)));
            opensea_parser::set_json_64bit(tier1Info, "Final LBA in Error", vNCQFrame.at(index).finalLBA, false);
            json_push_back(NCQInfo, tier1Info);
        }
    }
    else
    {
        json_push_back(NCQInfo, json_new_a("Empty NCQ Command Error Log", "data has not yet been gathered"));
    }
    json_push_back(masterData, NCQInfo);
    m_status = SUCCESS;
    return m_status;
}