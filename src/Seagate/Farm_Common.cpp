//
// Farm_Common.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2021 Seagate Technology LLC and/or its Affiliates
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
	//byte_Swap_32(firmware);
	firmwareRevStr.assign(reinterpret_cast<const char*>(firmware), sizeof(uint32_t));
	//byte_Swap_32(firmware2);
	firmwareRevStr.append(reinterpret_cast<const char*>(firmware2), sizeof(uint32_t));
	remove_trailing_whitespace_std_string(firmwareRevStr);
}
//-----------------------------------------------------------------------------
//
//! \fn get_Reallocation_Cause_Meanings()
//
//! \brief
//!   Description:  parse out the meaning of the reallocation
//
//  Entry:
//! \param meaning - string for the meaning of the cause
//! \param code - code for the what the meaning is
//
//  Exit:
//!   \return 
//
//---------------------------------------------------------------------------
   void CFarmCommon::get_Reallocation_Cause_Meanings(std::string &meaning, uint16_t code)
   {
		switch (code)
		{
		case HOST_READ_GENERIC:
			meaning = "host read - generic";
			break;
		case HOST_READ_UNCORRECTABLE:
			meaning = "host read - uncorrectable";
			break;
		case HOST_READ_RAW:
			meaning = "host read - RAW";
			break;
		case HOST_WRITE_GENERIC:
			meaning = "host write - generic";
			break;
		case HOST_WRITE_UNCORRECTABLE:
			meaning = "host write - uncorrectable";
			break;
		case HOST_WRITE_RAW:
			meaning = "host write - RAW";
			break;
		case BACKGROUND_READ_GENERIC:
			meaning = "background read - generic";
			break;
		case BACKGROUND_READ_RELIABILITY:
			meaning = "background read - reliability";
			break;
		case BACKGROUND_READ_RECOVERY:
			meaning = "background read - recovery";
			break;
		case BACKGROUND_READ_HOST_SELF_TEST:
			meaning = "background read - host self test";
			break;
		case BACKGROUND_WRITE_GENERIC:
			meaning = "background write - generic";
			break;
		case BACKGROUND_WRITE_RELIABILITY:
			meaning = "background write - reliability";
			break;
		case BACKGROUND_WRITE_RECOVERY:
			meaning = "background write - recovery";
			break;
		case BACKGROUND_WRITE_HOST_SELF_TEST:
			meaning = "background write - host self test";
			break;
		case SERVO_WEDGE:
			meaning = "servo wedge";
			break;
		default:
			meaning = "unknown";
			break;
		}
   }

  	//-----------------------------------------------------------------------------
	//
	//! \fn get_Assert_Code_Meaning()
	//
	//! \brief
	//!   Description: takes in the Assert code and will look up the meaning and fill in the string
	//
	//  Entry:
	//! \param meaning  =  meaning string
	//! \param code = the assert code 
	//
	//  Exit:
	//!   \return bool
	//
	//---------------------------------------------------------------------------
   void CFarmCommon::get_Assert_Code_Meaning(std::string &meaning, uint16_t code)
   {
		switch (code)
		{
		case ASSERT_UNKNOWN:
			meaning = "unknown";
			break;
		case MICROPROCESSOR_FAILED:
			meaning = "microprocessor failed";
			break;
		case DRAM_FAILED_POWERUP_OR_WRAM_FAIL:
			meaning = "DRAM failed powerup or WRAM failed";
			break;
		case SCC_FAILED_POWERUP_DIAGNOSTICS:
			meaning = "SCC failed powerup diagnostics";
			break;
		case FW_DOES_NOT_MATCH_THE_SCC_VERSION:
			meaning = "firmware does not match the SCC version";
			break;
		case UNIMPLEMENTED_OPCODE_INTERRUPT:
			meaning = "unimplemented opcode interrupt";
			break;
		case POWER_UP_XOR_FAILURE_FOR_FIBER_CH:
			meaning = "power up XOR failure for fiber CH";
			break;
		case EEPROM_VERIFY_ERROR_EVEN_BYTE:
			meaning = "EEPROM verify error even byte";
			break;
		case EEPROM_ERASE_ERROR_EVEN_BYTE:
			meaning = "EEPROM erase error even byte";
			break;
		case DOWNLOAD_TPM_FAILED_0:
			meaning = "download TPM failed 0";
			break;
		case DOWNLOAD_TPM_FAILED_1:
			meaning = "download TPM failed 1";
			break;
		case DOWNLOAD_TPM_FAILED_2:
			meaning = "download TPM failed 2";
			break;
		case DOWNLOAD_TPM_FAILED_3:
			meaning = "download TPM failed 3";
			break;
		case DOWNLOAD_TPM_FAILED_4:
			meaning = "download TPM failed 4";
			break;
		case DOWNLOAD_TPM_FAILED_5:
			meaning = "download TPM failed 5";
			break;
		case DOWNLOAD_TPM_FAILED_6:
			meaning = "download TPM failed 6";
			break;
		case DOWNLOAD_TPM_FAILED_7:
			meaning = "download TPM failed 7";
			break;
		case DOWNLOAD_TPM_FAILED_8:
			meaning = "download TPM failed 8";
			break;
		case DOWNLOAD_VOLTAGE_FAULT:
			meaning = "download voltage fault";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_0:
			meaning = "fails writting array data to flash 0";
			break;
		case FLASH_LOOKING_FOR_MEMORY_RANGE_ERROR:
			meaning = "flash Looking for memory range error";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_1:
			meaning = "fails writting array data to flash 1";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_2:
			meaning = "fails writting array data to flash 2";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_3:
			meaning = "fails writting array data to flash 3";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_4:
			meaning = "fails writting array data to flash 4";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_5:
			meaning = "fails writting array data to flash 5";
			break;
		case ALU_BUFFER_PARITY_ERROR:
			meaning = "ALU buffer party error";
			break;
		case PREFETCH_TCM_ECC_ERROR:
			meaning = "perfetch TCM ECC error";
			break;
		case ERROR_INJECTION_ASSERT:
			meaning = "Error injection assert";
			break;
		case DRAM_CONFIGURATION_PROCESS_FAILED:
			meaning = "DRAM configuration process failed";
			break;
		case FDE_BUS_PARITY_ERROR:
			meaning = "FDE Bus parity error";
			break;
		case PREFETCH_VECTOR_OR_STACK_POINTER_OUT:
			meaning = "prefetch vector or stack pointer out";
			break;
		case ERROR_IN_WRITING_TO_READ_CHIP:
			meaning = "error in writting to read chip";
			break;
		case IER_STACK_OVERFLOW:
			meaning = "IER stack overflow";
			break;
		case IER_STACK_UNDERFLOW:
			meaning = "IER stack underflow";
			break;
		case IER_STACK_NOT_EMPTY_ON_ENTRY_TO_SLEEP:
			meaning = "IER stack Not empty on entry to sleep";
			break;
		case IRAW_HAD_MISCOMPARE:
			meaning = "IRAW has miscompare";
			break;
		case UNDEFINED_INSTRUCTION:
			meaning = "underfined instruction";
			break;
		case LOGGING_SAVE_FAILED_EXCEEDED_ALLOCATED:
			meaning = "logging save failed exceeded allocated";
			break;
		case CANT_FIND_BACKPLANE_DATA_RATE:
			meaning = "cant find backplane data rate";
			break;
		case CONTROLLER_I_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "controller I TCM bouble bit ECC error";
			break;
		case CONTROLLER_D_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "controller D TCM double bit ECC error";
			break;
		case SERVO_I_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "servo I TCM double bit ECC error";
			break;
		case SERVO_D_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "servo D TCM double bit ECC error";
			break;
		case CDPRAM_UNRECOVERABLE_ERROR:
			meaning = "CDPRAM unrecoverable drror";
			break;
		case SDPRAM_UNRECOVERABLE_ERROR:
			meaning = "SDPRAM unrecoverable drror";
			break;
		case TCM_CRC_RESULT_IS_NON_ZERO:
			meaning = "TCM CRC result is non zero";
			break;
		case SWI_ASSERT_FLASH_CODE_BOOT:
			meaning = "SWI assert flash code boot";
			break;
		case SWI_ASSERT_FLASH_CODE_NQNR:
			meaning = "SWI assert flash code NQNR";
			break;
		case SWI_ASSERT_FLASH_CODE_DISC:
			meaning = "SWI assert flash code DISC";
			break;
		case REMOTE_ASSERT:
			meaning = "remode assert";
			break;
		case DRAM_INTEGRITY_FAILURE:
			meaning = "DRAM integrity failure";
			break;
		case CLOCK_FAILURE:
			meaning = "clock failure";
			break;
		case ASSERT_FLASH_CODE:
			meaning = "assert flash code";
			break;
		case ENSURE_FLASH_CODE:
			meaning = "ensure flash code";
			break;
		case REQUIRE_FLASH_CODE:
			meaning = "Require flash code";
			break;
		case SMART_FLASH_CODE:
			meaning = "SMART flash code";
			break;
		case SCSI_UNEXEPCTED_INTERRUPT:
			meaning = "SCSI unexpected interrupt";
			break;
		case SCSI_TIMEOUT:
			meaning = "SCSI timeout";
			break;
		case ILLEGAL_STATUS_CODE:
			meaning = "illegal status code";
			break;
		case SCSI_UNDER_OVER_RUN_OCCURRED:
			meaning = "SCSI under over run occurred";
			break;
		case UNEXPECTED_STATUS:
			meaning = "unexpected status";
			break;
		case DIVIDE_BY_ZERO_INTERRUPT:
			meaning = "divide by zero interrupt";
			break;
		case DATA_ABORT_CACHE_ECC_ERROR:
			meaning = "data abort cache ECC error";
			break;
		case DATA_ABORT_TCM_ECC_ERROR:
			meaning = "data abort TCM ECC error";
			break;
		case ABORT_INTERRUPT:
			meaning = "abort interrupt";
			break;
		case SELF_SEEK_FAILURE:
			meaning = "self seek failure";
			break;
		case CONTROLLER_NUKED_BY_FDE:
			meaning = "controller nuked by FDE";
			break;
		case FLASH_IOEDC_PARITY_ERROR:
			meaning = "flash IOEDC parity error";
			break;
		case SERIAL_PORT_DUMP_MODE:
			meaning = "serial port Ude";
			break;
		default:
			meaning = "unknow";
			break;
		}
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
//! \fn Get_NVC_Status()
//
//! \brief
//!   Description:  get the NVC status bits and create meaningful data
//
//  Entry:
//! \param NVC_Node - JSON Node to add the array of information to
//! \param status  =  what the status is for the NVC
//
//  Exit:
//!   \return void
//
//---------------------------------------------------------------------------
void CFarmCommon::Get_NVC_Status(JSONNODE* NVC_Node, uint64_t status)
{
	if (status != 0)
	{
		JSONNODE* myArray = json_new(JSON_ARRAY);
		json_set_name(myArray, ("NVC Status Events"));
		if (status & BIT8)
			json_push_back(myArray, json_new_a("NVC Status Events", "flash burn started"));
		if (status & BIT9)
			json_push_back(myArray, json_new_a("NVC Status Events", "flash burn finished"));
		if (status & BIT10)
			json_push_back(myArray, json_new_a("NVC Status Events", "validate flash erase pattern skipped"));
		if (status & BIT19)
			json_push_back(myArray, json_new_a("NVC Status Events", "servo r5 in WFE"));
		if (status & BIT20)
			json_push_back(myArray, json_new_a("NVC Status Events", "servo m0 in WFI"));
		if (status & BIT21)
			json_push_back(myArray, json_new_a("NVC Status Events", "servo r5 halt timeout"));
		if (status & BIT22)
			json_push_back(myArray, json_new_a("NVC Status Events", "servo m0 halt timeout"));
		if (status & BIT23)
			json_push_back(myArray, json_new_a("NVC Status Events", "AUX LLP stream timeout"));

		json_push_back(NVC_Node, myArray);
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
	default:
		*reason = "unknown";
		break;
	}
}
