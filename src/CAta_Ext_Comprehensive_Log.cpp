// CAta_Ext_Comprehensive_Log.cpp
//
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// CAta_Ext_Comprehensive_Log.cpp

#include "CAta_Ext_Comprehensive_Log.h"


using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CExtComp::CExtComp()
//
//! \brief
//!   Description: Class constructor
//
//
//---------------------------------------------------------------------------
CExtComp::CExtComp()
    :CLog()
    , m_name("Ext Comp Log")
{

}
//-----------------------------------------------------------------------------
//
//! \fn CExtComp::CExtComp()
//
//! \brief
//!   Description: Class constructor
//
//
//---------------------------------------------------------------------------
CExtComp::CExtComp(const std::string &fileName, JSONNODE *masterData)
    :CLog(fileName)
    , m_name("Ext Comp Log")
    , m_status(IN_PROGRESS)
{
    if (CLog::get_Log_Status() == SUCCESS)
    {
        if (m_bufferData != NULL)                           // if the buffer is null then exit something did go right
        {
            m_status = IN_PROGRESS;
            m_status = ParseExtCompLog(CLog::get_Buffer(), masterData);
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
//! \fn CExtComp::~CExtComp()
//
//! \brief
//!   Description: Class deconstructor 
//
//
//---------------------------------------------------------------------------
CExtComp::~CExtComp()
{
}
//-----------------------------------------------------------------------------
//
//! \fn Ext_Comp::ParseExtCompLog()
//
//! \brief
//!   Description: parse the extented Comp Log and set the pareto based on # of ecc errors
//
//  Entry:
//! \param pData  = pointer to the buffer data
//! \param mansterData = Json Node that holds all of the data 
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CExtComp::ParseExtCompLog(uint8_t *pData, JSONNODE *masterData)
{
    std::string myStr = "Parse Ext Comp Log";
    uint8_t  deviceControl = 0;
    uint16_t featureField = 0;
    uint16_t countField = 0;
    uint16_t lowLBA = 0;
    uint16_t midLBA = 0;
    uint16_t hiLBA = 0;
    uint8_t  deviceHead = 0;
    uint8_t  commandField = 0;
    uint8_t  reserved = 0;
    uint32_t timeStamp = 0;
    uint8_t  errorField = 0;
    uint8_t  status = 0;
    uint16_t lifeTime = 0;
    uint16_t COMPIndex = 0;
    uint32_t wOffset = 4; //Points to error data structure
    uint8_t State = 0;
    uint64_t LBA = 0;
    uint16_t deviceErrorCount = 0;
    std::string stateStr = "Vendor Specific      ";

    JSONNODE *EComp = json_new(JSON_NODE);
    json_set_name(EComp, "Ext Comp SMART log");

    json_push_back(EComp, json_new_i("Ext Comp SMART log Version", static_cast<uint32_t>(pData[0])));

    COMPIndex = pData[2];
    json_push_back(EComp, json_new_i("Ext Comp Index", static_cast<uint32_t>(COMPIndex)));


    for (int z = 0; z < 4; z++)
    {
        for (int cmddata = 1; cmddata < 6; cmddata++)
        {
            deviceControl = pData[wOffset];
            featureField = pData[wOffset + 1];
            countField = pData[wOffset + 3];
            lowLBA = pData[wOffset + 5];
            midLBA = pData[wOffset + 7];
            hiLBA = pData[wOffset + 9];
            deviceHead = pData[wOffset + 11];
            commandField = pData[wOffset + 12];
            reserved = pData[wOffset + 13];
            timeStamp = pData[wOffset + 14];
            word_Swap_32(&timeStamp);
            wOffset += 18;
#if defined(_DEBUG)
            printf("%x %x %x %x %x %x %x %x %d \n", \
                deviceControl, \
                featureField, \
                countField, \
                lowLBA, \
                midLBA, \
                hiLBA, \
                deviceHead, \
                commandField, \
                timeStamp);
#endif
        }
        //printf("offset = %d \n",wOffset);
        reserved = pData[wOffset];
        errorField = pData[wOffset + 1];
        countField = pData[wOffset + 3];
        lowLBA = pData[wOffset + 5];
        midLBA = pData[wOffset + 7];
        hiLBA = pData[wOffset + 9];
        deviceHead = pData[wOffset + 10];
        status = pData[wOffset + 11];
        State = pData[wOffset + 30];
        lifeTime = pData[wOffset + 32];
        LBA = ((uint64_t)(((uint64_t)hiLBA << 32) +
            ((uint64_t)midLBA << 16) +
            (lowLBA)));
        byte_Swap_16(&lifeTime);
        wOffset += 34;
#if defined (_DEBUG)
        printf("%x %x %x %x %x %x %x %x %x %d \n", \
            deviceControl, \
            errorField, \
            countField, \
            lowLBA, \
            midLBA, \
            hiLBA, \
            deviceHead, \
            status, \
            State, \
            lifeTime);
#endif

    }
    deviceErrorCount = pData[500];
    if (State == 0x0)
    {
        stateStr =  "Unknown:";
    }
    else if (State == 0x1)
    {
        stateStr = "Sleep:";
    }
    else if (State == 0x2)
    {
        stateStr =  "Standby:";
    }
    else if (State == 0x3)
    {
        stateStr =  "Active/Idle:";
    }
    else if (State >= 0x4)
    {
        snprintf((char*)myStr.c_str(), BASIC, "%x", State);
        json_push_back(EComp, json_new_a("Vendor Specific", (char*)myStr.c_str()));
    }
    else
    {
        stateStr = "    null:            ";
    }
    json_push_back(EComp, json_new_a("Ext Comp Error Log State", (char*)stateStr.c_str()));

    snprintf((char*)myStr.c_str(), BASIC, "%" PRId16"", lifeTime);
    json_push_back(EComp, json_new_a("Ext Comp Error Log Life Timestamp", (char*)myStr.c_str()));

    JSONNODE *opcode = json_new(JSON_NODE);
    json_set_name(opcode, "Opcode Content");

    snprintf((char*)myStr.c_str(), BASIC, "0x%x", errorField);
    json_push_back(opcode, json_new_a("Error", (char*)myStr.c_str()));

    snprintf((char*)myStr.c_str(), BASIC, "0x%" PRIx16"", countField);
    json_push_back(opcode, json_new_a("Count", (char*)myStr.c_str()));

    opensea_parser::set_json_64bit(opcode, "LBA", LBA , false);

    json_push_back(opcode, json_new_i("Device", static_cast<uint32_t>(deviceControl)));

    snprintf((char*)myStr.c_str(), BASIC, "0x%x", status);
    json_push_back(opcode, json_new_a("Status", (char*)myStr.c_str()));

    json_push_back(opcode, json_new_i("Comp Error count", static_cast<uint32_t>(deviceErrorCount)));

    json_push_back(masterData, opcode);

    if (status == 0x51 && errorField == 0x40)
    {
        return FAILURE;
    }
    return SUCCESS;
}


