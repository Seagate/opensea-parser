//
// CFarm_Combine.cpp   Implementation of class CFARMLog
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file CFarm_Combine.cpp  
//
// \brief Defines the function calls and structures for parsing Seagate combine FARM logs SAS and SATA

#include "CFarm_Combine.h"

using namespace opensea_parser;
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::CFarm_Combine()
	:bufferData()
	, m_LogSize(0)
	, m_status(IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_shwoStatus(false)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::CFarm_Combine(bool showStatus)
	:bufferData()
	, m_LogSize(0)
	, m_status(IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_shwoStatus(showStatus)
{

}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::CFarm_Combine(uint8_t* bufferData, size_t bufferSize, bool showStatus)
	:bufferData(bufferData)
	, m_LogSize(bufferSize)
	, m_status(IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_shwoStatus(showStatus)
{
	is_Combo_Log();
	combine_Device_Scsi();
}
//-----------------------------------------------------------------------------
//
//! \fn CFarm_Combine::CFarm_Combine()
//
//! \brief
//!   Description: Class Deconstructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarm_Combine::~CFarm_Combine()
{
	if (bufferData != NULL)
	{
		//delete[] bufferData;
	}
}
void CFarm_Combine::setCombine(uint8_t* bufferData, size_t bufferSize)
{
	setBuffer(bufferData); 
	setLogSize(bufferSize); 
	is_Combo_Log();
	combine_Device_Scsi();
	
}
//-----------------------------------------------------------------------------
//
//! \fn combine_Device_Scsi()
//
//! \brief
//!   Description:  check the first part of the buffer to see if it's a scsi log or sata \n
//!                 look at offset 0x18 for information
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::combine_Device_Scsi()
{
	uint32_t device = M_BytesTo4ByteValue(bufferData[0x18], bufferData[0x19], bufferData[0x1a], bufferData[0x1b]);
	if (device == FARMSAS)
	{
		m_combine_isScsi = true;
	}
	else
	{
		m_combine_isScsi = false;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn is_Device_Scsi()
//
//! \brief
//!   Description:  check the first part of the buffer to see if it's a scsi log or sata log
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::is_Device_Scsi()
{
	if (bufferData != NULL)
	{
		if (M_GETBITRANGE(bufferData[0], 5, 0) == 0x3D)
		{
			if (bufferData[1] == 03 || bufferData[1] == 04 || (bufferData[1] >= FARM_TIME_SERIES_0 && bufferData[1] <= FARM_TEMP_TRIGGER_LOG_PAGE))
			{
				m_isScsi = true;
			}
		}
	}
	m_isScsi = false;
}
//-----------------------------------------------------------------------------
//
//! \fn is_Combo_Log()
//
//! \brief
//!   Description:  check the log to see if it is a combo log or not
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::is_Combo_Log()
{
	uint64_t stx = M_BytesTo8ByteValue(bufferData[0], bufferData[1], bufferData[2], bufferData[3], bufferData[4], bufferData[5], bufferData[6], bufferData[7]);
	uint64_t combo = M_BytesTo8ByteValue(bufferData[8], bufferData[9], bufferData[10], bufferData[11], bufferData[12], bufferData[13], bufferData[14], bufferData[15]);
	if (stx == STX_FARM && combo == STX_COMBO)
	{
		m_isComboLog = true;
	}
	else
	{
		m_isComboLog = false;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_FARM_Reason_For_Capture()
//
//! \brief
//!   Description:  From the farm log Reason the FARM log was capture 
//
//  Entry:
//! \param headerNode - string for the information to be placed
//! \param flag - the uint8 value from the buffer
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_FARM_Type(std::string* reason, uint64_t dataType)
{
	if (dataType == FARM_CURRENT)
	{
		*reason = "Current FARM Log";
	}
	else if (dataType == FARM_TIME)
	{
		*reason = "Time Series FARM Log";
	}
	else if (dataType == FARM_SAVE)
	{
		*reason = "Saved FARM Log";
	}
	else if (dataType == FARM_FACTORY)
	{
		*reason = "Factory FARM Log";
	}
	else if (dataType == FARM_STICKY)
	{
		*reason = "Sticky FARM Log";
	}
	else if (dataType == FARM_WLDTR)
	{
		*reason = "FARM Work Load Log";
	}
	else
	{
		*reason = "unknown";
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Header_Info()
//
//! \brief
//!   Description:  Get the header information from the combined FARM log
//
//  Entry:
//! \param offset - offset of for the buffer 
//! \param headerInfo - pointer to the idenify data structure
//
//
//  Exit:
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Header_Info(sStringIdentifyData* headerInfo)
{
	
#define VERSION_OFFSET 16
#define DEVICE_OFFSET 24
#define MODELNUMBER_OFFSET 32
#define SERIALNUMBER_OFFSET 80
#define FIRMWARE_OFFSET 104

	uint64_t offset = MODELNUMBER_OFFSET;							//!< init the offset to MODELNUMBER_OFFSET for later

	uint64_t comboVer = M_BytesTo8ByteValue(bufferData[VERSION_OFFSET + 7], bufferData[VERSION_OFFSET + 6], bufferData[VERSION_OFFSET + 5], bufferData[VERSION_OFFSET + 4], bufferData[VERSION_OFFSET + 3], bufferData[VERSION_OFFSET + 2], bufferData[VERSION_OFFSET + 1], bufferData[VERSION_OFFSET]);
	uint32_t devInt = M_BytesTo4ByteValue(bufferData[DEVICE_OFFSET + 3], bufferData[DEVICE_OFFSET + 2], bufferData[DEVICE_OFFSET + 1], bufferData[DEVICE_OFFSET]);
	create_Device_Interface_String_Flat(headerInfo->deviceInterface, &devInt);
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("Interface Type:				%s \n", headerInfo->deviceInterface.c_str());
	}

	uint64_t modelNumber = M_BytesTo8ByteValue(bufferData[offset + 7], bufferData[offset + 6], bufferData[offset + 5], bufferData[offset + 4], bufferData[offset + 3], bufferData[offset + 2], bufferData[offset + 1], bufferData[offset]);
	offset = offset + 8;
	uint64_t modelNumber2 = M_BytesTo8ByteValue(bufferData[offset + 7], bufferData[offset + 6], bufferData[offset + 5], bufferData[offset + 4], bufferData[offset + 3], bufferData[offset + 2], bufferData[offset + 1], bufferData[offset]);
	create_Model_Number_String_Flat(headerInfo->modelNumber, &modelNumber);
	create_Model_Number_String_Flat(headerInfo->modelNumber, &modelNumber2);
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("Model Number:				%s \n", headerInfo->modelNumber.c_str());
	}

	offset = SERIALNUMBER_OFFSET;
	uint64_t serialNumber = M_BytesTo8ByteValue(bufferData[offset + 7], bufferData[offset + 6], bufferData[offset + 5], bufferData[offset + 4], bufferData[offset + 3], bufferData[offset + 2], bufferData[offset + 1], bufferData[offset]);
	offset = offset + 8;
	uint64_t serialNumber2 = M_BytesTo8ByteValue(bufferData[offset + 7], bufferData[offset + 6], bufferData[offset + 5], bufferData[offset + 4], bufferData[offset + 3], bufferData[offset + 2], bufferData[offset + 1], bufferData[offset]);
	create_Flat_SN(headerInfo->serialNumber, &serialNumber, &serialNumber2);
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("Serial Number:				%s \n", headerInfo->serialNumber.c_str());
	}

	offset = FIRMWARE_OFFSET;
	uint32_t firwareStr = M_BytesTo4ByteValue(bufferData[offset + 3], bufferData[offset + 2], bufferData[offset + 1], bufferData[offset]);
	offset += 4;
	uint32_t firwareStr2 = M_BytesTo4ByteValue(bufferData[offset +3], bufferData[offset + 2], bufferData[offset + 1], bufferData[offset]);
	create_Firmware_String_Flat(headerInfo->firmwareRev, &firwareStr, &firwareStr2);
	if (VERBOSITY_COMMAND_VERBOSE <= g_verbosity)
	{
		printf("Firmware Version:			%s \n", headerInfo->firmwareRev.c_str());
	}
}
//-----------------------------------------------------------------------------
//
//! \fn get_Data_Set()
//
//! \brief
//!   Description:  get the data set from the combined FARM log. Fills in the \n
//!  vdataSetInfo vector
//
//  Entry:
//! \param DataSetNumber - the number of data sets to look for
//
//
//  Exit:
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Data_Set(uint16_t DataSetNumber)
{
	vdataSetInfo.reserve(DataSetNumber);
	uint32_t offset = 0x100;
	for (uint16_t i = 0; i < DataSetNumber; i++)
	{

		sComboDataSet set;
		set = *(reinterpret_cast<sComboDataSet*>(&bufferData[offset]));
		byte_Swap_64(&set.dataSetType);
		vdataSetInfo.push_back(set);
		offset += sizeof(sComboDataSet);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn parse_FARM_Logs()
//
//! \brief
//!   Description:  get the data set from the combined FARM log. Fills in the \n
//!  vdataSetInfo vector
//
//  Entry:
//! \param offset - what the starting offset is
//
//
//  Exit:
//
//---------------------------------------------------------------------------
void CFarm_Combine::parse_FARM_Logs(uint64_t offset, size_t logSize, uint64_t dataType, JSONNODE* masterJson)
{
	if (m_combine_isScsi)
	{
		std::string temp;
		uint8_t subpage = bufferData[offset + 1];
		JSONNODE* farmInfo = json_new(JSON_NODE);
		get_FARM_Type(&temp,dataType);
		json_set_name(farmInfo, temp.c_str());

		CSCSI_Farm_Log* pCFarm;
		pCFarm = new CSCSI_Farm_Log(&bufferData[offset], logSize, subpage, m_shwoStatus);
		if (pCFarm->get_Log_Status() == SUCCESS)
		{
			pCFarm->print_All_Pages(farmInfo);
			m_status = SUCCESS;
		}
		else
		{
			m_status = pCFarm->get_Log_Status();
		}
		delete(pCFarm);
		json_push_back(masterJson, farmInfo);
	}
	else
	{
		CATA_Farm_Log* pCFarm;
		pCFarm = new CATA_Farm_Log(&bufferData[offset], logSize, m_shwoStatus);
		if (pCFarm->get_Log_Status() == IN_PROGRESS)
		{
			try
			{
				m_status = pCFarm->parse_Farm_Log();
				if (m_status == IN_PROGRESS)
				{
					pCFarm->print_All_Pages(masterJson);
					m_status = SUCCESS;
				}
			}
			catch (...)
			{
				delete (pCFarm);
				m_status = MEMORY_FAILURE;
			}

		}
		delete (pCFarm);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn combo_Parsing()
//
//! \brief
//!   Description:  parse out the Farm Log into a vector called Farm Frame.
//
//  Entry:
//! \param pData - pointer to the buffer
//
//  Exit:
//!   \return SUCCESS or FAILURE
//
//---------------------------------------------------------------------------
void CFarm_Combine::combo_Parsing(JSONNODE* masterJson)
{
	uint64_t offset = 0;
	sStringIdentifyData* headerInfo = new sStringIdentifyData();
	get_Header_Info(headerInfo);

	uint16_t dataSets = M_BytesTo2ByteValue(bufferData[253], bufferData[252]);
	get_Data_Set(dataSets);

	
	for (std::vector<sComboDataSet>::iterator dataItr = vdataSetInfo.begin(); dataItr != vdataSetInfo.end(); ++dataItr)
	{
		if (dataItr->dataSetType != FARM_WLDTR)
		{
			parse_FARM_Logs(dataItr->location,dataItr->dataSize, dataItr->dataSetType, masterJson);
		}
	}


}
