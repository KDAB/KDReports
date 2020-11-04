/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#ifndef KDREPORTSGLOBAL_H
#define KDREPORTSGLOBAL_H

#include <qglobal.h>

# ifdef KDREPORTS_STATICLIB
#  undef KDREPORTS_SHAREDLIB
#  define KDREPORTS_EXPORT
# else
#  ifdef KDREPORTS_BUILD_KDREPORTS_LIB
#   define KDREPORTS_EXPORT Q_DECL_EXPORT
#  else
#   define KDREPORTS_EXPORT Q_DECL_IMPORT
#  endif
# endif

#endif /* KDREPORTSGLOBAL_H */

