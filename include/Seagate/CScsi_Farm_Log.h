//
// CScsi_Farm_Log.h
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

// \file CScsi_Farm_Log.h
// \brief Defines the function calls and structures for pulling Seagate logs
#pragma once
#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>
#include "common.h"
#include "libjson.h"
//#include "Opensea_Parser_Helper.h"
#include "Scsi_Farm_Types.h"

namespace opensea_parser {
#ifndef SCSIFARM
#define SCSIFARM

    class CSCSI_Farm_Log 
    {
    protected:
#define FLASHEVENTS  8
#define REALLOCATIONEVENTS 15

        const int assertCodes[70] = { UNKNOWN, MICROPROCESSOR_FAILED, DRAM_FAILED_POWERUP_OR_WRAM_FAIL, SCC_FAILED_POWERUP_DIAGNOSTICS,
            FW_DOES_NOT_MATCH_THE_SCC_VERSION, UNIMPLEMENTED_OPCODE_INTERRUPT, POWER_UP_XOR_FAILURE_FOR_FIBER_CH,EEPROM_VERIFY_ERROR_EVEN_BYTE,
            EEPROM_ERASE_ERROR_EVEN_BYTE,DOWNLOAD_TPM_FAILED_0,DOWNLOAD_TPM_FAILED_1,DOWNLOAD_TPM_FAILED_2,DOWNLOAD_TPM_FAILED_3,
            DOWNLOAD_TPM_FAILED_4,DOWNLOAD_TPM_FAILED_5,DOWNLOAD_TPM_FAILED_6,DOWNLOAD_TPM_FAILED_7,DOWNLOAD_TPM_FAILED_8,DOWNLOAD_VOLTAGE_FAULT,
            FAILS_WRITING_ARRAY_DATA_TO_FLASH_0,FLASH_LOOKING_FOR_MEMORY_RANGE_ERROR,FAILS_WRITING_ARRAY_DATA_TO_FLASH_1,FAILS_WRITING_ARRAY_DATA_TO_FLASH_2,
            FAILS_WRITING_ARRAY_DATA_TO_FLASH_3,FAILS_WRITING_ARRAY_DATA_TO_FLASH_4,FAILS_WRITING_ARRAY_DATA_TO_FLASH_5,ALU_BUFFER_PARITY_ERROR,
            PREFETCH_TCM_ECC_ERROR,ERROR_INJECTION_ASSERT,DRAM_CONFIGURATION_PROCESS_FAILED,FDE_BUS_PARITY_ERROR,PREFETCH_VECTOR_OR_STACK_POINTER_OUT,
            ERROR_IN_WRITING_TO_READ_CHIP,IER_STACK_OVERFLOW,IER_STACK_UNDERFLOW,IER_STACK_NOT_EMPTY_ON_ENTRY_TO_SLEEP,IRAW_HAD_MISCOMPARE,
            UNDEFINED_INSTRUCTION,LOGGING_SAVE_FAILED_EXCEEDED_ALLOCATED,CANT_FIND_BACKPLANE_DATA_RATE,CONTROLLER_I_TCM_DOUBLE_BIT_ECC_ERROR,
            CONTROLLER_D_TCM_DOUBLE_BIT_ECC_ERROR,SERVO_I_TCM_DOUBLE_BIT_ECC_ERROR,SERVO_D_TCM_DOUBLE_BIT_ECC_ERROR,CDPRAM_UNRECOVERABLE_ERROR,
            SDPRAM_UNRECOVERABLE_ERROR,TCM_CRC_RESULT_IS_NON_ZERO,SWI_ASSERT_FLASH_CODE_BOOT,SWI_ASSERT_FLASH_CODE_NQNR,SWI_ASSERT_FLASH_CODE_DISC,
            REMOTE_ASSERT,DRAM_INTEGRITY_FAILURE,CLOCK_FAILURE,ASSERT_FLASH_CODE,ENSURE_FLASH_CODE,REQUIRE_FLASH_CODE,SMART_FLASH_CODE,SCSI_UNEXEPCTED_INTERRUPT,
            SCSI_TIMEOUT,ILLEGAL_STATUS_CODE,SCSI_UNDER_OVER_RUN_OCCURRED,UNEXPECTED_STATUS,DIVIDE_BY_ZERO_INTERRUPT,DATA_ABORT_CACHE_ECC_ERROR,
            DATA_ABORT_TCM_ECC_ERROR,ABORT_INTERRUPT,SELF_SEEK_FAILURE,CONTROLLER_NUKED_BY_FDE,FLASH_IOEDC_PARITY_ERROR,SERIAL_PORT_DUMP_MODE };

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

