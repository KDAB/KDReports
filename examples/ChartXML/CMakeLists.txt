set(SOURCES
    ChartXML.cpp
)

qt4_add_resources(SOURCES ChartXML.qrc)
include_directories(${KDChart_INCLUDE_DIR})
add_executable(ChartXML ${SOURCES})
target_link_libraries(ChartXML ${QT_LIBRARIES} ${KDChart_LIBRARIES} kdreports kdreporttesttools)
