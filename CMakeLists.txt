cmake_minimum_required(VERSION 3.15)
project(masteringutil LANGUAGES CXX VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the build type." FORCE)
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

if(MSVC)
    add_compile_options(/EHsc)
    set(RELEASE_FLAGS "/O2 /GL /Gy /GS /Oi /analyze-")
    set(DEBUG_FLAGS "/Zi /Od /RTC1")
else()
    set(RELEASE_FLAGS "-O3 -mtune=generic -pipe -flto -fvisibility=hidden -ffunction-sections -fdata-sections -fstack-protector-strong -fomit-frame-pointer")
    set(DEBUG_FLAGS "-g -O0")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")

set(MASTERINGUTIL_SOURCES
    src/MasteringUtil.cpp
)

add_library(masteringutil STATIC ${MASTERINGUTIL_SOURCES})

target_include_directories(masteringutil PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(masteringutil)
