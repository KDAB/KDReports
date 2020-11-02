set(SOURCES
    BigImage.cpp
)

qt5_add_resources(SOURCES BigImage.qrc)

add_executable(BigImage ${SOURCES})
target_link_libraries(BigImage ${QT_LIBRARIES} kdreporttesttools kdreports)
