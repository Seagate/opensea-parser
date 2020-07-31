//
// CScsi_Format_Status_Log.cpp  Format Status log page reports information about the most recent successful format operation
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Format_Status_Log.cpp  Format Status log page reports information about the most recent successful format operation
//
#include "CScsi_Format_Status_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiFormatStatusLog()
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
CScsiFormatStatusLog::CScsiFormatStatusLog()
    : pData()
    , m_FormatName("Format Status Log")
    , m_FormatStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength()
    , m_Value(0)
    , m_Format()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_FormatName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiFormatStatusLog()
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
CScsiFormatStatusLog::CScsiFormatStatusLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData(buffer)
    , m_FormatName("Format Status Log")
    , m_FormatStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_Value(0)
    , m_Format()
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_FormatName.c_str());
    }
    if (buffer != NULL)
    {
        m_FormatStatus = IN_PROGRESS;
    }
    else
    {
        m_FormatStatus = FAILURE;
    }

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiFormatStatusLog
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
CScsiFormatStatusLog::~CScsiFormatStatusLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Format_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the value information
//
//  Entry:
//! \param formatData - string to give the value string into
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CScsiFormatStatusLog::get_Format_Parameter_Code_Description(std::string *valueData)
{
    switch (m_Format->paramCode)
    {
    case 0x0000:
    {
        snprintf((char*)valueData->c_str(), BASIC, "Format Data Out count");
        break;
    }
    case 0x0001:
    {
        snprintf((char*)valueData->c_str(), BASIC, "Grown Defects During Certification count");
        break;
    }
    case 0x0002:
    {
        snprintf((char*)valueData->c_str(), BASIC, "Total Blocks Reassigned During Format count");
        break;
    }
    case 0x0003:
    {
        snprintf((char*)valueData->c_str(), BASIC, "Total New Blocks Reassigned count");
        break;
    }
    case 0x0004:
    {
        snprintf((char*)valueData->c_str(), BASIC, "Power On Minutes Since Format count");
        break;
    }
    default:
    {
        snprintf((char*)valueData->c_str(), BASIC, "Vendor Specific 0x%04" PRIx16"", m_Format->paramCode);
        break;
    }
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_Format_Status_Data
//
//! \brief
//!   Description: parser out the data for a Format Status
//
//  Entry:
//! \param eventData - Json node that parsed cache data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiFormatStatusLog::process_Format_Status_Data(JSONNODE *formatData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);

#if defined_DEBUG
    printf("Format Status Log\n");
#endif

    get_Format_Parameter_Code_Description(&myHeader);
    JSONNODE *formatInfo = json_new(JSON_NODE);
    json_set_name(formatInfo, (char*)myHeader.c_str());

    snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_Format->paramCode);
    json_push_back(formatInfo, json_new_a("Parameter Code", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Format->paramControlByte);
    json_push_back(formatInfo, json_new_a("Control Byte ", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Format->paramLength);
    json_push_back(formatInfo, json_new_a("Length ", (char*)myStr.c_str()));
    if (m_Format->paramLength == 8 || m_Value > UINT32_MAX)
    {
        set_json_64bit(formatInfo, "Value", m_Value, true);
    }
    else
    {
        if (M_GETBITRANGE(m_Value, 33, 15) == 0)
        {
            json_push_back(formatInfo, json_new_i("Value", static_cast<uint32_t>(m_Value)));
        }
        else
        {
            std::string printStr;
            snprintf((char*)printStr.c_str(), BASIC, "0x%08" PRIx64"", m_Value);
            json_push_back(formatInfo, json_new_a("Value", (char*)printStr.c_str()));
        }
    }
    json_push_back(formatData, formatInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn process_Format_Status_Data_Variable_Length
//
//! \brief
//!   Description: parser out the data for a format status for param with variable length
//
//  Entry:
//! \param eventData - Json node that parsed cache data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiFormatStatusLog::process_Format_Status_Data_Variable_Length(JSONNODE * formatData)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);

#if defined_DEBUG
    printf("Format Status Log  \n");
#endif
    get_Format_Parameter_Code_Description(&myHeader);
    JSONNODE *formatInfo = json_new(JSON_NODE);
    json_set_name(formatInfo, (char*)myHeader.c_str());

    snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", m_Format->paramCode);
    json_push_back(formatInfo, json_new_a("Format Status Parameter Code", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Format->paramControlByte);
    json_push_back(formatInfo, json_new_a("Format Status Control Byte ", (char*)myStr.c_str()));
    snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", m_Format->paramLength);
    json_push_back(formatInfo, json_new_a("Format Status Length ", (char*)myStr.c_str()));

    uint8_t lineNumber = 0;
    char *innerMsg = (char*)calloc(128, sizeof(char));
    char* innerStr = (char*)calloc(60, sizeof(char));
    uint8_t offset = 0;

    for (uint8_t outer = 0; outer < m_Format->paramLength - 1; )
    {
        snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIX32 "", lineNumber);
        sprintf(innerMsg, "%02" PRIX8 "", m_FormatDataOutParamValue[offset++]);
        // inner loop for creating a single ling of the buffer data
        for (uint8_t inner = 1; inner < 16 && offset < m_Format->paramLength - 1; inner++)
        {
            sprintf(innerStr, "%02" PRIX8"", m_FormatDataOutParamValue[offset]);
            if (inner % 4 == 0)
            {
                strncat(innerMsg, " ", 1);
            }
            strncat(innerMsg, innerStr, 2);
            offset++;
        }
        // push the line to the json node
        json_push_back(formatInfo, json_new_a((char*)myStr.c_str(), innerMsg));
        outer = offset;
        lineNumber = outer;
    }
    safe_Free(innerMsg);  //free the string
    safe_Free(innerStr);  // free the string

    json_push_back(formatData, formatInfo);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Format_Status_Data
//
//! \brief
//!   Description: parser out the data for the Format Status Log
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiFormatStatusLog::get_Format_Status_Data(JSONNODE *masterData)
{
    std::string headerStr = "";
    headerStr.resize(BASIC);
    eReturnValues retStatus = IN_PROGRESS;

    if (pData != NULL)
    {
        snprintf((char*)headerStr.c_str(), BASIC, "Format Status Log 08h");
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, (char*)headerStr.c_str());

        for (size_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_Format = (sFormatParams *)&pData[offset];
                offset += sizeof(sFormatParams);
                byte_Swap_16(&m_Format->paramCode);
                if (m_Format->paramCode == 0x0000 && (m_Format->paramLength > 8))
                {
                    if ((offset + m_Format->paramLength) < m_bufferLength)
                    {
                        m_FormatDataOutParamValue = (reinterpret_cast<uint8_t*>(&pData[offset]));
                        offset += m_Format->paramLength;
                        process_Format_Status_Data_Variable_Length(pageInfo);
                    }
                    else
                    {
                        //json_push_back(masterData, pageInfo);
                        return BAD_PARAMETER;
                    }
                }
                else
                {
                    switch (m_Format->paramLength)
                    {
                    case 1:
                    {
                        if ((offset + m_Format->paramLength) < m_bufferLength)
                        {
                            m_Value = pData[offset];
                            offset += m_Format->paramLength;
                        }
                        else
                        {
                            //json_push_back(masterData, pageInfo);
                            return BAD_PARAMETER;
                        }
                        break;
                    }
                    case 2:
                    {
                        if ((offset + m_Format->paramLength) < m_bufferLength)
                        {
                            m_Value = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
                            offset += m_Format->paramLength;
                        }
                        else
                        {
                            //json_push_back(masterData, pageInfo);
                            return BAD_PARAMETER;
                        }
                        break;
                    }
                    case 4:
                    {
                        if ((offset + m_Format->paramLength) < m_bufferLength)
                        {
                            m_Value = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
                            offset += m_Format->paramLength;
                        }
                        else
                        {
                            //json_push_back(masterData, pageInfo);
                            return BAD_PARAMETER;
                        }
                        break;
                    }
                    case 8:
                    {
                        if ((offset + m_Format->paramLength) < m_bufferLength)
                        {
                            m_Value = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
                            offset += m_Format->paramLength;
                        }
                        else
                        {
                            //json_push_back(masterData, pageInfo);
                            return BAD_PARAMETER;
                        }
                        break;
                    }
                    default:
                    {
                        //json_push_back(masterData, pageInfo);
                        return BAD_PARAMETER;
                        break;
                    }
                    }
                    process_Format_Status_Data(pageInfo);
                }
            }
            else
            {
                //json_push_back(masterData, pageInfo);
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