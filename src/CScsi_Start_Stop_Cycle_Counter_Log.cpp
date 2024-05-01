//
// CScsi_Start_Stop_Counter_Log.cpp  Implementation of CScsi start stop cycle counter Log class
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CScsi_Start_Stop_Counter_Log.cpp Implementation of CScsi start stop cycle counter Log class
//
#include "CScsi_Start_Stop_Cycle_Counter_Log.h"
#include <sstream>
#include <iomanip>

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CScsiStartStop()
//
//! \brief
//!   Description: Default Class constructor 
//
//  Entry:
// \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiStartStop::CScsiStartStop()
    : pData(NULL)
    , m_SSName("Start Stop Log")
    , m_StartStatus(eReturnValues::IN_PROGRESS)
    , m_PageLength(0)
    , m_SubPage(0)
    , m_Page(0)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CScsiStartStop()
//
//! \brief
//!   Description: Class constructor for the CScsiStartStop
//
//  Entry:
//! \param buffer = holds the buffer information
//! \param masterData - the pointer to the json node that holds all of the json data
//
//  Exit:
//
//---------------------------------------------------------------------------
CScsiStartStop::CScsiStartStop(uint8_t * buffer, size_t bufferSize, JSONNODE *masterData)
    : pData(NULL)
    , m_SSName("Start Stop Log")
    , m_StartStatus(eReturnValues::IN_PROGRESS)
    , m_PageLength(0)
    , m_SubPage(0)
    , m_Page(0)
{
    if (buffer != NULL)
    {
        pData = new uint8_t[bufferSize];								// new a buffer to the point				
#ifndef __STDC_SECURE_LIB__
        memcpy(pData, buffer, bufferSize);
#else
        memcpy_s(pData, bufferSize, buffer, bufferSize);// copy the buffer data to the class member pBuf
#endif
        if (pData != NULL)
        {
            if (bufferSize >= sizeof(sStartStopStruct))				// check for invaid log size < need to add in the size of the log page header
            {
                m_Page = reinterpret_cast<sStartStopStruct *>(pData);				// set a buffer to the point to the log page info
                m_StartStatus = parse_Start_Stop_Log(masterData);
            }
            else
            {
                m_StartStatus = static_cast<eReturnValues>(eReturnValues::INVALID_LENGTH);
            }
        }
        else
        {
            m_StartStatus = eReturnValues::MEMORY_FAILURE;
        }
    }
    else
    {
        m_StartStatus = eReturnValues::FAILURE;
    }

}

