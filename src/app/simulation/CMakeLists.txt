file(GLOB_RECURSE AppMACESimulation_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)
add_library(AppMACESimulation STATIC ${AppMACESimulation_SOURCES})
target_include_directories(AppMACESimulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AppMACESimulation PUBLIC MACEData
                                               MACEDetector
                                               MACEReconstruction
                                               MACESimulation
                                               Mustard::Mustard) 

add_subdirectory(MACE/PhaseI/SimMACEPhaseI)
add_subdirectory(MACE/SimDose)
add_subdirectory(MACE/SimECAL)
add_subdirectory(MACE/SimMACE)
add_subdirectory(MACE/SimMMS)
add_subdirectory(MACE/SimPTS)
add_subdirectory(MACE/SimTarget)
add_subdirectory(MACE/SimTTC)
