//
// Farm_Helper.h   Farm Related Functions
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
// \file Farm_Helper.h  
#pragma once

#include <inttypes.h>
#include <string>
#include "common.h"
#include "Farm_Types.h"


namespace opensea_parser {
#ifndef FARMCOMMON
#define FARMCOMMON

	class CFarmCommon 
	{
		public:
			CFarmCommon();
			virtual ~CFarmCommon();
			void get_Reallocation_Cause_Meanings(std::string &meaning, uint16_t code);
			void get_Assert_Code_Meaning(std::string &meaning, uint16_t code);
			void create_Year_Assembled_String(std::string &dateStr, uint16_t date, bool isSAS);
	};
    
#endif 
}