//-----------------------------------------------------------------------------
//
//! \fn CScsiStartStop
//
//! \brief
//!   Description: Class deconstructor 
//
//  Entry:
//! \param 
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CScsiStartStop::~CScsiStartStop()
{
    if (pData != NULL)
    {
        delete[] pData;
        pData = NULL;
    }
}
//-----------------------------------------------------------------------------
//
//! \fn parse_Start_Stop_Log
//
//! \brief
//!   Description:parse the start stop log into json data, Calling needed function to parse each parameter 
//
//  Entry:
//! \param masterData - the master json node pointer
//
//  Exit:
//!   \return eReturnValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiStartStop::parse_Start_Stop_Log(JSONNODE *masterData)
{
    eReturnValues status = eReturnValues::IN_PROGRESS;
    eReturnValues retStatus = eReturnValues::IN_PROGRESS;
    JSONNODE *pageInfo = json_new(JSON_NODE);
    json_set_name(pageInfo, "Scsi Start Stop Cycle Counter Log - Eh");
    byte_Swap_16(&m_Page->manufatureParamCode);
    if (static_cast<uint16_t>(eSSMLogParams::manufactureDate) == m_Page->manufatureParamCode)
    {
        retStatus = week_Year_Print(pageInfo, m_Page->manufatureParamCode, m_Page->paramLength1, m_Page->paramControlByte1, m_Page->year, m_Page->week, "Date of Manufacture", "Year of Manufacture", "Week of Manufacture");
    }
    else
    {
        retStatus = eReturnValues::BAD_PARAMETER;
    }

    if (retStatus == eReturnValues::SUCCESS)
    {
        byte_Swap_16(&m_Page->accountParamCode);
        if (static_cast<uint16_t>(eSSMLogParams::accountingDate) == m_Page->accountParamCode)
        {
            retStatus = week_Year_Print(pageInfo, m_Page->accountParamCode, m_Page->paramLength2, m_Page->paramControlByte2, m_Page->accYear, m_Page->accWeek, "Accounting Date", "Accounting Year", "Accounting Week");
        }
        else
        {
            retStatus = eReturnValues::BAD_PARAMETER;
        }
        if (retStatus == eReturnValues::SUCCESS)
        {
            byte_Swap_16(&m_Page->specCycleParamCode);
            if (static_cast<uint16_t>(eSSMLogParams::specLifetime) == m_Page->specCycleParamCode)
            {
                retStatus = get_Count(pageInfo, m_Page->specCycleParamCode, m_Page->paramLength3, m_Page->paramControlByte3, m_Page->specLifeTime, "Specified Cycle Count", "Specified Cycle Count Over Device Lifetime");
            }
            else
            {
                retStatus = eReturnValues::BAD_PARAMETER;
            }
            if (retStatus == eReturnValues::SUCCESS)
            {
                byte_Swap_16(&m_Page->AccumulatedParamCode);
                if (static_cast<uint16_t>(eSSMLogParams::accumulated) == m_Page->AccumulatedParamCode)
                {
                    retStatus = get_Count(pageInfo, m_Page->AccumulatedParamCode, m_Page->paramLength4, m_Page->paramControlByte4, m_Page->accumulatedCycles, "Accumulated Start-Stop Count", "Accumulated Start-Stop Over Device Lifetime");
                }
                else
                {
                    retStatus = eReturnValues::BAD_PARAMETER;
                }
                if (retStatus == eReturnValues::SUCCESS)
                {
                    byte_Swap_16(&m_Page->loadUnloadParamCode);
                    if (static_cast<uint16_t>(eSSMLogParams::loadUnload) == m_Page->loadUnloadParamCode)
                    {
                        retStatus = get_Count(pageInfo, m_Page->loadUnloadParamCode, m_Page->paramLength5, m_Page->paramControlByte5, m_Page->loadUnloadCount, "Load - Unload Count", "Specified Load-Unload Count Over Device Lifetime");
                    }
                    else
                    {
                        retStatus = eReturnValues::BAD_PARAMETER;
                    }
                    if (retStatus == eReturnValues::SUCCESS)
                    {
                        byte_Swap_16(&m_Page->accLoadUnloadParamCode);
                        if (static_cast<uint16_t>(eSSMLogParams::accumulatedLU) == m_Page->accLoadUnloadParamCode)
                        {
                            retStatus = get_Count(pageInfo, m_Page->accLoadUnloadParamCode, m_Page->paramLength6, m_Page->paramControlByte6, m_Page->accloadUnloadCount, "Accumulated Load - Unload Count", "Specified Load-Unload Count Over Device Lifetime");
                        }
                        else
                        {
                            retStatus = eReturnValues::BAD_PARAMETER;
                        }
                    }
                }
            }
        }
    }
    json_push_back(masterData, pageInfo);

    if (retStatus == eReturnValues::BAD_PARAMETER || retStatus == eReturnValues::IN_PROGRESS)
    {
        status = eReturnValues::BAD_PARAMETER;
    }
    else
    {
        status = eReturnValues::SUCCESS;
    }
    return status;
}

