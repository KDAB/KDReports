/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2011-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

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
