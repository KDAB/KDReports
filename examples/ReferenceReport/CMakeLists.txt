# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES ReferenceReport.cpp ResultModel.cpp ReferenceReport.qrc)

add_executable(
    ReferenceReport
    ${SOURCES}
)
target_link_libraries(
    ReferenceReport ${QT_LIBRARIES} kdreports kdreporttesttools
)
