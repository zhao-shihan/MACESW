add_library(AppMACEPhaseI STATIC MACE/PhaseI/PhaseI.c++)
target_include_directories(AppMACEPhaseI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AppMACEPhaseI PUBLIC AppMACESimulation
                                           AppMACEReconstruction
                                           AppMACEUtility
                                           Mustard::Mustard)
