# 
#
# Do NOT modify or remove this copyright and license
#
# Copyright (c) 2017 - 2019 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
#
# This software is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# ******************************************************************************************

NAME=opensea-parser
#Change the Major version when major interface changes are made.
MAJOR=0
#Change the Minor version when new features are added. 
MINOR=1
#Change the patch version when only bug fixes are made.
PATCH=14

UNAME := $(shell uname -s)
OS=$(shell uname)

ifeq ($(UNAME),Darwin)
    LXXFLAGS += -shared -undefined dynamic_lookup -Wall
else 
    LXXFLAGS += -shared -Wall
endif

VERSION=$(MAJOR).$(MINOR).$(PATCH)
SRC_DIR=../../src/
INC_DIR=-I../../include -I../../include/Seagate -I../../../opensea-common/include -I../../../libjson
CXX ?= g++
CXXFLAGS ?= -Wall -Wextra -Werror=old-style-cast -Wshadow -Wmaybe-uninitialized -Wvla -Wfloat-equal \
    -Wlogical-op -Wdouble-promotion -Wformat-security
CXXFLAGS += -c -fPIC -std=c++98 -I.
ARCHIVE ?= ar
LFLAGS ?= -Wall-Wl,--whole-archive ../../../opensea-common/Make/gcc/lib/$(FILE_OUTPUT_DIR)/libopensea-common.a -Wl,--no-whole-archive
LIB_SRC_FILES = \
    $(SRC_DIR)Opensea_Parser_Helper.cpp\
    $(SRC_DIR)CLog.cpp\
    $(SRC_DIR)CAta_Ext_DST_Log.cpp\
    $(SRC_DIR)CAta_Device_Stat_Log.cpp\
    $(SRC_DIR)CAta_Ext_Comprehensive_Log.cpp\
    $(SRC_DIR)CAta_Identify_Log.cpp\
    $(SRC_DIR)CAta_Power_Condition_Log.cpp\
    $(SRC_DIR)CAta_NCQ_Command_Error_Log.cpp\
    $(SRC_DIR)CAta_SMART_Log_Dir.cpp\
    $(SRC_DIR)CScsi_Log.cpp\
    $(SRC_DIR)CScsi_Application_Client_Log.cpp\
    $(SRC_DIR)CScsi_Background_Operation_Log.cpp\
    $(SRC_DIR)CScsi_Background_Scan_Log.cpp\
    $(SRC_DIR)CScsi_Cache_Statistics_Log.cpp\
    $(SRC_DIR)CScsi_Environmental_Logs.cpp\
    $(SRC_DIR)CScsi_Temperature_Log.cpp\
    $(SRC_DIR)CScsi_Error_Counter_Log.cpp\
    $(SRC_DIR)CScsi_Factory_Log.cpp\
    $(SRC_DIR)CScsi_Format_Status_Log.cpp\
    $(SRC_DIR)CScsi_Informational_Exeptions_Log.cpp\
    $(SRC_DIR)CScsi_Logical_Block_Provisioning_Log.cpp\
    $(SRC_DIR)CScsi_Non_Medium_Error_Count_Log.cpp\
    $(SRC_DIR)CScsi_Page_19h_Cache_Memory_Statistics_Log.cpp\
    $(SRC_DIR)CScsi_Page_19h_Command_Duration_Limits_Log.cpp\
    $(SRC_DIR)CScsi_Pending_Defects_Log.cpp\
    $(SRC_DIR)CScsi_Power_Condition_Transitions_Log.cpp\
    $(SRC_DIR)CScsi_Protocol_Specific_Port_Log.cpp\
    $(SRC_DIR)CScsi_Self_Test_Results_Log.cpp\
    $(SRC_DIR)CScsi_Solid_State_Drive_Log.cpp\
    $(SRC_DIR)CScsi_Start_Stop_Cycle_Counter_Log.cpp\
    $(SRC_DIR)CScsi_Supported_LogPages_Log.cpp\
    $(SRC_DIR)CScsi_Zoned_Device_Statistics_Log.cpp\
    $(SRC_DIR)/Seagate/Farm_Common.cpp\
    $(SRC_DIR)/Seagate/CFARM_Log.cpp\
    $(SRC_DIR)/Seagate/CAta_Farm_Log.cpp\
    $(SRC_DIR)/Seagate/CScsi_Farm_Log.cpp\