		void create_Serial_Number(std::string &serialNumber, const sScsiDriveInfo * const idInfo);
		void create_World_Wide_Name(std::string &worldWideName, const sScsiDriveInfo * const idInfo);
		void create_Firmware_String(std::string &firmwareRev, const sScsiDriveInfo * const idInfo);
		void create_Device_Interface_String(std::string &dInterface, const sScsiDriveInfo * const idInfo);
        void create_Model_Number_String(std::string &model, sGeneralDriveInfoPage06 * const idInfo);
        void create_Year_Assembled_String(std::string &dateStr, uint16_t *date);
        bool strip_Active_Status(uint64_t *value);
        bool swap_Bytes_sFarmHeader(sScsiFarmHeader *fh);
        bool swap_Bytes_sDriveInfo(sScsiDriveInfo *di);
        bool swap_Bytes_sDrive_Info_Page_06(sGeneralDriveInfoPage06 *gd);
        bool swap_Bytes_sWorkLoadStat(sScsiWorkLoadStat *wl);
        bool swap_Bytes_sErrorStat(sScsiErrorFrame * es);
        bool swap_Bytes_sEnvironmentStat(sScsiEnvironmentStat *es);
        bool swap_Bytes_EnvironmentPage07(sScsiEnvStatPage07 *ep);
        bool swap_Bytes_sScsiReliabilityStat(sScsiReliablility *ss);
        bool swap_Bytes_sLUNStruct(sLUNStruct *LUN);
        bool swap_Bytes_Flash_LED(sActuatorFLEDInfo *fled);
        bool swap_Bytes_Reallocation_Data(sActReallocationData *real);
        bool get_Head_Info(sHeadInformation *phead, uint8_t *buffer);
        void set_Head_Header(std::string &headerName, eLogPageTypes index);
        void get_LUN_Info(sLUNStruct *pLUN, uint8_t *buffer);
        void get_Assert_Code_Meaning(std::string &meaning, uint16_t code );
        void get_Reallocation_Cause_Meanings(std::string &meaning, uint16_t code);
		
		eReturnValues init_Header_Data();
        eReturnValues print_Header(JSONNODE *masterData);
        eReturnValues print_Drive_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_General_Drive_Information_Continued(JSONNODE *masterData, uint32_t page);
        eReturnValues print_WorkLoad(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Error_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Error_Information_Version_4(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Enviroment_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Enviroment_Statistics_Page_07(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Reli_Information(JSONNODE *masterData, uint32_t page);
        eReturnValues print_Head_Information(eLogPageTypes type, JSONNODE *masterData, uint32_t page);
        eReturnValues print_LUN_Actuator_Information(JSONNODE *masterData, uint32_t page, uint32_t index, uint16_t actNum);
        eReturnValues print_LUN_Actuator_FLED_Info(JSONNODE *masterData, uint32_t page, uint32_t index, uint16_t actNum);
        eReturnValues print_LUN_Actuator_Reallocation(JSONNODE *masterData, uint32_t page, uint32_t index, uint16_t actNum);
    public:
        CSCSI_Farm_Log();
        CSCSI_Farm_Log(uint8_t *bufferData, size_t bufferSize,bool showStatus);
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
