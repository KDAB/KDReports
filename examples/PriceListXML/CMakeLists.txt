#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES
    PriceListXML.cpp ../PriceList/PriceList.qrc PriceListXML.qrc
)

add_executable(PriceListXML ${SOURCES})
target_link_libraries(PriceListXML ${QT_LIBRARIES} kdreporttesttools kdreports)
