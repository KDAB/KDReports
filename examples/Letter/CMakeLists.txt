set(SOURCES
    Letter.cpp
)

add_executable(Letter ${SOURCES})
target_link_libraries(Letter ${QT_LIBRARIES} kdreports)
