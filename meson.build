
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


project('opensea-parser', 'cpp', version : '1.0.1', default_options : ['cpp_std=gnu++17'] )
message('opensea_parser')

opensea_src_dir = './src/'
include_dir = ['./include', './include/Seagate']

cc = meson.get_compiler('cpp')

project_defines = []

cpp = meson.get_compiler('cpp')
cpp_version = cpp.version()

machine = target_machine.system()
architecture = target_machine.cpu_family()

#cxx_flags = ['-Wall', '-Wextra', '-Werror=old-style-cast', '-Wshadow', '-Wmaybe-uninitialized', '-Wvla', '-Wfloat-equal', '-Wlogical-op', '-Wdouble-promotion', '-Wformat-security']
#cxx_flags += ['-pic', '-std=c++11', '-I.']

# Keep a small set of project-specific flags per buildtype.
cpp_args = []

# Ensure older GCC compilers get gnu++11 if they don't default to C++11
if cpp.get_id().contains('gcc')
    if cpp.version().version_compare('<5.0')
        if cpp.has_argument('-std=gnu++11')
            # Apply project-local C++11 flag for old compilers only
            add_project_arguments('-std=gnu++11', language: 'cpp')
        else
            error('C++11/GNU++11 standard is required but was not able to be set!')
        endif
    endif
endif

if cpp.get_id().contains('gcc') or cpp.get_id().contains('clang')
    #TODO: Add -Wcast-align=strict and fix these issues to help ensure better portability
    #NOTE: -Wsign-conversion can be useful while debugging, but there are numerous places this shows up
    #      and it is not useful, so only add it while debugging.
    #NOTE: 4/4/2024 - adding flags from https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++
    warning_flags = [
        #	'-Wcast-align=strict',
        '-Wshadow=compatible-local',
        '-Wvla',
        '-Wfloat-equal',
        '-Wnull-dereference',
        '-Wunused-const-variable',
        '-Wunused-parameter',
        '-Wunused-value',
        '-Wduplicated-cond',
        '-Wjump-misses-init',
        '-Wstringop-overflow',
        '-Wlogical-op',
        '-Wshift-overflow',
        '-Wshift-overflow=1',
        '-Wshift-overflow=2',
        '-Wdouble-promotion',
        '-Wformat-security',
        '-Wold-style-definition',
        '-Wstrict-prototypes',
        '-Wmissing-declarations',
        '-Wmissing-prototypes',
        '-Wchar-subscripts',
        '-Wundef',
        '-Wformat',
        '-Wformat=2',
        '-Wint-conversion',  #-Warith-conversion
        '-Wenum-conversion',
        '-Wfloat-conversion',
        '-Wint-to-pointer-cast',
        '-Wimplicit-fallthrough',
        '-D_GLIBCXX_ASSERTIONS',
        '-fstrict-flex-arrays=3',
        '-fno-delete-null-pointer-checks',
        '-fno-strict-overflow',
        '-fno-strict-aliasing',
        '-ftrivial-auto-var-init=zero',
        '-Wtrampolines',  #GCC only at this time
        '-Wincompatible-pointer-types-discards-qualifiers',
        '-Woverlength-strings',
        '-Wnewline-eof',
        '-Wno-c23-extensions',  #We do not want this warning since we are already checking for when C23 extensions are available before we use them. If not, we use a compiler specific definition, or make it an empty definition.
        '-Wparentheses',
        '-Wextra-semi',
        '-Wcast-qual',
        '-Werror=sometimes-uninitialized',
        '-Wuninitialized',
        '-Wunevaluated-expression',
        '-Wunsequenced',
        '-Wvarargs',
        '-Wwrite-strings',
        '-Wrestrict',
        '-Wstringop-truncation',
        '-Werror=trigraphs',
        '-Wunreachable-code',
        '-Wcomment',
        '-Wsequence-point',
        '-Wreturn-type',
        '-Wpointer-bool-conversion',
        '-Wmismatched-dealloc',
        '-Wfree-nonheap-object',
        '-fnullability',
        '-Wnullability',  # Warnings for nullability annotations
        '-Wnullability-completeness',  # Warn if nullability annotations are incomplete
        '-Wnullability-completeness-on-arrays',  # Warn if nullability annotations are incomplete
        # '-Wnullable-to-nonnull-conversion', # Basically warns so you have to cast to the correct nullability type. Not needed today -TJE
        '-Wswitch-enum',
        '-Walloc-zero',
        '-Walloc-size',
        '-Wcalloc-transposed-args',
        '-Werror=alloca',  # Do not allow use of the alloca function
        '-Wstring-compare',  # Helps detect when strcmp is used when the programmer likely meant to use strncmp
    ]

	if cpp.get_id().contains('gcc') and target_machine.system() == 'windows'
        #According to the link below, this is not needed in Windows...it also causes a bug in some versions of GCC for Windows.
        #https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90458
        #NOTE: While this appears to be fixed for versions 11 and 12, the current CI is failing on this with
        #      version 12.2. If we want to or need to enable this, it should be done based on which versions we
        #      know have been patched for this. -TJE
    else
        warning_flags += '-fstack-clash-protection'
    endif

    if target_machine.cpu_family() == 'ppc64'
        #power pc builds generate a lot of warnings/notes about ABI changes since GCC-5
        #this flag is disabling them because this is way too noisy.
        warning_flags += ['-Wno-psabi']
    elif target_machine.cpu_family() == 'x86_64'
        warning_flags += ['-fcf-protection=full']        #this may be linux only at this time.
    elif target_machine.cpu_family() == 'aarch64'
        warning_flags += ['-mbranch-protection=standard']
    endif


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


opensea_parser = static_library('opensea_parser', lib_src_files, c_args: cxx_flags, dependencies: [opensea_common_dep,libjson_dep] ,include_directories : include_dir)

opensea_parser_dep = declare_dependency(link_with : opensea_parser, include_directories : include_dir)


