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

#    Note: KDREPORTS_PATH is set in the calling .pro file
include (../examples/examples.pri)

contains( $$list($$[QT_VERSION]), 5.* ) {
    QT += testlib
    CONFIG += testcase
    macx:CONFIG -= app_bundle
} else {
    CONFIG += qtestlib
}
