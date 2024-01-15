/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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

class KDReports::XmlElementHandlerPrivate
{
public:
    KDReports::ErrorDetails m_details;
};

KDReports::XmlElementHandler::XmlElementHandler()
    : d(new XmlElementHandlerPrivate)
{
}

KDReports::XmlElementHandler::~XmlElementHandler() = default;

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

bool KDReports::XmlElementHandler::hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement)
{
    Q_UNUSED(hLineElement);
    Q_UNUSED(xmlElement);
    return true;
}

void KDReports::XmlElementHandler::customElement(const QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
}

void KDReports::XmlElementHandler::endReport(KDReports::Report &report, const QDomElement &xmlElement)
{
    Q_UNUSED(report);
    Q_UNUSED(xmlElement);
}

bool KDReports::XmlElementHandler::vspace(int &size, QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement);
    Q_UNUSED(size);
    return true;
}

bool KDReports::XmlElementHandler::variable(KDReports::VariableType &type, QDomElement &xmlElement)
{
    Q_UNUSED(type);
    Q_UNUSED(xmlElement);
    return true;
}

bool KDReports::XmlElementHandler::paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement &xmlElement)
{
    Q_UNUSED(left);
    Q_UNUSED(top);
    Q_UNUSED(right);
    Q_UNUSED(bottom);
    Q_UNUSED(xmlElement)
    return true;
}

bool KDReports::XmlElementHandler::tabs(QList<QTextOption::Tab> &tabs, const QDomElement &xmlElement)
{
    Q_UNUSED(tabs);
    Q_UNUSED(xmlElement);
    return true;
}

void KDReports::XmlElementHandler::setErrorDetails(const KDReports::ErrorDetails &details)
{
    d->m_details = details;
}

KDReports::ErrorDetails KDReports::XmlElementHandler::errorDetails()
{
    return d->m_details;
}

void KDReports::XmlElementHandler::virtual_hook(int id, void *data)
{
    Q_UNUSED(id)
    Q_UNUSED(data)
}
