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