cmake_minimum_required(VERSION 2.6)
project(spiedo_pc)
find_package(Qt4 REQUIRED)

enable_testing()

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(Spiedo_Pc_SRCS Spiedo_Pc.cpp main.cpp)

qt4_automoc(${Spiedo_Pc_SRCS})
add_executable(spiedo_pc ${Spiedo_Pc_SRCS})
target_link_libraries(spiedo_pc ${QT_QTCORE_LIBRARY})

add_subdirectory(tests)

install(TARGETS spiedo_pc RUNTIME DESTINATION bin)
