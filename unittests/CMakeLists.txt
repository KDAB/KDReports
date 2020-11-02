set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTTEST TRUE)

include_directories(../src/KDReports ${CMAKE_CURRENT_BINARY_DIR}/../src/KDReports)

remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

macro(add_unittest _source)
  get_filename_component(_name ${_source} NAME_WE)
  set(_srcs ${_source})
  set(ARGN_COPY ${ARGN})
  set(_resource)
  foreach(_entry ${ARGN_COPY})
    if(NOT _entry STREQUAL "RESOURCE")
      list(APPEND _srcs ${_entry})
    else()
      list(REMOVE_AT ARGN_COPY 0)   # remove RESOURCE entry
      set(_resource ${ARGN_COPY})
      break()
    endif()
    list(REMOVE_AT ARGN_COPY 0)
  endforeach()

  if(_resource)
    qt5_add_resources(_srcs ${_resource})
  endif()

  add_executable(${_name} ${_srcs})

  add_test(NAME kdreports-${_name} COMMAND ${_name})
  target_link_libraries(${_name} ${QT_LIBRARIES} Qt5::Test kdreports)
endmacro()

add_subdirectory(ElementCopying)
add_subdirectory(TextDocument)
add_subdirectory(Headers)
add_subdirectory(PageLayout)
add_subdirectory(XmlParser)
add_subdirectory(TableBreakingLogic)
add_subdirectory(SpreadsheetMode)
add_subdirectory(InThread)
