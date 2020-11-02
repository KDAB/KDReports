set(SOURCES
    DatabaseXML.cpp
)

qt5_add_resources(SOURCES DatabaseXML.qrc)

add_executable(DatabaseXML ${SOURCES})
target_link_libraries(DatabaseXML ${QT_LIBRARIES} kdreports)
