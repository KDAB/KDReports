##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

set(SOURCES
    LongTextReport.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/KDReports)
add_executable(LongTextReport ${SOURCES})
target_link_libraries(LongTextReport ${QT_LIBRARIES} kdreports)
