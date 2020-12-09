//
// Identify_Log_Types.h   Identify structure types
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
// \file Identify_Log_Types.h  

#include <inttypes.h>
#include "Opensea_Parser_Helper.h"

namespace opensea_parser {
#ifndef ATAIDENTIFYTYPES
#define ATAIDENTIFYTYPES

#define MAX_NUMBER_OF_ENTRIES   9
#define LEGACY_DRIVE_SEC_SIZE 512
#define MAX_48_BIT_LBA                (0xFFFFFFFFFFFFULL)
#pragma pack(push, 1)

	typedef struct _sSectorSizes
	{
		bool                    sectorSizeReported;
		uint32_t                logicalSectorSize;
		uint32_t                physicalSectorSize;
		uint8_t                 sectorSizeExponent;
		_sSectorSizes() : sectorSizeReported(false), logicalSectorSize(0), physicalSectorSize(0), sectorSizeExponent(0) {};
	}sSectorSizes;

	typedef struct _sTransportInfomation
	{
		bool                    transportReported;
		uint16_t                transportBits;
		uint16_t                transportMinorVersion;
		uint8_t                 transportType;
		uint8_t                 transportCounter;
		_sTransportInfomation() : transportReported(false), transportBits(0), transportMinorVersion(0), transportType(0), transportCounter(0) {};
	}sTransportInfo;

	typedef struct _sIDCapabilities
	{
		bool                    requestSenseDeviceFault;
		bool                    DSN;
		bool                    lowPowerStandby;
		bool                    setEPCPowerSource;
		bool                    maxAddr;
		bool                    DRAT;
		bool                    lpsMisalignmentReporting;
		bool                    readBufferDMA;
		bool                    writeBufferDMA;
		bool                    downloadMicrocodeDMA;
		bool                    bit28;
		bool                    rZAT;
		bool                    nOP;
		bool                    readBuffer;
		bool                    WriteBuffer;
		bool                    readLookAhead;
		bool                    volatileWriteCache;
		bool                    SMART;
		bool                    flushCacheExt;
		bool                    bit48;
		bool                    spinUp;
		bool                    puis;
		bool                    APM;
        bool                    CFA;
		bool                    downloadMicrocode;
		bool                    unload;
		bool                    writeFUAExt;
		bool                    GPL;
		bool                    Streaming;
		bool                    smartSelfTest;
		bool                    smartErrorLogging;
		bool                    EPC;
		bool                    senseData;
		bool                    freeFall;
		bool                    DMMode3;
		bool                    GPL_DM;
		bool                    writeUncorrectable;
		bool                    WRV;
		_sIDCapabilities() : requestSenseDeviceFault(false), DSN(false), lowPowerStandby(false), setEPCPowerSource(false), maxAddr(false), \
			DRAT(false), lpsMisalignmentReporting(false), readBufferDMA(false), writeBufferDMA(false), \
			downloadMicrocodeDMA(false), bit28(false), rZAT(false), nOP(false), readBuffer(false), WriteBuffer(false), \
			readLookAhead(false), volatileWriteCache(false), SMART(false), flushCacheExt(false), bit48(false), \
			spinUp(false), puis(false), APM(false), CFA(false), downloadMicrocode(false), unload(false), writeFUAExt(false), \
			GPL(false), Streaming(false), smartSelfTest(false), smartErrorLogging(false), EPC(false), senseData(false), \
			freeFall(false), DMMode3(false), GPL_DM(false), writeUncorrectable(false), WRV(false) {};
	}sIDCapabilities;

	typedef struct _sSecurityInformation
	{
		bool                    supported;
		uint8_t                 masterPasswordCapability;
		bool                    enhancedSecurityEraseSupported;
		bool                    securityCountExpired;
		bool                    frozen;
		bool                    locked;
		bool                    enabled;
		_sSecurityInformation() : supported(false), masterPasswordCapability(0), enhancedSecurityEraseSupported(false), securityCountExpired(false), frozen(false), locked(false), enabled(false) {};
	}sSecurityinformation;

	typedef struct _sCapacityUnitInformation
	{
		double                  capUnit;
		std::string               capacityUnit;
		_sCapacityUnitInformation() : capUnit(0), capacityUnit("    ") {};
	}sCapacityInformation;

