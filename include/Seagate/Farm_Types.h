//
// Farm_Types.h   Structures of FARM specific structures. 
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2026 Seagate Technology LLC and/or its Affiliates
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
#define ZONECOUNT               16
#define MAX_NUMBER_SENSE_DATA   44

// FARM Combine definitions ascii check values
#define FARM_CURRENT            0x4641524d20202020                      //!< "FARM    "
#define FARM_FACTORY            0x464143544f525920                      //!< "FACTORY "
#define FARM_TIME               0x4641524d54494d45                      //<! "FARMTIME"
#define FARM_STICKY             0x4641524d5354434b                      //<! "FARMSTCK"
#define FARM_WLDTR              0x574f524b4c445443                      //<! "WORKLDTR"
#define FARM_SAVE               0x4641524d53415645                      //<! "FARMSAVE"
#define FARM_LONG_SAVE          0x4641524d4c4f4e47                      //<! "FARMLONG"
#define FARM_NNW                0x52454d52414600c0                      //<! "FARMER  "

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
#define FARM_NEURAL_NETWORK                     0x10
#define FARM_AFTER_REGEN                        0x11
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
    uint64_t        historicalCopies;                           //!< Number of Historical Copies
    uint64_t        reasonForFrameCapture;                      //!< Reason for Frame Capture (added 4.14)
    uint64_t        historicalFrames;                           //!< Number of Historical frames (added 4.43)
    uint64_t        pageBitMap;                                 //!< Bit Map of Available Pages (each bit represent a supported page)

    _sFarmHeader() : signature(0), majorRev(0), minorRev(0), pagesSupported(0), logSize(0), pageSize(0), headsSupported(0), historicalCopies(0),
        reasonForFrameCapture(0), historicalFrames(0), pageBitMap(0) {};
}sFarmHeader;

#pragma pack(pop)
