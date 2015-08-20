set(SOURCES
    Database.cpp
)

add_executable(Database ${SOURCES})
target_link_libraries(Database ${QT_LIBRARIES} kdreports)
