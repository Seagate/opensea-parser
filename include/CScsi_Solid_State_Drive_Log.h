// CScsi_Solid_State_Drive_Log.h  Definition of Solid State Drive Log
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Solid_State_Drive_Log.h  Definition of Solid State Drive Log
#pragma once
#include <string>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef SCSISOLIDSTATEDRIVELOG
#define SCSISOLIDSTATEDRIVELOG

#pragma pack(push, 1)
    typedef struct _sSSDParameters
    {
        uint16_t		paramCode;							//<! The PARAMETER CODE field is defined
        uint8_t			paramControlByte;					//<! binary format list log parameter
        uint8_t			paramLength;						//<! The PARAMETER LENGTH field 
        _sSSDParameters() : paramCode(0), paramControlByte(0), paramLength(0) {};
    } sSSDParams;

#pragma pack(pop)
    class CScsiSolidStateDriveLog
    {
    private:
    protected:
        uint8_t						*pData;					//<! pointer to the data
        std::string					m_SSDName;				//<! class name	
        eReturnValues				m_SSDStatus;			//<! status of the class
        uint16_t					m_PageLength;			//<! length of the page
        size_t						m_bufferLength;			//<! length of the buffer from reading in the log
        uint64_t					m_SSDValue;				//<! Parameter SSD Value
        sSSDParams				    *m_SSDParam;			//<! SSD param structure 

        bool get_SSD_Parameter_Code_Description(std::string *ssdString);
        void process_Solid_State_Drive_Data(JSONNODE *ssdData);
        eReturnValues get_Solid_State_Drive_Data(JSONNODE *masterData);
    public:
        CScsiSolidStateDriveLog();
        CScsiSolidStateDriveLog(uint8_t * buffer, size_t bufferSize, uint16_t pageLength);
        virtual ~CScsiSolidStateDriveLog();
        virtual eReturnValues get_Solid_State_Drive_Log_Status() { return m_SSDStatus; };
        virtual eReturnValues parse_Solid_State_Drive_Log(JSONNODE *masterData) { return get_Solid_State_Drive_Data(masterData); };
    };
#endif
}