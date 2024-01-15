/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef TESTXMLELEMENTHANDLER_H
#define TESTXMLELEMENTHANDLER_H

#include "KDReportsXmlElementHandler.h"
#include <QStringList>

class TestXmlElementHandler : public KDReports::XmlElementHandler
{
public:
    explicit TestXmlElementHandler(KDReports::Report &report);
    QStringList callbacks() const
    {
        return cb;
    }

    bool startReport(KDReports::Report &report, QDomElement &xmlElement) override;
    bool startHeader(KDReports::Header &header, QDomElement &xmlElement) override;
    void endHeader(KDReports::Header &header, const QDomElement &xmlElement) override;
    bool startFooter(KDReports::Footer &footer, QDomElement &xmlElement) override;
    void endFooter(KDReports::Footer &footer, const QDomElement &xmlElement) override;
    bool textElement(KDReports::TextElement &textElement, QDomElement &xmlElement) override;
    bool htmlElement(KDReports::HtmlElement &htmlElement, QDomElement &xmlElement) override;
    bool startTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement) override;
    bool endTableElement(KDReports::TableElement &tableElement, QDomElement &xmlElement) override;
    bool startCell(KDReports::Cell &cell, QDomElement &xmlElement) override;
    bool endCell(KDReports::Cell &cell, QDomElement &xmlElement) override;
    bool autoTableElement(KDReports::AutoTableElement &tableElement, QDomElement &xmlElement) override;
    bool chartElement(KDReports::ChartElement &chartElement, QDomElement &xmlElement) override;
    bool imageElement(KDReports::ImageElement &imageElement, QDomElement &xmlElement) override;
    bool pageBreak(QDomElement &xmlElement) override;
    bool hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement) override;
    void customElement(const QDomElement &xmlElement) override;
    bool variable(KDReports::VariableType &type, QDomElement &xmlElement) override;
    bool vspace(int &size, QDomElement &xmlElement) override;
    bool paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement &xmlElement) override;
    bool tabs(QList<QTextOption::Tab> &tabs, const QDomElement &xmlElement) override;
    void endReport(KDReports::Report &report, const QDomElement &xmlElement) override;

private:
    QStringList cb;
    KDReports::Report &mReport;
};

#endif /* TESTXMLELEMENTHANDLER_H */
