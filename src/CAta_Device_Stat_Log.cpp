//
// CAta_Device_Stat_Log.cpp
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

#include "CAta_Device_Stat_Log.h"

#include <sstream>
#include <iomanip>

using namespace opensea_parser;
using namespace std;

#define BIT_59_SET (1 << 59)
#define BIT_60_SET (1 << 60)
#define BIT_61_SET (1 << 61)
#define BIT_62_SET (1 << 62)
#define BIT_63_SET (1 << 63)

//-----------------------------------------------------------------------------
//
//! \fn   CSAtaDevicStatisticsTempLogs()
//
//! \brief
//!   Description:  Default Class constructor for the CSAtaDevicStatisticsTempLogs
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CSAtaDevicStatisticsTempLogs::CSAtaDevicStatisticsTempLogs()
    :m_name("SCT Temp Log")
	, m_logSize(0)
	, m_status(eReturnValues::IN_PROGRESS)
    , m_tempData()
    , JsonData(M_NULLPTR)
{
}
//-----------------------------------------------------------------------------
//
//! \fn   CSAtaDevicStatisticsTempLogs()
//
//! \brief
//!   Description:  Class constructor for the CSAtaDevicStatisticsTempLogs
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CSAtaDevicStatisticsTempLogs::CSAtaDevicStatisticsTempLogs(uint8_t *buffer,JSONNODE *masterData)
    :m_name("SCT Temp Log")
	, m_logSize(0)
    , m_status(eReturnValues::IN_PROGRESS)
    , m_tempData()
    , JsonData(masterData)
{
    if (buffer != M_NULLPTR)
    {
        m_status = parse_SCT_Temp_Log(buffer);
        m_status = print_SCT_Temp_Log();
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   CSAtaDevicStatisticsTempLogs()
//
//! \brief
//!   Description:  Class constructor for the CSAtaDevicStatisticsTempLogs
//
//  Entry:
//! \param fileName = the file name that is to be parsed
//! \param masterData = The master JSON NODE that holds all of the parsed information
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CSAtaDevicStatisticsTempLogs::CSAtaDevicStatisticsTempLogs(const std::string &fileName, JSONNODE *masterData)
    : m_name("SCT Temp Log")
	, m_logSize(0)
	, m_status(eReturnValues::IN_PROGRESS)
    , m_tempData()
    , JsonData(masterData)
{

	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
	{
		if (cCLog->get_Buffer() != M_NULLPTR)
		{
			m_logSize = cCLog->get_Size();
			uint8_t* pData = new uint8_t[m_logSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
			memcpy(pData, cCLog->get_Buffer(), m_logSize);
#else
			memcpy_s(pData, m_logSize, cCLog->get_Buffer(), m_logSize);// copy the buffer data to the class member pBuf
#endif
			sLogPageStruct *idCheck;
			idCheck = reinterpret_cast<sLogPageStruct*>(&pData[0]);
			byte_Swap_16(&idCheck->pageLength);
			if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
			{
				byte_Swap_16(&idCheck->pageLength);  // now that we know it's not scsi we need to flip the bytes back
				m_status = parse_SCT_Temp_Log(pData);
			}
			else
			{
				m_status = eReturnValues::BAD_PARAMETER;
			}
            delete[] pData;
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
//! \fn   ~CSAtaDevicStatisticsTempLogs()
//
//! \brief
//!   Description:  Class deconstructor for for the CSCTLogs
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CSAtaDevicStatisticsTempLogs::~CSAtaDevicStatisticsTempLogs()
{

}
//-----------------------------------------------------------------------------
//
//! \fn   ParseSCTTempLog()
//
//! \brief
//!   Description: parse the SCT temp log from the CDF log.
//
//  Entry:
//
//
//  Exit:
//!  \return eReturnValues eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CSAtaDevicStatisticsTempLogs::parse_SCT_Temp_Log(uint8_t* pData)
{
    //std::string myStr = "Parse SCT Temp Log";
    m_tempData.version = M_BytesTo2ByteValue(pData[1], pData[0]);
    m_tempData.SamplePeriod = M_BytesTo2ByteValue(pData[3], pData[2]);
    m_tempData.Interval = M_BytesTo2ByteValue(pData[5], pData[4]);
    m_tempData.MaxOpLimit = pData[6];
    m_tempData.OverLimit = pData[7];
    m_tempData.MinOpLimit = pData[8];
    m_tempData.UnderLimit = pData[9];
    m_tempData.CBSize = M_BytesTo2ByteValue(pData[31], pData[30]);
    m_tempData.CBIndex = M_BytesTo2ByteValue(pData[33], pData[32]);
    m_tempData.Temperature = pData[(34 + m_tempData.CBIndex)];
    return eReturnValues::SUCCESS;
}

eReturnValues CSAtaDevicStatisticsTempLogs::print_SCT_Temp_Log()
{
    JSONNODE* sctTemp = json_new(JSON_NODE);
    json_set_name(sctTemp, "SCT Temp Log");
#if defined _DEBUG
    printf("\t%s%" PRId16" \n", "Temp Log Sample Period (in minutes):          ", m_tempData.SamplePeriod);
    printf("\t%s%" PRId16" \n", "Temp Log Interval (in minutes):              ", m_tempData.Interval);
    printf("\t%s%" PRId8"  \n", "Temp Log Max Op Limit:                       ", m_tempData.MaxOpLimit);
    printf("\t%s%" PRId8"  \n", "Temp Log Over Limit:                         ", m_tempData.OverLimit);
    printf("\t%s%" PRId8"  \n", "Temp Log Min Op Limit:                       ", m_tempData.MinOpLimit);
    printf("\t%s%" PRId8"  \n", "Temp Log Under Limit:                        ", m_tempData.UnderLimit);
    printf("\t%s%" PRId16" \n", "Temp Log CB Size (in entries):               ", m_tempData.CBSize);
    printf("\t%s%" PRId16" \n", "Temp Log CB Index (current entry):           ", m_tempData.CBIndex);
    printf("\t%s%" PRId8" \n", "Temp Log Temperature of CB Index (Celsius):   ", m_tempData.Temperature);
#endif
    std::ostringstream temp;

    if (m_logSize > 0 && m_logSize < (static_cast<size_t>(m_tempData.CBIndex) + 34))   // check the size fo the data
    {
        json_push_back(JsonData, sctTemp);
        return static_cast<eReturnValues>(eReturnValues::INVALID_LENGTH);
    }


    json_push_back(sctTemp, json_new_i("Temp Log Sample Period (in minutes)", m_tempData.SamplePeriod));
    json_push_back(sctTemp, json_new_i("Temp Log Interval (in minutes)", m_tempData.Interval));
    json_push_back(sctTemp, json_new_i("Temp Log Max Op Limit", m_tempData.MaxOpLimit));
    json_push_back(sctTemp, json_new_i("Temp Log Over Limit", m_tempData.OverLimit));
    json_push_back(sctTemp, json_new_i("Temp Log Min Op Limit", m_tempData.MinOpLimit));
    json_push_back(sctTemp, json_new_i("Temp Log Under Limit", m_tempData.UnderLimit));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_tempData.CBSize;
    json_push_back(sctTemp, json_new_a("Temp Log CB Size (in entries)", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << m_tempData.CBIndex;
    json_push_back(sctTemp, json_new_a("Temp Log CB Index (current entry)", temp.str().c_str()));
    temp.str("");temp.clear();
    temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(m_tempData.Temperature);
    json_push_back(sctTemp, json_new_a("Temp Log Temperature of CB Index (Celsius)", temp.str().c_str()));
    temp.str("");temp.clear();
    json_push_back(JsonData, sctTemp);
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn   CAtaDeviceStatisticsLogs()
//
//! \brief
//!   Description:  Class constructor for the CAtaDeviceStatisticsLogs
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaDeviceStatisticsLogs::CAtaDeviceStatisticsLogs()
    :m_name("Device Stat Log")
    , m_status(eReturnValues::IN_PROGRESS)
    , pData(M_NULLPTR)
    , m_deviceLogSize(0)
    , m_Response()
{
    m_deviceLogSize = 0;
}
//-----------------------------------------------------------------------------
//
//! \fn   CAtaDeviceStatisticsLogs()
//
//! \brief
//!   Description:  Class constructor for the CAtaDeviceStatisticsLogs
//
//  Entry:
//! \param logSize = the size of the device log
//! \param masterData = the pointer to the master json data
//! \param buffer = the bianry data that is to be parsed
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaDeviceStatisticsLogs::CAtaDeviceStatisticsLogs(uint32_t logSize, JSONNODE *masterData, uint8_t *buffer)
    : m_name("Device Stat Log")
    , m_status(eReturnValues::IN_PROGRESS)
    , pData(buffer)
    , m_deviceLogSize(logSize)
    , m_Response()
{
    if (pData != M_NULLPTR)
    {
        m_status = ParseSCTDeviceStatLog(masterData);
    }
    else
    {
        m_status = eReturnValues::FAILURE;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn   CAtaDeviceStatisticsLogs()
//
//! \brief
//!   Description: pClass constructor for the CAtaDeviceStatisticsLogs
//
//  Entry:
//! \param fileName = the name of the file to be opened that has the device log
//! \param masterData = the pointer to the master json data
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaDeviceStatisticsLogs::CAtaDeviceStatisticsLogs(const std::string &fileName, JSONNODE *masterData)
    : m_name("Device Stat Log")
	, m_status(eReturnValues::IN_PROGRESS)
	, pData()
	, m_deviceLogSize(0)
    , m_Response()
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == eReturnValues::SUCCESS)
	{
		if (cCLog->get_Buffer() != M_NULLPTR)
		{
			m_deviceLogSize = cCLog->get_Size();
			pData = new uint8_t[m_deviceLogSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
			memcpy(pData, cCLog->get_Buffer(), m_deviceLogSize);
#else
			memcpy_s(pData, m_deviceLogSize, cCLog->get_Buffer(), m_deviceLogSize);// copy the buffer data to the class member pBuf
#endif
			sLogPageStruct *idCheck;
			idCheck = reinterpret_cast<sLogPageStruct*>(&pData[0]);
			byte_Swap_16(&idCheck->pageLength);
			if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
			{
				byte_Swap_16(&idCheck->pageLength);  // now that we know it's not scsi we need to flip the bytes back
				m_status = ParseSCTDeviceStatLog(masterData);
			}
			else
			{
				m_status = eReturnValues::BAD_PARAMETER;
			}
            delete[] pData;
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
//! \fn   ~CAtaDeviceStatisticsLogs()
//
//! \brief
//!   Description:  Class deconstructor for for the ParseSCTDeviceStatLog()
//
//  Entry:
//
//  Exit:
//!  \return NONE
//
//---------------------------------------------------------------------------
CAtaDeviceStatisticsLogs::~CAtaDeviceStatisticsLogs()
{
}

//-----------------------------------------------------------------------------
//
//! \fn   ParseDeviceStatLog()
//
//! \brief
//!   Description: parse the Device Statistic log.
//
//  Entry:
//! \param masterData = The master Json Node that holds all of the data
//
//  Exit:
//!  \return eReturnValues eReturnValues::SUCCESS
//
//---------------------------------------------------------------------------
eReturnValues CAtaDeviceStatisticsLogs::ParseSCTDeviceStatLog(JSONNODE *masterData)
{
    sHeader *pDeviceHeader = M_NULLPTR;
    uint64_t *pLogPage = M_NULLPTR;
    if (eVerbosityLevels::VERBOSITY_DEFAULT < g_verbosity)
    {
        printf("\nStarting Device Statistics Parsing \n");
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           

    //Device Statistics information header 
    JSONNODE *deviceData = json_new(JSON_NODE);
    json_set_name(deviceData, "Device Statistics log");

    //Define each valid log page.
    for (uint32_t offset = 0; offset < m_deviceLogSize; offset += 512)
    {	
        pDeviceHeader = reinterpret_cast<sHeader*>(&pData[offset]);
        pLogPage = reinterpret_cast<uint64_t*>(&pData[offset]);

        //The members of the sHeader were updated - LogPageNum is corrected from uint16_t to uint8_t as per the spec
        //Nayana Commented the below ifcheck and kept it to know if any such scenario that satisfies this condition
        //Or is it because the LogPageNum data type was incorrect before
        
        /*if (pDeviceHeader->Reserved != 0x0000 && pDeviceHeader->Reserved != 0xffff)
        {
	    if (offset != 0)
            {            
               pDeviceHeader = (sHeader*)&pData[offset - 16];
            }
        }*/

        if (pDeviceHeader->RevNum == 0x0001)
        {
#if defined _DEBUG
            printf("\t%s%02x \n", "Log page number :  ", pDeviceHeader->LogPageNum);
#endif 
            switch (pDeviceHeader->LogPageNum)
            {
            case 00:
                logPage00(pLogPage);
                break;
            case 01:
                logPage01(pLogPage, deviceData);
                break;
            case 02:
                logPage02(pLogPage, deviceData);
                break;
            case 03:
                logPage03(pLogPage, deviceData);
                break;
            case 04:
                logPage04(pLogPage, deviceData);
                break;
            case 05:
                logPage05(pLogPage, deviceData);
                break;
            case 06:
                logPage06(pLogPage, deviceData);
                break;
            case 07:
                logPage07(pLogPage, deviceData);
                break;
            }

        }

    }
    json_push_back(masterData, deviceData);
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn   isBit59Set()
//
//! \brief
//!   Description: check to see if bit 59 is set, if so return true else false
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return bool
//
//---------------------------------------------------------------------------
bool CAtaDeviceStatisticsLogs::isBit59Set(uint64_t *value)
{
    if ((*value & BIT59) == BIT59)
    {
        return (true);
    }
    return (false);
}
//-----------------------------------------------------------------------------
//
//! \fn   isBit59Set()
//
//! \brief
//!   Description: check to see if bit 59 is set, if so return true else false
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return bool
//
//---------------------------------------------------------------------------
bool CAtaDeviceStatisticsLogs::isBit60Set(uint64_t *value)
{
    if ((*value & BIT60) == BIT60)
    {
        return (true);
    }
    return (false);
}
//-----------------------------------------------------------------------------
//
//! \fn   isBit61Set()
//
//! \brief
//!   Description: check to see if bit 61 is set, if so return true else false
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return bool
//
//---------------------------------------------------------------------------
bool CAtaDeviceStatisticsLogs::isBit61Set(uint64_t *value)
{
    if ((*value & BIT61) == BIT61)
    {
        return (true);
    }
    return (false);
}
//-----------------------------------------------------------------------------
//
//! \fn   isBit62Set()
//
//! \brief
//!   Description: check to see if bit 62 is set, if so return true else false
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return bool
//
//---------------------------------------------------------------------------
bool CAtaDeviceStatisticsLogs::isBit62Set(uint64_t *value)
{
    if ((*value & BIT62) == BIT62)
    {
        return (true);
    }
    return (false);
}
//-----------------------------------------------------------------------------
//
//! \fn   isBit63Set()
//
//! \brief
//!   Description: check to see if bit 63 is set, if so return true else false
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return bool
//
//---------------------------------------------------------------------------
bool CAtaDeviceStatisticsLogs::isBit63Set(uint64_t *value)
{
    if ((*value & BIT63) == BIT63)
    {
        return (true);
    }
    return (false);
}
//-----------------------------------------------------------------------------
//
//! \fn   DeviceStatFlag()
//
//! \brief
//!   Description: print out the flag information if the flag is set
//
//  Entry:
//! \param value = the 64 bit value 
//! \param masterData - the master json node that holds all of the data
//
//  Exit:
//!  \return bool
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::DeviceStatFlag(uint64_t *value, JSONNODE *masterData)
{
    //std::string myStr = "Device stat flag";
    
    JSONNODE *sctFlag = json_new(JSON_NODE);
    json_set_name(sctFlag, "Device Statistic Flags");
    //Bit 59 : Monitored Condition
    if (isBit59Set(&value[0]))
    {
        json_push_back(sctFlag, json_new_a("1", "The monitored condition is met."));
    }
    else
    {
        json_push_back(sctFlag, json_new_a("1", "The monitored condition is not met."));
    }

    //Bit 60 : Statistic Supports DSN
    if (isBit60Set(&value[0]))
    {
        json_push_back(sctFlag, json_new_a("2", "This device statistic supports device statistics notification."));
    }
    else
    {
        json_push_back(sctFlag, json_new_a("2", "This device statistic does not support device statistics notification."));
    }

    //Bit 61 : Normallized Statistic
    if (isBit61Set(&value[0]))
    {
        json_push_back(sctFlag, json_new_a("3", "The device statistic's value contains a normalized value."));
    }
    else
    {
        json_push_back(sctFlag, json_new_a("3", "The device statistic's value is not normalized."));
    }

    //Bit 62 : Valid value
    if (isBit62Set(&value[0]))
    {
        json_push_back(sctFlag, json_new_a("4", "The device statistic's value is valid."));
    }
    else
    {
        json_push_back(sctFlag, json_new_a("4", "The device statistic's value is not valid."));
    }

    //Bit 63 : Device statistic supported
    if (isBit63Set(&value[0]))
    {
        json_push_back(sctFlag, json_new_a("5", "Device statistic is supported."));
    }
    else
    {
        json_push_back(sctFlag, json_new_a("5", "Device statistic is not supported."));
    }
    json_push_back(masterData, sctFlag);
}
//-----------------------------------------------------------------------------
//
//! \fn   CheckStatusAndValid_8()
//
//! \brief
//!   Description: check the status on 62 and 63, if true on both then return value as uint8
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return uint8_t value
//
//---------------------------------------------------------------------------
uint8_t CAtaDeviceStatisticsLogs::CheckStatusAndValid_8(uint64_t *value)
{
    uint8_t retValue = 0;
    //Bit 62 : Valid value and 63 bit needs to be set
    if (isBit62Set(value) && isBit63Set(value))
    {
        retValue = static_cast<uint8_t>(*value);
    }
    return retValue;
}
//-----------------------------------------------------------------------------
//
//! \fn   CheckStatusAndValidSigned_8()
//
//! \brief
//!   Description: check the status on 62 and 63, if true on both then return value as int8
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return int8_t value
//
//---------------------------------------------------------------------------
int8_t CAtaDeviceStatisticsLogs::CheckStatusAndValidSigned_8(uint64_t *value)
{
    int8_t retValue = 0;
    //Bit 62 : Valid value and 63 bit needs to be set
    if (isBit62Set(value) && isBit63Set(value))
    {
        retValue = static_cast<int8_t>(*value);
    }
    return retValue;
}
//-----------------------------------------------------------------------------
//
//! \fn   CheckStatusAndValid_32()
//
//! \brief
//!   Description: check the status on 62 and 63, if true on both then return value as uint32
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return uint32_t value
//
//---------------------------------------------------------------------------
uint32_t CAtaDeviceStatisticsLogs::CheckStatusAndValid_32(uint64_t *value)
{
    uint32_t retValue = 0;
    //Bit 62 : Valid value and 63 bit needs to be set
    if (isBit62Set(value) && isBit63Set(value))
    {
        retValue = static_cast<uint32_t>(*value);
    }
    return retValue;
}
//-----------------------------------------------------------------------------
//
//! \fn   CheckStatusAndValid_64()
//
//! \brief
//!   Description: check the status on 62 and 63, if true on both then return value as uint64
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return uint64_t value
//
//---------------------------------------------------------------------------
uint64_t CAtaDeviceStatisticsLogs::CheckStatusAndValid_64(uint64_t* value)
{
    uint64_t retValue = 0;
    //Bit 62 : Valid value and 63 bit needs to be set
    if (isBit62Set(value) && isBit63Set(value))
    {
        retValue = static_cast<uint64_t>(*value) & UINT64_C(0x00FFFFFFFFFFFFFF);
    }
    return retValue;
}
//-----------------------------------------------------------------------------
//
//! \fn   logPage00()
//
//! \brief
//!   Description: prints out all the supporting logs
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage00(uint64_t *value)
{

    uint8_t *pEntries = reinterpret_cast<uint8_t*>(&value[0]);
    uint8_t TotalEntries = 0;
    TotalEntries = pEntries[8];

#if defined _DEBUG
    printf("*****List Of Supported Device(log Page 00h)*****");
    printf("\t%s %d \n\n", "Number of entries  : ", TotalEntries);
#else
    M_USE_UNUSED(TotalEntries);
#endif
}
//-----------------------------------------------------------------------------
//
//! \fn   logPage01()
//
//! \brief
//!   Description: parses and addes the General Statistics to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage01(uint64_t *value, JSONNODE *masterData)
{
    //General Statistics(log page 01) contains general information about the device.
	sLogPage01 *dsLog;
	dsLog = reinterpret_cast<sLogPage01*>(&value[0]);
    //string myStr = "Statistics";
    JSONNODE *sctStat = json_new(JSON_NODE);
    json_set_name(sctStat, "General Statistics(log Page 01h)");
#if defined _DEBUG
    
    printf("\t%s \n", "*****General Statistics(log Page 01h)*****");
#endif
    json_push_back(sctStat, json_new_i("Number of processed Power-On Reset", static_cast<uint32_t>(check_Status_Strip_Status(dsLog->lifeTimePWRResets))));
    json_push_back(sctStat, json_new_i("Power-On Hours", static_cast<uint32_t>(check_Status_Strip_Status(dsLog->poh))));
    opensea_parser::set_json_64bit_With_Check_Status(sctStat, "Logical Sectors Written", dsLog->logSectWritten, false);
    opensea_parser::set_json_64bit_With_Check_Status(sctStat, "Number Of Write Commands", dsLog->numberOfWrites, false);
    opensea_parser::set_json_64bit_With_Check_Status(sctStat, "Logical Sectors Read", dsLog->logSectRead, false);
    opensea_parser::set_json_64bit_With_Check_Status(sctStat, "Number of Read Commands", dsLog->numberOfReads, false);
    opensea_parser::set_json_64bit(sctStat, "Date and Time TimeStamp(hrs)", (static_cast<uint64_t>((check_Status_Strip_Status(dsLog->date)) / 1000) / 3600), false);
	json_push_back(sctStat, json_new_i("Pending Error Count", static_cast<uint32_t>(check_Status_Strip_Status(dsLog->pendingErrorCount))));
	json_push_back(sctStat, json_new_i("Workload Utilization", static_cast<uint32_t>(check_Status_Strip_Status(dsLog->workLoad))));
	opensea_parser::set_json_64bit_With_Check_Status(sctStat, "Utilization Usage Rate", dsLog->utilRate, false);
	json_push_back(sctStat, json_new_i("Resource Availibility", static_cast<uint32_t>(check_Status_Strip_Status(dsLog->resourceAval))));
	json_push_back(sctStat, json_new_i("Random Write Resources Used", static_cast<uint32_t>(check_Status_Strip_Status(dsLog->randomWriteResourcesUsed))));

    json_push_back(masterData, sctStat);
}
//-----------------------------------------------------------------------------
//
//! \fn   logPage02()
//
//! \brief
//!   Description: parses and addes the free-fall information to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage02(uint64_t *value, JSONNODE *masterData)
{
    //Free Fall Statistics(log page 02) contains free-fall information.
    uint64_t *cData = &value[0];
    uint32_t FallEventDet = 0;
    uint32_t OverlimitShock = 0;

    FallEventDet = CheckStatusAndValid_32(&cData[1]);
    OverlimitShock = CheckStatusAndValid_32(&cData[2]);


    JSONNODE *sctFreeFall = json_new(JSON_NODE);
    json_set_name(sctFreeFall, "Free Fall Statistics(log Page 02h)");
#if defined _DEBUG
 
    printf("\t%s ", "*****Free Fall Statistics(log Page 02h)*****");

#endif
    json_push_back(sctFreeFall, json_new_i("Number of Free-Fall Events Detected", FallEventDet));
    //DeviceStatFlag(&cData[1]);

    json_push_back(sctFreeFall, json_new_i("Overlimit Shock Events", OverlimitShock));
    //DeviceStatFlag(&cData[2]);

    json_push_back(masterData, sctFreeFall);
}
//-----------------------------------------------------------------------------
//
//! \fn   logPage03()
//
//! \brief
//!   Description: parses and addes the Rotating Media Statistics to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage03(uint64_t *value, JSONNODE *masterData)
{
    sDeviceLog3  m_sSCT3;
    sDeviceLog3 *pSCT3 = &m_sSCT3;
    //Rotating Media Statistics(log page 03)
    uint64_t *cData = &value[0];

    pSCT3->SpdPoh = static_cast<double>(((CheckStatusAndValid_64(&cData[1]) /1000) /3600.0));   // spec shows hrs. but seagate publishes in microseconds
    pSCT3->HeadFlyHour = static_cast<double>(((CheckStatusAndValid_64(&cData[2]) / 1000) / 3600.0));   // spec shows hrs. but seagate publishes in microseconds
    pSCT3->HeadLoadEvent = CheckStatusAndValid_32(&cData[3]);
    pSCT3->ReLogicalSec = CheckStatusAndValid_32(&cData[4]);
    pSCT3->ReadRecAtmp = CheckStatusAndValid_32(&cData[5]);
    pSCT3->MeStartFail = CheckStatusAndValid_32(&cData[6]);
    pSCT3->ReCandidate = CheckStatusAndValid_32(&cData[7]);
    pSCT3->UnloadEvent = CheckStatusAndValid_32(&cData[8]);

    JSONNODE *sctRotat = json_new(JSON_NODE);
    json_set_name(sctRotat, "Rotating Media Statistics(log Page 03h)");
#if defined _DEBUG
    printf("\t%s \n", "*****Rotating Media Statistics(log Page 03h)*****");
#endif
    json_push_back(sctRotat, json_new_f("Spindle Motor Power-on Hours(hrs)", pSCT3->SpdPoh ));
    json_push_back(sctRotat, json_new_f("Head Flying Hours(hrs)", pSCT3->HeadFlyHour));
    json_push_back(sctRotat, json_new_i("Head Load Events", pSCT3->HeadLoadEvent));
    json_push_back(sctRotat, json_new_i("Number of Reallocated Logical Sectors", pSCT3->ReLogicalSec));
    json_push_back(sctRotat, json_new_i("Read Recovery Attempts", pSCT3->ReadRecAtmp)); //The number of logical sectors that require three or more attemps to read the data from the media for each read command.
    json_push_back(sctRotat, json_new_i("Number of Mechanical Start Failures", pSCT3->MeStartFail));  //The number of mechanical start failures after device manufacture.
    json_push_back(sctRotat, json_new_i("Number of Reallocation Logical Sectors", pSCT3->ReCandidate));   //The number of logical sectors that are candidates for reallocation.
    json_push_back(sctRotat, json_new_i("Number of High Priority Unload Events", pSCT3->UnloadEvent));   //The number of emergency head unload events.
    json_push_back(masterData, sctRotat);

}
//-----------------------------------------------------------------------------
//
//! \fn   logPage04()
//
//! \brief
//!   Description: parses and addes the General Errors Statistics to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage04(uint64_t *value, JSONNODE *masterData)
{
    //General Errors Statistics(log page 04) contains general error infomation about the device.
	sDeviceLog04 *dslog04 = reinterpret_cast<sDeviceLog04*>(&value[0]);

    JSONNODE *sctError = json_new(JSON_NODE);
    json_set_name(sctError, "General Errors Statistics(log Page 04h)");
#if defined _DEBUG
    
    printf("\t%s \n", "*****General Errors Statistics(log Page 04h)*****");
#endif
	json_push_back(sctError, json_new_i("Number of Reported Uncorrectable Errors", static_cast<uint32_t>(check_Status_Strip_Status(dslog04->numberReportedECC))));
	json_push_back(sctError, json_new_i("Number of Resets btw Cmd Completion", static_cast<uint32_t>(check_Status_Strip_Status(dslog04->resets))));
	json_push_back(sctError, json_new_i("Physical Element Status Changed", static_cast<uint32_t>(check_Status_Strip_Status(dslog04->statusChanged))));

    json_push_back(masterData, sctError);

}
//-----------------------------------------------------------------------------
//
//! \fn   logPage05()
//
//! \brief
//!   Description: parses and addes the Temperature Statistics to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage05(uint64_t *value, JSONNODE *masterData)
{
    //Temperature Statistics(log page 05) in degrees Celsius.
    uint64_t *cData = &value[0];
    int8_t CurrentTemp = 0;
    int8_t AvgShortTemp = 0;
    int8_t AvgLongTemp = 0;
    int8_t HighestTemp = 0;
    int8_t LowestTemp = 0;
    int8_t HgstAvgShortTemp = 0;
    int8_t LowAvgShortTemp = 0;
    int8_t HgstAvgLongTemp = 0;
    int8_t LowAvgLongTemp = 0;
    uint32_t TimeInOverTemp = 0;
    int8_t MaxOperTemp = 0;
    int32_t TimeInUndTemp = 0;
    int8_t MinOperTemp = 0;

    CurrentTemp = CheckStatusAndValidSigned_8(&cData[1]);
    AvgShortTemp = CheckStatusAndValidSigned_8(&cData[2]);
    AvgLongTemp = CheckStatusAndValidSigned_8(&cData[3]);
    HighestTemp = CheckStatusAndValidSigned_8(&cData[4]);
    LowestTemp = CheckStatusAndValidSigned_8(&cData[5]);
    HgstAvgShortTemp = CheckStatusAndValidSigned_8(&cData[6]);
    LowAvgShortTemp = CheckStatusAndValidSigned_8(&cData[7]);
    HgstAvgLongTemp = CheckStatusAndValidSigned_8(&cData[8]);
    LowAvgLongTemp = CheckStatusAndValidSigned_8(&cData[9]);
    TimeInOverTemp = CheckStatusAndValid_32(&cData[10]);
    MaxOperTemp = CheckStatusAndValidSigned_8(&cData[11]);
    TimeInUndTemp = static_cast<int32_t>(CheckStatusAndValid_32(&cData[12]));
    MinOperTemp = CheckStatusAndValidSigned_8(&cData[13]);

    //string myStr = "Temperature Statistics";
    JSONNODE *sctTemp = json_new(JSON_NODE);
    json_set_name(sctTemp, "Temperature Statistics(log Page 05h)");
#if defined _DEBUG
    
    printf("\t%s \n", "*****Temperature Statistics(log Page 05h)*****");
#endif
    json_push_back(sctTemp, json_new_i("Current Temperature(Degrees Celsius)", (CurrentTemp)));
    json_push_back(sctTemp, json_new_i("Average Short Term Temperature(Celsius)", (AvgShortTemp)));
    json_push_back(sctTemp, json_new_i("Average Long Term Temperature(Celsius)", (AvgLongTemp)));
    json_push_back(sctTemp, json_new_i("Highest Temperature(Degrees Celsius)", (HighestTemp)));
    json_push_back(sctTemp, json_new_i("Lowest Temperature(Degrees Celsius)", (LowestTemp)));
    json_push_back(sctTemp, json_new_i("Highest Average Short Term Temp(Celsius)", (HgstAvgShortTemp)));
    json_push_back(sctTemp, json_new_i("Lowest Average Short Term Temp(Celsius)", (LowAvgShortTemp)));
    json_push_back(sctTemp, json_new_i("Highest Average Long Term Temp(Celsius)", (HgstAvgLongTemp)));
    json_push_back(sctTemp, json_new_i("Lowest Average Long Term Temp(Celsius)", (LowAvgLongTemp)));
    json_push_back(sctTemp, json_new_i("Time in Over-Temperature(Minutes)", TimeInOverTemp));
    json_push_back(sctTemp, json_new_i("Specified Maximum Operating Temp(Celsius)", (MaxOperTemp)));
    json_push_back(sctTemp, json_new_i("Time in Under-Temperature(Minutes)", TimeInUndTemp));
    json_push_back(sctTemp, json_new_i("Specified Minimum Operating Temp(Celsius)", MinOperTemp));
    json_push_back(masterData, sctTemp);
	
}

//-----------------------------------------------------------------------------
//
//! \fn   logPage06()
//
//! \brief
//!   Description: parses and addes the Transport Statistics to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage06(uint64_t *value, JSONNODE *masterData)
{
    //Transport Statistics(log page 06) contains contains interface transport information about the device.
    uint64_t *cData = &value[0];
sDeviceLog6  m_sSCT6;
    sDeviceLog6 *pSCT6 = &m_sSCT6;
//sDeviceLog6 *pSCT6 = {0};

    pSCT6->HwReset = CheckStatusAndValid_32(&cData[1]);
    pSCT6->ASREvent = CheckStatusAndValid_32(&cData[2]);
    pSCT6->CRCError = CheckStatusAndValid_32(&cData[3]);

    JSONNODE *sctGen = json_new(JSON_NODE);
    json_set_name(sctGen, "Transport Statistics(log Page 06h)");
#if defined _DEBUG
    
    printf("\t%s \n", "*****Transport Statistics(log Page 06h)*****");
#endif
    json_push_back(sctGen, json_new_i("Number of hardware resets", pSCT6->HwReset));
    json_push_back(sctGen, json_new_i("Number of ASR Events", pSCT6->ASREvent));
    json_push_back(sctGen, json_new_i("Number of Interface CRC Errors", pSCT6->CRCError));
    json_push_back(masterData, sctGen);
}
//-----------------------------------------------------------------------------
//
//! \fn   logPage07()
//
//! \brief
//!   Description: parses and addes the Solid State Device Statistics to the json node
//
//  Entry:
//! \param value = the 64 bit value 
//
//  Exit:
//!  \return 
//
//---------------------------------------------------------------------------
void CAtaDeviceStatisticsLogs::logPage07(uint64_t *value, JSONNODE *masterData)
{
    //Solid State Device Statistics(log page 07) contains solid state drive information about the device.
    uint64_t *cData = &value[0];
    uint8_t PercentUsed = 0;
    PercentUsed = CheckStatusAndValid_8(&cData[1]);

    JSONNODE *sctError = json_new(JSON_NODE);
    json_set_name(sctError, "Solid State Device Statistics(log Page 07h)");
#if defined _DEBUG

    printf("\t%s \n", "*****Solid State Device Statistics(log Page 07h)*****");

#endif
    json_push_back(sctError, json_new_i("Percentage Used Endurance Indicator", static_cast<uint32_t>(PercentUsed)));

    //DeviceStatFlag(&cData[1]);
    json_push_back(masterData, sctError);
}


