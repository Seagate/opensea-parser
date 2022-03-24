//
// CScsi_Factory_Log.cpp  Definition of Factory Log page for SAS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Factory_Log.cpp  Definition of Factory Log page for SAS
//
#include "CScsi_Factory_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiFactoryLog()
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
CScsiFactoryLog::CScsiFactoryLog()
    : pData()
    , m_FactoryName("Factory Log Page")
    , m_FactoryStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength()
    , m_Value(0)
    , m_factory()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_FactoryName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiFactoryLog()
//
//! \brief
//!   Description: Class constructor for the factory Log
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiFactoryLog::CScsiFactoryLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_FactoryName("Factory Log Page")
    , m_FactoryStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_Value(0)
    , m_factory()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_FactoryName.c_str());
    }
    if (buffer != NULL)
    {
        m_FactoryStatus = IN_PROGRESS;
    }
    else
    {
        m_FactoryStatus = FAILURE;
    }

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiFactoryLog
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
CScsiFactoryLog::~CScsiFactoryLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn process_Factorty_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed Factory data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiFactoryLog::process_Factorty_Data(JSONNODE *factoryData)
{
    std::string myStr = "";
    if (m_Value != 0)
    {
#if defined _DEBUG
        printf("Factory Description \n");
#endif
        byte_Swap_16(&m_factory->paramCode);
        std::ostringstream temp;
        temp << "Factory Description " << std::dec << m_factory->paramCode;
        JSONNODE *factoryInfo = json_new(JSON_NODE);
        json_set_name(factoryInfo, temp.str().c_str());

        temp.str().clear(); temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(4) << m_factory->paramCode;
        json_push_back(factoryInfo, json_new_a("Factory Parameter Code", temp.str().c_str()));

        temp.str().clear(); temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_factory->paramControlByte);
        json_push_back(factoryInfo, json_new_a("Factory Control Byte ", temp.str().c_str()));
        temp.str().clear(); temp.clear();
        temp << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_factory->paramLength);
        json_push_back(factoryInfo, json_new_a("FactoryLength ", temp.str().c_str()));
        if (m_factory->paramLength == 8 || m_Value > UINT32_MAX)
        {
            set_json_64bit(factoryInfo, "Factory value", m_Value, false);
        }
        else
        {
            json_push_back(factoryInfo, json_new_i("Factory value", static_cast<uint32_t>(m_Value)));
        }

        json_push_back(factoryData, factoryInfo);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Factory_Data
//
//! \brief
//!   Description: parser out the data for the Factory Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiFactoryLog::get_Factory_Data(JSONNODE *masterData)
{
    eReturnValues retStatus = IN_PROGRESS;
    if (pData != NULL)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Factory Log - 3Eh");

        for (size_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_factory = reinterpret_cast<sFactoryParams*>(&pData[offset]);
                offset += sizeof(sFactoryParams);
                switch (m_factory->paramLength)
                {
                case 1:
                {
                    if ((offset + m_factory->paramLength) <= m_bufferLength)
                    {
                        m_Value = pData[offset];
                        offset += m_factory->paramLength;
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
                    if ((offset + m_factory->paramLength) <= m_bufferLength)
                    {
                        m_Value = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
                        offset += m_factory->paramLength;
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
                    if ((offset + m_factory->paramLength) <= m_bufferLength)
                    {
                        m_Value = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
                        offset += m_factory->paramLength;
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
                    if ((offset + m_factory->paramLength) <= m_bufferLength)
                    {
                        m_Value = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
                        offset += m_factory->paramLength;
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
                process_Factorty_Data(pageInfo);
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