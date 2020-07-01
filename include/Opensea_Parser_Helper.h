//
// Opensea_Parser_Helper.h   Definition of SM2 specific structures. 
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
//
// \file Opensea_Parser_Helper.h   Definition of SeaParser specific functions, structures
#pragma once
//defining these macros for C++ to make older C++ compilers happy and work like the newer C++ compilers
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <inttypes.h>
#include <time.h>
#include "common.h"
#include "libjson.h"

extern eVerbosityLevels g_verbosity;
extern time_t g_currentTime;
extern char g_currentTimeString[64];
extern char *g_currentTimeStringPtr;



namespace opensea_parser {

#ifndef OPENSEA_PARSER
#define OPENSEA_PARSER

#define RELISTAT                24
#define WORLD_WIDE_NAME_LEN     18
#define DEVICE_INTERFACE_LEN    4
#define SERIAL_NUMBER_LEN       8
#define MODEL_NUMBER_LEN        20
#define FIRMWARE_REV_LEN        4
#define BASIC                   80


    // output file types
	typedef enum _eOpensea_print_Types
    {
        OPENSEA_LOG_PRINT_JSON,     // default
        OPENSEA_LOG_PRINT_TEXT,
        OPENSEA_LOG_PRINT_CSV,
        OPENSEA_LOG_PRINT_FLAT_CSV,
    }eOpensea_print_Types;
	// SCSI Parameter Control Bytes
	typedef enum _eOpenSea_SCSI_Log_Parameter_Types
	{
		OPENSEA_SCSI_LOG_BOUNDED_DATA_COUNTER = 0,   // default
		OPENSEA_SCSI_LOG_ASCII_FORMAT_LIST,
		OPENSEA_SCSI_LOG_BOUNDED_DATA_COUNTER_OR_UNBOUNDED_DATA_COUNTER,
		OPENSEA_SCSI_LOG_BINARY_FORMAT_LIST,
	}eOpenSea_SCSI_Log_Parameter_Types;
#pragma pack(push, 1)
	typedef struct _sLogPageStruct
	{
		uint8_t			pageCode;							//<! page code for the log lpage format
		uint8_t			subPage;							//<! subpage code for the log page format
		uint16_t		pageLength;							//<! this is different from size, see SCSI SPC Spec. 
		_sLogPageStruct() : pageCode(0), subPage(0), pageLength(0) {};
        _sLogPageStruct(uint8_t* buffer)
        {
            if (buffer != NULL)
            {
                pageCode = buffer[0];
                subPage = buffer[1];
                pageLength = *(reinterpret_cast<uint16_t*>(&buffer[2]));
            }
            else
            {
                pageCode = 0;
                subPage = 0;
                pageLength = 0;
            }
        }
	}sLogPageStruct;
#pragma pack(pop)
	typedef enum _eLogPageNames
	{
		SUPPORTED_LOG_PAGES = 0x00,
		WRITE_ERROR_COUNTER = 0x02,
		READ_ERROR_COUNTER = 0x03,
		VERIFY_ERROR_COUNTER = 0x05,
		NON_MEDIUM_ERROR = 0x06,
		FORMAT_STATUS = 0x08,
		LOGICAL_BLOCK_PROVISIONING = 0x0C,
		ENVIRONMENTAL = 0x0D,
		START_STOP_CYCLE_COUNTER = 0x0E,
		APPLICATION_CLIENT = 0x0F,
		SELF_TEST_RESULTS = 0x10,
		SOLID_STATE_MEDIA = 0x11,
		BACKGROUND_SCAN = 0x15,
		PROTOCOL_SPECIFIC_PORT = 0x18,
		POWER_CONDITION_TRANSITIONS = 0x1A,
		INFORMATIONAL_EXCEPTIONS = 0x2F,        
		CACHE_STATISTICS = 0x37,
		SEAGATE_SPECIFIC_LOG = 0x3D,
		FACTORY_LOG = 0x3E,        
	}eLogPageNames;

