//
// CAta_Farm_Log.h
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CAta_Farm_Log.h
// \brief Defines the function calls and structures for pulling Seagate logs
#pragma once
#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>
#include "common.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"
#include "Ata_Farm_Types.h"

namespace opensea_parser {

#ifndef ATAFARM
#define ATAFARM

    class CATA_Farm_Log 
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

            eReturnValues print_Header(JSONNODE *masterData);
            eReturnValues print_Drive_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Work_Load(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Error_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Enviroment_Information(JSONNODE *masterData, uint32_t page);
            eReturnValues print_Reli_Information(JSONNODE *masterData, uint32_t page);
			//-----------------------------------------------------------------------------
			//
			//! \fn create_Serial_Number()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit seiral number values and create a string serial number
			//
			//  Entry:
			//! \param serialNumber - string for holding the serial number of the drive ( putting it all together)
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return serialNumber = the string serialNumber
			//
			//---------------------------------------------------------------------------
			inline void create_Serial_Number(std::string *serialNumber, const sDriveInfo * const idInfo)
			{
				uint64_t sn = 0;
				sn = (idInfo->serialNumber & 0x00FFFFFFFFFFFFFFLL) | ((idInfo->serialNumber2 & 0x00FFFFFFFFFFFFFFLL) << 32);
				serialNumber->resize(SERIAL_NUMBER_LEN);
				memset((char*)serialNumber->c_str(), 0, SERIAL_NUMBER_LEN);
				strncpy((char *)serialNumber->c_str(), (char*)&sn, SERIAL_NUMBER_LEN);
				byte_Swap_String((char *)serialNumber->c_str());
			}
			//-----------------------------------------------------------------------------
			//
			//! \fn create_World_Wide_Name()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit world wide name values and create a string world wide name 
			//
			//  Entry:
			//! \param worldwideName - string to hold the world wide name ... putting it all together
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return wordWideName = the string wordWideName
			//
			//---------------------------------------------------------------------------
			inline void create_World_Wide_Name(std::string *worldWideName, const sDriveInfo * const idInfo)
			{
				uint64_t wwn = 0;
				uint64_t wwn1 = idInfo->worldWideName2 & 0x00FFFFFFFFFFFFFFLL;
				uint64_t wwn2 = idInfo->worldWideName & 0x00FFFFFFFFFFFFFFLL;
				word_Swap_64(&wwn1);
				word_Swap_64(&wwn2);
				wwn = (wwn2) | ((wwn1) >> 32);
				worldWideName->resize(WORLD_WIDE_NAME_LEN);
				memset((char *)worldWideName->c_str(), 0, WORLD_WIDE_NAME_LEN);
				snprintf((char *)worldWideName->c_str(), WORLD_WIDE_NAME_LEN, "0x%" PRIX64"", wwn);
			}
			//-----------------------------------------------------------------------------
			//
			//! \fn create_Firmware_String()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit firmware Rev values and create a string firmware Rev 
			//
			//  Entry:
			//! \param firmwareRev - string for holding the firmware rev
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return firmwareRev = the string firmwareRev
			//
			//---------------------------------------------------------------------------
			inline void create_Firmware_String(std::string *firmwareRev, const sDriveInfo * const idInfo)
			{
				uint64_t firm = 0;
				firm = (idInfo->firmware & 0x00FFFFFFFFFFFFFFLL);
				firmwareRev->resize(FIRMWARE_REV_LEN);
				memset((char *)firmwareRev->c_str(), 0, FIRMWARE_REV_LEN);
				strncpy((char *)firmwareRev->c_str(), (char*)&firm, FIRMWARE_REV_LEN);
				byte_Swap_String((char *)firmwareRev->c_str());
			}
			//-----------------------------------------------------------------------------
			//
			//! \fn create_Device_Interface_String()
			//
			//! \brief
			//!   Description:  takes the two uint64 bit Devie interface string values and create a string device interface  
			//
			//  Entry:
			//! \param dInterface - pointer to the Devie interface v, where once constructed, will hold the Devie interface of the drive
			//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
			//
			//  Exit:
			//!   \return dInterface = the string dInterface
			//
			//---------------------------------------------------------------------------
			inline void create_Device_Interface_String(std::string *dInterface, const sDriveInfo * const idInfo)
			{
				uint64_t dFace = 0;
				dFace = (idInfo->deviceInterface & 0x00FFFFFFFFFFFFFFLL);
				byte_Swap_64(&dFace);
				dFace = (dFace >> 32);
				dInterface->resize(DEVICE_INTERFACE_LEN);
				memset((char *)dInterface->c_str(), 0, DEVICE_INTERFACE_LEN);
				strncpy((char *)dInterface->c_str(), (char*)&dFace, DEVICE_INTERFACE_LEN);
			}

        public:
            CATA_Farm_Log();
            CATA_Farm_Log( uint8_t *bufferData, size_t bufferSize, bool showStatus);
            virtual ~CATA_Farm_Log();
            eReturnValues parse_Farm_Log();
            void print_All_Pages(JSONNODE *masterData);
            void print_Page(JSONNODE *masterData, uint32_t page);
            void print_Page_Without_Drive_Info(JSONNODE *masterData, uint32_t page);
            virtual eReturnValues get_Log_Status(){ return m_status; };
            virtual void get_Serial_Number(std::string sn){ sn.assign( vFarmFrame[0].identStringInfo.serialNumber); };
            virtual void get_Firmware_String(std::string firmware){ firmware.assign(vFarmFrame[0].identStringInfo.firmwareRev); };
			virtual void get_World_Wide_Name(std::string wwn) {wwn.assign(vFarmFrame[0].identStringInfo.worldWideName);};
    };
#endif //!ATAFARM
}
  

