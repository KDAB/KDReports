# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

set(SOURCES MailMergeXML.cpp MailMergeXML.qrc)

add_executable(
    MailMergeXML
    ${SOURCES}
)
target_link_libraries(
    MailMergeXML ${QT_LIBRARIES} kdreports kdreporttesttools
)
