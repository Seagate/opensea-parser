
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


project('opensea-parser', 'cpp',license: 'MPL-2.0', version : '1.0.2', default_options : ['warning_level=2', 'default_library=static'] )
message('opensea_parser')

opensea_src_dir = './src/'
include_dir = ['./include', './include/Seagate']

cpp = meson.get_compiler('cpp')
cpp_version = cpp.version()

project_defines = []
project_args = []
linker_flags = []

if cpp.get_id() == 'msvc'
  project_args += ['/std:c++17']
elif cpp.has_argument('-std=gnu++17')
  project_args += ['-std=gnu++17']
elif cpp.has_argument('-std=gnu++11')
  project_args += ['-std=gnu++11']
else
  error('Compiler does not support C++11 or C++17')
endif

add_project_arguments(project_args, language : 'cpp')

machine = target_machine.system()
architecture = target_machine.cpu_family()

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

if get_option('debug')
  add_project_arguments('-D_DEBUG', language : 'c')
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
        '-Wno-extra-semi',  # codebase widely uses 'ctor() {};' style; too noisy to be useful here
        '-Wcast-qual',
        '-Werror=sometimes-uninitialized',
        '-Wuninitialized',
        '-Wunevaluated-expression',
        '-Wno-unsequenced',
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
		'-Wno-template-body',  # GCC 14+ -Wtemplate-body false-positives/hard-errors in libstdc++ headers (e.g. exception_ptr.h make_exception_ptr); disable on all archs, not just aarch64.
		'-Wno-attributes'  # code_attributes.h/env_detect.h feature-detection macros trip 'attribute directive ignored' noise
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
		message('ppc64')
        #power pc builds generate a lot of warnings/notes about ABI changes since GCC-5
        #this flag is disabling them because this is way too noisy.
        warning_flags += ['-Wno-psabi']
    elif target_machine.cpu_family() == 'x86_64'
		message('x86_64')
        warning_flags += ['-fcf-protection=full']        #this may be linux only at this time.
    elif target_machine.cpu_family() == 'aarch64'
		message('aarch64')
        warning_flags += ['-mbranch-protection=standard']
    endif

