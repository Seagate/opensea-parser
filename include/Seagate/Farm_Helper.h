//
// Farm_Helper.h   Farm Related Functions
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2019 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file Farm_Helper.h  
#pragma once

#include <inttypes.h>
#include "libjson.h"
#include "Opensea_Parser_Helper.h"
namespace opensea_parser {
#ifndef FARMHELPER
#define FARMHELPER

	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_64()
	//
	//! \brief
	//!   Description:  set the json values for a 64 bit value
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param hexPrint =  if true then print the data in a hex format
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
	/*
	inline void set_json_64_Only(JSONNODE *nowNode, const std::string & myStr, int64_t value, bool hexPrint)
	{
		std::string printStr = " ";
		printStr.resize(BASIC);
		std::string lowStr = "64 bit Value Lower value";
		std::string upperStr = "64 bit Value Upper value";

		value = check_Status_Strip_Status(value);
		value = check_for_signed_int(value, 16);
		int32_t lowValue = static_cast<int32_t>(value);
		int32_t upperValue = static_cast<int32_t>(value >> 32);
		if (hexPrint)
		{
#ifndef _WIN64   //To make old gcc compilers happy
			JSONNODE *bigBit = json_new(JSON_NODE);
			json_set_name(bigBit, (char *)myStr.c_str());
			json_push_back(bigBit, json_new_b("64 bit Value String in Hex", true));
			snprintf((char*)printStr.c_str(), BASIC, "0x%014" PRIx64"", value);
			json_push_back(bigBit, json_new_a("64 bit Value String", (char*)printStr.c_str()));
			json_push_back(bigBit, json_new_i((char*)lowStr.c_str(), lowValue));
			json_push_back(bigBit, json_new_i((char*)upperStr.c_str(), upperValue));
			json_push_back(nowNode, bigBit);
#else		//json does not support 64 bit numbers. Therefore we will print it as a string
			snprintf((char*)printStr.c_str(), BASIC, "0x%08" PRIx64"", value);
			json_push_back(nowNode, json_new_a((char *)myStr.c_str(), (char*)printStr.c_str()));
#endif
		}
		else
		{
#ifndef _WIN64   //To make old gcc compilers happy
			JSONNODE *bigBit = json_new(JSON_NODE);
			json_set_name(bigBit, (char *)myStr.c_str());
			json_push_back(bigBit, json_new_b("64 bit Value String in Hex", false));
			snprintf((char*)printStr.c_str(), BASIC, "%" PRIi64"", value);
			json_push_back(bigBit, json_new_a("64 bit Value String", (char*)printStr.c_str()));
			json_push_back(bigBit, json_new_i((char*)lowStr.c_str(), lowValue));
			json_push_back(bigBit, json_new_i((char*)upperStr.c_str(), upperValue));
			json_push_back(nowNode, bigBit);
#else		//json does not support 64 bit numbers. Therefore we will print it as a string
			snprintf((char*)printStr.c_str(), BASIC, "%" PRId64"", value);
			json_push_back(nowNode, json_new_a((char *)myStr.c_str(), (char*)printStr.c_str()));
#endif
		}

	}
	*/
	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_64_bit_With_Status()
	//
	//! \brief
	//!   Description:  set the json values for a 64 bit value and will show status bits if the flag is set
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param hexPrint =  if true then print the data in a hex format
	//! \param showStatusBits = flag to force showing the status bits on the value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
	inline void set_json_64_bit_With_Status(JSONNODE *nowNode, const std::string & myStr, int64_t value, bool hexPrint, bool showStatusBits)
	{
		std::string printStr = " ";
		printStr.resize(BASIC);
		std::string lowStr = "64 bit Value Lower value";
		std::string upperStr = "64 bit Value Upper value";
		uint64_t strippedValue = check_Status_Strip_Status(value);
		
		if (!showStatusBits && (strippedValue < INT32_MAX && strippedValue > INT32_MIN))
		{
			if (hexPrint) 
			{
				snprintf((char*)printStr.c_str(), BASIC, "0x%08" PRIx32"", static_cast<int32_t>(M_DoubleWord0(value)));
				json_push_back(nowNode, json_new_a((char *)myStr.c_str(), (char*)printStr.c_str()));
			}
			else
			{
				json_push_back(nowNode, json_new_i((char *)myStr.c_str(), static_cast<int32_t>(M_DoubleWord0(value))));
			}
			return;
		}
		

		if (showStatusBits)
		{
            JSONNODE *bigBit = json_new(JSON_NODE);
            json_set_name(bigBit, (char *)myStr.c_str());
			if ((value & BIT63) == BIT63)
			{
				set_Json_Bool(bigBit, "Field Supported", true);
			}
			else
			{
				set_Json_Bool(bigBit, "Field Supported", false);
			}
			if ((value & BIT62) == BIT62)
			{
				set_Json_Bool(bigBit, "Field Valid", true);
			}
			else
			{
				set_Json_Bool(bigBit, "Field Valid", false);
			}
            set_json_64bit_With_Check_Status(bigBit, myStr, value, hexPrint);
			json_push_back(nowNode, bigBit);
		}
		else
		{
            set_json_64bit_With_Check_Status(nowNode, myStr, value, hexPrint);
		}
	}

	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_int_With_Status()
	//
	//! \brief
	//!   Description:  set the json values for a intiger value and will show status bits if the flag is set
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param showStatusBits = flag to force showing the status bits on the value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
	inline void set_json_int_With_Status(JSONNODE *nowNode, const std::string & myStr, int64_t value, bool showStatusBits)
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
			json_push_back(bigBit, json_new_i((char *)myStr.c_str(), static_cast<int32_t>(M_DoubleWord0(value))));
			json_push_back(nowNode, bigBit);
		}
		else
		{
			value = check_Status_Strip_Status(value);
			json_push_back(nowNode, json_new_i((char *)myStr.c_str(), static_cast<int32_t>(M_DoubleWord0(value))));
		}
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_string_With_Status()
	//
	//! \brief
	//!   Description:  set the json values for a string and will show status bits if the flag is set
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param  myStr = value in a string format
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param showStatusBits = flag to force showing the status bits on the value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
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
#endif 
}
