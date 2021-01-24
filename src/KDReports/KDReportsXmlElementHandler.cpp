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

#include "KDReportsXmlElementHandler.h"
#include "KDReportsErrorDetails_p.h"
#ifdef Q_CC_MSVC
// Q_UNUSED requires the type to be known in msvc
#include "KDReportsAutoTableElement.h"
#include "KDReportsCell.h"
#include "KDReportsChartElement.h"
#include "KDReportsHLineElement.h"
#include "KDReportsHeader.h"
#include "KDReportsHtmlElement.h"
#include "KDReportsImageElement.h"
#include "KDReportsReport.h"
#include "KDReportsTableElement.h"
#include "KDReportsTextElement.h"
#endif
#include <QDomElement>

KDReports::XmlElementHandler::XmlElementHandler() { }

KDReports::XmlElementHandler::~XmlElementHandler() { }

bool KDReports::XmlElementHandler::startReport(KDReports::Report &report, QDomElement &xmlElement)
{
    Q_UNUSED(report);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::startHeader(KDReports::Header &header, QDomElement &xmlElement)
{
    Q_UNUSED(header);
    Q_UNUSED(xmlElement);
    return true;
}

void KDReports::XmlElementHandler::endHeader(KDReports::Header &header, const QDomElement &xmlElement)
{
    Q_UNUSED(header);
    Q_UNUSED(xmlElement);
}

bool KDReports::XmlElementHandler::startFooter(KDReports::Footer &footer, QDomElement &xmlElement)
{
    Q_UNUSED(footer);
    Q_UNUSED(xmlElement);
    return true;
}

void KDReports::XmlElementHandler::endFooter(KDReports::Footer &footer, const QDomElement &xmlElement)
{
    Q_UNUSED(footer);
    Q_UNUSED(xmlElement);
}

bool KDReports::XmlElementHandler::textElement(KDReports::TextElement &textElement, QDomElement &xmlElement)
{
    Q_UNUSED(textElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::htmlElement(KDReports::HtmlElement &htmlElement, QDomElement &xmlElement)
{
    Q_UNUSED(htmlElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::startTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::endTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::startCell(KDReports::Cell &cell, QDomElement &xmlElement)
{
    Q_UNUSED(cell);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::endCell(KDReports::Cell &cell, QDomElement &xmlElement)
{
    Q_UNUSED(cell);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::autoTableElement(KDReports::AutoTableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::chartElement(KDReports::ChartElement &chartElement, QDomElement &xmlElement)
{
    Q_UNUSED(chartElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::imageElement(KDReports::ImageElement &imageElement, QDomElement &xmlElement)
{
    Q_UNUSED(imageElement);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::pageBreak(QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    return true;
}

#ifdef KDREPORTS_ALLOW_BINARY_INCOMPATIBILITY
bool KDReports::XmlElementHandler::hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement)
{
    Q_UNUSED(hLineElement);
    Q_UNUSED(xmlElement);
    return true;
}
#endif

void KDReports::XmlElementHandler::customElement(const QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
}

void KDReports::XmlElementHandler::endReport(KDReports::Report &report, const QDomElement &xmlElement)
{
    Q_UNUSED(report);
    Q_UNUSED(xmlElement);
}

void KDReports::XmlElementHandler::setErrorDetails(const KDReports::ErrorDetails &details)
{
    m_details = details;
}

KDReports::ErrorDetails KDReports::XmlElementHandler::errorDetails()
{
    return m_details;
}
