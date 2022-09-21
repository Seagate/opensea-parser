//
// Farm_Helper.h   Farm Related Functions
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
//
// \file Farm_Helper.h  
#pragma once

#include <string>
#include "common.h"
#include "Opensea_Parser_Helper.h"
#include "Farm_Types.h"


namespace opensea_parser {
#ifndef FARMCOMMON
#define FARMCOMMON

	class CFarmCommon 
	{
		public:

			CFarmCommon();
			virtual ~CFarmCommon();
			void create_Flat_SN(std::string& serialNumberStr, uint64_t* serialNumber,uint64_t* serialnumber2);
			void create_Serial_Number(std::string& serialNumberStr, uint32_t serialNumber, uint32_t serialNumber2, uint32_t majorRev, bool sas);
			void create_World_Wide_Name(std::string& worldWideName, uint64_t wwn, uint64_t wwn2, bool sas);
			void create_Firmware_String(std::string& firmwareRevStr, uint32_t firmware, uint32_t firmware2, bool sas);
			void create_Firmware_String_Flat(std::string& firmwareRevStr, uint32_t *firmware, uint32_t *firmware2);
			void create_Device_Interface_String(std::string& dInterfaceStr, uint32_t deviceInterface, bool sas);
			void create_Device_Interface_String_Flat(std::string& dInterfaceStr, uint32_t *deviceInterface);
			void create_Model_Number_String(std::string& modelStr, uint64_t* productID, bool sas);
			void create_Model_Number_String_Flat(std::string& modelStr, uint64_t* productID);
			void create_Version_Number(std::string &version, uint64_t* versionID);
			void get_Reallocation_Cause_Meanings(std::string &meaning, uint16_t code);
			void get_Assert_Code_Meaning(std::string &meaning, uint16_t code);
			void create_Year_Assembled_String(std::string &dateStr, uint16_t date, bool isSAS);
			void Get_NVC_Status(JSONNODE* NVC_Node, uint64_t status);
			void Get_FARM_Reason_For_Capture(std::string* reason, uint8_t flag);
	};
    
#endif 
}
