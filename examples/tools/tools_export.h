/****************************************************************************
** Copyright (C) 2007-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Reports library.
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2.1 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

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
