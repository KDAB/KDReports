##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

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
add_subdirectory(Headers)
add_subdirectory(XmlParser)
add_subdirectory(TableBreakingLogic)
add_subdirectory(SpreadsheetMode)
add_subdirectory(InThread)

if(NOT EMSCRIPTEN)
  add_subdirectory(PageLayout)
  add_subdirectory(TextDocument)
endif()
