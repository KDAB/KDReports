set(SOURCES
    LongTextReport.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/KDReports)
add_executable(LongTextReport ${SOURCES})
target_link_libraries(LongTextReport ${QT_LIBRARIES} kdreports) 
