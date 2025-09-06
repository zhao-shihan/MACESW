file(GLOB_RECURSE MACE_UTILITY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)

add_library(MACEUtility STATIC ${MACE_UTILITY_SRC})
target_include_directories(MACEUtility PUBLIC ${PROJECT_SOURCE_DIR}/lib/utility)
target_link_libraries(MACEUtility PUBLIC Mustard::Mustard)
