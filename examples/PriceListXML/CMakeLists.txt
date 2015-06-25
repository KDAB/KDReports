set(SOURCES
    PriceListXML.cpp
)

qt4_add_resources(SOURCES ../PriceList/PriceList.qrc PriceListXML.qrc)

add_executable(PriceListXML ${SOURCES})
target_link_libraries(PriceListXML ${QT_LIBRARIES} kdreporttesttools kdreports) 