	const int pageCodes[] = { SUPPORTED_LOG_PAGES,	WRITE_ERROR_COUNTER,
		READ_ERROR_COUNTER ,VERIFY_ERROR_COUNTER, NON_MEDIUM_ERROR ,
		FORMAT_STATUS ,	LOGICAL_BLOCK_PROVISIONING ,ENVIRONMENTAL,
		START_STOP_CYCLE_COUNTER ,	APPLICATION_CLIENT,	SELF_TEST_RESULTS,
		SOLID_STATE_MEDIA ,	BACKGROUND_SCAN , PROTOCOL_SPECIFIC_PORT,
		POWER_CONDITION_TRANSITIONS , INFORMATIONAL_EXCEPTIONS,
        CACHE_STATISTICS, SEAGATE_SPECIFIC_LOG, FACTORY_LOG,};

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
	//!   \return int64_t return the stipped value or a 0
	//
	//---------------------------------------------------------------------------
	inline int64_t check_Status_Strip_Status(uint64_t value)
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
	//-----------------------------------------------------------------------------
	//
	//! \fn check_for_signed_int()
	//
	//! \brief
	//!   Description:  check for the active status bit in the 64 bit value, then stips the status off and checks the 
	//!		bits to see if it is a signed int and saves the value
	//
	//  Entry:
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param length = the amount we need to move to the left and the right to remove the status 
	//
	//  Exit:
	//!   \return int64_t return the signed stipped value or a 0
	//
	//---------------------------------------------------------------------------
	inline int64_t check_for_signed_int(int64_t value, uint32_t length)
	{
		int8_t neg = -1;			// set to 1 if bit 56 is set then set it to -1
		value = value << length;
		if (value & BIT63)   // check to see if the bit is set for neg number
		{
			value = M_2sCOMPLEMENT(value);
			value = value * neg;
		}
		value = value >> length;      // move the back to the original number
		return(value);
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn set_json_64bit_With_Check_Status()
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
    inline void set_json_64bit_With_Check_Status(JSONNODE *nowNode, const std::string & myStr, uint64_t value, bool hexPrint)
    {
		value = check_Status_Strip_Status(value);
        char *printStr = (char*)calloc((BASIC), sizeof(char));

        if (hexPrint)
        {
            //json does not support 64 bit numbers. Therefore we will print it as a string
            snprintf(printStr, BASIC, "0x%016" PRIx64"", value);
            json_push_back(nowNode, json_new_a((char *)myStr.c_str(), printStr));
        }
        else
        {
            if (M_IGETBITRANGE(value, 63, 32) == 0)
            {
                json_push_back(nowNode, json_new_i((char *)myStr.c_str(), static_cast<int32_t>(M_DoubleWord0(value))));
            }
            else
            {
                // if the vale is greater then a unsigned 32 bit number print it as a string
                snprintf(printStr, BASIC, "%" PRIu64"", value);
                json_push_back(nowNode, json_new_a((char *)myStr.c_str(), printStr));
            }
        }
        safe_Free(printStr);
	}
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
    inline void set_json_64bit(JSONNODE *nowNode, const std::string & myStr, uint64_t value, bool hexPrint)
    {
        char *printStr = (char*)calloc((BASIC), sizeof(char));

        if (hexPrint)
        {
            //json does not support 64 bit numbers. Therefore we will print it as a string
            snprintf(printStr, BASIC, "0x%016" PRIx64"", value);
            json_push_back(nowNode, json_new_a((char *)myStr.c_str(), printStr));
        }
        else
        {
            if (M_IGETBITRANGE(value,63,32) == 0)
            {
                json_push_back(nowNode, json_new_i((char *)myStr.c_str(), static_cast<int32_t>(M_DoubleWord0(value))));
            }
            else
            {
                // if the vale is greater then a unsigned 32 bit number print it as a string
                snprintf(printStr, BASIC, "%" PRIu64"", value);
                json_push_back(nowNode, json_new_a((char *)myStr.c_str(), printStr));
            }
        }
        safe_Free(printStr);
    }
	//-----------------------------------------------------------------------------
	//
	//! \fn set_Json_Bool()
	//
	//! \brief
	//!   Description:  set the json values for bool types
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  myStr = the string data what will be adding to
	//! \param workingValue  =  boolean value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
    inline void set_Json_Bool(JSONNODE *nowNode, const std::string & myStr, bool workingValue)
    {
        if (workingValue)
            json_push_back(nowNode, json_new_b((char*)myStr.c_str(), true));
        else
            json_push_back(nowNode, json_new_b((char*)myStr.c_str(), false));
    }
	//-----------------------------------------------------------------------------
	//
	//! \fn count_Occurance()
	//
	//! \brief
	//!   Description:  count the number of occurances in a string
	//
	//  Entry:
	//! \param  referenceStr = string to be referenced
	//! \param  subStr = what we are looking for in a string
	//
	//  Exit:
	//!   \return the uint32_t value 
	//
	//-----------------------------------------------------------------------------
    inline uint32_t count_Occurance(const std::string &referenceStr, const std::string &subStr)
    {
        const size_t step = subStr.size();
        size_t count(0);
        size_t pos(0);
        while ((pos = referenceStr.find(subStr, pos)) != std::string::npos)
        {
            pos += step;
            ++count;
        }
        return static_cast<uint32_t>(count);
    }
	//-----------------------------------------------------------------------------
	//
	//! \fn IsScsiLogPage()
	//
	//! \brief
	//!   Description:  count the number of occurances in a string
	//
	//  Entry:
	//! \param  length = string to be referenced
	//! \param  code = code from the sas
	//
	//  Exit:
	//!   \return boolean 
	//
	//-----------------------------------------------------------------------------
	inline bool IsScsiLogPage(uint16_t length, uint8_t code)
	{
		if (length != 0)
		{
			for (size_t i = 0; i < (sizeof(pageCodes) / sizeof(*pageCodes)); i++)
			{
				if (pageCodes[i] == code)
				{
					return true;
				}
			}
		}
		return false;
	}
#endif // !OPENSEA_PARSER
}


