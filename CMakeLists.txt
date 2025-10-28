cmake_minimum_required(VERSION 3.15)
project(masteringutil LANGUAGES CXX VERSION 0.9.1)

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

if(MSVC)
    add_compile_options(/EHsc)
endif()


if(MSVC)
    set(ADDITIONAL_RELEASE_FLAGS "/OX /WX")
    set(ADDITIONAL_LINKER_FLAGS "/LTCG")
    set(ADDITIONAL_DEBUG_FLAGS "/Zi /Od /RTC1")
    set(ADDITIONAL_LINKER_DEBUG_FLAGS "/DEBUG")
else()
    set(ADDITIONAL_RELEASE_FLAGS "-O3 -mtune=generic -pipe -flto -Werror")
    set(ADDITIONAL_LINKER_FLAGS "-flto")
    set(ADDITIONAL_DEBUG_FLAGS "-g -O0")
    set(ADDITIONAL_LINKER_DEBUG_FLAGS "")
endif()

set(CMAKE_EXE_LINKER_FLAGS_RELEASE   "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${ADDITIONAL_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${ADDITIONAL_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG     "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${ADDITIONAL_LINKER_DEBUG_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${ADDITIONAL_LINKER_DEBUG_FLAGS}")

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

add_executable(masteringutil_launcher
    ${MASTERINGUTIL_SOURCES}
    ${LAUNCHER_SOURCES}
    ${ARGUMENTPARSER_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/launcher.rc
)
set_target_properties(masteringutil_launcher PROPERTIES OUTPUT_NAME MasteringUtility)

add_executable(masteringutil_wizard
    ${MASTERINGUTIL_SOURCES}
    ${ARGUMENTPARSER_SOURCES}
    ${WIZARD_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/wizard.rc
)
set_target_properties(masteringutil_wizard PROPERTIES OUTPUT_NAME MasteringWizard)

add_executable(masteringutil_tests
    ${MASTERINGUTIL_SOURCES}
    ${TESTS_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/tests.rc
)
set_target_properties(masteringutil_tests PROPERTIES OUTPUT_NAME MasteringTests)
