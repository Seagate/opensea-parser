//
// Farm_Types.h   Structures of FARM specific structures. 
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
// \file Farm_Types.h  
#pragma once

#include <inttypes.h>

#define MAX_HEAD_COUNT			24
#define FARMSIGNATURE           0x00004641524D4552
#define MAJORVERSION2           2
#define MAJORVERSION3           3
#define MAJORVERSION4           4
#define FACTORYCOPY             0x00464143544f5259
#define FARMEMPTYSIGNATURE      0x00FFFFFFFFFFFFFF
#define FARMPADDINGSIGNATURE    0x0000000000000000
#define DATE_YEAR_DATE_SIZE     2
#define FLASH_EVENTS            8
#define REALLOCATIONEVENTS      15
#define PRINTABLE_MODEL_NUMBER  12

// FARM Combine definitions ascii check values
#define FARM_CURRENT            0x4641524d20202020                      //!< "FARM    "
#define FARM_FACTORY            0x464143544f525920                      //!< "FACTORY "
#define FARM_TIME               0x4641524d54494d45                      //<! "FARMTIME"
#define FARM_STICKY             0x4641524d5354434b                      //<! "FARMSTCK"
#define FARM_WLDTR              0x574f524b4c445443                      //<! "WORKLDTR"
#define FARM_SAVE               0x4641524d53415645                      //<! "FARMSAVE"
#define FARM_LONG_SAVE          0x4641524d4c4f4e47                      //<! "FARMLONG"

// Frame Type Identification  0x0 - 0xF
#define CURRENT_FRAME                           0x00
#define TIME_SERIES_FRAME                       0x01
#define LONG_TERM_FRAME                         0x02
#define GLIST_DISC_ENTRIES_FRAME                0x03
#define FIRST_UNRECOVERED_READ_ERROR_FRAME      0x04
#define TENTH_UNRECOVERED_READ_ERROR_FRAME      0x05
#define FIRST_FATAL_CTO_FRAME                   0x06
#define BEFORE_CFW_UPDATE_FRAME                 0x07
#define TEMP_EXCEDED_FRAME                      0x08
#define RESERVED_FRAME                          0x09
#define FACTORY_COPY_FRAME                      0x0A
#define FARM_LOG_DISC_COPY_FRAME                0x0B  //sata only
#define IDD_FARM_LOG_DISC_FRAME                 0x0C  //sata only
#define FARM_DRAM_COPY_FARME                    0x0D  //sata only
#define FARM_DRAM_COPY_DIAG_FRAME               0x0E  
#define FARM_UDS_COPY_FRAME                     0x0F
#define FARM_EMPTY_FRAME                        0xff

// SAS log page information
#define FARMLOGPAGE                             0x3D

#define FARM_LOG_PAGE                           0x03
#define FARM_FACTORY_LOG_PAGE                   0x04
#define FARM_TIME_SERIES_0                      0x10
#define FARM_TIME_SERIES_1                      0x11
#define FARM_TIME_SERIES_2                      0x12
#define FARM_TIME_SERIES_3                      0x13
#define FARM_TIME_SERIES_4                      0x14
#define FARM_TIME_SERIES_5                      0x15
#define FARM_TIME_SERIES_6                      0x16
#define FARM_TIME_SERIES_7                      0x17
#define FARM_TIME_SERIES_8                      0x18
#define FARM_TIME_SERIES_9                      0x19
#define FARM_TIME_SERIES_10                     0x1A
#define FARM_TIME_SERIES_11                     0x1B
#define FARM_TIME_SERIES_12                     0x1C
#define FARM_TIME_SERIES_13                     0x1D
#define FARM_TIME_SERIES_14                     0x1E
#define FARM_TIME_SERIES_15                     0x1F
#define FARM_LONG_TERM_SAVES_0                  0xC0
#define FARM_LONG_TERM_SAVES_1                  0xC1
#define FARM_GLIST_ENTRIES                      0xC2
#define FARM_FIRST_UNRECOVERABLE                0xC3
#define FARM_TENTH_UNRECOVERABLE                0xC4
#define FARM_FIRST_CTO                          0xC5
#define FARM_LAST_FRAME                         0xC6
#define FARM_TEMP_TRIGGER_LOG_PAGE              0xC7

