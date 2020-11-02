set(SOURCES
    HelloWorldXML.cpp
)

qt5_add_resources(SOURCES HelloWorldXML.qrc)

add_executable(HelloWorldXML ${SOURCES})
target_link_libraries(HelloWorldXML ${QT_LIBRARIES} kdreports)
