#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../src/KDReports ${CMAKE_CURRENT_BINARY_DIR}/../src/KDReports)

remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

# Adds a unittest for the specified source. the test name is derived from the source file name.
macro(add_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    set(_name "tst_${_name}") #string(PREPEND _name "tst_") requires CMake 3.10+
    set(_srcs ${_source})
    set(ARGN_COPY ${ARGN})
    set(_resource)
    foreach(_entry ${ARGN_COPY})
        if(NOT _entry STREQUAL "RESOURCE")
            list(APPEND _srcs ${_entry})
        else()
            list(REMOVE_AT ARGN_COPY 0) # remove RESOURCE entry
            set(_resource ${ARGN_COPY})
            break()
        endif()
        list(REMOVE_AT ARGN_COPY 0)
    endforeach()

    if(_resource)
        set(_srcs ${_srcs} ${_resource})
    endif()

    add_executable(${_name} ${_srcs})

    add_test(NAME kdreports-${_name} COMMAND ${_name})
    target_link_libraries(
        ${_name}
        Qt${Qt_VERSION_MAJOR}::Core
        Qt${Qt_VERSION_MAJOR}::Gui
        Qt${Qt_VERSION_MAJOR}::Sql
        Qt${Qt_VERSION_MAJOR}::Test
        kdreports
    )
endmacro()

add_subdirectory(ElementCopying)
add_subdirectory(Headers)
add_subdirectory(XmlParser)
add_subdirectory(TableBreakingLogic)
add_subdirectory(SpreadsheetMode)
add_subdirectory(InThread)

if(NOT EMSCRIPTEN)
    if(TARGET Qt${Qt_VERSION_MAJOR}::Sql)
        list(APPEND QT_LIBRARIES Qt${Qt_VERSION_MAJOR}::Sql)
        add_subdirectory(PageLayout)
        add_subdirectory(TextDocument)
    else()
        message(STATUS "QtSql not found. skipping the Sql-based unittests")
    endif()
endif()
