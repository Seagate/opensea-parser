//
// CAta_Identify_log.h
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CAta_Identify_log.h
// \brief Defines the function calls and structures for pulling Seagate logs

#include "common.h"
#include "CLog.h"
#include "libjson.h"
#include <string>
#include "Identify_Log_Types.h"
#include "Opensea_Parser_Helper.h"


namespace opensea_parser {
#ifndef ATAIDENTIFY
#define ATAIDENTIFY

    class CAta_Identify_log
    {
    protected:
        std::string                         m_name;                                                     //!< name of the class
        uint8_t                             *pData;                                                     //!< pointer to the data
        eReturnValues                       m_status;                                                   //!< the status of the class
        sDriveInformation                   m_sDriveInfo;                                               //!< drive information structure
        void create_Serial_Number(uint16_t offset);
        void create_Firmware_String(uint16_t offset);
        void create_Model_Number( uint16_t offset);
        void create_WWN_Info();
        eReturnValues parse_Device_Info();
    public:
        CAta_Identify_log();
        CAta_Identify_log(uint8_t *Buffer);
        CAta_Identify_log(const std::string & fileName);
        ~CAta_Identify_log();
		eReturnValues get_Identify_Information_Status() { return m_status; };
        eReturnValues print_Identify_Information(JSONNODE *masterData);
    };

    class CAta_Identify_Log_00 
    {

    protected:
        std::string                         m_name;                                                     //!< name of the class
        uint8_t                             *pData;                                                     //!< pointer to the data
        eReturnValues                       m_status;                                                   //!< the status of the class  
        sLogPage00                          *m_pLog0;  
    public:
        CAta_Identify_Log_00(uint8_t *Buffer);
        ~CAta_Identify_Log_00();
        bool is_Page_Supported(uint8_t pageNumber);
        eReturnValues get_Log_Page00(uint8_t *pData, JSONNODE *masterData);
        
    };

    class CAta_Identify_Log_02 
    {

    protected:
        std::string                         m_name;                                                     //!< name of the class
        uint8_t                             *pData;                                                     //!< pointer to the data
        eReturnValues                       m_status;                                                   //!< the status of the class    
        sLogPage02                          *pCapacity;                                                 //!< pointer to the structure

        bool get_Device_Capacity(JSONNODE *capData);
        bool get_Sector_Size(JSONNODE *sectorData);
        //bool get_Logical_Sector_Size(JSONNODE *logicalData);
        //bool get_Normal_Buffer_size(JSONNODE *bufferData);

