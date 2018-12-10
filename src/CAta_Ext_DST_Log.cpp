//
// CAta_Ext_DST_Log.cpp
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CAta_Ext_DST_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Ext_DST_Log::CAta_Ext_DST_Log(
//
//! \brief
//!   Description:  constructor of the Ext DST Log Class
//
//  Entry:
//
//  Exit:DST_Enum
//
//---------------------------------------------------------------------------
CAta_Ext_DST_Log::CAta_Ext_DST_Log(const std::string &fileName, JSONNODE *masterData)
    :CLog(fileName)
    , m_name("Ext DST Log")                                         //!< name of the class
    , m_jData(masterData)
    , m_status(IN_PROGRESS)
{
    m_logSize = 0;
    if (CLog::get_Log_Status() == SUCCESS)
    {
        uint8_t *DSTData= CLog::get_Buffer();
        if (m_bufferData != NULL && DSTData != NULL)                           // if the buffer is null then exit something did go right
        {
            m_status = IN_PROGRESS;
            m_logSize = CLog::get_Size();
            m_status = parse_Ext_Self_Test_Log(DSTData);
        }
    }
    else
    {
        m_status = IN_PROGRESS;
        m_logSize = 0;
    }
    
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Ext_DST_Log::CAta_Ext_DST_Log(
//
//! \brief
//!   Description:  constructor of the Ext DST Log Class
//
//  Entry:
//
//  Exit:DST_Enum
//
//---------------------------------------------------------------------------
CAta_Ext_DST_Log::CAta_Ext_DST_Log(uint8_t *pBufferData, JSONNODE *masterData)
    :CLog()
    , m_name("Ext DST Log")                                         //!< name of the class
    , m_jData(masterData)
    , m_status(IN_PROGRESS)
{
    m_logSize = 0;
    m_status = parse_Ext_Self_Test_Log(pBufferData);
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Ext_DST_Log::~CAta_Ext_DST_Log(
//
//! \brief
//!   Description:  Deconstructor of the Ext DST Log Class
//
//  Entry:
//
//  Exit:DST_Enum
//
//---------------------------------------------------------------------------
CAta_Ext_DST_Log::~CAta_Ext_DST_Log()
{

}
//-----------------------------------------------------------------------------
//
//! \fn CAta_Ext_DST_Log::parse_Ext_Self_Test_Log()
//
//! \brief
//!   Description: parse the ext self test dst log number 7
//
//  Entry:
//! \param pData  pointer to the buffer data
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CAta_Ext_DST_Log::parse_Ext_Self_Test_Log(uint8_t *DSTData)
{
    std::string myStr = "Start of Ext DST";
    JSONNODE *DstJson = json_new(JSON_NODE);
    uint16_t DSTIndex = 0;
    uint8_t StatusByte = 0;
    uint16_t timeStamp = 0;
    uint16_t compTime = 0;
    int8_t checkPointByte = 0;
    uint64_t LBA = 0;

    json_set_name(DstJson, "DST Log");
	uint16_t index = ((uint16_t)DSTData[3] << 8) | ((uint16_t)DSTData[2] << 0);
	json_push_back(DstJson, json_new_i("Self Test Index", static_cast<uint32_t>(index)));
    DSTIndex += 4;
    for (int i = 1; i <= 19; i++)
    {

        
        StatusByte = DSTData[DSTIndex + 1];
        timeStamp = ((uint16_t)DSTData[DSTIndex + 3] << 8) | ((uint16_t)DSTData[DSTIndex + 2] << 0);
        compTime = ((uint16_t)DSTData[DSTIndex + 13] << 8) | ((uint16_t)DSTData[DSTIndex + 12] << 0);
        checkPointByte = DSTData[DSTIndex + 4];
        LBA = ((uint64_t)DSTData[DSTIndex + 10] << 40) |
            ((uint64_t)DSTData[DSTIndex + 9] << 32) |
            ((uint64_t)DSTData[DSTIndex + 8] << 24) |
            ((uint64_t)DSTData[DSTIndex + 7] << 16) |
            ((uint64_t)DSTData[DSTIndex + 6] << 8) |
            ((uint64_t)DSTData[DSTIndex + 5] << 0);
        
        JSONNODE *runInfo = json_new(JSON_NODE);
        snprintf((char*)myStr.c_str(), BASIC, "Run %3d ", i);
        json_set_name(runInfo, (char*)myStr.c_str());
        snprintf((char*)myStr.c_str(), BASIC, "%u", timeStamp);
        json_push_back(runInfo, json_new_a("Timestamp", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "0x%02x", int(StatusByte));
        json_push_back(runInfo, json_new_a("Status Byte", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "0x%02x", checkPointByte);
        json_push_back(runInfo, json_new_a("CheckPoint Byte", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "%u", compTime);
        json_push_back(runInfo, json_new_a("Completion Time", (char*)myStr.c_str()));
        set_json_64bit(runInfo, "LBA", LBA, false);

        DSTIndex += 26;
        json_push_back(DstJson, runInfo);
    }

    json_push_back(m_jData, DstJson);

    return SUCCESS;
}


