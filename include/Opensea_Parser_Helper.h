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
#include "common_types.h"
#include "bit_manip.h"
#include "math_utils.h"
#include "libjson.h"
#include <limits.h>

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <string>

extern eVerbosityLevels g_verbosity;
extern eDataFormat g_dataformat;
extern bool	g_parseUnknown;
extern bool g_parseNULL;
extern bool g_convertHeaderToLowercase;
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

	enum class eInterfaceTypes
	{
		INTERFACE_UNKNOWN = 0,
		INTERFACE_TYPE_SCSI = 1,
		INTERFACE_TYPE_FC = 2,
		INTERFACE_TYPE_SAS = 4,
		INTERFACE_TYPE_SATA = 8,
	};

	enum class ePrintTypes
	{
		LOG_PRINT_JSON,     // default
		LOG_PRINT_TEXT,
		LOG_PRINT_CSV,
		LOG_PRINT_FLAT_CSV,
		LOG_PRINT_PROM,
		LOG_PRINT_TELEMETRY,
		LOG_PRINT_PYTHON_DICTIONARY,
		LOG_PRINT_IBT_CSV,
		LOG_PRINT_FAILURE,
		LOG_PRINT_TO_SCREEN,
	};

	enum class eLogTypes
	{
		LOG_TYPE_UNKNOWN = 0,
		LOG_TYPE_FARM,
		LOG_TYPE_DEVICE_STATISTICS_LOG,
		LOG_TYPE_EXT_COMPREHENSIVE_LOG,
		LOG_TYPE_EXT_DST_LOG,
		LOG_TYPE_IDENTIFY_LOG,
		LOG_TYPE_IDENTIFY_DEVICE_DATA,
		LOG_TYPE_SCT_TEMP_LOG,
		LOG_TYPE_POWER_CONDITION_LOG,
		LOG_TYPE_NCQ_CMD_ERROR_LOG,
		LOG_TYPE_SCSI_LOG_PAGES,

	};
	enum class eLogPageNames: int
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
	};