	typedef struct _sDriveInformation
	{
		std::string             serialNumber;
		std::string             modelNumber;
		std::string             firmware;
		std::string             worldWideName;
		std::string             ieeeOUI;
		std::string             uniqueID;
		uint64_t                IDDevCap;
		uint64_t                IDPhySecSize;
		uint64_t                IDLogSecSize;
		uint64_t                IDBufSize;
		uint64_t                IDCapabilities;
		uint64_t                IDMicrocode;
		uint64_t                IDMediaRotRate;
		uint64_t                IDFormFactor;
		uint64_t                IDWRVSecCount2;
		uint64_t                IDWRVSecCount3;
		uint64_t                IDWWN;
		bool                    smartFeatureEnabled;
		bool                    gplFeatureSupported;
		bool                    readWriteLogExtDMASupported;
		bool                    fourtyEightBitSupported;
		uint16_t                ataReportedLogicalSectorSize;                               // this will be overridden if the logical sector size of the child drive is larger than this
		bool                    isAtaSSD;                                                   // will be set to true if we read the rotation rate as 0x0001
		bool                    tcgSupported;
		bool                    dmaSupported;
		bool                    extLBA;
		bool                    sctSupported;
		bool                    sctDataTables;
		bool                    multipleValid;
		uint16_t                numDRQBlocks;
		uint16_t                identRotationRate;
		uint32_t                maxLBA28;
		uint64_t                maxLBA48;
		uint32_t                logicalSectorAlignment;
		uint8_t                 sataGenSupported;
		uint8_t                 sataNegotiated;
		uint8_t                 ataSpecCounter;                                             // counter for quick printing of the ata spec version ( print use only)
		uint16_t                ataSpecBits;
		uint16_t                minorSpecVersionNumber;
		uint16_t                formFactor;
		uint64_t                IDSecurityStatus;
		uint16_t                SecurityID;
		sCapacityInformation    sCapInfo;
		sSectorSizes            sSizes;
		sTransportInfo          sTrans;
		sIDCapabilities         sIdentCap;
		sSecurityinformation    sSecurityInfo;
		_sDriveInformation() :serialNumber(""), modelNumber(""), firmware(""), worldWideName(""), ieeeOUI(""), uniqueID(""), \
			IDDevCap(0), IDPhySecSize(0), IDLogSecSize(0), IDBufSize(0), IDCapabilities(0), IDMicrocode(0), IDMediaRotRate(0), IDFormFactor(0), IDWRVSecCount2(0), \
			IDWRVSecCount3(0), IDWWN(0), smartFeatureEnabled(false), gplFeatureSupported(false), readWriteLogExtDMASupported(false), fourtyEightBitSupported(false), ataReportedLogicalSectorSize(0), \
			isAtaSSD(false), tcgSupported(false), dmaSupported(false), extLBA(false), sctSupported(false), sctDataTables(false), multipleValid(false), numDRQBlocks(0), identRotationRate(0), \
			maxLBA28(0), maxLBA48(0), logicalSectorAlignment(0), sataGenSupported(0), sataNegotiated(0), ataSpecCounter(0), ataSpecBits(0), minorSpecVersionNumber(0), \
			formFactor(0), IDSecurityStatus(0), SecurityID(0), sCapInfo(), sSizes(), sTrans(), sIdentCap(), sSecurityInfo() {};
	}sDriveInformation;

	typedef struct _sLogPage00
	{
		uint64_t        header;                                             //<! data log information header                                            0 - 7
		uint8_t         entries;                                            //<! Number of entries in the log                                           8 
		uint8_t         pageSupported[MAX_NUMBER_OF_ENTRIES];               //<! shall be cleared to zero to show that it is supported                  9
	}sLogPage00;

	typedef struct _sLogPage02
	{
		uint64_t        header;                                             //<! data log information header                                            0 - 7
		uint64_t        deviceCapacity;                                     //<! device Capacity                                                       8 - 15
		uint64_t        sectorSize;                                         //<! Physical / Logical Sector size                                         16 - 23
		uint64_t        logicalSize;                                        //<! Logical Sector Size                                                    24 - 31
		uint64_t        bufferSize;                                         //<! Normal Sector Size                                                     32 - 39
		_sLogPage02() : header(0), deviceCapacity(0), sectorSize(0), logicalSize(0), bufferSize(0) {};
	}sLogPage02;

