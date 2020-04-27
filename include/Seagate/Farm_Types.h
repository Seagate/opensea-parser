//
// Farm_Types.h   Structures of FARM specific structures. 
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
// \file Farm_Types.h  
#pragma once

#include <inttypes.h>
#include "Opensea_Parser_Helper.h"
#include "Farm_Helper.h"

#define MAX_HEAD_COUNT			24
#define FARMSIGNATURE           0x00004641524D4552
#define MAJORVERSION2                2
#define MAJORVERSION3                3
#define FACTORYCOPY             0xc0464143544f5259

#pragma pack(push, 1)

typedef struct _sStringIdentifyData
{
	std::string serialNumber;									//!< seiral number of the drive
	std::string worldWideName;									//!< World Wide Name of the device
	std::string deviceInterface;								//!< Device Interface 
	std::string firmwareRev;									//!< Firmware Rev
    std::string modelNumber;                                    //!< Model Number (2.15 and newer will have this)
}sStringIdentifyData;

typedef struct _sFarmHeader
{
	uint64_t        signature;                                  //!< Log Signature = 0x00004641524D4552
	uint64_t        majorRev;                                   //!< Log Major rev
	uint64_t        minorRev;                                   //!< minor rev 
	uint64_t        pagesSupported;                             //!< number of pages supported
	uint64_t        logSize;                                    //!< log size in bytes
	uint64_t        pageSize;                                   //!< page size in bytes
	uint64_t        headsSupported;                             //!< Maximum Drive Heads Supported
	uint64_t        copies;                                     //!< Number of Historical Copies
	_sFarmHeader() : signature(0), majorRev(0), minorRev(0), pagesSupported(0), logSize(0), pageSize(0), headsSupported(0), copies(0) {};
}sFarmHeader;

typedef struct _sWorkLoadStat
{
	uint64_t        pageNumber;                                 //!< Page Number = 2
	uint64_t        copyNumber;                                 //!< Copy Number
	uint64_t        workloadPercentage;                         //!< rated Workload Percentage
	uint64_t        totalReadCommands;                          //!< Total Number of Read Commands
	uint64_t        totalWriteCommands;                         //!< Total Number of Write Commands
	uint64_t        totalRandomReads;                           //!< Total Number of Random Read Commands
	uint64_t        totalRandomWrites;                          //!< Total Number of Random Write Commands
	uint64_t        totalNumberofOtherCMDS;                     //!< Total Number Of Other Commands
	uint64_t        logicalSecWritten;                          //!< Logical Sectors Written
	uint64_t        logicalSecRead;                             //!< Logical Sectors Read
    union
    {
        uint64_t        dither;                                     //!< ATA Number of dither events during current power cycle (added 3.4)
        uint64_t        totalReadCmdsFromFrames;                    //!< SCSI Total Read Commands by Radius from last 3 SMART Summary Frames 
    };
    union
    {
        uint64_t        ditherRandom;                               //!< ATA Number of times dither was held off during random workloads during current power cycle(added 3.4)
        uint64_t        totalWriteCmdsFromFrames;                   //!< SCSI Total Write Commands by Radius from last 3 SMART Summary Frames 
    };
    uint64_t        ditherSequential;                           //!< Number of times dither was held off during sequential workloads during current power cycle(added 3.4)
	_sWorkLoadStat() : pageNumber(0), copyNumber(0), workloadPercentage(0), totalReadCommands(0), totalWriteCommands(0), totalRandomReads(0), totalRandomWrites(0), \
		totalNumberofOtherCMDS(0), logicalSecWritten(0), logicalSecRead(0), dither(0), ditherRandom(0), ditherSequential(0) {};
}sWorkLoadStat;

#pragma pack(pop)


