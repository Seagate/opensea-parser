//
// CAta_Ext_DST_Log.cpp
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
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
    :m_name("Ext DST Log")                                         //!< name of the class
    , v_Buff()
    , m_logSize(0)
    , m_status(eReturnValues::IN_PROGRESS)
{
    CLog *cCLog;
    cCLog = new CLog(fileName,true);
    if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
    {
        cCLog->get_vBuffer(v_Buff);
        if (v_Buff.size() != 0)                           // if the buffer is null then exit something did not go right
        {
            m_logSize = static_cast<uint64_t>(v_Buff.size());

            sLogPageStruct *idCheck;
            idCheck = reinterpret_cast<sLogPageStruct*>(&v_Buff.at(0));
            byte_Swap_16(&idCheck->pageLength);
            if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
            {
                byte_Swap_16(&idCheck->pageLength);  // now that we know it's not scsi we need to flip the bytes back
                
            }
        }
        else
        {
            m_status = eReturnValues::FAILURE;
        }
    }
    else
    {
        m_status = eReturnValues::IN_PROGRESS;
        m_logSize = 0;
    }
    m_status = eReturnValues::SUCCESS;
    delete (cCLog);
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
CAta_Ext_DST_Log::CAta_Ext_DST_Log(const std::vector<uint8_t>& BufferData, size_t logSize, JSONNODE *masterData)
    :m_name("Ext DST Log")                                         //!< name of the class
    , v_Buff(BufferData)
    , m_logSize(logSize)
    , m_status(eReturnValues::IN_PROGRESS)
{
    if (logSize > v_Buff.size()) {
        m_status = eReturnValues::FAILURE;
        return;
    }
    if (!v_Buff.empty() && masterData != nullptr) {
        try {
            m_status = parse_Ext_Self_Test_Log(masterData);
        }
        catch (...) {
            m_status = eReturnValues::FAILURE;
        }
    }
    else {
        m_status = eReturnValues::FAILURE;
    }
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
//! \fn Get_Status_Meaning
//
//! \brief
//!   Description: fill in a string of the meaning of the status data
//
//  Entry:
//! \param meaning - string to fill in the meaing
//! \param status - the status to fill in the meaing 
//
//  Exit:
//!   \return eReturnValues eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
void CAta_Ext_DST_Log::Get_Status_Meaning(std::string &meaning, uint8_t status)
{
    if (status == 0x00)
    {
        meaning = "Self Test completed without error.";
    }
    else if (status == 0x01)
    {
        meaning = "Was Aborted by the host";
    }
    else if (status == 0x02)
    {
        meaning = "Was interrepted by the host with a hard reset of a soft reset";
    }
    else if (status == 0x03)
    {
        meaning = "unknown error and Self Test was unable to complete";
    }
    else if (status == 0x04)
    {
        meaning = "Completed and has failed and the element is unknown";
    }
    else if (status == 0x05)
    {
        meaning = "Completed With an electrical element failing";
    }
    else if (status == 0x06)
    {
        meaning = "Completed having a servo element failure";
    }
    else if (status == 0x07)
    {
        meaning = "Completed having a read element failure";
    }
    else if (status == 0x08)
    {
        meaning = "Completed having handling damage";
    }
    else if (status == 0x09)
    {
        meaning = "Completed having suspected handling damage";
    }
    else if (status == 0x15)
    {
        meaning = "Self Test is in progress";
    }
    else
    {
        meaning = "Reserved";
    }
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
//!   \return eReturnValues eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CAta_Ext_DST_Log::parse_Ext_Self_Test_Log(JSONNODE *masterData)
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
    uint16_t index = M_BytesTo2ByteValue(v_Buff.at(3), v_Buff.at(2));
	json_push_back(DstJson, json_new_i("Self Test Index", static_cast<uint32_t>(index)));
    DSTIndex += 4;
    for (int i = 1; i <= 19; i++)
    {
        StatusByte = v_Buff.at(DSTIndex + 1);
        timeStamp = M_BytesTo2ByteValue(v_Buff.at(DSTIndex + 3), v_Buff.at(DSTIndex + 2));
        compTime = M_BytesTo2ByteValue(v_Buff.at(DSTIndex + 13), v_Buff.at(DSTIndex + 12));
        checkPointByte = static_cast<int8_t>(v_Buff.at(DSTIndex + 4));
        LBA = M_BytesTo8ByteValue(0,0, v_Buff.at(DSTIndex + 10), v_Buff.at(DSTIndex + 9), v_Buff.at(DSTIndex + 8), v_Buff.at(DSTIndex + 7), v_Buff.at(DSTIndex + 6), v_Buff.at(DSTIndex + 5));
        JSONNODE *runInfo = json_new(JSON_NODE);

        std::ostringstream temp;
        temp << "Run " << std::dec << std::setw(3) << i;
        json_set_name(runInfo, temp.str().c_str());
        temp.str("");temp.clear();
        temp << std::dec << timeStamp;
        json_push_back(runInfo, json_new_a("Timestamp", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(StatusByte);
        json_push_back(runInfo, json_new_a("Status Byte", temp.str().c_str()));
        Get_Status_Meaning(myStr,StatusByte);
        json_push_back(runInfo, json_new_a("Status Meaning", myStr.c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(checkPointByte);
        json_push_back(runInfo, json_new_a("CheckPoint Byte", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << compTime;
        json_push_back(runInfo, json_new_a("Completion Time", temp.str().c_str()));
        set_json_64bit(runInfo, "LBA", LBA, false);

        DSTIndex += 26;
        json_push_back(DstJson, runInfo);
    }

    json_push_back(masterData, DstJson);

    return eReturnValues::SUCCESS;

}


