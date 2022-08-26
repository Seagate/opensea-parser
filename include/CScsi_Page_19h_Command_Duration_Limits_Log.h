// CScsi_Page_19h_Command_Duration_Limits_Log.h  Definition of the command Duration limits log
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Page_19h_Command_Duration_Limits_Log.h  
#pragma once
#include <string>
#include <vector>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"


namespace opensea_parser {
#ifndef SCSICOMMANDDURATIONLIMITSLOG
#define SCSICOMMANDDURATIONLIMITSLOG

    class CScsiCmdDurationLimitsLog
    {
    private:
    protected:
#define PARAMLENGTH 4
#pragma pack(push, 1)

        typedef struct _sCommandDurationLimitsParameter
        {
            uint16_t  paramCode;					        // Paramcode
            uint8_t	  controlByte;					        // Controlbyte
            uint8_t	  paramLength;                          // Param Length
            uint32_t  inactiveMiss;                         // NUMBER OF INACTIVE TARGET MISS COMMANDS
            uint32_t  activeMiss;                           // NUMBER OF ACTIVE TARGET MISS COMMANDS
            uint32_t  latencyMiss;                          // NUMBER OF LATENCY MISS COMMANDS
            uint32_t  nonconformingMiss;                    // NUMBER OF NONCONFORMING MISS COMMANDS
            uint32_t  predictiveLatencyMiss;                // NUMBER OF PREDICTIVE LATENCY MISS COMMANDS
            uint32_t  latencyMissesDoToError;               // NUMBER OF LATENCY MISSES ATTRIBUTABLE TO ERRORS
            uint32_t  latencyMissesDeferredErrors;          // NUMBER LATENCY MISSES ATTRIBUTABLE TO DEFERRED ERRORS
            uint32_t  missedDoToBackgroundOperations;       // NUMBER OF LATENCY MISSES ATTRIBUTABLE TO BACKGROUND OPERATIONS
        } sCommandDurationLimits;


#pragma pack(pop)

        uint8_t						            *pData;						    //<! pointer to the data
        std::string					            m_CMDLimitsName;	            //<! class name
        eReturnValues				            m_LogStatus;			        //<! status of the class
        uint16_t					            m_PageLength;				    //<! length of the page
        size_t						            m_bufferLength;			        //<! length of the buffer from reading in the log
        sLogParams				                *m_commandLog;                  //<! read commands
        sCommandDurationLimits                  *m_limitsLog;                   //<! command duration limits params
        uint64_t                                m_Value;

        void get_Parameter_Code_Description(uint16_t paramCode, std::string *generalStr);
        void process_Generic_Data(JSONNODE* genericData);
        void process_Achievable_Data(JSONNODE * achievableData);
        void process_Duration_Limits_Data(JSONNODE* limitData);
        void populate_Generic_Param_Value(uint8_t paramLength, uint32_t offset);
        eReturnValues get_Limits_Data(JSONNODE *masterData);

    public:
        CScsiCmdDurationLimitsLog();
        CScsiCmdDurationLimitsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
        virtual ~CScsiCmdDurationLimitsLog();
        virtual eReturnValues get_Limits_Log_Status() { return m_LogStatus; };
        virtual eReturnValues parse_Limits_Log(JSONNODE *masterData) { return get_Limits_Data(masterData); };
    };
#endif
}
