// Opensea_Parser_Helper.cpp   Definition of SeaParser specific functions, structures
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file Opensea_Parser_Helper.cpp   Definition of SeaParser specific functions, structures
#include "Opensea_Parser_Helper.h"

eVerbosityLevels    g_verbosity = VERBOSITY_DEFAULT;
eDataFormat         g_dataformat = JSON_DATA;
bool                g_parseUnknown = true;
time_t              g_currentTime;
char                g_currentTimeString[64];
char                *g_currentTimeStringPtr = g_currentTimeString;
