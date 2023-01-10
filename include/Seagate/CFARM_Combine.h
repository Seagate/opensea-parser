//
// CFarm_Combine.h
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CFarm_Combine.h
// \brief Defines the function calls and structures for parsing Seagate logs
#pragma once
#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>
#include "common.h"
#include "libjson.h"
#include "Farm_Helper.h"
#include "Farm_Common.h"
#include "CAta_Farm_Log.h"
#include "CScsi_Farm_Log.h"
#include "CFARM_WLM_Log.h"
#include <sstream>

namespace opensea_parser {

#ifndef FARMCOMBINE
#define FARMCOMBINE

#define STXFARM     0x5354585f
#define STX_FARM    0x5354585f4641524d
#define STX_COMBO   0x5f434f4d42494e45
#define FARMSASCOMBO   0x53415320  
#define FARMSAS     0x53415300
#define FARMSATA    0x53415441
//#define MAXFARMLOGSIZE  0x2680
//#define MAXFARMATALOGSIZ 0x18200
#define ATALOGSIZE   0x18000
#define EMPTYLOGSIZE 0xffffffff
#define PADDINGSIZE  0x00000000

    class CFarm_Combine : public CFarmCommon
    {
    protected:
        uint8_t*                    bufferData;						     //!< pointer to the buffer	
        size_t						m_LogSize;							 //!< size of the log
        eReturnValues               m_status;                            //!< status of the class
        bool                        m_isScsi;                            //!< true if the log is Scsi
        bool                        m_combine_isScsi;                    //!< true if the combine log is a scsi log
        bool                        m_isComboLog;                        //!< true if the log is a combine log
        bool						m_showStatus;						 //!< if true then we will show all the status bits for each entry
        uint64_t                    m_version;                           //!< version number of the FARM Combined log
        size_t                      m_maxLogSize;                        //!< max log size from  the current log size
        std::vector<sComboDataSet>  vdataSetInfo;                        //!< vector holding the header data for each FARM Log


        void setBuffer(uint8_t* buffer) { bufferData = buffer; };
        void setLogSize(size_t bufferSize) { m_LogSize = bufferSize; };
        void combine_Device_Scsi();
        void is_Device_Scsi();
        void is_Combo_Log();

        uint64_t get_Log_Size(size_t offset);
        void get_FARM_Type(std::string* reason, uint64_t dataType);
        void get_Header_Info(sStringIdentifyData* headerInfo);
        void get_Data_Set(uint16_t DataSetNumber);
        bool is_Subpage_Neeeded(uint64_t dataType);
        void get_Sticky_Frame_Type(std::string* reason, uint8_t frame);
        void parse_FARM_Logs(size_t offset, size_t logSize, uint64_t dataType, JSONNODE* farmJson);
        void print_Combine_Log_Header(sStringIdentifyData* headerInfo, JSONNODE* header);
        void print_Header_Debug( JSONNODE* labelJson);
       
        

    public:
        CFarm_Combine();
        explicit CFarm_Combine(bool showStatus);
        CFarm_Combine(uint8_t* buffer, size_t bufferSize, bool showStatus);
        virtual ~CFarm_Combine();
        eReturnValues get_Combo_Status() { return m_status; };
        void setCombine(uint8_t* buffer, size_t bufferSize);
        bool get_showStatus() { return m_showStatus; };
        bool get_IsScsi() { return m_isScsi; };
        bool get_Combo_IsScsi() {  return m_combine_isScsi; };
        bool getIsCombo() {  return  m_isComboLog; };
        eReturnValues combo_Parsing(JSONNODE* masterJson);
    };
#endif
}
