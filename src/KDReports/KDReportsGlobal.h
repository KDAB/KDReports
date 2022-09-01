/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSGLOBAL_H
#define KDREPORTSGLOBAL_H

#include <qglobal.h>

#ifdef KDREPORTS_STATICLIB
#undef KDREPORTS_SHAREDLIB
#define KDREPORTS_EXPORT
#else
#ifdef KDREPORTS_BUILD_KDREPORTS_LIB
#define KDREPORTS_EXPORT Q_DECL_EXPORT
#else
#define KDREPORTS_EXPORT Q_DECL_IMPORT
#endif
#endif

#endif /* KDREPORTSGLOBAL_H */
