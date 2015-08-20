set(SOURCES
    LongReport.cpp
)

add_executable(LongReport ${SOURCES})
target_link_libraries(LongReport ${QT_LIBRARIES} kdreports) 
