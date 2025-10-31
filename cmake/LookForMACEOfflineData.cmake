message(STATUS "Looking for macesw_offline_data")

set(MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED 0.25.103013)

if(NOT MACESW_BUILTIN_MACESW_OFFLINE_DATA)
    find_package(macesw_offline_data ${MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED})
    if(NOT macesw_offline_data_FOUND)
        set(MACESW_BUILTIN_MACESW_OFFLINE_DATA ON)
        message(NOTICE "***Notice: macesw_offline_data not found (minimum required is ${MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED}). For the time turning on MACESW_BUILTIN_MACESW_OFFLINE_DATA")
    endif()
endif()

if(MACESW_BUILTIN_MACESW_OFFLINE_DATA)
    message(STATUS "MACESW will use built-in macesw_offline_data")
    # check built-in version
    if(MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION VERSION_LESS MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED)
        message(NOTICE "***Notice: Provided MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION is ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION}, which is less than the requirement (${MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED}). Changing to ${MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED}")
        set(MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION ${MACESW_MACESW_OFFLINE_DATA_MINIMUM_REQUIRED})
    endif()
    # set download dest and URL
    set(MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR "${MACESW_PROJECT_3RDPARTY_DIR}/macesw_offline_data-v${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION}")
    set(MACESW_BUILTIN_MACESW_OFFLINE_DATA_URL "https://code.ihep.ac.cn/zhaoshh7/macesw_offline_data/-/archive/v${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION}/macesw_offline_data-v${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION}.tar.gz")
    # reuse or download
    include(FetchContent)
    if(EXISTS "${MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR}/CMakeLists.txt")
        FetchContent_Declare(macesw_offline_data SOURCE_DIR "${MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR}")
        message(STATUS "Reusing macesw_offline_data source ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR}")
    else()
        FetchContent_Declare(macesw_offline_data SOURCE_DIR "${MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR}"
                                                 URL "${MACESW_BUILTIN_MACESW_OFFLINE_DATA_URL}")
        message(STATUS "macesw_offline_data will be downloaded from ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_URL} to ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR}")
    endif()
    # configure it
    message(STATUS "Downloading (if required) and configuring macesw_offline_data (version: ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION})")
    FetchContent_MakeAvailable(macesw_offline_data)
    message(STATUS "Downloading (if required) and configuring macesw_offline_data (version: ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION}) - done")
    # check download
    if(NOT EXISTS "${MACESW_BUILTIN_MACESW_OFFLINE_DATA_SRC_DIR}/CMakeLists.txt")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/macesw_offline_data-build")
        file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/_deps/macesw_offline_data-subbuild")
        message(FATAL_ERROR "It seems that the download of macesw_offline_data has failed. You can try running cmake again, or manually download macesw_offline_data from ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_URL} and extract it to ${MACESW_PROJECT_3RDPARTY_DIR} (and keep the directory structure). If the error persists, you can try cleaning the build tree and restarting the build.")
    endif()
endif()

if(NOT MACESW_BUILTIN_MACESW_OFFLINE_DATA)
    message(STATUS "Looking for macesw_offline_data - found (version: ${macesw_offline_data_VERSION})")
else()
    message(STATUS "Looking for macesw_offline_data - built-in (version: ${MACESW_BUILTIN_MACESW_OFFLINE_DATA_VERSION})")
endif()