elif cpp.get_id().contains('msvc')
    #See here for enabling/disabling msvc warnings:
    #https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170
    #warnings off by default: https://learn.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default?view=msvc-170
    warning_flags = [
        #Turn off the following warnings. If using /wall in Windows, many of these show all over the Windows API
        #This is likely not an issue with meson, but matching VS project files for now
        '/wd4214',  # nonstandard extension used : bit field types other than int
        '/wd4201',  # nonstandard extension used : nameless struct/union
        '/wd4668',  # 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'. While like -Wundef, this creates too many warnings in system headers to use
        '/wd4820',  # 'bytes' bytes padding added after construct 'member_name'
        '/wd4710',  # 'function' : function not inlined
        '/wd5045',  # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        '/wd4711',  # function 'function' selected for inline expansion
        '/wd4324',  # 'struct_name' : structure was padded due to __declspec(align())
        '/wd4221',  # nonstandard extension used : 'identifier' : cannot be initialized using address of automatic variable
        '/wd4204',  # nonstandard extension used : non-constant aggregate initializer
        '/wd5105',  # macro expansion producing 'defined' has undefined behavior
        '/wd4746',  # volatile access of '<expression>' is subject to /volatile:[iso|ms] setting; consider using __iso_volatile_load/store intrinsic functions.
        #Turn on the following warnings to make the output more useful or like GCC/clang
        '/w14255',  # 'function' : no function prototype given: converting '()' to '(void)'
        '/w14062',  # enumerator 'identifier' in switch of enum 'enumeration' is not handled
        '/w14101',  # 'identifier' : unreferenced local variable
        '/w14189',  # 'identifier' : local variable is initialized but not referenced
        '/w15031',  # #pragma warning(pop): likely mismatch, popping warning state pushed in different file
        '/w15032',  # detected #pragma warning(push) with no corresponding #pragma warning(pop)
        '/w15262',  # implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]] when a break statement is intentionally omitted between cases
        '/w14255',  # 'function' : no function prototype given: converting '()' to '(void)' #NOTE: Only needed for /Wall, otherwise enabling can be good-TJE
        '/w14242',  # identifier conversion from type 1 to type 2, possible loss of data (matches -wconversion above)
        '/w14254',  # operator conversion from type 1 to type 2, possible loss of data (matches -wconversion above)
        '/w14287',  # operator: unsigned/negative constant mismatch (matches -wconversion above)
        '/w14296',  # operator: expression is always false
        '/w14365',  # action: conversion from type 1 to type 2, signed/unsigned mismatch (matches -wconversion above)
        '/w14388',  # implicit conversion warning during a comparison (matches -wconversion above)
        '/w14545',  # expression before comma evaluates to a function which is missing an argument list
        '/w14546',  # function call before comma missing argument list
        '/w14547',  # 'operator' : operator before comma has no effect; expected operator with side-effect
        '/w14548',  # expression before comma has no effect; expected expression with side-effect
        '/w14549',  # 'operator1': operator before comma has no effect; did you intend 'operator2'?
        '/w14574',  # 'identifier' is defined to be '0': did you mean to use '#if identifier'?
        '/w14605',  # 	'/Dmacro' specified on current command line, but was not specified when precompiled header was built
        '/w14555',  # expression has no effect; expected expression with side-effect
        '/w14774',  # 'string' : format string expected in argument number is not a string literal
        '/w14777',  # 'function' : format string 'string' requires an argument of type 'type1', but variadic argument number has type 'type2'
        '/w14826',  # Conversion from 'type1' to 'type2' is sign-extended. This may cause unexpected runtime behavior (more -wconversion)
        '/w15219',  # implicit conversion from 'type-1' to 'type-2', possible loss of data (-wconversion)
        '/w15240',  # 'attribute-name': attribute is ignored in this syntactic position
        '/w15245',  # 'function': unreferenced function with internal linkage has been removed
        '/w14555',  # expression has no effect; expected expression with side-effect
        '/w15264',  # 'variable-name': 'const' variable is not used
        '/w24302',  # 'conversion': truncation from 'type1' to 'type2'
        '/w14311',  # 'variable': pointer truncation from 'type' to 'type'
        '/w14312',  # 'operation': conversion from 'type1' to 'type2' of greater size
        '/w14319',  # 'operator': zero extending 'type1' to 'type2' of greater size
        '/w14702',  # warning for unreachable code
        #Treat the following as errors
        '/we4431',  # missing type specifier - int assumed. Note: C no longer supports default-int
        '/we4905',  # wide string literal cast to 'LPSTR'
        '/we4906',  # string literal cast to 'LPWSTR'
        '/we4837',  # trigraph detected: '??character' replaced by 'character'
        '/we4628',  # digraphs not supported with -Ze. Character sequence 'digraph' not interpreted as alternate token for 'char'
        '/we4289',  # nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope
        #'/we4464',  # relative include path contains '..'
        '/GS',  #security cookie for stack protection
        '/sdl',  #adds recommended security development lifecycle checks
        '/Qspectre',
        '/guard:cf',  #control flow guard
        '/d2guard4',  #control flow guard
    ]

    if target_machine.cpu_family() == 'x86_64' or target_machine.cpu_family() == 'x86'
        # https://learn.microsoft.com/en-us/cpp/build/reference/qintel-jcc-erratum?view=msvc-170
        warning_flags += '/QIntel-jcc-erratum'
    endif

    linker_flags += [
        '/guard:cf',  #control flow guard
        '/SafeSEH',  #on by default in x64 so it is unrecognized otherwise.
        '/NXCOMPAT',  #data execution prevention
        '/dynamicbase',  #address space randomization
    ]
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

opensea_common = subproject('opensea-common')
opensea_common_dep = opensea_common.get_variable('opensea_common_dep')

libjson = subproject('libjson')
libjson_dep = libjson.get_variable('libjson_dep')

opensea_parser = static_library('opensea_parser', lib_src_files, cpp_args: cpp.get_supported_arguments(warning_flags), dependencies: [opensea_common_dep,libjson_dep] ,include_directories : include_dir)

opensea_parser_dep = declare_dependency(link_with : opensea_parser, include_directories : include_dir)


