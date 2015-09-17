set(SOURCES
    Labels.cpp
)

add_executable(Labels ${SOURCES})
target_link_libraries(Labels ${QT_LIBRARIES} kdreports)
