//
// CAta_NCQ_Command_Error_Log.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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
    :CLog()
    , m_name("ATA NCQ Command Error Log")
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
    :CLog(fileName)
    , m_name("ATA NCQ Command Error Log")
    , m_status(IN_PROGRESS)
{
    if (CLog::get_Log_Status() == SUCCESS)
    {
        pBuf = CLog::get_Buffer();
        if (pBuf != NULL)
        {

            m_status = IN_PROGRESS;
        }
        else
        {
            m_status = FAILURE;
        }
    }
    else
    {
        m_status = CLog::get_Log_Status();
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
CAta_NCQ_Command_Error_Log::CAta_NCQ_Command_Error_Log(uint8_t *buffer)
    :CLog()
    , m_name("ATA NCQ Command Error Log")
    , m_status(IN_PROGRESS)
{
    pBuf = buffer;
    if (pBuf != NULL)
    {

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
bool CAta_NCQ_Command_Error_Log::get_Bit_Name_Info(JSONNODE *NCQInfo)
{
    std::string myStr = " ";
    myStr.resize(BASIC);
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

bool CAta_NCQ_Command_Error_Log::create_LBA()
{
    m_LBA = ((uint64_t)ncqError->lba6 << 40) | ((uint64_t)ncqError->lba5 << 32) | ((uint64_t)ncqError->lba4 << 24) | ((uint64_t)ncqError->lba3 << 16) | ((uint64_t)ncqError->lba2 << 8) | ((uint64_t)ncqError->lba1);
    return true;
}
eReturnValues CAta_NCQ_Command_Error_Log::get_NCQ_Command_Error_Log(JSONNODE *masterData)
{
    JSONNODE *NCQInfo = json_new(JSON_NODE);
    json_set_name(NCQInfo, "NCQ Command Error Log");
    ncqError = (sNCQError *)&pBuf[0];
    get_Bit_Name_Info(NCQInfo);
    create_LBA();
    json_push_back(NCQInfo, json_new_i("Status", static_cast<uint32_t>(ncqError->status)));
    json_push_back(NCQInfo, json_new_i("Error", static_cast<uint32_t>(ncqError->error)));
    json_push_back(NCQInfo, json_new_i("Device", static_cast<uint32_t>(ncqError->device)));
    json_push_back(NCQInfo, json_new_i("Count", static_cast<uint32_t>(ncqError->count)));
    opensea_parser::set_json_64bit(NCQInfo, "LBA", m_LBA, false);
    json_push_back(NCQInfo, json_new_i("Sense Key", static_cast<uint32_t>(ncqError->senseKey)));
    json_push_back(NCQInfo, json_new_i("Sense Code Field", static_cast<uint32_t>(ncqError->senseCodeField)));
    json_push_back(NCQInfo, json_new_i("Sense code Qualifeir", static_cast<uint32_t>(ncqError->senseCodeQualifier)));
    opensea_parser::set_json_64bit(NCQInfo, "Final LBA in Error", ncqError->finalLBA, false);

    json_push_back(masterData, NCQInfo);
    m_status = SUCCESS;
    return m_status;
}