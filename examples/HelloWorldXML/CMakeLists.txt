#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES HelloWorldXML.cpp HelloWorldXML.qrc)

add_executable(
    HelloWorldXML
    ${SOURCES}
)
target_link_libraries(
    HelloWorldXML ${QT_LIBRARIES} kdreports
)
