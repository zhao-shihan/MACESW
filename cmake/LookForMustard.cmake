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

message(STATUS "Looking for Mustard")

set(MACESW_MUSTARD_MINIMUM_REQUIRED 0.25.1103)

if(NOT MACESW_BUILTIN_MUSTARD)
    find_package(Mustard ${MACESW_MUSTARD_MINIMUM_REQUIRED})
    if(NOT Mustard_FOUND)
        set(MACESW_BUILTIN_MUSTARD ON)
        message(NOTICE "***Notice: Mustard not found (minimum required is ${MACESW_MUSTARD_MINIMUM_REQUIRED}). For the time turning on MACESW_BUILTIN_MUSTARD")
    endif()
endif()

if(MACESW_BUILTIN_MUSTARD)
    message(STATUS "MACESW will use built-in Mustard")
    # check built-in version
    if(MACESW_BUILTIN_MUSTARD_VERSION VERSION_LESS MACESW_MUSTARD_MINIMUM_REQUIRED)
        message(NOTICE "***Notice: Provided MACESW_BUILTIN_MUSTARD_VERSION is ${MACESW_BUILTIN_MUSTARD_VERSION}, which is less than the requirement (${MACESW_MUSTARD_MINIMUM_REQUIRED}). Changing to ${MACESW_MUSTARD_MINIMUM_REQUIRED}")
        set(MACESW_BUILTIN_MUSTARD_VERSION ${MACESW_MUSTARD_MINIMUM_REQUIRED})
    endif()
    # set download dest and URL
    set(MACESW_BUILTIN_MUSTARD_SRC_DIR "${MACESW_PROJECT_3RDPARTY_DIR}/Mustard-${MACESW_BUILTIN_MUSTARD_VERSION}")
    set(MACESW_BUILTIN_MUSTARD_URL "https://github.com/zhao-shihan/Mustard/archive/refs/tags/v${MACESW_BUILTIN_MUSTARD_VERSION}.tar.gz")
    # reuse or download
    include(FetchContent)
    if(EXISTS "${MACESW_BUILTIN_MUSTARD_SRC_DIR}/CMakeLists.txt")
        FetchContent_Declare(Mustard SOURCE_DIR "${MACESW_BUILTIN_MUSTARD_SRC_DIR}")
        message(STATUS "Reusing Mustard source ${MACESW_BUILTIN_MUSTARD_SRC_DIR}")
    else()
        FetchContent_Declare(Mustard SOURCE_DIR "${MACESW_BUILTIN_MUSTARD_SRC_DIR}"
                                     URL "${MACESW_BUILTIN_MUSTARD_URL}")
        message(STATUS "Mustard will be downloaded from ${MACESW_BUILTIN_MUSTARD_URL} to ${MACESW_BUILTIN_MUSTARD_SRC_DIR}")
    endif()
    # set options
    set(MUSTARD_ENABLE_ASAN_IN_DEBUG_BUILD ${MACESW_ENABLE_ASAN_IN_DEBUG_BUILD} CACHE INTERNAL "")
    set(MUSTARD_ENABLE_IPO ${MACESW_ENABLE_IPO} CACHE INTERNAL "")
    set(MUSTARD_ENABLE_UBSAN_IN_DEBUG_BUILD ${MACESW_ENABLE_UBSAN_IN_DEBUG_BUILD} CACHE INTERNAL "")
    set(MUSTARD_FULL_UNITY_BUILD ${MACESW_FULL_UNITY_BUILD} CACHE INTERNAL "")
    set(MUSTARD_SHOW_EVEN_MORE_COMPILER_WARNINGS ${MACESW_SHOW_EVEN_MORE_COMPILER_WARNINGS} CACHE INTERNAL "")
    set(MUSTARD_SHOW_MORE_COMPILER_WARNINGS ${MACESW_SHOW_MORE_COMPILER_WARNINGS} CACHE INTERNAL "")
    set(MUSTARD_UNITY_BUILD ${MACESW_UNITY_BUILD} CACHE INTERNAL "")
    set(MUSTARD_USE_SHARED_MSVC_RT ${MACESW_USE_SHARED_MSVC_RT} CACHE INTERNAL "")
    # configure it
    message(STATUS "Downloading (if required) and configuring Mustard (version: ${MACESW_BUILTIN_MUSTARD_VERSION})")
    FetchContent_MakeAvailable(Mustard)
    message(STATUS "Downloading (if required) and configuring Mustard (version: ${MACESW_BUILTIN_MUSTARD_VERSION}) - done")
    # check download
    if(NOT EXISTS "${MACESW_BUILTIN_MUSTARD_SRC_DIR}/CMakeLists.txt")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/mustard-build")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/mustard-subbuild")
        message(FATAL_ERROR "It seems that the download of Mustard has failed. You can try running cmake again, or manually download Mustard from ${MACESW_BUILTIN_MUSTARD_URL} and extract it to ${MACESW_PROJECT_3RDPARTY_DIR} (and keep the directory structure). If the error persists, you can try cleaning the build tree and restarting the build.")
    endif()
endif()

if(NOT MACESW_BUILTIN_MUSTARD)
    message(STATUS "Looking for Mustard - found (version: ${Mustard_VERSION})")
else()
    message(STATUS "Looking for Mustard - built-in (version: ${MACESW_BUILTIN_MUSTARD_VERSION})")
endif()
