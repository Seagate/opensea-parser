// CAta_Ext_Comprehensive_Log.cpp
//
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2015 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// CAta_Ext_Comprehensive_Log.cpp

#include "CAta_Ext_Comprehensive_Log.h"


using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CExtComp::CExtComp()
//
//! \brief
//!   Description: Class constructor
//
//
//---------------------------------------------------------------------------
CExtComp::CExtComp()
    : m_name("Ext Comp Log")
{

}
//-----------------------------------------------------------------------------
//
//! \fn CExtComp::CExtComp()
//
//! \brief
//!   Description: Class constructor
//
//
//---------------------------------------------------------------------------
CExtComp::CExtComp(uint8_t *buffer, JSONNODE *masterData)
	:pData(buffer)
	, m_name("Ext Comp Log")
	, m_status(IN_PROGRESS)
{

	if (buffer != NULL)                           // if the buffer is null then exit something did go right
	{
		m_status = IN_PROGRESS;
		m_status = ParseExtCompLog( masterData);
	}
	else
	{
		m_status = FAILURE;
	}

}
//-----------------------------------------------------------------------------
//
//! \fn CExtComp::CExtComp()
//
//! \brief
//!   Description: Class constructor
//
//
//---------------------------------------------------------------------------
CExtComp::CExtComp(const std::string &fileName, JSONNODE *masterData)
    :pData()
    , m_name("Ext Comp Log")
    , m_status(IN_PROGRESS)
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			size_t bufferSize = cCLog->get_Size();
			pData = new uint8_t[cCLog->get_Size()];								// new a buffer to the point				
#ifdef __linux__ //To make old gcc compilers happy
			memcpy(pData, cCLog->get_Buffer(), bufferSize);
#else
			memcpy_s(pData, bufferSize, cCLog->get_Buffer(), bufferSize);// copy the buffer data to the class member pBuf
