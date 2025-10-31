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

project(MACESW VERSION 0.0.0
               LANGUAGES C CXX)

# =============================================================================
# Variables
# =============================================================================

# Dirs
set(MACESW_PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(MACESW_PROJECT_CMAKE_DIR ${MACESW_PROJECT_ROOT_DIR}/cmake)
set(MACESW_PROJECT_DOCS_DIR ${MACESW_PROJECT_ROOT_DIR}/docs)
set(MACESW_PROJECT_SOURCE_DIR ${MACESW_PROJECT_ROOT_DIR}/src)
set(MACESW_PROJECT_3RDPARTY_DIR ${MACESW_PROJECT_ROOT_DIR}/thirdparty)
set(MACESW_PROJECT_TOOL_DIR ${MACESW_PROJECT_ROOT_DIR}/tooling)

# Install dirs
include(GNUInstallDirs)
set(MACESW_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR}/MACE)

# =============================================================================
# Options
# =============================================================================

# Options (in alphabetical order) (ON/OFF)
option(MACESW_BUILTIN_GENFIT "Use built-in GenFit (network or pre-downloaded source is required)." ON)
option(MACESW_BUILTIN_GENFIT_EVENT_DISPLAY "Build GenFit event display module if GenFit is built-in." ON)
option(MACESW_BUILTIN_MACESW_OFFLINE_DATA "Use built-in offline data (network or pre-downloaded data are required)." OFF)
option(MACESW_BUILTIN_MACESW_TEST_DATA "Use built-in test data (network or pre-downloaded data are required)." OFF)
option(MACESW_BUILTIN_MUSTARD "Use built-in Mustard (network or pre-downloaded source is required)." OFF)
option(MACESW_BUILTIN_PMP "Use built-in pmp (network or pre-downloaded source is required)." OFF)
option(MACESW_CLANG_FORMAT "Requires source code format to be checked by clang-format (requires clang-format)" ON)
option(MACESW_CLANG_FORMAT_WERROR "Let clang-format treat warnings as errors (only takes effect when MACESW_CLANG_FORMAT=ON)" OFF)
option(MACESW_CLANG_TIDY "Requires source code to be analysed by clang-tidy (requires clang-tidy)" OFF)
option(MACESW_CLANG_TIDY_AMEND_UNITY_BUILD "Amend unity-build-missed clang-tidy checks in normal build (only takes effect when both MACESW_CLANG_TIDY=ON and MACESW_UNITY_BUILD=OFF)" OFF)
option(MACESW_CLANG_TIDY_WERROR "Let clang-tidy treat warnings as errors (only takes effect when MACESW_CLANG_TIDY=ON)" OFF)
option(MACESW_ENABLE_ASAN_IN_DEBUG_BUILD "Enable AddressSanitizer in debug build" OFF)
option(MACESW_ENABLE_IPO "Build MACESW with interprocedural optimization" ON)
option(MACESW_ENABLE_UBSAN_IN_DEBUG_BUILD "Enable UndefinedBehaviorSanitizer in debug build" ON)
option(MACESW_FULL_UNITY_BUILD "Building MACESW use unity build technique COMPLETELY." OFF)
option(MACESW_INSTALL_OFFLINE_DATA "Install offline data. If disabled, offline data need to be installed manually." ON)
option(MACESW_SHOW_EVEN_MORE_COMPILER_WARNINGS "Add compiler flag to show even more compiler warnings (GCC: -Weffc++, Clang: -Weverything, MSVC: /Wall)." OFF)
option(MACESW_SHOW_MORE_COMPILER_WARNINGS "Show compiler warnings raise from external dependencies." OFF)
option(MACESW_UNITY_BUILD "Build MACESW with unity build technique." OFF)
option(MACESW_USE_SHARED_MSVC_RT "Select the MSVC runtime library for use (MSVC only). ON for shared (.dll), OFF for static (.lib). If you run into link error related to MSVC runtime, you can try flip this." ON)
option(MACESW_WERROR "Treat MACESW compilation warnings as errors." OFF)
option(MACESW_WITH_DEBUG_INFO "Always add debugging information" OFF)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build using shared libraries")
set(MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION 0.25.103013 CACHE STRING "Set built-in offline data version (if required).")
set(MACESW_BUILTIN_MACESW_TEST_DATA_VERSION 0.25.10301318 CACHE STRING "Set built-in test data version (if required).")
set(MACESW_BUILTIN_MUSTARD_VERSION 0.25.1030 CACHE STRING "Set built-in Mustard version (if required).")
set(MACESW_BUILTIN_PMP_VERSION 3.0.0 CACHE STRING "Set built-in pmp version (if required).")

# =============================================================================
# Installation path
# =============================================================================

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE ${MACESW_PROJECT_ROOT_DIR}/install)
endif()
message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX} (add -DCMAKE_INSTALL_PREFIX=<install_path> into CMake command if you wish to change it)")

# =============================================================================
# Global compile options
# =============================================================================

include(${MACESW_PROJECT_CMAKE_DIR}/GlobalCompileConfig.cmake)

# =============================================================================
# Dependencies
# =============================================================================

# Required:

# Mustard
include(${MACESW_PROJECT_CMAKE_DIR}/LookForMustard.cmake)

# Required, built-in if not found:

# GenFit
include(${MACESW_PROJECT_CMAKE_DIR}/LookForGenFit.cmake)
# PMP
include(${MACESW_PROJECT_CMAKE_DIR}/LookForPMP.cmake)

# data
if(MACESW_INSTALL_OFFLINE_DATA)
    include(${MACESW_PROJECT_CMAKE_DIR}/LookForMACEOfflineData.cmake)
else()
    message(NOTICE "***Notice: Offline data installation disabled, offline data need to be installed manually")
endif()

# test data
include(${MACESW_PROJECT_CMAKE_DIR}/LookForMACETestData.cmake)

# =============================================================================
# MACESW configs
# =============================================================================

# Config for MACESW
include(${MACESW_PROJECT_CMAKE_DIR}/MACESWCompileConfig.cmake)

# Format MACESW source code
include(${MACESW_PROJECT_CMAKE_DIR}/MACESWClangFormat.cmake)

# Lint MACESW source code
include(${MACESW_PROJECT_CMAKE_DIR}/MACESWClangTidy.cmake)

# =============================================================================
# Let's go!
# =============================================================================

# libraries
add_subdirectory(${MACESW_PROJECT_SOURCE_DIR})
# main program
add_executable(MACE MACE.c++)
target_link_libraries(MACE AppMACEPhaseI
                           AppMACEReconstruction
                           AppMACESimulation
                           AppMACEUtility
                           Mustard::Mustard)
install(TARGETS MACE DESTINATION ${CMAKE_INSTALL_BINDIR})
