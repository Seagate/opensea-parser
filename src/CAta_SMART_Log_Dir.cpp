//
// CAta_SMART_Log_Dir.cpp
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CAta_SMART_Log_Dir.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn   CAta_SMART_Log_Dir()
//
//! \brief
//!   Description:  Default Class constructor for the CSeaTreasureLog
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_SMART_Log_Dir::CAta_SMART_Log_Dir()
    : m_name("ATA SMART Log Directory")
    , pData(NULL)
    , m_logSize(0)
    , m_status(IN_PROGRESS)
    , m_freeBuffer(false)
    , m_hasHostSpecific(false)
    , m_hasVendorSpecific(false)
{
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_SMART_Log_Dir()
//
//! \brief
//!   Description:  Default Class constructor for the CSeaTreasureLog
//
//  Entry:
//! \parama fileName = the name of the file that we need to open to get the buffer data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_SMART_Log_Dir::CAta_SMART_Log_Dir(const std::string &fileName)
    : m_name("ATA SMART Log Directory")
    , pData(NULL)
    , m_logSize(0)
    , m_status(IN_PROGRESS)
    , m_freeBuffer(false)
    , m_hasHostSpecific(false)
    , m_hasVendorSpecific(false)
{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == SUCCESS)
    {
        if (cCLog->get_Buffer() != NULL)
        {
            size_t bufferSize = cCLog->get_Size();
            pData = new uint8_t[cCLog->get_Size()];								// new a buffer to the point				
#ifndef _WIN64
            memcpy(pData, cCLog->get_Buffer(), bufferSize);
#else
            memcpy_s(pData, bufferSize, cCLog->get_Buffer(), bufferSize);// copy the buffer data to the class member pBuf
#endif
            m_logSize = bufferSize;
            m_status = parse_SMART_Log_Dir();
            m_freeBuffer = true;
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
    delete(cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_SMART_Log_Dir()
//
//! \brief
//!   Description:  Default Class constructor for the CSeaTreasureLog
//
//  Entry:
//! \parama BufferData = pointer, length and allocation structure to the data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_SMART_Log_Dir::CAta_SMART_Log_Dir(uint8_t *bufferData, size_t logSize)
    : m_name("ATA SMART Log Directory")
    , pData(bufferData)
    , m_logSize(logSize)
    , m_status(IN_PROGRESS)
    , m_freeBuffer(false)
    , m_hasHostSpecific(false)
    , m_hasVendorSpecific(false)
{
    if (bufferData != NULL)
    {
        m_status = parse_SMART_Log_Dir();
    }
    else
    {
        m_status = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_SMART_Log_Dir::~CAta_SMART_Log_Dir()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param pData  pointer to the buffer data
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_SMART_Log_Dir::~CAta_SMART_Log_Dir()
{
    if (!m_logDetailList.empty())
    {
        m_logDetailList.clear();
    }

    if (pData != NULL && m_freeBuffer)
    {
        delete[] pData;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn parse_SMART_Log_Dir
//
//! \brief
//!   Description: parse the log
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_SMART_Log_Dir::parse_SMART_Log_Dir()
{
    eReturnValues ret = SUCCESS;

    for (uint16_t offset = 2; offset < m_logSize; offset += 2)
    {
        uint16_t logSize = M_BytesTo2ByteValue(pData[offset + 1], pData[offset]);
        if (logSize != 0)
        {
            sLogDetailStructure logDetails;
            logDetails.numberOfPages = logSize;
            logDetails.logAddress = offset / 2;

            if (!m_hasHostSpecific && is_Host_Specific_Log(logDetails.logAddress))
            {
                m_hasHostSpecific = true;
            }
            else if (!m_hasVendorSpecific && is_Vendor_Specific_Log(logDetails.logAddress))
            {
                m_hasVendorSpecific = true;
            }

            m_logDetailList.push_back(logDetails);
        }
    }

    return ret;
}
bool CAta_SMART_Log_Dir::is_Host_Specific_Log(uint8_t logAddress)
{
    if (logAddress >= 0x80 && logAddress < 0xA0)
        return true;
    else
        return false;
}
bool CAta_SMART_Log_Dir::is_Vendor_Specific_Log(uint8_t logAddress)
{
    if (logAddress >= 0xA0 && logAddress < 0xE0)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------
//
//! \fn print_SMART_Log_Dir
//
//! \brief
//!   Description: print out the SMART Directory Log
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
eReturnValues CAta_SMART_Log_Dir::print_SMART_Log_Dir(JSONNODE *masterData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string printStr;
    printStr.resize(BASIC);

    JSONNODE *dirInfo = json_new(JSON_NODE);
    json_set_name(dirInfo, "SMART Log Directory");

    JSONNODE *hostInfo = NULL;
    JSONNODE *vendorInfo = NULL;

    if (m_hasHostSpecific)
    {
        hostInfo = json_new(JSON_NODE);
        json_set_name(hostInfo, "Host Specific Log");
    }

    if (m_hasVendorSpecific)
    {
        vendorInfo = json_new(JSON_NODE);
        json_set_name(vendorInfo, "Vendor Specific Log");
    }

    std::vector<sLogDetailStructure>::iterator logItr = m_logDetailList.begin();
    for (; logItr != m_logDetailList.end(); logItr++)
    {
        sLogDetailStructure logDetail = *logItr;
        snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", logDetail.logAddress);
        snprintf((char*)printStr.c_str(), BASIC, "%" PRIu16"", logDetail.numberOfPages);

        if (is_Host_Specific_Log(logDetail.logAddress) && hostInfo != NULL)
            json_push_back(hostInfo, json_new_a((char*)myStr.c_str(), (char*)printStr.c_str()));
        else if (is_Vendor_Specific_Log(logDetail.logAddress) && vendorInfo != NULL)
            json_push_back(vendorInfo, json_new_a((char*)myStr.c_str(), (char*)printStr.c_str()));
        else
            json_push_back(dirInfo, json_new_a((char*)myStr.c_str(), (char*)printStr.c_str()));
    }

    if (hostInfo != NULL)
        json_push_back(dirInfo, hostInfo);
    if (vendorInfo != NULL)
        json_push_back(dirInfo, vendorInfo);

    json_push_back(masterData, dirInfo);

    return SUCCESS;
}