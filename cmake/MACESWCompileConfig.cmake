# Copyright (C) 2020-2025  MACESW developers
#
# This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
# offline software.
#
# MACESW is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# MACESW. If not, see <https://www.gnu.org/licenses/>.

# This file is included after "find_package"s and "LookFor"s.

# =============================================================================
# MACESW at C++20
# =============================================================================

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard.")
if(CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "MACESW should be built, at least, with C++20")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "MACESW will be compiled with C++${CMAKE_CXX_STANDARD}")

# =============================================================================
# By default, no C++ extensions available for MACESW
# =============================================================================

set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "Boolean specifying whether compiler specific extensions are requested.")

# =============================================================================
# Unity build for MACESW
# =============================================================================

if(MACESW_UNITY_BUILD)
    set(CMAKE_UNITY_BUILD ON)
    if(MACESW_FULL_UNITY_BUILD)
        set(CMAKE_UNITY_BUILD_BATCH_SIZE 0)
    else()
        set(CMAKE_UNITY_BUILD_BATCH_SIZE 8)
    endif()
    if(CMAKE_UNITY_BUILD_BATCH_SIZE GREATER 0)
        message(STATUS "Unity build enabled for MACESW (batch size: ${CMAKE_UNITY_BUILD_BATCH_SIZE})")
    else()
        message(STATUS "Unity build enabled for MACESW (batch size: unlimited)")
    endif()
endif()

# =============================================================================
# LTO/IPO for MACESW
# =============================================================================

if(MACESW_ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT MACESW_ENABLE_IPO_SUPPORTED
                        OUTPUT MACESW_IPO_SUPPORTED_ERROR)
    if(MACESW_ENABLE_IPO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
        message(STATUS "LTO/IPO enabled for MACESW")
    else()
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF)
        message(NOTICE "***Notice: LTO/IPO not supported. Turning off CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE")
    endif()
endif()

# =============================================================================
# Compile options and definitions for MACESW
# =============================================================================

set(MACESW_COMPILE_OPTIONS "")
set(MACESW_COMPILE_DEFINITIONS "")

# =============================================================================
# Compile warnings for MACESW
# =============================================================================

# Surpress some, if required
if(NOT MACESW_SHOW_MORE_COMPILER_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # nothing for now
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|IntelLLVM)$")
        # nothing for now
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # TODO: below is ancient; need update.
        # # ROOT (conditional expression is constant)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd4127)
        # # Common ('argument': conversion from 'type1' to 'type2', possible loss of data)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd4244)
        # # Common ('var': conversion from 'size_t' to 'type', possible loss of data)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd4267)
        # # MSVC std::tuple ('derived class' : destructor was implicitly defined as deleted because a base class destructor is inaccessible or deleted)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd4624)
        # # Common (The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd4819)
        # # ROOT (using a function, class member, variable, or typedef that's marked deprecated)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd4996)
        # # Eigen (operator 'operator-name': deprecated between enumerations of different types)
        # list(APPEND MACESW_COMPILE_OPTIONS /wd5054)
    endif()

# Even more warnings, if required
elseif(MACESW_SHOW_EVEN_MORE_COMPILER_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND MACESW_COMPILE_OPTIONS -Weffc++)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|IntelLLVM)$")
        list(APPEND MACESW_COMPILE_OPTIONS -Weverything)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND MACESW_COMPILE_OPTIONS /Wall)
    endif()
endif()

# Warnings as errors, if required
if(MACESW_WERROR)
    if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|IntelLLVM)$")
        list(APPEND MACESW_COMPILE_OPTIONS -Werror)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND MACESW_COMPILE_OPTIONS /WX)
    endif()
endif()

# =============================================================================
# Add compile options and definitions here
# =============================================================================

add_compile_options(${MACESW_COMPILE_OPTIONS})
add_compile_definitions(${MACESW_COMPILE_DEFINITIONS})
