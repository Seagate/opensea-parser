
# ---------------------------------------------
# File: meson.build
# Author: Ankur Gaikwad
# Last Modified: 3/26/2024 
#
# Seagate 2024
#
# Description: Builds ParserSolutions executables using 
# meson. Places executables in meson base folder.
# ---------------------------------------------


project('opensea-parser', 'cpp')


opensea_src_dir = './src/'
include_dir = ['./include', './include/Seagate']

cc = meson.get_compiler('cpp')

project_defines = []

cxx_flags = ['-Wall', '-Wextra', '-Werror=old-style-cast', '-Wshadow', '-Wmaybe-uninitialized', '-Wvla', '-Wfloat-equal', '-Wlogical-op', '-Wdouble-promotion', '-Wformat-security']
cxx_flags += ['-pic', '-std=c++11', '-I.']


lib_src_files = files(
   opensea_src_dir+ 'Opensea_Parser_Helper.cpp', opensea_src_dir + 'CLog.cpp' , opensea_src_dir + 'CAta_Ext_DST_Log.cpp', opensea_src_dir + 'CAta_Device_Stat_Log.cpp', opensea_src_dir + 'CAta_Ext_Comprehensive_Log.cpp' , opensea_src_dir + 'CAta_Identify_Log.cpp', opensea_src_dir + 'CAta_Power_Condition_Log.cpp' , 

opensea_src_dir + 'CAta_NCQ_Command_Error_Log.cpp', opensea_src_dir + 'CAta_SMART_Log_Dir.cpp', opensea_src_dir + 'CScsi_Log.cpp', opensea_src_dir + 'CScsi_Application_Client_Log.cpp',

opensea_src_dir + 'CScsi_Background_Operation_Log.cpp', 

opensea_src_dir + 'CScsi_Background_Scan_Log.cpp',
    opensea_src_dir + 'CScsi_Cache_Statistics_Log.cpp',
    opensea_src_dir + 'CScsi_Environmental_Logs.cpp',
    opensea_src_dir + 'CScsi_Temperature_Log.cpp',
    opensea_src_dir + 'CScsi_Error_Counter_Log.cpp',
    opensea_src_dir + 'CScsi_Factory_Log.cpp',
    opensea_src_dir + 'CScsi_Format_Status_Log.cpp',
    opensea_src_dir + 'CScsi_Informational_Exeptions_Log.cpp',
    opensea_src_dir + 'CScsi_Logical_Block_Provisioning_Log.cpp',
    opensea_src_dir +  'CScsi_Non_Medium_Error_Count_Log.cpp',
    opensea_src_dir +  'CScsi_Page_19h_Cache_Memory_Statistics_Log.cpp',
    opensea_src_dir + 'CScsi_Page_19h_Command_Duration_Limits_Log.cpp',
    opensea_src_dir + 'CScsi_Pending_Defects_Log.cpp',
    opensea_src_dir + 'CScsi_Power_Condition_Transitions_Log.cpp',
    opensea_src_dir + 'CScsi_Protocol_Specific_Port_Log.cpp',
    opensea_src_dir + 'CScsi_Self_Test_Results_Log.cpp',
    opensea_src_dir + 'CScsi_Solid_State_Drive_Log.cpp',
    opensea_src_dir + 'CScsi_Start_Stop_Cycle_Counter_Log.cpp',
    opensea_src_dir + 'CScsi_Supported_LogPages_Log.cpp',
    opensea_src_dir + 'CScsi_Zoned_Device_Statistics_Log.cpp',
    opensea_src_dir + '/Seagate/Farm_Common.cpp',
    opensea_src_dir + '/Seagate/CFARM_Log.cpp',
    opensea_src_dir + '/Seagate/CAta_Farm_Log.cpp',
    opensea_src_dir + '/Seagate/CScsi_Farm_Log.cpp'
)



# Add more warnings for specific GCC versions
if cc.get_id() == 'gcc'
    gcc_ver = cc.version()
    if gcc_ver.version_compare('>=5.5')
        cxx_flags += '-Wlogical-not-parentheses'
    endif
    if gcc_ver.version_compare('>=6.5')
        cxx_flags += ['-Wnull-dereference', '-Wduplicated-cond', '-Wshift-overflow=2']
    endif
endif


opensea_common = subproject('opensea-common')
opensea_common_dep = opensea_common.get_variable('opensea_common_dep')

libjson = subproject('libjson')
libjson_dep = libjson.get_variable('libjson_dep')


opensea_parser = static_library('opensea_parer', lib_src_files, c_args: cxx_flags, dependencies: [opensea_common_dep,libjson_dep] ,include_directories : include_dir)

opensea_parser_dep = declare_dependency(link_with : opensea_parser, include_directories : include_dir)


