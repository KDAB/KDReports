set(SOURCES
    ReferenceReport.cpp
    ResultModel.cpp
)
qt4_add_resources(SOURCES
    ReferenceReport.qrc
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/KDReports)
add_executable(ReferenceReport ${SOURCES})
target_link_libraries(ReferenceReport ${QT_LIBRARIES} kdreports kdreporttesttools)
