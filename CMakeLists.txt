cmake_minimum_required(VERSION 3.15)
project(masteringutil LANGUAGES CXX VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the build type." FORCE)
endif()

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
	message(STATUS "Tip: For faster incremental builds, try using -G \"Ninja\"")
endif()

function(get_git_commit_hash_out out_var)
	execute_process(
		COMMAND git rev-parse --short HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE _git_hash
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
	)
	if(NOT _git_hash)
		set(_git_hash "unknown")
	endif()

	execute_process(
		COMMAND git status --porcelain --untracked-files=no
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE _git_status
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
	)
	if(NOT "${_git_status}" STREQUAL "")
		set(_git_hash "@${PROJECT_VERSION}+${_git_hash}+mod")
	else()
		set(_git_hash "@${PROJECT_VERSION}+${_git_hash}")
	endif()

	set(${out_var} "${_git_hash}" PARENT_SCOPE)
endfunction()

get_git_commit_hash_out(FULL_VERSION_STRING)
message(STATUS "Mastering Utility ${FULL_VERSION_STRING}")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
	set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
	set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

option(USE_LLD "Use lld linker if available" ON)
if(USE_LLD)
	find_program(LLD_EXE lld)
	if(LLD_EXE)
		message(STATUS "Using lld linker: ${LLD_EXE}")
		if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
			set(CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
		endif()
	endif()
endif()

if(NOT MSVC)
    add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
endif()

if(MSVC)
    add_compile_options(/EHsc)

    set(RELEASE_FLAGS "/O2 /GL /Gy /GS /Oi /analyze-")
    set(LINKER_FLAGS "/LTCG /INCREMENTAL:NO /OPT:REF /OPT:ICF /DYNAMICBASE /NXCOMPAT /GUARD:CF")
    set(DEBUG_FLAGS "/Zi /Od /RTC1")
    set(LINKER_DEBUG_FLAGS "/DEBUG")

    set(RELWITHDEBINFO_FLAGS "${RELEASE_FLAGS} /Zi")
    set(MINSIZEREL_FLAGS "${RELEASE_FLAGS} /O1")
else()
    set(RELEASE_FLAGS "-O3 -mtune=generic -pipe -flto -Werror -fvisibility=hidden -ffunction-sections -fdata-sections -fstack-protector-strong -fomit-frame-pointer")
    set(LINKER_FLAGS "-flto -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now ")
    set(DEBUG_FLAGS "-g -O0")
    set(LINKER_DEBUG_FLAGS "")

    set(RELWITHDEBINFO_FLAGS "${RELEASE_FLAGS} -g")
    set(MINSIZEREL_FLAGS "${RELEASE_FLAGS} -Os")
endif()


set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${RELWITHDEBINFO_FLAGS}")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} ${MINSIZEREL_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG      "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${LINKER_DEBUG_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${LINKER_DEBUG_FLAGS}")

set(SCRIPTS_DIR "${CMAKE_SOURCE_DIR}/scripts")

if(WIN32)
	enable_language(RC)
endif()
configure_file("src/launcher/resources.rc" "${CMAKE_CURRENT_BINARY_DIR}/launcher.rc" @ONLY)
configure_file("src/wizard/resources.rc" "${CMAKE_CURRENT_BINARY_DIR}/wizard.rc" @ONLY)
configure_file("src/tests/resources.rc" "${CMAKE_CURRENT_BINARY_DIR}/tests.rc" @ONLY)

set(MASTERINGUTIL_SOURCES
	src/MasteringUtil.cpp
)

set(ARGUMENTPARSER_SOURCES
	src/argumentParser.cpp
)

file(GLOB_RECURSE LAUNCHER_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/launcher/*.cpp")
file(GLOB_RECURSE WIZARD_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/wizard/*.cpp")
file(GLOB_RECURSE TESTS_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/tests/*.cpp")

if(WIN32)
	configure_file("scripts/install.wxs" "${CMAKE_CURRENT_BINARY_DIR}/install.wxs")
endif()

add_library(masteringutil STATIC
	src/MasteringUtil.cpp
)

target_include_directories(masteringutil PUBLIC
	${CMAKE_SOURCE_DIR}/src
)

add_library(consoleutil STATIC
	${CMAKE_SOURCE_DIR}/extern/DConsole/dconsole.cpp
)

target_include_directories(consoleutil PUBLIC
	${CMAKE_SOURCE_DIR}/extern/DConsole
)

add_executable(masteringutil_launcher
	${LAUNCHER_SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/launcher.rc
)
target_link_libraries(masteringutil_launcher
	PRIVATE masteringutil consoleutil
)
set_target_properties(masteringutil_launcher PROPERTIES OUTPUT_NAME MasteringUtility)

add_executable(masteringutil_wizard
	${WIZARD_SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/wizard.rc
)
target_link_libraries(masteringutil_wizard
	PRIVATE masteringutil consoleutil
)
set_target_properties(masteringutil_wizard PROPERTIES OUTPUT_NAME MasteringWizard)

add_executable(masteringutil_tests
	${TESTS_SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/tests.rc
)
target_link_libraries(masteringutil_tests
	PRIVATE masteringutil consoleutil
)
set_target_properties(masteringutil_tests PROPERTIES OUTPUT_NAME MasteringTests)

if(WIN32)
set(MSI_FILE "${CMAKE_BINARY_DIR}/dist/masteringutil64.msi")
add_custom_command(
    OUTPUT "${MSI_FILE}"
    COMMAND "${SCRIPTS_DIR}/makemsi.cmd" ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${SCRIPTS_DIR}
    DEPENDS masteringutil_wizard masteringutil_launcher
    COMMENT "Generating MSI installer"
)

add_custom_target(build_msi ALL
    DEPENDS "${MSI_FILE}"
)

add_custom_target(install_msi
    COMMAND ${CMAKE_COMMAND} -E sleep 1
    COMMAND powershell -Command "Start-Process 'msiexec.exe' -ArgumentList '/i', '${MSI_FILE}', '/quiet', '/norestart' -Verb RunAs"
    COMMENT "Deploying MSI"
)
add_dependencies (install_msi build_msi)
endif()
