# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES LongTextReport.cpp)

add_executable(
    LongTextReport
    ${SOURCES}
)
target_link_libraries(
    LongTextReport ${QT_LIBRARIES} kdreports
)
