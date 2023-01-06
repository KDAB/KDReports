#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES SpreadsheetAutoTable.cpp)

add_executable(
    SpreadsheetAutoTable
    ${SOURCES}
)
target_link_libraries(
    SpreadsheetAutoTable ${QT_LIBRARIES} kdreports
)
