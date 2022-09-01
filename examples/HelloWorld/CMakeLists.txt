#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES
    HelloWorld.cpp
)

add_executable(HelloWorld ${SOURCES})
target_link_libraries(HelloWorld ${QT_LIBRARIES} kdreports)
