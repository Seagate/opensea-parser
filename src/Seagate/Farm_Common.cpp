//
// Farm_Common.cpp
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2014 - 2020 Seagate Technology LLC and/or its Affiliates
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
			meaning = "Host Read - Generic";
			break;
		case HOST_READ_UNCORRECTABLE:
			meaning = "Host Read - Uncorrectable";
			break;
		case HOST_READ_RAW:
			meaning = "Host Read - RAW";
			break;
		case HOST_WRITE_GENERIC:
			meaning = "Host Write - Generic";
			break;
		case HOST_WRITE_UNCORRECTABLE:
			meaning = "Host Write - Uncorrectable";
			break;
		case HOST_WRITE_RAW:
			meaning = "Host Write - RAW";
			break;
		case BACKGROUND_READ_GENERIC:
			meaning = "Background Read - Generic";
			break;
		case BACKGROUND_READ_RELIABILITY:
			meaning = "Background Read - Reliability";
			break;
		case BACKGROUND_READ_RECOVERY:
			meaning = "Background Read - Recovery";
			break;
		case BACKGROUND_READ_HOST_SELF_TEST:
			meaning = "Background Read - Host Self Test";
			break;
		case BACKGROUND_WRITE_GENERIC:
			meaning = "Background Write - Generic";
			break;
		case BACKGROUND_WRITE_RELIABILITY:
			meaning = "Background Write - Reliability";
			break;
		case BACKGROUND_WRITE_RECOVERY:
			meaning = "Background Write - Recovery";
			break;
		case BACKGROUND_WRITE_HOST_SELF_TEST:
			meaning = "Background Write - Host Self Test";
			break;
		case SERVO_WEDGE:
			meaning = "Servo Wedge";
			break;
		default:
			meaning = "Unknown";
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
			meaning = "Unknow";
			break;
		case MICROPROCESSOR_FAILED:
			meaning = "Microprocessor Failed";
			break;
		case DRAM_FAILED_POWERUP_OR_WRAM_FAIL:
			meaning = "DRAM Failed powerup or WRAM Failed";
			break;
		case SCC_FAILED_POWERUP_DIAGNOSTICS:
			meaning = "SCC Failed Powerup Diagnostics";
			break;
		case FW_DOES_NOT_MATCH_THE_SCC_VERSION:
			meaning = "Firmware does not match teh SCC Version";
			break;
		case UNIMPLEMENTED_OPCODE_INTERRUPT:
			meaning = "Unimplemented opcode interrupt";
			break;
		case POWER_UP_XOR_FAILURE_FOR_FIBER_CH:
			meaning = "Power up XOR failure for fiber CH";
			break;
		case EEPROM_VERIFY_ERROR_EVEN_BYTE:
			meaning = "EEPROM verify error even byte";
			break;
		case EEPROM_ERASE_ERROR_EVEN_BYTE:
			meaning = "EEPROM erase error even byte";
			break;
		case DOWNLOAD_TPM_FAILED_0:
			meaning = "Download TPM failed 0";
			break;
		case DOWNLOAD_TPM_FAILED_1:
			meaning = "Download TPM failed 1";
			break;
		case DOWNLOAD_TPM_FAILED_2:
			meaning = "Download TPM failed 2";
			break;
		case DOWNLOAD_TPM_FAILED_3:
			meaning = "Download TPM failed 3";
			break;
		case DOWNLOAD_TPM_FAILED_4:
			meaning = "Download TPM failed 4";
			break;
		case DOWNLOAD_TPM_FAILED_5:
			meaning = "Download TPM failed 5";
			break;
		case DOWNLOAD_TPM_FAILED_6:
			meaning = "Download TPM failed 6";
			break;
		case DOWNLOAD_TPM_FAILED_7:
			meaning = "Download TPM failed 7";
			break;
		case DOWNLOAD_TPM_FAILED_8:
			meaning = "Download TPM failed 8";
			break;
		case DOWNLOAD_VOLTAGE_FAULT:
			meaning = "Download Voltage fault";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_0:
			meaning = "Fails writting array data to flash 0";
			break;
		case FLASH_LOOKING_FOR_MEMORY_RANGE_ERROR:
			meaning = "Flash Looking for memory range error";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_1:
			meaning = "Fails writting array data to flash 1";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_2:
			meaning = "Fails writting array data to flash 2";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_3:
			meaning = "Fails writting array data to flash 3";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_4:
			meaning = "Fails writting array data to flash 4";
			break;
		case FAILS_WRITING_ARRAY_DATA_TO_FLASH_5:
			meaning = "Fails writting array data to flash 5";
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
			meaning = "DRAM Configuration Process Failed";
			break;
		case FDE_BUS_PARITY_ERROR:
			meaning = "FDE Bus parity error";
			break;
		case PREFETCH_VECTOR_OR_STACK_POINTER_OUT:
			meaning = "Prefetch vector or stack pointer out";
			break;
		case ERROR_IN_WRITING_TO_READ_CHIP:
			meaning = "Error in writting to read chip";
			break;
		case IER_STACK_OVERFLOW:
			meaning = "IER Stack overflow";
			break;
		case IER_STACK_UNDERFLOW:
			meaning = "IER Stack underflow";
			break;
		case IER_STACK_NOT_EMPTY_ON_ENTRY_TO_SLEEP:
			meaning = "IER Stack Not Empty on Entry to Sleep";
			break;
		case IRAW_HAD_MISCOMPARE:
			meaning = "IRAW has miscompare";
			break;
		case UNDEFINED_INSTRUCTION:
			meaning = "Underfined instruction";
			break;
		case LOGGING_SAVE_FAILED_EXCEEDED_ALLOCATED:
			meaning = "Logging save failed exceeded allocated";
			break;
		case CANT_FIND_BACKPLANE_DATA_RATE:
			meaning = "Cant Find backplane data rate";
			break;
		case CONTROLLER_I_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "Controller I TCM Double bit ECC Error";
			break;
		case CONTROLLER_D_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "Controller D TCM double bit ECC Error";
			break;
		case SERVO_I_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "Servo I TCM Double bit ECC Error";
			break;
		case SERVO_D_TCM_DOUBLE_BIT_ECC_ERROR:
			meaning = "Servo D TCM double bit ECC ERROR";
			break;
		case CDPRAM_UNRECOVERABLE_ERROR:
			meaning = "CDPRAM unrecoverable Error";
			break;
		case SDPRAM_UNRECOVERABLE_ERROR:
			meaning = "SDPRAM unrecoverable Error";
			break;
		case TCM_CRC_RESULT_IS_NON_ZERO:
			meaning = "TCM CRC result is non zero";
			break;
		case SWI_ASSERT_FLASH_CODE_BOOT:
			meaning = "SWI Assert Flash Code Boot";
			break;
		case SWI_ASSERT_FLASH_CODE_NQNR:
			meaning = "SWI Assert Flash Code NQNR";
			break;
		case SWI_ASSERT_FLASH_CODE_DISC:
			meaning = "SWI Assert Flash Code DISC";
			break;
		case REMOTE_ASSERT:
			meaning = "Remode Assert";
			break;
		case DRAM_INTEGRITY_FAILURE:
			meaning = "DRAM Integrity Failure";
			break;
		case CLOCK_FAILURE:
			meaning = "Clock Failure";
			break;
		case ASSERT_FLASH_CODE:
			meaning = "Assert Flash Code";
			break;
		case ENSURE_FLASH_CODE:
			meaning = "Ensure Flash Code";
			break;
		case REQUIRE_FLASH_CODE:
			meaning = "Require Flash Code";
			break;
		case SMART_FLASH_CODE:
			meaning = "SMART Flash Code";
			break;
		case SCSI_UNEXEPCTED_INTERRUPT:
			meaning = "SCSI Unexpected Interrupt";
			break;
		case SCSI_TIMEOUT:
			meaning = "SCSI Timeout";
			break;
		case ILLEGAL_STATUS_CODE:
			meaning = "Illegal Status Code";
			break;
		case SCSI_UNDER_OVER_RUN_OCCURRED:
			meaning = "SCSI Under Over Run Occurred";
			break;
		case UNEXPECTED_STATUS:
			meaning = "Unexpected Status";
			break;
		case DIVIDE_BY_ZERO_INTERRUPT:
			meaning = "Divide by Zero Interrupt";
			break;
		case DATA_ABORT_CACHE_ECC_ERROR:
			meaning = "Data Abort Cache ECC Error";
			break;
		case DATA_ABORT_TCM_ECC_ERROR:
			meaning = "Data Abort TCM ECC Error";
			break;
		case ABORT_INTERRUPT:
			meaning = "Abort Interrupt";
			break;
		case SELF_SEEK_FAILURE:
			meaning = "Self Seek Failure";
			break;
		case CONTROLLER_NUKED_BY_FDE:
			meaning = "Controller Nuked By FDE";
			break;
		case FLASH_IOEDC_PARITY_ERROR:
			meaning = "Flash IOEDC Parity Error";
			break;
		case SERIAL_PORT_DUMP_MODE:
			meaning = "Serial Port Dump Mode";
			break;
		default:
			meaning = "Unknow";
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
			strncpy((char *)dateStr.c_str(), (char*)&date, DATE_YEAR_DATE_SIZE);
		}
   }

