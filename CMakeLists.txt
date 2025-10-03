cmake_minimum_required(VERSION 3.21)

# Enable MSVC_RUNTIME_LIBRARY target property
# See https://cmake.org/cmake/help/latest/policy/CMP0091.html
cmake_policy(SET CMP0091 NEW)
# Set CMP0077 to new in order to configure options for build-in libraries
# See https://discourse.cmake.org/t/what-is-the-correct-way-to-set-options-of-a-project-before-fetch-content/268/6
# See also https://cmake.org/cmake/help/latest/policy/CMP0077.html
cmake_policy(SET CMP0077 NEW)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    # ExternalProject_Add sets the timestamps of extracted contents to the time of extraction
    # See https://cmake.org/cmake/help/latest/policy/CMP0135.html
    cmake_policy(SET CMP0135 NEW)
endif()

project(MACE VERSION 0.0.0
             LANGUAGES C CXX)

# =============================================================================
# Variables
# =============================================================================

# Dirs
set(MACE_PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(MACE_PROJECT_CMAKE_DIR ${MACE_PROJECT_ROOT_DIR}/cmake)
set(PROJECT_SOURCE_DIR ${MACE_PROJECT_ROOT_DIR}/src)
set(MACE_PROJECT_3RDPARTY_DIR ${MACE_PROJECT_ROOT_DIR}/thirdparty)

# Install dirs
include(GNUInstallDirs)
set(MACE_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/MACE)

# =============================================================================
# Options
# =============================================================================

include(CMakeDependentOption)

# Basic
# Options (ON/OFF)
option(MACE_BUILTIN_GENFIT "Use built-in GenFit (network or pre-downloaded source is required)." ON)
option(MACE_BUILTIN_GENFIT_EVENT_DISPLAY "Build GenFit event display module if GenFit is built-in." ON)
option(MACE_BUILTIN_MACE_OFFLINE_DATA "Use built-in offline data (network or pre-downloaded data are required)." OFF)
option(MACE_BUILTIN_MUSTARD "Use built-in Mustard (network or pre-downloaded source is required)." OFF)
option(MACE_BUILTIN_PMP "Use built-in pmp (network or pre-downloaded source is required)." OFF)
option(MACE_INSTALL_OFFLINE_DATA "Install offline data. If disabled, offline data need to be installed manually." ON)

# Advance
# Options (ON/OFF)
option(MACE_ENABLE_ASAN_IN_DEBUG_BUILD "Enable AddressSanitizer in debug build" OFF)
option(MACE_ENABLE_IPO "Build MACE with interprocedural optimization" ON)
option(MACE_ENABLE_UBSAN_IN_DEBUG_BUILD "Enable UndefinedBehaviorSanitizer in debug build" ON)
option(MACE_FULL_UNITY_BUILD "Building MACE use unity build technique COMPLETELY." OFF)
option(MACE_SHOW_EVEN_MORE_COMPILER_WARNINGS "Add compiler flag to show even more compiler warnings (GCC: -Weffc++, Clang: -Weverything, MSVC: /Wall)." OFF)
option(MACE_USE_SHARED_MSVC_RT "Select the MSVC runtime library for use (MSVC only). ON for shared (.dll), OFF for static (.lib). If you run into link error related to MSVC runtime, you can try flip this." ON)
option(MACE_WITH_DEBUG_INFO "Always add debugging information" OFF)
cmake_dependent_option(MACE_ENABLE_UNITY_BUILD "Build MACE with unity build technique." OFF "NOT MACE_FULL_UNITY_BUILD" ON)
cmake_dependent_option(MACE_SHOW_MORE_COMPILER_WARNINGS "Show compiler warnings raise from external dependencies." OFF "NOT MACE_SHOW_EVEN_MORE_COMPILER_WARNINGS" ON)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build using shared libraries")
set(MACE_BUILTIN_MACE_OFFLINE_DATA_VERSION 0.0.1 CACHE STRING "Set built-in offline data version (if required).")
set(MACE_BUILTIN_MUSTARD_VERSION 0.25.930 CACHE STRING "Set built-in Mustard version (if required).")
set(MACE_BUILTIN_PMP_VERSION 3.0.0 CACHE STRING "Set built-in pmp version (if required).")

# =============================================================================
# Installation path
# =============================================================================

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE ${MACE_PROJECT_ROOT_DIR}/install)
endif()
message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX} (add -DCMAKE_INSTALL_PREFIX=<install_path> into CMake command if you wish to change it)")

# =============================================================================
# Global compile options
# =============================================================================

include(${MACE_PROJECT_CMAKE_DIR}/GlobalCompileConfig.cmake)

# =============================================================================
# Dependencies
# =============================================================================

# Required:

# Mustard
include(${MACE_PROJECT_CMAKE_DIR}/LookForMustard.cmake)

# Required, built-in if not found:

# GenFit
include(${MACE_PROJECT_CMAKE_DIR}/LookForGenFit.cmake)
# PMP
include(${MACE_PROJECT_CMAKE_DIR}/LookForPMP.cmake)

# data
if(MACE_INSTALL_OFFLINE_DATA)
    include(${MACE_PROJECT_CMAKE_DIR}/LookForMACEOfflineData.cmake)
else()
    message(NOTICE "***Notice: Offline data installation disabled, offline data need to be installed manually")
endif()

# =============================================================================
# Let's go!
# =============================================================================

# Config for MACE
include(${MACE_PROJECT_CMAKE_DIR}/MACECompileConfig.cmake)
# libraries
add_subdirectory(${PROJECT_SOURCE_DIR})
# main program
add_executable(MACE MACE.c++)
target_link_libraries(MACE AppMACEPhaseI
                           AppMACEReconstruction
                           AppMACESimulation
                           AppMACEUtility
                           Mustard::Mustard)
install(TARGETS MACE DESTINATION ${CMAKE_INSTALL_BINDIR})
