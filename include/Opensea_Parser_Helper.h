//
// Opensea_Parser_Helper.h   Definition of SM2 specific structures. 
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
#include <limits.h>

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <string>

enum class eVerbosity_open
{
	VERBOSITY_QUIET = 0,
	VERBOSITY_DEFAULT = 1,
	VERBOSITY_COMMAND_NAMES = 2,
	VERBOSITY_COMMAND_VERBOSE = 3,
	VERBOSITY_BUFFERS = 4
};

extern eVerbosity_open g_verbosity;
extern eDataFormat g_dataformat;
extern bool	g_parseUnknown;
extern bool g_parseNULL;
extern time_t g_currentTime;
extern char g_currentTimeString[64];
extern char *g_currentTimeStringPtr;



namespace opensea_parser {

#ifndef OPENSEA_PARSER
#define OPENSEA_PARSER

#define OFFSETZERO 0
#define OFFSETONE 1
#define OFFSETTWO 2
#define OFFSETTHREE 3
	// quick size for of the ints for case statements
#define ONE_INT_SIZE 1
#define TWO_INT_SIZE 2
#define FOUR_INT_SIZE 4 
#define EIGHT_INT_SIZE 8

#define LOGPAGESIZE 4
#define PARAMSIZE   4

#define RELISTAT                24
#define WORLD_WIDE_NAME_LEN     19
#define DEVICE_INTERFACE_LEN    4
#define SERIAL_NUMBER_LEN       8
#define MODEL_NUMBER_LEN        20
#define FIRMWARE_REV_LEN        4
#define SAS_FIRMWARE_REV_LEN    4
#define BASIC                   80

#define SAS_SUBPAGE_ZERO			0x00
#define SAS_SUBPAGE_ONE				0x01
#define SAS_SUBPAGE_TWO				0x02
#define SAS_SUBPAGE_THREE			0x03
#define SAS_SUBPAGE_FOUR			0x04
#define SAS_SUBPAGE_FIVE			0x05
#define SAS_SUBPAGE_SIX				0x06
#define SAS_SUBPAGE_SEVEN			0x07
#define SAS_SUBPAGE_EIGHT			0x08
#define SAS_SUBPAGE_NINE			0x09
#define SAS_SUBPAGE_A				0x0A
#define SAS_SUBPAGE_B				0x0B
#define SAS_SUBPAGE_C				0x0C
#define SAS_SUBPAGE_D				0x0D
#define SAS_SUBPAGE_E				0x0E
#define SAS_SUBPAGE_F				0x0F
#define SAS_SUBPAGE_20				0x20
#define SAS_SUBPAGE_21				0x21
#define SAS_SUBPAGE_FF				0xFF

#define COMMAND_DURATION_LIMITS_LOG 0x19

	typedef enum _eParserInterfaceTypes
	{
		PARSER_INTERFACE_UNKNOWN = 0,
		PARSER_INTERFACE_TYPE_SCSI = 1,
		PARSER_INTERFACE_TYPE_FC = 2,
		PARSER_INTERFACE_TYPE_SAS = 4,
		PARSER_INTERFACE_TYPE_SATA = 8,
	}eInterfaceTypes;

    // output file types
	typedef enum _eOpensea_print_Types
    {
        OPENSEA_LOG_PRINT_JSON,     // default
        OPENSEA_LOG_PRINT_TEXT,
        OPENSEA_LOG_PRINT_CSV,
        OPENSEA_LOG_PRINT_FLAT_CSV,
        OPENSEA_LOG_PRINT_PROM,
        OPENSEA_LOG_PRINT_PYTHON_DICTIONARY,
    }eOpensea_print_Types;

	// SCSI Parameter Control Bytes
	typedef enum _eOpenSea_SCSI_Log_Parameter_Types
	{
		OPENSEA_SCSI_LOG_BOUNDED_DATA_COUNTER = 0,   // default
		OPENSEA_SCSI_LOG_ASCII_FORMAT_LIST,
		OPENSEA_SCSI_LOG_BOUNDED_DATA_COUNTER_OR_UNBOUNDED_DATA_COUNTER,
		OPENSEA_SCSI_LOG_BINARY_FORMAT_LIST,
	}eOpenSea_SCSI_Log_Parameter_Types;