	typedef struct _sLogPage03
	{
		uint64_t        header;                                             //<! data log information header                                            0 - 7
		uint64_t        supportedCapabilities;                              //<! supported Capabilities                                                 8 - 15
		uint64_t        download;                                           //<! DownLoad Microcode Capabiliites                                        16 - 23
		uint64_t        rotationRate;                                       //<! Rotation Rate                                                          24 - 31
		uint64_t        formFactor;                                         //<! form factor                                                            32 - 39
		uint64_t        mode3;                                              //<! write read verify mode 3                                               40 - 47
		uint64_t        mode2;                                              //<! write read verify mode 2                                               48 - 55
		uint16_t        wwn;                                                //<! world wide name                                                        56 - 71
		uint16_t        wwn1;
		uint16_t        wwn2;
		uint16_t        wwn3;
		uint64_t        wwnStatus;                                          //<! bit 128 will hold the status bit
		uint64_t        dataSet;                                            //<! Data Set managment                                                     72 - 79
		uint64_t        utilization;                                        //<! Utilization Per Unit Time                                              80 - 95
		uint64_t        utilTime2;                                          //<! uppper bits to the 
		uint64_t        utilRate;                                           //<! Utilization Usage Rate Supported                                       96 - 103
		uint64_t        zoned;                                              //<! Zoned Capabilities                                                     104 - 111
		uint64_t        ZAC;                                                //<! Supported ZAC Capabilites                                              112 - 119
		uint64_t        backgroundOpsCapabilities;                          //<! Advanced Background Operations Capabilities                            120 - 127
		uint64_t        backgroundOpsRecommendations;                       //<! Advanced Background Operations Recommendations                         128 - 135
		uint64_t        queueDepth;                                         //<! Queue depth                                                            136 - 143
		uint64_t        sctCapabilities;                                    //<! supported SCT Capabilities                                             144 - 151
		uint64_t        depopCapabilities;                                  //<! Depopulation Capabilities                                              152 - 159
		uint64_t        depopExTime;                                        //<! Depopulation Execution Time                                            160 - 167
		_sLogPage03() : header(0), supportedCapabilities(0), download(0), rotationRate(0), formFactor(0), mode3(0), mode2(0), \
			wwn(0), wwn1(0), wwn2(0), wwn3(0), wwnStatus(0), dataSet(0), utilization(0), utilTime2(0), utilRate(0), zoned(0), ZAC(0), \
			backgroundOpsCapabilities(0), backgroundOpsRecommendations(0), queueDepth(0), sctCapabilities(0), depopCapabilities(0), depopExTime(0) {};
	}sLogPage03;

