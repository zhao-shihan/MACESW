message(STATUS "Looking for Mustard")

set(MACE_MUSTARD_MINIMUM_REQUIRED 0.25.1015)

if(NOT MACE_BUILTIN_MUSTARD)
    find_package(Mustard ${MACE_MUSTARD_MINIMUM_REQUIRED})
    if(NOT Mustard_FOUND)
        set(MACE_BUILTIN_MUSTARD ON)
        message(NOTICE "***Notice: Mustard not found (minimum required is ${MACE_MUSTARD_MINIMUM_REQUIRED}). For the time turning on MACE_BUILTIN_MUSTARD")
    endif()
endif()

if(MACE_BUILTIN_MUSTARD)
    message(STATUS "MACE will use built-in Mustard")
    # check built-in version
    if(MACE_BUILTIN_MUSTARD_VERSION VERSION_LESS MACE_MUSTARD_MINIMUM_REQUIRED)
        message(NOTICE "***Notice: Provided MACE_BUILTIN_MUSTARD_VERSION is ${MACE_BUILTIN_MUSTARD_VERSION}, which is less than the requirement (${MACE_MUSTARD_MINIMUM_REQUIRED}). Changing to ${MACE_MUSTARD_MINIMUM_REQUIRED}")
        set(MACE_BUILTIN_MUSTARD_VERSION ${MACE_MUSTARD_MINIMUM_REQUIRED})
    endif()
    # set download dest and URL
    set(MACE_BUILTIN_MUSTARD_SRC_DIR "${MACE_PROJECT_3RDPARTY_DIR}/Mustard-${MACE_BUILTIN_MUSTARD_VERSION}")
    set(MACE_BUILTIN_MUSTARD_URL "https://github.com/zhao-shihan/Mustard/archive/refs/tags/v${MACE_BUILTIN_MUSTARD_VERSION}.tar.gz")
    # reuse or download
    include(FetchContent)
    if(EXISTS "${MACE_BUILTIN_MUSTARD_SRC_DIR}/CMakeLists.txt")
        FetchContent_Declare(Mustard SOURCE_DIR "${MACE_BUILTIN_MUSTARD_SRC_DIR}")
        message(STATUS "Reusing Mustard source ${MACE_BUILTIN_MUSTARD_SRC_DIR}")
    else()
        FetchContent_Declare(Mustard SOURCE_DIR "${MACE_BUILTIN_MUSTARD_SRC_DIR}"
                                     URL "${MACE_BUILTIN_MUSTARD_URL}")
        message(STATUS "Mustard will be downloaded from ${MACE_BUILTIN_MUSTARD_URL} to ${MACE_BUILTIN_MUSTARD_SRC_DIR}")
    endif()
    # set options
    set(MUSTARD_ENABLE_ASAN_IN_DEBUG_BUILD ${MACE_ENABLE_ASAN_IN_DEBUG_BUILD} CACHE INTERNAL "")
    set(MUSTARD_ENABLE_IPO ${MACE_ENABLE_IPO} CACHE INTERNAL "")
    set(MUSTARD_ENABLE_UBSAN_IN_DEBUG_BUILD ${MACE_ENABLE_UBSAN_IN_DEBUG_BUILD} CACHE INTERNAL "")
    set(MUSTARD_FULL_UNITY_BUILD ${MACE_FULL_UNITY_BUILD} CACHE INTERNAL "")
    set(MUSTARD_SHOW_EVEN_MORE_COMPILER_WARNINGS ${MACE_SHOW_EVEN_MORE_COMPILER_WARNINGS} CACHE INTERNAL "")
    set(MUSTARD_SHOW_MORE_COMPILER_WARNINGS ${MACE_SHOW_MORE_COMPILER_WARNINGS} CACHE INTERNAL "")
    set(MUSTARD_USE_SHARED_MSVC_RT ${MACE_USE_SHARED_MSVC_RT} CACHE INTERNAL "")
    # configure it
    message(STATUS "Downloading (if required) and configuring Mustard (version: ${MACE_BUILTIN_MUSTARD_VERSION})")
    FetchContent_MakeAvailable(Mustard)
    message(STATUS "Downloading (if required) and configuring Mustard (version: ${MACE_BUILTIN_MUSTARD_VERSION}) - done")
    # check download
    if(NOT EXISTS "${MACE_BUILTIN_MUSTARD_SRC_DIR}/CMakeLists.txt")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/mustard-build")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/mustard-subbuild")
        message(FATAL_ERROR "It seems that the download of Mustard has failed. You can try running cmake again, or manually download Mustard from ${MACE_BUILTIN_MUSTARD_URL} and extract it to ${MACE_PROJECT_3RDPARTY_DIR} (and keep the directory structure). If the error persists, you can try cleaning the build tree and restarting the build.")
    endif()
endif()

if(NOT MACE_BUILTIN_MUSTARD)
    message(STATUS "Looking for Mustard - found (version: ${Mustard_VERSION})")
else()
    message(STATUS "Looking for Mustard - built-in (version: ${MACE_BUILTIN_MUSTARD_VERSION})")
endif()
