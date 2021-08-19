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

#include "KDReportsXmlParser_p.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsCell.h"
#include "KDReportsChartElement.h"
#include "KDReportsHLineElement.h"
#include "KDReportsHeader_p.h"
#include "KDReportsHtmlElement.h"
#include "KDReportsImageElement.h"
#include "KDReportsMainTable.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsReport_p.h"
#include "KDReportsTableElement.h"
#include "KDReportsTextElement.h"
#include "KDReportsXmlElementHandler.h"
#include "KDReportsXmlHelper.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QModelIndex>

/* The default report builder adds things right away into the textdocument
 * However for table cells we want to call the Cell methods, which store the stuff
 * until the table is added to the report.
 */
class CellReportBuilder : public KDReports::ReportBuilder
{
public:
    CellReportBuilder(KDReports::Cell &cell, REPORTBUILDER_CTOR_ARGDEFS)
        : ReportBuilder(REPORTBUILDER_CTOR_ARGNAMES)
        , m_cell(cell)
    {
    }

    /*reimp*/ void addInlineElementPublic(const KDReports::Element &element) override { m_cell.addInlineElement(element); }
    /*reimp*/ void addBlockElementPublic(const KDReports::Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &color = QColor()) override
    {
        Q_UNUSED(color);
        m_cell.addElement(element, horizontalAlignment);
    }
    /*reimp*/ void addVariablePublic(KDReports::VariableType variable) override { m_cell.addVariable(variable); }
    /*reimp*/ void addVerticalSpacingPublic(qreal space) override
    {
        Q_UNUSED(space);
        qWarning("Unexpected vspace inside cell!");
    }

private:
    KDReports::Cell &m_cell;
};

