/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORT_TESTTOOLS_EXPORT_H
#define KDREPORT_TESTTOOLS_EXPORT_H

#include <qglobal.h>

#ifdef TESTTOOLS_STATICLIB
#undef TESTTOOLS_SHAREDLIB
#define TESTTOOLS_EXPORT
#else
#ifdef KDREPORT_BUILD_TESTTOOLS_LIB
#define TESTTOOLS_EXPORT Q_DECL_EXPORT
#else
#define TESTTOOLS_EXPORT Q_DECL_IMPORT
#endif
#endif

#endif // KDREPORT_TESTTOOLS_EXPORT_H
