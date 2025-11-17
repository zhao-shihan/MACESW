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

message(STATUS "Looking for pmp")

set(MACESW_PMP_MINIMUM_REQUIRED 3.0.0)

if(NOT MACESW_BUILTIN_PMP)
    find_package(pmp ${MACESW_PMP_MINIMUM_REQUIRED})
    if(NOT pmp_FOUND)
        set(MACESW_BUILTIN_PMP ON)
        message(NOTICE "***Notice: pmp not found (minimum required is ${MACESW_PMP_MINIMUM_REQUIRED}). For the time turning on MACESW_BUILTIN_PMP")
    endif()
endif()

if(MACESW_BUILTIN_PMP)
    message(STATUS "MACESW will use built-in pmp")
    # check built-in version
    if(MACESW_BUILTIN_PMP_VERSION VERSION_LESS MACESW_PMP_MINIMUM_REQUIRED)
        message(NOTICE "***Notice: Provided MACESW_BUILTIN_PMP_VERSION is ${MACESW_BUILTIN_PMP_VERSION}, which is less than the requirement (${MACESW_PMP_MINIMUM_REQUIRED}). Changing to ${MACESW_PMP_MINIMUM_REQUIRED}")
        set(MACESW_BUILTIN_PMP_VERSION ${MACESW_PMP_MINIMUM_REQUIRED})
    endif()
    # set download dest and URL
    set(MACESW_BUILTIN_PMP_SRC_DIR "${MACESW_PROJECT_3RDPARTY_DIR}/pmp-library-${MACESW_BUILTIN_PMP_VERSION}")
    set(MACESW_BUILTIN_PMP_URL "https://github.com/pmp-library/pmp-library/archive/refs/tags/${MACESW_BUILTIN_PMP_VERSION}.tar.gz")
    # reuse or download
    include(FetchContent)
    if(EXISTS "${MACESW_BUILTIN_PMP_SRC_DIR}/CMakeLists.txt")
        FetchContent_Declare(pmp SOURCE_DIR "${MACESW_BUILTIN_PMP_SRC_DIR}")
        message(STATUS "Reusing pmp source ${MACESW_BUILTIN_PMP_SRC_DIR}")
    else()
        FetchContent_Declare(pmp SOURCE_DIR "${MACESW_BUILTIN_PMP_SRC_DIR}"
                                 URL "${MACESW_BUILTIN_PMP_URL}")
        message(STATUS "pmp will be downloaded from ${MACESW_BUILTIN_PMP_URL} to ${MACESW_BUILTIN_PMP_SRC_DIR}")
    endif()
    # set options
    set(PMP_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(PMP_BUILD_TESTS OFF CACHE INTERNAL "")
    set(PMP_BUILD_DOCS OFF CACHE INTERNAL "")
    set(PMP_BUILD_VIS OFF CACHE INTERNAL "")
    set(PMP_INSTALL ON CACHE INTERNAL "")
    set(PMP_STRICT_COMPILATION OFF CACHE INTERNAL "")
    set(PMP_SCALAR_TYPE 64 CACHE INTERNAL "")
    set(PMP_INDEX_TYPE 64 CACHE INTERNAL "")
    # configure it
    message(STATUS "Downloading (if required) and configuring pmp (version: ${MACESW_BUILTIN_PMP_VERSION})")
    FetchContent_MakeAvailable(pmp)
    message(STATUS "Downloading (if required) and configuring pmp (version: ${MACESW_BUILTIN_PMP_VERSION}) - done")
    # check download
    if(NOT EXISTS "${MACESW_BUILTIN_PMP_SRC_DIR}/CMakeLists.txt")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/pmp-build")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/pmp-subbuild")
        message(FATAL_ERROR "It seems that the download of pmp has failed. You can try running cmake again, or manually download pmp from ${MACESW_BUILTIN_PMP_URL} and extract it to ${MACESW_PROJECT_3RDPARTY_DIR} (and keep the directory structure). If the error persists, you can try cleaning the build tree and restarting the build.")
    endif()
endif()

if(NOT MACESW_BUILTIN_PMP)
    message(STATUS "Looking for pmp - found (version: ${pmp_VERSION})")
    set(PMP_INCLUDE_DIRS ${pmp_DIR}/include)
else()
    message(STATUS "Looking for pmp - built-in (version: ${MACESW_BUILTIN_PMP_VERSION})")
    set(PMP_INCLUDE_DIRS ${MACESW_BUILTIN_PMP_SRC_DIR}/src)
endif()
