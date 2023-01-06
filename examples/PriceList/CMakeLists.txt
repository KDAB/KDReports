#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES PriceList.cpp PriceList.qrc)

add_executable(
    PriceList
    ${SOURCES}
)
target_link_libraries(
    PriceList ${QT_LIBRARIES} kdreporttesttools kdreports
)
