//
// CScsi_Solid_State_Drive_Log.cpp  Definition of Solid State Drive Log
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Solid_State_Drive_Log.cpp  Definition of Solid State Drive Log
//
#include "CScsi_Solid_State_Drive_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiSolidStateDriveLog()
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
CScsiSolidStateDriveLog::CScsiSolidStateDriveLog()
    : pData()
    , m_SSDName("Solid State Drive Log")
    , m_SSDStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength(0)
    , m_SSDValue(0)
    , m_SSDParam()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_SSDName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiSolidStateDriveLog()
//
//! \brief
//!   Description: Class constructor for the Solid State Drive Log
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param bufferSize - Full size of the buffer 
//! \param pageLength - the size of the page for the parameter header
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiSolidStateDriveLog::CScsiSolidStateDriveLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_SSDName("Solid State Drive Log")
    , m_SSDStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_SSDValue(0)
    , m_SSDParam()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_SSDName.c_str());
    }
    if (buffer != NULL)
    {
        m_SSDStatus = IN_PROGRESS;
    }
    else
    {
        m_SSDStatus = FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiSolidStateDriveLog
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
CScsiSolidStateDriveLog::~CScsiSolidStateDriveLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_SSD_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for SSD Parameter Information
//
//  Entry:
//! \param description - string to give the SSD Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
bool CScsiSolidStateDriveLog::get_SSD_Parameter_Code_Description(std::string *ssdString)
{
    bool descriptionFound = false;
    switch (m_SSDParam->paramCode)
    {
    case 0x0001:
    {
        snprintf((char*)ssdString->c_str(), BASIC, "Percentage Used Indicator");
        descriptionFound = true;
        break;
    }
    default:
    {
        snprintf((char*)ssdString->c_str(), BASIC, "Vendor Specific 0x%04" PRIx16"", m_SSDParam->paramCode);
        break;
    }
    }
    return descriptionFound;
}
//-----------------------------------------------------------------------------
//
//! \fn process_Solid_State_Drive_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed SSD counter data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiSolidStateDriveLog::process_Solid_State_Drive_Data(JSONNODE *ssdData)
{
    bool descriptionFound = false;
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);

#if defined _DEBUG
    printf("Solid State Drive Log  \n");
#endif
    byte_Swap_16(&m_SSDParam->paramCode);
    descriptionFound = get_SSD_Parameter_Code_Description(&myHeader);

    JSONNODE *ssdInfo = json_new(JSON_NODE);
    json_set_name(ssdInfo, (char*)myHeader.c_str());

    snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_SSDParam->paramCode);
    json_push_back(ssdInfo, json_new_a("Solid State Drive Param Code", (char*)myStr.c_str()));

    if (!descriptionFound)
    {
        snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_SSDParam->paramControlByte);
        json_push_back(ssdInfo, json_new_a("Solid State Drive Param Control Byte ", (char*)myStr.c_str()));
        snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_SSDParam->paramLength);
        json_push_back(ssdInfo, json_new_a("Solid State Drive Param Length ", (char*)myStr.c_str()));
    }

    if (m_SSDParam->paramLength == 8 || m_SSDValue > UINT32_MAX)
    {
        set_json_64bit(ssdInfo, "Solid State Drive Param Value", m_SSDValue, false);
    }
    else
    {
        if (M_GETBITRANGE(m_SSDValue, 31, 16) == 0)
        {
            json_push_back(ssdInfo, json_new_i("Solid State Drive Param Value", static_cast<uint32_t>(m_SSDValue)));
        }
        else
        {
            snprintf((char*)myStr.c_str(), BASIC, "%" PRIu32"", static_cast<uint32_t>(m_SSDValue));
            json_push_back(ssdInfo, json_new_a("Solid State Drive Param Value", (char*)myStr.c_str()));
        }
    }

    json_push_back(ssdData, ssdInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Solid_State_Drive_Data
//
//! \brief
//!   Description: parser out the data for the SSD log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiSolidStateDriveLog::get_Solid_State_Drive_Data(JSONNODE *masterData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string headerStr = "";
    headerStr.resize(BASIC);
    eReturnValues retStatus = IN_PROGRESS;
    if (pData != NULL)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Solid State Drive Log - 11h");

        for (size_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_SSDParam = (sSSDParams *)&pData[offset];
                offset += sizeof(sSSDParams);
                switch (m_SSDParam->paramLength)
                {
                case 1:
                {
                    if ((offset + m_SSDParam->paramLength) < m_bufferLength)
                    {
                        m_SSDValue = pData[offset];
                        offset += m_SSDParam->paramLength;
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
                    if ((offset + m_SSDParam->paramLength) < m_bufferLength)
                    {
                        m_SSDValue = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
                        offset += m_SSDParam->paramLength;
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
                    if ((offset + m_SSDParam->paramLength) < m_bufferLength)
                    {
                        m_SSDValue = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
                        offset += m_SSDParam->paramLength;
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
                    if ((offset + m_SSDParam->paramLength) < m_bufferLength)
                    {
                        m_SSDValue = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
                        offset += m_SSDParam->paramLength;
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
                    break;
                }
                }
                process_Solid_State_Drive_Data(pageInfo);
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