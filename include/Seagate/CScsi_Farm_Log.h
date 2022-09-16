//
// CScsi_Farm_Log.h
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

// \file CScsi_Farm_Log.h
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
#include "Scsi_Farm_Types.h"

namespace opensea_parser {
#ifndef SCSIFARM
#define SCSIFARM

#define FARMLOGPAGE 0x3D
#define FARMSUBPAGE 0x03
#define FARMSUBPAGEFACTORY 0x04

#define FARM_LOG_PAGE                   0x03
#define FARM_FACTORY_LOG_PAGE           0x04
#define FARM_TIME_SERIES_0              0x10
#define FARM_TIME_SERIES_1              0x11
#define FARM_TIME_SERIES_2              0x12
#define FARM_TIME_SERIES_3              0x13
#define FARM_TIME_SERIES_4              0x14
#define FARM_TIME_SERIES_5              0x15
#define FARM_TIME_SERIES_6              0x16
#define FARM_TIME_SERIES_7              0x17
#define FARM_TIME_SERIES_8              0x18
#define FARM_TIME_SERIES_9              0x19
#define FARM_TIME_SERIES_10             0x1A
#define FARM_TIME_SERIES_11             0x1B
#define FARM_TIME_SERIES_12             0x1C
#define FARM_TIME_SERIES_13             0x1D
#define FARM_TIME_SERIES_14             0x1E
#define FARM_TIME_SERIES_15             0x1F
#define FARM_LONG_TERM_SAVES_0          0xC0
#define FARM_LONG_TERM_SAVES_1          0xC1
#define FARM_GLIST_ENTRIES              0xC2
#define FARM_FIRST_UNRECOVERABLE        0xC3
#define FARM_TENTH_UNRECOVERABLE        0xC4
#define FARM_FIRST_CTO                  0xC5
#define FARM_LAST_FRAME                 0xC6
#define FARM_TEMP_TRIGGER_LOG_PAGE      0xC7

    class CSCSI_Farm_Log : public CFarmCommon
    {
    protected:

        std::vector <sScsiFarmFrame > vFarmFrame;
   
        uint32_t                    m_totalPages;                                     //!< number of pages supported
		uint16_t                    m_logSize;                                        //!< log size in bytes
        uint32_t                    m_pageSize;                                       //!< page size in bytes
		uint64_t                    m_heads;										  //!< number of heads - first fill in with header information then changed to heads reported on the drive
		uint64_t					m_MaxHeads;										  //!< Maximum Drive Heads Supported
        uint32_t                    m_copies;                                         //!< Number of Historical Copies
        uint32_t                    m_MajorRev;                                       //!< holds the Major Revision number
		uint32_t					m_MinorRev;										  //!< minor rev saved off to pick up the changes in the spec
        uint8_t                     *pBuf;                                            //!< pointer to the buffer data that is the binary of FARM LOG
        eReturnValues               m_status;                                         //!< status of the class	
		sScsiLogParameter			*m_logParam;                                      //!< pointer to the log page param for all of the log
        sScsiPageParameter          *m_pageParam;                                     //!< pointer to the page parameters 
        sScsiFarmHeader				*m_pHeader;										  //!< Member pointer to the header of the farm log
        sScsiDriveInfo              *m_pDriveInfo;                                    //!< Drive information pointer with header information
        bool                        m_alreadySet;                                     //!< set true one it's already set..  (APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER)
		bool						m_showStatusBits;								  //!< show the status bits of each entry
        bool                        m_fromScsiLogPages;                               //!< bool if passed from scsi log pages set as true. We will be off by 4 bytes
        uint8_t                     m_farmSubPage;                                    //!< the subpage for the farm so we know which one it is

		
        
        bool strip_Active_Status(uint64_t *value);
        bool swap_Bytes_sFarmHeader(sScsiFarmHeader *fh, uint8_t* pData);
        bool swap_Bytes_sDriveInfo(sScsiDriveInfo *di);
        bool swap_Bytes_sDrive_Info_Page_06(sGeneralDriveInfoPage06 *gd);
        bool swap_Bytes_sWorkLoadStat(sScsiWorkLoadStat *wl);
        bool swap_Bytes_sErrorStat(sScsiErrorFrame * es);
        bool swap_Bytes_sEnvironmentStat(sScsiEnvironmentStat *es);
        bool swap_Bytes_EnvironmentPage07(sScsiEnvStatPage07 *ep);
        bool swap_Bytes_WorkloadPage08(sScsiWorkloadStatPage08 *ep);
        bool swap_Bytes_sScsiReliabilityStat(sScsiReliablility *ss);
        bool swap_Bytes_sLUNStruct(sLUNStruct *LUN);
        bool swap_Bytes_Flash_LED(sActuatorFLEDInfo *fled);
        bool swap_Bytes_Reallocation_Data(sActReallocationData *real);
        bool get_Head_Info(sHeadInformation *phead, uint8_t *buffer);
        bool set_Head_Header(std::string &headerName, eLogPageTypes index);
        void get_LUN_Info(sLUNStruct *pLUN, uint8_t *buffer);
		
		eReturnValues init_Header_Data();
        //eReturnValues init_buffer_Header_Data();
        eReturnValues print_Header(JSONNODE *masterData);
        eReturnValues print_Drive_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_General_Drive_Information_Continued(JSONNODE *masterData, uint32_t page);
        eReturnValues print_WorkLoad(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Error_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Error_Information_Version_4(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Enviroment_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Enviroment_Statistics_Page_07(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Workload_Statistics_Page_08(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Reli_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Head_Information(eLogPageTypes type, JSONNODE *masterData, uint32_t page);
        eReturnValues print_LUN_Actuator_Information(JSONNODE * LUNData, uint32_t page, uint16_t actNum);
        eReturnValues print_LUN_Actuator_FLED_Info(JSONNODE * LUNFLED, uint32_t page, uint16_t actNum);
        eReturnValues print_LUN_Actuator_Reallocation(JSONNODE * LUNNReallocation, uint32_t page, uint16_t actNum);
        void farm_PrePython_Str(JSONNODE* masterData, const char* name, const char* statType, const char* unit, const char* location, int pageNum, const char* value);
        void farm_PrePython_Int(JSONNODE* masterData, const char* name, const char* statType, const char* header, \
            const char* unit, int pageNum, int64_t value);
        void farm_PrePython_Float(JSONNODE* masterData, const char* name, const char* statType, const char* header, \
            const char* unit, int pageNum, double value);
        void prePython_Head_Float(JSONNODE* masterData, const char* name, const char* statType, uint32_t headNumber, \
            const char* unit, int pageNum, double value);
        void prePython_Head_Int(JSONNODE* masterData, const char* name, const char* statType, uint32_t headNumber, \
            const char* unit, int pageNum, int64_t value);
    public:
        CSCSI_Farm_Log();
        CSCSI_Farm_Log(uint8_t* bufferData, size_t bufferSize, uint8_t subPage, bool m_fromScsiLogPages);
        CSCSI_Farm_Log(uint8_t *bufferData, size_t bufferSize, uint8_t subpage, bool m_fromScsiLogPages, bool showStatus);
        virtual ~CSCSI_Farm_Log();
        eReturnValues parse_Farm_Log();
        void print_All_Pages(JSONNODE *masterData);
        void print_Page(JSONNODE *masterData, uint32_t page);
        void print_Page_One_Node(JSONNODE *masterData);
        void print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page);
        virtual eReturnValues get_Log_Status(){ return m_status; };
    };
#endif // !SCSIFARM
}
