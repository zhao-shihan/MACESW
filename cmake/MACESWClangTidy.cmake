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

set(MACESW_CLANG_TIDY_FIX_OPTION_1 "- -readability-redundant-declaration")
configure_file(${MACESW_PROJECT_TOOL_DIR}/clang-tidy.in
               ${MACESW_PROJECT_TOOL_DIR}/clang-tidy-fix-unity-build.yml
               @ONLY)
set(MACESW_CLANG_TIDY_FIX_OPTION_1 "")
configure_file(${MACESW_PROJECT_TOOL_DIR}/clang-tidy.in
               ${MACESW_PROJECT_ROOT_DIR}/.clang-tidy
               @ONLY)

if(MACESW_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy)
    if(NOT CLANG_TIDY_EXE)
        set(MACESW_CLANG_TIDY OFF)
        message(WARNING "clang-tidy not found. Temporarily turning off MACESW_CLANG_TIDY")
    elseif(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
        set(MACESW_CLANG_TIDY OFF)
        message(WARNING "Not using GCC or LLVM Clang. Temporarily turning off MACESW_CLANG_TIDY")
    else()
        execute_process(COMMAND ${CLANG_TIDY_EXE} --version
                        OUTPUT_VARIABLE clang_tidy_version_output
                        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(TOLOWER "${clang_tidy_version_output}" clang_tidy_version_output)
        if(clang_tidy_version_output MATCHES "amd" OR clang_tidy_version_output MATCHES "aocc")
            set(MACESW_CLANG_TIDY OFF)
            message(WARNING "AMD AOCC clang-tidy is not supported. Temporarily turning off MACESW_CLANG_TIDY")
        endif()
    endif()
endif()

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
        message(STATUS "MACESW source code will be analyzed by ${CLANG_TIDY_EXE} (warnings as errors)")
    else()
        message(STATUS "MACESW source code will be analyzed by ${CLANG_TIDY_EXE}")
    endif()
else()
    message(NOTICE "***Notice: MACESW source code will not be analyzed by clang-tidy")
endif()
