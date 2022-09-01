/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef XMLHELPER_H
#define XMLHELPER_H

#include "KDReportsGlobal.h"
#include "KDReportsReport.h"
#include "KDReportsVariableType.h"
#include <QDomElement>

namespace KDReports {

class KDREPORTS_EXPORT XmlHelper // krazy:exclude=dpointer
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
