# cmake_minimum_required(VERSION 2.6)
# project(spiedo_pc)
# find_package(Qt4 REQUIRED)
# 
# enable_testing()
# 
# include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
# 
# set(Spiedo_Pc_SRCS Spiedo_Pc.cpp main.cpp)
# 
# qt4_automoc(${Spiedo_Pc_SRCS})
# add_executable(spiedo_pc ${Spiedo_Pc_SRCS})
# target_link_libraries(spiedo_pc ${QT_QTCORE_LIBRARY})
# 
# add_subdirectory(tests)
# 
# install(TARGETS spiedo_pc RUNTIME DESTINATION bin)

cmake_minimum_required(VERSION 2.8.11 )

project(spiedo_pc_gui)
# project(spiedo_pc)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)


find_package(Qt5PrintSupport REQUIRED)

# add_executable(spiedo_pc main.cpp )

add_executable(spiedo_pc_gui mainGUi.cpp
                selectionWindows.cpp moc_selectionWindows.cpp 
                spiedino.cpp moc_spiedino.cpp 
                counterino.cpp moc_counterino.cpp 
                selectionCounterWindows.cpp moc_selectionCounterWindows.cpp
                mainWindow.cpp moc_mainWindow.cpp 
                qcustomplot.cpp moc_qcustomplot.cpp
                controlwindows.cpp
                moc_controlwindows.cpp
                )

# target_link_libraries(spiedo_pc Qt5::Core Qt5::SerialPort Qt5::Widgets Qt5::Gui Qt5::PrintSupport )
target_link_libraries(spiedo_pc_gui Qt5::Core Qt5::SerialPort Qt5::Widgets Qt5::Gui Qt5::PrintSupport )
# 