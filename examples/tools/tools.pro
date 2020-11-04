##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

TEMPLATE = lib

include(../../variables.pri)

TARGET = $${KDREPORTSTESTTOOLSLIB}

# TESTTOOLS_STATICLIB needs to be defined for tools_export.h
staticlib {
  DEFINES += TESTTOOLS_STATICLIB
}

# For QAIM::reset()
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

DEFINES += KDREPORT_BUILD_TESTTOOLS_LIB
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += TableModel.h
SOURCES += TableModel.cpp
