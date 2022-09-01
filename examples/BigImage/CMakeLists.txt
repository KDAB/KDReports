#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES BigImage.cpp BigImage.qrc)

add_executable(
    BigImage
    ${SOURCES}
)
target_link_libraries(
    BigImage ${QT_LIBRARIES} kdreporttesttools kdreports
)
