file(GLOB_RECURSE MACESW_SIMULATION_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)

add_library(MACESimulation STATIC ${MACESW_SIMULATION_SRC})
target_include_directories(MACESimulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MACESimulation PUBLIC Mustard::Mustard MACEData MACEDetector MACEReconstruction)
