//
// CScsi_Cache_Statistics_Log.cpp  Definition of Cache Statistics page for SAS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Cache_Statistics_Log.cpp  Definition of Cache Statistics page for SAS
//
#include "CScsi_Cache_Statistics_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiCacheLog()
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
CScsiCacheLog::CScsiCacheLog()
    : pData()
    , m_CacheName("Cache Statistics Log")
    , m_CacheStatus(eReturnValues::IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength()
    , m_Value(0)
    , m_cache()
{
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_CacheName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiCacheLog()
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
CScsiCacheLog::CScsiCacheLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_CacheName("Cache Statistics Log")
    , m_CacheStatus(eReturnValues::IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_Value(0)
    , m_cache()
{
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_CacheName.c_str());
    }
    if (buffer != M_NULLPTR)
    {
        m_CacheStatus = eReturnValues::IN_PROGRESS;
    }
    else
    {
        m_CacheStatus = eReturnValues::FAILURE;
    }

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiCacheLog
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
CScsiCacheLog::~CScsiCacheLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Cache_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for Cache Parameter Information
//
//  Entry:
//! \param description - string to give the Cache Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
bool CScsiCacheLog::get_Cache_Parameter_Code_Description(std::string *cache)
{
    bool descriptionFound = false;
    switch (m_cache->paramCode)
    {
    case 0x0000:
    {
        *cache = "Number of logical blocks that have been Sent";
        descriptionFound = true;
        break;
    }
    case 0x0001:
    {
        *cache = "Number of logical blocks that have been Received";
        descriptionFound = true;
        break;
    }
    case 0x0002:
    {
        *cache = "Number of logical blocks READ from the Cache Memory";
        descriptionFound = true;
        break;
    }
    case 0x0003:
    {
        *cache = "# of R and W Commands lengths equal or less than the current segment size";
        descriptionFound = true;
        break;
    }
    case 0x0004:
    {
        *cache = "# of READ and WRITE Commands lengths greater than the current segment size";
        descriptionFound = true;
        break;
    }
    default:
    {
        std::ostringstream temp;
        temp << "Vendor specific 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_cache->paramCode;
        cache->assign(temp.str());
        break;
    }
    }
    return descriptionFound;
}
//-----------------------------------------------------------------------------
//
//! \fn process_Cache_Event_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed cache data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiCacheLog::process_Cache_Event_Data(JSONNODE *cacheData)
{
    std::string myStr;
#if defined _DEBUG
    printf("Cache Event Description \n");
#endif
    if (m_Value != 0)
    {
        byte_Swap_16(&m_cache->paramCode);
        bool discriptionIsFound = get_Cache_Parameter_Code_Description(&myStr);
        JSONNODE *cacheInfo = json_new(JSON_NODE);
        json_set_name(cacheInfo, myStr.c_str());
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_cache->paramCode;
        json_push_back(cacheInfo, json_new_a("Cache Statistics Parameter Code", temp.str().c_str()));
        if (!discriptionIsFound)
        {
            temp.str("");temp.clear();

            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_cache->paramControlByte);
            json_push_back(cacheInfo, json_new_a("Cache Statistics Control Byte ", temp.str().c_str()));
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_cache->paramLength);

            json_push_back(cacheInfo, json_new_a("Cache Statistics Length ", temp.str().c_str()));
        }
        set_json_64bit(cacheInfo, "Cache Statistics Value", m_Value, false);

        json_push_back(cacheData, cacheInfo);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data
//
//! \brief
//!   Description: parser out the data for the Cache Statistics Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiCacheLog::get_Cache_Data(JSONNODE *masterData)
{
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    if (pData != M_NULLPTR)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Cache Statistics Log - 37h");

        for (size_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_cache = reinterpret_cast<sCacheParams*>(&pData[offset]);
                offset += sizeof(sCacheParams);
                switch (m_cache->paramLength)
                {
                case 1:
                {
                    if ((offset + m_cache->paramLength) < m_bufferLength)
                    {
                        m_Value = pData[offset];
                        offset += m_cache->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return eReturnValues::BAD_PARAMETER;
                    }
                    break;
                }
                case 2:
                {
                    if ((offset + m_cache->paramLength) < m_bufferLength)
                    {
                        m_Value = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
                        offset += m_cache->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return eReturnValues::BAD_PARAMETER;
                    }
                    break;
                }
                case 4:
                {
                    if ((offset + m_cache->paramLength) < m_bufferLength)
                    {
                        m_Value = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
                        offset += m_cache->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return eReturnValues::BAD_PARAMETER;
                    }
                    break;
                }
                case 8:
                {
                    if ((offset + m_cache->paramLength) < m_bufferLength)
                    {
                        m_Value = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
                        offset += m_cache->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return eReturnValues::BAD_PARAMETER;
                    }
                    break;
                }
                default:
                {
                    json_push_back(masterData, pageInfo);
                    return eReturnValues::BAD_PARAMETER;
                }
                }
                process_Cache_Event_Data(pageInfo);
            }
            else
            {
                json_push_back(masterData, pageInfo);
                return eReturnValues::BAD_PARAMETER;
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