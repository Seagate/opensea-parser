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
    :pData()
	, m_logSize(0)
	, m_name("Ext Comp Log")
	, m_status(IN_PROGRESS)
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
CExtComp::CExtComp(uint8_t *buffer, size_t logSize, JSONNODE *masterData)
	:pData(buffer)
	, m_logSize(logSize)
	, m_name("Ext Comp Log")
	, m_status(IN_PROGRESS)
    , fileName(false)
{

	if (buffer != NULL)                           // if the buffer is null then exit something did go right
	{
		m_status = IN_PROGRESS;
		m_status = parse_Ext_Comp_Log( masterData);
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
	, m_logSize()
    , m_name("Ext Comp Log")
    , m_status(IN_PROGRESS)
    , fileName(true)
{
	CLog *cCLog;
	cCLog = new CLog(fileName);
	if (cCLog->get_Log_Status() == SUCCESS)
	{
		if (cCLog->get_Buffer() != NULL)
		{
			m_logSize = cCLog->get_Size();
			pData = new uint8_t[m_logSize];								// new a buffer to the point				
#ifndef _WIN64
			memcpy(pData, cCLog->get_Buffer(), m_logSize);
#else
			memcpy_s(pData, m_logSize, cCLog->get_Buffer(), m_logSize);// copy the buffer data to the class member pBuf
#endif
			sLogPageStruct *idCheck;
			idCheck = (sLogPageStruct *)&pData[0];
			byte_Swap_16(&idCheck->pageLength);
			if (IsScsiLogPage(idCheck->pageLength, idCheck->pageCode) == false)
			{
				byte_Swap_16(&idCheck->pageLength);  // now that we know it's not scsi we need to flip the bytes back
				m_status = parse_Ext_Comp_Log(masterData);
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
    if (fileName)
    {
        if (pData != NULL)
        {
            delete[] pData;
        }
    }
}
//-----------------------------------------------------------------------------
//
//! \fn Ext_Comp::get_State_Meaning()
//
//! \brief
//!   Description: parse the state and find the string meaning
//
//  Entry:
//! \param stateMeaning  = string to put the information in
//! \param state = parsed out state
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CExtComp::get_State_Meaning(std::string *stateMeaning, uint8_t state)
{
	switch (state)
	{
		case 0x0:
		{
			stateMeaning->assign("Unknown");
			break;
		}
		case 0x1:
		{
			stateMeaning->assign("Sleep");
			break;
		}
		case 0x2:
		{
			stateMeaning->assign("Standby");
			break;
		}
		case 0x3:
		{
			stateMeaning->assign("Active/Idle");
			break;
		}
		case 0x4:
		default:
		{
			stateMeaning->assign("Vendor Specific");
			break;
		}

	}
	return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn Ext_Comp::parse_Ext_CompLog()
//
//! \brief
//!   Description: parse the extented Comp Log and set the pareto based on # of ecc errors
//
//  Entry:
//! \param mansterData = Json Node that holds all of the data 
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CExtComp::parse_Ext_Comp_Structure(uint32_t structNumber, uint32_t sector, JSONNODE *structureData)
{
    std::string myStr = "Parse Ext Comp Log";
	myStr.resize(BASIC);
    uint8_t  deviceControl = 0;
    uint16_t featureField = 0;
    uint16_t countField = 0;
    uint16_t lowLBA = 0;
    uint16_t midLBA = 0;
    uint16_t hiLBA = 0;
    uint8_t  deviceHead = 0;
    uint8_t  commandField = 0;
    //uint8_t  reserved = 0;
    uint32_t timeStamp = 0;
    uint8_t  errorField = 0;
    uint8_t  status = 0;
    uint16_t lifeTime = 0;
    uint16_t COMPIndex = 0;
    uint32_t wOffset = sector + 4; //Points to error data structure
    uint8_t state = 0;
    uint64_t LBA = 0;
    uint16_t deviceErrorCount = 0;

	snprintf((char*)myStr.c_str(), BASIC, "Ext Comp SMART Log Sturcture %" PRId16"", structNumber);
    JSONNODE *EComp = json_new(JSON_NODE);
    json_set_name(EComp, (char*)myStr.c_str());

	if (sector == 0)
	{
		json_push_back(EComp, json_new_i("Ext Comp SMART Log Version", static_cast<int>(pData[0])));

		COMPIndex = pData[2];
		json_push_back(EComp, json_new_i("Ext Comp Log Index", static_cast<int>(COMPIndex)));

		deviceErrorCount = pData[500];
		json_push_back(EComp, json_new_i("Ext Comp Device Error Count", static_cast<int>(deviceErrorCount)));
	}
    for (uint16_t z = 1; z < 5; z++)
    {
		snprintf((char*)myStr.c_str(), BASIC, "Opcode Content %" PRId16"", (z + (structNumber* 4)));
		JSONNODE *opcode = json_new(JSON_NODE);
		json_set_name(opcode, (char*)myStr.c_str());
        for (int cmddata = 1; cmddata < 6; cmddata++)
        {
			snprintf((char*)myStr.c_str(), BASIC, "Command Data Structure %" PRId16"", cmddata);
			JSONNODE *cmdNode = json_new(JSON_NODE);
			json_set_name(cmdNode, (char*)myStr.c_str());
            deviceControl = pData[wOffset];
            featureField = pData[wOffset + 1];
            countField = pData[wOffset + 3];
            lowLBA = pData[wOffset + 5];
            midLBA = pData[wOffset + 7];
            hiLBA = pData[wOffset + 9];
            deviceHead = pData[wOffset + 11];
            commandField = pData[wOffset + 12];
            //reserved = pData[wOffset + 13];
            timeStamp = pData[wOffset + 14];
            word_Swap_32(&timeStamp);
            wOffset += 18;
#if defined _DEBUG
			printf("\nControl  Feature  Count  lowLBA   midLBA  hiLBA device  command  timeStamp");
            printf("\n0x%02x     0x%04x   0x%04x 0x%04x   0x%04x  0x%04x 0x%02x   0x%02x      %d \n", \
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
			snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", deviceControl);
			json_push_back(cmdNode, json_new_a("Device Control", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", featureField);
			json_push_back(cmdNode, json_new_a("Feature Field", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", countField);
			json_push_back(cmdNode, json_new_a("Count Field", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", lowLBA);
			json_push_back(cmdNode, json_new_a("lowLBA", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", midLBA);
			json_push_back(cmdNode, json_new_a("CmidLBA", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%04" PRIx16"", hiLBA);
			json_push_back(cmdNode, json_new_a("hiLBA", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", deviceHead);
			json_push_back(cmdNode, json_new_a("Device Head", (char*)myStr.c_str()));
			snprintf((char*)myStr.c_str(), BASIC, "0x%02" PRIx8"", commandField);
			json_push_back(cmdNode, json_new_a("Command Field", (char*)myStr.c_str()));

			json_push_back(cmdNode, json_new_i("Time Stamp", static_cast<int>(timeStamp)));


			json_push_back(opcode, cmdNode);

        }
        //printf("offset = %d \n",wOffset);
        //reserved = pData[wOffset];
        errorField = pData[wOffset + 1];
        countField = pData[wOffset + 3];
        lowLBA = pData[wOffset + 5];
        midLBA = pData[wOffset + 7];
        hiLBA = pData[wOffset + 9];
        deviceHead = pData[wOffset + 10];
        status = pData[wOffset + 11];
        state = pData[wOffset + 30];
        lifeTime = pData[wOffset + 32];
        LBA = ((uint64_t)(((uint64_t)hiLBA << 32) +
            ((uint64_t)midLBA << 16) +
            (lowLBA)));
        byte_Swap_16(&lifeTime);
        wOffset += 34;
#if defined _DEBUG
        printf("%x %x %x %x %x %x %x %x %x %d \n", \
            deviceControl, \
            errorField, \
            countField, \
            lowLBA, \
            midLBA, \
            hiLBA, \
            deviceHead, \
            status, \
            state, \
            lifeTime);
#endif
		snprintf((char*)myStr.c_str(), BASIC, "0x%x", errorField);
		json_push_back(opcode, json_new_a("Error", (char*)myStr.c_str()));

		snprintf((char*)myStr.c_str(), BASIC, "0x%" PRIx16"", countField);
		json_push_back(opcode, json_new_a("Count", (char*)myStr.c_str()));

		opensea_parser::set_json_64bit(opcode, "LBA", LBA, false);

		json_push_back(opcode, json_new_i("Device", static_cast<int>(deviceControl)));

		snprintf((char*)myStr.c_str(), BASIC, "0x%x", status);
		json_push_back(opcode, json_new_a("Status", (char*)myStr.c_str()));

		get_State_Meaning(&myStr, state);
		json_push_back(opcode, json_new_a("Ext Comp Error Log State", (char*)myStr.c_str()));

		snprintf((char*)myStr.c_str(), BASIC, "%" PRId16"", lifeTime);
		json_push_back(opcode, json_new_a("Ext Comp Error Log Life Timestamp", (char*)myStr.c_str()));

		json_push_back(EComp, opcode);
    }
    
    
    json_push_back(structureData, EComp);

    return SUCCESS;
}
//-----------------------------------------------------------------------------
//
//! \fn Ext_Comp::parse_Ext_CompLog()
//
//! \brief
//!   Description: parse the extented Comp Log and set the pareto based on # of ecc errors
//
//  Entry:
//! \param mansterData = Json Node that holds all of the data 
//
//  Exit:
//!   \return eReturnValues success
//
//---------------------------------------------------------------------------
eReturnValues CExtComp::parse_Ext_Comp_Log(JSONNODE *masterData)
{
	eReturnValues retStatus = IN_PROGRESS;
	uint32_t sectorNumber = 0;
	JSONNODE *ExtCompData = json_new(JSON_NODE);
	json_set_name(ExtCompData, "Extended Comperhensive SMART Error Log");
	for (uint32_t sector = 0; sector < m_logSize; sector += 512)
	{
		// call the structure for each sector(512) of data
		retStatus = parse_Ext_Comp_Structure(sectorNumber, sector, ExtCompData);
		if (retStatus != SUCCESS)
		{
			break;
		}
		sectorNumber++;
	}
	json_push_back(masterData, ExtCompData);
	return retStatus;
}

