// Do NOT modify or remove this copyright and license
//
//Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

#include "CAta_Identify_Log.h"
#include <sstream>
#include <iomanip>

#define ATA_SERIAL_NUMBER_LEN       20
#define ATA_MODEL_NUMBER_LEN        40
#define ATA_FIRMWARE_REV_LEN        8


using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn ltrim()
//
//! \brief
//!   Description:  trim from beginning of string (left)
//
//  Entry:
//! \param string
//
//
//---------------------------------------------------------------------------
inline std::string ltrim(std::string& s, const char* t = " \t\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}
//-----------------------------------------------------------------------------
//
//! \fn rtrim()
//
//! \brief
//!   Description:  trim from beginning of string (right)
//
//  Entry:
//! \param string
//
//
//---------------------------------------------------------------------------
inline std::string rtrim(std::string& s, const char* t = " \t\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}
//-----------------------------------------------------------------------------
//
//! \fn check_For_Active_Status()
//
//! \brief
//!   Description:  check for the active status bit in the 63 bit value
//
//  Entry:
//! \param value  =  64 bit value to check to see if the bit is set or not
//
//  Exit:
//!   \return bool - false or true
//
//---------------------------------------------------------------------------
inline bool check_For_Active_Status(const uint64_t *value)
{
    if (*value & BIT63)
    {
        return true;
    }
    return false;
}

// *****************************************************************************
CAta_Identify_log::CAta_Identify_log()
    : m_name("ATA Identify Log")
    , pData(M_NULLPTR)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_sDriveInfo()
{
    //m_sDriveInfo = {};
}

//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::CAta_Identify_log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//! \param pdata = pointer to the buffer data.
//! \param securityPrintLevel = the level at which to print
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_Identify_log::CAta_Identify_log(uint8_t *buffer)
    : m_name("ATA Identify Log")
    , pData(buffer)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_sDriveInfo()
{
    if (pData != M_NULLPTR)
    {
        parse_Device_Info();
        m_status = eReturnValues::IN_PROGRESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::CAta_Identify_log()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//! \param pdata = pointer to the buffer data.
//! \param securityPrintLevel = the level at which to print
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_Identify_log::CAta_Identify_log(const std::string & fileName)
    : m_name("ATA Identify Log")
    , pData(M_NULLPTR)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_sDriveInfo()
{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
    {
        if (cCLog->get_Buffer() != M_NULLPTR)
        {
            // create a buffer for the first part of the buffer to check to make sure it is not a sas log
            uint8_t* idCheckBuf = new uint8_t[sizeof(sLogPageStruct)];
#ifndef __STDC_SECURE_LIB__
            memcpy(idCheckBuf, cCLog->get_Buffer(), sizeof(sLogPageStruct));
#else
            memcpy_s(idCheckBuf, sizeof(sLogPageStruct), cCLog->get_Buffer(), sizeof(sLogPageStruct));// copy the buffer data to the class member pBuf
#endif
            sLogPageStruct* idCheck;
            idCheck = reinterpret_cast<sLogPageStruct*>(&idCheckBuf[0]);
            byte_Swap_16(&idCheck->pageLength);
            // verify that it is not a sas log
            if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
            {
                size_t  bufferSize = cCLog->get_Size();
                pData = new uint8_t[(bufferSize - 0x200)];								// new a buffer to the point	
                // First page of the identify is not used. we will strip it out and parse for the second Sector
#ifndef __STDC_SECURE_LIB__
                memcpy(pData, cCLog->get_Buffer_Offset(0x200), (bufferSize - 0x200));
#else
                memcpy_s(pData, (bufferSize - 0x200), cCLog->get_Buffer_Offset(0x200), (bufferSize - 0x200));// copy the buffer data to the class member pBuf
#endif
                parse_Device_Info();
                m_status = eReturnValues::IN_PROGRESS;
                delete[] pData;
            }
            else
            {
                m_status = eReturnValues::BAD_PARAMETER;
            }
            delete [] idCheckBuf;
        }
        else
        {
            m_status = eReturnValues::FAILURE;
        }
    }
    else
    {
        m_status = cCLog->get_Log_Status();
    }
    delete(cCLog);
}

//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::~CAta_Identify_log()
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CAta_Identify_log::~CAta_Identify_log()
{

}
//-----------------------------------------------------------------------------
//
//! \fn create_Serial_Number()
//
//! \brief
//!   Description:  takes the two uint64 bit seiral number values and create a string serial number
//
//  Entry:
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//
//---------------------------------------------------------------------------

void CAta_Identify_log::create_Serial_Number(uint16_t offset)
{

    m_sDriveInfo.serialNumber.assign(reinterpret_cast<const char*>(&pData[offset]), ATA_SERIAL_NUMBER_LEN);
    byte_swap_std_string(m_sDriveInfo.serialNumber);
    ltrim(m_sDriveInfo.serialNumber);
    m_sDriveInfo.serialNumber.resize(ATA_SERIAL_NUMBER_LEN);//don't know why this was here, but assuming there is a reason - TJE   
}

//-----------------------------------------------------------------------------
//
//! \fn create_Firmware_String()
//
//! \brief
//!   Description:  takes the two uint64 bit firmware Rev values and create a string firmware Rev 
//
//  Entry:
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//
//---------------------------------------------------------------------------
void CAta_Identify_log::create_Firmware_String(uint16_t offset)
{
    m_sDriveInfo.firmware.assign(reinterpret_cast<const char*>(&pData[offset]), ATA_FIRMWARE_REV_LEN);
    byte_swap_std_string(m_sDriveInfo.firmware);
    rtrim(m_sDriveInfo.firmware);
    m_sDriveInfo.firmware.resize(ATA_FIRMWARE_REV_LEN);
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::create_Model_Number()
//
//! \brief
//!   Description:  parse out the model number from the log
//
//  Entry:
//! \param offset  location of the data
//
//  Exit:
//
//---------------------------------------------------------------------------
void CAta_Identify_log::create_Model_Number(uint16_t offset)
{
    m_sDriveInfo.modelNumber.assign(reinterpret_cast<const char*>(&pData[offset]), ATA_MODEL_NUMBER_LEN);
    byte_swap_std_string(m_sDriveInfo.modelNumber);
    rtrim(m_sDriveInfo.modelNumber);
    m_sDriveInfo.modelNumber.resize(ATA_MODEL_NUMBER_LEN);
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::create_WWN_Info()
//
//! \brief
//!   Description:  parse out the wwn information from the log
//
//  Entry:
//! \param offset  location of the data
//
//  Exit:
//
//---------------------------------------------------------------------------
void CAta_Identify_log::create_WWN_Info()
{
    uint16_t *identWordPtr = (reinterpret_cast<uint16_t*>(&pData[0])); // move it to the second page / sector

    m_sDriveInfo.worldWideName.resize(WORLD_WIDE_NAME_LEN);
    m_sDriveInfo.ieeeOUI.resize(WORLD_WIDE_NAME_LEN);
    m_sDriveInfo.uniqueID.resize(WORLD_WIDE_NAME_LEN);
    uint64_t wwn = M_WordsTo8ByteValue(identWordPtr[108], identWordPtr[109], identWordPtr[110], identWordPtr[111]);
    std::ostringstream temp;
    temp << std::hex << std::uppercase << wwn;
    m_sDriveInfo.worldWideName.assign(temp.str());
    uint64_t ieeeOUI = static_cast<uint64_t>((wwn & UINT64_C(0x0FFFFFF000000000)) >> 36);
    uint64_t uniqueID = static_cast<uint64_t>(wwn & UINT64_C(0x0000000FFFFFFFFF));

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::uppercase << std::setfill('0') << std::setw(6) << ieeeOUI;
    m_sDriveInfo.ieeeOUI.assign(temp.str());

    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::uppercase << std::setfill('0') << std::setw(9) << uniqueID;
    m_sDriveInfo.uniqueID.assign(temp.str());
    m_sDriveInfo.worldWideName.resize(WORLD_WIDE_NAME_LEN);
    m_sDriveInfo.ieeeOUI.resize(WORLD_WIDE_NAME_LEN);
    m_sDriveInfo.uniqueID.resize(WORLD_WIDE_NAME_LEN);

}

//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::ParseDeviceInfo()
//
//! \brief
//!   Description:  parse out key items fro mthe identify information
//
//  Entry:
//! \param ptr  pointer to the buffer data
//! \param 
//
//  Exit:
//!   \return string of the firmware rev
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_log::parse_Device_Info()
{
    uint8_t *IDptr = pData;
    uint16_t *identWordPtr = (reinterpret_cast<uint16_t*>(&pData[0])); // move it to the second page / sector

    if ((identWordPtr[48] & BIT0) > 0)
    {
        m_sDriveInfo.tcgSupported = true;
    }
    if ((identWordPtr[49] & BIT8) > 0)
    {
        m_sDriveInfo.dmaSupported = true;
    }
    if ((identWordPtr[119] & BIT3) > 0 || (identWordPtr[120] & BIT3) > 0)
    {
        m_sDriveInfo.readWriteLogExtDMASupported = true;
    }
    if ((identWordPtr[83] & BIT10) > 0 || (identWordPtr[86] & BIT10) > 0)
    {
        m_sDriveInfo.fourtyEightBitSupported = true;
    }
    if ((identWordPtr[69] & BIT3) > 0 || m_sDriveInfo.fourtyEightBitSupported == true)
    {
        m_sDriveInfo.extLBA = true;
    }
    if ((identWordPtr[84] & BIT5) > 0 || (identWordPtr[87] & BIT5) > 0)
    {
        m_sDriveInfo.gplFeatureSupported = true;
    }
    if ((identWordPtr[85] & BIT0) > 0)
    {
        m_sDriveInfo.smartFeatureEnabled = true;
    }
    if ((static_cast<uint32_t>(identWordPtr[117] << 8) & identWordPtr[118]) > 0)
    {
        m_sDriveInfo.ataReportedLogicalSectorSize = static_cast<uint32_t>(identWordPtr[117] << 8) & identWordPtr[118];
    }
    if ((identWordPtr[206] & BIT0) > 0)
    {
        m_sDriveInfo.sctSupported = true;
        if ((identWordPtr[206] & BIT5) > 0)
        {
            m_sDriveInfo.sctDataTables = true;
        }
    }

    memcpy(&m_sDriveInfo.identRotationRate, &identWordPtr[217], 1);
    if (m_sDriveInfo.identRotationRate == 0x0001)
    {
        m_sDriveInfo.isAtaSSD = true;
    }

    if ((identWordPtr[59] & BIT8) > 0)
    {
        m_sDriveInfo.multipleValid = true;
        m_sDriveInfo.numDRQBlocks = static_cast<uint16_t>(identWordPtr[59] & UINT16_C(0x00FF));
    }

    //parse out some identifying information
    create_Model_Number(54);
    create_Serial_Number(20);
    create_Firmware_String(46);
    //world wide name (word 84bit 8 shows support)
    if (identWordPtr[84] & BIT8 || identWordPtr[87] & BIT8)
    {
        create_WWN_Info();
    }
    else
    {
        m_sDriveInfo.worldWideName.assign("Not Supported");
    }

    //maxLBA
    m_sDriveInfo.maxLBA28 = M_WordsTo4ByteValue(identWordPtr[60], identWordPtr[61]);
    m_sDriveInfo.maxLBA48 = M_WordsTo8ByteValue(identWordPtr[100], identWordPtr[101], identWordPtr[102], identWordPtr[103]);
    word_Swap_32(&m_sDriveInfo.maxLBA28);
    word_Swap_64(&m_sDriveInfo.maxLBA48);

    //sector sizes
    if (((identWordPtr[106] & BIT14) == BIT14) && ((identWordPtr[106] & BIT15) == 0))       //making sure this word has valid data
    {
        m_sDriveInfo.sSizes.sectorSizeReported = true;
        //word 117 is only valid when word 106 bit 12 is set
        if ((identWordPtr[106] & BIT12) == BIT12)
        {
            m_sDriveInfo.sSizes.logicalSectorSize =  M_WordsTo4ByteValue(identWordPtr[118], identWordPtr[117]);
            m_sDriveInfo.sSizes.logicalSectorSize *= 2;                                              //convert to words to bytes
        }
        else                                                                                //means that logical sector size is 512bytes
        {
            m_sDriveInfo.sSizes.logicalSectorSize = 512;
        }
        if ((identWordPtr[106] & BIT13) == 0)
        {
            m_sDriveInfo.sSizes.physicalSectorSize = m_sDriveInfo.sSizes.logicalSectorSize;
        }
        else                                                                                //multiple logical sectors per physical sector
        {
            m_sDriveInfo.sSizes.sectorSizeExponent = M_Byte0(identWordPtr[106] & UINT16_C(0x000F));                     //get the number of logical blocks per physical blocks
            if (m_sDriveInfo.sSizes.sectorSizeExponent != 0)
            {
                uint8_t logicalPerPhysical = 1;
                uint8_t shiftCounter = 0;
                while (shiftCounter < m_sDriveInfo.sSizes.sectorSizeExponent)
                {
                    logicalPerPhysical = static_cast<uint8_t>(logicalPerPhysical << 1);                           //multiply by 2
                    shiftCounter++;
                }
                m_sDriveInfo.sSizes.physicalSectorSize = m_sDriveInfo.sSizes.logicalSectorSize * logicalPerPhysical;
            }
        }
        //logical sector alignment
        m_sDriveInfo.logicalSectorAlignment = static_cast<uint32_t>(identWordPtr[209] & UINT16_C(0x3FFF));

    }

    m_sDriveInfo.sCapInfo.capUnit = static_cast<double>(m_sDriveInfo.maxLBA48 * m_sDriveInfo.sSizes.logicalSectorSize);
    m_sDriveInfo.sCapInfo.capacityUnit.resize(3);
    char capUnitBuf[5] = { 0 };
    char * capUnit = &capUnitBuf[0];
    capacity_Unit_Convert(&m_sDriveInfo.sCapInfo.capUnit, &capUnit);
    m_sDriveInfo.sCapInfo.capacityUnit.assign(capUnit);

    //sata speeds
    m_sDriveInfo.sataGenSupported = static_cast<uint8_t>((identWordPtr[76] & 0x000F) >> 1);                         //we only care about the lower bits right now
    m_sDriveInfo.sataNegotiated = static_cast<uint8_t>((identWordPtr[77] & 0x000F) >> 1);                           //coded value
    //ATA spec supported

    m_sDriveInfo.ataSpecBits = identWordPtr[80];
    for (m_sDriveInfo.ataSpecCounter = 0; m_sDriveInfo.ataSpecCounter < 15; m_sDriveInfo.ataSpecCounter++)
    {
        if (m_sDriveInfo.ataSpecBits == 0x0001 || m_sDriveInfo.ataSpecBits == 0x0000)
        {
            break;
        }
        m_sDriveInfo.ataSpecBits = static_cast<uint16_t>(m_sDriveInfo.ataSpecBits >> 1);
    }

    //minor spec version number
    m_sDriveInfo.minorSpecVersionNumber = identWordPtr[81];

    //Transport Supported
    if (identWordPtr[222] == 0xFFFF || identWordPtr[222] == 0x0000)
    {
        m_sDriveInfo.sTrans.transportReported = false;
    }
    else
    {
        m_sDriveInfo.sTrans.transportType = get_8bit_range_uint16(identWordPtr[222], 15, 12);
        m_sDriveInfo.sTrans.transportCounter = 0;
        m_sDriveInfo.sTrans.transportBits = static_cast<uint16_t>(identWordPtr[222] & UINT16_C(0x0FFF));
        for (m_sDriveInfo.sTrans.transportCounter = 0; m_sDriveInfo.sTrans.transportCounter < 11; m_sDriveInfo.sTrans.transportCounter++)
        {
            if (m_sDriveInfo.sTrans.transportBits == 0x0001 || m_sDriveInfo.sTrans.transportBits == 0x0000)
            {
                break;
            }
            m_sDriveInfo.sTrans.transportBits = static_cast<uint16_t>(m_sDriveInfo.sTrans.transportBits >> 1);
        }

        m_sDriveInfo.sTrans.transportReported = true;
    }
    m_sDriveInfo.sTrans.transportMinorVersion = identWordPtr[223];

    //form factor
    m_sDriveInfo.formFactor = (identWordPtr[168]);

    uint8_t page = 1;  

    //fill in data for page 1
    m_sDriveInfo.IDDevCap = M_BytesTo8ByteValue(0, IDptr[page * 512 + 14], IDptr[page * 512 + 13], IDptr[page * 512 + 12], IDptr[page * 512 + 11], IDptr[page * 512 + 10], IDptr[page * 512 + 9], IDptr[page * 512 + 8]);

    m_sDriveInfo.IDPhySecSize = M_BytesTo8ByteValue(IDptr[page * 512 + 23], IDptr[page * 512 + 22], IDptr[page * 512 + 21], IDptr[page * 512 + 20], IDptr[page * 512 + 19], IDptr[page * 512 + 18], IDptr[page * 512 + 17], IDptr[page * 512 + 16]);
    m_sDriveInfo.IDLogSecSize = M_BytesTo8ByteValue(IDptr[page * 512 + 31], IDptr[page * 512 + 30], IDptr[page * 512 + 29], IDptr[page * 512 + 28], IDptr[page * 512 + 27], IDptr[page * 512 + 26], IDptr[page * 512 + 25], IDptr[page * 512 + 24]);
    m_sDriveInfo.IDBufSize =    M_BytesTo8ByteValue(IDptr[page * 512 + 39], IDptr[page * 512 + 38], IDptr[page * 512 + 37], IDptr[page * 512 + 36], IDptr[page * 512 + 35], IDptr[page * 512 + 34], IDptr[page * 512 + 33], IDptr[page * 512 + 32]);

    //fill in data for page 2
    page = 2;
    m_sDriveInfo.IDCapabilities = M_BytesTo8ByteValue(IDptr[page * 512 + 15], IDptr[page * 512 + 14], IDptr[page * 512 + 13], IDptr[page * 512 + 12], IDptr[page * 512 + 11], IDptr[page * 512 + 10], IDptr[page * 512 + 9], IDptr[page * 512 + 8]);
    m_sDriveInfo.IDMicrocode =    M_BytesTo8ByteValue(IDptr[page * 512 + 23], IDptr[page * 512 + 22], IDptr[page * 512 + 21], IDptr[page * 512 + 20], IDptr[page * 512 + 19], IDptr[page * 512 + 18], IDptr[page * 512 + 17], IDptr[page * 512 + 16]);
    m_sDriveInfo.IDMediaRotRate = M_BytesTo8ByteValue(IDptr[page * 512 + 31], IDptr[page * 512 + 30], IDptr[page * 512 + 29], IDptr[page * 512 + 28], IDptr[page * 512 + 27], IDptr[page * 512 + 26], IDptr[page * 512 + 25], IDptr[page * 512 + 24]);
    m_sDriveInfo.IDFormFactor =   M_BytesTo8ByteValue(IDptr[page * 512 + 39], IDptr[page * 512 + 38], IDptr[page * 512 + 37], IDptr[page * 512 + 36], IDptr[page * 512 + 35], IDptr[page * 512 + 34], IDptr[page * 512 + 33], IDptr[page * 512 + 32]);
    m_sDriveInfo.IDWRVSecCount2 = M_BytesTo8ByteValue(IDptr[page * 512 + 47], IDptr[page * 512 + 46], IDptr[page * 512 + 45], IDptr[page * 512 + 44], IDptr[page * 512 + 43], IDptr[page * 512 + 42], IDptr[page * 512 + 41], IDptr[page * 512 + 40]);
    m_sDriveInfo.IDWRVSecCount3 = M_BytesTo8ByteValue(IDptr[page * 512 + 55], IDptr[page * 512 + 54], IDptr[page * 512 + 53], IDptr[page * 512 + 52], IDptr[page * 512 + 51], IDptr[page * 512 + 50], IDptr[page * 512 + 49], IDptr[page * 512 + 48]);
    m_sDriveInfo.IDWWN =          M_BytesTo8ByteValue(IDptr[page * 512 + 71], IDptr[page * 512 + 70], IDptr[page * 512 + 69], IDptr[page * 512 + 68], IDptr[page * 512 + 67], IDptr[page * 512 + 66], IDptr[page * 512 + 65], IDptr[page * 512 + 64]);
    
    //fill in data for page 5
    //data in page 5    
    page = 5;
    m_sDriveInfo.IDSecurityStatus = M_BytesTo8ByteValue(IDptr[page * 512 + 23], IDptr[page * 512 + 22], IDptr[page * 512 + 21], IDptr[page * 512 + 20], IDptr[page * 512 + 19], IDptr[page * 512 + 18], IDptr[page * 512 + 17], IDptr[page * 512 + 16]);

    // Device Capabilities:
    if (m_sDriveInfo.IDCapabilities & BIT45) //bit 45
    {
        m_sDriveInfo.sIdentCap.requestSenseDeviceFault = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT44) //bit 44
    {
        m_sDriveInfo.sIdentCap.DSN = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT43) //bit 43
    {
        m_sDriveInfo.sIdentCap.lowPowerStandby = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT42) //bit 42
    {
        m_sDriveInfo.sIdentCap.setEPCPowerSource = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT41) //bit 41
    {
        m_sDriveInfo.sIdentCap.maxAddr = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT39) //bit 39
    {
        m_sDriveInfo.sIdentCap.DRAT = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT38) //bit 38
    {
        m_sDriveInfo.sIdentCap.lpsMisalignmentReporting = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT36) //bit 36
    {
        m_sDriveInfo.sIdentCap.readBufferDMA = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT35) //bit 35
    {
        m_sDriveInfo.sIdentCap.writeBufferDMA = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT33) //bit 33
    {
        m_sDriveInfo.sIdentCap.downloadMicrocodeDMA = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT32) //bit 32
    {
        m_sDriveInfo.sIdentCap.bit28 = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT30) //bit 31
    {
        m_sDriveInfo.sIdentCap.rZAT = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT29) //bit 29
    {
        m_sDriveInfo.sIdentCap.nOP = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT28) //bit 28
    {
        m_sDriveInfo.sIdentCap.readBuffer = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT27) //bit 27
    {
        m_sDriveInfo.sIdentCap.WriteBuffer = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT25) //bit 25
    {
        m_sDriveInfo.sIdentCap.readLookAhead = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT24) //bit 24
    {
        m_sDriveInfo.sIdentCap.volatileWriteCache = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT23) //bit 23
    {
        m_sDriveInfo.sIdentCap.SMART = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT22) //bit 22
    {
        m_sDriveInfo.sIdentCap.flushCacheExt = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT20) //bit 20
    {
        m_sDriveInfo.sIdentCap.bit48 = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT18) //bit 18
    {
        m_sDriveInfo.sIdentCap.spinUp = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT17) //bit 17
    {
        m_sDriveInfo.sIdentCap.puis = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT16) //bit 16
    {
        m_sDriveInfo.sIdentCap.APM = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT15) //bit 15
    {
        m_sDriveInfo.sIdentCap.CFA = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT14) //bit 14
    {
        m_sDriveInfo.sIdentCap.downloadMicrocode = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT13) //bit 13
    {
        m_sDriveInfo.sIdentCap.unload = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT12) //bit 12
    {
        m_sDriveInfo.sIdentCap.writeFUAExt = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT11)//bit 11
    {
        m_sDriveInfo.sIdentCap.GPL = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT10) //bit 10
    {
        m_sDriveInfo.sIdentCap.Streaming = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT8) //bit 8
    {
        m_sDriveInfo.sIdentCap.smartSelfTest = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT7) //bit 7
    {
        m_sDriveInfo.sIdentCap.smartErrorLogging = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT6) //bit 6
    {
        m_sDriveInfo.sIdentCap.EPC = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT5) //bit 5
    {
        m_sDriveInfo.sIdentCap.senseData = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT4) //bit 4
    {
        m_sDriveInfo.sIdentCap.freeFall = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT3) //bit 3
    {
        m_sDriveInfo.sIdentCap.DMMode3 = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT2) //bit 2
    {
        m_sDriveInfo.sIdentCap.GPL_DM = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT1) //bit 1
    {
        m_sDriveInfo.sIdentCap.writeUncorrectable = true;
    }
    if (m_sDriveInfo.IDCapabilities & BIT0) //bit 0
    {
        m_sDriveInfo.sIdentCap.WRV = true;
    }

    // security
    m_sDriveInfo.SecurityID = identWordPtr[128];
    if (m_sDriveInfo.IDSecurityStatus != 0)
    {
        if (m_sDriveInfo.IDSecurityStatus  & BIT6) //bit 6
        {
            m_sDriveInfo.sSecurityInfo.supported = true;
        }
        m_sDriveInfo.sSecurityInfo.masterPasswordCapability = m_sDriveInfo.IDSecurityStatus  & BIT5; //bit 5
        if (m_sDriveInfo.IDSecurityStatus  & BIT4) //bit 4
        {
            m_sDriveInfo.sSecurityInfo.enhancedSecurityEraseSupported = true;
        }
        if (m_sDriveInfo.IDSecurityStatus  & BIT3) //bit 3
        {
            m_sDriveInfo.sSecurityInfo.securityCountExpired = true;
        }
        if (m_sDriveInfo.IDSecurityStatus  & BIT2) //bit 2
        {
            m_sDriveInfo.sSecurityInfo.frozen = true;
        }
        if (m_sDriveInfo.IDSecurityStatus  & BIT1) //bit 1
        {
            m_sDriveInfo.sSecurityInfo.locked = true;
        }
        if (m_sDriveInfo.IDSecurityStatus  & BIT0) //bit 0
        {
            m_sDriveInfo.sSecurityInfo.enabled = true;
        }
    }
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_log::print_Identify_Information()
//
//! \brief
//!   Description:  takes the parsed infromation in the struct and prints it out into a json object 
//
//  Entry:
//! \param masterData  a json node for holding all the data
//! \param 
//
//  Exit:
//!   \return eReturnValues::SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_log::print_Identify_Information(JSONNODE *masterData)
{
    JSONNODE *identifyInfo = json_new(JSON_NODE);
    json_set_name(identifyInfo, "Identify Device Information");

    //print the strings
    json_push_back(identifyInfo, json_new_a("Model Number", m_sDriveInfo.modelNumber.c_str()));

    json_push_back(identifyInfo, json_new_a("Serial Number", m_sDriveInfo.serialNumber.c_str()));

    json_push_back(identifyInfo, json_new_a("Firmware Revision", m_sDriveInfo.firmware.c_str()));
    // world wide name

    json_push_back(identifyInfo, json_new_a("World Wide Name", m_sDriveInfo.worldWideName.c_str()));
    JSONNODE *IEEEinfo = json_new(JSON_NODE);
    json_set_name(IEEEinfo, "IEEE Registered");
    json_push_back(IEEEinfo, json_new_a("IEEE Company ID", m_sDriveInfo.ieeeOUI.c_str()));
    json_push_back(IEEEinfo, json_new_a("Vendor Specific ID", m_sDriveInfo.uniqueID.c_str()));
    json_push_back(identifyInfo, IEEEinfo);

    //maxLBA
    std::ostringstream temp;
    temp << std::dec << m_sDriveInfo.maxLBA28;
    json_push_back(identifyInfo, json_new_a("MaxLBA (28bit)", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << std::dec << m_sDriveInfo.maxLBA48;
    json_push_back(identifyInfo, json_new_a("MaxLBA (48bit)", temp.str().c_str()));
    temp.str("");temp.clear();
    // sector sizes
    JSONNODE *sectorSize = json_new(JSON_NODE);
    json_set_name(sectorSize, "Sector Sizes");
    if (m_sDriveInfo.sSizes.sectorSizeReported)
    {
        temp.str("");temp.clear();
        temp << std::dec << m_sDriveInfo.sSizes.logicalSectorSize;
        json_push_back(sectorSize, json_new_a("Logical", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(m_sDriveInfo.sSizes.sectorSizeExponent);
        json_push_back(sectorSize, json_new_a("Sector Size Exponent", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << m_sDriveInfo.sSizes.physicalSectorSize;
        json_push_back(sectorSize, json_new_a("Physical", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << m_sDriveInfo.logicalSectorAlignment;
        json_push_back(sectorSize, json_new_a("Logical Sector alignment", temp.str().c_str()));
    }
    else
    {
        json_push_back(sectorSize, json_new_a("Sector Size", "512 byte assumed"));
    }
    json_push_back(identifyInfo, sectorSize);

    // capacity
    temp.str("");temp.clear();
    temp << std::fixed << std::setprecision(2) << std::setfill('0') << m_sDriveInfo.sCapInfo.capUnit << " " << m_sDriveInfo.sCapInfo.capacityUnit;
    json_push_back(identifyInfo, json_new_a("Identify Log Device Capacity", temp.str().c_str()));
    // Spec 
    temp.str("");temp.clear();
    switch (m_sDriveInfo.ataSpecCounter)
    {
    case 15:
        temp << "ACS-8";
        break;
    case 14:
        temp <<  "ACS-7";
        break;
    case 13:
        temp <<  "ACS-6";
        break;
    case 12:
        temp <<  "ACS-5";
        break;
    case 11:
        temp <<  "ACS-4";
        break;
    case 10:
        temp <<  "ACS-3";
        break;
    case 9:
        temp <<  "ACS-2";
        break;
    case 8:
        temp <<  "ATA8 - ACS";
        break;
    case 7:
        temp <<  "ATA/ATAPI-7";
        break;
    case 6:
        temp <<  "ATA/ATAPI-6";
        break;
    case 5:
        temp <<  "ATA/ATAPI-5";
        break;
    case 4:
        temp <<  "ATA/ATAPI-4";
        break;
    case 3:
        temp <<  "ATA-3";
        break;
    case 2:
        temp <<  "ATA-2";
        break;
    case 1:
        temp <<  "ATA-1";
        break;
    default:
        temp <<  " Unknown / Not Reported ";
        break;
    }
    json_push_back(identifyInfo, json_new_a("ATA Spec Supported", temp.str().c_str()));
    //minor spec version number
    temp.str("");temp.clear();
    temp << std::dec << m_sDriveInfo.minorSpecVersionNumber;
    json_push_back(identifyInfo, json_new_a("ATA Spec Minor Version Number", temp.str().c_str()));

    //Transport
    temp.str("");temp.clear();
    if (m_sDriveInfo.sTrans.transportReported)
    {
        switch (m_sDriveInfo.sTrans.transportType)
        {
        case 0://parallel
            switch (m_sDriveInfo.sTrans.transportCounter)
            {
            case 1:
                temp << "ATA/ATAPI-7";
                break;
            case 0:
                temp << "ATA8-APT";
                break;
            default:
                temp << "Reserved";
                break;
            }
            break;
        case 1://serial
            switch (m_sDriveInfo.sTrans.transportCounter)
            {
            case 8:
                temp << "SATA 3.3";
                break;
            case 7:
                temp << "SATA 3.2";
                break;
            case 6:
                temp << "SATA 3.1";
                break;
            case 5:
                temp << "SATA 3.0";
                break;
            case 4:
                temp << "SATA 2.6";
                break;
            case 3:
                temp << "SATA 2.5";
                break;
            case 2:
                temp << "SATA II - Extensions";
                break;
            case 1:
                temp << "SATA 1.0a";
                break;
            case 0:
                temp << "ATA8-AST";
                break;
            default:
                temp << "Reserved";
            }

            break;
        default://unknown/undefined, just dump the bits
            temp << "Unknown Transport";
            break;
        }
    }
    else
    {
        temp << "Not Reported";
    }
    json_push_back(identifyInfo, json_new_a("Transport Supported", temp.str().c_str()));

    temp.str("");temp.clear();
    temp << std::dec << m_sDriveInfo.sTrans.transportMinorVersion;
    json_push_back(identifyInfo, json_new_a("Transport Minor Version", temp.str().c_str()));

    // Form Factor
    temp.str("");temp.clear();
    switch (m_sDriveInfo.formFactor)
    {
    case 0:
        temp << "Not Reported";
        break;
    case 1:
        temp << "5.25 inch";
        break;
    case 2:
        temp << "3.5 inch";
        break;
    case 3:
        temp << "2.5 inch";
        break;
    case 4:
        temp << "1.8 inch";
        break;
    case 5:
        temp << "Less than 1.8 inch";
        break;
    default:
        temp << "Reserved";
        break;
    }
    json_push_back(identifyInfo, json_new_a("Form Factor", temp.str().c_str()));

    // Drives Capablilities 
    JSONNODE *driveCap = json_new(JSON_NODE);
    json_set_name(driveCap, "Device Capabilities");

    opensea_parser::set_Json_Bool(driveCap, "Request Sense Device Fault", m_sDriveInfo.sIdentCap.requestSenseDeviceFault);

    opensea_parser::set_Json_Bool(driveCap, "DSN", m_sDriveInfo.sIdentCap.DSN);

    opensea_parser::set_Json_Bool(driveCap, "Low Power Standby", m_sDriveInfo.sIdentCap.lowPowerStandby);

    opensea_parser::set_Json_Bool(driveCap, "Set EPC Power Source", m_sDriveInfo.sIdentCap.setEPCPowerSource);

    opensea_parser::set_Json_Bool(driveCap, "MAX ADDR", m_sDriveInfo.sIdentCap.maxAddr);

    opensea_parser::set_Json_Bool(driveCap, "DRAT", m_sDriveInfo.sIdentCap.DRAT);

    opensea_parser::set_Json_Bool(driveCap, "LPS Misalignment Reporting", m_sDriveInfo.sIdentCap.lpsMisalignmentReporting);

    opensea_parser::set_Json_Bool(driveCap, "Read Buffer DMA", m_sDriveInfo.sIdentCap.readBufferDMA);

    opensea_parser::set_Json_Bool(driveCap, "Write Buffer DMA", m_sDriveInfo.sIdentCap.writeBufferDMA);

    opensea_parser::set_Json_Bool(driveCap, "Download Microcode DMA", m_sDriveInfo.sIdentCap.downloadMicrocodeDMA);

    opensea_parser::set_Json_Bool(driveCap, "28-Bit", m_sDriveInfo.sIdentCap.bit28);

    opensea_parser::set_Json_Bool(driveCap, "RZAT", m_sDriveInfo.sIdentCap.rZAT);

    opensea_parser::set_Json_Bool(driveCap, "NOP", m_sDriveInfo.sIdentCap.nOP);

    opensea_parser::set_Json_Bool(driveCap, "Read Buffer", m_sDriveInfo.sIdentCap.readBuffer);

    opensea_parser::set_Json_Bool(driveCap, "Write Buffer", m_sDriveInfo.sIdentCap.WriteBuffer);

    opensea_parser::set_Json_Bool(driveCap, "Read Look-Ahead", m_sDriveInfo.sIdentCap.readLookAhead);

    opensea_parser::set_Json_Bool(driveCap, "Volatile Write Cache", m_sDriveInfo.sIdentCap.volatileWriteCache);

    opensea_parser::set_Json_Bool(driveCap, "SMART", m_sDriveInfo.sIdentCap.SMART);

    opensea_parser::set_Json_Bool(driveCap, "Flush Cache Ext", m_sDriveInfo.sIdentCap.flushCacheExt);

    opensea_parser::set_Json_Bool(driveCap, "48-Bit", m_sDriveInfo.sIdentCap.bit48);

    opensea_parser::set_Json_Bool(driveCap, "Spin-Up", m_sDriveInfo.sIdentCap.spinUp);

    opensea_parser::set_Json_Bool(driveCap, "PUIS", m_sDriveInfo.sIdentCap.puis);

    opensea_parser::set_Json_Bool(driveCap, "APM", m_sDriveInfo.sIdentCap.APM);

    opensea_parser::set_Json_Bool(driveCap, "CFA", m_sDriveInfo.sIdentCap.CFA);

    opensea_parser::set_Json_Bool(driveCap, "Download Microcode", m_sDriveInfo.sIdentCap.downloadMicrocode);

    opensea_parser::set_Json_Bool(driveCap, "Unload", m_sDriveInfo.sIdentCap.unload);

    opensea_parser::set_Json_Bool(driveCap, "Write FUA Ext", m_sDriveInfo.sIdentCap.writeFUAExt);

    opensea_parser::set_Json_Bool(driveCap, "GPL", m_sDriveInfo.sIdentCap.GPL);

    opensea_parser::set_Json_Bool(driveCap, "Streaming", m_sDriveInfo.sIdentCap.Streaming);

    opensea_parser::set_Json_Bool(driveCap, "Smart Self-Test", m_sDriveInfo.sIdentCap.smartSelfTest);

    opensea_parser::set_Json_Bool(driveCap, "Smart Error Logging", m_sDriveInfo.sIdentCap.smartErrorLogging);

    opensea_parser::set_Json_Bool(driveCap, "EPC", m_sDriveInfo.sIdentCap.EPC);

    opensea_parser::set_Json_Bool(driveCap, "Sense Data", m_sDriveInfo.sIdentCap.senseData);

    opensea_parser::set_Json_Bool(driveCap, "Free-Fall", m_sDriveInfo.sIdentCap.freeFall);

    opensea_parser::set_Json_Bool(driveCap, "DM Mode 3", m_sDriveInfo.sIdentCap.DMMode3);

    opensea_parser::set_Json_Bool(driveCap, "GPL DM", m_sDriveInfo.sIdentCap.GPL_DM);

    opensea_parser::set_Json_Bool(driveCap, "Write Uncorrectable", m_sDriveInfo.sIdentCap.writeUncorrectable);

    opensea_parser::set_Json_Bool(driveCap, "WRV", m_sDriveInfo.sIdentCap.WRV);

    json_push_back(identifyInfo, driveCap);

    // security
    if (m_sDriveInfo.SecurityID != 0X0000)
    {
        JSONNODE *secruity = json_new(JSON_NODE);
        json_set_name(secruity, "Device Security");

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_sDriveInfo.SecurityID;
        json_push_back(secruity, json_new_a("Security Status Bit:", temp.str().c_str()));

        opensea_parser::set_Json_Bool(secruity, "Security Supported", m_sDriveInfo.sSecurityInfo.supported);

        json_push_back(secruity, json_new_i("Master Password Capability", m_sDriveInfo.sSecurityInfo.masterPasswordCapability));

        opensea_parser::set_Json_Bool(secruity, "Enhanced Security Erase Supported", m_sDriveInfo.sSecurityInfo.enhancedSecurityEraseSupported);

        opensea_parser::set_Json_Bool(secruity, "Security Count Expired", m_sDriveInfo.sSecurityInfo.securityCountExpired);

        opensea_parser::set_Json_Bool(secruity, "Security Frozen", m_sDriveInfo.sSecurityInfo.frozen);

        opensea_parser::set_Json_Bool(secruity, "Security Locked", m_sDriveInfo.sSecurityInfo.locked);

        opensea_parser::set_Json_Bool(secruity, "Security Enabled", m_sDriveInfo.sSecurityInfo.enabled);

        json_push_back(identifyInfo, secruity);
    }

    json_push_back(masterData, identifyInfo);
    return eReturnValues::SUCCESS;
}
// *******************************************************************************
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_00()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 00
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_00::CAta_Identify_Log_00(uint8_t *Buffer)
    : m_name("ATA Identify Log Page 00")
    , m_status(eReturnValues::IN_PROGRESS)
    , m_pLog0()
{
    pData = Buffer;
    if (pData != M_NULLPTR)
    {
        m_pLog0 = reinterpret_cast<sLogPage00 *>(pData);
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        printf(" create the Log00 class -> NULL \n");
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_00()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_00
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_00::~CAta_Identify_Log_00()
{
}
//-----------------------------------------------------------------------------
//
//! \fn  is_Page_Supported()
//
//! \brief
//!   Description:  Call to find out if the page is supported 
//
//  Entry:
//!  pageNumber   = give it the page number and it will return true if it is supporte else return false
//
//  Exit:
//!  \return bool  
//
//-------------------------------------------------------------------------
bool CAta_Identify_Log_00::is_Page_Supported(uint8_t pageNumber)
{
    bool isSupported = false;
    if (pageNumber == 0)
    {
        if (m_pLog0->pageSupported[pageNumber] == 0x0)
        {
            isSupported = true;
        }
    }
    else
    {
        if (m_pLog0->pageSupported[pageNumber] != 0x0)
        {
            isSupported = true;
        }
    }
    return isSupported;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page00()
//
//! \brief
//!   Description:  parse Log page 00
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_00::get_Log_Page00(JSONNODE *masterData)
{
#define LOG_PAGE_00   0x0000
    uint16_t pageNumber = 0;
    uint16_t revision = 0;
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;

    pageNumber = M_Word1(m_pLog0->header);
    revision = M_Word0(m_pLog0->header);
    if (pageNumber == LOG_PAGE_00)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 00h");
#if defined _DEBUG
        printf("Log Page 00h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", m_pLog0->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog0->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));
        // get the pages supported
        for (uint8_t pageNumber_1 = 0; pageNumber_1 <= MAX_NUMBER_OF_ENTRIES; pageNumber_1++)
        {
            temp.str("");temp.clear();
            temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(pageNumber_1);//cast is to make sure it doesn't try interpretting a uint8 as a char instead of a decimal - TJE
            opensea_parser::set_Json_Bool(pageInfo, temp.str().c_str(), is_Page_Supported(pageNumber_1));
        }

        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}

// *******************************************************************************
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_02()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 02
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_02::CAta_Identify_Log_02(uint8_t *Buffer)
    : m_name("ATA Identify Log Page 02")
    , pData(Buffer)
    , m_status(eReturnValues::IN_PROGRESS)
    , pCapacity()
{
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_02()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_02
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_02::~CAta_Identify_Log_02()
{
}

//-----------------------------------------------------------------------------
//
//! \fn get_Device_Capacity()
//
//! \brief
//!   Description:  parse and Formats the device capacity;
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_02::get_Device_Capacity(JSONNODE *capData)
{
#if defined _DEBUG
    printf("Device Capacity \n");
    printf("\tDevice Capacity =   0x%016" PRIx64"  \n", pCapacity->deviceCapacity);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase <<std::setfill('0') << std::setw(16) << pCapacity->deviceCapacity;
    json_push_back(capData, json_new_a("Device Capacity Header", temp.str().c_str()));
    if (check_For_Active_Status(&pCapacity->deviceCapacity))
    {
        uint64_t myUint48 = pCapacity->deviceCapacity &  MAX_48_BIT_LBA;
        double capacity = static_cast<double>(myUint48);
        uint8_t unitCounter = 0;
        while ((capacity / 1024.0) >= 1 && (unitCounter + 1) < 8)
        {
            capacity = capacity / 1024.00;
            unitCounter += 1;
        }
        temp.str("");temp.clear();
        switch (unitCounter)
        {
        case 0:
            temp << std::dec << myUint48 << "B";
            break;
        case 1:
            temp << std::dec << myUint48 << "KiB";
            break;
        case 2:
            temp << std::dec << myUint48 << "MiB";
            break;
        case 3:
            temp << std::dec << myUint48 << "GiB";
            break;
        case 4:
            temp << std::dec << myUint48 << "TiB";
            break;
        case 5:
            temp << std::dec << myUint48 << "PiB";
            break;
        case 6:
            temp << std::dec << myUint48 << "EiB";
            break;
        case 7:
            temp << std::dec << myUint48 << "ZiB";
            break;
        case 8:
            temp << std::dec << myUint48 << "YiB";
            break;
        default:
            temp << "unknown size";
            break;
        }
#if defined _DEBUG
        printf("\tDevice Capacity =   %" PRIu64"  \n", myUint48);
        printf("\tCapacity =   %s \n", temp.str().c_str());
#endif
        json_push_back(capData, json_new_a("Capacity", temp.str().c_str()));

    }
    else
    {
        return false;
    }
    return true;
}

bool CAta_Identify_Log_02::get_Sector_Size(JSONNODE *sectorData)
{
#if defined _DEBUG
    printf("Physical / Logical sector size \n");
    printf("\tSector Size =   0x%016" PRIx64"  \n", pCapacity->sectorSize);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pCapacity->sectorSize;
    json_push_back(sectorData, json_new_a("Pysical and Logical Sector Size", temp.str().c_str()));
    if (check_For_Active_Status(&pCapacity->deviceCapacity))
    {
        if (pCapacity->sectorSize & BIT62)
        {
            opensea_parser::set_Json_Bool(sectorData, "Logical to Physical Sector Relationship Supported bit", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(sectorData, "Logical to Physical Sector Relationship Supported bit", false);
        }
        if (pCapacity->sectorSize & BIT61)
        {
            opensea_parser::set_Json_Bool(sectorData, "Logical Sector Size Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(sectorData, "Logical Sector Size Supported", false);
        }
        uint8_t alignment = M_Byte2(pCapacity->sectorSize);
        if (alignment & BIT1)
        {
            opensea_parser::set_Json_Bool(sectorData, "Long Physical Sector Alignment Error reporting is enabled", true);
        }
        else if (alignment & BIT2)
        {
            opensea_parser::set_Json_Bool(sectorData, "device shall report command aborted if an Alignment Error occurs", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(sectorData, "Long Physical Sector Alignment Error reporting is enabled", false);
        }
        uint16_t sectorOffset = M_Word0(pCapacity->sectorSize);
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << sectorOffset;
        json_push_back(sectorData, json_new_a("Logical Sector Offset", temp.str().c_str()));
    }

    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page02()
//
//! \brief
//!   Description:  parse Log page 02
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_02::get_Log_Page02(uint8_t *lp2pData, JSONNODE *masterData)
{
#define LOG_PAGE_02   0x0002
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage02 logPage02;
    pCapacity = &logPage02;
    pCapacity = reinterpret_cast<sLogPage02*>(&lp2pData[0]);
    uint16_t pageNumber = M_Word1(pCapacity->header);
    uint16_t revision = M_Word0(pCapacity->header);
    if (pageNumber == LOG_PAGE_02)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 02h");
#if defined _DEBUG
        printf("Log Page 02h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", pCapacity->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pCapacity->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));

        get_Device_Capacity(pageInfo);
        get_Sector_Size(pageInfo);


        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}

// *******************************************************************************

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_03()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 03
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_03::CAta_Identify_Log_03(uint8_t *Buffer)
    : m_name("ATA Identify Log Page 03")
    , pData(Buffer)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_pCap()
    , m_sSupported()
    , m_sDownloadMicrocode()
    , m_sSCTCap()
{
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_03()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_03
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_03::~CAta_Identify_Log_03()
{
}
//-----------------------------------------------------------------------------
//
//! \fn set_Supported_Capabilities()
//
//! \brief
//!   Description:  checks for active status and then checks all the bits and sets the sturcture TRUE OR FALSE
//
//  Entry:
//! \param ptr  pointer to the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::set_Supported_Capabilities(uint64_t *value)
{
    if (check_For_Active_Status(value))
    {
        if (*value & BIT0)
        {
            m_sSupported.WRVSupported = true;
        }
        if (*value & BIT1)
        {
            m_sSupported.writeUnccorctableSupportd = true;
        }
        if (*value & BIT2)
        {
            m_sSupported.GPLDMASupportd = true;
        }
        if (*value & BIT3)
        {
            m_sSupported.DMMode3Supported = true;
        }
        if (*value & BIT4)
        {
            m_sSupported.freeFallSupported = true;
        }
        if (*value & BIT5)
        {
            m_sSupported.senseDataSupported = true;
        }
        if (*value & BIT6)
        {
            m_sSupported.epcSupported = true;
        }
        if (*value & BIT7)
        {
            m_sSupported.smartErrorLoggingSupported = true;
        }
        if (*value & BIT8)
        {
            m_sSupported.smartSelfTestSupported = true;
        }
        // bit 9 is a reserved
        if (*value & BIT10)
        {
            m_sSupported.streamingSupported = true;
        }
        if (*value & BIT11)
        {
            m_sSupported.GPLSupported = true;
        }
        if (*value & BIT12)
        {
            m_sSupported.writeFUAExtSupported = true;
        }
        if (*value & BIT13)
        {
            m_sSupported.unloadSupported = true;
        }
        if (*value & BIT14)
        {
            m_sSupported.downloadMicroCodeSuppored = true;
        }
        if (*value & BIT15)
        {
            m_sSupported.CFASupported = true;
        }
        if (*value & BIT16)
        {
            m_sSupported.APMSupporteed = true;
        }
        if (*value & BIT17)
        {
            m_sSupported.PUISSupported = true;
        }
        if (*value & BIT18)
        {
            m_sSupported.spinupSupported = true;
        }
        // bit 19 is reserved
        if (*value & BIT20)
        {
            m_sSupported.bit48Supported = true;
        }
        // bit 21 is reserved
        if (*value & BIT22)
        {
            m_sSupported.flushCacheExtSupported = true;
        }
        if (*value & BIT23)
        {
            m_sSupported.smartSupported = true;
        }
        if (*value & BIT24)
        {
            m_sSupported.volatileWriteCacheSupported = true;
        }
        if (*value & BIT25)
        {
            m_sSupported.readLookAheadSupported = true;
        }
        //bit 26 is reserved
        if (*value & BIT27)
        {
            m_sSupported.writeBufferSupported = true;
        }
        if (*value & BIT28)
        {
            m_sSupported.readBufferSupported = true;
        }
        if (*value & BIT29)
        {
            m_sSupported.NOPSupported = true;
        }
        // bit 30 is reserved
        if (*value & BIT31)
        {
            m_sSupported.RZATSupported = true;
        }
        if (*value & BIT32)
        {
            m_sSupported.bit28Supported = true;
        }
        if (*value & BIT33)
        {
            m_sSupported.downLoadMicroCodeDMASupported = true;
        }
        //bit 34 is reserved
        if (*value & BIT35)
        {
            m_sSupported.writeBufferDMASupported = true;
        }
        if (*value & BIT36)
        {
            m_sSupported.readBufferDMASupported = true;
        }
        // bit 37 is reserved
        if (*value & BIT38)
        {
            m_sSupported.LPSMisalignmentReportingSupported = true;
        }
        if (*value & BIT39)
        {
            m_sSupported.DRATSupported = true;
        }
        // bit 40 is reserved
        if (*value & BIT41)
        {
            m_sSupported.AMAXAddrSupported = true;
        }
        if (*value & BIT42)
        {
            m_sSupported.setEPCPowerSourceSupported = true;
        }
        if (*value & BIT43)
        {
            m_sSupported.lowPowerStandbySupported = true;
        }
        if (*value & BIT44)
        {
            m_sSupported.DSNSupported = true;
        }
        if (*value & BIT45)
        {
            m_sSupported.senseDeviceFaultSupported = true;
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn set_DownLoad_MicroCode()
//
//! \brief
//!   Description:  checks for active status and then checks all the bits and sets the sturcture TRUE OR FALSE
//
//  Entry:
//! \param ptr  pointer to the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::set_DownLoad_MicroCode(uint64_t *value)
{
    if (check_For_Active_Status(value))
    {
        if (*value & BIT34)
        {
            m_sDownloadMicrocode.dmOffsetsImmediateSupported = true;
        }
        if (*value & BIT33)
        {
            m_sDownloadMicrocode.dmImmedaiateSupported = true;
        }
        if (*value & BIT32)
        {
            m_sDownloadMicrocode.dmDefferedSupported = true;
        }
        m_sDownloadMicrocode.dmMaxTransferSize = M_Word1(*value);
        m_sDownloadMicrocode.dmMinTranserSize = M_Word0(*value);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Supported_Capabilities()
//
//! \brief
//!   Description:  parse and Formats the current  settings of page 03 Supported Capabilities
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Supported_Capabilities(JSONNODE *currentData)
{
#if defined _DEBUG
    printf("Current Settings \n");
    printf("\tCurrent Settings =   0x%016" PRIx64"  \n", m_pCap->supportedCapabilities);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->supportedCapabilities;
    json_push_back(currentData, json_new_a("Supported Capabilities", temp.str().c_str()));
    if (set_Supported_Capabilities(&m_pCap->supportedCapabilities))
    {
        JSONNODE *currentSettings = json_new(JSON_NODE);
        json_set_name(currentSettings, "Supported Capabilities bits");
        opensea_parser::set_Json_Bool(currentSettings, "Request Sense Device Fault Supported", m_sSupported.senseDeviceFaultSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Dsn Supported", m_sSupported.DSNSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Low Power Standby Supported", m_sSupported.lowPowerStandbySupported);
        opensea_parser::set_Json_Bool(currentSettings, "Set Epc Power Source Supported", m_sSupported.setEPCPowerSourceSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Amax Addr Supported", m_sSupported.AMAXAddrSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Drat Supported", m_sSupported.DRATSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Lps Misalignment Reporting Supported", m_sSupported.LPSMisalignmentReportingSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Read Buffer Dma Supported", m_sSupported.readBufferDMASupported);
        opensea_parser::set_Json_Bool(currentSettings, "Write Buffer Dma Supported", m_sSupported.writeBufferDMASupported);
        opensea_parser::set_Json_Bool(currentSettings, "Download Microcode Dma Supported", m_sSupported.downLoadMicroCodeDMASupported);
        opensea_parser::set_Json_Bool(currentSettings, "28-bit Supported", m_sSupported.bit28Supported);
        opensea_parser::set_Json_Bool(currentSettings, "Rzat Supported", m_sSupported.RZATSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Nop Supported", m_sSupported.NOPSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Read Buffer Supported", m_sSupported.readBufferSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Write Buffer Supported", m_sSupported.writeBufferSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Read Look-Ahead Supported", m_sSupported.readLookAheadSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Volatile Write Cache Supported", m_sSupported.volatileWriteCacheSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Smart", m_sSupported.smartSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Flush Cache Ext Supported", m_sSupported.flushCacheExtSupported);
        opensea_parser::set_Json_Bool(currentSettings, "48-Bit Supported", m_sSupported.bit48Supported);
        opensea_parser::set_Json_Bool(currentSettings, "Spin-Up Supported", m_sSupported.spinupSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Puis Supported", m_sSupported.PUISSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Apm Supported", m_sSupported.APMSupporteed);
        opensea_parser::set_Json_Bool(currentSettings, "CFA Supported", m_sSupported.CFASupported);
        opensea_parser::set_Json_Bool(currentSettings, "Download Microcode Supported", m_sSupported.downloadMicroCodeSuppored);
        opensea_parser::set_Json_Bool(currentSettings, "Unload Supported", m_sSupported.unloadSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Write Fua Ext Supported", m_sSupported.writeFUAExtSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Gpl Supported", m_sSupported.GPLSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Streaming Supported", m_sSupported.streamingSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Smart Self-Test Supported", m_sSupported.smartSelfTestSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Smart Error Logging Supported", m_sSupported.smartErrorLoggingSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Epc Supported", m_sSupported.epcSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Sense Data Supported", m_sSupported.senseDataSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Free-Fall Supported", m_sSupported.freeFallSupported);
        opensea_parser::set_Json_Bool(currentSettings, "Dm Mode 3 Supported", m_sSupported.DMMode3Supported);
        opensea_parser::set_Json_Bool(currentSettings, "Gpl Dma Supported", m_sSupported.GPLDMASupportd);
        opensea_parser::set_Json_Bool(currentSettings, "Write Uncorrectable Supported", m_sSupported.writeUnccorctableSupportd);
        opensea_parser::set_Json_Bool(currentSettings, "Wrv Supported", m_sSupported.WRVSupported);
        json_push_back(currentData, currentSettings);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_DownLoad_MicroCode()
//
//! \brief
//!   Description:  parse and Formats the current  settings of page 03 Download Microcode 
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_DownLoad_MicroCode(JSONNODE *DM)
{
#if defined _DEBUG
    printf("Download Microcode \n");
    printf("\tDownload Microcode =   0x%016" PRIx64"  \n", m_pCap->download);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->download;
    json_push_back(DM, json_new_a("Download Microcode ", temp.str().c_str()));
    if (set_DownLoad_MicroCode(&m_pCap->download))
    {
        JSONNODE *download = json_new(JSON_NODE);
        json_set_name(download, "Download Microcode Capabilities");
        opensea_parser::set_Json_Bool(download, "DM Offsets Deferred Supported", m_sDownloadMicrocode.dmDefferedSupported);
        opensea_parser::set_Json_Bool(download, "DM Immediate Supported", m_sDownloadMicrocode.dmImmedaiateSupported);
        opensea_parser::set_Json_Bool(download, "DM Offsets Immediate Supported", m_sDownloadMicrocode.dmOffsetsImmediateSupported);
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_sDownloadMicrocode.dmMaxTransferSize;
        json_push_back(download, json_new_a("Dm Maximum Transfer Size", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_sDownloadMicrocode.dmMinTranserSize;
        json_push_back(download, json_new_a("Dm Minimum Transfer Size", temp.str().c_str()));

        json_push_back(DM, download);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_DownLoad_MicroCode()
//
//! \brief
//!   Description:  parse and Formats the current  settings of page 03 Download Microcode 
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Media_Rotation_Rate(JSONNODE *MRR)
{
#define NOT_REPORTED        0x0000
#define NON_ROTATING_MEDIA  0x0001
#define RESERVED_RATE       0xFFFF
#if defined _DEBUG
    printf("Media Rotation Rate \n");
    printf("\tMedia Rotation Rate =   0x%016" PRIx64"  \n", m_pCap->rotationRate);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->rotationRate;
    json_push_back(MRR, json_new_a("Media Rotation Rate", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->rotationRate))
    {
        uint16_t rate = M_Word0(m_pCap->rotationRate);

        if (rate == NOT_REPORTED)
        {
            opensea_parser::set_Json_Bool(MRR, "Rotation Rate NOT Reported", true);
        }
        else if (rate == NON_ROTATING_MEDIA)
        {
            opensea_parser::set_Json_Bool(MRR, "Non-Roatating Media", true);
        }
        else if (rate == RESERVED_RATE)
        {
            opensea_parser::set_Json_Bool(MRR, "Reserved", true);
        }
        else
        {
            temp.str("");temp.clear();
            temp << std::dec << rate;
            json_push_back(MRR, json_new_a("Nominal Media Rotation Rate", temp.str().c_str()));
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Form_Factor()
//
//! \brief
//!   Description:  parse and Formats the data for the form factor
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Form_Factor(JSONNODE *ff)
{
#if defined _DEBUG
    printf("Form Factor \n");
    printf("\tForm Factor =   0x%016" PRIx64"  \n", m_pCap->formFactor);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->formFactor;
    json_push_back(ff, json_new_a("Form Factor", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->formFactor))
    {

        uint8_t form = M_Byte0(m_pCap->formFactor);
        if (form == 0)
        {
            opensea_parser::set_Json_Bool(ff, "Nominal Form Factor not reported", true);
        }
        else if (form == 1)
        {
            opensea_parser::set_Json_Bool(ff, "5.25 inch nominal Form Factor", true);
        }
        else if (form == 2)
        {
            opensea_parser::set_Json_Bool(ff, "3.5 inch nominal Form Factor", true);
        }
        else if (form == 3)
        {
            opensea_parser::set_Json_Bool(ff, "2.5 inch nominal Form Factor", true);
        }
        else if (form == 4)
        {
            opensea_parser::set_Json_Bool(ff, "1.8 inch nominal Form Factor", true);
        }
        else if (form == 5)
        {
            opensea_parser::set_Json_Bool(ff, "Less than 1.8 inch nominal Form Factor", true);
        }
        else
        {
            temp.str("");temp.clear();
            temp << std::dec << static_cast<uint16_t>(form);
            json_push_back(ff, json_new_a("Nominal Form Factor", temp.str().c_str()));
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Write_Read_Verify_Mode3()
//
//! \brief
//!   Description:  parse and Formats the write and read verify information for mode 3
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Write_Read_Verify_Mode3(JSONNODE *mode3)
{
#if defined _DEBUG
    printf("Write Read Verify Mode 3 \n");
    printf("\tWrite Read Verify =   0x%016" PRIx64"  \n", m_pCap->mode3);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->mode3;
    json_push_back(mode3, json_new_a("Write Read Verify Mode 3", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->mode3))
    {
        JSONNODE *wrv = json_new(JSON_NODE);
        json_set_name(wrv, "Write Read Verify sector count Mode 3");
        temp.str("");temp.clear();
        temp << std::dec << M_DoubleWord0(m_pCap->mode3);
        json_push_back(wrv, json_new_a("WRV Mode 3 count", temp.str().c_str()));

        json_push_back(mode3, wrv);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Write_Read_Verify_Mode2()
//
//! \brief
//!   Description:  parse and Formats the write and read verify information for mode 2
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Write_Read_Verify_Mode2(JSONNODE *mode2)
{
#if defined _DEBUG
    printf("Write Read Verify Mode 2 \n");
    printf("\tWrite Read Verify =   0x%016" PRIx64"  \n", m_pCap->mode2);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->mode2;
    json_push_back(mode2, json_new_a("Write Read Verify Mode 2", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->mode2))
    {
        JSONNODE *wrv = json_new(JSON_NODE);
        json_set_name(wrv, "Write Read Verify sector count Mode 2");
        temp.str("");temp.clear();
        temp << std::dec << M_DoubleWord0(m_pCap->mode2);
        json_push_back(wrv, json_new_a("WRV Mode 2 count", temp.str().c_str()));

        json_push_back(mode2, wrv);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn world_Wide_Name()
//
//! \brief
//!   Description:  parse and Formats the work wide name
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::world_Wide_Name(JSONNODE *WWN)
{
#if defined _DEBUG
    printf("World Wide Name \n");
    printf("\tWorld Wide Name =   0x%04" PRIx16"  \n", m_pCap->wwn);
    printf("\tWorld Wide Name2 =   0x%04" PRIx16"  \n", m_pCap->wwn2);
#endif
    if (check_For_Active_Status(&m_pCap->wwnStatus))
    {
        uint64_t wwnReal = M_WordsTo8ByteValue(m_pCap->wwn, m_pCap->wwn1, m_pCap->wwn2, m_pCap->wwn3);
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << wwnReal;
        std::string wwnStr = temp.str();
        ltrim(wwnStr);
        rtrim(wwnStr);
        // world wide name

        json_push_back(WWN, json_new_a("World Wide Name", wwnStr.c_str()));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data_Set_Managment()
//
//! \brief
//!   Description:  parse and Formats the current data Set Managment
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Data_Set_Managment(JSONNODE *setManagement)
{
#if defined _DEBUG
    printf("Data Set Management \n");
    printf("\tData Set Management =   0x%016" PRIx64"  \n", m_pCap->dataSet);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->dataSet;
    json_push_back(setManagement, json_new_a("Data Set Management Value", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->dataSet))
    {
        if (m_pCap->dataSet & BIT0)
        {
            opensea_parser::set_Json_Bool(setManagement, "Trim Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(setManagement, "Trim Supported", false);
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Utilization_Unit_Time()
//
//! \brief
//!   Description:  parse and Formats the current Utilaization unit time
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Utilization_Unit_Time(JSONNODE *unitTime)
{
#if defined _DEBUG
    printf("Utilization Per Unit Time \n");
    printf("\tUtilization Per Unit Time =   0x%016" PRIx64"  \n", m_pCap->utilization);
    printf("\tUtilization Per Unit Time2 =   0x%016" PRIx64"  \n", m_pCap->utilTime2);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->utilization;
    json_push_back(unitTime, json_new_a("Utilization Per Unit Time", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->utilTime2;
    json_push_back(unitTime, json_new_a("Utilization Per Unit Time2", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->utilization))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Utilization Per Unit Time");
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_Byte2(m_pCap->utilization));
        json_push_back(data, json_new_a("Utiliztion Type field", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_Byte1(m_pCap->utilization));
        json_push_back(data, json_new_a("Utiliztion Unites Field", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(M_Byte0(m_pCap->utilization));
        json_push_back(data, json_new_a("Utiliztion Interval Field", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << M_DoubleWord1(m_pCap->utilTime2);
        json_push_back(data, json_new_a("Utiliztion B Field", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << M_DoubleWord0(m_pCap->utilTime2);
        json_push_back(data, json_new_a("Utiliztion A Field", temp.str().c_str()));

        json_push_back(unitTime, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Utilization_Usage_Rate_Support()
//
//! \brief
//!   Description:  parse and Formats the current utilization usage rate supported
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Utilization_Usage_Rate_Support(JSONNODE *rate)
{
#if defined _DEBUG
    printf("Utilization Usage Rate Support \n");
    printf("\tUtilization Usage Rate Support =   0x%016" PRIx64"  \n", m_pCap->utilRate);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->utilRate;
    json_push_back(rate, json_new_a("Utilization Usage Rate Support", temp.str().c_str()));

    if (check_For_Active_Status(&m_pCap->utilRate))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Utilization Usage Rate Support");
        if (m_pCap->utilRate & BIT23)
        {
            opensea_parser::set_Json_Bool(data, "Setting Rate Basis Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Setting Rate Basis Supported", false);
        }
        if (m_pCap->utilRate & BIT8)
        {
            opensea_parser::set_Json_Bool(data, "Since Power On Rate Basis Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Since Power On Rate Basis Supported", false);
        }
        if (m_pCap->utilRate & BIT4)
        {
            opensea_parser::set_Json_Bool(data, "Power On Hours Rate Basis Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Power On Hours Rate Basis Supported", false);
        }
        if (m_pCap->utilRate & BIT0)
        {
            opensea_parser::set_Json_Bool(data, "Date/Time Rate Basis Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Date/Time Rate Basis Supported", false);
        }
        json_push_back(rate, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Zoned_Capabilities()
//
//! \brief
//!   Description:  parse and Formats the current zoned Capabilities
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Zoned_Capabilities(JSONNODE *zoned)
{
#if defined _DEBUG
    printf("Zoned Capabilities \n");
    printf("\tZoned Capabilities =   0x%016" PRIx64"  \n", m_pCap->zoned);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->zoned;
    json_push_back(zoned, json_new_a("Zoned Capabilities", temp.str().c_str()));

    if (check_For_Active_Status(&m_pCap->zoned))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Zoned Capabilities");
        if (m_pCap->zoned & BIT0)
        {
            opensea_parser::set_Json_Bool(data, "Not reported", true);
        }
        else if (m_pCap->zoned & BIT1)
        {
            opensea_parser::set_Json_Bool(data, "Device supports the Host Aware Zones feature set", true);
        }
        else if (m_pCap->zoned & BIT0 && m_pCap->zoned & BIT1)
        {
            opensea_parser::set_Json_Bool(data, "Device supports device managed zoned device capabilities", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "reserved", true);
        }

        json_push_back(zoned, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Supported_ZAC_Capabilities()
//
//! \brief
//!   Description:  parse and Formats the current Supported ZAC Capabilities
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Supported_ZAC_Capabilities(JSONNODE *zac)
{
#if defined _DEBUG
    printf("Supported ZAC Capabilitiest \n");
    printf("\tSupported ZAC Capabilities=   0x%016" PRIx64"  \n", m_pCap->ZAC);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->ZAC;
    json_push_back(zac, json_new_a("Supported ZAC Capabilities", temp.str().c_str()));

    if (check_For_Active_Status(&m_pCap->ZAC))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Supported ZAC Capabilities");
        if (m_pCap->ZAC & BIT4)
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Reset Write Pointers Ext Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Reset Write Pointers Ext Supported", false);
        }
        if (m_pCap->ZAC & BIT3)
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Finish Zone Ext Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Finish Zone Ext Supported", false);
        }
        if (m_pCap->ZAC & BIT2)
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Close Zone Ext Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Close Zone Ext Supported", false);
        }
        if (m_pCap->ZAC & BIT1)
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Open Zone Ext Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Non-Data Open Zone Ext Supported", false);
        }
        if (m_pCap->ZAC & BIT0)
        {
            opensea_parser::set_Json_Bool(data, "Report Zones Ext Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Report Zones Ext Supported", false);
        }
        json_push_back(zac, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Background_Operations_Capabilities()
//
//! \brief
//!   Description:  parse and Formats the current data background operations capabilities
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Background_Operations_Capabilities(JSONNODE *bgOperation)
{
#if defined _DEBUG
    printf("Advanced Background Operations Capabilities \n");
    printf("\tAdvanced Background Operations Capabilities =   0x%016" PRIx64"  \n", m_pCap->backgroundOpsCapabilities);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->backgroundOpsCapabilities;
    json_push_back(bgOperation, json_new_a("Advanced Background Operations Capabilities", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->backgroundOpsCapabilities))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Advanced Background Operations Capabilities");
        if (m_pCap->backgroundOpsCapabilities & BIT62)
        {
            opensea_parser::set_Json_Bool(data, "ABO Foreground Mode Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "ABO Foreground Mode Supported", false);
        }
        if (m_pCap->backgroundOpsCapabilities & BIT61)
        {
            opensea_parser::set_Json_Bool(data, "ABO IR Mode Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "ABO IR Mode Supported", false);
        }
        temp.str("");temp.clear();
        temp << std::dec << M_WordsTo4ByteValue(M_Word2(m_pCap->backgroundOpsCapabilities), M_Word1(m_pCap->backgroundOpsCapabilities));
        json_push_back(data, json_new_a("ABO Minimum Fraction field", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << std::dec << M_Word0(m_pCap->backgroundOpsCapabilities);
        json_push_back(data, json_new_a("ABO Minimum Supported Timelimit field", temp.str().c_str()));
        json_push_back(bgOperation, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Background_Operations_Recommendations()
//
//! \brief
//!   Description:  parse and Formats the current data for background operations recommendations
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Background_Operations_Recommendations(JSONNODE *recommendations)
{
#if defined _DEBUG
    printf("Advanced Background Operations Recommendations \n");
    printf("\tAdvanced Background Operations Recommendations =   0x%016" PRIx64"  \n", m_pCap->backgroundOpsRecommendations);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->backgroundOpsRecommendations;
    json_push_back(recommendations, json_new_a("Advanced Background Operations Recommendations", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->backgroundOpsRecommendations))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Advanced Background Operations Recommendations");
        temp.str("");temp.clear();
        temp << std::dec << M_Word1(m_pCap->backgroundOpsRecommendations);
        json_push_back(data, json_new_a("Device Maintenance Polling Time field", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << M_Word0(m_pCap->backgroundOpsRecommendations);
        json_push_back(data, json_new_a("ABO Recommended Abo Start Interval field", temp.str().c_str()));
        json_push_back(recommendations, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Queue_Depth()
//
//! \brief
//!   Description:  pget the queue depth
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Queue_Depth(JSONNODE *qd)
{
#if defined _DEBUG
    printf("Queue Depth \n");
    printf("\tQueue Depth =   0x%016" PRIx64"  \n", m_pCap->queueDepth);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->queueDepth;
    json_push_back(qd, json_new_a("Queue Depth", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->queueDepth))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Queue Depth");
        temp.str("");temp.clear();
        temp << std::dec << M_Byte0(m_pCap->queueDepth);
        json_push_back(data, json_new_a("Queue Depth", temp.str().c_str()));

        json_push_back(qd, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn set_SCT_Capabilities()
//
//! \brief
//!   Description:  set the sturcture to true if the bit is set
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::set_SCT_Capabilities(uint64_t *value)
{
    if (check_For_Active_Status(value))
    {
        if (*value & BIT0)
        {
            m_sSCTCap.sct_Supported = true;
        }
        if (*value & BIT2)
        {
            m_sSCTCap.sct_Write_Same_Supported = true;
        }
        if (*value & BIT3)
        {
            m_sSCTCap.sct_Error_Recovery_Control_Supported = true;
        }
        if (*value & BIT4)
        {
            m_sSCTCap.sct_Feature_Control_Supported = true;
        }
        if (*value & BIT5)
        {
            m_sSCTCap.sct_Data_Tables_Supported = true;
        }
        if (*value & BIT16)
        {
            m_sSCTCap.sct_Write_Same_Function_1_Supported = true;
        }
        if (*value & BIT17)
        {
            m_sSCTCap.sct_Write_Same_Function_2_Supported = true;
        }
        if (*value & BIT18)
        {
            m_sSCTCap.sct_Write_Same_Function_3_Supported = true;
        }
        if (*value & BIT24)
        {
            m_sSCTCap.sct_Write_Same_Function_101_Supported = true;
        }
        if (*value & BIT25)
        {
            m_sSCTCap.sct_Write_Same_Function_102_Supported = true;
        }
        if (*value & BIT26)
        {
            m_sSCTCap.sct_Write_Same_Function_103_Supported = true;
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_SCT_Capabilities()
//
//! \brief
//!   Description:  pget the queue depth
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_SCT_Capabilities(JSONNODE *sct)
{
#if defined _DEBUG
    printf("Supported SCT Capabilities \n");
    printf("\tSupported SCT Capabilities =   0x%016" PRIx64"  \n", m_pCap->sctCapabilities);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->sctCapabilities;
    json_push_back(sct, json_new_a("Supported SCT Capabilities", temp.str().c_str()));
    if (set_Supported_Capabilities(&m_pCap->sctCapabilities))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Supported SCT Capabilities");
        opensea_parser::set_Json_Bool(data, "Sct Write Same Function 103 Supported", m_sSCTCap.sct_Write_Same_Function_103_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Write Same Function 102 Supported", m_sSCTCap.sct_Write_Same_Function_102_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Write Same Function 101 Supported", m_sSCTCap.sct_Write_Same_Function_101_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Write Same Function 3 Supported", m_sSCTCap.sct_Write_Same_Function_3_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Write Same Function 2 Supported", m_sSCTCap.sct_Write_Same_Function_2_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Write Same Function 1 Supported", m_sSCTCap.sct_Write_Same_Function_1_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Data Tables Supported", m_sSCTCap.sct_Data_Tables_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Feature Control Supported", m_sSCTCap.sct_Feature_Control_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Error Recovery Control Supported", m_sSCTCap.sct_Error_Recovery_Control_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Write Same Supported", m_sSCTCap.sct_Write_Same_Supported);
        opensea_parser::set_Json_Bool(data, "Sct Supported", m_sSCTCap.sct_Supported);
        json_push_back(sct, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Depop_Capabilities()
//
//! \brief
//!   Description: get the depop capabilities
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Depop_Capabilities(JSONNODE *depop)
{
#if defined _DEBUG
    printf("Depopulation Capabilities \n");
    printf("\tDepopulation Capabilities =   0x%016" PRIx64"  \n", m_pCap->depopCapabilities);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->depopCapabilities;
    json_push_back(depop, json_new_a("Depopulation Capabilities", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->depopCapabilities))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Depopulation Capabilities");
        if (m_pCap->depopCapabilities & BIT0)
        {
            opensea_parser::set_Json_Bool(data, "Remove Element And Truncate Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Remove Element And Truncate Supported", false);
        }
        if (m_pCap->depopCapabilities & BIT1)
        {
            opensea_parser::set_Json_Bool(data, "Get Physical Element Status Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(data, "Get Physical Element Status Supported", false);
        }
        json_push_back(depop, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Depopulation_Execution_Time()
//
//! \brief
//!   Description:  get the depop execution time
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_03::get_Depopulation_Execution_Time(JSONNODE *depop)
{
#if defined _DEBUG
    printf("Depopulation Execution Times \n");
    printf("\tDepopulation Execution Time =   0x%016" PRIx64"  \n", m_pCap->depopExTime);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->depopExTime;
    json_push_back(depop, json_new_a("Depopulation Execution Time", temp.str().c_str()));
    if (check_For_Active_Status(&m_pCap->depopExTime))
    {
        JSONNODE *data = json_new(JSON_NODE);
        json_set_name(data, "Depopulation Execution Time");
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << (m_pCap->depopExTime & UINT64_C(0x00FFFFFFFFFFFFFF));
        json_push_back(depop, json_new_a("Depopulation Time", temp.str().c_str()));
        json_push_back(depop, data);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page03()
//
//! \brief
//!   Description:  parse Log page 03
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_03::get_Log_Page03(uint8_t *lp3pData, JSONNODE *masterData)
{
#define LOG_PAGE_03   0x0003
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage03 logPage03;
    m_pCap = &logPage03;
    m_pCap = reinterpret_cast<sLogPage03 *>(&lp3pData[0]);
    uint16_t pageNumber = M_Word1(m_pCap->header);
    uint16_t revision = M_Word0(m_pCap->header);
    if (pageNumber == LOG_PAGE_03)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 03h");
#if defined _DEBUG
        printf("Log Page 03h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", m_pCap->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pCap->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));
        get_Supported_Capabilities(pageInfo);
        get_DownLoad_MicroCode(pageInfo);
        get_Media_Rotation_Rate(pageInfo);
        get_Form_Factor(pageInfo);
        get_Write_Read_Verify_Mode3(pageInfo);
        get_Write_Read_Verify_Mode2(pageInfo);
        world_Wide_Name(pageInfo);
        get_Data_Set_Managment(pageInfo);
        get_Utilization_Unit_Time(pageInfo);
        get_Utilization_Usage_Rate_Support(pageInfo);
        get_Zoned_Capabilities(pageInfo);
        get_Supported_ZAC_Capabilities(pageInfo);
        get_Background_Operations_Capabilities(pageInfo);
        get_Background_Operations_Recommendations(pageInfo);
        get_Queue_Depth(pageInfo);
        get_SCT_Capabilities(pageInfo);
        get_Depop_Capabilities(pageInfo);
        get_Depopulation_Execution_Time(pageInfo);

        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}
// *******************************************************************************

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_04()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 04
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_04::CAta_Identify_Log_04(uint8_t *Buffer)
    : m_name("Log Page 04")
    , pData()
    , m_status(eReturnValues::IN_PROGRESS)
    , pLog()
    , m_CS()
    , m_FS()
{
    pData = Buffer;
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_04()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_04
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_04::~CAta_Identify_Log_04()
{
}

//-----------------------------------------------------------------------------
//
//! \fn set_Current_Settings()
//
//! \brief
//!   Description:  checks for active status and then checks all the bits and sets the sturcture TRUE OR FALSE
//
//  Entry:
//! \param ptr  pointer to the data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::set_Current_Settings(uint64_t *value)
{
    if (check_For_Active_Status(value))
    {
        if (*value & BIT0)
        {
            m_CS.wrvEnabled = true;
        }
        if (*value & BIT1)
        {
            m_CS.freeFallEnabled = true;
        }
        if (*value & BIT2)
        {
            m_CS.apmEnabled = true;
        }
        if (*value & BIT3)
        {
            m_CS.puisEnabled = true;
        }
        // bit 4 is a reserved
        // bit 5 is a reserved
        if (*value & BIT6)
        {
            m_CS.smartEnabled = true;
        }
        if (*value & BIT7)
        {
            m_CS.readLookAheadEnabled = true;
        }
        if (*value & BIT8)
        {
            m_CS.nonVolatilWriteCache = true;
        }
        // bit 10 is a reserved
        if (*value & BIT10)
        {
            m_CS.senseDataEnabled = true;
        }
        if (*value & BIT11)
        {
            m_CS.revertingToDefaultsEnabled = true;
        }
        // bit 12 is a reserved
        if (*value & BIT13)
        {
            m_CS.volatileWriteCacheEnabled = true;
        }
        // bit 14 is a reserved
        if (*value & BIT15)
        {
            m_CS.epcEnabled = true;
        }
        if (*value & BIT16)
        {
            m_CS.dsnEnabled = true;
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Current_Settings()
//
//! \brief
//!   Description:  parse and Formats the current  settings of page 04h and set the sFeatrueSettings m_FS;
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Current_Settings(JSONNODE *currentData)
{
#if defined _DEBUG
    printf("Current Settings \n");
    printf("\tCurrent Settings =   0x%016" PRId64"  \n", pLog->currentSettings);
#endif

    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->currentSettings;
    json_push_back(currentData, json_new_a("Current Settings", temp.str().c_str()));
    if (set_Current_Settings(&pLog->currentSettings))
    {
        JSONNODE *currentSettings = json_new(JSON_NODE);
        json_set_name(currentSettings, "Current bit Settings");

        opensea_parser::set_Json_Bool(currentSettings, "WRV Enabled", m_CS.wrvEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Free - Fall Enabled", m_CS.freeFallEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "APM Enabled", m_CS.apmEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "PUIS Enabled", m_CS.puisEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "SMART Enabled", m_CS.smartEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Read Look Ahead Enabled", m_CS.readLookAheadEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "NON Volatile Write Cache Enabled", m_CS.nonVolatilWriteCache);
        opensea_parser::set_Json_Bool(currentSettings, "Sense Data Enabled", m_CS.senseDataEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Reverting To Defaults Enabled", m_CS.revertingToDefaultsEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Volatile Write Cache Enabled", m_CS.volatileWriteCacheEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "EPC Enabled", m_CS.epcEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "DSN Enabled", m_CS.dsnEnabled);
        json_push_back(currentData, currentSettings);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn set_Feature_Settings()
//
//! \brief
//!   Description:  parse the feature settings of page 04h and set the sFeatrueSettings m_FS;
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::set_Feature_Settings(uint64_t *value)
{
    if (check_For_Active_Status(value))
    {
        m_FS.wrvMode = (M_Byte0(*value));
        m_FS.apmLevel = (M_Byte1(*value));
        m_FS.powerScource = (M_Byte2(*value));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Feature_Settings()
//
//! \brief
//!   Description:  parse and Formats the feature settings of page 04h and set the sFeatrueSettings m_FS;
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Feature_Settings(JSONNODE *featureData)
{
#if defined _DEBUG
    printf("Feature Settings \n");
    printf("\tFeature Settings =   0x%016" PRIx64"  \n", pLog->featureSettings);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->featureSettings;
    json_push_back(featureData, json_new_a("Feature Settings", temp.str().c_str()));

    if (set_Feature_Settings(&pLog->featureSettings))
    {
        JSONNODE *featSettings = json_new(JSON_NODE);
        json_set_name(featSettings, "Feature Settings breakdown");

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_FS.wrvMode;
        json_push_back(featSettings, json_new_a("Write Read Verify", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_FS.apmLevel;
        json_push_back(featSettings, json_new_a("APM Level", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_FS.powerScource;
        if (m_FS.powerScource & BIT1)
        {
            json_push_back(featSettings, json_new_a("Power Source Battery", temp.str().c_str()));
        }
        else if (m_FS.powerScource & BIT2)
        {
            json_push_back(featSettings, json_new_a("Power Source NOT Battery", temp.str().c_str()));
        }
        else if (m_FS.powerScource & BIT1 && m_FS.powerScource & BIT2)
        {
            json_push_back(featSettings, json_new_a("Power Source Reserved", temp.str().c_str()));
        }
        else
        {
            json_push_back(featSettings, json_new_a("Power Source Unknown", temp.str().c_str()));
        }

        json_push_back(featureData, featSettings);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_DMS_Times()
//
//! \brief
//!   Description:  parse and Formats the feature settings of page 04h and set the sFeatrueSettings m_FS;
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_DMS_Times(JSONNODE *dmaData)
{
#if defined _DEBUG
    printf("DMA Host Interface Sector Times \n");
    printf("\tDMA Host Interface Sector Times =   0x%016" PRIx64"  \n", pLog->DMAHostInterfaceSectorTimes);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->DMAHostInterfaceSectorTimes;
    json_push_back(dmaData, json_new_a("DMA Host Interface Sector Times", temp.str().c_str()));
    if (set_Feature_Settings(&pLog->DMAHostInterfaceSectorTimes))
    {
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word0(pLog->DMAHostInterfaceSectorTimes);
        json_push_back(dmaData, json_new_a("DMA Sector Time ", temp.str().c_str()));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_PIO_Times()
//
//! \brief
//!   Description:  parse and Formats the PIO settings;
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_PIO_Times(JSONNODE *pioData)
{
#if defined _DEBUG
    printf("PIO Host Interface Sector Times \n");
    printf("\tPIO Host Interface Sector Times =   0x%016" PRIx64"  \n", pLog->PioHostInterfaceSectorTImes);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->PioHostInterfaceSectorTImes;
    json_push_back(pioData, json_new_a("PIO Host Interface Sector Times", temp.str().c_str()));
    if (set_Feature_Settings(&pLog->PioHostInterfaceSectorTImes))
    {
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word0(pLog->PioHostInterfaceSectorTImes);
        json_push_back(pioData, json_new_a("PIO Sector Time ", temp.str().c_str()));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Streaming_Min_Request_Times()
//
//! \brief
//!   Description:  parse and Formats the Streaming Minimum request size
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Streaming_Min_Request_Times(JSONNODE *streamMinData)
{
#if defined _DEBUG
    printf("Streaming Minimum request Times \n");
    printf("\tStreaming Minimum request Times =   0x%016" PRIx64"  \n", pLog->streamingMinRequestSize);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->streamingMinRequestSize;
    json_push_back(streamMinData, json_new_a("Streaming Minimum request Times", temp.str().c_str()));
    if (set_Feature_Settings(&pLog->streamingMinRequestSize))
    {
        temp.str("");
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word0(pLog->streamingMinRequestSize);
        json_push_back(streamMinData, json_new_a("Steaming Min Request Size", temp.str().c_str()));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Streaming_Access_Latency()
//
//! \brief
//!   Description:  parse and Formats the Streaming Access Latency
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Streaming_Access_Latency(JSONNODE *accessData)
{
#if defined _DEBUG
    printf("Streaming Access Latency \n");
    printf("\tStreaming Access Latency =   0x%016" PRIx64"  \n", pLog->streamingAccessLatency);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->streamingAccessLatency;
    json_push_back(accessData, json_new_a("Streaming Access Latency", temp.str().c_str()));
    if (set_Feature_Settings(&pLog->streamingAccessLatency))
    {
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word0(pLog->streamingAccessLatency);
        json_push_back(accessData, json_new_a("Steaming Access Latency data", temp.str().c_str()));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Streaming_Performance_Granularity()
//
//! \brief
//!   Description:  parse and Formats the Streaming Performance Granularity
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Streaming_Performance_Granularity(JSONNODE *performanceData)
{
#if defined _DEBUG
    printf("Streaming Performance Granularity \n");
    printf("\tStreaming Performance Granularity =   0x%016" PRIx64"  \n", pLog->streamingPerformanceGranularity);
#endif

    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->streamingPerformanceGranularity;
    json_push_back(performanceData, json_new_a("Streaming Performance Granularity", temp.str().c_str()));

    if (set_Feature_Settings(&pLog->streamingPerformanceGranularity))
    {
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << M_DoubleWord0(pLog->streamingPerformanceGranularity);
        json_push_back(performanceData, json_new_a("Steaming Granularity", temp.str().c_str()));
    }
    else
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
//
//! \fn get_Free_Fall_Control()
//
//! \brief
//!   Description:  parse and Formats the Free Fall control sensitivity 
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Free_Fall_Control(JSONNODE *freeFallData)
{
#if defined _DEBUG
    printf("Free Fall control sensitivity  \n");
    printf("\tFree Fall control sensitivity  =   0x%016" PRIx64"  \n", pLog->freeFallControlSensitivity);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->freeFallControlSensitivity;
    json_push_back(freeFallData, json_new_a("Free Fall control sensitivity ", temp.str().c_str()));
    if (set_Feature_Settings(&pLog->freeFallControlSensitivity))
    {
        temp.str("");
        temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(M_Byte0(pLog->freeFallControlSensitivity));
        json_push_back(freeFallData, json_new_a("Free Fall control sensitivity settings", temp.str().c_str()));
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Device_Maintenance_Schedule()
//
//! \brief
//!   Description:  parse and Formats the Device Maintenance Schedule
//
//  Entry:
//! \param JSON ptr  pointer to the json data for formating
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_04::get_Device_Maintenance_Schedule(JSONNODE *maintenaceData)
{
    std::string myStr;
#if defined _DEBUG
    printf("Device Maintenance Schedule \n");
    printf("\tDevice Maintenance Schedule =   0x%016" PRIx64"  \n", pLog->deviceMaintenaceSchedule);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->deviceMaintenaceSchedule;
    json_push_back(maintenaceData, json_new_a("Device Maintenace Schedule", temp.str().c_str()));
    if (set_Feature_Settings(&pLog->deviceMaintenaceSchedule))
    {
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word0(pLog->deviceMaintenaceSchedule);
        json_push_back(maintenaceData, json_new_a("Minimum Inactive Time", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word1(pLog->deviceMaintenaceSchedule);
        json_push_back(maintenaceData, json_new_a("Time to Preformance Degradation", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_Word2(pLog->deviceMaintenaceSchedule);
        json_push_back(maintenaceData, json_new_a("Time Scheduled for Device Maintenance", temp.str().c_str()));

        JSONNODE *deviceMaintenace = json_new(JSON_NODE);
        json_set_name(deviceMaintenace, "Device Maintenace Meaning");
        uint16_t mcWord = M_Word0(pLog->deviceMaintenaceSchedule);
        if (mcWord == 0x0000)
        {
            myStr = "0%";
        }
        else if (mcWord == 0x0001)
        {
            myStr = "0% to 10%";
        }
        else if (mcWord == 0x0002)
        {
            myStr = "10% to 20%";
        }
        else if (mcWord == 0x0003)
        {
            myStr = "20% to 30%";
        }
        else if (mcWord == 0x0004)
        {
            myStr = "30% to 40%";
        }
        else if (mcWord == 0x0005)
        {
            myStr = "40% to 50%";
        }
        else if (mcWord == 0x0006)
        {
            myStr = "50% to 60%";
        }
        else if (mcWord == 0x0007)
        {
            myStr = "60% to 70%";
        }
        else if (mcWord == 0x0008)
        {
            myStr = "70% to 80%";
        }
        else if (mcWord == 0x0009)
        {
            myStr = "80% to 90%";
        }
        else if (mcWord == 0x0010)
        {
            myStr = "90% to 100%";
        }
        else
        {
            myStr = "unknown";
        }
        json_push_back(deviceMaintenace, json_new_a("MC Level", myStr.c_str()));

        json_push_back(maintenaceData, deviceMaintenace);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page04()
//
//! \brief
//!   Description:  parse Log page 04
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_04::get_Log_Page04(uint8_t *lp4pData, JSONNODE *masterData)
{
#define LOG_PAGE_04   0x0004
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage04 logPage;
    pLog = &logPage;
    memset(pLog, 0, sizeof(sLogPage04));
    pLog = reinterpret_cast<sLogPage04 *>(&lp4pData[0]);
    uint16_t pageNumber = M_Word1(pLog->header);
    uint16_t revision = M_Word0(pLog->header);
    if (pageNumber == LOG_PAGE_04)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 04h");
#if defined _DEBUG
        printf("Log Page 04h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", pLog->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << pLog->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));

        get_Current_Settings(pageInfo);
        get_Feature_Settings(pageInfo);
        get_DMS_Times(pageInfo);
        get_PIO_Times(pageInfo);
        get_Streaming_Min_Request_Times(pageInfo);
        get_Streaming_Access_Latency(pageInfo);
        get_Streaming_Performance_Granularity(pageInfo);
        get_Free_Fall_Control(pageInfo);
        get_Device_Maintenance_Schedule(pageInfo);

        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}

// *******************************************************************************

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_05()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 05
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_05::CAta_Identify_Log_05(uint8_t *Buffer)
    : m_name("ATA Identify Log Page 05")
    , m_status(eReturnValues::IN_PROGRESS)
    , m_pLog()
    , m_pPrintable()
{
    pData = Buffer;
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_05()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_05
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_05::~CAta_Identify_Log_05()
{
}

//-----------------------------------------------------------------------------
//
//! \fn create_Serial_Number()
//
//! \brief
//!   Description:  takes the two uint64 bit seiral number values and create a string serial number
//
//  Entry:
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------

bool CAta_Identify_Log_05::create_Serial_Number()
{
    if (m_pLog->serialNumber != M_NULLPTR)
    {
        m_pPrintable->serialStr.assign(reinterpret_cast<const char*>(&m_pLog->serialNumber), LOG5_SERIAL_NUMBER);
        byte_swap_std_string(m_pPrintable->serialStr);
        m_pPrintable->serialStr.resize(LOG5_SERIAL_NUMBER);
        ltrim(m_pPrintable->serialStr);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn create_Firmware_Rev()
//
//! \brief
//!   Description:  takes the two uint64 bi model number values and create a string 
//
//  Exit:
//!   \return bool
//
//  Exit:
//
//---------------------------------------------------------------------------

bool CAta_Identify_Log_05::create_Firmware_Rev()
{
    if (m_pLog->firmwareRev != M_NULLPTR)
    {
        m_pPrintable->firmwareStr.assign(reinterpret_cast<const char*>(&m_pLog->firmwareRev), LOG5_FIRMWARE_REV);
        byte_swap_std_string(m_pPrintable->firmwareStr);
        m_pPrintable->firmwareStr.resize(LOG5_FIRMWARE_REV);
        ltrim(m_pPrintable->firmwareStr);
        rtrim(m_pPrintable->firmwareStr);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn create_Model_Number()
//
//! \brief
//!   Description:  takes the two uint64 bit model number values and create a string 
//
//  Entry:
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------

bool CAta_Identify_Log_05::create_Model_Number()
{
    if (m_pLog->modelNumber != M_NULLPTR)
    {
        m_pPrintable->modelNumberStr.assign(reinterpret_cast<const char*>(&m_pLog->modelNumber), LOG5_MODEL_NUMBER);
        byte_swap_std_string(m_pPrintable->modelNumberStr);
        m_pPrintable->modelNumberStr.resize(LOG5_MODEL_NUMBER);
        ltrim(m_pPrintable->modelNumberStr);
        rtrim(m_pPrintable->modelNumberStr);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn create_Product_sting()
//
//! \brief
//!   Description:  takes the two uint64 bit product string and create a string 
//
//  Exit:
//!   \return bool
//
//  Exit:
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_05::create_Product_string()
{
    if (m_pLog->productInformation != M_NULLPTR)
    {
        m_pPrintable->productStr.assign(reinterpret_cast<const char*>(&m_pLog->productInformation), LOG5_PRODUCT_INFO);
        byte_swap_std_string(m_pPrintable->productStr);
        m_pPrintable->productStr.resize(LOG5_PRODUCT_INFO);
        ltrim(m_pPrintable->productStr);
        rtrim(m_pPrintable->productStr);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_printables()
//
//! \brief
//!   Description:  takes the two uint64 bit product string and create a string 
//
//  Exit:
//!   \return bool
//
//  Exit:
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_05::get_printables(JSONNODE *pageInfo)
{
#if defined _DEBUG
    printf("Device String Page \n");

#endif
    if (m_pLog != M_NULLPTR)
    {
        if (!create_Serial_Number())
        {
            return false;
        }
        if (!create_Model_Number())
        {
            return false;
        }
        if (!create_Firmware_Rev())
        {
            return false;
        }
        if (!create_Product_string())
        {
            return false;
        }
        JSONNODE *deviceInfo = json_new(JSON_NODE);
        json_set_name(deviceInfo, "Device String Page");

        if (m_pPrintable->serialStr != "")
        {
            json_push_back(deviceInfo, json_new_a("Serial Number", m_pPrintable->serialStr.c_str()));
        }
        if (m_pPrintable->firmwareStr != "")
        {
            json_push_back(deviceInfo, json_new_a("Firmware Revision", m_pPrintable->firmwareStr.c_str()));
        }
        if (m_pPrintable->modelNumberStr != "")
        {
            json_push_back(deviceInfo, json_new_a("Model Number", m_pPrintable->modelNumberStr.c_str()));
        }
        if (m_pPrintable->productStr != "")
        {
            json_push_back(deviceInfo, json_new_a("Product Identifier", m_pPrintable->productStr.c_str()));
        }

        json_push_back(pageInfo, deviceInfo);
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page05()
//
//! \brief
//!   Description:  parse Log page 05
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_05::get_Log_Page05(uint8_t *lp5pData, JSONNODE *masterData)
{
#define LOG_PAGE_05   0x0005
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage05 logPage;
    m_pLog = &logPage;
    memset(m_pLog, 0, sizeof(sLogPage05));
    m_pLog = reinterpret_cast<sLogPage05*>(&lp5pData[0]);
    sPrintablePage05 printLog;
    m_pPrintable = &printLog;
    uint16_t pageNumber = M_Word1(m_pLog->header);
    uint16_t revision = M_Word0(m_pLog->header);
    if (pageNumber == LOG_PAGE_05)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 05h");
#if defined _DEBUG
        printf("Log Page 05h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", m_pLog->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));

        get_printables(pageInfo);


        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}


// *******************************************************************************

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_06()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 06
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_06::CAta_Identify_Log_06(uint8_t *Buffer)
    : m_name("ATA Identify Log Page 06")
    , m_status(eReturnValues::IN_PROGRESS)
    , m_pLog()
    , m_sSCapabilities()
    , m_sSInformation()
{
    pData = Buffer;
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_06()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_06
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_06::~CAta_Identify_Log_06()
{
}
//-----------------------------------------------------------------------------
//
//! \fn set_Security_Information()
//
//! \brief
//!   Description: get the value of the 64 bit and check for active status and then parse out the 
//!   supported bits for the security information
//
//  Entry:
//! \param value - the 64 bit value
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::set_Security_Settings(uint64_t *value)
{
    if (check_For_Active_Status(value))
    {
        if (*value  & BIT6) //bit 6
        {
            m_sSInformation.supported = true;
        }
        if (*value  & BIT5) //bit 5
        {
            m_sSInformation.masterPasswordCapability = true;
        }
        if (*value  & BIT4) //bit 4
        {
            m_sSInformation.enhancedSecurityEraseSupported = true;
        }
        if (*value & BIT3) //bit 3
        {
            m_sSInformation.securityCountExpired = true;
        }
        if (*value & BIT2) //bit 2
        {
            m_sSInformation.frozen = true;
        }
        if (*value  & BIT1) //bit 1
        {
            m_sSInformation.locked = true;
        }
        if (*value  & BIT0) //bit 0
        {
            m_sSInformation.enabled = true;
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Security_Settings()
//
//! \brief
//!   Description: get the value of the 64 bit and call set security informaiton the print out the data
//
//  Entry:
//! \param value - the 64 bit value
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::get_Security_Settings(JSONNODE *si)
{
#if defined _DEBUG
    printf("Security Settings \n");
    printf("\tSecurity Settings =   0x%016" PRIx64"  \n", m_pLog->securitySetting);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->securitySetting;
    json_push_back(si, json_new_a("Security Settings", temp.str().c_str()));

    if (set_Security_Settings(&m_pLog->securitySetting))
    {
        JSONNODE *securityBits = json_new(JSON_NODE);
        json_set_name(securityBits, "Security Bits Settings");

        opensea_parser::set_Json_Bool(securityBits, "Security Supported", m_sSInformation.supported);

        json_push_back(securityBits, json_new_i("Master Password Capability", m_sSInformation.masterPasswordCapability));

        opensea_parser::set_Json_Bool(securityBits, "Enhanced Security Erase Supported", m_sSInformation.enhancedSecurityEraseSupported);

        opensea_parser::set_Json_Bool(securityBits, "Security Count Expired", m_sSInformation.securityCountExpired);

        opensea_parser::set_Json_Bool(securityBits, "Security Frozen", m_sSInformation.frozen);

        opensea_parser::set_Json_Bool(securityBits, "Security Locked", m_sSInformation.locked);

        opensea_parser::set_Json_Bool(securityBits, "Security Enabled", m_sSInformation.enabled);
        json_push_back(si, securityBits);
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Master_Password_Identifier()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the master password identifier
//
//  Entry:
//! \param value - the 64 bit value
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::get_Master_Password_Identifier(JSONNODE *mpIdent)
{
#if defined _DEBUG
    printf("Master Password Identifier \n");
    printf("\tMaster Password Identifier =   0x%016" PRIx64"  \n", m_pLog->masterPassword);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->masterPassword;
    json_push_back(mpIdent, json_new_a("Master Password Identifier", temp.str().c_str()));
    if (check_For_Active_Status(&m_pLog->masterPassword))
    {
        uint16_t master = M_Word0(m_pLog->masterPassword);
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(master);
        json_push_back(mpIdent, json_new_a("Master Password Identifier field", temp.str().c_str()));
        if (master & BIT0)
        {
            opensea_parser::set_Json_Bool(mpIdent, "Master Password Capability is Maximum", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(mpIdent, "Master Password Capability is High", true);
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Time_for_Enhanced_Erase()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the time to do an enhanced erase
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::get_Time_for_Enhanced_Erase(JSONNODE *enhanced)
{
#if defined _DEBUG
    printf("Time for Enhanced Erase \n");
    printf("\tTime for Enhanced Erase =   0x%016" PRIx64"  \n", m_pLog->timeEnhancedErase);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->timeEnhancedErase;
    json_push_back(enhanced, json_new_a("Time for Enhanced Erase", temp.str().c_str()));
    if (check_For_Active_Status(&m_pLog->timeEnhancedErase))
    {
        if (m_pLog->timeEnhancedErase & BIT15)
        {
            opensea_parser::set_Json_Bool(enhanced, "Enhanced Security Erased Time format bit", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(enhanced, "Enhanced Security Erased Time format bit", false);
        }
        temp.str("");temp.clear();
        temp << std::dec << (static_cast<uint16_t>(M_Word0(m_pLog->timeEnhancedErase)) & 0x0FFF) * 2;
        json_push_back(enhanced, json_new_a("Time for Enhanced Erase in Minutes", temp.str().c_str()));
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Time_for_Normal_Erase()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the time to do an normal erase
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::get_Time_for_Normal_Erase(JSONNODE *normal)
{
#if defined _DEBUG
    printf("Time for Normal Erase \n");
    printf("\tTime for Normal Erase =   0x%016" PRIx64"  \n", m_pLog->timeNormalErase);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->timeNormalErase;
    json_push_back(normal, json_new_a("Time for Normal Erase", temp.str().c_str()));
    if (check_For_Active_Status(&m_pLog->timeNormalErase))
    {
        if (m_pLog->timeNormalErase & BIT15)
        {
            opensea_parser::set_Json_Bool(normal, "Normal Security Time format bit", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(normal, "Normal Security Time format bit", false);
        }
        temp.str("");temp.clear();
        temp << std::dec << (static_cast<uint16_t>(M_Word0(m_pLog->timeNormalErase)) & 0x0FFF) * 2;
        json_push_back(normal, json_new_a("Time for Normal Erase in Minutes", temp.str().c_str()));
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Trusted_Computing_Feature_Set()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the Trusted Computing Features
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::get_Trusted_Computing_Feature_Set(JSONNODE *tdc)
{
#if defined _DEBUG
    printf("Trusted Computing Feature Set \n");
    printf("\tTrusted Computing Feature Set =   0x%016" PRIx64"  \n", m_pLog->tcfeatures);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->tcfeatures;
    json_push_back(tdc, json_new_a("Trusted Computing Feature Set", temp.str().c_str()));
    if (check_For_Active_Status(&m_pLog->tcfeatures))
    {
        if (m_pLog->tcfeatures & BIT0)
        {
            opensea_parser::set_Json_Bool(tdc, "Trusted Computing Supported", true);
        }
        else
        {
            opensea_parser::set_Json_Bool(tdc, "Trusted Computing Supported", false);
        }

    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn set_Security_Capabilities()
//
//! \brief
//!   Description: set the bool settings based on if the bits are set or not
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::set_Security_Capabilities()
{
    if (check_For_Active_Status(&m_pLog->tcfeatures))
    {
        if (m_pLog->securitySetting  & BIT6) //bit 6
        {
            m_sSCapabilities.sanitize = true;                                       //<! ACS - 3 COMMANDS ALLOWED BY SANITIZE                           bit 6
        }
        if (m_pLog->securitySetting  & BIT5) //bit 5
        {
            m_sSCapabilities.antifreeze = true;                                      //<! SANITIZE ANTIFREEZE LOCK SUPPORTED                             bit 5
        }
        if (m_pLog->securitySetting  & BIT4) //bit 4
        {
            m_sSCapabilities.eraseSupported = true;                                  //<! BLOCK ERASE SUPPORTED                                          bit 4
        }
        if (m_pLog->securitySetting  & BIT3) //bit 3
        {
            m_sSCapabilities.overwriteSupported = true;                             //<! OVERWRITE SUPPORTED                                            bit 3
        }
        if (m_pLog->securitySetting  & BIT2) //bit 2
        {
            m_sSCapabilities.cryptoScrambleSupported = true;                         //<! CRYPTO SCRAMBLE SUPPORTED                                      bit 2
        }
        if (m_pLog->securitySetting  & BIT1) //bit 1
        {
            m_sSCapabilities.sanitizeSupported = true;                               //<! SANITIZE SUPPORTED                                             bit 1
        }
        if (m_pLog->securitySetting  & BIT0) //bit 0
        {
            m_sSCapabilities.encryptSupported = true;                                //<! ENCRYPT ALL SUPPORTED                                          bit 0
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Security_Capabilities()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the Security Capabilities
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_06::get_Security_Capabilities(JSONNODE *sCap)
{
#if defined _DEBUG
    printf("Trusted Computing Feature Set \n");
    printf("\tTrusted Computing Feature Set =   0x%016" PRIx64"  \n", m_pLog->tcfeatures);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->tcfeatures;
    json_push_back(sCap, json_new_a("Trusted Computing Feature Set", temp.str().c_str()));
    if (set_Security_Capabilities())
    {
        JSONNODE *securityBits = json_new(JSON_NODE);
        json_set_name(securityBits, "Security Capabilities bits");
        opensea_parser::set_Json_Bool(securityBits, "ACS-3 Commands Allowed By Sanitize Bit", m_sSCapabilities.sanitize);
        opensea_parser::set_Json_Bool(securityBits, "Sanitize Antifreeze Lock Supported Bit", m_sSCapabilities.antifreeze);
        opensea_parser::set_Json_Bool(securityBits, "Block Erase Supported Bit", m_sSCapabilities.eraseSupported);
        opensea_parser::set_Json_Bool(securityBits, "Overwrite Supported Bit", m_sSCapabilities.overwriteSupported);
        opensea_parser::set_Json_Bool(securityBits, "Crypto Scramble Supported Bit", m_sSCapabilities.cryptoScrambleSupported);
        opensea_parser::set_Json_Bool(securityBits, "Sanitize Supported Bit", m_sSCapabilities.sanitizeSupported);
        opensea_parser::set_Json_Bool(securityBits, "Encrypt All Supported Bit", m_sSCapabilities.eraseSupported);
        json_push_back(sCap, securityBits);
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page06()
//
//! \brief
//!   Description:  parse Log page 06
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_06::get_Log_Page06(uint8_t *lp6pData, JSONNODE *masterData)
{
#define LOG_PAGE_06   0x0006
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage06 logPage;
    m_pLog = &logPage;
    memset(m_pLog, 0, sizeof(sLogPage06));
    m_pLog = reinterpret_cast<sLogPage06 *>(&lp6pData[0]);

    uint16_t pageNumber = M_Word1(m_pLog->header);
    uint16_t revision = M_Word0(m_pLog->header);
    if (pageNumber == LOG_PAGE_06)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 06h");
#if defined _DEBUG
        printf("Log Page 06h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", m_pLog->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));
        get_Master_Password_Identifier(pageInfo);
        get_Security_Settings(pageInfo);
        get_Time_for_Enhanced_Erase(pageInfo);
        get_Time_for_Normal_Erase(pageInfo);
        get_Trusted_Computing_Feature_Set(pageInfo);
        get_Security_Capabilities(pageInfo);
        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}

// *******************************************************************************

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_07()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 07
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_07::CAta_Identify_Log_07(uint8_t *Buffer)
    : m_name("ATA Identify Log Page 07")
    , m_status(eReturnValues::IN_PROGRESS)
    , m_pLog()
    , m_ATACap()
    , m_hardwareRR()
{
    pData = Buffer;
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_07()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_07
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_07::~CAta_Identify_Log_07()
{
}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page07()
//
//! \brief
//!   Description:  parse Log page 07
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_07::get_Log_Page07(uint8_t *lp7pData, JSONNODE *masterData)
{
#define LOG_PAGE_07   0x0007
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage07 logPage;
    m_pLog = &logPage;
    memset(m_pLog, 0, sizeof(sLogPage07));
    m_pLog = reinterpret_cast<sLogPage07 *>(&lp7pData[0]);

    uint16_t pageNumber = M_Word1(m_pLog->header);
    uint16_t revision = M_Word0(m_pLog->header);
    if (pageNumber == LOG_PAGE_07)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 07h");
#if defined _DEBUG
        printf("Log Page 07h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", m_pLog->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));


        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}

// *******************************************************************************

//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_08()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log page 08
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_08::CAta_Identify_Log_08(uint8_t *Buffer)
    :m_name("ATA Identify Log Page 08")
    , m_status(eReturnValues::IN_PROGRESS)
    , m_pLog(M_NULLPTR)
    , m_SATACap()
    , m_CurrentSet()
{

    pData = Buffer;
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CAta_Identify_Log_08()
//
//! \brief
//!   Description:  Class deconstructor for for the CAta_Identify_Log_08
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_08::~CAta_Identify_Log_08()
{
}
//-----------------------------------------------------------------------------
//
//! \fn set_sata_Capabilities()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the SATA Capablities. 
//!   preset to false. if bit is set then it will now set it to true.
//
//  Entry:
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_08::set_Sata_Capabilities()
{
    if (check_For_Active_Status(&m_pLog->sataCapabilities))
    {
        if (m_pLog->sataCapabilities  & BIT31) //bit 31
        {
            m_SATACap.powerDisableFeatureAlwaysEnabled = true;
        }
        if (m_pLog->sataCapabilities  & BIT30) //bit 30
        {
            m_SATACap.powerDisableFeatureSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT29) //bit 29
        {
            m_SATACap.rebuildAssistSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT28) //bit 28
        {
            m_SATACap.supportedDIPM_SSP = true;
        }
        if (m_pLog->sataCapabilities  & BIT27) //bit 27
        {
            m_SATACap.hybridInformationSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT26) //bit 26
        {
            m_SATACap.devSleepToReducedWrstateCapablitySupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT25) //bit 25
        {
            m_SATACap.deviceSleepSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT24) //bit 24
        {
            m_SATACap.ncqAutosenseSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT23) //bit 23
        {
            m_SATACap.softwareSettingsPreservationSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT22) //bit 22
        {
            m_SATACap.hardwareFeatureControlSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT21) //bit 21
        {
            m_SATACap.inOrderDataDeliverySupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT20) //bit 20
        {
            m_SATACap.deviceInitiatedPowerManagementSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT19) //bit 19
        {
            m_SATACap.dmaSetupAutoActivationSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT18) //bit 18
        {
            m_SATACap.nonZeroBufferOffsetsSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT17) //bit 17
        {
            m_SATACap.sendAndReceiveQueuedCommandsSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT16) //bit 16
        {
            m_SATACap.ncqQueueManagementCommandSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT15) //bit 15
        {
            m_SATACap.ncqStreamingSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT14) //bit 14
        {
            m_SATACap.readLogDmaExtAsEquivalentToReadLogExtSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT13) //bit 13
        {
            m_SATACap.deviceAutomaticPartialToSlumberTransitionsSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT12) //bit 12
        {
            m_SATACap.hostAutomaticPartialToSlumberTransitionsSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT11) //bit 11
        {
            m_SATACap.ncqPriorityInformationSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT10) //bit 10
        {
            m_SATACap.unloadWhileNcqCommandsAreOutstandingSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT9) //bit 9
        {
            m_SATACap.sataPhyEventCountersLogSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT8) //bit 8
        {
            m_SATACap.receiptOfHostInitiatedPowerManagementRequestsSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT7) //bit 7
        {
            m_SATACap.ncqFeatureSetSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT2) //bit 2
        {
            m_SATACap.sataGen3SignalingSpeedSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT1) //bit 1
        {
            m_SATACap.sataGen2SignalingSpeedSupported = true;
        }
        if (m_pLog->sataCapabilities  & BIT0) //bit 0
        {
            m_SATACap.sataGen1SignalingSpeedSupported = true;
        }
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_sata_Capabilities()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the SATA Capabilities
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_08::get_Sata_Capabilities(JSONNODE *cap)
{
#if defined _DEBUG
    printf("SATA Capabilities\n");
    printf("\tSATA Capabilities =   0x%016" PRIx64"  \n", m_pLog->sataCapabilities);
#endif

    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->sataCapabilities;
    json_push_back(cap, json_new_a("SATA Capabilities QWord", temp.str().c_str()));

    if (set_Sata_Capabilities())
    {
        JSONNODE *capBitSettings = json_new(JSON_NODE);
        json_set_name(capBitSettings, "SATA Capabilities");

        opensea_parser::set_Json_Bool(capBitSettings, "Power Disable Feature Always Enabled", m_SATACap.powerDisableFeatureAlwaysEnabled);
        opensea_parser::set_Json_Bool(capBitSettings, "Power Disable Feature Supported", m_SATACap.powerDisableFeatureSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Rebuild Assist Supported", m_SATACap.rebuildAssistSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Dipm Ssp Preservation Supported", m_SATACap.supportedDIPM_SSP);
        opensea_parser::set_Json_Bool(capBitSettings, "Hybrid Information Supported", m_SATACap.hybridInformationSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Devsleep To Reducedpwrstate Capability Supported", m_SATACap.devSleepToReducedWrstateCapablitySupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Device Sleep Supported", m_SATACap.deviceSleepSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "NCQ Autosense Supported", m_SATACap.ncqAutosenseSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Software Settings Preservation Supported", m_SATACap.softwareSettingsPreservationSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Hardware Feature Control Supported", m_SATACap.hardwareFeatureControlSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "In-Order Data Delivery Supported", m_SATACap.inOrderDataDeliverySupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Device Initiated Power Management Supported", m_SATACap.deviceInitiatedPowerManagementSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "DMA Setup Auto-Activation Supported", m_SATACap.dmaSetupAutoActivationSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Non-Zero Buffer Offsets Supported", m_SATACap.nonZeroBufferOffsetsSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Send And Receive Queued Commands Supported", m_SATACap.sendAndReceiveQueuedCommandsSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "NCQ Queue Management Command Supported", m_SATACap.ncqQueueManagementCommandSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "NCQ Streaming Supported", m_SATACap.ncqStreamingSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Read Log DMA Ext As Equivalent To Read Log Ext Supported", m_SATACap.readLogDmaExtAsEquivalentToReadLogExtSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Device Automatic Partial To Slumber Transitions Supported", m_SATACap.deviceAutomaticPartialToSlumberTransitionsSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Host Automatic Partial To Slumber Transitions Supported", m_SATACap.hostAutomaticPartialToSlumberTransitionsSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "NCQ Priority Information Supported", m_SATACap.ncqPriorityInformationSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Unload While NCQ Commands Are Outstanding Supported", m_SATACap.unloadWhileNcqCommandsAreOutstandingSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "SATA Phy Event Counters Log Supported", m_SATACap.sataPhyEventCountersLogSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "Receipt Of Host Initiated Power Management Requests Supported", m_SATACap.receiptOfHostInitiatedPowerManagementRequestsSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "NCQ Feature Set Supported", m_SATACap.ncqFeatureSetSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "SATA Gen3 Signaling Speed Supported", m_SATACap.sataGen3SignalingSpeedSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "SATA Gen2 Signaling Speed Supported", m_SATACap.sataGen2SignalingSpeedSupported);
        opensea_parser::set_Json_Bool(capBitSettings, "SATA Gen1 Signaling Speed Supported", m_SATACap.sataGen1SignalingSpeedSupported);


        json_push_back(cap, capBitSettings);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn set_current_Sata()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the Current SATA settings
//
//  Entry:
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_08::set_Current_Sata()
{
    if (check_For_Active_Status(&m_pLog->currentSata))
    {
        if (m_pLog->currentSata  & BIT13) //bit 13
        {
            m_CurrentSet.hybridEnabled = true;
        }
        if (m_pLog->currentSata  & BIT12) //bit 12
        {
            m_CurrentSet.rebuidAssistEnabled = true;
        }
        if (m_pLog->currentSata  & BIT11) //bit 11
        {
            m_CurrentSet.powerDisabledFeatrueEnabled = true;
        }
        if (m_pLog->currentSata  & BIT10) //bit 10
        {
            m_CurrentSet.deviceSleepEnalbed = true;
        }
        if (m_pLog->currentSata  & BIT9) //bit 9
        {
            m_CurrentSet.slumberEnabled = true;
        }
        if (m_pLog->currentSata  & BIT8) //bit 8
        {
            m_CurrentSet.softwareSettingsPreservationEnabled = true;
        }
        if (m_pLog->currentSata  & BIT7) //bit 7
        {
            m_CurrentSet.hardwareFeatureControlEnabled = true;
        }
        if (m_pLog->currentSata  & BIT6) //bit6
        {
            m_CurrentSet.inOrderDataDeliveryEnabled = true;
        }
        if (m_pLog->currentSata  & BIT5) //bit 5
        {
            m_CurrentSet.deviceInitiatedPowerManagementEnabled = true;
        }
        if (m_pLog->currentSata  & BIT4) //bit 4
        {
            m_CurrentSet.dmaSetupEnabled = true;
        }
        if (m_pLog->currentSata  & BIT3) //bit 3
        {
            m_CurrentSet.nonZeroBufferEnabled = true;
        }
        if ((m_pLog->currentSata  & BIT1) && (m_pLog->currentSata  & BIT0)) //bit 0 and bit 1
        {
            m_CurrentSet.currentGen3SignalingSpeed = true;
        }
        if (m_pLog->currentSata  & BIT2) //bit 2
        {
            m_CurrentSet.currentGen2SignalingSpeed = true;
        }
        if (m_pLog->currentSata  & BIT1 && !(m_pLog->currentSata  & BIT0)) //bit 1
        {
            m_CurrentSet.currentGen1SignalingSpeed = true;
        }
        if (m_pLog->currentSata  & BIT0 && !(m_pLog->currentSata  & BIT1)) //bit 0
        {
            m_CurrentSet.currentSignallingNotSupported = true;
        }

    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_current_Sata()
//
//! \brief
//!   Description: get the value of the 64 bit and print out the Current SATA Settings
//
//  Entry:
//! \param masterData - the JSONNODE pointer to all of the json data 
//
//  Exit:
//!   \return bool
//
//---------------------------------------------------------------------------
bool CAta_Identify_Log_08::get_Current_Sata(JSONNODE *current)
{
#if defined _DEBUG
    printf("Current SATA Settings\n");
    printf("\tCurrent SATA Settingss =   0x%016" PRIx64"  \n", m_pLog->currentSata);
#endif
    std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->currentSata;
    json_push_back(current, json_new_a("Current SATA Settings QWord", temp.str().c_str()));
    if (set_Current_Sata())
    {
        JSONNODE *currentSettings = json_new(JSON_NODE);
        json_set_name(currentSettings, "Current SATA Settings");
        opensea_parser::set_Json_Bool(currentSettings, "Hybrid Enabled", m_CurrentSet.hybridEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Rebuid Assist Enabled", m_CurrentSet.rebuidAssistEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Power Disabled Featrue Enabled", m_CurrentSet.powerDisabledFeatrueEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Device Sleep Enalbed", m_CurrentSet.deviceSleepEnalbed);
        opensea_parser::set_Json_Bool(currentSettings, "Automatic Partial To Slumber Transitions Enabled", m_CurrentSet.slumberEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Software Settings Preservation Enabled", m_CurrentSet.softwareSettingsPreservationEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Hardware Feature Control Is Enabled", m_CurrentSet.hardwareFeatureControlEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "In-Order Data Delivery Enabled ", m_CurrentSet.inOrderDataDeliveryEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Device Initiated Power Management Enabled", m_CurrentSet.deviceInitiatedPowerManagementEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Dma Setup Auto-Activation Enabled", m_CurrentSet.dmaSetupEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Non-Zero Buffer Offsets Enabled", m_CurrentSet.nonZeroBufferEnabled);
        opensea_parser::set_Json_Bool(currentSettings, "Current signalling speed is Gen3", m_CurrentSet.currentGen3SignalingSpeed);
        opensea_parser::set_Json_Bool(currentSettings, "Current signalling speed is Gen2", m_CurrentSet.currentGen2SignalingSpeed);
        opensea_parser::set_Json_Bool(currentSettings, "Current signalling speed is Gen1", m_CurrentSet.currentGen1SignalingSpeed);
        opensea_parser::set_Json_Bool(currentSettings, "Reporting of current signalling speed is not supported", m_CurrentSet.currentSignallingNotSupported);

        json_push_back(current, currentSettings);
    }
    else
    {
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//
//! \fn get_current_Hardware()
//
//! \brief
//!   Description: get the value of Current Hardware feature control identifier / offset 40 - 41
//
//  Entry:
//! \param hardware - the JSONNODE pointer to all of the json data 
//
//  Exit:
//
//---------------------------------------------------------------------------
void CAta_Identify_Log_08::get_Current_Hardware(JSONNODE *hardware)
{
    if (m_pLog->currentHardware &BIT1)
    {
        JSONNODE *currentHardware = json_new(JSON_NODE);
        json_set_name(currentHardware, "Current SATA Hardware settings");
        opensea_parser::set_Json_Bool(currentHardware, "Software Reset", true);
        opensea_parser::set_Json_Bool(currentHardware, "Hardware Reset", true);
        json_push_back(hardware, currentHardware);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_supported_Hardware()
//
//! \brief
//!   Description: get the value of supported Hardware feature control identifier / offset 42 - 43
//
//  Entry:
//! \param supported - the JSONNODE pointer to all of the json data 
//
//  Exit:
//
//---------------------------------------------------------------------------
void CAta_Identify_Log_08::get_Supported_Hardware(JSONNODE *supported)
{
    if (m_pLog->currentHardware &BIT1)
    {
        JSONNODE *supportedHardware = json_new(JSON_NODE);
        json_set_name(supportedHardware, "Supported SATA Hardware settings");
        opensea_parser::set_Json_Bool(supportedHardware, "Software Reset", true);
        opensea_parser::set_Json_Bool(supportedHardware, "Hardware Reset", true);
        json_push_back(supported, supportedHardware);
    }
}
//-----------------------------------------------------------------------------
//
//! \fn get_Device_sleep_Timing_Variables()
//
//! \brief
//!   Description: get the value of device sleep varibles
//
//  Entry:
//! \param sleep - the JSONNODE pointer to all of the json data 
//
//  Exit:
//
//---------------------------------------------------------------------------
void CAta_Identify_Log_08::get_Device_Sleep_Timing_Variables(JSONNODE *sleep)
{
    if (m_pLog->deviceSleepTiming &BIT63)
    {
        opensea_parser::set_Json_Bool(sleep, "DEVSLP Timing Variables Supported", true);
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << M_GETBITRANGE(m_pLog->deviceSleepTiming, 15, 8);
        json_push_back(sleep, json_new_a("DEVSLEEP Exit Timeout Field (DETO)", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << M_GETBITRANGE(m_pLog->deviceSleepTiming, 4, 0);
        json_push_back(sleep, json_new_a("Minimum DEVSLP Assertion Time Field (MDAT)", temp.str().c_str()));

    }
    else
    {
        opensea_parser::set_Json_Bool(sleep, "DEVSLP Timing Variables Supported", false);
    }

}
//-----------------------------------------------------------------------------
//
//! \fn get_Log_Page08()
//
//! \brief
//!   Description:  parse Log page 08
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------
eReturnValues CAta_Identify_Log_08::get_Log_Page08(uint8_t *lp8pData, JSONNODE *masterData)
{
#define LOG_PAGE_08   0x0008
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    sLogPage08 logPage;
    m_pLog = &logPage;
    memset(m_pLog, 0, sizeof(sLogPage08));
    m_pLog = reinterpret_cast<sLogPage08 *>(&lp8pData[0]);

    uint16_t pageNumber = M_Word1(m_pLog->header);
    uint16_t revision = M_Word0(m_pLog->header);
    if (pageNumber == LOG_PAGE_08)
    {
        JSONNODE *pageInfo = json_new(JSON_NODE);
        json_set_name(pageInfo, "Log Page 08h");
#if defined _DEBUG
        printf("Log Page 08h \n");
        printf("\tHeader =   0x%016" PRIx64"  \n", m_pLog->header);
#endif
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_pLog->header;
        json_push_back(pageInfo, json_new_a("Page Header", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << pageNumber;
        json_push_back(pageInfo, json_new_a("Page Number", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << revision;
        json_push_back(pageInfo, json_new_a("Revision Number", temp.str().c_str()));

        get_Sata_Capabilities(pageInfo);
        get_Current_Sata(pageInfo);
        get_Current_Hardware(pageInfo);
        get_Supported_Hardware(pageInfo);
        get_Device_Sleep_Timing_Variables(pageInfo);

        json_push_back(masterData, pageInfo);
        retStatus = eReturnValues::SUCCESS;
    }
    else
    {
        retStatus = eReturnValues::FAILURE;
    }
    return retStatus;
}

// *******************************************************************************
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_30()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log 30
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_30::CAta_Identify_Log_30(uint8_t *pBufferData)
    :pData(pBufferData)
    , m_name("log page 30")
    , m_status(eReturnValues::IN_PROGRESS)
{
    if (pData != M_NULLPTR)
    {
        m_status = eReturnValues::SUCCESS;
    }
    else
    {
        m_status = eReturnValues::BAD_PARAMETER;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   CAta_Identify_Log_30()
//
//! \brief
//!   Description:  Class constructor for the CIdentify log 30
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAta_Identify_Log_30::CAta_Identify_Log_30(const std::string & fileName)
    :pData()
    , m_name("log page 30")
    , m_status(eReturnValues::IN_PROGRESS)
{
    CLog *cCLog;
    cCLog = new CLog(fileName);
    if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
    {
        if (cCLog->get_Buffer() != M_NULLPTR)
        {
            size_t bufferSize = cCLog->get_Size();
            pData = new uint8_t[cCLog->get_Size()];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
            memcpy(pData, cCLog->get_Buffer(), bufferSize);
#else
            memcpy_s(pData, bufferSize, cCLog->get_Buffer(), bufferSize);// copy the buffer data to the class member pBuf
#endif
            sLogPageStruct *idCheck;
            idCheck = reinterpret_cast<sLogPageStruct*>(&pData[0]);
            byte_Swap_16(&idCheck->pageLength);
            if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
            {
                m_status = eReturnValues::IN_PROGRESS;
            }
            else
            {
                m_status = eReturnValues::BAD_PARAMETER;
            }
        }
        else
        {

            m_status = eReturnValues::FAILURE;
        }
    }
    else
    {
        m_status = cCLog->get_Log_Status();
    }
    delete (cCLog);
}
//-----------------------------------------------------------------------------
//
//! \fn   ~CIdentifyLog30()
//
//! \brief
//!   Description:  Class deconstructor for for the CIdentifyLog30
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//-------------------------------------------------------------------------
CAta_Identify_Log_30::~CAta_Identify_Log_30()
{
    if (pData != M_NULLPTR)
    {
        delete[] pData;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn CAta_Identify_Log_30::GetInterfaceType()
//
//! \brief
//!   Description:  parse out the interface Type from the log
//
//  Entry:
//! \param ptr  pointer to the buffer data
//
//  Exit:
//!   \return string the interface type
//
//---------------------------------------------------------------------------

/*eReturnValues CAta_Identify_Log_30::get_Interface_Type()
{
    std::string interfaceType("unknow");
    uint16_t interfaceOffset = 0x0180 + 512;

    if (pData[interfaceOffset] == 'S')
    {
        interfaceType = "SCSI";
    }
    else if (pData[interfaceOffset] == 'A')
    {
        interfaceType = "ATA";
    }
    else if (pData[interfaceOffset] == 'F')
    {
        interfaceType = "FC";
    }
    else if (pData[interfaceOffset] == 'U')
    {
        interfaceType = "USB";
    }
    else
    {
        interfaceType = "unknown";
    }

    return eReturnValues::SUCCESS;
}*/

eReturnValues CAta_Identify_Log_30::parse_Identify_Log_30(JSONNODE *masterData)
{
    // Parse the log page 00.
    CAta_Identify_Log_00 *cLogPage00;
    cLogPage00 = new CAta_Identify_Log_00(&pData[0x000]);
    cLogPage00->get_Log_Page00(masterData);

    // Parse the Log page 01h
    if (cLogPage00->is_Page_Supported(1))
    {
        CAta_Identify_log *cIdent;
        cIdent = new CAta_Identify_log(&pData[0x200]);
        m_status = cIdent->print_Identify_Information(masterData);
        delete (cIdent);
    }
    // Parse the log page 02h
    if (cLogPage00->is_Page_Supported(2))
    {
        CAta_Identify_Log_02 *cLogPage02;
        cLogPage02 = new CAta_Identify_Log_02(&pData[0x400]);
        cLogPage02->get_Log_Page02(&pData[0x400], masterData);
        delete (cLogPage02);
    }
    // Parse the log page 03h
    if (cLogPage00->is_Page_Supported(2))
    {
        CAta_Identify_Log_03 *cLogPage03;
        cLogPage03 = new CAta_Identify_Log_03(&pData[0x600]);
        cLogPage03->get_Log_Page03(&pData[0x600], masterData);
        delete (cLogPage03);
    }
    // Parse the log page 04h
    if (cLogPage00->is_Page_Supported(4))
    {
        CAta_Identify_Log_04 *cLogPage04;
        cLogPage04 = new CAta_Identify_Log_04(&pData[0x800]);
        cLogPage04->get_Log_Page04(&pData[0x800], masterData);
        delete (cLogPage04);
    }
    // Parse the log page 05h
    if (cLogPage00->is_Page_Supported(5))
    {
        CAta_Identify_Log_05 *cLogPage05;
        cLogPage05 = new CAta_Identify_Log_05(&pData[0xa00]);
        cLogPage05->get_Log_Page05(&pData[0xa00], masterData);
        delete (cLogPage05);
    }
    // Parse the log page 06h
    if (cLogPage00->is_Page_Supported(6))
    {
        CAta_Identify_Log_06 *cLogPage06;
        cLogPage06 = new CAta_Identify_Log_06(&pData[0xc00]);
        cLogPage06->get_Log_Page06(&pData[0xc00], masterData);
        delete (cLogPage06);
    }
    // Parse the log page 07h
    if (cLogPage00->is_Page_Supported(7))
    {
        CAta_Identify_Log_07 *cLogPage07;
        cLogPage07 = new CAta_Identify_Log_07(&pData[0xe00]);
        cLogPage07->get_Log_Page07(&pData[0xe00], masterData);
        delete (cLogPage07);
    }
    // Parse the log page 08h
    if (cLogPage00->is_Page_Supported(8))
    {
        CAta_Identify_Log_08 *cLogPage08;
        cLogPage08 = new CAta_Identify_Log_08(&pData[0x1000]);
        cLogPage08->get_Log_Page08(&pData[0x1000], masterData);
        delete (cLogPage08);
    }
    //get_Interface_Type();

    delete (cLogPage00);
    return eReturnValues::SUCCESS;
};

