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

#ifndef KDREPORTSXMLPARSER_H
#define KDREPORTSXMLPARSER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Reports API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#include "KDReportsErrorDetails.h"
#include "KDReportsGlobal.h"
#include "KDReportsReport.h"
#include <QPixmap>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE
class QDomDocument;
class QDomNode;
QT_END_NAMESPACE

namespace KDReports {
class TableElement;
class XmlElementHandler;
class AbstractTableElement;

/**
 * @internal
 * XML parser for Report
 */
class XmlParser
{
public:
    XmlParser(const QHash<QString, QString> &textValues, const QHash<QString, QImage> &imageValues, XmlElementHandler *xmlHandler, KDReports::Report *report, ErrorDetails *details)
        : m_textValues(textValues)
        , m_imageValues(imageValues)
        , m_xmlElementHandler(xmlHandler)
        , m_report(report)
        , m_errorDetails(details)
    {
    }

    bool processDocument(const QDomDocument &document, KDReports::ReportBuilder *builder);

private:
    bool processNode(const QDomNode &node, KDReports::ReportBuilder *builder, bool inHeader, bool inFooter);
    void addElement(KDReports::Element &reportElement, KDReports::ReportBuilder *builder, const QDomElement &element);
    bool parseTableContents(KDReports::TableElement &table, const QDomNode &tableNode, KDReports::ReportBuilder &builder, bool inHeader, bool inFooter);
    void parseCommonTableAttributes(KDReports::AbstractTableElement &tableElement, QDomElement &element);
    void parseTabs(KDReports::ReportBuilder *builder, const QDomElement &element);
    void parseParagraphMargins(KDReports::ReportBuilder *builder, const QDomElement &element);
    QString extractText(const QDomElement &element, QString *id, const QAbstractItemModel *currentModel = 0, int currentRow = -1) const;
    QImage extractImage(const QDomElement &element, QString *pId) const;
    bool testForErrorAndFillErrorDetails();
    void error(const QString &errorString);

    QHash<QString, QString> m_textValues;
    QHash<QString, QImage> m_imageValues;
    XmlElementHandler *m_xmlElementHandler;
    KDReports::Report *m_report;
    ErrorDetails *m_errorDetails;
};

}

#endif
