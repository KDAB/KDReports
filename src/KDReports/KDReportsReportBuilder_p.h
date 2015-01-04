/****************************************************************************
** Copyright (C) 2007-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Reports library.
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2.1 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#ifndef KDREPORTSREPORTBUILDER_H
#define KDREPORTSREPORTBUILDER_H

#include <QTextCursor>
#include "KDReportsVariableType.h"
#include "KDReportsTextDocumentData_p.h"
class QTextDocument;

namespace KDReports {
class Element;
class Report;
class TextDocument;
class TextDocumentData;

#define REPORTBUILDER_CTOR_ARGDEFS                      \
        KDReports::TextDocumentData& contentDocument,   \
        const QTextCursor& cursor,                      \
        KDReports::Report* report

#define REPORTBUILDER_CTOR_ARGNAMES contentDocument, cursor, report

/**
 * @internal
 */
class KDREPORTS_EXPORT ReportBuilder {

public:
    ReportBuilder( KDReports::TextDocumentData& contentDocument,
                   const QTextCursor& cursor,
                   Report* report );
    virtual ~ReportBuilder() {}

    QTextCursor& cursor() {
            m_contentDocument.aboutToModifyContents(TextDocumentData::Append);
            return m_cursor;
    }
    Report* report() { return m_report; }

    // Warning, changes to those virtual methods must be propagated to CellReportBuilder.
    virtual void addPageBreakPublic();
    virtual void addInlineElementPublic( const Element& element );
    virtual void addBlockElementPublic( const Element& element, Qt::AlignmentFlag horizontalAlignment,
                                        const QColor& backgroundColor = QColor() );
    virtual void addVariablePublic( KDReports::VariableType variable );
    virtual void addVerticalSpacingPublic( qreal space );
    virtual void insertFragmentPublic( const QTextDocumentFragment& fragment );

    // Warning, changes to those virtual methods must be propagated to CellReportBuilder and HeaderReportBuilder.
    virtual void addInlineElement( const Element& element );
    virtual void addBlockElement( const Element& element, Qt::AlignmentFlag horizontalAlignment,
                                  const QColor& backgroundColor = QColor() );
    virtual void addVariable( KDReports::VariableType variable );
    virtual void addVerticalSpacing( qreal space );

    TextDocumentData& currentDocumentData() {
        return m_contentDocument;
    }

    QTextDocument& currentDocument() {
        return currentDocumentData().document();
    }

    // should not be used directly, apart for creating another builder (e.g. cell builder)
    TextDocumentData& contentDocumentData() {
        return m_contentDocument;
    }
    QTextCursor& contentDocumentCursor() {
        return m_cursor;
    }

    void setupBlockFormat( QTextBlockFormat& blockFormat );

    void setTabPositions( const QList<QTextOption::Tab>& tabs ); // in mm
    //const QList<QTextOption::Tab>& tabPositions() const { return m_tabPositions; }
    void setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom); // in mm
    void copyStateFrom( ReportBuilder& parentBuilder );
    int currentPosition();

private:
    KDReports::TextDocumentData& m_contentDocument;
    QTextCursor m_cursor;
    QTextCursor m_layoutDocCursor;
    QList<QTextOption::Tab> m_tabPositions;
    qreal m_leftMargin;
    qreal m_rightMargin;
    qreal m_topMargin;
    qreal m_bottomMargin;
    Report* m_report;
    bool m_first;
};

}

QDebug operator<<(QDebug& dbg, const QTextOption::Tab& tab);

#endif /* KDREPORTSREPORTBUILDER_H */

