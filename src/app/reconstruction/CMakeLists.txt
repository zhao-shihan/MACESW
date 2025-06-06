find_package(ROOT COMPONENTS Minuit2)

file(GLOB_RECURSE AppMACEReconstruction_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)
add_library(AppMACEReconstruction STATIC ${AppMACEReconstruction_SOURCES})
target_include_directories(AppMACEReconstruction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AppMACEReconstruction PUBLIC ROOT::Minuit2
                                                   MACEData
                                                   MACEDetector
                                                   MACEReconstruction
                                                   Mustard::Mustard)

add_subdirectory(MACE/SmearMACE)
