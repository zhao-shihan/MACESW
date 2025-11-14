file(GLOB_RECURSE AppMACEGenerator_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)
add_library(AppMACEGenerator STATIC ${AppMACEGenerator_SOURCES})
target_include_directories(AppMACEGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AppMACEGenerator PUBLIC MACEDetector
                                              MACEUtility
                                              Mustard::Mustard) 