#pragma pack(push, 1)
typedef struct _sStringIdentifyData
{
    std::string serialNumber;									//!< seiral number of the drive
    std::string worldWideName;									//!< World Wide Name of the device
    std::string deviceInterface;								//!< Device Interface 
    std::string firmwareRev;									//!< Firmware Rev
    std::string modelNumber;                                    //!< Model Number (4.3 and newer will have this)
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
    uint64_t        reserved;                                   //!< Reserved
    uint64_t        reasonForFrameCapture;                      //!< Reason for Frame Capture (added 4.14)
    _sFarmHeader() : signature(0), majorRev(0), minorRev(0), pagesSupported(0), logSize(0), pageSize(0), headsSupported(0), reserved(0), reasonForFrameCapture(0) {};
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
    union 
    {
        uint64_t        numberOfWriteCmds2;                         //!< ATA - Number of Write commands from 3.125-25% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        numReadTransferSmall;                      //!< SCSI - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
    };
    union
    {
        uint64_t        numberOfWriteCmds3;                         //!< ATA - Number of Write commands from 25-50% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        numReadTransferMid1;                      //!< SCSI - Number of Read Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames
    };
    union
    {
        uint64_t        numberOfWriteCmds4;                         //!< ATA - Number of Write commands from 50-100% of LBA space for last 3 SMART Summary Frames (added 4.4)
        uint64_t        numReadTransferMid2;                      //!< SCSI - Number of read Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
    };
    union 
    {
        uint64_t        numReadTransferLarge;                      //!< SCSI - Number of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames
        uint64_t        numReadTransferSmallATA;                      //!< ATA - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
    };
    
    union 
    {
        uint64_t        numWriteTransferSmall;                      //!< SCSI - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
        uint64_t        numReadTransferMid1ATA;                      //!< ATA - Number of Read Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames    
    };
    union 
    {
        uint64_t        numWriteTransferMid1;                       //!< SCSI - Number of Write Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames
        uint64_t        numReadTransferMid2ATA;                      //!< ATA - Number of Read Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
    };
    union 
    {
        uint64_t        numWriteTransferMid2;                       //!< SCSI - Number of Write Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
        uint64_t        numReadTransferLargeATA;                      //!< ATA - Number of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames    };
    };
    union 
    {
        uint64_t        numWriteTransferLarge;                      //!< SCSI - Number of Write Commands of transfer length > 2MB for last 3 SMART Summary Frames
        uint64_t        numWriteTransferSmallATA;                      //!< ATA - Number of Write Commands of transfer length <=16KB for last 3 SMART Summary Frames
    };

    uint64_t        numWriteTransferMid1ATA;                      //!< ATA - Number of Read Commands of transfer length (16KB – 512KB] for last 3 SMART Summary Frames
    uint64_t        numWriteTransferMid2ATA;                      //!< ATA - Number of Read Commands of transfer length (512KB – 2MB] for last 3 SMART Summary Frames
    uint64_t        numWriteTransferLargeATA;                     //!< ATA - Number of Read Commands of transfer length > 2MB for last 3 SMART Summary Frames 
    uint64_t        cntQueueDepth1;                               //!< ATA - Count of Queue Depth =1 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth2;                               //!< ATA - Count of Queue Depth =2 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth3to4;                            //!< ATA - Count of Queue Depth =3-4 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth5to8;                            //!< ATA - Count of Queue Depth =5-8 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth9to16;                           //!< ATA - Count of Queue Depth =9-16 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth17to32;                          //!< ATA - Count of Queue Depth =17-32 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepth33to64;                          //!< ATA - Count of Queue Depth =33-64 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        cntQueueDepthmorethan64;                      //!< ATA - Count of Queue Depth >64 at 30s intervals for last 3 SMART Summary Frames
    uint64_t        numDithEvtAct1;                               //!< ATA - Number of dither events during current power cycle, Actuator 1
    uint64_t        numRandWLDitherHoldOffAct1;                   //!< ATA - Number of times dither was held off during random workloads during current power cycle, Actuator 1
    uint64_t        numSequentialWLDitherHoldOffAct1;             //!< ATA - Number of times dither was held off during sequential workloads during current power cycle, Actuator 1
    uint64_t        reserved[350];



    _sWorkLoadStat() : pageNumber(0), copyNumber(0), workloadPercentage(0), totalReadCommands(0), totalWriteCommands(0), totalRandomReads(0), totalRandomWrites(0),
        totalNumberofOtherCMDS(0), logicalSecWritten(0), logicalSecRead(0), dither(0), ditherRandom(0), ditherSequential(0), numberOfReadCmds1(0), numberOfReadCmds2(0),
        numberOfReadCmds3(0), numberOfReadCmds4(0), numberOfWriteCmds1(0), numberOfWriteCmds2(0), numberOfWriteCmds3(0), numberOfWriteCmds4(0),
        numReadTransferLarge(0), numWriteTransferSmall(0), numWriteTransferMid1(0), numWriteTransferMid2(0), numWriteTransferLarge(0),
        numWriteTransferMid1ATA(0), numWriteTransferMid2ATA(0), numWriteTransferLargeATA(0), cntQueueDepth1(0), cntQueueDepth2(0),
        cntQueueDepth3to4(0), cntQueueDepth5to8(0), cntQueueDepth9to16(0), cntQueueDepth17to32(0), cntQueueDepth33to64(0),
        cntQueueDepthmorethan64(0), numDithEvtAct1(0), numRandWLDitherHoldOffAct1(0), numSequentialWLDitherHoldOffAct1(0)
//#if defined __cplusplus && __cplusplus >= 201103L
        ,reserved {0}
//#endif
    {};
}sWorkLoadStat;


#pragma pack(pop)
