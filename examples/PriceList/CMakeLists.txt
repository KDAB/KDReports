set(SOURCES
    PriceList.cpp
)

qt4_add_resources(SOURCES PriceList.qrc)

add_executable(PriceList ${SOURCES})
target_link_libraries(PriceList ${QT_LIBRARIES} kdreporttesttools kdreports) 
