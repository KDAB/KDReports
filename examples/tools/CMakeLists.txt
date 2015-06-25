
if(KDReports_STATIC)
  add_definitions(-DTESTTOOLS_STATICLIB)
endif()

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x000000)

set(SOURCES
    TableModel.cpp
)

add_library(kdreporttesttools ${KDReports_LIBRARY_MODE} ${SOURCES})
target_link_libraries(kdreporttesttools ${QT_LIBRARIES})
set_target_properties(kdreporttesttools PROPERTIES DEFINE_SYMBOL KDREPORT_BUILD_TESTTOOLS_LIB)
set_target_properties(kdreporttesttools PROPERTIES VERSION ${${PROJECT_NAME}_VERSION})
