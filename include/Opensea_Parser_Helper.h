//
// Opensea_Parser_Helper.h   Definition of SM2 specific structures. 
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
// \file Opensea_Parser_Helper.h   Definition of SeaParser specific functions, structures
#pragma once
#include <time.h>
#include "common.h"
#include "libjson.h"

extern eVerbosityLevels g_verbosity;
extern time_t g_currentTime;
extern char g_currentTimeString[64];
extern char *g_currentTimeStringPtr;

namespace opensea_parser {

#define RELISTAT                24
#define WORLD_WIDE_NAME_LEN     18
#define DEVICE_INTERFACE_LEN    4
#define SERIAL_NUMBER_LEN       8
#define MODEL_NUMBER_LEN        20
#define FIRMWARE_REV_LEN        4
#define BASIC                   80


	
	
	typedef enum _eReturnParserValues
	{
		OPENSEA_PARSER_SUCCESS = 0,
		OPENSEA_PARSER_FAILURE = 1,
		OPENSEA_PARSER_NOT_SUPPORTED = 2,
		OPENSEA_PARSER_PARSER_FAILURE = 3,
		OPENSEA_PARSER_IN_PROGRESS = 4,			//another command is in progress, or a command has started and is now in progress in the background
		OPENSEA_PARSER_ABORTED = 5,				//something bad happend and we need to aborted
		OPENSEA_PARSER_BAD_PARAMETER = 6,			//within a structure or log where the parameters don't match the spec 
		OPENSEA_PARSER_MEMORY_FAILURE = 7,			//could not allocate memory
		OPENSEA_PARSER_INVALID_LENGTH = 9,			//log size is invalid, a spec issue or a parameter issue on the lengths of the data.
		OPENSEA_PARSER_FILE_OPEN_ERROR = 10,
	}eReturnParserValues;

    // output file types
    enum eOpensea_print_Types
    {
        OPENSEA_LOG_PRINT_JSON,     // default
        OPENSEA_LOG_PRINT_TEXT,
        OPENSEA_LOG_PRINT_CSV,
        OPENSEA_LOG_PRINT_FLAT_CSV,
    };
	// SCSI Parameter Control Bytes
	enum eOpenSea_SCSI_Log_Parameter_Types
	{
		OPENSEA_SCSI_LOG_BOUNDED_DATA_COUNTER = 0,   //
		OPENSEA_SCSI_LOG_ASCII_FORMAT_LIST,
		OPENSEA_SCSI_LOG_BOUNDED_DATA_COUNTER_OR_UNBOUNDED_DATA_COUNTER,
		OPENSEA_SCSI_LOG_BINARY_FORMAT_LIST,
	};

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
		SEAGATE_FARM_LOG = 0x3D,
		FACTORY_LOG = 0x3E,
	}eLogPageNames;


	typedef struct _sLogPageStruct
	{
		uint8_t			pageCode;							//<! page code for the log lpage format
		uint8_t			subPage;							//<! subpage code for the log page format
		uint16_t		pageLength;							//<! this is different from size, see SCSI SPC Spec. 
        _sLogPageStruct(): pageCode(0),subPage(0),pageLength(0){};
	}sLogPageStruct;

    inline void set_json_64bit(JSONNODE *nowNode, const std::string & myStr, uint64_t value, bool hexPrint)
    {
        std::string printStr = " ";
        printStr.resize(BASIC);
        std::string lowStr = "64 bit Value Lower value";
        std::string upperStr = "64 bit Value Upper value";
        uint32_t lowValue = static_cast<uint32_t>(value);
        uint32_t upperValue = static_cast<uint32_t>(value >> 32);

        JSONNODE *bigBit = json_new(JSON_NODE);
        json_set_name(bigBit, (char *)myStr.c_str());
        if (hexPrint)
        {
            json_push_back(bigBit, json_new_b("64 bit Value String in Hex", true));
            snprintf((char*)printStr.c_str(), BASIC, "0x%" PRIx64"", value);
            json_push_back(bigBit, json_new_a("64 bit Value String", (char*)printStr.c_str()));
            json_push_back(bigBit, json_new_i((char*)lowStr.c_str(), lowValue));
            json_push_back(bigBit, json_new_i((char*)upperStr.c_str(), upperValue));
        }
        else
        {
            json_push_back(bigBit, json_new_b("64 bit Value String in Hex", false));
            snprintf((char*)printStr.c_str(), BASIC, "%" PRIu64"", value);
            json_push_back(bigBit, json_new_a("64 bit Value String", (char*)printStr.c_str()));
            json_push_back(bigBit, json_new_i((char*)lowStr.c_str(), lowValue));
            json_push_back(bigBit, json_new_i((char*)upperStr.c_str(), upperValue));
        }
        json_push_back(nowNode, bigBit);
    }

    inline void set_Json_Bool(JSONNODE *nowNode, const std::string & myStr, bool workingValue)
    {
        if (workingValue)
            json_push_back(nowNode, json_new_b((char*)myStr.c_str(), true));
        else
            json_push_back(nowNode, json_new_b((char*)myStr.c_str(), false));
    }

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
}

