file(GLOB_RECURSE AppMACEUtility_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)
add_library(AppMACEUtility STATIC ${AppMACEUtility_SOURCES})
target_include_directories(AppMACEUtility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AppMACEUtility PUBLIC MACEDetector
                                            MACESimulation
                                            MACEUtility
                                            Mustard::Mustard) 
