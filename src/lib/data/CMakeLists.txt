file(GLOB_RECURSE MACESW_DATA_MODEL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)

add_library(MACEData STATIC ${MACESW_DATA_MODEL_SRC})
target_include_directories(MACEData PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MACEData PUBLIC Mustard::Mustard)
