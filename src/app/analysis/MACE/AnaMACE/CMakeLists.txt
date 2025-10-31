file(GLOB_RECURSE MACESW_AnaMACE_srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.c++)

add_executable(AnaMACE ${MACESW_AnaMACE_srcs})
target_link_libraries(AnaMACE MACEData MACEDetector Mustard::Mustard MACEAnalysis)

install(TARGETS AnaMACE DESTINATION ${CMAKE_INSTALL_BINDIR})
