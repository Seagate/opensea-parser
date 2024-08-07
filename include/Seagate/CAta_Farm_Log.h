//
// CAta_Farm_Log.h
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

// \file CAta_Farm_Log.h
// \brief Defines the function calls and structures for parsing Seagate logs
#pragma once
#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>
#include "common_types.h"
#include "libjson.h"
#include "Farm_Helper.h"
#include "Farm_Common.h"
#include "Ata_Farm_Types.h"
#include <sstream>

namespace opensea_parser {

#ifndef ATAFARM
#define ATAFARM

    class CATA_Farm_Log : public CFarmCommon
    {
        protected:

            std::vector <sFarmFrame > vFarmFrame;
            std::vector <sFarmFrame >vBlankFarmFrame;

            uint64_t                    m_totalPages;                       //!< number of pages supported
            uint64_t                    m_logSize;                          //!< log size in bytes
            uint64_t                    m_pageSize;                         //!< page size in bytes
            uint64_t                    m_heads;                            //!< number of heads - first fill in with header information then changed to heads reported on the drive
			uint64_t					m_MaxHeads;							//!< Maximum Drive Heads Supported
            uint64_t                    m_copies;                           //!< Number of Historical Copies  
            eReturnValues               m_status;                           //!< status of the class
			bool						m_showStatusBits;					//!< show the status bits of each entry
            sFarmHeader                 *m_pHeader;                         //!< Member pointer to the header of the farm log  
            uint8_t                     *pBuf;                              //!< pointer to the buffer data that is the binary of FARM LOG
            uint32_t                    m_MajorRev;                         //!< holds the Major Revision number
            uint32_t                    m_MinorRev;                         //!< holds the minor revision number
            uint8_t                     m_FrameReason;                      //!< holds the reason for Frame Capture information
            bool                        m_ShowAct1;                         //!< set to true to show the acutator 1 information
            bool                        m_showStatic;                       //!< set to true to show all data statically 
 
            bool Check_Page_number(uint64_t page, uint16_t pageNumber);
            eReturnValues print_Header(JSONNODE *masterData);
            eReturnValues print_Drive_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Work_Load(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Error_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Enviroment_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Reli_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Head_Information(JSONNODE *masterData, uint32_t page);
            
        public:
            CATA_Farm_Log();
            CATA_Farm_Log( uint8_t *bufferData, size_t bufferSize, bool showStatus, bool showStatic);
            virtual ~CATA_Farm_Log();
            eReturnValues parse_Farm_Log();
            //void get_Reallocated_Sector_By_Cause(std::string *description, uint64_t readWriteRetry);
            void print_All_Pages(JSONNODE *masterData);
            void print_Page(JSONNODE *masterData, uint32_t page);
            void print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page);
            void print_Page_One_Node(JSONNODE *masterData);
            virtual eReturnValues get_Log_Status(){ return m_status; };
            virtual void get_Serial_Number(std::string sn){ sn.assign( vFarmFrame.at(0).identStringInfo.serialNumber); };
            virtual void get_Firmware_String(std::string firmware){ firmware.assign(vFarmFrame.at(0).identStringInfo.firmwareRev); };
			virtual void get_World_Wide_Name(std::string wwn) {wwn.assign(vFarmFrame.at(0).identStringInfo.worldWideName);};
            uint8_t get_FrameReason() { return m_FrameReason; };
    };
#endif //!ATAFARM
}
  

