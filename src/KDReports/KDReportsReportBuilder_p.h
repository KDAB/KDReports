/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#ifndef KDREPORTSREPORTBUILDER_H
#define KDREPORTSREPORTBUILDER_H

#include "KDReportsTextDocumentData_p.h"
#include "KDReportsVariableType.h"
#include <QTextCursor>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace KDReports {
class Element;
class Report;
class TextDocument;
class TextDocumentData;

#define REPORTBUILDER_CTOR_ARGDEFS KDReports::TextDocumentData &contentDocument, const QTextCursor &cursor, KDReports::Report *report

#define REPORTBUILDER_CTOR_ARGNAMES contentDocument, cursor, report

/**
 * @internal
 */
class KDREPORTS_EXPORT ReportBuilder
{

public:
    ReportBuilder(KDReports::TextDocumentData &contentDocument, const QTextCursor &cursor, Report *report);
    virtual ~ReportBuilder()
    {
    }

    QTextCursor &cursor()
    {
        m_contentDocument.aboutToModifyContents(TextDocumentData::Append);
        return m_cursor;
    }
    Report *report()
    {
        return m_report;
    }

    // Warning, changes to those virtual methods must be propagated to CellReportBuilder.
    virtual void addPageBreakPublic();
    virtual void addInlineElementPublic(const Element &element);
    virtual void addBlockElementPublic(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor = QColor());
    virtual void addVariablePublic(KDReports::VariableType variable);
    virtual void addVerticalSpacingPublic(qreal space);
    virtual void insertFragmentPublic(const QTextDocumentFragment &fragment);

    // Warning, changes to those virtual methods must be propagated to CellReportBuilder and HeaderReportBuilder.
    virtual void addInlineElement(const Element &element);
    virtual void addBlockElement(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor = QColor());
    virtual void addVariable(KDReports::VariableType variable);
    virtual void addVerticalSpacing(qreal space);

    TextDocumentData &currentDocumentData()
    {
        return m_contentDocument;
    }

    QTextDocument &currentDocument()
    {
        return currentDocumentData().document();
    }

    // Store default font in builder, so that toplevel text elements use the report's default font
    // but table cells use the table's default font (if any)
    void setDefaultFont(const QFont &font)
    {
        m_defaultFont = font;
    }

    QFont defaultFont() const
    {
        return m_defaultFont;
    }

    // should not be used directly, apart for creating another builder (e.g. cell builder)
    TextDocumentData &contentDocumentData()
    {
        return m_contentDocument;
    }
    QTextCursor &contentDocumentCursor()
    {
        return m_cursor;
    }

    void setupBlockFormat(QTextBlockFormat &blockFormat) const;

    void setTabPositions(const QList<QTextOption::Tab> &tabs); // in mm
    // const QList<QTextOption::Tab>& tabPositions() const { return m_tabPositions; }
    void setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom); // in mm
    void copyStateFrom(const ReportBuilder &parentBuilder);
    int currentPosition();

private:
    ReportBuilder(const ReportBuilder &other) = delete;
    ReportBuilder &operator=(const ReportBuilder &other) = delete;

    KDReports::TextDocumentData &m_contentDocument;
    QTextCursor m_cursor;
    QTextCursor m_layoutDocCursor;
    QList<QTextOption::Tab> m_tabPositions;
    qreal m_leftMargin = 0;
    qreal m_rightMargin = 0;
    qreal m_topMargin = 0;
    qreal m_bottomMargin = 0;
    Report *m_report;
    QFont m_defaultFont;
    bool m_first = true;
};
}

QDebug operator<<(QDebug &dbg, const QTextOption::Tab &tab);

#endif /* KDREPORTSREPORTBUILDER_H */
