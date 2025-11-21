cmake_minimum_required(VERSION 3.15)
project(masteringutil LANGUAGES CXX VERSION 2.0.1)

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
    set(RELEASE_FLAGS "/O2 /GL /Gy /GS /Oi /analyze- /W4")
    set(DEBUG_FLAGS "/Zi /Od /RTC1")
else()
    set(RELEASE_FLAGS "-O3 -mtune=generic -pipe -flto -fvisibility=hidden -ffunction-sections -fdata-sections -fstack-protector-strong -fomit-frame-pointer -Wall -Wextra -Wpedantic")
    set(DEBUG_FLAGS "-g -O0")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")

set(MASTERINGUTIL_SOURCES
    src/backend/cpp/MasteringUtil.cpp
)

set(TESTS_SOURCES
    src/frontend/tests/cpp/tests.cpp
)

set(LAUNCHER_SOURCES
    src/frontend/launcher/cpp/launcher.cpp
)

set(WIZARD_SOURCES
    src/frontend/wizard/cpp/wizard.cpp
) 

include(FetchContent)

FetchContent_Declare(
    DConsole
    GIT_REPOSITORY https://github.com/DanielLMcGuire/DConsole.git
    GIT_TAG main
)

FetchContent_MakeAvailable(DConsole)

add_library(masteringutil STATIC ${MASTERINGUTIL_SOURCES})

target_include_directories(masteringutil PUBLIC
    ${CMAKE_SOURCE_DIR}/src/backend/cpp
)

target_link_libraries(masteringutil)
set_target_properties(masteringutil PROPERTIES OUTPUT_NAME masteringutil-cpp)

add_executable(masteringutil_tests ${TESTS_SOURCES})
target_link_libraries(masteringutil_tests
	PRIVATE masteringutil DConsole
)
set_target_properties(masteringutil_tests PROPERTIES OUTPUT_NAME MasteringTests)

add_executable(masteringutil_wizard ${WIZARD_SOURCES})
target_link_libraries(masteringutil_wizard
	PRIVATE masteringutil DConsole
)
set_target_properties(masteringutil_wizard PROPERTIES OUTPUT_NAME MasteringWizard-cpp)

add_executable(masteringutil_launcher ${LAUNCHER_SOURCES})
target_link_libraries(masteringutil_launcher
	PRIVATE masteringutil DConsole
)
set_target_properties(masteringutil_launcher PROPERTIES OUTPUT_NAME MasteringUtility-cpp)