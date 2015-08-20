set(SOURCES
    ReportSection.cpp
)

add_executable(ReportSection ${SOURCES})
target_link_libraries(ReportSection ${QT_LIBRARIES} kdreports)