	typedef struct _sSupportedCapabilities
	{
		bool        senseDeviceFaultSupported;			                            //<! REQUEST SENSE DEVICE FAULT SUPPORTED bit  bit 45       	
		bool        DSNSupported;					                                //<! DSN SUPPORTED bit                         bit 44       	
		bool        lowPowerStandbySupported;			                            //<! LOW POWER STANDBY SUPPORTED bit           bit 43       	
		bool        setEPCPowerSourceSupported;		                                //<! SET EPC POWER SOURCE SUPPORTED bit        bit 42       	
		bool        AMAXAddrSupported;				                                //<! AMAX ADDR SUPPORTED bit                   bit 41       	
		bool        reserved8;					                                    //<! Reserved for CFA                          bit 40       	
		bool        DRATSupported;					                                //<! DRAT SUPPORTED bit                        bit 39       	
		bool        LPSMisalignmentReportingSupported;		                        //<! LPS MISALIGNMENT REPORTING SUPPORTED bit  bit 38       	
		bool        reserved7;					                                    //<! Reserved                                  bit 37       	
		bool        readBufferDMASupported;				                            //<! READ BUFFER DMA SUPPORTED bit             bit 36       	
		bool        writeBufferDMASupported;				                        //<! WRITE BUFFER DMA SUPPORTED bit            bit 35       	
		bool        reserved6;     					                                //<! Reserved                                  bit 34       	
		bool        downLoadMicroCodeDMASupported;			                        //<! DOWNLOAD MICROCODE DMA SUPPORTED bit      bit 33       	
		bool        bit28Supported;					                                //<! 28-BIT SUPPORTED bit                      bit 32       	
		bool        RZATSupported;					                                //<! RZAT SUPPORTED bit                        bit 31       	
		bool        reserved5;					                                    //<! Reserved                                  bit 30       	
		bool        NOPSupported;					                                //<! NOP SUPPORTED bit                         bit 29       	
		bool        readBufferSupported;				                            //<! READ BUFFER SUPPORTED bit                 bit 28       	
		bool        writeBufferSupported;				                            //<! WRITE BUFFER SUPPORTED bit                bit 27       	
		bool        reserved4;					                                    //<! Reserved                                  bit 26       	
		bool        readLookAheadSupported;				                            //<! READ LOOK-AHEAD SUPPORTED bit             bit 25       	
		bool        volatileWriteCacheSupported;			                        //<! VOLATILE WRITE CACHE SUPPORTED bit        bit 24       	
		bool        smartSupported;					                                //<! SMART bit                                 bit 23       	
		bool        flushCacheExtSupported;				                            //<! FLUSH CACHE EXT SUPPORTED bit             bit 22       	
		bool        reserved3;					                                    //<! Reserved                                  bit 21       	
		bool        bit48Supported;					                                //<! 48-BIT SUPPORTED bit                      bit 20       	
		bool        reserved2;					                                    //<! Reserved                                  bit 19       	
		bool        spinupSupported;					                            //<! SPIN-UP SUPPORTED bit                     bit 18       	
		bool        PUISSupported;					                                //<! PUIS SUPPORTED bit                        bit 17       	
		bool        APMSupporteed;					                                //<! APM SUPPORTED bit                         bit 16       	
		bool        CFASupported;					                                //<! Reserved for CFA                          bit 15       	
		bool        downloadMicroCodeSuppored;			                            //<! DOWNLOAD MICROCODE SUPPORTED bit          bit 14       	
		bool        unloadSupported;					                            //<! UNLOAD SUPPORTED bit                      bit 13       	
		bool        writeFUAExtSupported;				                            //<! WRITE FUA EXT SUPPORTED bit               bit 12       	
		bool        GPLSupported;					                                //<! GPL SUPPORTED bit                         bit 11       	
		bool        streamingSupported;				                                //<! STREAMING SUPPORTED bit                   bit 10       	
		bool        reserved;					                                    //<! Reserved                                  bit 9        	
		bool        smartSelfTestSupported; 				                        //<! SMART SELF-TEST SUPPORTED bit             bit 8        	
		bool        smartErrorLoggingSupported;			                            //<! SMART ERROR LOGGING SUPPORTED bit         bit 7        	
		bool        epcSupported;					                                //<! EPC SUPPORTED bit                         bit 6        	
		bool        senseDataSupported; 				                            //<! SENSE DATA SUPPORTED bit                  bit 5        	
		bool        freeFallSupported; 				                                //<! FREE-FALL SUPPORTED bit                   bit 4        	
		bool        DMMode3Supported; 				                                //<! DM MODE 3 SUPPORTED bit                   bit 3        	
		bool        GPLDMASupportd; 					                            //<! GPL DMA SUPPORTED bit                     bit 2        	
		bool        writeUnccorctableSupportd; 			                            //<! WRITE UNCORRECTABLE SUPPORTED bit         bit 1        	
		bool        WRVSupported; 					                                //<! WRV SUPPORTED bit                         bit 0 
		_sSupportedCapabilities() :senseDeviceFaultSupported(false), DSNSupported(false), lowPowerStandbySupported(false), setEPCPowerSourceSupported(false), \
			AMAXAddrSupported(false), reserved8(false), DRATSupported(false), LPSMisalignmentReportingSupported(false), reserved7(false), readBufferDMASupported(false), \
			writeBufferDMASupported(false), reserved6(false), downLoadMicroCodeDMASupported(false), bit28Supported(false), RZATSupported(false), reserved5(false), \
			NOPSupported(false), readBufferSupported(false), writeBufferSupported(false), reserved4(false), readLookAheadSupported(false), \
			volatileWriteCacheSupported(false), smartSupported(false), flushCacheExtSupported(false), reserved3(false), bit48Supported(false), reserved2(false), \
			spinupSupported(false), PUISSupported(false), APMSupporteed(false), CFASupported(false), downloadMicroCodeSuppored(false), unloadSupported(false), \
			writeFUAExtSupported(false), GPLSupported(false), streamingSupported(false), reserved(false), smartSelfTestSupported(false), smartErrorLoggingSupported(false), \
			epcSupported(false), senseDataSupported(false), freeFallSupported(false), DMMode3Supported(false), GPLDMASupportd(false), writeUnccorctableSupportd(false), WRVSupported(false) {};
	}sSupportedCapabilities;

	typedef struct _sDownloadMicroCode
	{
		bool        dmDefferedSupported;                                            //<! DM offset deffered Supported              bit 34
		bool        dmImmedaiateSupported;                                          //<! DM Immediate Supported                    bit 33
		bool        dmOffsetsImmediateSupported;                                    //<! DM OFFSETS IMMEDIATE SUPPORTED            bit 32
		uint16_t    dmMaxTransferSize;                                              //<! DM MAXIMUM TRANSFER SIZE field            16 - 31
		uint16_t    dmMinTranserSize;                                               //<! DM MINIMUM TRANSFER SIZE field            0  - 15
		_sDownloadMicroCode() : dmDefferedSupported(false), dmImmedaiateSupported(false), dmOffsetsImmediateSupported(false), dmMaxTransferSize(0), dmMinTranserSize(0) {};
	}sDownloadMicroCode;