//-----------------------------------------------------------------------------
//
//! \fn week_Year_Print
//
//! \brief
//!   Description:parse out the manufaturing and Accounting Date parameter of the log
//
//  Entry:
//! \param manData - node to add the manufacturing data to
//! \param param - param code 
//! \param paramlength - the length of the parm information
//! \param paramConByte - the control byte data
//! \param year - the binary data for the year
//! \param week - the binary data for the week
//! \param strHeader - the string data for the header of the json data
//! \param strYear - the string for the year
//! \param strWeek - the string for the week
//
//  Exit:
//!   \return eReturnParserValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiStartStop::week_Year_Print(JSONNODE *data, uint16_t param, uint8_t paramlength, uint8_t paramConByte, uint32_t year, uint16_t week, const std::string strHeader, const std::string strYear, const std::string strWeek)
{
#define YEARSIZE 4
#define WEEKSIZE 2
	std::string myStr;

	JSONNODE *dateInfo = json_new(JSON_NODE);
	json_set_name(dateInfo, strHeader.c_str());
	
    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        std::ostringstream temp;

        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << param;
        json_push_back(dateInfo, json_new_a("Parameter Code", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(paramlength); //cast is because streams interpret char/unsigned char as a character, but 16 bits wide will be ok to cast to to get around this. -TJE
        json_push_back(dateInfo, json_new_a("Parameter Length", temp.str().c_str()));

        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') <<std::setw(2) << static_cast<uint16_t>(paramConByte); //cast is because streams interpret char/unsigned char as a character, but 16 bits wide will be ok to cast to to get around this. -TJE
        json_push_back(dateInfo, json_new_a("Control Byte", temp.str().c_str()));
    }
	
    if (year != UINT32_C(0x20202020))
    {
        bool validASCII = true;
        char charYear[YEARSIZE + 1] = { 0 };
        memcpy(&charYear[0], &year, YEARSIZE);
        //Check if all ascii characters or not
        for (uint8_t i = 0; i < YEARSIZE; ++i)
        {
            if (!is_ASCII(charYear[i]) || !isdigit(charYear[i]))
            {
                validASCII = false;
                break;
            }
        }
        if (validASCII)
        {
            myStr.assign(reinterpret_cast<const char*>(&year), sizeof(year));
        }
        else
        {
            myStr = "0000";
        }
    }
    else
    {
        myStr = "0000";
    }
	json_push_back(dateInfo, json_new_a(strYear.c_str(), myStr.c_str()));
    if (week != UINT16_C(0x2020))
    {
        bool validASCII = true;
        char charWeek[WEEKSIZE + 1] = { 0 };
        memcpy(&charWeek[0], &week, WEEKSIZE);
        //Check if all ascii characters or not
        for (uint8_t i = 0; i < WEEKSIZE; ++i)
        {
            if (!is_ASCII(charWeek[i]) || !isdigit(charWeek[i]))
            {
                validASCII = false;
                break;
            }
        }
        if (validASCII)
        {
            myStr.assign(reinterpret_cast<const char*>(&week), sizeof(week));
        }
        else
        {
            myStr = "00";
        }
    }
    else
    {
        myStr = "00";
    }
	json_push_back(dateInfo, json_new_a(strWeek.c_str(), myStr.c_str()));

    json_push_back(data, dateInfo);
    return eReturnValues::SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Count
//
//! \brief
//!   Description:parse out the cycle count over the lifetime of the device
//
//  Entry:
//! \param countData - node to add the cycle count data to
//
//  Exit:
//!   \return eReturnParserValues
//
//---------------------------------------------------------------------------
eReturnValues CScsiStartStop::get_Count(JSONNODE *countData, uint16_t param, uint8_t paramlength, uint8_t paramConByte, uint32_t count, const std::string strHeader, const std::string strCount)
{
	JSONNODE *countInfo = json_new(JSON_NODE);
	json_set_name(countInfo, strHeader.c_str());

    if (eVerbosityLevels::VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
    {
        std::ostringstream temp;
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << param;
        json_push_back(countInfo, json_new_a("Parameter Code", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << std::dec << static_cast<uint16_t>(paramlength);
        json_push_back(countInfo, json_new_a("Parameter Length", temp.str().c_str()));
        temp.str("");temp.clear();
        temp << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(paramConByte);
        json_push_back(countInfo, json_new_a("Control Byte", temp.str().c_str()));
    }
	byte_Swap_32(&count);
	json_push_back(countInfo, json_new_i(strCount.c_str(),  count )); 

	json_push_back(countData, countInfo);
	return eReturnValues::SUCCESS;
}