PROJECT_DEFINES += #-D_DEBUG

#All of the source files have associated object files
LIB_OBJ_FILES = $(LIB_SRC_FILES:.cpp=.o)
LIBS = lib$(NAME).a

FILE_OUTPUT_DIR=lib

#DEPFILES = $(LIB_SRC_FILES:.c=.d)

#-include $(DEPFILES)

#Add more warnings for the GCC versions that support them
#gcc 5.5 and up : -Wlogical-not-parentheses
#    6.5 and up : -Wnull-dereference -Wduplicated-cond -Wshift-overflow=2
ifeq ($(UNAME),Linux)
	GCC_VERSION_STRING = $(shell $(CC) -dumpversion)
	GCC_VER = $(subst ., ,$(GCC_VERSION_STRING))
	GCC_MAJOR = $(word 1,$(GCC_VER))
	GCC_MINOR = $(word 2,$(GCC_VER))
	GCC_SUBMINOR = $(word 3,$(GCC_VER))
	ifeq ($(GCC_MINOR),)
		GCC_MINOR = 0
	endif 
	ifeq ($(GCC_SUBMINOR),)
		GCC_SUBMINOR = 0
	endif
	ifeq ($(shell test $(GCC_MAJOR) -gt 4; echo $$?),0)
		ifeq ($(shell test $(GCC_MINOR) -gt 4; echo $$?),0)
			CXXFLAGS += -Wlogical-not-parentheses
		endif
	endif
	ifeq ($(shell test $(GCC_MAJOR) -gt 5; echo $$?),0)
		ifeq ($(shell test $(GCC_MINOR) -gt 4; echo $$?),0)
			CXXFLAGS += -Wnull-dereference -Wduplicated-cond -Wshift-overflow=2
		endif
	endif
endif

.PHONY: all
 
python: PROJECT_DEFINES += -DPREPYTHON
python: all 

all: clean mkoutputdir $(LIBS) done

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROJECT_DEFINES) $(INC_DIR) $< -o $@

$(LIBS): $(LIB_OBJ_FILES)
	rm -f $(FILE_OUTPUT_DIR)/$@
	$(ARCHIVE) cq $(FILE_OUTPUT_DIR)/$@ $(LIB_OBJ_FILES)
# Removing the line below since we don't need a shared object for this right now...a static library is enough for the common stuff
	$(CXX) $(LXXFLAGS) $(LIB_OBJ_FILES) -o lib$(NAME).so.$(VERSION)
	cd $(FILE_OUTPUT_DIR) && ln -s lib$(NAME).so* lib$(NAME).so

clean: banner
	rm -f $(SRC_DIR)*.o
	rm -f $(SRC_DIR)Seagate/*.o
	rm -f $(FILE_OUTPUT_DIR)/lib$(NAME).a $(FILE_OUTPUT_DIR)/lib$(NAME).so.$(VERSION) *.o ../../src/*.o ../../src/Seagate/*.o
	rm -rf $(FILE_OUTPUT_DIR)
	@echo "============================================================"
	@echo "       CLEANED"
	@echo "============================================================"
	
mkoutputdir:
	mkdir -p $(FILE_OUTPUT_DIR)

banner:
	@echo "============================================================"
	@echo "opensea-parser Makefile version: "$(MAJOR).$(MINOR).$(PATCH)  "OS: "$(OS)
	@echo "============================================================"

done:
	@echo "============================================================"
	@echo "Build of opensea-parser has completed "
	@echo "============================================================"