#pragma pack(push, 1)
	typedef struct _sLogPageStruct
	{
		uint8_t			pageCode;							//<! page code for the log lpage format
		uint8_t			subPage;							//<! subpage code for the log page format
		uint16_t		pageLength;							//<! this is different from size, see SCSI SPC Spec. 
		explicit _sLogPageStruct() : pageCode(0), subPage(0), pageLength(0) {};
        _sLogPageStruct(uint8_t* buffer)
        {
            if (buffer != M_NULLPTR)
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

	const eLogPageNames pageCodes[] = { eLogPageNames::SUPPORTED_LOG_PAGES,	eLogPageNames::WRITE_ERROR_COUNTER,
		eLogPageNames::READ_ERROR_COUNTER ,eLogPageNames::VERIFY_ERROR_COUNTER, eLogPageNames::NON_MEDIUM_ERROR ,
		eLogPageNames::FORMAT_STATUS ,	eLogPageNames::LOGICAL_BLOCK_PROVISIONING ,eLogPageNames::ENVIRONMENTAL,
		eLogPageNames::START_STOP_CYCLE_COUNTER ,	eLogPageNames::APPLICATION_CLIENT,	eLogPageNames::SELF_TEST_RESULTS,
		eLogPageNames::SOLID_STATE_MEDIA ,	eLogPageNames::ZONED_DEVICE_STATISTICS , eLogPageNames::BACKGROUND_SCAN , eLogPageNames::CACHE_MEMORY_STATISTICES,
		eLogPageNames::PROTOCOL_SPECIFIC_PORT, eLogPageNames::POWER_CONDITION_TRANSITIONS , eLogPageNames::INFORMATIONAL_EXCEPTIONS,
		eLogPageNames::CACHE_STATISTICS, eLogPageNames::SEAGATE_SPECIFIC_LOG, eLogPageNames::FACTORY_LOG,};

	M_NODISCARD static M_INLINE int16_t b_swap_int16(int16_t value)
	{
#if defined(HAVE_BUILTIN_BSWAP)
		return __builtin_bswap16(value);
#elif defined(HAVE_WIN_BSWAP)
		return _byteswap_ushort(value);
#else
		return (((value & INT16_C(0x00FF)) << 8) | ((value & INT16_C(0xFF00)) >> 8));
#endif
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn std_replace_spaces_with_underscore()
	//
	//! \brief
	//!   Description:  Converts all spaces and replaces them with underscores
	//
	//  Entry:
	//! \param myStr  = The string to be converted by replacing all the spaces with underscores
	//
	//  Exit:
	//!   \return void
	//
	//---------------------------------------------------------------------------
	inline void std_replace_spaces_with_underscore(std::string& myStr)
	{
		std::replace(myStr.begin(), myStr.end(), ' ', '_');
	}
	//-----------------------------------------------------------------------------
	//
	//! \fn std_string_to_lowercase()
	//
	//! \brief
	//!   Description:  Converts all characters in a given string to lowercase.
	//! This function takes a reference to a `std::string` and transforms all of its 
	//! characters to lowercase using the `std::transform` algorithm and `std::tolower` function.
	//
	//  Entry:
	//! \param stringToLowercase  = The string to be converted to lowercase. The conversion is done in place
	//
	//  Exit:
	//!   \return void
	//
	//---------------------------------------------------------------------------
	inline void std_string_to_lowercase(std::string& stringToLowercase)
	{
		std::transform(stringToLowercase.begin(), stringToLowercase.end(), stringToLowercase.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	}
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
			if (M_Byte7(value) != UINT8_C(0xFF))
			{
				value = value & UINT64_C(0x00FFFFFFFFFFFFFF);
			}
			else
			{
				value = 0;
			}
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
	//! \fn set_json_string_With_Status()
	//
	//! \brief
	//!   Description:  set the json values for a string and will show status bits if the flag is set
	//
	//  Entry:
	//! \param  nowNode = the Json node that the data will be added to
	//! \param  header = the string data what will be adding to
	//! \param  strValue = value in a string format
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
	inline void set_json_string_With_NO_Status(JSONNODE* nowNode, const std::string& header, const std::string& strValue)
	{
		if (header.size() != 0 && !strValue.empty())
		{
			std::string myStr = header;
			if (g_convertHeaderToLowercase)
			{
				opensea_parser::std_replace_spaces_with_underscore(myStr);
				opensea_parser::std_string_to_lowercase(myStr);
			}
			json_push_back(nowNode, json_new_a(myStr.c_str(), strValue.c_str()));
		}
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
	//! \param  header = the string data what will be adding to
	//! \param value  =  64 bit value to check to see if the bit is set or not
	//! \param hexPrint =  if true then print the data in a hex format
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
    inline void set_json_64bit_With_Check_Status(JSONNODE *nowNode,const std::string & header, uint64_t value, bool hexPrint)
    {
		std::string myStr = header;
		int64_t statusValue = 0;
		statusValue = check_Status_Strip_Status(value);
        std::ostringstream temp;
		if (g_convertHeaderToLowercase)
		{
			opensea_parser::std_replace_spaces_with_underscore(myStr);
			opensea_parser::std_string_to_lowercase(myStr);
		}
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
            if ((M_IGETBITRANGE(statusValue, 63, 59) == 0) && check_For_Active_Status(&value) == true)
            {
                json_push_back(nowNode, json_new_i(myStr.c_str(), static_cast<json_int_t>(statusValue)));
            }
			else if ((M_IGETBITRANGE(statusValue, 63, 59) == 0) && check_Status_Strip_Status(value) == 0)
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
					// if the value is greater then a unsigned 56 bit number print it as a string
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
    //! \param  header = the string data what will be adding to
    //! \param value  =  64 bit value to check to see if the bit is set or not
    //! \param hexPrint =  if true then print the data in a hex format
    //
    //  Exit:
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    inline void set_json_64bit(JSONNODE *nowNode, const std::string & header, uint64_t value, bool hexPrint)
    {
		std::string myStr = header;
        std::ostringstream temp;
		if (g_convertHeaderToLowercase)
		{
			opensea_parser::std_replace_spaces_with_underscore(myStr);
			opensea_parser::std_string_to_lowercase(myStr);
		}
        if (hexPrint)
        {
            //json does not support 64 bit numbers. Therefore we will print it as a string
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
            json_push_back(nowNode, json_new_a(myStr.c_str(), temp.str().c_str()));
        }
        else
        {
            if (M_IGETBITRANGE(value,63,59) == 0)
            {
                json_push_back(nowNode, json_new_i(myStr.c_str(), static_cast<json_int_t>(value)));
            }
            else
            {
                // if the vale is greater then a unsigned 59 bit number print it as a string
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
	//! \param  header = the string data what will be adding to
	//! \param workingValue  =  boolean value
	//
	//  Exit:
	//!   \return void
	//
	//-----------------------------------------------------------------------------
    inline void set_Json_Bool(JSONNODE *nowNode, const std::string & header, bool workingValue)
    {
		std::string myStr = header;
		if (g_convertHeaderToLowercase)
		{
			std_replace_spaces_with_underscore(myStr);
			std_string_to_lowercase(myStr);
		}
        if (workingValue)
			json_push_back(nowNode, json_new_b(myStr.c_str(), true));
        else
            json_push_back(nowNode, json_new_b(myStr.c_str(), false));
    }
	
	inline void set_Json_name(JSONNODE* nowNode, const std::string& header)
	{
		std::string myStr = header;
		if (g_convertHeaderToLowercase)
		{
			opensea_parser::std_replace_spaces_with_underscore(myStr);
			opensea_parser::std_string_to_lowercase(myStr);
		}
		json_set_name(nowNode, myStr.c_str());
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
				if (code == static_cast<uint8_t>(pageCodes[i]))
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
	
	inline eReturnValues fill_Log_Params(sLogParams &myStruct, uint8_t *buffer )
	{

		myStruct.paramCode = M_BytesTo2ByteValue(buffer[OFFSETZERO],buffer[OFFSETONE]);
		myStruct.paramControlByte = buffer[OFFSETTWO];
		myStruct.paramLength = buffer[OFFSETTHREE];
		return eReturnValues::SUCCESS;

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


