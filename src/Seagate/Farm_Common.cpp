//
// Farm_Common.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2024 Seagate Technology LLC and/or its Affiliates
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
		if (firmware != 0 && firmware != 0x20202020)
		{
			firm = firmware;
		}
		else
		{
			firm = firmware2;
		}
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
		*reason = "DRAM Copy";
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
	case FARM_NEURAL_NETWORK:
		*reason = "FARM Neural Network Data";
		break;
	case FARM_AFTER_REGEN:
		*reason = "FARM saved after a successful Regen";
		break;
	case FARM_EMPTY_FRAME:
		*reason = "Empty FARM Log";
		break;
	default:
		*reason = "unknown";
		break;
	}
}
//-----------------------------------------------------------------------------
//
//! \fn floatHeadData()
//
//! \brief
//!   Description:  Takes a float data and creates the JSON information in a NODE or array depending on showStatic
//
//  Entry:
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//! \param showStatic = true it will print the static json node way
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::floatHeadData(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		float_NODE_Data(Node, title, value, heads, showStatusBits);
	}
	else
	{
		float_Array_Data(Node, title, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn float_NODE_Data()
//
//! \brief
//!   Description:  Takes a fload data and creates the JSON information in a NODE
//
//  Entry:
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_NODE_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                  // write out the title plus the Head count
		uint64_t dsHead = check_Status_Strip_Status(value[loopCount]);
		uint8_t negCheck = M_Byte6(value[loopCount]);									// get 6 or checking for negitive number
		int16_t whole = M_WordInt2(dsHead);							                    // get 5:4 whole part of the float
		double decimal = static_cast<double>(M_DoubleWord0(dsHead));					// get 3:0 for the Deciaml Part of the float
		number = 0.0;
		// check bit 1 of Byte 6 for negitive number
		if (negCheck & BIT1)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}
		set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn float_Array_Data()
//
//! \brief
//!   Description:  Takes a fload data and creates the JSON information into an array
//
//  Entry:
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Array_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	double number = 0.0;
	JSONNODE* header = json_new(JSON_ARRAY);
	json_set_name(header, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		uint64_t dsHead = check_Status_Strip_Status(value[loopCount]);
		uint8_t negCheck = M_Byte6(value[loopCount]);										// get 6 or checking for negitive number
		int16_t whole = M_WordInt2(dsHead);													// get 5:4 whole part of the float
		double decimal = static_cast<double>(M_DoubleWord0(dsHead));						// get 3:0 for the Deciaml Part of the float
		number = 0.0;						
		// check bit 1 of Byte 6 for negitive number
		if (negCheck & BIT1)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}

		set_json_float_With_Status(header, title, number, value[loopCount], showStatusBits);
	}
	json_push_back(Node, header);
}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeightData()
//
//! \brief
//!   Description:  Takes a sflyHeight data array and creates the JSON information in a NODE or array depending on showStatic
//
//  Entry:
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the sflyheight
//! \param sizeAmount = defined size for word, word int, dword 0 ore dword 1 
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//! \param showStatic = true it will print the static json node way
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sflyHeightData(JSONNODE* Node, const std::string& title, double calculation, sflyHeight* value, int track, int sizeAmount, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		sflyHeight_Node_Data(Node, title, calculation, value, track, sizeAmount, heads, showStatusBits);
	}
	else
	{
		sflyHeight_Array_Data(Node, title, calculation, value, track, sizeAmount, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeight_Node_Data()
//
//! \brief
//!   Description:  Takes a sflyHeight data array and creates the JSON information in a static NODE.
//
//  Entry:
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the sflyheight
//! \param sizeAmount = defined size for word, word int, dword 0 ore dword 1 
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sflyHeight_Node_Data(JSONNODE* Node, const std::string& title, double calculation, sflyHeight* value, int track, int sizeAmount, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		double number = 0;
		switch (track)
		{
		case INNER:
			switch (sizeAmount)
			{
			case WORD0:
				number = static_cast<double>(M_Word0(value[loopCount].inner) * calculation);
                break;
			case WORDINT0:
				number = static_cast<double>(M_WordInt0(value[loopCount].inner) * calculation);
                break;
			case DWORD0:
				number = static_cast<double>(M_DoubleWord0(value[loopCount].inner) * calculation);
                break;
			default:
				number = static_cast<double>(M_WordInt0(value[loopCount].inner) * calculation);
                break;
			}
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].inner, showStatusBits);
			break;
		case OUTER:
			switch (sizeAmount)
			{
			case WORD0:
				number = static_cast<double>(M_Word0(value[loopCount].outer) * calculation);
                break;
			case WORDINT0:
				number = static_cast<double>(M_WordInt0(value[loopCount].outer) * calculation);
                break;
			case DWORD0:
				number = static_cast<double>(M_DoubleWord0(value[loopCount].outer) * calculation);
                break;
			default:
				number = static_cast<double>(M_WordInt0(value[loopCount].outer) * calculation);
                break;
			}
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].outer, showStatusBits);
			break;
		case MIDDLE:
			switch (sizeAmount)
			{
			case WORD0:
				number = static_cast<double>(M_Word0(value[loopCount].middle) * calculation);
                break;
			case WORDINT0:
				number = static_cast<double>(M_WordInt0(value[loopCount].middle) * calculation);
                break;
			case DWORD0:
				number = static_cast<double>(M_DoubleWord0(value[loopCount].middle) * calculation);
                break;
			default:
				number = static_cast<double>(M_WordInt0(value[loopCount].middle) * calculation);
                break;
			}
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].middle, showStatusBits);
			break;
		default:
			number = static_cast<double>(M_WordInt0(value[loopCount].inner) * calculation);
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].inner, showStatusBits);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeight_Array_Data()
//
//! \brief
//!   Description:  get the sFlyHeight array data
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the sflyheight
//! \param sizeAmount = defined size for word, word int, dword 0 ore dword 1 
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sflyHeight_Array_Data(JSONNODE* Node, const std::string& title, double calculation, sflyHeight* value, int track, int sizeAmount, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headerror = json_new(JSON_ARRAY);
	json_set_name(headerror, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		double number = 0;
		switch (track)
		{
		case INNER:
			switch (sizeAmount)
			{
			case WORD0:
				number = static_cast<double>(M_Word0(value[loopCount].inner) * calculation);
                break;
			case WORDINT0:
				number = static_cast<double>(M_WordInt0(value[loopCount].inner) * calculation);
                break;
			case DWORD0:
				number = static_cast<double>(M_DoubleWord0(value[loopCount].inner) * calculation);
                break;
			default:
				number = static_cast<double>(M_WordInt0(value[loopCount].inner) * calculation);
                break;
			}
			set_json_float_With_Status(headerror, title, number, value[loopCount].inner, showStatusBits);
			break;
		case OUTER:
			switch (sizeAmount)
			{
			case WORD0:
				number = static_cast<double>(M_Word0(value[loopCount].outer) * calculation);
                break;
			case WORDINT0:
				number = static_cast<double>(M_WordInt0(value[loopCount].outer) * calculation);
                break;
			case DWORD0:
				number = static_cast<double>(M_DoubleWord0(value[loopCount].outer) * calculation);
                break;
			default:
				number = static_cast<double>(M_WordInt0(value[loopCount].outer) * calculation);
                break;
			}
			set_json_float_With_Status(headerror, title, number, value[loopCount].outer, showStatusBits);
			break;
		case MIDDLE:
			switch (sizeAmount)
			{
			case WORD0:
				number = static_cast<double>(M_Word0(value[loopCount].middle) * calculation);
                break;
			case WORDINT0:
				number = static_cast<double>(M_WordInt0(value[loopCount].middle) * calculation);
                break;
			case DWORD0:
				number = static_cast<double>(M_DoubleWord0(value[loopCount].middle) * calculation);
                break;
			default:
				number = static_cast<double>(M_WordInt0(value[loopCount].middle) * calculation);
                break;
			}
			set_json_float_With_Status(headerror, title, number, value[loopCount].middle, showStatusBits);
			break;
		default:
			number = static_cast<double>(M_WordInt0(value[loopCount].inner) * calculation);
			set_json_float_With_Status(headerror, title, number, value[loopCount].inner, showStatusBits);
			break;
		}
	}
	json_push_back(Node, headerror);
}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeight_Float_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for sflyHeaight array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the sflyheight
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sflyHeight_Float_Data(JSONNODE* Node, const std::string& title, sflyHeight* value, int track, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		sflyHeight_Float_Node_Data(Node, title, value, track, heads, showStatusBits);
	}
	else
	{
		sflyHeight_Float_Array_Data(Node, title, value, track, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeight_Float_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for sflyHeaight array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the sflyheight
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sflyHeight_Float_Node_Data(JSONNODE* Node, const std::string& title, sflyHeight* value, int track, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	uint8_t negCheck = 0;												// check byte 6 bit 1 for negitive value
	uint64_t dsHead = 0;
	int16_t whole = 0;													// get 5:4 whole part of the float
	double decimal = 0.0;												// get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		negCheck = 0;													// check byte 6 bit 1 for negitive value
		dsHead = 0;
		whole = 0;														// get 5:4 whole part of the float
		decimal = 0.0;													// get 3:0 for the Deciaml Part of the float
		number = 0.0;

		switch (track)
		{
		case INNER:
			dsHead = check_Status_Strip_Status(value[loopCount].inner);
			negCheck = M_Byte6(value[loopCount].inner);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case OUTER:
			dsHead = check_Status_Strip_Status(value[loopCount].outer);
			negCheck = M_Byte6(value[loopCount].outer);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case MIDDLE:
			dsHead = check_Status_Strip_Status(value[loopCount].middle);
			negCheck = M_Byte6(value[loopCount].middle);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		}

		// if bit 1 is set number should be negitive
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}
		switch (track)
		{
		case INNER:
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].inner, showStatusBits);
			break;
		case OUTER:
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].outer, showStatusBits);
			break;
		case MIDDLE:
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].middle, showStatusBits);
			break;
		}

	}

}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeight_Float_Array_Data()
//
//! \brief
//!   Description:  get the dynamic build of the data for sflyHeaight array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the sflyheight
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sflyHeight_Float_Array_Data(JSONNODE* Node, const std::string& title, sflyHeight* value, int track, uint64_t heads, bool showStatusBits)
{
	JSONNODE* valueNode = json_new(JSON_ARRAY);
	json_set_name(valueNode, title.c_str());
	uint8_t negCheck = 0;												// check byte 6 bit 1 for negitive value
	uint64_t dsHead = 0;
	int16_t whole = 0;													// get 5:4 whole part of the float
	double decimal = 0.0;												// get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		negCheck = 0;
		dsHead = 0;
		whole = 0;													// get 5:4 whole part of the float
		decimal = 0.0;												// get 3:0 for the Deciaml Part of the float
		number = 0.0;

		switch (track)
		{
		case INNER:
			dsHead = check_Status_Strip_Status(value[loopCount].inner);
			negCheck = M_Byte6(value[loopCount].inner);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case OUTER:
			dsHead = check_Status_Strip_Status(value[loopCount].outer);
			negCheck = M_Byte6(value[loopCount].outer);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case MIDDLE:
			dsHead = check_Status_Strip_Status(value[loopCount].middle);
			negCheck = M_Byte6(value[loopCount].middle);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		}

		// check bit 1 to see if the number should be negitive
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}

		switch (track)
		{
		case INNER:
			set_json_float_With_Status(valueNode, title, number, value[loopCount].inner, showStatusBits);
			break;
		case OUTER:
			set_json_float_With_Status(valueNode, title, number, value[loopCount].outer, showStatusBits);
			break;
		case MIDDLE:
			set_json_float_With_Status(valueNode, title, number, value[loopCount].middle, showStatusBits);
			break;
		}

	}
	json_push_back(Node, valueNode);
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for h2sat array information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Data(JSONNODE* Node, const std::string& title, int movement, H2SAT* value, int track, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		h2sat_Node_Data(Node, title, movement, value, track, heads, showStatusBits);
	}
	else
	{
		h2sat_Array_Data(Node, title, movement, value, track, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Node_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for h2sat array information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Node_Data(JSONNODE* Node, const std::string& title, int movement, H2SAT* value, int track, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		int16_t number = 0;
		if (movement == WORDINT0)
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<int16_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 16));
				break;
			case ZONE1:
				number = static_cast<int16_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 16));
				break;
			case ZONE2:
				number = static_cast<int16_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 16));
				break;
			}
		}
		else
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<int16_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 32));
				break;
			case ZONE1:
				number = static_cast<int16_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 32));
				break;
			case ZONE2:
				number = static_cast<int16_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 32));
				break;
			}
		}
		switch (track)
		{
		case ZONE0:
			set_json_int_Check_Status(Node, temp.str().c_str(), number, value[loopCount].zone0, showStatusBits);
			break;
		case ZONE1:
			set_json_int_Check_Status(Node, temp.str().c_str(), number, value[loopCount].zone1, showStatusBits);
			break;
		case ZONE2:
			set_json_int_Check_Status(Node, temp.str().c_str(), number, value[loopCount].zone2, showStatusBits);
			break;
		}

	}
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Array_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for h2sat array information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Array_Data(JSONNODE* Node, const std::string& title, int movement, H2SAT* value, int track, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headerror = json_new(JSON_ARRAY);
	json_set_name(headerror, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		int16_t number = 0;
		if (movement == WORDINT0)
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<int16_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 16));
				break;
			case ZONE1:
				number = static_cast<int16_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 16));
				break;
			case ZONE2:
				number = static_cast<int16_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 16));
				break;
			}
		}
		else
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<int16_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 32));
				break;
			case ZONE1:
				number = static_cast<int16_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 32));
				break;
			case ZONE2:
				number = static_cast<int16_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 32));
				break;
			}
		}
		switch (track)
		{
		case ZONE0:
			set_json_int_Check_Status(headerror, title, number, value[loopCount].zone0, showStatusBits);
			break;
		case ZONE1:
			set_json_int_Check_Status(headerror, title, number, value[loopCount].zone1, showStatusBits);
			break;
		case ZONE2:
			set_json_int_Check_Status(headerror, title, number, value[loopCount].zone2, showStatusBits);
			break;
		}

	}
	json_push_back(Node, headerror);
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Float_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for h2sat array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Float_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, H2SAT* value, int track, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		h2sat_Float_Node_Data(Node, title, movement, calculation, value, track, heads, showStatusBits);
	}
	else
	{
		h2sat_Float_Array_Data(Node, title, movement, calculation, value, track, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Float_Node_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for H2SAT as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Float_Node_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, H2SAT* value, int track, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		double number = 0;
		if (movement == WORDINT0 || movement == WORD0)
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 16) * calculation);
				break;
			case ZONE1:
				number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 16) * calculation);
				break;
			case ZONE2:
				number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 16) * calculation);
				break;
			}
		}
		else
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 32) * calculation);
				break;
			case ZONE1:
				number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 32) * calculation);
				break;
			case ZONE2:
				number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 32) * calculation);
				break;
			}
		}
		switch (track)
		{
		case ZONE0:
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].zone0, showStatusBits);
			break;
		case ZONE1:
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].zone1, showStatusBits);
			break;
		case ZONE2:
			set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount].zone2, showStatusBits);
			break;
		}

	}
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Float_Array_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for H2SAT array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Float_Array_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, H2SAT* value, int track, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headerror = json_new(JSON_ARRAY);
	json_set_name(headerror, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		double number = 0;
		if (movement == WORDINT0 || movement == WORD0)
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 16) * calculation);
				break;
			case ZONE1:
				number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 16) * calculation);
				break;
			case ZONE2:
				number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 16) * calculation);
				break;
			}
		}
		else
		{
			switch (track)
			{
			case ZONE0:
				number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone0)), 32) * calculation);
				break;
			case ZONE1:
				number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone1)), 32) * calculation);
				break;
			case ZONE2:
				number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount].zone2)), 32) * calculation);
				break;
			}
		}
		switch (track)
		{
		case ZONE0:
			set_json_float_With_Status(headerror, title, number, value[loopCount].zone0, showStatusBits);
			break;
		case ZONE1:
			set_json_float_With_Status(headerror, title, number, value[loopCount].zone1, showStatusBits);
			break;
		case ZONE2:
			set_json_float_With_Status(headerror, title, number, value[loopCount].zone2, showStatusBits);
			break;
		}

	}
	json_push_back(Node, headerror);
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Float_Dword_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for h2sat array as dword calculation information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Float_Dword_Data(JSONNODE* Node, const std::string& title, H2SAT* value, int track, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		h2sat_Float_Dword_Node_Data(Node, title, value, track, heads, showStatusBits);
	}
	else
	{
		h2sat_Float_Dword_Array_Data(Node, title, value, track, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Float_Dword_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for h2sat array as dword calculation information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Float_Dword_Node_Data(JSONNODE* Node, const std::string& title, H2SAT* value, int track, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	uint64_t dsHead = 0;
	uint8_t negCheck = 0;										// get 6 byte to check for negitive number
	int16_t whole = 0;							                // get 5:4 whole part of the float
	double decimal = 0.0;										// get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		
		dsHead = 0;
		negCheck = 0;
		whole = 0;							                    // get 5:4 whole part of the float
		decimal = 0.0;											// get 3:0 for the Deciaml Part of the float
		number = 0.0;

		switch (track)
		{
		case ZONE0:
			dsHead = check_Status_Strip_Status(value[loopCount].zone0);
			negCheck = M_Byte6(value[loopCount].zone0);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case ZONE1:
			dsHead = check_Status_Strip_Status(value[loopCount].zone1);
			negCheck = M_Byte6(value[loopCount].zone1);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case ZONE2:
			dsHead = check_Status_Strip_Status(value[loopCount].zone2);
			negCheck = M_Byte6(value[loopCount].zone2);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		}

		// check to see if byte6 bit 1 is set. If set then set number to a negitive number
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}
		// check if byte6 bit 0 is set then the data is a percentage
		if (negCheck & BIT0)
		{
			myStr.str(""); myStr.clear();
			myStr << title.c_str() << " percentage" << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		}
		else
		{
			myStr.str(""); myStr.clear();
			myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		}
		switch (track)
		{
		case ZONE0:
			set_json_float_With_Status(Node, myStr.str().c_str(), number, value[loopCount].zone0, showStatusBits);
			break;
		case ZONE1:
			set_json_float_With_Status(Node, myStr.str().c_str(), number, value[loopCount].zone1, showStatusBits);
			break;
		case ZONE2:
			set_json_float_With_Status(Node, myStr.str().c_str(), number, value[loopCount].zone2, showStatusBits);
			break;
		}

	}
}
//-----------------------------------------------------------------------------
//
//! \fn h2sat_Float_Dword_Array_Data()
//
//! \brief
//!   Description:  get the static build of the data for h2sat array as dword calculation information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute H2SAT
//! \param track = get the data for the inner outer middle of the the array data for the H2SAT
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::h2sat_Float_Dword_Array_Data(JSONNODE* Node, const std::string& title, H2SAT* value, int track, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	uint8_t negCheck = 0;										// get 6 byte to check for negitive number
	uint64_t dsHead = 0;
	int16_t whole = 0;							                // get 5:4 whole part of the float
	double decimal = 0.0;										// get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	switch (track)
	{
	case ZONE0:
		negCheck = M_Byte6(value->zone0);
		break;
	case ZONE1:
		negCheck = M_Byte6(value->zone1);
		break;
	case ZONE2:
		negCheck = M_Byte6(value->zone2);
		break;
	default:
		negCheck = M_Byte6(value->zone0);
		break;
	}
	JSONNODE* valueNode = json_new(JSON_ARRAY);
	if (negCheck & BIT0)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " percentage" ;                    
	}
	else
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str();                   
	}
	json_set_name(valueNode, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		negCheck = 0;
		dsHead = 0;
		whole = 0;							                     
		decimal = 0.0;                  
		number = 0.0;

		switch (track)
		{
		case ZONE0:
			dsHead = check_Status_Strip_Status(value[loopCount].zone0);
			negCheck = M_Byte6(value[loopCount].zone0);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case ZONE1:
			dsHead = check_Status_Strip_Status(value[loopCount].zone1);
			negCheck = M_Byte6(value[loopCount].zone1);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		case ZONE2:
			dsHead = check_Status_Strip_Status(value[loopCount].zone2);
			negCheck = M_Byte6(value[loopCount].zone1);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		default:
			dsHead = check_Status_Strip_Status(value[loopCount].zone0);
			negCheck = M_Byte6(value[loopCount].zone0);
			whole = M_WordInt2(dsHead);
			decimal = static_cast<double>(M_DoubleWord0(dsHead));
			break;
		}

		// Check byte 6 bit 1 to see if the value should be negitivbe
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}

		switch (track)
		{
		case ZONE0:
			set_json_float_With_Status(valueNode, title, number, value[loopCount].zone0, showStatusBits);
			break;
		case ZONE1:
			set_json_float_With_Status(valueNode, title, number, value[loopCount].zone1, showStatusBits);
			break;
		case ZONE2:
			set_json_float_With_Status(valueNode, title, number, value[loopCount].zone2, showStatusBits);
			break;
		}

	}
	json_push_back(Node, valueNode);
}
//-----------------------------------------------------------------------------
//
//! \fn uint_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for uint64_t data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::uint_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		uint_Node_Data(Node, title, value, heads, showStatusBits);
	}
	else
	{
		uint_Array_Data(Node, title, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn uint_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for uint64_t data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::uint_Node_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		set_json_int_With_Status(Node, myStr.str().c_str(), value[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn uint_Array_Data()
//
//! \brief
//!   Description:  Set the json uint with status using the below information for head data
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::uint_Array_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headInt = json_new(JSON_ARRAY);
	json_set_name(headInt, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		set_json_int_With_Status(headInt, title, value[loopCount], showStatusBits);
	}
	json_push_back(Node, headInt);
}
//-----------------------------------------------------------------------------
//
//! \fn int_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		int_Node_Data(Node, title, value, heads, showStatusBits);
	}
	else
	{
		int_Array_Data(Node, title, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for int64_t data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void  CFarmCommon::int_Node_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		set_json_int_With_Status(Node, myStr.str().c_str(), value[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Array_Data()
//
//! \brief
//!   Description:  Set the json int with status using the below information for head data
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Array_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headInt = json_new(JSON_ARRAY);
	json_set_name(headInt, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		set_json_int_With_Status(headInt, title, value[loopCount], showStatusBits);
	}
	json_push_back(Node, headInt);
}
//-----------------------------------------------------------------------------
//
//! \fn int_Dword_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Dword_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		int_Dword_Node_Data(Node, title, value, heads, showStatusBits);
	}
	else
	{
		int_Dword_Array_Data(Node, title, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Dword_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Dword_Node_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		// valid number is a int32_t need to strip off all the upper bits and check if negitive
		int32_t number = static_cast<int32_t>(check_for_signed_int(M_DoubleWord0(check_Status_Strip_Status(value[loopCount])), 32));
		set_json_int_Check_Status(Node, myStr.str().c_str(), number, value[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Dword_Array_Data()
//
//! \brief
//!   Description:  Set the json int with status using the below information for head data
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Dword_Array_Data(JSONNODE* Node, const std::string& title, int64_t* value, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headInt = json_new(JSON_ARRAY);
	json_set_name(headInt, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		// valid number is a int32_t need to strip off all the upper bits and check if negitive
		int32_t number = static_cast<int32_t>(check_for_signed_int(M_DoubleWord0(check_Status_Strip_Status(value[loopCount])), 32));
		set_json_int_Check_Status(headInt, title.c_str(), number, value[loopCount], showStatusBits);
	}
	json_push_back(Node, headInt);
}
//-----------------------------------------------------------------------------
//
//! \fn int_Cal_Byte_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Cal_Byte_Data(JSONNODE* Node, const std::string& title, int16_t calculation, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		int_Cal_Byte_Node_Data(Node, title, calculation, param, heads, showStatusBits);
	}
	else
	{
		int_Cal_Byte_Array_Data(Node, title, calculation, param, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Cal_Byte_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Cal_Byte_Node_Data(JSONNODE* Node, const std::string& title, int16_t calculation, int64_t* param, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		set_json_int_Check_Status(Node, myStr.str().c_str(), (static_cast<int64_t>(M_ByteInt0(param[loopCount])) * calculation), param[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Cal_Byte_Array_Data()
//
//! \brief
//!   Description:  get the static build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Cal_Byte_Array_Data(JSONNODE* Node, const std::string& title, int16_t calculation, int64_t* param, uint64_t heads, bool showStatusBits)
{
	JSONNODE* cal = json_new(JSON_ARRAY);
	json_set_name(cal, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		set_json_int_Check_Status(cal, title,  static_cast<int64_t>(M_ByteInt0(param[loopCount])) * calculation, param[loopCount], showStatusBits);
	}
	json_push_back(Node, cal);
}
//-----------------------------------------------------------------------------
//
//! \fn int_Percent_Dword_Data()
//
//! \brief
//!   Description:  get the static build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Percent_Dword_Data(JSONNODE* Node, const std::string& title, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		int_Percent_Dword_Node_Data(Node, title, param, heads, showStatusBits);
	}
	else
	{
		int_Percent_Dword_Array_Data(Node, title, param, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Percent_Dword_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Percent_Dword_Node_Data(JSONNODE* Node, const std::string& title, int64_t* param, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	int16_t whole = 0;
	int64_t delta = 0;
	uint8_t negCheck = 0;								// check bit 1 of 6, if set then value is negitive
	double decimal = 0.0;								// get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " for Head " << std::dec << loopCount;                    // write out the title plus the Head count
		delta = M_IGETBITRANGE((opensea_parser::check_Status_Strip_Status(param[loopCount])), 47, 0);
		negCheck = M_Byte6(param[loopCount]);									// check bit 1 of 6, if set then value is negitive
		whole = M_WordInt2(delta);												// get 5:4 whole part of the float
		decimal = static_cast<double>(M_DoubleWord0(delta));					// get 3:0 for the Deciaml Part of the float
		// check byte6 bit 1 for the number to be negitive
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}

		std::ostringstream value;
		if ((param[loopCount] & BIT63) == BIT63 && (param[loopCount] & BIT62) == BIT62)
		{
			value.str(""); value.clear();
			value << std::setfill('0') << std::setprecision(4) << ROUNDF(number, 10000);
		}
		else
		{
			value << "NULL";
		}
		set_json_string_With_Status(Node, myStr.str().c_str(), value.str().c_str(), param[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn int_Percent_Dword_Array_Data()
//
//! \brief
//!   Description:  get the dynamic build of the data for int64_t Dword data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::int_Percent_Dword_Array_Data(JSONNODE* Node, const std::string& title, int64_t* param, uint64_t heads, bool showStatusBits)
{
	JSONNODE* cal = json_new(JSON_ARRAY);
	json_set_name(cal, title.c_str());
	int16_t whole = 0;
	int64_t delta = 0;
	uint8_t negCheck = 0;								// check bit 1 of 6, if set then value is negitive
	double decimal = 0.0;								// get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		delta = M_IGETBITRANGE((check_Status_Strip_Status(param[loopCount])), 47, 0);
		negCheck = M_Byte6(param[loopCount]);								// check bit 1 of 6, if set then value is negitive
		whole = M_WordInt2(delta);											// get 5:4 whole part of the float
		decimal = static_cast<double>(M_DoubleWord0(delta));				// get 3:0 for the Deciaml Part of the float
		// check byte 6 bit 1 for a negitive number
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}

		std::ostringstream value;
		if ((param[loopCount] & BIT63) == BIT63 && (param[loopCount] & BIT62) == BIT62)
		{
			set_json_float_With_Status(cal, title, ROUNDF(number, 10000), param[loopCount], showStatusBits);
		}
		else
		{
			value << "NULL";
			set_json_string_With_Status(cal, title, value.str().c_str(), param[loopCount], showStatusBits);
		}

	}
	json_push_back(Node, cal);
}
//-----------------------------------------------------------------------------
//
//! \fn float_Cal_Word_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for float calculated data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Cal_Word_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		float_Cal_Word_Node_Data(Node, title, calculation, param, heads, showStatusBits);
	}
	else
	{
		float_Cal_Word_Array_Data(Node, title, calculation, param, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn float_Cal_Word_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for float calculated data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Cal_Word_Node_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		set_json_float_With_Status(Node, myStr.str().c_str(), (static_cast<double>(M_WordInt0(param[loopCount])) * calculation), param[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn float_Cal_Word_Array_Data()
//
//! \brief
//!   Description:  get the dynamic build of the data for float calculated data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Cal_Word_Array_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits)
{
	JSONNODE* cal = json_new(JSON_ARRAY);
	json_set_name(cal, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		set_json_float_With_Status(cal, title, (static_cast<double>(M_WordInt0(param[loopCount])) * calculation), param[loopCount], showStatusBits);
	}
	json_push_back(Node, cal);
}
//-----------------------------------------------------------------------------
//
//! \fn float_Cal_DoubleWord_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for float calculated data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Cal_DoubleWord_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		float_Cal_DoubleWord_Node_Data(Node, title, calculation, param, heads, showStatusBits);
	}
	else
	{
		float_Cal_DoubleWord_Array_Data(Node, title, calculation, param, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn float_Cal_DoubleWord_Node_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for float calculated data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Cal_DoubleWord_Node_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits)
{
	std::ostringstream myStr;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		myStr.str(""); myStr.clear();
		myStr << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		set_json_float_With_Status(Node, myStr.str().c_str(), (static_cast<double>(M_DoubleWord0(param[loopCount])) / calculation), param[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn float_Cal_DoubleWord_Array_Data()
//
//! \brief
//!   Description:  get the dynamic build of the data for float calculated data information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param calculation = M_Byte0 fo the param times the calculation
//! \param param = pointer to the frame attribute int64
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::float_Cal_DoubleWord_Array_Data(JSONNODE* Node, const std::string& title, double calculation, int64_t* param, uint64_t heads, bool showStatusBits)
{
	JSONNODE* cal = json_new(JSON_ARRAY);
	json_set_name(cal, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		set_json_float_With_Status(cal, title, (static_cast<double>(M_DoubleWord0(param[loopCount])) / calculation), param[loopCount], showStatusBits);
	}
	json_push_back(Node, cal);
}
//-----------------------------------------------------------------------------
//
//! \fn sas_Head_Float_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute int64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_Float_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		sas_Head_Float_Node_Data(Node, title, movement, calculation, value, heads, showStatusBits);
	}
	else
	{
		Sas_Head_Float_Array_Data(Node, title, movement, calculation, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn sas_Head_Float_Node_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute int64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_Float_Node_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		double number = 0;
		if (movement == WORDINT0 || movement == WORD0)
		{
			number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount])), 16) * calculation);
		}
		else
		{
			number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount])), 32) * calculation);
		}
		set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn Sas_Head_Float_Array_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute int64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::Sas_Head_Float_Array_Data(JSONNODE* Node, const std::string& title, int movement, double calculation, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headerror = json_new(JSON_ARRAY);
	json_set_name(headerror, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		double number = 0;
		if (movement == WORDINT0 || movement == WORD0)
		{
			number = static_cast<double>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount])), 16) * calculation);
		}
		else
		{
			number = static_cast<double>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount])), 32) * calculation);
		}

		set_json_float_With_Status(headerror, title, number, value[loopCount], showStatusBits);


	}
	json_push_back(Node, headerror);
}
//-----------------------------------------------------------------------------
//
//! \fn sas_Head_int_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t array as int information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute uint64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_int_Data(JSONNODE* Node, const std::string& title, int movement, uint64_t calculation, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		sas_Head_int_Node_Data(Node, title, movement, calculation, value, heads, showStatusBits);
	}
	else
	{
		Sas_Head_int_Array_Data(Node, title, movement, calculation, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn sas_Head_int_Node_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t as int information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute uint64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_int_Node_Data(JSONNODE* Node, const std::string& title, int movement, uint64_t calculation, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count
		int32_t number = 0;
		if (movement == WORDINT0 || movement == WORD0)
		{
			number = static_cast<int32_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount])), 16) * calculation);
		}
		else
		{
			number = static_cast<int32_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount])), 32) * calculation);
		}
		set_json_int_Check_Status(Node, temp.str().c_str(), static_cast<long long>(number), value[loopCount], showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn Sas_Head_int_Array_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for int64_t array as int information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param movement = the amount to move inorder to check the status of the value
//! \param calculation = tell the function which word from the value it need to use for the information
//! \param value = pointer to the frame attribute uint64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::Sas_Head_int_Array_Data(JSONNODE* Node, const std::string& title, int movement, uint64_t calculation, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	JSONNODE* headerror = json_new(JSON_ARRAY);
	json_set_name(headerror, title.c_str());
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		int32_t number = 0;
		if (movement == WORDINT0 || movement == WORD0)
		{
			number = static_cast<int32_t>(check_for_signed_int(M_WordInt0(check_Status_Strip_Status(value[loopCount])), 16) * calculation);
		}
		else
		{
			number = static_cast<int32_t>(check_for_signed_int(M_DoubleWordInt0(check_Status_Strip_Status(value[loopCount])), 32) * calculation);
		}

		set_json_int_Check_Status(headerror, title, static_cast<long long>(number), value[loopCount], showStatusBits);


	}
	json_push_back(Node, headerror);
}
//-----------------------------------------------------------------------------
//
//! \fn sas_Head_Double_Float_Data()
//
//! \brief
//!   Description:  get the static or dynamic build of the data for uint64_t array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64_t
//! \param track = get the data for the inner outer middle of the the array data for the uint64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_Double_Float_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits, bool showStatic)
{
	if (showStatic)
	{
		sas_Head_Double_Float_Node_Data(Node, title, value, heads, showStatusBits);
	}
	else
	{
		sas_Head_Double_Float_Array_Data(Node, title, value, heads, showStatusBits);
	}
}
//-----------------------------------------------------------------------------
//
//! \fn sas_Head_Double_Float_Node_Data()
//
//! \brief
//!   Description:  get the static build of the data for uint64_t array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_Double_Float_Node_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	std::ostringstream temp;
	uint64_t dsHead = 0;
	uint8_t negCheck = 0;					// check bit 1 of 6, if set then value is negitive
	int16_t whole = 0;						// get 5:4 whole part of the float
	double decimal = 0.0;                   // get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		temp.str(""); temp.clear();
		temp << title.c_str() << " by Head " << std::dec << loopCount;                    // write out the title plus the Head count

		dsHead = check_Status_Strip_Status(value[loopCount]);
		negCheck = M_Byte6(dsHead);
		whole = M_WordInt2(dsHead);
		decimal = static_cast<double>(M_DoubleWord0(dsHead));
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}
		set_json_float_With_Status(Node, temp.str().c_str(), number, value[loopCount], showStatusBits);
	}

}
//-----------------------------------------------------------------------------
//
//! \fn sflyHeight_Float_Array_Data()
//
//! \brief
//!   Description:  get the dynamic build of the data for uint64_t array as float information
//
//  Entry:
//! \param Node - JSON Node to add the array of information to
//! \param title  =  string information for that attribute 
//! \param value = pointer to the frame attribute uint64_t
//! \param heads = the total number of heads
//! \param showStatusBits = when set to true the valid and supported bits will be shown
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::sas_Head_Double_Float_Array_Data(JSONNODE* Node, const std::string& title, uint64_t* value, uint64_t heads, bool showStatusBits)
{
	JSONNODE* valueNode = json_new(JSON_ARRAY);
	json_set_name(valueNode, title.c_str());
	uint64_t dsHead = 0;
	uint8_t negCheck = 0;					// check bit 1 of 6, if set then value is negitive
	int16_t whole = 0;						// get 5:4 whole part of the float
	double decimal = 0.0;                   // get 3:0 for the Deciaml Part of the float
	double number = 0.0;
	for (uint32_t loopCount = 0; loopCount < heads; ++loopCount)
	{
		dsHead = check_Status_Strip_Status(value[loopCount]);
		negCheck = M_Byte6(dsHead);
		whole = M_WordInt2(dsHead);
		decimal = static_cast<double>(M_DoubleWord0(dsHead));
		if (negCheck & BIT1 || whole <= 0)
		{
			number = static_cast<double>(whole) - (decimal * .0001);
		}
		else
		{
			number = static_cast<double>(whole) + (decimal * .0001);
		}

		set_json_float_With_Status(valueNode, title, number, value[loopCount], showStatusBits);
	}
	json_push_back(Node, valueNode);
}

