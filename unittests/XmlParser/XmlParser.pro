##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

KDREPORTS_PATH = $$PWD/../..

include( $$KDREPORTS_PATH/unittests/unittests.pri )

QT += xml

DESTDIR = .
HEADERS = 
SOURCES = XmlParser.cpp TestXmlElementHandlerV1.cpp TestXmlElementHandler.cpp

RESOURCES += XmlParser.qrc

test.target = test
test.commands = ./$(TARGET)
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test

OTHER_FILES = \
  broken.xml \
  doc_footer.xml \
  doc_header.xml \
  fonts.xml \
  handlerWithError.xml \
  handler.xml \
  htmlid.xml \
  margins.xml \
  simple.xml \
  spreadsheet.xml \
  tabs.xml \
  textid.xml \
  vspaceInHeader.xml \
  wrongTopElement.xml \

