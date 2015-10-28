include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(Spiedo_PcTest_SRCS Spiedo_PcTest.cpp)

qt4_automoc(${Spiedo_PcTest_SRCS})
add_executable(Spiedo_PcTest ${Spiedo_PcTest_SRCS})
add_test(Spiedo_PcTest Spiedo_PcTest)
target_link_libraries(Spiedo_PcTest ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
