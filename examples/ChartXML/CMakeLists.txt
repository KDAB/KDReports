#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES ChartXML.cpp ChartXML.qrc)

add_executable(
    ChartXML
    ${SOURCES}
)
target_link_libraries(
    ChartXML
    ${QT_LIBRARIES}
    KDChart::kdchart
    kdreports
    kdreporttesttools
)