	typedef struct _sSCT_Capabilities
	{
		bool        sct_Write_Same_Function_103_Supported;                          //<!   Sct Write Same Function 103 Supported Bit  
		bool        sct_Write_Same_Function_102_Supported;                          //<!   Sct Write Same Function 102 Supported Bit  
		bool        sct_Write_Same_Function_101_Supported;                          //<!   Sct Write Same Function 101 Supported Bit  
		bool        sct_Write_Same_Function_3_Supported;                            //<!   Sct Write Same Function 3 Supported Bit    
		bool        sct_Write_Same_Function_2_Supported;                            //<!   Sct Write Same Function 2 Supported Bit    
		bool        sct_Write_Same_Function_1_Supported;                            //<!   Sct Write Same Function 1 Supported Bit    
		bool        sct_Data_Tables_Supported;     	                                //<!   Sct Data Tables Supported Bit              
		bool        sct_Feature_Control_Supported; 	                                //<!   Sct Feature Control Supported Bit          
		bool        sct_Error_Recovery_Control_Supported;                           //<!   Sct Error Recovery Control Supported Bit   
		bool        sct_Write_Same_Supported;      	                                //<!   Sct Write Same Supported Bit               
		bool        sct_Supported; 			                                        //<!   Sct Supported Bit   
		_sSCT_Capabilities() : sct_Write_Same_Function_103_Supported(false), sct_Write_Same_Function_102_Supported(false), sct_Write_Same_Function_101_Supported(false), \
			sct_Write_Same_Function_3_Supported(false), sct_Write_Same_Function_2_Supported(false), sct_Write_Same_Function_1_Supported(false), sct_Data_Tables_Supported(false), \
			sct_Feature_Control_Supported(false), sct_Error_Recovery_Control_Supported(false), sct_Write_Same_Supported(false), sct_Supported(false) {};
	}sSCT_Capabilities;

	typedef struct _sLogPage04
	{
		uint64_t        header;                                             //<! data log information header                                            0 - 7
		uint64_t        currentSettings;                                    //<! current settings                                                       8 - 15
		uint64_t        featureSettings;                                    //<! Feature settings                                                       16 - 23
		uint64_t        DMAHostInterfaceSectorTimes;                        //<! DMA Host Interface Sector Times                                        24 - 31
		uint64_t        PioHostInterfaceSectorTImes;                        //<! Pio Host Interface Sector Times                                        32 - 39
		uint64_t        streamingMinRequestSize;                            //<! Streaming Min Request Size                                             40 - 47
		uint64_t        streamingAccessLatency;                             //<! streaming Access Latency                                               48 - 55
		uint64_t        streamingPerformanceGranularity;                    //<! streaming Performance Granularity                                      56 - 63
		uint64_t        freeFallControlSensitivity;                         //<! free Fall Control Sensitivity                                          64 - 71
		uint64_t        deviceMaintenaceSchedule;                           //<! device Maintenace Schedule                                             72 - 79
	}sLogPage04;
	typedef struct _sCurrentSettingBools
	{
		bool            dsnEnabled;                                         //<! DSN ENABLED bit                                    16
		bool            epcEnabled;                                         //<! EPC ENABLED bit                                    15
		bool            reserved5;                                          //<! Reserved                                           14
		bool            volatileWriteCacheEnabled;                          //<! VOLATILE WRITE CACHE ENABLED bit                   13
		bool            reserved4;                                          //<! Reserved                                           12
		bool            revertingToDefaultsEnabled;                         //<! REVERTING TO DEFAULTS ENABLED bit                  11
		bool            senseDataEnabled;                                   //<! SENSE DATA ENABLED bit                             10
		bool            reserved3;                                          //<! Reserved                                           9
		bool            nonVolatilWriteCache;                               //<! NON - VOLATILE WRITE CACHE bit                     8
		bool            readLookAheadEnabled;                               //<! READ LOOK - AHEAD ENABLED bit                      7
		bool            smartEnabled;                                       //<! SMART ENABLED bit                                  6
		bool            reserved2;                                          //<!  Reserved                                          5
		bool            reserved1;                                          //<!  Reserved                                          4
		bool            puisEnabled;                                        //<! PUIS ENABLED bit                                   3
		bool            apmEnabled;                                         //<! APM ENABLED bit                                    2
		bool            freeFallEnabled;                                    //<! FREE - FALL ENABLED                            bit 1
		bool            wrvEnabled;                                         //<! WRV enabled bit                                bit 0
	}sCurrentSettingBools;
	typedef struct _sFeatureSettings
	{
		uint8_t wrvMode;
		uint8_t apmLevel;
		uint8_t powerScource;
		_sFeatureSettings() : wrvMode(0), apmLevel(0), powerScource(0) {};
	}sFeatrueSettings;

