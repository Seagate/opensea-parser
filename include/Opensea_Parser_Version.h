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
// \file Opensea_Parser_Version.h
// \brief Defines the versioning information for opensea-parser


#pragma once

#define COMBINE_PARSER_VERSIONS_(x,y,z) #x "." #y "." #z
#define COMBINE_PARSER_VERSIONS(x,y,z) COMBINE_PARSER_VERSIONS_(x,y,z)

#define OPENSEA_PARSER_MAJOR_VERSION	1
#define OPENSEA_PARSER_MINOR_VERSION	0
#define OPENSEA_PARSER_PATCH_VERSION	2

#define OPENSEA_PARSER_VERSION COMBINE_PARSER_VERSIONS(OPENSEA_PARSER_MAJOR_VERSION,OPENSEA_PARSER_MINOR_VERSION,OPENSEA_PARSER_PATCH_VERSION)


