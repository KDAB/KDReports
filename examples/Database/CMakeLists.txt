#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES Database.cpp)

add_executable(
    Database
    ${SOURCES}
)
target_link_libraries(
    Database ${QT_LIBRARIES} kdreports
)
