//
// CScsi_Zoned_Device_Statistics_Log.cpp  
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Zoned_Device_Statistics_Log.cpp  Definition of Zoned Device Statistics Log
//
#include "CScsi_Zoned_Device_Statistics_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiZonedDeviceStatisticsLog()
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
CScsiZonedDeviceStatisticsLog::CScsiZonedDeviceStatisticsLog()
    : pData()
    , m_ZDSName("Zoned Device Statistics Log")
    , m_ZDSStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength(0)
    , m_ZDSValue(0)
    , m_ZDSParam()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_ZDSName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiZonedDeviceStatisticsLog()
//
//! \brief
//!   Description: Class constructor for the Error Counter for READ WRITE VERIFY ERRORS
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//! \param type - need to know the type of the page   READ or WRITE or VERIFY
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiZonedDeviceStatisticsLog::CScsiZonedDeviceStatisticsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_ZDSName("Zoned Device Statistics Log")
    , m_ZDSStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_ZDSValue(0)
    , m_ZDSParam()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_ZDSName.c_str());
    }
    if (buffer != NULL)
    {
        m_ZDSStatus = IN_PROGRESS;
    }
    else
    {
        m_ZDSStatus = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiZonedDeviceStatisticsLog
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
CScsiZonedDeviceStatisticsLog::~CScsiZonedDeviceStatisticsLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_ZDS_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for ZDS Parameter Information
//
//  Entry:
//! \param description - string to give the ZDS Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
bool CScsiZonedDeviceStatisticsLog::get_ZDS_Parameter_Code_Description(std::string *zdsString)
{
    bool descriptionFound = false;
    switch (m_ZDSParam->paramCode)
    {
    case 0x0000:
    {
        zdsString->assign("maximum open zones");
        descriptionFound = true;
        break;
    }
    case 0x0001:
    {
        zdsString->assign("maximum explicitly open zones");
        descriptionFound = true;
        break;
    }
    case 0x0002:
    {
        zdsString->assign("maximum implicitly open zones");
        descriptionFound = true;
        break;
    }
    case 0x0003:
    {
        zdsString->assign("minimum empty zones");
        descriptionFound = true;
        break;
    }
    case 0x0004:
    {
        zdsString->assign("maximum non-sequential zones");
        descriptionFound = true;
        break;
    }
    case 0x0005:
    {
        zdsString->assign("zones emptied");
        descriptionFound = true;
        break;
    }
    case 0x0006:
    {
        zdsString->assign("suboptimal write commands");
        descriptionFound = true;
        break;
    }
    case 0x0007:
    {
        zdsString->assign("commands exceeding optimal limit");
        descriptionFound = true;
        break;
    }
    case 0x0008:
    {
        zdsString->assign("failed explicit opens");
        descriptionFound = true;
        break;
    }
    case 0x0009:
    {
        zdsString->assign("read rule violations");
        descriptionFound = true;
        break;
    }
    case 0x000A:
    {
        zdsString->assign("write rule violations");
        descriptionFound = true;
        break;
    }
    default:
    {
        std::ostringstream temp;
        temp << "Vendor Specific 0x" << std::hex << std::setfill('0') << std::setw(4) <<  m_ZDSParam->paramCode;
        zdsString->assign(temp.str());
        break;
    }
    }
    return descriptionFound;
}
//-----------------------------------------------------------------------------
//
//! \fn process_Zoned_Device_Statistics_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed ZDS counter data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiZonedDeviceStatisticsLog::process_Zoned_Device_Statistics_Data(JSONNODE *zdsData)
{
    std::string myHeader;
    if (m_ZDSValue != 0)
    {
#if defined _DEBUG
        printf("Zoned Device Statistics Log  \n");
#endif
        byte_Swap_16(&m_ZDSParam->paramCode);
        bool descriptionFound = get_ZDS_Parameter_Code_Description( &myHeader);

        JSONNODE *zdsInfo = json_new(JSON_NODE);
        json_set_name(zdsInfo, myHeader.c_str());

        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_ZDSParam->paramCode;
        json_push_back(zdsInfo, json_new_a("Zoned Device Statistics Param Code", temp.str().c_str()));

        if (!descriptionFound)
        {
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_ZDSParam->paramControlByte);
            json_push_back(zdsInfo, json_new_a("Zoned Device Statistics Param Control Byte ", temp.str().c_str()));
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_ZDSParam->paramLength);
            json_push_back(zdsInfo, json_new_a("Zoned Device Statistics Param Length ", temp.str().c_str()));
        }

        if (m_ZDSParam->paramLength == 8 || m_ZDSValue > UINT32_MAX)
        {
            set_json_64bit(zdsInfo, "Zoned Device Statistics Param Value", m_ZDSValue, false);
        }
        else
        {
            if (M_GETBITRANGE(m_ZDSValue, 31, 16) == 0)
            {
                json_push_back(zdsInfo, json_new_i("Zoned Device Statistics Param Value", static_cast<uint32_t>(m_ZDSValue)));
            }
            else
            {
                temp.str("");temp.clear();
                temp << std::dec << m_ZDSValue;
                json_push_back(zdsInfo, json_new_a("Zoned Device Statistics Param Value", temp.str().c_str()));
            }
        }

        json_push_back(zdsData, zdsInfo);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Zoned_Device_Statistics_Data
//
//! \brief
//!   Description: parser out the data for the ZDS counter log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiZonedDeviceStatisticsLog::get_Zoned_Device_Statistics_Data(JSONNODE *masterData)
{
    eReturnValues retStatus = IN_PROGRESS;
    if (pData != NULL)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Zoned Device Statistics Log - 14h");

        for (size_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_ZDSParam = reinterpret_cast<sZDSParams*>(&pData[offset]);
                offset += sizeof(sZDSParams);
                switch (m_ZDSParam->paramLength)
                {
                case 1:
                {
                    if ((offset + m_ZDSParam->paramLength) < m_bufferLength)
                    {
                        m_ZDSValue = pData[offset];
                        offset += m_ZDSParam->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return BAD_PARAMETER;
                    }
                    break;
                }
                case 2:
                {
                    if ((offset + m_ZDSParam->paramLength) < m_bufferLength)
                    {
                        m_ZDSValue = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
                        offset += m_ZDSParam->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return BAD_PARAMETER;
                    }
                    break;
                }
                case 4:
                {
                    if ((offset + m_ZDSParam->paramLength) < m_bufferLength)
                    {
                        m_ZDSValue = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
                        offset += m_ZDSParam->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return BAD_PARAMETER;
                    }
                    break;
                }
                case 8:
                {
                    if ((offset + m_ZDSParam->paramLength) < m_bufferLength)
                    {
                        m_ZDSValue = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
                        offset += m_ZDSParam->paramLength;
                    }
                    else
                    {
                        json_push_back(masterData, pageInfo);
                        return BAD_PARAMETER;
                    }
                    break;
                }
                default:
                {
                    json_push_back(masterData, pageInfo);
                    return BAD_PARAMETER;
                }
                }
                process_Zoned_Device_Statistics_Data(pageInfo);
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
