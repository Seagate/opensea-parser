//
// Farm_Types.h   Structures of FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file Farm_Types.h  
#pragma once

#include <inttypes.h>
#include "Opensea_Parser_Helper.h"

#define FARMSIGNATURE           0x00004641524D4552
#define MAX_HEAD_COUNT 24
#pragma pack(push, 1)

//-----------------------------------------------------------------------------
//
//! \fn check_For_Active_Status()
//
//! \brief
//!   Description:  check for the active status bit in the 64 bit value
//
//  Entry:
//! \param value  =  64 bit value to check to see if the bit is set or not
//
//  Exit:
//!   \return bool - false or true
//
//---------------------------------------------------------------------------
inline bool check_For_Active_Status(uint64_t *value)
{
    if ((*value & BIT63) == BIT63 && (*value & BIT62) == BIT62)
    {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
//
//! \fn check_Status_Strip_Status()
//
//! \brief
//!   Description:  check for the active status bit in the 64 bit value
//
//  Entry:
//! \param value  =  64 bit value to check to see if the bit is set or not
//
//  Exit:
//!   \return uint64_t return the stipped value or a 0
//
//---------------------------------------------------------------------------
inline uint64_t check_Status_Strip_Status(uint64_t value)
{
	if (check_For_Active_Status(&value))
	{
		value = value & 0x00FFFFFFFFFFFFFFLL;
	}
	else
	{
		value = 0;
	}
	return value;
}

inline void set_json_64_bit_With_Status(JSONNODE *nowNode, const std::string & myStr, uint64_t value, bool hexPrint, bool showStatusBits)
{
	std::string printStr = " ";
	printStr.resize(BASIC);
	std::string lowStr = "64 bit Value Lower value";
	std::string upperStr = "64 bit Value Upper value";


	JSONNODE *bigBit = json_new(JSON_NODE);
	json_set_name(bigBit, (char *)myStr.c_str());
	if (showStatusBits)
	{
		if ((value & BIT63) == BIT63)
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Supported", true);
		}
		else
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Supported", false);
		}
		if ((value & BIT62) == BIT62)
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Valid", true);
		}
		else
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Valid", false);
		}
	}
	value = check_Status_Strip_Status(value);
	uint32_t lowValue = static_cast<uint32_t>(value);
	uint32_t upperValue = static_cast<uint32_t>(value >> 32);
	if (hexPrint)
	{
		if (value > UINT32_MAX)
		{
			json_push_back(bigBit, json_new_b("64 bit Value String in Hex", true));
			snprintf((char*)printStr.c_str(), BASIC, "0x%014" PRIx64"", value);
			json_push_back(bigBit, json_new_a("64 bit Value String", (char*)printStr.c_str()));
			json_push_back(bigBit, json_new_i((char*)lowStr.c_str(), lowValue));
			json_push_back(bigBit, json_new_i((char*)upperStr.c_str(), upperValue));
			json_push_back(nowNode, bigBit);
		}
		else
		{
			snprintf((char*)printStr.c_str(), BASIC, "0x%08" PRIx32"", lowValue);
			json_push_back(nowNode, json_new_a((char *)myStr.c_str(), (char*)printStr.c_str()));
		}
	}
	else
	{
		if (value > UINT32_MAX)
		{
			json_push_back(bigBit, json_new_b("64 bit Value String in Hex", false));
			snprintf((char*)printStr.c_str(), BASIC, "%" PRIi64"", value);
			json_push_back(bigBit, json_new_a("64 bit Value String", (char*)printStr.c_str()));
			json_push_back(bigBit, json_new_i((char*)lowStr.c_str(), lowValue));
			json_push_back(bigBit, json_new_i((char*)upperStr.c_str(), upperValue));
			json_push_back(nowNode, bigBit);
		}
		else
		{
			snprintf((char*)printStr.c_str(), BASIC, "%" PRIi64"", value);
			json_push_back(nowNode, json_new_i((char *)myStr.c_str(), static_cast<int32_t>(value)));
		}
	}
	
}

inline void set_json_int_With_Status(JSONNODE *nowNode, const std::string & myStr, uint64_t value, bool showStatusBits)
{
	std::string printStr = " ";
	printStr.resize(BASIC);


	if (showStatusBits)
	{
		JSONNODE *bigBit = json_new(JSON_NODE);
		json_set_name(bigBit, (char *)myStr.c_str());
		if ((value & BIT63) == BIT63)
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Supported", true);
		}
		else
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Supported", false);
		}
		if ((value & BIT62) == BIT62)
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Valid", true);
		}
		else
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Valid", false);
		}
		value = check_Status_Strip_Status(value);
		json_push_back(bigBit, json_new_i((char *)myStr.c_str(), static_cast<uint32_t>(M_Word0(value))));
		json_push_back(nowNode, bigBit);
	}
	else
	{
		value = check_Status_Strip_Status(value);
		json_push_back(nowNode, json_new_i((char *)myStr.c_str(), static_cast<uint32_t>(M_Word0(value))));
	}
}

inline void set_json_string_With_Status(JSONNODE *nowNode, const std::string & myStr, const std::string & strValue, uint64_t value, bool showStatusBits)
{
	std::string printStr = " ";
	printStr.resize(BASIC);

	if (showStatusBits)
	{
		JSONNODE *bigBit = json_new(JSON_NODE);
		json_set_name(bigBit, (char *)myStr.c_str());
		if ((value & BIT63) == BIT63)
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Supported", true);
		}
		else
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Supported", false);
		}
		if ((value & BIT62) == BIT62)
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Valid", true);
		}
		else
		{
			opensea_parser::set_Json_Bool(bigBit, "Field Valid", false);
		}
		json_push_back(bigBit, json_new_a((char *)myStr.c_str(), (char *)strValue.c_str()));
		json_push_back(nowNode, bigBit);
	}
	else
	{
		json_push_back(nowNode, json_new_a((char *)myStr.c_str(), (char *)strValue.c_str()));
	}
}