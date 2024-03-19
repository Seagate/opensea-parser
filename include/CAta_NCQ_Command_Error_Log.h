//
// CAta_NCQ_Command_Error_Log.h   parser the NCQ Command Error log
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************//
// \file CAta_NCQ_Command_Error_Log.h   
// \brief
#pragma once
#include <vector>
#include "common.h"
#include "CLog.h"
#include "libjson.h"
#include "Opensea_Parser_Helper.h"


namespace opensea_parser {
#ifndef ATANCQCMDERROR
#define ATANCQCMDERROR

#define MAX_QUEUE           32
#pragma pack(push, 1)
	typedef struct _sNCQError
	{
		uint8_t         NCQbit;         //!< ncq bit name
		uint8_t         reserved;
		uint8_t         status;
		uint8_t         error;
		uint8_t         lba1;
		uint8_t         lba2;
		uint8_t         lba3;
		uint8_t         device;
		uint8_t         lba4;
		uint8_t         lba5;
		uint8_t         lba6;
		uint8_t         reserved1;
		uint16_t        count;
		uint8_t         senseKey;
		uint8_t         senseCodeField;
		uint8_t         senseCodeQualifier;
		uint64_t        finalLBA;

		_sNCQError() : NCQbit(0), reserved(0), status(0), error(0), lba1(0), lba2(0), lba3(0), device(0), 
			lba4(0), lba5(0), lba6(0), reserved1(0), count(0), senseKey(0), senseCodeField(0), senseCodeQualifier(0), finalLBA(0) {};
	}sNCQError;


#pragma pack(pop)
    class CAta_NCQ_Command_Error_Log 
    {
    private:
        std::string             m_name;                                 //!< name of the class
        eReturnValues           m_status;                               //!< the status of the class  
		
		std::vector <sNCQError > vNCQFrame;								//!< vector of the structure


        bool get_Bit_Name_Info(JSONNODE *NCQInfo, sNCQError* ncqError);
        uint64_t create_LBA( sNCQError* ncqError);
    public:
        CAta_NCQ_Command_Error_Log();
		explicit CAta_NCQ_Command_Error_Log(const std::string & fileName);
		explicit CAta_NCQ_Command_Error_Log(uint8_t *buffer,size_t length);
        ~CAta_NCQ_Command_Error_Log();
		eReturnValues get_NCQ_Command_Error_Log_Status() { return m_status; };
        eReturnValues get_NCQ_Command_Error_Log(JSONNODE *masterData);
    };
#endif  //ATANCQCMDERROR
}