/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORT_TESTTOOLS_EXPORT_H
#define KDREPORT_TESTTOOLS_EXPORT_H

#include <qglobal.h>

# ifdef TESTTOOLS_STATICLIB
#  undef TESTTOOLS_SHAREDLIB
#  define TESTTOOLS_EXPORT
# else
#  ifdef KDREPORT_BUILD_TESTTOOLS_LIB
#   define TESTTOOLS_EXPORT Q_DECL_EXPORT
#  else
#   define TESTTOOLS_EXPORT Q_DECL_IMPORT
#  endif
# endif

#endif // KDREPORT_TESTTOOLS_EXPORT_H
