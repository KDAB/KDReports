/****************************************************************************
** Copyright (C) 2011-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#ifndef XMLHELPER_H
#define XMLHELPER_H

#include "KDReportsGlobal.h"
#include "KDReportsVariableType.h"
#include "KDReportsReport.h"
#include <QDomElement>

namespace KDReports {

class KDREPORTS_EXPORT XmlHelper //krazy:exclude=dpointer
{
public:
    static Qt::AlignmentFlag stringToAlignment(const QString &str);
    static KDReports::VariableType stringToVariableType(const QString &type);
    static QColor readColor(const QDomElement &element, const char *attributeName);
    static QColor readBackground(const QDomElement &element);
    static KDReports::HeaderLocations parseHeaderLocation(const QString &xmlAttr);
};
}
#endif // XMLHELPER_H