    typedef enum  _eSMARTFrameFlags
    {
        SMART_FRAME_FLAG_SAVED_AT_MSI_PERIOD = 0x00,
        SMART_FRAME_FLAG_SAVED_DUE_TO_COLD_START = 0x01,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_REZERO_COMMAND = 0x02,
        SMART_FRAME_FLAG_SAVED_DUE_TO_03_XX_ERROR = 0x03,
        SMART_FRAME_FLAG_SAVED_DUE_TO_04_XX_ERROR = 0x04,
        SMART_FRAME_FLAG_SAVED_DUE_TO_1_5D_ERROR_FOR_PREDICTIVE_FAILURE = 0x05,
        SMART_FRAME_FLAG_SAVED_DUE_TO_FACTORY_COMMAND = 0x06,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SEND_DIAGNOSTIC_COMMAND_WITH_SELFTEST_BIT_SET = 0x07,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_0B_08_ERROR = 0x08,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_09_XX_ERROR = 0x09,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_REZERO_COMMAND_WITH_HARD_RESET = 0x0A,
        SMART_FRAME_FLAG_SAVED_DUE_TO_0B_01_WARNING = 0x0B,
        SMART_FRAME_FLAG_SAVED_DUE_TO_POWER_DOWN = 0x0C,
        SMART_FRAME_FLAG_SAVED_DUE_TO_BEGINNING_OF_SCSI_FORMAT_OPERATION = 0x0D,
        SMART_FRAME_FLAG_SAVED_DUE_TO_RESET_FLASH_LED_ERROR = 0x0E,
        SMART_FRAME_FLAG_RESERVED = 0x0F,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_DOWNLOAD_COMMAND = 0x10,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SCSI_TRIP_ALGORITHM = 0x11,
        SMART_FRAME_FLAG_SAVED_DUE_TO_NHT_HEALTH_TRIP = 0x12,
        SMART_FRAME_FLAG_DUE_TO_ISSRA_WARNING = 0x13,
        SMART_FRAME_FLAG_DUE_TO_FACTORY_COMMAND_SAVE = 0x14,
        SMART_FRAME_FLAG_DUE_TO_EXIT_REMANUFACTURING = 0x15,
        SMART_FRAME_FLAG_DUE_TO_HITACHI_ORM_OR_HD_TEST = 0x16,
        SMART_FRAME_FLAG_STOP_UNIT_FRAME = 0x17,
        SMART_FRAME_FLAG_INTERIM_FRAME_10MIN_IDLE_TIME = 0x18,
        SMART_FRAME_FLAG_SAVED_DUE_TO_WRITE_PROTECT_INVOCATION = 0x19,
        SMART_FRAME_FLAG_SAVED_DUE_TO_FACTORY_COMMAND_TO_INVOCATE_A_FRAME_THAT_CAN_BE_INTERRUPTABLE = 0x1A,
        SMART_FRAME_FLAG_SAVED_DUE_TO_SMART_SCT_COMMAND = 0x1B,
        SMART_FRAME_FLAG_SAVED_DUE_TO_HEAD_UNLOADING = 0x1C,
        SMART_FRAME_FLAG_SAVED_DUE_TO_START_OF_SANITIZE_COMMAND = 0x1D,
        SMART_FRAME_FLAG_SAVED_DUE_TO_END_OF_SANITIZE_COMMAND = 0x1E,
        SMART_FRAME_FLAG_SAVED_DUE_TO_FORMAT_COMPLETION = 0x1F,
        SMART_FRAME_FLAG_UNRECOVERED_WRITE_NVC_RESET_FRAME = 0x20,
        SMART_FRAME_FLAG_QUICK_FORMAT_FINISH = 0x21,
        SMART_FRAME_FLAG_SATA_DEVICE_FAULT = 0x22,
        SMART_FRAME_FLAG_SATA_UNCORRECTABLE_ERROR = 0x23,
        SMART_FRAME_FLAG_SATA_ERC_UNCORRETABLE_ERROR = 0x24,
        SMART_FRAME_FLAG_DRIVE_SELF_TEST_COMPLETE_WITH_FAILURE = 0x25,
        SMART_FRAME_FLAG_DEFECT_LIST_COMMAND = 0x26,
        SMART_FRAME_FLAG_F7_CDB_TRIGGER = 0x27,
    }eSMARTFrameFlags;

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
                pageCode = buffer[OFFSETZERO];
                subPage = buffer[OFFSETONE];
                pageLength = *(reinterpret_cast<uint16_t*>(&buffer[OFFSETTWO]));
            }
            else
            {
                pageCode = 0;
                subPage = 0;
                pageLength = 0;
            }
        }
	}sLogPageStruct;

	typedef struct _sLogPageParamStruct
	{
		uint16_t		paramCode;							//<! The PARAMETER CODE field is described in 5.2.2.2.2, and shall be set as shown in table 352 for the Temperature log parameter.
		uint8_t			paramControlByte;					//<! binary format list log parameter
		uint8_t			paramLength;						//<! The PARAMETER LENGTH field is described in 5.2.2.2.2, and shall be set as shown in table 352 for the Temperature log parameter.
		_sLogPageParamStruct() : paramCode(0), paramControlByte(0), paramLength(0) {};
	}sLogParams;

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
        ZONED_DEVICE_STATISTICS = 0x14,
		BACKGROUND_SCAN = 0x15,
		PROTOCOL_SPECIFIC_PORT = 0x18,
		CACHE_MEMORY_STATISTICES = 0x19,
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
		SOLID_STATE_MEDIA ,	ZONED_DEVICE_STATISTICS , BACKGROUND_SCAN , CACHE_MEMORY_STATISTICES,
		PROTOCOL_SPECIFIC_PORT, POWER_CONDITION_TRANSITIONS , INFORMATIONAL_EXCEPTIONS,
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
	inline bool check_For_Active_Status(uint64_t *const value)
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
			value = value & UINT64_C(0x00FFFFFFFFFFFFFF);
		}
		else
		{
			value = 0;
		}
		return static_cast<int64_t>(value);
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
		int64_t statusValue = 0;
		statusValue = check_Status_Strip_Status(value);
        std::ostringstream temp;
        if (hexPrint)
        {
			if (g_parseNULL && check_For_Active_Status(reinterpret_cast<uint64_t*>(&value)) == false)
			{
				json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
			}
			else
			{
				//64 bit in hex is only 8 in width
				temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << statusValue;
				json_push_back(nowNode, json_new_a(myStr.c_str(), temp.str().c_str()));
			}
        }
        else
        {
            if ((M_IGETBITRANGE(statusValue, 63, 56) == 0) && check_For_Active_Status(&value) == true)
            {
                json_push_back(nowNode, json_new_i(myStr.c_str(), static_cast<json_int_t>(statusValue)));
            }
            else
            {
				if (g_parseNULL && check_For_Active_Status(&value) == false)
				{
					json_push_back(nowNode, json_new_a(myStr.c_str(), "NULL"));
				}
				else
				{
					// if the value is greater then a unsigned 48 bit number print it as a string
					temp << std::dec << statusValue;
					json_push_back(nowNode, json_new_a(myStr.c_str(), temp.str().c_str()));
				}
            }
        }
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
        std::ostringstream temp;
        if (hexPrint)
        {
            //json does not support 64 bit numbers. Therefore we will print it as a string
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
            json_push_back(nowNode, json_new_a(myStr.c_str(), temp.str().c_str()));
        }
        else
        {
            if (M_IGETBITRANGE(value,63,48) == 0)
            {
                json_push_back(nowNode, json_new_i(myStr.c_str(), static_cast<json_int_t>(value)));
            }
            else
            {
                // if the vale is greater then a unsigned 32 bit number print it as a string
                temp << std::dec << value;
                json_push_back(nowNode, json_new_a(myStr.c_str(), temp.str().c_str()));
            }
        }
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
            json_push_back(nowNode, json_new_b(myStr.c_str(), true));
        else
            json_push_back(nowNode, json_new_b(myStr.c_str(), false));
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


    inline void byte_swap_std_string(std::string &stringToSwap)
    {
        std::stringstream tempString;
        for (size_t strOffset = 0; (strOffset + 1) < stringToSwap.size(); strOffset += 2)
        {
			tempString << stringToSwap.at(strOffset + 1);
            tempString << stringToSwap.at(strOffset);
        }
        stringToSwap.clear();//clear out the old byte swapped string
        stringToSwap = tempString.str();//assign it to the correctly swapped string
    }

    inline void remove_trailing_whitespace_std_string(std::string &stringToTrim)
    {
        //search for the last of ASCII characters...so use find_last_of the printable characters that are NOT spaces should do the trick...-TJE
		size_t lastChar = stringToTrim.find_last_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=~!@#$%^&*()_+[]{};':\"\\|,./<>?`") + 1;
		if(lastChar != std::string::npos)
			stringToTrim.erase(lastChar, stringToTrim.size() - lastChar);
    }

    inline void std_string_to_lowercase(std::string &stringToLowercase)
    {
		for(size_t iter = 0; iter < stringToLowercase.size(); ++iter)
		{
			stringToLowercase.at(iter) = static_cast<char>(std::tolower(stringToLowercase.at(iter)));
		}
		//Below is c++11:
        // std::transform(stringToLowercase.begin(), stringToLowercase.end(), stringToLowercase.begin(),
        //     [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }
	inline eReturnValues fill_Log_Params(sLogParams &myStruct, uint8_t *buffer )
	{

		myStruct.paramCode = M_BytesTo2ByteValue(buffer[OFFSETZERO],buffer[OFFSETONE]);
		myStruct.paramControlByte = buffer[OFFSETTWO];
		myStruct.paramLength = buffer[OFFSETTHREE];
		return SUCCESS;

	}

	inline double remove_Double_Transfer_Digits(double* const decimalValue)
	{
		double newValue = 0.0;
		std::ostringstream temp;
		temp << std::fixed << std::setprecision(6) << *decimalValue;
		newValue = std::atof(temp.str().c_str());
		return newValue;
	}

#endif // !OPENSEA_PARSER
}


