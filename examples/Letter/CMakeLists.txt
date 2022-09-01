#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES Letter.cpp)

add_executable(
    Letter
    ${SOURCES}
)
target_link_libraries(
    Letter ${QT_LIBRARIES} kdreports
)
