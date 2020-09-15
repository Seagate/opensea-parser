//
// Farm_Helper.cpp   farm heler
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file farm_Helper.cpp  
//
#include "Farm_Helper.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn get_Reallocation_Cause_Meanings()
//
//! \brief
//!   Description:  parse out the meaning of the reallocation
//
//  Entry:
//! \param meaning - string for the meaning of the cause
//! \param code - code for the what the meaning is
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
void opensea_parser::get_Reallocation_Cause_Meanings(std::string &meaning, uint16_t code)
{
    switch (code)
    {
    case HOST_READ_GENERIC:
        meaning = "Host Read - Generic";
        break;
    case HOST_READ_UNCORRECTABLE:
        meaning = "Host Read - Uncorrectable";
        break;
    case HOST_READ_RAW:
        meaning = "Host Read - RAW";
        break;
    case HOST_WRITE_GENERIC:
        meaning = "Host Write - Generic";
        break;
    case HOST_WRITE_UNCORRECTABLE:
        meaning = "Host Write - Uncorrectable";
        break;
    case HOST_WRITE_RAW:
        meaning = "Host Write - RAW";
        break;
    case BACKGROUND_READ_GENERIC:
        meaning = "Background Read - Generic";
        break;
    case BACKGROUND_READ_RELIABILITY:
        meaning = "Background Read - Reliability";
        break;
    case BACKGROUND_READ_RECOVERY:
        meaning = "Background Read - Recovery";
        break;
    case BACKGROUND_READ_HOST_SELF_TEST:
        meaning = "Background Read - Host Self Test";
        break;
    case BACKGROUND_WRITE_GENERIC:
        meaning = "Background Write - Generic";
        break;
    case BACKGROUND_WRITE_RELIABILITY:
        meaning = "Background Write - Reliability";
        break;
    case BACKGROUND_WRITE_RECOVERY:
        meaning = "Background Write - Recovery";
        break;
    case BACKGROUND_WRITE_HOST_SELF_TEST:
        meaning = "Background Write - Host Self Test";
        break;
    case SERVO_WEDGE:
        meaning = "Servo Wedge";
        break;
    default:
        meaning = "Unknown";
        break;
    }
}
