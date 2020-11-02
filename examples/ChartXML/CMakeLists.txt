set(SOURCES
    ChartXML.cpp
)

qt5_add_resources(SOURCES ChartXML.qrc)
add_executable(ChartXML ${SOURCES})
target_link_libraries(ChartXML ${QT_LIBRARIES} KDChart::kdchart kdreports kdreporttesttools)
