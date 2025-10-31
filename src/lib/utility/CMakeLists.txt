file(GLOB_RECURSE MACESW_UTILITY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)

add_library(MACEUtility STATIC ${MACESW_UTILITY_SRC})
target_include_directories(MACEUtility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MACEUtility PUBLIC Mustard::Mustard)