#endif
			sLogPageStruct *idCheck;
			idCheck = (sLogPageStruct *)&pData[0];
			byte_Swap_16(&idCheck->pageLength);
			if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
			{
				m_status = ParseExtCompLog(masterData);
				m_status = SUCCESS;
			}
			else
			{
				m_status = BAD_PARAMETER;
			}
		}
		else
		{

			m_status = FAILURE;
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
//! \fn CExtComp::~CExtComp()
//
//! \brief
//!   Description: Class deconstructor 
//
//
//---------------------------------------------------------------------------
CExtComp::~CExtComp()
{
}
//-----------------------------------------------------------------------------
//
//! \fn Ext_Comp::ParseExtCompLog()
//
//! \brief
//!   Description: parse the extented Comp Log and set the pareto based on # of ecc errors
//
//  Entry:
//! \param pData  = pointer to the buffer data
//! \param mansterData = Json Node that holds all of the data 
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CExtComp::ParseExtCompLog( JSONNODE *masterData)
{
    std::string myStr = "Parse Ext Comp Log";
    uint8_t  deviceControl = 0;
    uint16_t featureField = 0;
    uint16_t countField = 0;
    uint16_t lowLBA = 0;
    uint16_t midLBA = 0;
    uint16_t hiLBA = 0;
    uint8_t  deviceHead = 0;
    uint8_t  commandField = 0;
    uint8_t  reserved = 0;
    uint32_t timeStamp = 0;
    uint8_t  errorField = 0;
    uint8_t  status = 0;
    uint16_t lifeTime = 0;
    uint16_t COMPIndex = 0;
    uint32_t wOffset = 4; //Points to error data structure
    uint8_t State = 0;
    uint64_t LBA = 0;
    uint16_t deviceErrorCount = 0;
    std::string stateStr = "Vendor Specific      ";

    JSONNODE *EComp = json_new(JSON_NODE);
    json_set_name(EComp, "Ext Comp SMART log");

    json_push_back(EComp, json_new_i("Ext Comp SMART Log Version", static_cast<int>(pData[0])));

    COMPIndex = pData[2];
    json_push_back(EComp, json_new_i("Ext Comp Log Index", static_cast<int>(COMPIndex)));

	deviceErrorCount = pData[500];
	json_push_back(EComp, json_new_i("Ext Comp Device Error Count", static_cast<int>(deviceErrorCount)));

    for (uint16_t z = 1; z < 5; z++)
    {
		snprintf((char*)myStr.c_str(), BASIC, "Opcode Content %" PRId16"", z);
		JSONNODE *opcode = json_new(JSON_NODE);
		json_set_name(opcode, (char*)myStr.c_str());
        for (int cmddata = 1; cmddata < 6; cmddata++)
        {
            deviceControl = pData[wOffset];
            featureField = pData[wOffset + 1];
            countField = pData[wOffset + 3];
            lowLBA = pData[wOffset + 5];
            midLBA = pData[wOffset + 7];
            hiLBA = pData[wOffset + 9];
            deviceHead = pData[wOffset + 11];
            commandField = pData[wOffset + 12];
            reserved = pData[wOffset + 13];
            timeStamp = pData[wOffset + 14];
            word_Swap_32(&timeStamp);
            wOffset += 18;
#if defined(_DEBUG)
            printf("%x %x %x %x %x %x %x %x %d \n", \
                deviceControl, \
                featureField, \
                countField, \
                lowLBA, \
                midLBA, \
                hiLBA, \
                deviceHead, \
                commandField, \
                timeStamp);
#endif
        }
        //printf("offset = %d \n",wOffset);
        reserved = pData[wOffset];
        errorField = pData[wOffset + 1];
        countField = pData[wOffset + 3];
        lowLBA = pData[wOffset + 5];
        midLBA = pData[wOffset + 7];
        hiLBA = pData[wOffset + 9];
        deviceHead = pData[wOffset + 10];
        status = pData[wOffset + 11];
        State = pData[wOffset + 30];
        lifeTime = pData[wOffset + 32];
        LBA = ((uint64_t)(((uint64_t)hiLBA << 32) +
            ((uint64_t)midLBA << 16) +
            (lowLBA)));
        byte_Swap_16(&lifeTime);
        wOffset += 34;
#if defined (_DEBUG)
        printf("%x %x %x %x %x %x %x %x %x %d \n", \
            deviceControl, \
            errorField, \
            countField, \
            lowLBA, \
            midLBA, \
            hiLBA, \
            deviceHead, \
            status, \
            State, \
            lifeTime);
#endif

		snprintf((char*)myStr.c_str(), BASIC, "0x%x", errorField);
		json_push_back(opcode, json_new_a("Error", (char*)myStr.c_str()));

		snprintf((char*)myStr.c_str(), BASIC, "0x%" PRIx16"", countField);
		json_push_back(opcode, json_new_a("Count", (char*)myStr.c_str()));

		opensea_parser::set_json_64bit(opcode, "LBA", LBA, false);

		json_push_back(opcode, json_new_i("Device", static_cast<uint32_t>(deviceControl)));

		snprintf((char*)myStr.c_str(), BASIC, "0x%x", status);
		json_push_back(opcode, json_new_a("Status", (char*)myStr.c_str()));
		if (State == 0x0)
		{
			stateStr = "Unknown:";
		}
		else if (State == 0x1)
		{
			stateStr = "Sleep:";
		}
		else if (State == 0x2)
		{
			stateStr = "Standby:";
		}
		else if (State == 0x3)
		{
			stateStr = "Active/Idle:";
		}
		else if (State >= 0x4)
		{
			snprintf((char*)myStr.c_str(), BASIC, "%x", State);
			json_push_back(opcode, json_new_a("Vendor Specific", (char*)myStr.c_str()));
		}
		else
		{
			stateStr = "    null:            ";
		}
		json_push_back(opcode, json_new_a("Ext Comp Error Log State", (char*)stateStr.c_str()));

		snprintf((char*)myStr.c_str(), BASIC, "%" PRId16"", lifeTime);
		json_push_back(opcode, json_new_a("Ext Comp Error Log Life Timestamp", (char*)myStr.c_str()));


		json_push_back(EComp, opcode);
    }
    
    

    

    json_push_back(masterData, EComp);

    if (status == 0x51 && errorField == 0x40)
    {
        return FAILURE;
    }
    return SUCCESS;
}


