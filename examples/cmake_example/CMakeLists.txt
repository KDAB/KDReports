cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
include_directories(${QT_INCLUDES})

# Use FindKDReports.cmake from two directories up
# Usually you would copy FindKDReports.cmake to your own project
# and adjust the path below.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
find_package(KDReports REQUIRED)
include_directories(${KDReports_INCLUDE_DIR})

set(sources HelloWorld.cpp)
#automoc4(HelloWorld sources)
add_executable(HelloWorld ${sources})
target_link_libraries(HelloWorld ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${KDReports_LIBRARIES})

