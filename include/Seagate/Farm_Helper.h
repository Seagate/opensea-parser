//
// Farm_Helper.h   Farm Related Functions
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
// \file Farm_Helper.h  
#pragma once

#include <inttypes.h>
#include "common_types.h"
#include "bit_manip.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"
#include <sstream>
#include <iomanip>


namespace opensea_parser {
#ifndef FARMHELPER
#define FARMHELPER

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
	inline void set_json_64_bit_With_Status(JSONNODE *nowNode, const std::string & myStr, uint64_t value, bool hexPrint, bool showStatusBits)
	{
		// if the 31 bit is set it will turn the value to a negitive number
        if (!showStatusBits  && (M_GETBITRANGE(value, 63, 31) == 0))
		{
			if (g_parseNULL)
			{
				json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
			}
			else
			{
				if (hexPrint)
				{
					std::ostringstream temp;
					temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << static_cast<int32_t>(M_DoubleWord0(value));	
					json_push_back(nowNode, json_new_a(myStr.c_str(), temp.str().c_str()));
				}
				else
				{
					json_push_back(nowNode, json_new_i(myStr.c_str(), static_cast<int32_t>(M_DoubleWord0(value))));
				}
			}
			return;
		}
		

		if (showStatusBits)
		{
            JSONNODE *bigBit = json_new(JSON_NODE);
			if (!bigBit) return; // Check for allocation failure

            json_set_name(bigBit, myStr.c_str());
			set_Json_Bool(bigBit, "Field Supported", (value & BIT63) == BIT63);
			set_Json_Bool(bigBit, "Field Valid", (value & BIT62) == BIT62);
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
		if (showStatusBits)
		{
			JSONNODE *bigBit = json_new(JSON_NODE);
			if (!bigBit) return; // Check for allocation failure

			json_set_name(bigBit, myStr.c_str());

			set_Json_Bool(bigBit, "Field Supported", (value & BIT63) == BIT63);
			set_Json_Bool(bigBit, "Field Valid", (value & BIT62) == BIT62);

			value = check_Status_Strip_Status(static_cast<uint64_t>(value));
			json_push_back(bigBit, json_new_i(myStr.c_str(), value));
			json_push_back(nowNode, bigBit);
		}
		else
		{
			if (g_parseNULL && check_For_Active_Status(reinterpret_cast<uint64_t*>(&value)) ==false)
			{
				json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
			}
			else
			{
				value = check_Status_Strip_Status(static_cast<uint64_t>(value));
				json_push_back(nowNode, json_new_i(myStr.c_str(), value));
			}
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
	inline void set_json_string_With_Status(JSONNODE* nowNode, const std::string& myStr, const std::string& strValue, uint64_t value, bool showStatusBits)
	{
		if (showStatusBits)
		{
			JSONNODE* bigBit = json_new(JSON_NODE);
			if (!bigBit) return; // Check for allocation failure
			json_set_name(bigBit, myStr.c_str());
			set_Json_Bool(bigBit, "Field Supported", (value & BIT63) == BIT63);
			set_Json_Bool(bigBit, "Field Valid", (value & BIT62) == BIT62);
			json_push_back(bigBit, json_new_a(myStr.c_str(), strValue.c_str()));
			json_push_back(nowNode, bigBit);
		}
		else
		{
			if (!check_For_Active_Status(&value))
			{
				if (g_parseNULL)
				{
					json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
				}
				else
				{
					json_push_back(nowNode, json_new_f(myStr.c_str(), 0.0));
				}
			}
			else
			{
				json_push_back(nowNode, json_new_a(myStr.c_str(), strValue.c_str()));
			}
		}
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_float_With_Status()
	//
	//! \brief
	//!   Description:  set the json values for a intiger value and will show status bits if the flag is set
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param value  =  double for pushing to the float
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param showStatusBits = flag to force showing the status bits on the value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
	inline void set_json_float_With_Status(JSONNODE* nowNode, const std::string& myStr, double value, uint64_t fullValue, bool showStatusBits)
	{
		if (showStatusBits)
		{
			JSONNODE* statusBit = json_new(JSON_NODE);
			if (!statusBit) return; // Check for allocation failure
			json_set_name(statusBit, myStr.c_str());
			set_Json_Bool(statusBit, "Field Supported", (fullValue & BIT63) == BIT63);
			set_Json_Bool(statusBit, "Field Valid", (fullValue & BIT62) == BIT62);
			if (!check_For_Active_Status(&fullValue))
			{
				json_push_back(statusBit, json_new_f(myStr.c_str(), 0.0));
			}
			else
			{
				json_push_back(statusBit, json_new_f(myStr.c_str(), value));
			}
			json_push_back(nowNode, statusBit);
		}
		else
		{
			if (!check_For_Active_Status(&fullValue))
			{
				if (g_parseNULL)
				{
					json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
				}
				else
				{
					json_push_back(nowNode, json_new_f(myStr.c_str(), 0.0));
				}
			}
			else
			{
				json_push_back(nowNode, json_new_f(myStr.c_str(), value));
			}
		}
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_int_Check_Status()
	//
	//! \brief
	//!   Description:  set the json values for a intiger value and will show status bits if the flag is set
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param value  =  int value that would have already been calculated.
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param showStatusBits = flag to force showing the status bits on the value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
	inline void set_json_int_Check_Status(JSONNODE* nowNode, const std::string& myStr, long long value, uint64_t fullValue, bool showStatusBits)
	{
		if (showStatusBits)
		{
			JSONNODE* statusBit = json_new(JSON_NODE);
			if (!statusBit) return; // Check for allocation failure
			json_set_name(statusBit, myStr.c_str());
			set_Json_Bool(statusBit, "Field Supported", (fullValue & BIT63) == BIT63);
			set_Json_Bool(statusBit, "Field Valid", (fullValue & BIT62) == BIT62);
			if (!check_For_Active_Status(&fullValue))
			{
				json_push_back(statusBit, json_new_i(myStr.c_str(), 0));
			}
			else
			{
				json_push_back(statusBit, json_new_i(myStr.c_str(), value));
			}
			json_push_back(nowNode, statusBit);
		}
		else
		{
			if (!check_For_Active_Status(&fullValue))
			{
				if (g_parseNULL)
				{
					json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
				}
				else
				{
					json_push_back(nowNode, json_new_i(myStr.c_str(), 0));
				}
			}
			else
			{
				json_push_back(nowNode, json_new_i(myStr.c_str(), value));
			}
		}
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_bool_With_Status()
	//
	//! \brief
	//!   Description:  set the json values for a boolean and will show status bits if the flag is set
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
	inline void set_json_bool_With_Status(JSONNODE* nowNode, const std::string& myStr, uint64_t value, bool showStatusBits)
	{
		if (showStatusBits)
		{
			JSONNODE* bigBit = json_new(JSON_NODE);
			if (!bigBit) return; // Check for allocation failure

			json_set_name(bigBit, myStr.c_str());
			set_Json_Bool(bigBit, "Field Supported", (value & BIT63) == BIT63);
			set_Json_Bool(bigBit, "Field Valid", (value & BIT62) == BIT62);
			if (check_Status_Strip_Status(value) != 0)
			{
				set_Json_Bool(bigBit, myStr, true);
			}
			else
			{
				set_Json_Bool(bigBit, myStr, false);
			}
			json_push_back(nowNode, bigBit);
		}
		else
		{
			if (check_Status_Strip_Status(value) != 0)
			{
				set_Json_Bool(nowNode, myStr, true);
			}
			else
			{
				// if parseNull and the status bits are not set then set it to NULL
				if (g_parseNULL && !check_For_Active_Status(&value))
				{
					json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
				}
				else
				{
					set_Json_Bool(nowNode, myStr, false);
				}
			}
		}
	}

#endif 
}
