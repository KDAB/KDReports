
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x000000)

set(SOURCES
    TableModel.cpp
)

add_library(kdreporttesttools ${KDReports_LIBRARY_MODE} ${SOURCES})
if(${PROJECT_NAME}_STATIC)
  target_compile_definitions(kdreporttesttools PUBLIC TESTTOOLS_STATICLIB)
else()
  target_compile_definitions(kdreporttesttools PRIVATE -DKDREPORT_BUILD_TESTTOOLS_LIB)
endif()

target_link_libraries(kdreporttesttools ${QT_LIBRARIES})
set_target_properties(kdreporttesttools PROPERTIES DEFINE_SYMBOL KDREPORT_BUILD_TESTTOOLS_LIB)
set_target_properties(kdreporttesttools PROPERTIES
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  VERSION ${${PROJECT_NAME}_VERSION}
)