    public:
        CAta_Identify_Log_02(uint8_t *Buffer);
        ~CAta_Identify_Log_02();
        eReturnValues get_Log_Page02(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_03
    {
    protected:
        std::string                         m_name;                                             //!< name of the class
        uint8_t                             *pData;                                             //!< pointer to the data
        eReturnValues                       m_status;                                           //!< the status of the class    
        sLogPage03                          *m_pCap;                                            //!< structure Capabilites
        sSupportedCapabilities              m_sSupported;                                       //!< structure of supported Capabililities
        sDownloadMicroCode                  m_sDownloadMicrocode;                               //!< structure for holding the data
        sSCT_Capabilities                   m_sSCTCap;                                          //!< sturcture for holding the sct data

        bool set_Supported_Capabilities(uint64_t *value);
        bool get_Supported_Capabilities(JSONNODE *Capablities);
        bool set_DownLoad_MicroCode(uint64_t *value);
        bool get_DownLoad_MicroCode(JSONNODE *DM);
        bool get_Media_Rotation_Rate(JSONNODE *MRR);
        bool get_Form_Factor(JSONNODE *ff);
        bool get_Write_Read_Verify_Mode3(JSONNODE *mode3);
        bool get_Write_Read_Verify_Mode2(JSONNODE *mode2);
        bool world_Wide_Name(JSONNODE *WWN);
        bool get_Data_Set_Managment(JSONNODE *setManagement);
        bool get_Utilization_Unit_Time(JSONNODE *unitTime);
        bool get_Utilization_Usage_Rate_Support(JSONNODE *rate);
        bool get_Zoned_Capabilities(JSONNODE *zoned);
        bool get_Supported_ZAC_Capabilities(JSONNODE *zac);
        bool get_Background_Operations_Capabilities(JSONNODE *bgOperation);
        bool get_Background_Operations_Recommendations(JSONNODE *recommendations);
        bool get_Queue_Depth(JSONNODE *qd);
        bool set_SCT_Capabilities(uint64_t *value);
        bool get_SCT_Capabilities(JSONNODE *sct);
        bool get_Depop_Capabilities(JSONNODE *depop);
        bool get_Depopulation_Execution_Time(JSONNODE *depop);

    public:
        CAta_Identify_Log_03(uint8_t *Buffer);
        ~CAta_Identify_Log_03();
        eReturnValues get_Log_Page03(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_04 
    {
    protected:
        std::string                         m_name;                             //<! name of the class
        uint8_t                             *pData;                             //<! pointer to the data
        eReturnValues                       m_status;                           //<! the status of the class    
        sLogPage04                          *pLog;                              //<! pointer to the log page 04
        sCurrentSettingBools                m_CS;                               //<! struct to the current Settings in bool settings
        sFeatrueSettings                    m_FS;                               //<! struct to the Feature Settings

        bool set_Current_Settings(uint64_t *value);
        bool get_Current_Settings(JSONNODE *currentData);
        bool set_Feature_Settings(uint64_t *value);
        bool get_Feature_Settings(JSONNODE *featureData);
        bool get_DMS_Times(JSONNODE *dmaData);
        bool get_PIO_Times(JSONNODE *pioData);
        bool get_Streaming_Min_Request_Times(JSONNODE *streamMinData);
        bool get_Streaming_Access_Latency(JSONNODE *accessData);
        bool get_Streaming_Performance_Granularity(JSONNODE *performanceData);
        bool get_Free_Fall_Control(JSONNODE *freeFallData);
        bool get_Device_Maintenance_Schedule(JSONNODE *freeFallData);
    public:
        CAta_Identify_Log_04(uint8_t *Buffer);
        ~CAta_Identify_Log_04();
        eReturnValues get_Log_Page04(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_05
    {
#define LOG5_SERIAL_NUMBER 20
#define LOG5_FIRMWARE_REV  8
#define LOG5_MODEL_NUMBER  40
#define LOG5_PRODUCT_INFO  8
#pragma pack(push, 1)
        typedef struct _sLogPage05
        {
            uint64_t            header;                                             //<! data log information header                                            0 - 7
            uint8_t             serialNumber[LOG5_SERIAL_NUMBER];                   //<! Serial Number                                                          8 - 27
            uint32_t            reserved;                                           //<! Reserved                                                               28 - 31
            uint8_t             firmwareRev[LOG5_FIRMWARE_REV];                     //<! firmware Revision                                                      32 - 39
            uint8_t             reserved1[LOG5_FIRMWARE_REV];                       //<! reserved                                                               40 - 47
            uint8_t             modelNumber[LOG5_MODEL_NUMBER];                     //<! Model Number                                                           48 - 87
            uint8_t             reserved2[LOG5_PRODUCT_INFO];                       //<! reserved                                                               88 - 95
            uint8_t             productInformation[LOG5_PRODUCT_INFO];              //<! streaming Performance Granularity                                      95 - 103
                
        }sLogPage05;
        typedef struct _sPrintablePage05
        {
            std::string         serialStr;
            std::string         firmwareStr;
            std::string         modelNumberStr;
            std::string         productStr;
        }sPrintablePage05;
#pragma pack(pop)
    protected:
        std::string                         m_name;                                                     //!< name of the class
        uint8_t                             *pData;                                                     //!< pointer to the data
        eReturnValues                       m_status;                                                   //!< the status of the class    
        sLogPage05                          *m_pLog;
        sPrintablePage05                    *m_pPrintable;
        bool create_Serial_Number();
        bool create_Firmware_Rev();
        bool create_Model_Number();
        bool create_Product_string();
        bool get_printables(JSONNODE *masterData);
    public:
        CAta_Identify_Log_05(uint8_t *Buffer);
        ~CAta_Identify_Log_05();
        eReturnValues get_Log_Page05(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_06 
    {
    protected:
        std::string                         m_name;                             //!< name of the class
        uint8_t                             *pData;                             //!< pointer to the data
        eReturnValues                       m_status;                           //!< the status of the class    
        sLogPage06                          *m_pLog;
        sSecurityCapabilities               m_sSCapabilities;                   //!< structure for the security Capabilities
		sSecurityinformation                m_sSInformation;                    //!< structure for the security Information

        bool set_Security_Settings(uint64_t *value);
        bool get_Security_Settings(JSONNODE *si);
        bool get_Master_Password_Identifier( JSONNODE *mpIdent);
        bool get_Time_for_Enhanced_Erase(JSONNODE *enhanced);
        bool get_Time_for_Normal_Erase(JSONNODE *normal);
        bool get_Trusted_Computing_Feature_Set(JSONNODE *tdc);
        bool set_Security_Capabilities();
        bool get_Security_Capabilities(JSONNODE *sCap);
    public:
        CAta_Identify_Log_06(uint8_t *Buffer);
        ~CAta_Identify_Log_06();
        eReturnValues get_Log_Page06(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_07 
    {
    protected:
        std::string                         m_name;                             //<! name of the class
        uint8_t                             *pData;                             //<! pointer to the data
        eReturnValues                       m_status;                           //<! the status of the class    
        sLogPage07                          *m_pLog;
        sATACapabilites                     m_ATACap;                           //<! structure to the ATA Capabliites
        ATAHardwareResetResult              m_hardwareRR;                       //<! structur to the ATA Hardware Reset Results

    public:
        CAta_Identify_Log_07(uint8_t *Buffer);
        ~CAta_Identify_Log_07();
        eReturnValues get_Log_Page07(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_08 
    {
    protected:
        std::string                         m_name;                             //!< name of the class
        uint8_t                             *pData;                             //!< pointer to the data
        eReturnValues                       m_status;                           //!< the status of the class    
        sLogPage08                          *m_pLog;
        sSATACapabilities                   m_SATACap;                          //!< structure for the sata capablities
        sCurrentSettings                    m_CurrentSet;                       //!< structure for the current sata settings

        bool set_Sata_Capabilities();
        bool get_Sata_Capabilities(JSONNODE *cap);
        bool set_Current_Sata();
        bool get_Current_Sata(JSONNODE *current);
        void get_Current_Hardware(JSONNODE *hardware);
        void get_Supported_Hardware(JSONNODE *supported);
        void get_Device_Sleep_Timing_Variables(JSONNODE *sleep);
    public:
        CAta_Identify_Log_08(uint8_t *Buffer);
        ~CAta_Identify_Log_08();
        eReturnValues get_Log_Page08(uint8_t *pData, JSONNODE *masterData);
    };

    class CAta_Identify_Log_30 
    {

    protected:
		uint8_t								* pData;							//<! pointer to the buffer data
        std::string                         m_name;                             //<! name of the class
        eReturnValues                       m_status;                           //<! the status of the class   

        eReturnValues get_Interface_Type();
        
    public:
        CAta_Identify_Log_30( const std::string & fileName);
		CAta_Identify_Log_30(uint8_t *pBufferData);
        virtual ~CAta_Identify_Log_30();
        eReturnValues get_identify_Status(){ return m_status; };
        eReturnValues parse_Identify_Log_30(JSONNODE *masterData);

    };
#endif // ATAIDENTIFY
}
