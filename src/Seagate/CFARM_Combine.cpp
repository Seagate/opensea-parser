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

#include "CFARM_Combine.h"

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
	, m_version(0)
	, m_maxLogSize(0)
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
	, m_version(0)
	, m_maxLogSize(0)
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
CFarm_Combine::CFarm_Combine(uint8_t* buffer, size_t bufferSize, bool showStatus)
	:bufferData(buffer)
	, m_LogSize(bufferSize)
	, m_status(IN_PROGRESS)
	, m_isScsi(false)
	, m_combine_isScsi(false)
	, m_isComboLog(false)
	, m_shwoStatus(showStatus)
	, m_version(0)
	, m_maxLogSize(0)
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
//-----------------------------------------------------------------------------
//
//! \fn setCombine()
//
//! \brief
//!   Description:  setup the combine class by settting the buffer, logsize, checking if combo file, checking if sas
//
//  Entry:
//
//  Exit:
//!   \return bool = True or False
//
//---------------------------------------------------------------------------
void CFarm_Combine::setCombine(uint8_t* buffer, size_t bufferSize)
{
	setBuffer(buffer); 
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
//! \fn get_FARM_Type()
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
		*reason = "Long Term Saved FARM Log";
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
//! \fn is_Subpage_Neeeded()
//
//! \brief
//!   Description: Check the dataType to see if we need to create a subpage for the Json data
//
//  Entry:
//! \param dataType - pointer to the dataType in sComboDataSet structure
//
//
//  Exit:
//!   \return bool - true if we need a subpage in the JSON
//
//---------------------------------------------------------------------------
bool CFarm_Combine::is_Subpage_Neeeded(uint64_t* dataType)
{
	if (*dataType == FARM_TIME)						// FARM TimeSeries Frame
	{
		return true;
	}
	if (*dataType == FARM_STICKY)					// FARM Sticky Frame
	{
		return true;
	}
	if (*dataType == FARM_SAVE)						// Farm Saved Frame
	{
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
//
//! \fn get_Sticky_Frame_Type()
//
//! \brief
//!   Description: get the sticky frame names 
//
//  Entry:
//! \param reason - pointer to the string 
//! \param frame - the frame number  (note we have to add 3 to get the right names)
//
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Sticky_Frame_Type(std::string* reason, uint8_t frame)
{
	switch (frame+3)
	{
	case GLIST_DISC_ENTRIES_FRAME:
		*reason = "1000 g-list disc entries frame";
		break;
	case FIRST_UNRECOVERED_READ_ERROR_FRAME:
		*reason = "1st unrecovered read error";
		break;
	case TENTH_UNRECOVERED_READ_ERROR_FRAME:
		*reason = "10th unrecovered read error";
		break;
	case FIRST_FATAL_CTO_FRAME:
		*reason = "1st fatal command time out";
		break;
	case BEFORE_CFW_UPDATE_FRAME:
		*reason = "last frame prior to most recent CFW or SFW update";
		break;
	case TEMP_EXCEDED_FRAME:
		*reason = "temperature exceeds 70 degress celsius";
		break;
	default:
		*reason = "Unknown Frame Type";
		break;
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
//!   \return void
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

	m_version = M_BytesTo8ByteValue(bufferData[VERSION_OFFSET], bufferData[VERSION_OFFSET + 1], bufferData[VERSION_OFFSET + 2], bufferData[VERSION_OFFSET + 3], bufferData[VERSION_OFFSET + 4], bufferData[VERSION_OFFSET + 5], bufferData[VERSION_OFFSET + 6], bufferData[VERSION_OFFSET + 7]);
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
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::get_Data_Set(uint16_t DataSetNumber)
{
#define DATAOFFSET  0x100
	vdataSetInfo.reserve(DataSetNumber);
	uint32_t offset = DATAOFFSET;
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
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::parse_FARM_Logs(size_t offset, size_t logSize, uint64_t dataType, JSONNODE* farmJson)
{
	size_t page = 0;
	for (size_t counter = 0; counter <= logSize;)
	{
		if (m_combine_isScsi)
		{
			uint8_t subpage = bufferData[offset + 1];
			std::ostringstream temp;
			temp << "Subpage 0x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<uint16_t> (subpage);
			JSONNODE* farmInfo = json_new(JSON_NODE);
			json_set_name(farmInfo, temp.str().c_str());

			CSCSI_Farm_Log* pCFarm;
			pCFarm = new CSCSI_Farm_Log(&bufferData[offset], logSize, subpage, m_shwoStatus);
			if (pCFarm->get_Log_Status() == SUCCESS)
			{
				page = static_cast<size_t>(pCFarm->get_LogSize()) + 4;      // need add in the param length back in
				if (page > m_maxLogSize)
				{
					m_status = INVALID_LENGTH;
				}
				else
				{
					pCFarm->print_All_Pages(farmInfo);
					m_status = SUCCESS;
				}
			}
			else
			{
				m_status = pCFarm->get_Log_Status();
			}
			delete(pCFarm);
			json_push_back(farmJson, farmInfo);
		}
		else
		{

			CATA_Farm_Log* pCFarm;
			pCFarm = new CATA_Farm_Log(&bufferData[offset], logSize, m_shwoStatus);
			if (pCFarm->get_Log_Status() == IN_PROGRESS)
			{
				try
				{
					bool subFrame = is_Subpage_Neeeded(&dataType);  // check to see if the subpage is needed

					JSONNODE* farmInfo;
					if (!subFrame)
					{
						farmInfo = farmJson;
					}
					else
					{
						std::string reason;
						farmInfo = json_new(JSON_NODE);
						json_set_name(farmInfo, reason.c_str());
					}
					
					m_status = pCFarm->parse_Farm_Log();
					if (m_status == IN_PROGRESS)
					{
						page = static_cast<size_t>(pCFarm->get_LogSize());
						if (page == EMPTYLOGSIZE || page == PADDINGSIZE)
						{
							page = m_maxLogSize;
							pCFarm->print_All_Pages(farmInfo);
							m_status = SUCCESS;
						}
						else if (page > m_maxLogSize)
						{
							m_status = INVALID_LENGTH;
						}
						else
						{
							pCFarm->print_All_Pages(farmInfo);
							m_status = SUCCESS;
						}
					}
					if (subFrame)
					{
						json_push_back(farmJson, farmInfo);
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
		counter += page;
		// check do to buffer padding of zero's
		if (counter + page > logSize || m_status != SUCCESS)
		{
			break;
		}
		offset += page;
	}
}

//-----------------------------------------------------------------------------
//
//! \fn print_Combine_Log_Header()
//
//! \brief
//!   Description:  print out the header information sn, firmware rev, interface, model and version number
//
//  Entry:
//! \param sStringIdentifyData - structue from the identifydata for the drive
//! \param labelJson - json node to add the data to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::print_Combine_Log_Header(sStringIdentifyData* headerInfo, JSONNODE* header)
{
	JSONNODE* label = json_new(JSON_NODE);
	json_set_name(label, "FARM Combined Labels");
	json_push_back(label, json_new_a("serial_number", headerInfo->serialNumber.c_str()));
	json_push_back(label, json_new_a("firmware_rev", headerInfo->firmwareRev.c_str()));
	json_push_back(label, json_new_a("Drive Interface", headerInfo->deviceInterface.c_str()));
	json_push_back(label, json_new_a("Model Number", headerInfo->modelNumber.c_str()));
	std::string verStr;
	create_Version_Number(verStr, &m_version);
	json_push_back(label, json_new_a("FARM Combined version", verStr.c_str()));
	json_push_back(header, label);
}
//-----------------------------------------------------------------------------
//
//! \fn print_Header_Debug()
//
//! \brief
//!   Description:  print out the header information on where everything is located
//
//  Entry:
//! \param labelJson - json node to add the data to
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarm_Combine::print_Header_Debug( JSONNODE* labelJson)
{
#if defined _DEBUG
	std::ostringstream temp;
	std::string dataStr = "Debug Combo Data Set";
	JSONNODE* label = json_new(JSON_ARRAY);
	json_set_name(label, dataStr.c_str());

	for (std::vector<sComboDataSet>::iterator dataItr = vdataSetInfo.begin(); dataItr != vdataSetInfo.end(); ++dataItr)
	{
		JSONNODE* dblabel = json_new(JSON_NODE);
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->dataSetType;
		json_push_back(dblabel, json_new_a("Type", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->dataSize;
		json_push_back(dblabel, json_new_a("Size", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->startTime;
		json_push_back(dblabel, json_new_a("start time", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->endTime;
		json_push_back(dblabel, json_new_a("end time", temp.str().c_str()));
		temp.str(""); temp.clear();
		temp << "0x" << std::hex << std::nouppercase << dataItr->location;
		json_push_back(dblabel, json_new_a("location", temp.str().c_str()));
		json_push_back(label, dblabel);
	}

	json_push_back(labelJson, label);
#endif
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
eReturnValues CFarm_Combine::combo_Parsing(JSONNODE* masterJson)
{
	eReturnValues retStatus = IN_PROGRESS;
	// get the drives identify data from the log
	sStringIdentifyData* headerInfo = new sStringIdentifyData();
	get_Header_Info(headerInfo);
	print_Combine_Log_Header(headerInfo, masterJson);
	
	// get the number of data sets in the log
	uint16_t dataSets = M_BytesTo2ByteValue(bufferData[253], bufferData[252]);
	get_Data_Set(dataSets);
	//set the max logs size from the data set info 
	m_maxLogSize = vdataSetInfo.at(0).dataSize;

	// if debug then print the header information - location, time, size.
	print_Header_Debug(masterJson);	

	if (dataSets > 1)
	{
		for (std::vector<sComboDataSet>::iterator dataItr = vdataSetInfo.begin(); dataItr != vdataSetInfo.end(); ++dataItr)
		{
			if (dataItr->dataSetType != FARM_WLDTR)
			{
				if (dataItr->dataSetType == FARM_TIME || dataItr->dataSetType == FARM_SAVE || dataItr->dataSetType == FARM_STICKY)
				{
					JSONNODE* farmInfo = json_new(JSON_ARRAY);
					std::string typeStr;
					get_FARM_Type(&typeStr, dataItr->dataSetType);
					json_set_name(farmInfo, typeStr.c_str());
					parse_FARM_Logs(dataItr->location, dataItr->dataSize, dataItr->dataSetType, farmInfo);
					json_push_back(masterJson, farmInfo);
				}
				else
				{
					JSONNODE* farmInfo = json_new(JSON_NODE);
					std::string typeStr;
					get_FARM_Type(&typeStr, dataItr->dataSetType);
					json_set_name(farmInfo, typeStr.c_str());
					parse_FARM_Logs(dataItr->location, dataItr->dataSize, dataItr->dataSetType, farmInfo);
					json_push_back(masterJson, farmInfo);
				}
			}
			else
			{
				CFARMWLM* cWLM;
				cWLM = new CFARMWLM(&bufferData[dataItr->location], dataItr->dataSize, masterJson);
				retStatus = cWLM->get_WLM_Status();

				delete(cWLM);
			}
		}

	}
	return retStatus;
}
