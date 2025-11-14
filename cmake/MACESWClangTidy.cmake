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