	typedef struct _sLogPage06
	{
		uint64_t            header;                                         //<! data log information header                                            0 - 7
		uint64_t            masterPassword;                                 //<! Master Password Identifier                                             8 - 15
		uint64_t            securitySetting;                                //<! Security settings                                                      16 - 23
		uint64_t            timeEnhancedErase;                              //<! Time required for an Enhanced Erase mode SECURITY ERASE UNIT command   24 - 31
		uint64_t            timeNormalErase;                                //<! Time required for a Normal Erase mode SECURITY ERASE UNIT command      32 - 39
		uint64_t            tcfeatures;                                     //<! Trusted Computing feature set                                          40 - 47
		uint64_t            securityCapabilities;                           //<! Security Capabilities                                                  48 - 55
	}sLogPage06;

	typedef struct _sSecurityCapabilities
	{
		bool                sanitize;                                       //<! ACS - 3 COMMANDS ALLOWED BY SANITIZE                           bit 6
		bool                antifreeze;                                     //<! SANITIZE ANTIFREEZE LOCK SUPPORTED                             bit 5
		bool                eraseSupported;                                 //<! BLOCK ERASE SUPPORTED                                          bit 4
		bool                overwriteSupported;                             //<! OVERWRITE SUPPORTED                                            bit 3
		bool                cryptoScrambleSupported;                        //<! CRYPTO SCRAMBLE SUPPORTED                                      bit 2
		bool                sanitizeSupported;                              //<! SANITIZE SUPPORTED                                             bit 1
		bool                encryptSupported;                               //<! ENCRYPT ALL SUPPORTED                                          bit 0
	}sSecurityCapabilities;

	typedef struct _sLogPage07
	{
		uint64_t            header;                                         //<! data log information header                                            0 - 7
		uint64_t            parallelCapabilities;                           //<! parallel Capabilities                                                  8 - 15
		uint64_t            pioSupported;                                   //<! PIO Supported                                                          16 - 23
		uint64_t            multiwordDMA;                                   //<! Multiword DMA transfer cycle time                                      24 - 31
		uint64_t            minPIOTransfer;                                 //<! Minimum PIO Transfer cycle times                                       32 - 39
		uint64_t            setTransferMode;                                //<! Set Transfer Mode                                                      40 - 47
		uint64_t            parallelResetResult;                            //<! Parallel ATA Hardware Reset Result                                     48 - 55
	}sLogPage07;

	typedef struct _sATACapabilities
	{
		bool 		iordySupported;                     	                //<!  IORDY SUPPORTED bit                      bit 8
		bool 		iordyDisableSupported;             	                    //<!  IORDY DISABLE SUPPORTED bit              bit 7
		bool 		dmaSupported;                       	                //<!  DMA SUPPORTED bit                        bit 6
		bool 		multiwordDmaMode2Enabled;        	                    //<!  MULTIWORD DMA MODE 2 ENABLED bit         bit 5
		bool 		multiwordDmaMode1Enabled;        	                    //<!  MULTIWORD DMA MODE 1 ENABLED bit         bit 4
		bool 		multiwordDmaMode0Enabled;       	                    //<!  MULTIWORD DMA MODE 0 ENABLED bit         bit 3
		bool 		multiwordDmaMode2Supported;      	                    //<!  MULTIWORD DMA MODE 2 SUPPORTED bit       bit 2
		bool 		multiwordDmaMode1Supported;      	                    //<!  MULTIWORD DMA MODE 1 SUPPORTED bit       bit 1
		bool 		multiwordDmaMode0Supported;      	                    //<!  MULTIWORD DMA MODE 0 SUPPORTED bit       bit 0
		bool 		udmaMode6Enabled;                 	                    //<!  UDMA MODE 6 ENABLED bit                  bit 9
		bool 		udmaMode5Enabled;                 	                    //<!  UDMA MODE 5 ENABLED bit                  bit 8
		bool 		udmaMode4Enabled;                 	                    //<!  UDMA MODE 4 ENABLED bit                  bit 7
		bool 		udmaMode3Enabled;                 	                    //<!  UDMA MODE 3 ENABLED bit                  bit 6
		bool 		udmaMode2Enabled;                 	                    //<!  UDMA MODE 2 ENABLED bit                  bit 5
		bool 		udmaMode1Enabled;                 	                    //<!  UDMA MODE 1 ENABLED bit                  bit 4
		bool 		udmaMode0Enabled;                 	                    //<!  UDMA MODE 0 ENABLED bit                  bit 3
		bool 		udmaMode6Supported;               	                    //<!  UDMA MODE 6 SUPPORTED bit                bit 2
		bool 		udmaMode5Supported;               	                    //<!  UDMA MODE 5 SUPPORTED bit                bit 1
		bool 		udmaMode4Supported;               	                    //<!  UDMA MODE 4 SUPPORTED bit                bit 0
		bool 		udmaMode3Supported;               	                    //<!  UDMA MODE 3 SUPPORTED bit                bit 9
		bool 		udmaMode2Supported;               	                    //<!  UDMA MODE 2 SUPPORTED bit                bit 8
		bool 		udmaMode1Supported;               	                    //<!  UDMA MODE 1 SUPPORTED bit                bit 7
		bool 		udmaMode0Supported;               	                    //<!  UDMA MODE 0 SUPPORTED bit                bit 6
	}sATACapabilites;

