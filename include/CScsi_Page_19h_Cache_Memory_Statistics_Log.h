// CScsi_Page_19h_Cache_Memory_Statistics_Log.h  Definition of SCSI 0x19,20 cache memory statistics
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Page_19h_Cache_Memory_Statistics_Log.h  Definition of SCSI cache memory Statistics Page
#pragma once
#include <string>
#include <vector>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSICACHMEMSTATLOG
#define SCSICACHMEMSTATLOG

    class CScsiCacheMemStatLog
    {
    private:
    protected:

#pragma pack(push, 1)

        //Time From Last Hard Reset
        typedef struct _sTimeIntervalDescriptors
        {
            uint16_t  paramCode;					        // Paramcode
            uint8_t	  controlByte;					        // Controlbyte
            uint8_t	  paramLength;                          // Param Length
            int32_t  intervalExponent;                     // Number of Reads
            uint32_t  intervalInteger;                      // Number of Writes            
        } sTimeIntervalDescriptors;

#pragma pack(pop)

        uint8_t						            *pData;						    //<! pointer to the data
        std::string					            m_CacheMemName;             	//<! class name	
        eReturnValues				            m_LogStatus;			        //<! status of the class
        uint16_t					            m_PageLength;				    //<! length of the page
        size_t						            m_bufferLength;			        //<! length of the buffer from reading in the log
        sLogParams                              *m_CacheMemLog;
        sTimeIntervalDescriptors                *m_TimeIntervalDescriptorParam;
        uint64_t                                m_Value;

        void get_Parameter_Code_Description(uint16_t paramCode, std::string *performanceStatistics);
        void process_Cache_Memory_Statistics_interval_Data(JSONNODE *cacheData);
        void process_Generic_Data(JSONNODE *genData);
        void populate_Generic_Param_Value(uint8_t paramLength, uint32_t offset);
        eReturnValues get_Cache_Memory_Statistics_Data(JSONNODE *masterData);

    public:
        CScsiCacheMemStatLog();
        CScsiCacheMemStatLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
        virtual ~CScsiCacheMemStatLog();
        virtual eReturnValues get_Cache_Memory_Statistics_Log_Status() { return m_LogStatus; };
        virtual eReturnValues parse_Cache_Memory_Statistics_Log(JSONNODE *masterData) { return get_Cache_Memory_Statistics_Data(masterData); };
    };
#endif
}
