cmake_minimum_required(VERSION 3.15)
project(masteringutil LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This project requires an x64 (64-bit) architecture.")
endif()

# Default build type for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Recommend Ninja for faster builds
if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(STATUS "For faster incremental builds, consider configuring with the Ninja generator: -G \"Ninja\"")
endif()

# Optional: ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# MSVC: enable parallel compile
if(MSVC)
    add_compile_options(/MP)
endif()

# Optional: lld linker
option(USE_LLD "Use lld linker if available" ON)
if(USE_LLD)
    find_program(LLD_EXE lld)
    if(LLD_EXE)
        message(STATUS "Using lld linker: ${LLD_EXE}")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
        endif()
    endif()
endif()

# Release optimization flags
if(MSVC)
    set(ADDITIONAL_RELEASE_FLAGS "/O2 /Ot /GL")
    set(ADDITIONAL_LINKER_FLAGS "/LTCG")
else()
    set(ADDITIONAL_RELEASE_FLAGS "-O3 -march=native -pipe -flto")
    set(ADDITIONAL_LINKER_FLAGS "-flto")
endif()

# Windows RC
if(WIN32)
    enable_language(RC)
endif()

# Parallel compile (MSVC)
if(MSVC)
    add_compile_options(/MP)
endif()

# Apply release flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ADDITIONAL_RELEASE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${ADDITIONAL_LINKER_FLAGS}")

# Sources
set(MASTERINGUTIL_SOURCES
    src/MasteringUtil.cpp
)

# --- Mastering Utility Launcher ---
add_executable(masteringutil_launcher
    ${MASTERINGUTIL_SOURCES}
    src/launcher/main.cpp
    src/launcher/resources.rc
)
set_target_properties(masteringutil_launcher PROPERTIES OUTPUT_NAME MasteringUtility)

# --- Wizard version ---
add_executable(masteringutil_wizard
    ${MASTERINGUTIL_SOURCES}
    src/wizard/main.cpp
    src/wizard/resources.rc
)
set_target_properties(masteringutil_wizard PROPERTIES OUTPUT_NAME MasteringWizard)
