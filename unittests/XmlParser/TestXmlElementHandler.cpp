/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "TestXmlElementHandler.h"
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
#include <QDebug>
#include <QtXml/QDomElement>

TestXmlElementHandler::TestXmlElementHandler(KDReports::Report &report)
    : mReport(report)
{
}

bool TestXmlElementHandler::startReport(KDReports::Report &report, QDomElement &xmlElement)
{
    Q_UNUSED(report); // unused in release mode
    Q_ASSERT(&report == &mReport);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "report");
    cb << "startReport";
    return true;
}

bool TestXmlElementHandler::startHeader(KDReports::Header &header, QDomElement &xmlElement)
{
    Q_UNUSED(header);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "header");
    cb << "startHeader";
    return true;
}

void TestXmlElementHandler::endHeader(KDReports::Header &header, const QDomElement &xmlElement)
{
    Q_UNUSED(header);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "header");
    cb << "endHeader";
}

bool TestXmlElementHandler::startFooter(KDReports::Footer &footer, QDomElement &xmlElement)
{
    Q_UNUSED(footer);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "footer");
    cb << "startFooter";
    return true;
}

void TestXmlElementHandler::endFooter(KDReports::Footer &footer, const QDomElement &xmlElement)
{
    Q_UNUSED(footer);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "footer");
    cb << "endFooter";
}

bool TestXmlElementHandler::textElement(KDReports::TextElement &textElement, QDomElement &xmlElement)
{
    Q_UNUSED(textElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "text");
    cb << "textElement";
    if (textElement.id() == "testModification")
        textElement.setId("testModified");
    return true;
}

bool TestXmlElementHandler::htmlElement(KDReports::HtmlElement &htmlElement, QDomElement &xmlElement)
{
    Q_UNUSED(htmlElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "html");
    cb << "htmlElement";
    return true;
}

bool TestXmlElementHandler::startTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "table");
    cb << "startTableElement";
    return true;
}

bool TestXmlElementHandler::endTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "table");
    cb << "endTableElement";
    return true;
}

bool TestXmlElementHandler::startCell(KDReports::Cell &cell, QDomElement &xmlElement)
{
    Q_UNUSED(cell);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "cell");
    cb << "startCell";
    return true;
}

bool TestXmlElementHandler::endCell(KDReports::Cell &cell, QDomElement &xmlElement)
{
    Q_UNUSED(cell);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "cell");
    cb << "endCell";
    return true;
}

bool TestXmlElementHandler::autoTableElement(KDReports::AutoTableElement &tableElement, QDomElement &xmlElement)
{
    Q_UNUSED(tableElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "table");
    cb << "autoTableElement";
    return true;
}

bool TestXmlElementHandler::chartElement(KDReports::ChartElement &chartElement, QDomElement &xmlElement)
{
    Q_UNUSED(chartElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "chart");
    cb << "chartElement";
    return true;
}

bool TestXmlElementHandler::imageElement(KDReports::ImageElement &imageElement, QDomElement &xmlElement)
{
    Q_UNUSED(imageElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "image");

    if (xmlElement.hasAttribute("broken")) {
        KDReports::ErrorDetails details;
        details.setDriverMessage(QString("PC LOAD LETTER"));
        setErrorDetails(details);
        return false;
    }
    cb << "imageElement";
    return true;
}

bool TestXmlElementHandler::pageBreak(QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "page-break");
    cb << "pageBreak";
    return true;
}

void TestXmlElementHandler::customElement(const QDomElement &xmlElement)
{
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "custom");
    cb << "customElement";
    // Test inserting elements from the XML element handler
    if (xmlElement.attribute("id") == "lastParagraph")
        mReport.addElement(KDReports::TextElement("Custom element"));
}

bool TestXmlElementHandler::hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement)
{
    Q_UNUSED(hLineElement);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "hline");
    cb << "hLineElement";
    return true;
}

bool TestXmlElementHandler::variable(KDReports::VariableType &type, QDomElement &xmlElement)
{
    Q_UNUSED(type);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "variable");
    cb << "variable";
    return true;
}

bool TestXmlElementHandler::vspace(int &size, QDomElement &xmlElement)
{
    Q_UNUSED(size);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "vspace");
    cb << "vspace";
    return true;
}

bool TestXmlElementHandler::paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement &xmlElement)
{
    Q_UNUSED(left);
    Q_UNUSED(top);
    Q_UNUSED(right);
    Q_UNUSED(bottom);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "paragraph-margins");
    cb << "paragraph-margins";
    return true;
}

bool TestXmlElementHandler::tabs(QList<QTextOption::Tab> &tabs, const QDomElement &xmlElement)
{
    Q_UNUSED(tabs);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "tabs");
    cb << "tabs";
    return true;
}

void TestXmlElementHandler::endReport(KDReports::Report &report, const QDomElement &xmlElement)
{
    Q_UNUSED(report); // unused in release mode
    Q_ASSERT(&report == &mReport);
    Q_UNUSED(xmlElement); // unused in release mode
    Q_ASSERT(xmlElement.tagName() == "report");
    cb << "endReport";
}
