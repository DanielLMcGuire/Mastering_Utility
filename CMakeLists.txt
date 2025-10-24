cmake_minimum_required(VERSION 3.15)
project(masteringutil LANGUAGES CXX VERSION 0.1.5)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(STATUS "For faster incremental builds, consider: -G \"Ninja\"")
endif()

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

if(MSVC)
    set(ADDITIONAL_RELEASE_FLAGS "/OX")
    set(ADDITIONAL_LINKER_FLAGS "/LTCG")

    set(ADDITIONAL_DEBUG_FLAGS "/Zi /Od /RTC1")
    set(ADDITIONAL_LINKER_DEBUG_FLAGS "/DEBUG")
else()
    set(ADDITIONAL_RELEASE_FLAGS "-O3 -mtune=generic -pipe -flto")
    set(ADDITIONAL_LINKER_FLAGS "-flto")

    set(ADDITIONAL_DEBUG_FLAGS "-g -O0")
    set(ADDITIONAL_LINKER_DEBUG_FLAGS "")
endif()

if(WIN32)
    enable_language(RC)
endif()
configure_file("src/launcher/resources.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/launcher.rc" @ONLY)
configure_file("src/wizard/resources.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/wizard.rc" @ONLY)

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${ADDITIONAL_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${ADDITIONAL_LINKER_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${ADDITIONAL_LINKER_DEBUG_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${ADDITIONAL_LINKER_DEBUG_FLAGS}")

set(MASTERINGUTIL_SOURCES
    src/MasteringUtil.cpp
)

add_executable(masteringutil_launcher
    ${MASTERINGUTIL_SOURCES}
    src/launcher/main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/launcher.rc
)
set_target_properties(masteringutil_launcher PROPERTIES OUTPUT_NAME MasteringUtility)

add_executable(masteringutil_wizard
    ${MASTERINGUTIL_SOURCES}
    src/wizard/main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/wizard.rc
)
set_target_properties(masteringutil_wizard PROPERTIES OUTPUT_NAME MasteringWizard)
