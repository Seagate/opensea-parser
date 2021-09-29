//
// CScsi_Page_19h_Command_Duration_Limits_Log.cpp  
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Page_19h_Command_Duration_Limits_Log.cpp  Definition of SCSI Command Duration Limits Log
//
#include "CScsi_Page_19h_Command_Duration_Limits_Log.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiCmdDurationLimitsLog()
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
CScsiCmdDurationLimitsLog::CScsiCmdDurationLimitsLog()
    : pData()
    , m_CMDLimitsName("Command Duration Limits Log")
    , m_LogStatus(IN_PROGRESS)
    , m_PageLength(0)
    , m_bufferLength(0)
    , m_commandLog(NULL)
    , m_limitsLog(NULL)
    , m_Value(0)
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_CMDLimitsName.c_str());
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CScsiPerformanceStatisticsLog()
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
CScsiCmdDurationLimitsLog::CScsiCmdDurationLimitsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength)
    : pData()
    , m_CMDLimitsName("Command Duration Limits Log")
    , m_LogStatus(IN_PROGRESS)
    , m_PageLength(pageLength)
    , m_bufferLength(bufferSize)
    , m_commandLog(NULL)
    , m_limitsLog(NULL)
    , m_Value(0)
{
    if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        printf("%s \n", m_CMDLimitsName.c_str());
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
//! \fn CScsiCmdDurationLimitsLog
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
CScsiCmdDurationLimitsLog::~CScsiCmdDurationLimitsLog()
{

}
//-----------------------------------------------------------------------------
//
//! \fn get_Parameter_Code_Description
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
void CScsiCmdDurationLimitsLog::get_Parameter_Code_Description(uint16_t paramCode, std::string* generalStr)
{
    if (paramCode >= 0x0011 && paramCode <= 0x0017)
    {
        if (g_dataformat == PREPYTHON_DATA)
        {
            snprintf((char*)generalStr->c_str(), BASIC, "cmd_druation_limit_T2B");
        }
        else
        {
            snprintf((char*)generalStr->c_str(), BASIC, "Command Duration Limit T2A");
        }
    }
    else if (paramCode >= 0x0011 && paramCode <= 0x0017)
    {
        if (g_dataformat == PREPYTHON_DATA)
        {
            snprintf((char*)generalStr->c_str(), BASIC, "cmd_druation_limit_T2B");
        }
        else
        {
            snprintf((char*)generalStr->c_str(), BASIC, "Command Duration Limit T2B");
        }
    }
    else
    {
        if (g_dataformat == PREPYTHON_DATA)
        {
            snprintf((char*)generalStr->c_str(), BASIC, "vendor_specific_0x%04" PRIx16"", paramCode);
        }
        else
        {
            snprintf((char*)generalStr->c_str(), BASIC, "vendor specific 0x%04" PRIx16"", paramCode);
        }

    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_Generic_Data()
//
//! \brief
//!   Description: function for parsing out the Generic information
//
//  Entry:
//! \param fuaData - the Json node olding all the FUA data 
//! \param paramCode - passed in param code
//! \param offset - offset where the data came from
//
//  Exit:
//
//---------------------------------------------------------------------------
void CScsiCmdDurationLimitsLog::process_Generic_Data(JSONNODE* genericData, uint16_t paramCode, uint32_t offset)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);

#if defined_DEBUG
    printf("Genaric Log Description\n");
#endif
    byte_Swap_16(&m_commandLog->paramCode);
    get_Parameter_Code_Description(m_commandLog->paramCode, &myHeader);
    if (g_dataformat == PREPYTHON_DATA)
    {
        prePython_int(genericData, (char*)myHeader.c_str(), "generic", "count", m_Value, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, paramCode, offset);

    }
    else
    {
        snprintf((char*)myStr.c_str(), BASIC, "%" PRIu64"", m_Value);
        json_push_back(genericData, json_new_a((char*)myHeader.c_str(), (char*)myStr.c_str()));
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_Achievable_Data()
//
//! \brief
//!   Description: function for parsing out the achivable Latency target information
//
//  Entry:
//! \param fuaData - the Json node olding all the FUA data 
//! \param paramCode - passed in param code
//! \param offset - offset where the data came from
//
//  Exit:
//
//---------------------------------------------------------------------------
void CScsiCmdDurationLimitsLog::process_Achievable_Data(JSONNODE * achievableData, uint16_t paramCode, uint32_t offset)
{
    std::string myStr = "";
    myStr.resize(BASIC);
    std::string myHeader = "";
    myHeader.resize(BASIC);

#if defined_DEBUG
    printf("Achievable Latency Target Log Description\n");
#endif
    byte_Swap_16(&m_commandLog->paramCode);
    if (g_dataformat == PREPYTHON_DATA)
    {
        prePython_int(achievableData, "achievable_latency", "total read commands", "count", m_Value, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, paramCode, offset);

    }
    else
    {
        myHeader = "Achievable Latency Target";
        snprintf((char*)myStr.c_str(), BASIC, "%" PRIu64"", m_Value);
        json_push_back(achievableData, json_new_a((char*)myHeader.c_str(), (char*)myStr.c_str()));
    }
}
//-----------------------------------------------------------------------------
//
//! \fn process_Duration_Limits_Data()
//
//! \brief
//!   Description: function for parsing out the fua data for 0x0004 param
//
//  Entry:
//! \param fuaData - the Json node olding all the FUA data 
//! \param offset - offset where the data came from
//
//  Exit:
//
//---------------------------------------------------------------------------
void CScsiCmdDurationLimitsLog::process_Duration_Limits_Data(JSONNODE* limitData, uint32_t offset)
{
    std::string myStr = "";
    myStr.resize(BASIC);

#if defined_DEBUG
    printf("Command DurationLimits Log Description\n");
#endif
    byte_Swap_16(&m_limitsLog->paramCode);
    get_Parameter_Code_Description(m_limitsLog->paramCode, &myStr);

    byte_Swap_32(&m_limitsLog->activeMiss);
    byte_Swap_64(&m_limitsLog->inactiveMiss);
    byte_Swap_32(&m_limitsLog->latencyMiss);
    byte_Swap_32(&m_limitsLog->latencyMissesDeferredErrors);
    byte_Swap_32(&m_limitsLog->latencyMissesDoToError);
    byte_Swap_32(&m_limitsLog->missedDoToBackgroundOperations);
    byte_Swap_32(&m_limitsLog->nonconformingMiss);
    byte_Swap_32(&m_limitsLog->predictiveLatencyMiss);

    if (g_dataformat == PREPYTHON_DATA)
    {
        prePython_int(limitData, (char*)myStr.c_str(), "total inactive target miss", "commands", m_limitsLog->inactiveMiss, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total active target miss", "commands", m_limitsLog->activeMiss, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total latency miss", "commands", m_limitsLog->latencyMiss, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total noncoformin miss", "commands", m_limitsLog->nonconformingMiss, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total predictive latency miss", "commands", m_limitsLog->predictiveLatencyMiss, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total attributable to errors", "commands", m_limitsLog->latencyMissesDoToError, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total attributable to deferred errors", "commands", m_limitsLog->latencyMissesDeferredErrors, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);
        prePython_int(limitData, (char*)myStr.c_str(), "total attributable to background operations", "commands", m_limitsLog->missedDoToBackgroundOperations, COMMAND_DURATION_LIMITS_LOG, SAS_SUBPAGE_ZERO, m_limitsLog->paramCode, offset);

    }
    else
    {
        JSONNODE* limitInfo = json_new(JSON_NODE);
        json_set_name(limitInfo, (char*)myStr.c_str());

        set_json_64bit(limitInfo, "Number of Inactive Target Miss Commands", m_limitsLog->inactiveMiss, false);
        set_json_64bit(limitInfo, "Number of Write FUA commands", m_limitsLog->activeMiss, false);
        set_json_64bit(limitInfo, "Number of read FUA_NV commands", m_limitsLog->latencyMiss, false);
        set_json_64bit(limitInfo, "Number of write FUA_NV commands", m_limitsLog->nonconformingMiss, false);
        set_json_64bit(limitInfo, "Read FUA Commands Processing Itervals", m_limitsLog->predictiveLatencyMiss, false);
        set_json_64bit(limitInfo, "Write FUA Commands Processing Itervals", m_limitsLog->latencyMissesDoToError, false);
        set_json_64bit(limitInfo, "Read FUA_NV Commands Processing Itervals", m_limitsLog->latencyMissesDeferredErrors, false);
        set_json_64bit(limitInfo, "Read FUA_NV Commands Processing Itervals", m_limitsLog->missedDoToBackgroundOperations, false);

        json_push_back(limitData, limitInfo);
    }
}

void CScsiCmdDurationLimitsLog::populate_Generic_Param_Value(uint8_t paramLength, uint32_t offset)
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
//! \fn get_Limits_Data
//
//! \brief
//!   Description: parser out the data for the command duration limit
//
//  Entry:
//! \param masterData - Json node that holds all the data 
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiCmdDurationLimitsLog::get_Limits_Data(JSONNODE *masterData)
{

    std::string myStr = "";
    myStr.resize(BASIC);
    std::string headerStr = "";
    headerStr.resize(BASIC);
    eReturnValues retStatus = IN_PROGRESS;
    if (pData != NULL)
    {
        snprintf((char*)headerStr.c_str(), BASIC, "Command Duration Limits Log - 19h, 21h");
        JSONNODE* pageInfo;
        if (g_dataformat == PREPYTHON_DATA)
        {
            pageInfo = masterData;
        }
        else
        {
            pageInfo = json_new(JSON_NODE);
            json_set_name(pageInfo, (char*)headerStr.c_str());
        }
        for (uint32_t offset = 0; offset < m_PageLength; )
        {
            if (offset < m_bufferLength && offset < UINT16_MAX)
            {
                uint16_t paramCode = *(reinterpret_cast<uint16_t*>(&pData[offset]));
                byte_Swap_16(&paramCode);
                if (paramCode == 0x0001)
                {
                    m_commandLog = (sLogParams*)&pData[offset];
                    process_Achievable_Data(pageInfo, paramCode, offset);
                    offset += m_commandLog->paramLength + LOGPAGESIZE;
                }
                else if (paramCode >= 0x0011 && paramCode <= 0x0017)
                {

                    m_limitsLog = (sCommandDurationLimits*)&pData[offset];
                    process_Duration_Limits_Data(pageInfo, offset);
                    offset += m_limitsLog->paramLength + LOGPAGESIZE;
                }
                else if (paramCode >= 0x0021 && paramCode <= 0x0027)
                {

                    m_limitsLog = (sCommandDurationLimits*)&pData[offset];
                    process_Duration_Limits_Data(pageInfo, offset);
                    offset += m_limitsLog->paramLength + LOGPAGESIZE;
                }
                else
                {
                    m_commandLog = (sLogParams*)&pData[offset];
                    populate_Generic_Param_Value(m_commandLog->paramLength,offset + LOGPAGESIZE);
                    process_Generic_Data(pageInfo, paramCode,offset);
                    offset += (m_commandLog->paramLength + LOGPAGESIZE);
                }
            }
            else
            {
                if (g_dataformat != PREPYTHON_DATA)
                {
                    json_push_back(masterData, pageInfo);
                }
                return BAD_PARAMETER;
            }
        }
        if (g_dataformat != PREPYTHON_DATA)
        {
            json_push_back(masterData, pageInfo);
        }
        retStatus = SUCCESS;
    }
    else
    {
        retStatus = MEMORY_FAILURE;
    }
    return retStatus;
}
