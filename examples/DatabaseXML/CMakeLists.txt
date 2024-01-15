# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES DatabaseXML.cpp DatabaseXML.qrc)

add_executable(
    DatabaseXML
    ${SOURCES}
)
target_link_libraries(
    DatabaseXML ${QT_LIBRARIES} kdreports
)