	typedef struct _sATAHardwareResetResult
	{
		bool		CBLID;                              	                //<!  CBLID bit                                bit 55 
		bool		D1_PDIAG;                           	                //<!  D1 PDIAG bit                             bit 15 
		uint8_t		D1_DeviceNumberDetectField;          	                //<!  D1 DEVICE NUMBER DETECT field            Field 9:8
		bool		D0_PDIAG;                           	                //<!  D0 PDIAG bit                             bit 7  
		bool		D0_DASP;                            	                //<!  D0 DASP bit                              bit 6  
		bool		selection;                    	 	                    //<!  D0/D1 SELECTION bit                      bit 5  
		bool		D0_Diagnostics;                     	                //<!  D0 DIAGNOSTICS bit                       bit 4  
		uint8_t		D0_DeviceNumberDetect;          	 	                //<!  D0 DEVICE NUMBER DETECT field            Field 1:0
	}ATAHardwareResetResult;

	typedef struct _sLogPage08
	{
		uint64_t            header;                                         //!< data log information header                                            0 - 7
		uint64_t            sataCapabilities;                               //!< sata Capabilities                                                      8 - 15
		uint64_t            currentSata;                                    //!< Current sata settings                                                  16 - 23
		uint64_t            reserved;                                       //!< reserved                                                               24 - 31
		uint64_t            reserved1;                                      //!< reserved                                                               32 - 39
		uint16_t            currentHardware;                                //!< currente hardware feature control identifier                           40 - 41
		uint16_t            supportedHardware;                              //!< Supported Hardware feature control identifier                          42 - 43
		uint32_t            reserved2;                                      //!< reserved for serial ATA                                                44 - 47
		uint64_t            deviceSleepTiming;                              //!< Device sleep Timing Variables                                         48 - 55
	}sLogPage08;

