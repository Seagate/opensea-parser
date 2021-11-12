// CScsi_Zoned_Device_Statistics_Log.h  Definition of Zoned Device Statistics Log
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Zoned_Device_Statistics_Log.h  Definition of Zoned Device Statistics Log
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSIZONEDDEVICESTATISTICSLOG
#define SCSIZONEDDEVICESTATISTICSLOG

#pragma pack(push, 1)
    typedef struct _sZDSParameters
    {
        uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
        uint8_t			paramControlByte;					//<! binary format list log parameter
        uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
        _sZDSParameters() : paramCode(0), paramControlByte(0), paramLength(0) {};
    } sZDSParams;

#pragma pack(pop)
    class CScsiZonedDeviceStatisticsLog
    {
    private:
    protected:
        uint8_t						*pData;					//<! pointer to the data
        std::string					m_ZDSName;				//<! class name	
        eReturnValues				m_ZDSStatus;			//<! status of the class
        uint16_t					m_PageLength;			//<! length of the page
        size_t						m_bufferLength;			//<! length of the buffer from reading in the log
        uint64_t					m_ZDSValue;				//<! Parameter ZDS Value
        sZDSParams				    *m_ZDSParam;			//<! ZDS param structure 

        bool get_ZDS_Parameter_Code_Description( std::string *zdsString);
        void process_Zoned_Device_Statistics_Data(JSONNODE *zdsData);
        eReturnValues get_Zoned_Device_Statistics_Data(JSONNODE *masterData);
    public:
        CScsiZonedDeviceStatisticsLog();
        CScsiZonedDeviceStatisticsLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
        virtual ~CScsiZonedDeviceStatisticsLog();
        virtual eReturnValues get_Zoned_Device_Statistics_Log_Status() { return m_ZDSStatus; };
        virtual eReturnValues parse_Zoned_Device_Statistics_Log(JSONNODE *masterData) { return get_Zoned_Device_Statistics_Data(masterData); };
    };
#endif
}