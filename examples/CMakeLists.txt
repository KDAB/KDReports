# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_subdirectory(tools)
add_subdirectory(Letter)
add_subdirectory(Labels)
add_subdirectory(HelloWorld)
add_subdirectory(ReferenceReport)
add_subdirectory(PriceList)
add_subdirectory(HelloWorldXML)
add_subdirectory(PriceListXML)
add_subdirectory(SpreadsheetAutoTable)
add_subdirectory(LongTextReport)
add_subdirectory(BigImage)
add_subdirectory(MailMergeXML)
add_subdirectory(ReportSection)

if(NOT EMSCRIPTEN)
    if(TARGET Qt${Qt_VERSION_MAJOR}::Sql)
        list(APPEND QT_LIBRARIES Qt${Qt_VERSION_MAJOR}::Sql)
        add_subdirectory(Database)
        add_subdirectory(DatabaseXML)
    else()
        message(STATUS "QtSql not found.  skipping the Sql examples")
    endif()
else()
    add_subdirectory(DemoWasm)
endif()

if(KDChart_FOUND)
    add_subdirectory(ChartXML)
else()
    message(STATUS "KDChart not found.  skipping the ChartXML example")
endif()
