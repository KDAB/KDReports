set(SOURCES
    HelloWorld.cpp
)

add_executable(HelloWorld ${SOURCES})
target_link_libraries(HelloWorld ${QT_LIBRARIES} kdreports)
