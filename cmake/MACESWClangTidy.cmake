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

find_program(CLANG_TIDY_EXE clang-tidy)
if(NOT CLANG_TIDY_EXE)
    set(MACESW_CLANG_TIDY OFF)
    message(NOTICE "***Notice: clang-tidy not found. For the time turning off MACESW_CLANG_TIDY")
endif()
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
    set(MACESW_CLANG_TIDY OFF)
    message(NOTICE "***Notice: Not using GCC or LLVM Clang. For the time turning off MACESW_CLANG_TIDY")
endif()

include(CMakePackageConfigHelpers)
set(MACESW_CLANG_TIDY_FIX_OPTION_1 "- -readability-redundant-declaration")
configure_package_config_file(${MACESW_PROJECT_TOOL_DIR}/clang-tidy.in
                              ${MACESW_PROJECT_TOOL_DIR}/clang-tidy-fix-unity-build.yml
                              INSTALL_DESTINATION ${MACESW_PROJECT_TOOL_DIR})
set(MACESW_CLANG_TIDY_FIX_OPTION_1 "")
configure_package_config_file(${MACESW_PROJECT_TOOL_DIR}/clang-tidy.in
                              ${MACESW_PROJECT_ROOT_DIR}/.clang-tidy
                              INSTALL_DESTINATION ${MACESW_PROJECT_ROOT_DIR})

if(MACESW_CLANG_TIDY)
    set(MACESW_CLANG_TIDY_FULL_COMMAND
            ${CLANG_TIDY_EXE}
                --header-filter=${MACESW_PROJECT_SOURCE_DIR}/.*)
    if(MACESW_UNITY_BUILD)
        list(APPEND MACESW_CLANG_TIDY_FULL_COMMAND --config-file=${MACESW_PROJECT_TOOL_DIR}/clang-tidy-fix-unity-build.yml)
    elseif(MACESW_CLANG_TIDY_AMEND_UNITY_BUILD)
        list(APPEND MACESW_CLANG_TIDY_FULL_COMMAND --config-file=${MACESW_PROJECT_TOOL_DIR}/clang-tidy-amend-unity-build.yml)
    else()
        list(APPEND MACESW_CLANG_TIDY_FULL_COMMAND --config-file=${MACESW_PROJECT_ROOT_DIR}/.clang-tidy)
    endif()
    if(MACESW_CLANG_FORMAT_WERROR)
        list(APPEND MACESW_CLANG_TIDY_FULL_COMMAND --warnings-as-errors=*)
    endif()
    set(CMAKE_CXX_CLANG_TIDY ${MACESW_CLANG_TIDY_FULL_COMMAND})
    if(MACESW_CLANG_TIDY_WERROR)
        message(STATUS "MACESW source code will be analysed by ${CLANG_TIDY_EXE} (warnings as errors)")
    else()
        message(STATUS "MACESW source code will be analysed by ${CLANG_TIDY_EXE}")
    endif()
else()
    message(NOTICE "***Notice: MACESW source code will not be analysed by clang-tidy")
endif()
