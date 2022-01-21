//
// CScsi_Page_19h_Cache_Memory_Statistics_Log.cpp  Definition of SCSI cache memory statistics log
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Page_19h_Cache_Memory_Statistics_Log.cpp  Definition of SCSI cache memory Statistics Page
//
#include "CScsi_Page_19h_Cache_Memory_Statistics_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiCacheMemStatLog()
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
CScsiCacheMemStatLog::CScsiCacheMemStatLog()
    : pData()
    , m_CacheMemName("Cache Memory Statistics Log")
    , m_LogStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength(0)
    , m_CacheMemLog()
    , m_TimeIntervalDescriptorParam(NULL)
    , m_Value(0)
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_CacheMemName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiCacheMemStatLog()
//
//! \brief
//!   Description: Class constructor for VOLTAGE MONITORING LOG
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiCacheMemStatLog::CScsiCacheMemStatLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_CacheMemName("Cache Memory Statistics Log")
    , m_LogStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_CacheMemLog()
    , m_TimeIntervalDescriptorParam(NULL)
    , m_Value(0)
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_CacheMemName.c_str());
    }
    if (buffer != NULL)
    {
        m_LogStatus = IN_PROGRESS;
    }
    else
    {
        m_LogStatus = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiCacheMemStatLog
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
CScsiCacheMemStatLog::~CScsiCacheMemStatLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for Cache Memory Parameter Information
//
//  Entry:
//! \param description - string to give the Cache Memory Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiCacheMemStatLog::get_Parameter_Code_Description(uint16_t paramCode, std::string *cacheStatistics)
{
    std::ostringstream temp;
    switch (paramCode)
    {
    case 0x0001:
            *cacheStatistics = "read cache memory hits";
        break;
    case 0x0002:
            *cacheStatistics = "reads to cache memory";
        break;
    case 0x0003:
            *cacheStatistics = "writes cache memory hits";
        break;
    case 0x0004:
            *cacheStatistics = "writes from cache memory";
        break;
    case 0x0005:
            *cacheStatistics = "time from last hard reset";
        break;
    case 0x0006:
            *cacheStatistics = "time interval";
        break;
    default:
        temp << "vendor specific 0x" << std::hex << std::setfill('0') << std::setw(4) << paramCode;
            *cacheStatistics = temp.str();
        break;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for Cache Memory Parameter Information
//
//  Entry:
//! \param description - string to give the Cache Memory Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void opensea_parser::CScsiCacheMemStatLog::process_Cache_Memory_Statistics_interval_Data(JSONNODE * cacheData)
{
    std::string myStr = "";
    myStr.resize(BASIC);

#if defined_DEBUG
    printf("Cache Memory Statistics Log Description\n");
#endif

    byte_Swap_16(&m_TimeIntervalDescriptorParam->paramCode);
    get_Parameter_Code_Description(m_TimeIntervalDescriptorParam->paramCode, &myStr);

    byte_Swap_Int32(&m_TimeIntervalDescriptorParam->intervalExponent);
    byte_Swap_32(&m_TimeIntervalDescriptorParam->intervalInteger);

    JSONNODE* cacheStatisticsInfo = json_new(JSON_NODE);
    json_set_name(cacheStatisticsInfo, myStr.c_str());

    json_push_back(cacheStatisticsInfo, json_new_i("Time Interval Descriptor-Exponent", m_TimeIntervalDescriptorParam->intervalExponent));
    json_push_back(cacheStatisticsInfo, json_new_i("Time Interval Descriptor-Integer", m_TimeIntervalDescriptorParam->intervalExponent));

    json_push_back(cacheData, cacheStatisticsInfo);
}

//-----------------------------------------------------------------------------
//
//! \fn process_Generic_Data
//
//! \brief
//!   Description: parser out the data for Cache Memory Parameter Information
//
//  Entry:
//! \param description - string to give the Cache Memory Parameter depending on what the code is
//! \param offset - the offset to where params came from
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiCacheMemStatLog::process_Generic_Data(JSONNODE *genData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);

#if defined_DEBUG
    printf("Cache Memory Statistics Log Description\n");
#endif
    byte_Swap_16(&m_CacheMemLog->paramCode);
    get_Parameter_Code_Description(m_CacheMemLog->paramCode, &myHeader);

    std::ostringstream temp;
    temp << std::dec << m_Value;
    json_push_back(genData, json_new_a(myHeader.c_str(), temp.str().c_str()));
}
//-----------------------------------------------------------------------------
//
//! \fn populate_Generic_Param_Value
//
//! \brief
//!   Description: parser out the data for Cache Memory Parameter Information
//
//  Entry:
//! \param paramLength - string to give the Cache Memory Parameter depending on what the code
//! \param offset - the offset to get the value
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiCacheMemStatLog::populate_Generic_Param_Value(uint8_t paramLength, uint32_t offset)
{
    switch (paramLength)
    {
    case ONE_INT_SIZE:
        m_Value = pData[offset];
        break;
    case TWO_INT_SIZE:
        m_Value = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
        break;
    case FOUR_INT_SIZE:
        m_Value = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
        break;
    case EIGHT_INT_SIZE:
        m_Value = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
        break;
    default:
        m_Value = 0;
        break;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Cache_Memory_Statistics_Data
//
//! \brief
//!   Description: parser out the data for the error counter log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiCacheMemStatLog::get_Cache_Memory_Statistics_Data(JSONNODE *masterData)
{

    std::string myStr = "";
    myStr.resize(BASIC);
    std::string headerStr = "";
    headerStr.resize(BASIC);
    eReturnValues retStatus = IN_PROGRESS;
    if (pData != NULL)
    {
        headerStr = "Cache Memory Statistics Log - 19h";
        JSONNODE* pageInfo = json_new(JSON_NODE);

        json_set_name(pageInfo, headerStr.c_str());

        for (uint32_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                uint16_t paramCode = *(reinterpret_cast<uint16_t*>(&pData[offset]));
                byte_Swap_16(&paramCode);
                if (paramCode == 0x0005)
                {
                    m_TimeIntervalDescriptorParam = reinterpret_cast<sTimeIntervalDescriptors*>(&pData[offset]);
                    process_Cache_Memory_Statistics_interval_Data(pageInfo);
                    offset += m_TimeIntervalDescriptorParam->paramLength + LOGPAGESIZE;
                }
                else
                {
                    m_CacheMemLog = reinterpret_cast<sLogParams*>(&pData[offset]);
                    populate_Generic_Param_Value(m_CacheMemLog->paramLength,offset + LOGPAGESIZE);
                    process_Generic_Data(pageInfo);
                    offset += (m_CacheMemLog->paramLength + LOGPAGESIZE);
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
