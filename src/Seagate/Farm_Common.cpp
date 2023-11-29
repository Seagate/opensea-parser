//
// Farm_Common.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2023 Seagate Technology LLC and/or its Affiliates
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************

// \file Farm_Common.cpp  
//
#include "Farm_Common.h"

using namespace opensea_parser;

//-----------------------------------------------------------------------------
//
//! \fn FarmCommon
//
//! \brief
//!   Description: default Class constructor
//
//  Entry:
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
CFarmCommon::CFarmCommon()
{

}
//-----------------------------------------------------------------------------
//
//! \fn CFarmCommon
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
CFarmCommon::~CFarmCommon()
{

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
bool CFarmCommon::is_Device_Scsi(uint8_t buff0, uint8_t buff1)
{
	if (M_GETBITRANGE(buff0, 5, 0) == 0x3D)
	{
		if (buff1 == FARM_LOG_PAGE || buff1 == FARM_FACTORY_LOG_PAGE || (buff1 >= FARM_TIME_SERIES_0 && buff1 <= FARM_TEMP_TRIGGER_LOG_PAGE))
		{
			return true;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------
//
//! \fn create_Flat_SN()
//
//! \brief
//!   Description:  takes the two uint64 bit seiral number values and create a string serial number
//
//  Entry:
//! \param serialNumberStr - pointer to the serial number, where once constructed, will hold the serial number of the drive
//! \param serialNumber  =  first part of the serial number.
//! \param serialNumber2  =  second part of the serial number.
//
//  Exit:
//!   \return serialNumber = the string serialNumber
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Flat_SN(std::string& serialNumberStr, uint64_t* serialNumber, uint64_t* serialNumber2)
{
	serialNumberStr.clear();
	//byte_Swap_64(serialNumber);
	serialNumberStr.assign(reinterpret_cast<const char*>(serialNumber), SERIAL_NUMBER_LEN);
	//byte_Swap_64(serialNumber2);
	serialNumberStr.append(reinterpret_cast<const char*>(serialNumber2), SERIAL_NUMBER_LEN);
	remove_trailing_whitespace_std_string(serialNumberStr);
}
//-----------------------------------------------------------------------------
//
//! \fn create_Serial_Number()
//
//! \brief
//!   Description:  takes the two uint32 bit seiral number values and create a string serial number
//
//  Entry:
//! \param serialNumberStr - pointer to the serial number, where once constructed, will hold the serial number of the drive
//! \param serialNumber  =  first part of the serial number.
//! \param serialNumber2  =  second part of the serial number.
//! \param majorRev  =  the major Rev infomration.
//! \param sas = True if sas drive
//
//  Exit:
//!   \return serialNumber = the string serialNumber
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Serial_Number(std::string& serialNumberStr, uint32_t serialNumber, uint32_t serialNumber2, uint32_t majorRev, bool sas = false  )
{
	uint64_t sn = 0;
	uint64_t sn1 = serialNumber;
	uint64_t sn2 = serialNumber2;
	if (sas)
	{
		if (majorRev < MAJORVERSION3)
		{
			sn = (sn2 << 32 | (sn1));
			byte_Swap_64(&sn);
		}
		else
		{
			sn = (sn1 | (sn2 << 32));
			byte_Swap_64(&sn);
		}
	}
	else  // sata
	{
		sn = (sn1 | (sn2 << 32));
	}
	serialNumberStr.clear();
    serialNumberStr.assign(reinterpret_cast<const char*>(&sn), SERIAL_NUMBER_LEN);
	if (!sas)   // sata we need to byte swap the string
	{
		byte_swap_std_string(serialNumberStr);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn create_Model_Number_String()
//
//! \brief
//!   Description:  fill in the model number of the drive 
//
//  Entry:
//! \param modelStr - pointer to the  model number of the drive
//! \param productID  =  pointer to the drive info structure that holds the infromation needed
//! \param sas = True if sas drive
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Model_Number_String(std::string& modelStr, uint64_t *productID, bool sas = false)
{
#define MAXSIZE  5
	uint32_t modelParts[MAXSIZE] = { 0,0,0,0,0 };
	// need  to clear the modelStr for timeseries 
	modelStr.clear();
	// loop for and get the information from the lower bits
	for (uint8_t i = 0; i < MAXSIZE; i++)
	{
		if (sas)
			modelParts[i] = M_DoubleWord1(productID[i]);
		else // sata need to be the dword 0
		{
			modelParts[i] = M_DoubleWord0(productID[i]);
		}
		modelStr.append(reinterpret_cast<const char*>(&modelParts[i]), sizeof(uint32_t));
	}
	if (!sas)   // sata we need to byte swap the string
	{
		byte_swap_std_string(modelStr);
	}
	remove_trailing_whitespace_std_string(modelStr);

}
//-----------------------------------------------------------------------------
//
//! \fn create_Model_Number_String_Flat()
//
//! \brief
//!   Description:  fill in the model number of the drive 
//
//  Entry:
//! \param modelStr - pointer to the  model number of the drive
//! \param productID  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Model_Number_String_Flat(std::string& modelStr, uint64_t* productID)
{
	//byte_Swap_64(productID);
	modelStr.append(reinterpret_cast<const char*>(productID), sizeof(uint64_t));
	remove_trailing_whitespace_std_string(modelStr);
}
//-----------------------------------------------------------------------------
//
//! \fn create_Device_Interface_String()
//
//! \brief
//!   Description:  takes the two uint64 bit Devie interface string values and create a string device interface  
//
//  Entry:
//! \param dInterfaceStr - pointer to the Devie interface v, where once constructed, will hold the Devie interface of the drive
//! \param deviceInterface  =  pointer to the drive info structure that holds the infromation needed
//! \param sas = True if sas drive
//
//  Exit:
//!   \return dInterface = the string dInterface
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Device_Interface_String(std::string& dInterfaceStr, uint32_t deviceInterface, bool sas = false)
{
	uint32_t dFace = 0;
	dFace = deviceInterface;
	if (!sas)
	{
		byte_Swap_32(&dFace);
	}
	dInterfaceStr = "0000";
	dInterfaceStr.assign(reinterpret_cast<const char*>(&dFace), DEVICE_INTERFACE_LEN);
}
//-----------------------------------------------------------------------------
//
//! \fn create_Device_Interface_String_Flat()
//
//! \brief
//!   Description:  takes the two uint64 bit Devie interface string values and create a string device interface  
//
//  Entry:
//! \param dInterfaceStr - pointer to the Devie interface v, where once constructed, will hold the Devie interface of the drive
//! \param deviceInterface  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return dInterface = the string dInterface
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Device_Interface_String_Flat(std::string& dInterfaceStr, uint32_t* deviceInterface)
{
	dInterfaceStr.assign(reinterpret_cast<const char*>(deviceInterface), DEVICE_INTERFACE_LEN);
	remove_trailing_whitespace_std_string(dInterfaceStr);
}
//-----------------------------------------------------------------------------
//
//! \fn create_World_Wide_Name()
//
//! \brief
//!   Description:  takes the two uint64 bit world wide name values and create a string world wide name 
//
//  Entry:
//! \param worldWideName - pointer to the world wide name, where once constructed, will hold the world wide name of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//! \param sas = True if sas drive
//
//  Exit:
//!   \return wordWideName = the string wordWideName
//
//---------------------------------------------------------------------------
void CFarmCommon::create_World_Wide_Name(std::string& worldWideName, uint64_t wwn, uint64_t wwn2, bool sas =false)
{
	uint64_t wwnFinal = 0;
	uint32_t stwwn = M_DoubleWord0(wwn);
	uint32_t stwwn2 = M_DoubleWord0(wwn2);
	if (!sas)
	{
		word_Swap_32(&stwwn);
		word_Swap_32(&stwwn2);
	}
	wwnFinal = M_DWordsTo8ByteValue(stwwn2, stwwn);
	worldWideName = "0000000000000000000";
	std::ostringstream temp;
    temp << "0x" << std::hex << std::uppercase << wwnFinal;
    worldWideName.assign(temp.str());
}
//-----------------------------------------------------------------------------
//
//! \fn create_Firmware_String()
//
//! \brief
//!   Description:  takes the two uint64 bit firmware Rev values and create a string firmware Rev 
//
//  Entry:
//! \param firmwareRevStr - pointer to the firmware Rev, where once constructed, will hold the firmware Rev of the drive
//! \param idInfo  =  pointer to the drive info structure that holds the infromation needed
//! \param sas = True if sas drive
//
//  Exit:
//!   \return firmwareRev = the string firmwareRev
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Firmware_String(std::string& firmwareRevStr, uint32_t firmware, uint32_t firmware2, bool sas = false)
{
	uint64_t firm = 0;
	uint64_t firm1 = firmware;
	uint64_t firm2 = firmware2;
	if (sas)
	{
		firm = (firm2 | (firm1 << 32));
		byte_Swap_64(&firm);
	}
	else
	{
		firm = firmware;
	}
	firmwareRevStr = "00000000";
	firmwareRevStr.assign(reinterpret_cast<const char*>(&firm), 8);
	if (!sas)
	{
		byte_swap_std_string(firmwareRevStr);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn create_Firmware_String_Flat()
//
//! \brief
//!   Description:  takes the two uint64 bit firmware Rev values and create a string firmware Rev 
//
//  Entry:
//! \param firmwareRevStr - pointer to the firmware Rev, where once constructed, will hold the firmware Rev of the drive
//! \param firmware  =  pointer to the drive info structure that holds the infromation needed
//! \param firmware2  =  pointer to the drive info structure that holds the infromation needed
//
//  Exit:
//!   \return firmwareRev = the string firmwareRev
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Firmware_String_Flat(std::string& firmwareRevStr, uint32_t *firmware, uint32_t *firmware2)
{
	firmwareRevStr.assign(reinterpret_cast<const char*>(firmware), sizeof(uint32_t));
	firmwareRevStr.append(reinterpret_cast<const char*>(firmware2), sizeof(uint32_t));
	remove_trailing_whitespace_std_string(firmwareRevStr);
}
//-----------------------------------------------------------------------------
//
//! \fn create_Version_Number()
//
//! \brief
//!   Description:  create the version number for the hex information
//
//  Entry:
//! \param version - pointer to the version string that the function will create
//! \param versionID  =  hex information for the vesion ID number
//
//  Exit:
//!   \return none
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Version_Number(std::string &version, uint64_t versionID)
{
	uint8_t minor = M_Byte0(versionID);
	uint8_t major = M_Byte1(versionID);
	version = "0000000";
	std::ostringstream temp;
	temp << "" << std::dec << std::setw(1) << static_cast<uint16_t>(major) << "."  << static_cast<uint16_t>(minor);
	version.assign(temp.str());
}
//-----------------------------------------------------------------------------
//
//! \fn create_Year_Assembled_String()
//
//! \brief
//!   Description:  fill in the date string from the date. used for year and week assembled
//
//  Entry:
//! \param dateStr - pointer to the date string
//! \param date  =  pointer to the date data
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::create_Year_Assembled_String(std::string &dateStr, uint16_t date, bool isSAS)
{
	if (isSAS)
	{
		byte_Swap_16(&date);
	}
	if (date >= 0xFFFF)
	{
		dateStr = "00";
	}
	else
	{
        dateStr.assign(reinterpret_cast<const char*>(&date), sizeof(uint16_t));
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
void CFarmCommon::Get_FARM_Reason_For_Capture(std::string* reason, uint8_t flag)
{
	switch (flag)
	{
	case CURRENT_FRAME:
		*reason = "current FARM log";
		break;
	case TIME_SERIES_FRAME:
		*reason = "time series frame";
		break;
	case LONG_TERM_FRAME:
		*reason = "long term frame";
		break;
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
	case RESERVED_FRAME:
		*reason = "reserved";
		break;
	case FACTORY_COPY_FRAME:
		*reason = "FARM factory frame copy";
		break;
	case FARM_LOG_DISC_COPY_FRAME:
		*reason = "FARM log disc copy";
		break;
	case IDD_FARM_LOG_DISC_FRAME:
		*reason = "IDD FARM log disc copy";
		break;
	case FARM_DRAM_COPY_FARME:
		*reason = "FARM log DRAM copy";
		break;
	case FARM_DRAM_COPY_DIAG_FRAME:
		*reason = "FARM log DRAM copy via diag command";
		break;
	case FARM_UDS_COPY_FRAME:
		*reason = "FARM log via UDS";
		break;
	case FARM_EMPTY_FRAME:
		*reason = "Empty FARM Log";
		break;
	default:
		*reason = "unknown";
		break;
	}
}
