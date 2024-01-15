/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsXmlHelper.h"
#include <QDebug>

Qt::AlignmentFlag KDReports::XmlHelper::stringToAlignment(const QString &str)
{
    if (str == QLatin1String("left"))
        return Qt::AlignLeft;
    else if (str == QLatin1String("right"))
        return Qt::AlignRight;
    else if (str == QLatin1String("hcenter"))
        return Qt::AlignHCenter;
    else
        qWarning("Unexpected alignment flag in KDReports::Report::stringToAlignment(): %s", qPrintable(str));
    return Qt::AlignLeft;
}

KDReports::VariableType KDReports::XmlHelper::stringToVariableType(const QString &type)
{
    if (type == QLatin1String("pagenumber"))
        return KDReports::PageNumber;
    else if (type == QLatin1String("pagecount"))
        return KDReports::PageCount;
    else if (type == QLatin1String("textdate"))
        return KDReports::TextDate;
    else if (type == QLatin1String("isodate"))
        return KDReports::ISODate;
    else if (type == QLatin1String("localedate"))
        return KDReports::LocaleDate;
    else if (type == QLatin1String("texttime"))
        return KDReports::TextTime;
    else if (type == QLatin1String("isotime"))
        return KDReports::ISOTime;
    else if (type == QLatin1String("localetime"))
        return KDReports::LocaleTime;
    qWarning("Unexpected variable type: %s", qPrintable(type));
    return KDReports::PageNumber;
}

QColor KDReports::XmlHelper::readColor(const QDomElement &element, const char *attributeName)
{
    QColor ret;
    const QString name = element.attribute(QLatin1String(attributeName));
    if (!name.isEmpty()) {
        ret = QColor(name);
    }
    return ret;
}

QColor KDReports::XmlHelper::readBackground(const QDomElement &element)
{
    QColor ret;
    if (element.hasAttribute(QStringLiteral("background"))) {
        const QString name = element.attribute(QStringLiteral("background"));
        ret = QColor(name);
    } else if (element.hasAttribute(QStringLiteral("bgred")) && element.hasAttribute(QStringLiteral("bggreen")) && element.hasAttribute(QStringLiteral("bgblue"))) {
        int red = 0;
        int green = 0;
        int blue = 0;
        bool ok = true;
        red = element.attribute(QStringLiteral("bgred")).toInt(&ok);
        if (ok) {
            green = element.attribute(QStringLiteral("bggreen")).toInt(&ok);
            if (ok) {
                blue = element.attribute(QStringLiteral("bgblue")).toInt(&ok);
                if (ok) {
                    ret.setRed(red);
                    ret.setGreen(green);
                    ret.setBlue(blue);
                }
            }
        }
    }
    return ret;
}

KDReports::HeaderLocations KDReports::XmlHelper::parseHeaderLocation(const QString &xmlAttr)
{
    if (xmlAttr.isEmpty())
        return KDReports::AllPages;
    KDReports::HeaderLocations loc;
    const QStringList tokens = xmlAttr.toLower().split(QLatin1Char(','));
    for (const QString &s : tokens) {
        QString token = s.trimmed();
        if (token == QLatin1String("first"))
            loc |= KDReports::FirstPage;
        else if (token == QLatin1String("last"))
            loc |= KDReports::LastPage;
        else if (token == QLatin1String("odd"))
            loc |= KDReports::OddPages;
        else if (token == QLatin1String("even"))
            loc |= KDReports::EvenPages;
        else if (token == QLatin1String("all"))
            loc |= KDReports::AllPages;
        else
            qWarning() << "Found unexpected token in header location attribute:" << token;
    }
    return loc;
}
