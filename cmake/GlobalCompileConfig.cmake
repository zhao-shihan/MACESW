# This file is included before "find_package"s and "LookFor"s.

# =============================================================================
# MACESW build type
# =============================================================================

if(DEFINED CMAKE_CONFIGURATION_TYPES)
    # multi-config generator
    message(STATUS "Using multi-config generator \"${CMAKE_GENERATOR}\"")
    message(STATUS "It provides the following build types: ${CMAKE_CONFIGURATION_TYPES}")
    if(DEFINED CMAKE_BUILD_TYPE)
        message(NOTICE "***Notice: CMAKE_BUILD_TYPE is defined while using a multi-config generator \"${CMAKE_GENERATOR}\".")
        message(NOTICE "           CMAKE_BUILD_TYPE is ignored by the multi-config generator and the build type should be specified at build time")
    endif()
else()
    # single-config generator
    message(STATUS "Using single-config generator \"${CMAKE_GENERATOR}\"")
    if(CMAKE_BUILD_TYPE STREQUAL "")
        # Default to "Release"
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASED_CMAKE_BUILD_TYPE)
    message(STATUS "MACESW build type: ${CMAKE_BUILD_TYPE}")
endif()

# =============================================================================
# Select MSVC runtime library
# =============================================================================

# Respect to MACESW_USE_SHARED_MSVC_RT
if(MACESW_USE_SHARED_MSVC_RT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
endif()

# =============================================================================
# AddressSanitizer
# =============================================================================

if(MACESW_ENABLE_ASAN_IN_DEBUG_BUILD)
    if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|IntelLLVM)$")
        add_compile_options($<$<CONFIG:Debug>:-fsanitize=undefined>
                            $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
                            $<$<CONFIG:RelWithDebInfo>:-fsanitize=undefined>
                            $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>)
        add_link_options($<$<CONFIG:Debug>:-fsanitize=undefined>
                         $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
                         $<$<CONFIG:RelWithDebInfo>:-fsanitize=undefined>
                         $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>)
        if(DEFINED CMAKE_CONFIGURATION_TYPES)
            # multi-config generator
            message(STATUS "AddressSanitizer will be enabled in debug build (-fsanitize=address)")
        else()
            # single-config generator
            if(LOWERCASED_CMAKE_BUILD_TYPE STREQUAL "debug")
                message(STATUS "AddressSanitizer enabled (-fsanitize=address)")
            endif()
        endif()
    endif()
endif()

# =============================================================================
# UndefinedBehaviorSanitizer
# =============================================================================

if(MACESW_ENABLE_UBSAN_IN_DEBUG_BUILD)
    if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|IntelLLVM)$")
        add_compile_options($<$<CONFIG:Debug>:-fsanitize=undefined> $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
                            $<$<CONFIG:RelWithDebInfo>:-fsanitize=undefined>)
        add_link_options($<$<CONFIG:Debug>:-fsanitize=undefined> $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
                         $<$<CONFIG:RelWithDebInfo>:-fsanitize=undefined>)
        if(DEFINED CMAKE_CONFIGURATION_TYPES)
            # multi-config generator
            message(STATUS "UndefinedBehaviorSanitizer will be enabled in debug build (-fsanitize=undefined)")
        else()
            # single-config generator
            if(LOWERCASED_CMAKE_BUILD_TYPE STREQUAL "debug")
                message(STATUS "UndefinedBehaviorSanitizer enabled (-fsanitize=undefined)")
            endif()
        endif()
    endif()
endif()

# =============================================================================
# Add debug information
# =============================================================================

if(MACESW_WITH_DEBUG_INFO)
    add_compile_options($<$<C_COMPILER_ID:GNU,Clang>:-g>   
                        $<$<CXX_COMPILER_ID:GNU,Clang>:-g> 
                        $<$<C_COMPILER_ID:MSVC>:/Zi> 
                        $<$<CXX_COMPILER_ID:MSVC>:/Zi>)
endif()
