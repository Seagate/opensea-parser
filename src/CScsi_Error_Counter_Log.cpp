//
// CScsi_Error_Counter_Log.cpp  Definition of Error Counter for READ WRITE VERIFY ERRORS
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Error_Counter_Log.cpp  Definition of Error Counter for READ WRITE VERIFY ERRORS
//
#include "CScsi_Error_Counter_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiErrorCounterLog()
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
CScsiErrorCounterLog::CScsiErrorCounterLog()
    : pData()
    , m_ErrorName("Error Counter Log")
    , m_ErrorStatus(eReturnValues::IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength(0)
    , m_ErrorValue(0)
    , m_Error()
    , m_pageType(WRITE)
{
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_ErrorName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiErrorCounterLog()
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
CScsiErrorCounterLog::CScsiErrorCounterLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength, uint8_t type)
    : pData(buffer)
    , m_ErrorName("Error Counter Log")
    , m_ErrorStatus(eReturnValues::IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_ErrorValue(0)
    , m_Error()
    , m_pageType(type)
{
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_ErrorName.c_str());
    }
    if (buffer != M_NULLPTR)
    {
        if (m_pageType == WRITE || m_pageType == READ || m_pageType == VERIFY)
        {
            m_ErrorStatus = eReturnValues::IN_PROGRESS;
        }
        else
        {
            m_ErrorStatus = eReturnValues::BAD_PARAMETER;
        }
    }
    else
    {
        m_ErrorStatus = eReturnValues::FAILURE;
    }

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiErrorCounterLog
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
CScsiErrorCounterLog::~CScsiErrorCounterLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn append_Error_Log_Page_Number
//
//! \brief
//!   Description: appends the Error Log Page Number so people can search on the number
//
//  Entry:
//! \param *typeStr - pointer to the string that we will be creating
//! \param main - this is the main header string that we will be adding the master string to
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CScsiErrorCounterLog::append_Error_Log_Page_Number(std::string *typeStr, std::string main)
{
    std::ostringstream temp;
    switch (m_pageType)
    {
    case 0x02:
    {
        temp << main << "02h";// WRITE
        break;
    }
    case 0x03:
    {
        temp << main << "03h"; // READ
        break;
    }
    case 0x05:
    {
        temp << main << "05h"; // VERIFY
        break;
    }
    default:
        temp << main << "03h";  //READ
        break;
    }
    typeStr->assign(temp.str());
}
//-----------------------------------------------------------------------------
//
//! \fn set_Master_String
//
//! \brief
//!   Description: creates the master string for the header information
//
//  Entry:
//! \param *typeStr - pointer to the string that we will be creating
//! \param main - this is the main header string that we will be adding the master string to
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void CScsiErrorCounterLog::set_Master_String(std::string *typeStr, std::string main)
{
    std::ostringstream temp;
    switch (m_pageType)
    {
    case 0x02:
    {
        temp << "WRITE" << main;
        break;
    }
    case 0x03:
    {
        temp << "READ" << main;
        break;
    }
    case 0x05:
    {
        temp << "VERIFY" << main;
        break;
    }
    default:
        temp << "READ" << main;
        break;
    }
    typeStr->assign(temp.str());
}
//-----------------------------------------------------------------------------
//
//! \fn get_Error_Parameter_Code_Description
//
//! \brief
//!   Description: parser out the data for Error Parameter Information
//
//  Entry:
//! \param description - string to give the Error Parameter depending on what the code is
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
bool CScsiErrorCounterLog::get_Error_Parameter_Code_Description(std::string *error)
{
    bool descriptionFound = false;
    switch (m_Error->paramCode)
    {
    case 0x0000:
    {
        error->assign("Corrected Without Substantial Delay");
        descriptionFound = true;
        break;
    }
    case 0x0001:
    {
        error->assign("Corrected With Possible Delay");
        descriptionFound = true;
        break;
    }
    case 0x0002:
    {
        error->assign("Number of Errors that are Corrected by Applying Retries");
        descriptionFound = true;
        break;
    }
    case 0x0003:
    {
        error->assign("Total Number of Errors Corrected");
        descriptionFound = true;
        break;
    }
    case 0x0004:
    {
        error->assign("Total Times Correction Algorithm Processed");
        descriptionFound = true;
        break;
    }
    case 0x0005:
    {
        error->assign("Total Bytes Processed");
        descriptionFound = true;
        break;
    }
    case 0x0006:
    {
        error->assign("Total Uncorrected Errors");
        descriptionFound = true;
        break;
    }
    default:
    {
        std::ostringstream temp;
        temp << "Vendor Specific 0x" << std::hex << std::uppercase << std::setfill('0') << std::setw << m_Error->paramCode;
        error->assign(temp.str());
        break;
    }
    }
    return descriptionFound;
}
//-----------------------------------------------------------------------------
//
//! \fn process_Error_Data
//
//! \brief
//!   Description: parser out the data for a single event
//
//  Entry:
//! \param eventData - Json node that parsed error counter data will be added to
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CScsiErrorCounterLog::process_Error_Data(JSONNODE *errorData)
{
    bool descriptionFound = false;
    std::string myStr = "";
    std::string myHeader = "";
#if defined _DEBUG
    printf("Error Counter Log  \n");
#endif
    byte_Swap_16(&m_Error->paramCode);
    descriptionFound = get_Error_Parameter_Code_Description(&myHeader);
    if (m_ErrorValue != 0)
    {
        set_Master_String(&myStr, myHeader);
        JSONNODE *errorInfo = json_new(JSON_NODE);
        json_set_name(errorInfo, myStr.c_str());
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_Error->paramCode;
        json_push_back(errorInfo, json_new_a("Error Counter Code", temp.str().c_str()));
        if (!descriptionFound)
        {
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Error->paramControlByte);
            json_push_back(errorInfo, json_new_a("Error Counter Control Byte ", temp.str().c_str()));
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_Error->paramLength);
            json_push_back(errorInfo, json_new_a("Error Counter Length ", temp.str().c_str()));
        }
        if (m_Error->paramLength == 8 || m_ErrorValue > UINT32_MAX)
        {
            set_json_64bit(errorInfo, "Error Count", m_ErrorValue, false);
        }
        else
        {
            if (M_GETBITRANGE(m_ErrorValue, 33, 15) == 0)
            {
                json_push_back(errorInfo, json_new_i("Error Count", static_cast<uint32_t>(m_ErrorValue)));
            }
            else
            {
                temp.str("");temp.clear();
                temp << std::dec << static_cast<uint32_t>(m_ErrorValue);
                json_push_back(errorInfo, json_new_a(myStr.c_str(), temp.str().c_str()));
            }
        }

        json_push_back(errorData, errorInfo);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Error_Counter_Data
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
eReturnValues CScsiErrorCounterLog::get_Error_Counter_Data(JSONNODE *masterData)
{
    std::string myStr = "";
    std::string headerStr = "";
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    if (pData != M_NULLPTR)
    {
        set_Master_String(&headerStr, "Error Counter Log");
        append_Error_Log_Page_Number(&myStr, headerStr);
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, myStr.c_str());

        for (size_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                m_Error = reinterpret_cast<sErrorParams*>(&pData[offset]);
                offset += sizeof(sErrorParams);
                switch (m_Error->paramLength)
                {
                case 1:
                {
                    if ((offset + m_Error->paramLength) < m_bufferLength)
                    {
                        m_ErrorValue = pData[offset];
                        offset += m_Error->paramLength;
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
                    if ((offset + m_Error->paramLength) < m_bufferLength)
                    {
                        m_ErrorValue = M_BytesTo2ByteValue(pData[offset], pData[offset + 1]);
                        offset += m_Error->paramLength;
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
                    if ((offset + m_Error->paramLength) < m_bufferLength)
                    {
                        m_ErrorValue = M_BytesTo4ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3]);
                        offset += m_Error->paramLength;
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
                    if ((offset + m_Error->paramLength) < m_bufferLength)
                    {
                        m_ErrorValue = M_BytesTo8ByteValue(pData[offset], pData[offset + 1], pData[offset + 2], pData[offset + 3], pData[offset + 4], pData[offset + 5], pData[offset + 6], pData[offset + 7]);
                        offset += m_Error->paramLength;
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
                process_Error_Data(pageInfo);
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