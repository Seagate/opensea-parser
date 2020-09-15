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
#define MAJORVERSION2           2
#define MAJORVERSION3           3
#define MAJORVERSION4           4
#define FACTORYCOPY             0xc0464143544f5259



#pragma pack(push, 1)
typedef enum _eReallocationCauses
{
    HOST_READ_GENERIC,
    HOST_READ_UNCORRECTABLE,
    HOST_READ_RAW,
    HOST_WRITE_GENERIC,
    HOST_WRITE_UNCORRECTABLE,
    HOST_WRITE_RAW,
    BACKGROUND_READ_GENERIC,
    BACKGROUND_READ_RELIABILITY,
    BACKGROUND_READ_RECOVERY,
    BACKGROUND_READ_HOST_SELF_TEST,
    BACKGROUND_WRITE_GENERIC,
    BACKGROUND_WRITE_RELIABILITY,
    BACKGROUND_WRITE_RECOVERY,
    BACKGROUND_WRITE_HOST_SELF_TEST,
    SERVO_WEDGE,
}eReallocationCauses;


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
    uint64_t        reasonForFrameCpature;                      //!< Reason for Frame Capture (added 4.14)
    _sFarmHeader() : signature(0), majorRev(0), minorRev(0), pagesSupported(0), logSize(0), pageSize(0), headsSupported(0), copies(0), reasonForFrameCpature(0) {};
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
        uint64_t        dither;                                 //!< ATA - Number of dither events during current power cycle (added 3.4)
        uint64_t        totalReadCmdsFromFrames1;               //!< SCSI - Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        ditherRandom;                           //!< ATA - Number of times dither was held off during random workloads during current power cycle(added 3.4)
        uint64_t        totalReadCmdsFromFrames2;               //!< SCSI - Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        ditherSequential;                       //!< ATA - Number of times dither was held off during sequential workloads during current power cycle(added 3.4)
        uint64_t        totalReadCmdsFromFrames3;               //!< SCSI - Number of Read commands from 25-75% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds1;                      //!< ATA - Number of Read commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalReadCmdsFromFrames4;               //!< SCSI - Number of Read commands from 75-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds2;                      //!< ATA - Number of Read commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames1;              //!< SCSI - Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds3;                      //!< ATA - Number of Read commands from 25-75% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames2;              //!< SCSI - Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfReadCmds4;                      //!< ATA - Number of Read commands from 75-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames3;              //!< SCSI - Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    union
    {
        uint64_t        numberOfWriteCmds1;                     //!< ATA - Number of Write commands from 0-3.125% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        totalWriteCmdsFromFrames4;              //!< SCSI - Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
    };
    uint64_t        numberOfWriteCmds2;                         //!< ATA - Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
    uint64_t        numberOfWriteCmds3;                         //!< ATA - Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames (added 4.4)
    uint64_t        numberOfWriteCmds4;                         //!< ATA - Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
    _sWorkLoadStat() : pageNumber(0), copyNumber(0), workloadPercentage(0), totalReadCommands(0), totalWriteCommands(0), totalRandomReads(0), totalRandomWrites(0), \
        totalNumberofOtherCMDS(0), logicalSecWritten(0), logicalSecRead(0), dither(0), ditherRandom(0), ditherSequential(0), numberOfReadCmds1(0), numberOfReadCmds2(0), \
        numberOfReadCmds3(0), numberOfReadCmds4(0), numberOfWriteCmds1(0), numberOfWriteCmds2(0), numberOfWriteCmds3(0), numberOfWriteCmds4(0) {};
}sWorkLoadStat;

#pragma pack(pop)

