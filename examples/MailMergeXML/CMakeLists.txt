set(SOURCES
    MailMergeXML.cpp
)

qt4_add_resources(SOURCES MailMergeXML.qrc)

add_executable(MailMergeXML ${SOURCES})
target_link_libraries(MailMergeXML ${QT_LIBRARIES} kdreports kdreporttesttools)
