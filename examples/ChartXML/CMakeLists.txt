set(SOURCES
    ChartXML.cpp
)

qt4_add_resources(SOURCES ChartXML.qrc)
add_executable(ChartXML ${SOURCES})
target_link_libraries(ChartXML ${QT_LIBRARIES} KDChart kdreports kdreporttesttools)
