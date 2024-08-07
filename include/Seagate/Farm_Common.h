//
// CFarm_Common.h   Farm Related Functions
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
//
// \file CFarm_Common.h  
#pragma once

#include <string>
#include "common_types.h"
#include "Opensea_Parser_Helper.h"
#include "Farm_Types.h"
#include "Farm_Helper.h"
#include "Ata_Farm_Types.h"


namespace opensea_parser {
#ifndef FARMCOMMON
#define FARMCOMMON

#define INNER 0
#define OUTER 1
#define MIDDLE 2

#define ZONE0 0
#define ZONE1 1
#define ZONE2 2

#define WORD0 1
#define WORD1 2
#define WORDINT0 3
#define WORDINT1 4
#define DWORD0 5
#define DWORD1 6

	class CFarmCommon 
	{
		public:

			explicit CFarmCommon();
			virtual ~CFarmCommon();
			bool is_Device_Scsi(uint8_t buff0, uint8_t buff1);
			void create_Flat_SN(std::string& serialNumberStr, uint64_t* serialNumber,uint64_t* serialnumber2);
			void create_Serial_Number(std::string& serialNumberStr, uint32_t serialNumber, uint32_t serialNumber2, uint32_t majorRev, bool sas);
			void create_World_Wide_Name(std::string& worldWideName, uint64_t wwn, uint64_t wwn2, bool sas);
			void create_Firmware_String(std::string& firmwareRevStr, uint32_t firmware, uint32_t firmware2, bool sas);
			void create_Firmware_String_Flat(std::string& firmwareRevStr, uint32_t *firmware, uint32_t *firmware2);
			void create_Device_Interface_String(std::string& dInterfaceStr, uint32_t deviceInterface, bool sas);
			void create_Device_Interface_String_Flat(std::string& dInterfaceStr, uint32_t *deviceInterface);
			void create_Model_Number_String(std::string& modelStr, uint64_t* productID, bool sas);
			void create_Model_Number_String_Flat(std::string& modelStr, uint64_t* productID);
			void create_Version_Number(std::string &version, uint64_t versionID);
			void create_Year_Assembled_String(std::string &dateStr, uint16_t date, bool isSAS);
			void Get_FARM_Reason_For_Capture(std::string* reason, uint8_t flag);

			void floatHeadData(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void float_NODE_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits);
			void float_Array_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits);

			void sflyHeightData(JSONNODE* Node, const std::string& title, double calculation, sflyHeight* value, int track, int sizeAmount, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void sflyHeight_Node_Data(JSONNODE* Node, const std::string& title, double calculation, sflyHeight* value, int track, int sizeAmount, uint64_t heads, bool showStatusBits);
			void sflyHeight_Array_Data(JSONNODE* Node, const std::string& title, double calculation, sflyHeight* value, int track, int sizeAmount, uint64_t heads, bool showStatusBits);

			void sflyHeight_Float_Data(JSONNODE* Node, const std::string& title, sflyHeight* value, int track, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void sflyHeight_Float_Node_Data(JSONNODE* Node, const std::string& title, sflyHeight* value, int track, uint64_t heads, bool showStatusBits);
			void sflyHeight_Float_Array_Data(JSONNODE* Node, const std::string& title, sflyHeight* value, int track, uint64_t heads, bool showStatusBits);

			void h2sat_Data(JSONNODE* Node, const std::string& title, int movement, H2SAT* value, int track, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void h2sat_Node_Data(JSONNODE* Node, const std::string& title, int movement, H2SAT* value, int track, uint64_t heads, bool showStatusBits);
			void h2sat_Array_Data(JSONNODE* Node, const std::string& title, int movement, H2SAT* value, int track, uint64_t heads, bool showStatusBits);

			void h2sat_Float_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, H2SAT* value, int track, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void h2sat_Float_Node_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, H2SAT* value, int track, uint64_t heads, bool showStatusBits);
			void h2sat_Float_Array_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, H2SAT* value, int track, uint64_t heads, bool showStatusBits);

			void h2sat_Float_Dword_Data(JSONNODE* Node, const std::string& title, H2SAT* value, int track, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void h2sat_Float_Dword_Node_Data(JSONNODE* Node, const std::string& title, H2SAT* value, int track, uint64_t heads, bool showStatusBits);
			void h2sat_Float_Dword_Array_Data(JSONNODE* Node, const std::string& title, H2SAT* value, int track, uint64_t heads, bool showStatusBits);

			void uint_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void uint_Node_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits);
			void uint_Array_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits);

			void int_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void int_Node_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits);
			void int_Array_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits);

			void int_Dword_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void int_Dword_Node_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits);
			void int_Dword_Array_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits);

			void int_Cal_Byte_Data(JSONNODE* Node, const std::string& title, int16_t calculation, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void int_Cal_Byte_Node_Data(JSONNODE* Node, const std::string& title, int16_t calculation, int64_t* param, uint64_t heads, bool showStatusBits);
			void int_Cal_Byte_Array_Data(JSONNODE* Node, const std::string& title, int16_t calculation, int64_t* param, uint64_t heads, bool showStatusBits);

			void int_Percent_Dword_Data(JSONNODE* Node, const std::string& title, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void int_Percent_Dword_Node_Data(JSONNODE* Node, const std::string& title, int64_t* param, uint64_t heads, bool showStatusBits);
			void int_Percent_Dword_Array_Data(JSONNODE* Node, const std::string& title, int64_t* param, uint64_t heads, bool showStatusBits);

			void float_Cal_Word_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void float_Cal_Word_Node_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits);
			void float_Cal_Word_Array_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits);

			void float_Cal_DoubleWord_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void float_Cal_DoubleWord_Node_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits);
			void float_Cal_DoubleWord_Array_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits);

			void sas_Head_Float_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void sas_Head_Float_Node_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, uint64_t* value, uint64_t heads, bool showStatusBits);
			void Sas_Head_Float_Array_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, uint64_t* value, uint64_t heads, bool showStatusBits);

			void sas_Head_int_Data(JSONNODE* Node, const std::string& title, int movement, uint64_t calculation, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void sas_Head_int_Node_Data(JSONNODE* Node, const std::string& title, int movement, uint64_t calculation, uint64_t* value, uint64_t heads, bool showStatusBits);
			void Sas_Head_int_Array_Data(JSONNODE* Node, const std::string& title, int movement, uint64_t calculation, uint64_t* value, uint64_t heads, bool showStatusBits);

			void sas_Head_Double_Float_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic = false);
			void sas_Head_Double_Float_Node_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits);
			void sas_Head_Double_Float_Array_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits);
	};
    
#endif 
}