	typedef struct _sSataCapabilities
	{
		bool        powerDisableFeatureAlwaysEnabled;                       //!<    POWER DISABLE FEATURE ALWAYS ENABLED bit                        bit 31
		bool        powerDisableFeatureSupported;                           //!<    POWER DISABLE FEATURE SUPPORTED bit                             bit 30
		bool        rebuildAssistSupported;                                 //!<    REBUILD ASSIST SUPPORTED bit                                    bit 29
		bool        supportedDIPM_SSP;                                      //!<    DIPM SSP PRESERVATION SUPPORTED bit                             bit 28
		bool        hybridInformationSupported;                             //!<    HYBRID INFORMATION SUPPORTED bit                                bit 27
		bool        devSleepToReducedWrstateCapablitySupported;             //!<    DEVSLEEP TO REDUCEDPWRSTATE CAPABILITY SUPPORTED bit            bit 26
		bool        deviceSleepSupported;                                   //!<    DEVICE SLEEP SUPPORTED bit                                      bit 25
		bool 		ncqAutosenseSupported;                                  //!<    NCQ AUTOSENSE SUPPORTED					                        bit 24
		bool 		softwareSettingsPreservationSupported;                  //!<    SOFTWARE SETTINGS PRESERVATION SUPPORTED       		            bit 23
		bool 		hardwareFeatureControlSupported;                        //!<    HARDWARE FEATURE CONTROL SUPPORTED     			                bit 22
		bool 		inOrderDataDeliverySupported;                           //!<    IN-ORDER DATA DELIVERY SUPPORTED       			                bit 21
		bool 		deviceInitiatedPowerManagementSupported;                //!<    DEVICE INITIATED POWER MANAGEMENT SUPPORTED    		            bit 20
		bool 		dmaSetupAutoActivationSupported;                        //!<    DMA SETUP AUTO-ACTIVATION SUPPORTED    			                bit 19
		bool 		nonZeroBufferOffsetsSupported;                          //!<    NON-ZERO BUFFER OFFSETS SUPPORTED      			                bit 18
		bool 		sendAndReceiveQueuedCommandsSupported;                  //!<    SEND AND RECEIVE QUEUED COMMANDS SUPPORTED     		            bit 17
		bool 		ncqQueueManagementCommandSupported;                     //!<    NCQ QUEUE MANAGEMENT COMMAND SUPPORTED 			                bit 16
		bool 		ncqStreamingSupported;                                  //!<    NCQ STREAMING SUPPORTED        				                    bit 15
		bool 		readLogDmaExtAsEquivalentToReadLogExtSupported;         //!<    READ LOG DMA EXT AS EQUIVALENT TO READ LOG EXT SUPPORTED        bit 14
		bool 		deviceAutomaticPartialToSlumberTransitionsSupported;    //!<    DEVICE AUTOMATIC PARTIAL TO SLUMBER TRANSITIONS SUPPORTED       bit 13
		bool 		hostAutomaticPartialToSlumberTransitionsSupported;      //!<    HOST AUTOMATIC PARTIAL TO SLUMBER TRANSITIONS SUPPORTED         bit 12
		bool 		ncqPriorityInformationSupported;                        //!<    NCQ PRIORITY INFORMATION SUPPORTED     			                bit 11
		bool 		unloadWhileNcqCommandsAreOutstandingSupported;          //!<    UNLOAD WHILE NCQ COMMANDS ARE OUTSTANDING SUPPORTED    	        bit 10
		bool 		sataPhyEventCountersLogSupported;                       //!<    SATA PHY EVENT COUNTERS LOG SUPPORTED  			                bit 9 
		bool 		receiptOfHostInitiatedPowerManagementRequestsSupported; //!<    RECEIPT OF HOST INITIATED POWER MANAGEMENT REQUESTS SUPPORTED   bit 8 
		bool 		ncqFeatureSetSupported;                                 //!<    NCQ FEATURE SET SUPPORTED      				                    bit 7 
		bool 		sataGen3SignalingSpeedSupported;                        //!<    SATA GEN3 SIGNALING SPEED SUPPORTED    			                bit 2 
		bool 		sataGen2SignalingSpeedSupported;                        //!<    SATA GEN2 SIGNALING SPEED SUPPORTED    			                bit 1 
		bool 		sataGen1SignalingSpeedSupported;                        //!<    SATA GEN1 SIGNALING SPEED SUPPORTED    			                bit 0 
	}sSATACapabilities;

	typedef struct _sCurrentSATASettings
	{
		bool        hybridEnabled;                                          //!< HYBRID ENABLED bit                                                 bit 13
		bool        rebuidAssistEnabled;                                    //!< REBUILD ASSIST ENABLED bit                                         bit 12
		bool        powerDisabledFeatrueEnabled;                            //!< POWER DISABLE FEATURE ENABLED bit                                  bit 11
		bool        deviceSleepEnalbed;                                     //!< DEVICE SLEEP ENABLED bit                                           bit 10
		bool 		slumberEnabled;              				            //!< AUTOMATIC PARTIAL TO SLUMBER TRANSITIONS ENABLED 	                bit  9  
		bool 		softwareSettingsPreservationEnabled;                    //!< SOFTWARE SETTINGS PRESERVATION ENABLED   		                    bit  8  
		bool 		hardwareFeatureControlEnabled;                          //!< HARDWARE FEATURE CONTROL IS ENABLED      		                    bit  7  
		bool 		inOrderDataDeliveryEnabled;                             //!< IN-ORDER DATA DELIVERY ENABLED   			                        bit  6  
		bool 		deviceInitiatedPowerManagementEnabled;                  //!< DEVICE INITIATED POWER MANAGEMENT ENABLED        	                bit  5  
		bool 		dmaSetupEnabled;                             		    //!< DMA SETUP AUTO-ACTIVATION ENABLED        		                    bit  4  
		bool 		nonZeroBufferEnabled;                               	//!< NON-ZERO BUFFER OFFSETS ENABLED  			                        bit  3  
		bool 		currentGen3SignalingSpeed;                              //!<    Current signalling speed is Gen2    			                bit  2 
		bool 		currentGen2SignalingSpeed;                              //!<    Current signalling speed is Gen2   			                    bit  1 
		bool 		currentGen1SignalingSpeed;                              //!<    Current signalling speed is Gen1   			                    bit 0
		bool        currentSignallingNotSupported;
	}sCurrentSettings;
#pragma pack(pop)
#endif 
}