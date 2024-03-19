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
#define SIZEPARAM   8

#ifndef SCSIFARM
#define SCSIFARM

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
        sLogPageStruct              *m_logParam;                                      //!< pointer to the log page param for all of the log
        sLogParams                  *m_pageParam;                                     //!< pointer to the page parameters 
        sScsiFarmHeader             *m_pHeader;										  //!< Member pointer to the header of the farm log
        sScsiDriveInfo              *m_pDriveInfo;                                    //!< Drive information pointer with header information
        bool                        m_alreadySet;                                     //!< set true one it's already set..  (APPLIED_FLY_HEIGHT_CLEARANCE_DELTA_PER_HEAD_IN_THOUSANDTHS_OF_ONE_ANGSTROM_OUTER)
		bool						m_showStatusBits;								  //!< show the status bits of each entry
        bool                        m_fromScsiLogPages;                               //!< bool if passed from scsi log pages set as true. We will be off by 4 bytes
        uint8_t                     m_farmSubPage;                                    //!< the subpage for the farm so we know which one it is

		
        
        
        bool get_sFarmHeader(sScsiFarmHeader* fh, uint8_t* pData, uint64_t position);
        bool Get_sDriveInfo(sScsiDriveInfo *di, uint64_t offset);
        bool Get_sDrive_Info_Page_06(sGeneralDriveInfoPage06 *gd,uint64_t offset);
        bool Get_sWorkLoadStat(sScsiWorkLoadStat *wl, uint64_t offset);
        bool Get_sErrorStat(sScsiErrorFrame *es, uint64_t offset);
        bool Get_sEnvironmentStat(sScsiEnvironmentStat *es,uint64_t offset);
        bool Get_EnvironmentPage07(sScsiEnvStatPage07 *ep,uint64_t offset);
        bool Get_WorkloadPage08(sScsiWorkloadStatPage08 *ep,uint64_t offset);
        bool Get_sScsiReliabilityStat(sScsiReliablility *ss,uint64_t offset);
        bool Get_sLUNStruct(sLUNStruct *LUN,uint64_t offset);
        bool Get_Flash_LED(sActuatorFLEDInfo *fled, uint64_t offset);
        bool Get_Reallocation_Data(sActReallocationData *real, uint64_t offset);
        bool get_Head_Info(sHeadInformation *phead, uint8_t *buffer);
        bool set_Head_Header(std::string &headerName, eSASLogPageTypes index);
		
		eReturnValues init_Header_Data();
        eReturnValues print_Header(JSONNODE *masterData);
        eReturnValues print_Drive_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues get_Regen_Head_Mask(JSONNODE* headMask, uint64_t mask);
        eReturnValues print_General_Drive_Information_Continued(JSONNODE *masterData, uint32_t page);
        eReturnValues print_WorkLoad(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Error_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Enviroment_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Enviroment_Statistics_Page_07(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Workload_Statistics_Page_08(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Reli_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Head_Information(eSASLogPageTypes type, JSONNODE *masterData, uint32_t page);
        eReturnValues print_LUN_Actuator_Information(JSONNODE * LUNData, uint32_t page, uint16_t actNum);
        eReturnValues print_LUN_Actuator_FLED_Info(JSONNODE * LUNFLED, uint32_t page, uint16_t actNum);
        eReturnValues print_LUN_Actuator_Reallocation(JSONNODE * LUNNReallocation, uint32_t page, uint16_t actNum);
  
    public:
        CSCSI_Farm_Log();
        CSCSI_Farm_Log(uint8_t* bufferData, size_t bufferSize, uint8_t subPage, bool m_fromScsiLogPages);
        CSCSI_Farm_Log(uint8_t *bufferData, size_t bufferSize, uint8_t subpage, bool m_fromScsiLogPages, bool showStatus);
        virtual ~CSCSI_Farm_Log();
        bool strip_Active_Status(uint64_t* value);
        eReturnValues parse_Farm_Log();
        void print_All_Pages(JSONNODE *masterData);
        //void print_Page(JSONNODE *masterData, uint32_t page);
        void print_Page_One_Node(JSONNODE *masterData);
        void print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page);
        virtual eReturnValues get_Log_Status(){ return m_status; };

        uint32_t get_LogSize() { return m_logSize;};                                  //<! return the page size for the combine farm log
    };
#endif // !SCSIFARM
}