bool KDReports::XmlParser::parseTableContents(KDReports::TableElement &table, const QDomNode &tableNode, KDReports::ReportBuilder &builder, bool inHeader, bool inFooter)
{
    // Loop over elements
    for (QDomElement element = tableNode.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {
        if (testForErrorAndFillErrorDetails())
            return false;

        const QString name = element.tagName();
        if (name == QLatin1String("cell")) {
            const int row = element.attribute(QStringLiteral("row")).toInt();
            const int column = element.attribute(QStringLiteral("column")).toInt();
            const int rowSpan = element.attribute(QStringLiteral("rowspan"), QLatin1String("1")).toInt();
            const int colSpan = element.attribute(QStringLiteral("colspan"), QLatin1String("1")).toInt();
            KDReports::Cell &cell = table.cell(row, column);
            cell.setRowSpan(rowSpan);
            cell.setColumnSpan(colSpan);
            const QColor bgColor = KDReports::XmlHelper::readBackground(element);
            if (bgColor.isValid())
                cell.setBackground(bgColor);
            CellReportBuilder cellReportBuilder(cell, builder.contentDocumentData(), builder.contentDocumentCursor(), builder.report());
            cellReportBuilder.copyStateFrom(builder);

            if (m_xmlElementHandler && !m_xmlElementHandler->startCell(cell, element))
                continue;

            if (!processNode(element, &cellReportBuilder, inHeader, inFooter))
                return false;

            if (m_xmlElementHandler && !m_xmlElementHandler->endCell(cell, element))
                continue;
        }
    }

    if (testForErrorAndFillErrorDetails())
        return false;

    return true;
}

void KDReports::XmlParser::parseCommonTableAttributes(KDReports::AbstractTableElement &tableElement, QDomElement &element)
{
    const QColor bgColor = KDReports::XmlHelper::readBackground(element);
    if (bgColor.isValid())
        tableElement.setBackground(bgColor);
    if (element.hasAttribute(QStringLiteral("border")))
        tableElement.setBorder(element.attribute(QStringLiteral("border")).toDouble());
    if (element.hasAttribute(QStringLiteral("width"))) {
        QString widthStr = element.attribute(QStringLiteral("width"));
        if (widthStr.endsWith(QLatin1Char('%'))) {
            widthStr.truncate(widthStr.length() - 1);
            tableElement.setWidth(widthStr.toInt(), KDReports::Percent);
        } else {
            tableElement.setWidth(widthStr.toInt());
        }
    }
}

static QFont parseFontAttributes(const QDomElement &element)
{
    QFont font;
    if (element.hasAttribute(QStringLiteral("pointsize"))) {
        const int pointSize = element.attribute(QStringLiteral("pointsize")).toInt();
        if (pointSize)
            font.setPointSize(pointSize);
    }
    if (element.hasAttribute(QStringLiteral("font"))) {
        font.setFamily(element.attribute(QStringLiteral("font")));
    }
    return font;
}

static void parseHeaderFooterAttribute(KDReports::Header &header, const QDomElement &element)
{
    if (element.hasAttribute(QStringLiteral("font")) || element.hasAttribute(QStringLiteral("pointsize"))) {
        const QFont font = parseFontAttributes(element);
        header.setDefaultFont(font);
    }
}

bool KDReports::XmlParser::processDocument(const QDomDocument &doc, KDReports::ReportBuilder *builder)
{
    // Top-level element should be <report>
    QDomElement docElement = doc.documentElement();
    if (docElement.tagName() != QLatin1String("report")) {
        error(QObject::tr("Expected \"<report>\" as the topmost element, but found \"<%1>\"").arg(docElement.tagName()));
        return false;
    }

    // Support for default page orientation
    if (docElement.hasAttribute(QStringLiteral("orientation"))) {
        const QString orientation = docElement.attribute(QStringLiteral("orientation"));
        if (orientation == QLatin1String("landscape"))
            m_report->setPageOrientation(QPageLayout::Landscape);
        else if (orientation == QLatin1String("portrait"))
            m_report->setPageOrientation(QPageLayout::Portrait);
    }

    // Support for margins
    double marginTop = 20.0; // defaults are duplicated in KDReportsReport.cpp
    if (docElement.hasAttribute(QStringLiteral("margin-top")))
        marginTop = docElement.attribute(QStringLiteral("margin-top")).toDouble();
    double marginLeft = 20.0;
    if (docElement.hasAttribute(QStringLiteral("margin-left")))
        marginLeft = docElement.attribute(QStringLiteral("margin-left")).toDouble();
    double marginBottom = 20.0;
    if (docElement.hasAttribute(QStringLiteral("margin-bottom")))
        marginBottom = docElement.attribute(QStringLiteral("margin-bottom")).toDouble();
    double marginRight = 20.0;
    if (docElement.hasAttribute(QStringLiteral("margin-right")))
        marginRight = docElement.attribute(QStringLiteral("margin-right")).toDouble();
    m_report->setMargins(marginTop, marginLeft, marginBottom, marginRight);

    if (docElement.hasAttribute(QStringLiteral("header-body-spacing")))
        m_report->setHeaderBodySpacing(docElement.attribute(QStringLiteral("header-body-spacing")).toDouble());
    if (docElement.hasAttribute(QStringLiteral("footer-body-spacing")))
        m_report->setFooterBodySpacing(docElement.attribute(QStringLiteral("footer-body-spacing")).toDouble());

    // Support for default font
    if (docElement.hasAttribute(QStringLiteral("font")) || docElement.hasAttribute(QStringLiteral("pointsize"))) {
        const QFont font = parseFontAttributes(docElement);
        m_report->setDefaultFont(font);
    }

    if (m_xmlElementHandler && !m_xmlElementHandler->startReport(*m_report, docElement)) {
        if (m_errorDetails)
            *m_errorDetails = m_xmlElementHandler->errorDetails();

        return false;
    }

    if (!processNode(docElement, builder, false, false))
        return false;

    if (m_xmlElementHandler)
        m_xmlElementHandler->endReport(*m_report, docElement);

    if (testForErrorAndFillErrorDetails())
        return false;

    return true;
}

void KDReports::XmlParser::addElement(KDReports::Element &reportElement, KDReports::ReportBuilder *builder, const QDomElement &element)
{
    if (!builder) {
        error(QObject::tr("<%1> is only supported in WordProcessing mode").arg(element.tagName()));
        return;
    }
    if (element.hasAttribute(QStringLiteral("inline"))) {
        builder->addInlineElementPublic(reportElement);
    } else {
        Qt::AlignmentFlag alignment = Qt::AlignLeft;
        if (element.hasAttribute(QStringLiteral("alignment")))
            alignment = KDReports::XmlHelper::stringToAlignment(element.attribute(QStringLiteral("alignment")));
        builder->addBlockElementPublic(reportElement, alignment);
    }
}

bool KDReports::XmlParser::processNode(const QDomNode &node, KDReports::ReportBuilder *builder, bool inHeader, bool inFooter)
{
    // Loop over elements
    for (QDomElement element = node.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {

        if (testForErrorAndFillErrorDetails())
            return false;

        const QString name = element.tagName();
        if (name == QLatin1String("text")) {
            // Handle <text> element
            KDReports::TextElement textElement;
            QString id;
            const QString text = extractText(element, &id, m_report->d->m_currentModel, m_report->d->m_currentRow);
            textElement.setText(text);
            textElement.setId(id);
            const QColor bgColor = KDReports::XmlHelper::readBackground(element);
            if (bgColor.isValid())
                textElement.setBackground(bgColor);
            if (element.hasAttribute(QStringLiteral("pointsize"))) {
                const int pointSize = element.attribute(QStringLiteral("pointsize")).toInt();
                if (pointSize)
                    textElement.setPointSize(pointSize);
            }
            if (element.hasAttribute(QStringLiteral("color"))) {
                const QString name = element.attribute(QStringLiteral("color"));
                textElement.setTextColor(QColor(name));
            }
            if (element.hasAttribute(QStringLiteral("font"))) {
                textElement.setFontFamily(element.attribute(QStringLiteral("font")));
            }
            if (element.hasAttribute(QStringLiteral("bold"))) {
                bool bold = false;
                if (element.attribute(QStringLiteral("bold")) == QLatin1String("true"))
                    bold = true;
                textElement.setBold(bold);
            }
            if (element.hasAttribute(QStringLiteral("italic"))) {
                bool italic = false;
                if (element.attribute(QStringLiteral("italic")) == QLatin1String("true"))
                    italic = true;
                textElement.setItalic(italic);
            }
            if (element.hasAttribute(QStringLiteral("strikeout"))) {
                bool strikeOut = false;
                if (element.attribute(QStringLiteral("strikeout")) == QLatin1String("true"))
                    strikeOut = true;
                textElement.setStrikeOut(strikeOut);
            }
            if (element.hasAttribute(QStringLiteral("underline"))) {
                bool underline = false;
                if (element.attribute(QStringLiteral("underline")) == QLatin1String("true"))
                    underline = true;
                textElement.setUnderline(underline);
            }

            const QString oldId = textElement.id();

            if (m_xmlElementHandler && !m_xmlElementHandler->textElement(textElement, element))
                continue;

            if (textElement.id() != oldId) {
                // The handler modified the text element's id, look up the value again.
                const QHash<QString, QString>::const_iterator it = m_textValues.constFind(textElement.id());
                if (it != m_textValues.constEnd()) {
                    textElement.setText(*it);
                }
            }

            if (!builder) {
                error(QObject::tr("<text> is only supported in WordProcessing mode"));
            } else {
                if (element.hasAttribute(QStringLiteral("inline"))) {
                    builder->addInlineElementPublic(textElement);
                } else {
                    Qt::AlignmentFlag alignment = Qt::AlignLeft;
                    if (element.hasAttribute(QStringLiteral("alignment")))
                        alignment = KDReports::XmlHelper::stringToAlignment(element.attribute(QStringLiteral("alignment")));
                    const QColor bgColor = KDReports::XmlHelper::readColor(element, "paragraph-background");
                    builder->addBlockElementPublic(textElement, alignment, bgColor);
                }
            }

        } else if (name == QLatin1String("html")) {
            // Handle <html> element
            KDReports::HtmlElement htmlElement;
            QString id;
            const QString text = extractText(element, &id);
            htmlElement.setHtml(text);
            htmlElement.setId(id);
            QColor bgColor = KDReports::XmlHelper::readBackground(element);
            if (bgColor.isValid())
                htmlElement.setBackground(bgColor);

            const QString oldId = htmlElement.id();

            if (m_xmlElementHandler && !m_xmlElementHandler->htmlElement(htmlElement, element))
                continue;

            if (htmlElement.id() != oldId) {
                // The handler modified the text element's id, look up the value again.
                const QHash<QString, QString>::const_iterator it = m_textValues.constFind(htmlElement.id());
                if (it != m_textValues.constEnd()) {
                    htmlElement.setHtml(*it);
                }
            }

            addElement(htmlElement, builder, element);

        } else if (name == QLatin1String("tabs")) {

            if (!builder) {
                error(QObject::tr("<tabs> is only supported in WordProcessing mode"));
            } else {
                parseTabs(builder, element);
            }

        } else if (name == QLatin1String("paragraph-margins")) {

            if (!builder) {
                error(QObject::tr("<paragraph-margins> is only supported in WordProcessing mode"));
            } else {
                parseParagraphMargins(builder, element);
            }

        } else if (name == QLatin1String("hr")) {
            // Handle <hr> element
            KDReports::HtmlElement htmlElement;
            htmlElement.setHtml(QStringLiteral("<hr>"));
            if (m_xmlElementHandler && !m_xmlElementHandler->htmlElement(htmlElement, element))
                continue;

            addElement(htmlElement, builder, element);

        } else if (name == QLatin1String("vspace")) {
            // Handle <vspace> element
            if (!element.hasAttribute(QStringLiteral("size")))
                continue; // size attribute is mandatory for VSpace
            int size = element.attribute(QStringLiteral("size")).toInt();
            if (!builder) {
                error(QObject::tr("<vspace> is only supported in WordProcessing mode"));
            } else {
                if (builder != m_report->d->builder()) {
                    error(QObject::tr("<vspace> not allowed in headers, footers, or table cells"));
                    return false;
                }
            }
            if (m_xmlElementHandler && !m_xmlElementHandler->vspace(size, element))
                continue;
            m_report->addVerticalSpacing(size);
        } else if (name == QLatin1String("table")) {
            // Handle <table> element
            const QString model = element.attribute(QStringLiteral("model"));
            if (model.isEmpty()) {
                if (!builder) {
                    error(QObject::tr("<table> without a model is only supported in WordProcessing mode"));
                    continue;
                }
                KDReports::TableElement tableElement;
                const int headerRowCount = element.attribute(QStringLiteral("headerRowCount")).toInt(); // default 0
                tableElement.setHeaderRowCount(headerRowCount);
                if (element.hasAttribute(QStringLiteral("cellpadding")))
                    tableElement.setPadding(element.attribute(QStringLiteral("cellpadding")).toInt());
                parseCommonTableAttributes(tableElement, element);

                if (m_xmlElementHandler && !m_xmlElementHandler->startTableElement(tableElement, element))
                    continue;

                if (!parseTableContents(tableElement, element, *builder, inHeader, inFooter))
                    return false;

                if (m_xmlElementHandler && !m_xmlElementHandler->endTableElement(tableElement, element))
                    continue;

                addElement(tableElement, builder, element);

            } else {
                KDReports::AutoTableElement tableElement(model);
                if (element.attribute(QStringLiteral("verticalHeaderVisible")) == QLatin1String("false"))
                    tableElement.setVerticalHeaderVisible(false);
                if (element.attribute(QStringLiteral("horizontalHeaderVisible")) == QLatin1String("false"))
                    tableElement.setHorizontalHeaderVisible(false);
                QColor headerBgColor = KDReports::XmlHelper::readColor(element, "header-background");
                if (headerBgColor.isValid())
                    tableElement.setHeaderBackground(headerBgColor);
                parseCommonTableAttributes(tableElement, element);
                if (m_xmlElementHandler && !m_xmlElementHandler->autoTableElement(tableElement, element))
                    continue;

                if (m_report->reportMode() == Report::SpreadSheet) {
                    m_report->mainTable()->setAutoTableElement(tableElement);
                } else {
                    addElement(tableElement, builder, element);
                }
            }
        } else if (name == QLatin1String("chart")) {
            // Handle <chart> element

            KDReports::ChartElement chartElement(element.attribute(QStringLiteral("model")));
            QColor bgColor = KDReports::XmlHelper::readBackground(element);
            if (bgColor.isValid())
                chartElement.setBackground(bgColor);
            int width = 100, height = 100;
            Unit unit = Millimeters;
            if (element.hasAttribute(QStringLiteral("width"))) {
                QString str = element.attribute(QStringLiteral("width"));
                if (str.endsWith(QLatin1Char('%'))) {
                    str.chop(1);
                    unit = Percent;
                }
                width = str.toInt();
            }
            if (element.hasAttribute(QStringLiteral("height"))) {
                QString str = element.attribute(QStringLiteral("height"));
                if (str.endsWith(QLatin1Char('%'))) {
                    str.chop(1);
                    unit = Percent;
                }
                height = str.toInt();
            }
            chartElement.setSize(width, height, unit);

            if (m_xmlElementHandler && !m_xmlElementHandler->chartElement(chartElement, element))
                continue;

            addElement(chartElement, builder, element);

        } else if (name == QLatin1String("image")) {
            // Handle <image> element

            QString id;
            QImage image = extractImage(element, &id);
            KDReports::ImageElement imageElement(image);
            imageElement.setId(id);
            if (element.hasAttribute(QStringLiteral("width"))) {
                QString widthStr = element.attribute(QStringLiteral("width"));
                if (widthStr.endsWith(QLatin1Char('%'))) {
                    widthStr.truncate(widthStr.length() - 1);
                    imageElement.setWidth(widthStr.toInt(), KDReports::Percent);
                } else {
                    imageElement.setWidth(widthStr.toInt());
                }
            } else if (element.hasAttribute(QStringLiteral("height"))) { // mutually exclusive with "width"!
                QString heightStr = element.attribute(QStringLiteral("height"));
                if (heightStr.endsWith(QLatin1Char('%'))) {
                    heightStr.truncate(heightStr.length() - 1);
                    imageElement.setHeight(heightStr.toInt(), KDReports::Percent);
                } else {
                    imageElement.setHeight(heightStr.toInt());
                }
            } else if (element.hasAttribute(QStringLiteral("fitToPage"))) {
                imageElement.setFitToPage();
            }

            if (m_xmlElementHandler && !m_xmlElementHandler->imageElement(imageElement, element))
                continue;

            addElement(imageElement, builder, element);

        } else if (name == QLatin1String("header")) {
            const KDReports::HeaderLocations loc = KDReports::XmlHelper::parseHeaderLocation(element.attribute(QStringLiteral("location")));
            KDReports::Header &header = m_report->header(loc);
            parseHeaderFooterAttribute(header, element);
            if (m_xmlElementHandler && !m_xmlElementHandler->startHeader(header, element))
                continue;
            if (!processNode(element, &header.d->m_builder, true, false))
                return false;
            if (m_xmlElementHandler)
                m_xmlElementHandler->endHeader(header, element);
        } else if (name == QLatin1String("footer")) {
            const KDReports::HeaderLocations loc = KDReports::XmlHelper::parseHeaderLocation(element.attribute(QStringLiteral("location")));
            KDReports::Footer &footer = m_report->footer(loc);
            parseHeaderFooterAttribute(footer, element);
            if (m_xmlElementHandler && !m_xmlElementHandler->startFooter(footer, element))
                continue;
            if (!processNode(element, &footer.d->m_builder, false, true))
                return false;
            if (m_xmlElementHandler)
                m_xmlElementHandler->endFooter(footer, element);
        } else if (name == QLatin1String("variable")) {
            if (!inHeader && !inFooter) {
                error(QObject::tr("<variable> tags only allowed in headers and footers"));
                return false;
            }
            if (!element.hasAttribute(QStringLiteral("type"))) {
                error(QObject::tr("<variable> tags must have a 'type' attribute"));
                return false;
            }
            Q_ASSERT(builder);
            if (builder) {
                const QString type = element.attribute(QStringLiteral("type"));
                KDReports::VariableType vt = KDReports::XmlHelper::stringToVariableType(type);
                if (m_xmlElementHandler && !m_xmlElementHandler->variable(vt, element))
                    continue;
                builder->addVariablePublic(vt);
            }
        } else if (name == QLatin1String("page-break")) {
            if (m_xmlElementHandler && !m_xmlElementHandler->pageBreak(element))
                continue;
            m_report->addPageBreak();
        } else if (name == QLatin1String("ifdef")) {
            if (element.hasAttribute(QStringLiteral("id"))) {
                const QString id = element.attribute(QStringLiteral("id"));
                const bool skip = m_textValues.value(id).isEmpty();
                if (!skip) {
                    if (!processNode(element, builder, inHeader, inFooter))
                        return false;
                }
            }
        } else if (name == QLatin1String("custom")) {
            if (m_xmlElementHandler)
                m_xmlElementHandler->customElement(element);
        } else if (name == QLatin1String("hline")) {
            KDReports::HLineElement hLineElement;

            if (element.hasAttribute(QStringLiteral("thickness"))) {
                const double thickness = element.attribute(QStringLiteral("thickness")).toDouble();
                hLineElement.setThickness(thickness);
            }

            if (element.hasAttribute(QStringLiteral("color"))) {
                const QColor color = KDReports::XmlHelper::readColor(element, "color");
                hLineElement.setColor(color);
            }

            if (element.hasAttribute(QStringLiteral("margin"))) {
                const int margin = element.attribute(QStringLiteral("margin")).toInt();
                hLineElement.setMargin(margin);
            }

            if (m_xmlElementHandler && !m_xmlElementHandler->hLineElement(hLineElement, element))
                continue;

            addElement(hLineElement, builder, element);
        } else {
            error(QObject::tr("KDReports::Report::loadFromXML(): Unknown element %1").arg(name));
        }
    }

    if (testForErrorAndFillErrorDetails())
        return false;

    return true;
}

void KDReports::XmlParser::parseTabs(KDReports::ReportBuilder *builder, const QDomElement &tabsElement)
{
    QList<QTextOption::Tab> tabs;
    for (QDomElement element = tabsElement.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {
        if (element.tagName() == QLatin1String("tab")) {

            QTextOption::Tab tab;
            const QString tabType = element.attribute(QStringLiteral("type"));
            if (tabType == QLatin1String("right")) {
                tab.type = QTextOption::RightTab;
            } else if (tabType == QLatin1String("center")) {
                tab.type = QTextOption::CenterTab;
            } else if (tabType == QLatin1String("delimiter")) {
                tab.type = QTextOption::DelimiterTab;
            } else {
                tab.type = QTextOption::LeftTab;
            }

            const QString delimiter = element.attribute(QStringLiteral("delimiter"));
            if (!delimiter.isEmpty())
                tab.delimiter = delimiter.at(0);

            const QString strPos = element.attribute(QStringLiteral("position"));
            double pos = -1;
            if (strPos == QLatin1String("page")) {
                tab.delimiter = QChar::fromLatin1('P');
            } else {
                pos = strPos.toDouble();
            }
            tab.position = pos;
            tabs.append(tab);
        }
    }
    if (!m_xmlElementHandler || m_xmlElementHandler->tabs(tabs, tabsElement))
        builder->setTabPositions(tabs);
}

void KDReports::XmlParser::parseParagraphMargins(KDReports::ReportBuilder *builder, const QDomElement &element)
{
    qreal left = element.attribute(QStringLiteral("left")).toDouble();
    qreal top = element.attribute(QStringLiteral("top")).toDouble();
    qreal right = element.attribute(QStringLiteral("right")).toDouble();
    qreal bottom = element.attribute(QStringLiteral("bottom")).toDouble();
    if (!m_xmlElementHandler || m_xmlElementHandler->paragraphMargin(left, top, right, bottom, element))
        builder->setParagraphMargins(left, top, right, bottom);
}

QImage KDReports::XmlParser::extractImage(const QDomElement &element, QString *pId) const
{
    if (element.hasAttribute(QStringLiteral("id"))) {
        const QString id = element.attribute(QStringLiteral("id"));
        *pId = id;
        const QHash<QString, QImage>::const_iterator it = m_imageValues.find(id);
        if (it != m_imageValues.end()) {
            return *it;
        }
    }

    QImage image;
    if (element.hasAttribute(QStringLiteral("file"))) {
        const QString fileName = element.attribute(QStringLiteral("file"));
        if (!image.load(fileName)) {
            qWarning("Image not found or invalid: %s", qPrintable(fileName));
        }
    }
    return image;
}

QString KDReports::XmlParser::extractText(const QDomElement &element, QString *pId, const QAbstractItemModel *currentModel, int currentRow) const
{
    if (element.hasAttribute(QStringLiteral("id"))) {
        const QString id = element.attribute(QStringLiteral("id"));
        *pId = id;
        const QHash<QString, QString>::const_iterator it = m_textValues.find(id);
        if (it != m_textValues.end()) {
            return *it;
        }
    } else if (element.hasAttribute(QStringLiteral("model"))) {
        const QString modelName = element.attribute(QStringLiteral("model"));
        QAbstractItemModel *model = KDReports::modelForKey(modelName);
        if (model) {
            int row;
            if (model == currentModel && currentRow > -1) {
                row = currentRow;
            } else {
                row = element.attribute(QStringLiteral("row")).toInt();
            }
            const int column = element.attribute(QStringLiteral("column")).toInt();
            const QModelIndex index = model->index(row, column);
            return model->data(index).toString();
        }
    }

    return element.text();
}

bool KDReports::XmlParser::testForErrorAndFillErrorDetails()
{
    if (m_xmlElementHandler && m_xmlElementHandler->errorDetails().hasError()) {
        if (m_errorDetails)
            *m_errorDetails = m_xmlElementHandler->errorDetails();
        return true;
    }
    return false;
}

void KDReports::XmlParser::error(const QString &errorString)
{
    if (m_errorDetails)
        m_errorDetails->setDriverMessage(errorString);
    else
        qWarning("%s", qPrintable(errorString));
}
