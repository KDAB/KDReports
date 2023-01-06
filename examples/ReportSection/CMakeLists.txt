#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES ReportSection.cpp)

add_executable(
    ReportSection
    ${SOURCES}
)
target_link_libraries(
    ReportSection ${QT_LIBRARIES} kdreports
)
