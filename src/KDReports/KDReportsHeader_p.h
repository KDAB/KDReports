/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORTSHEADER_P_H
#define KDREPORTSHEADER_P_H

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

#include "KDReportsHeader.h"
#include "KDReportsReport.h"
#include "KDReportsReport_p.h"
#include "KDReportsTextDocument_p.h"
#include "KDReportsReportBuilder_p.h"

namespace KDReports
{

/**
 * @internal
 */
void setVariableMarker( QTextDocument& textDoc, int pos, KDReports::VariableType variableType, int valueLength );
/**
 * @internal
 */
QString variableValue( int pageNumber, KDReports::Report* report, VariableType type );
/**
 * @internal
 */
void cleanupVariableProperties( QTextCharFormat &charFormat );

/**
 * @internal
 */
class HeaderReportBuilder : public KDReports::ReportBuilder
{
public:
    HeaderReportBuilder( REPORTBUILDER_CTOR_ARGDEFS )  //krazy:exclude=explicit
        : ReportBuilder( REPORTBUILDER_CTOR_ARGNAMES ) {}

    /*reimp*/ void addInlineElement( const KDReports::Element& element ) override {
        ReportBuilder::addInlineElement( element );
        report()->d->headerChanged();
    }
    /*reimp*/ void addBlockElement( const KDReports::Element& element, Qt::AlignmentFlag horizontalAlignment, const QColor& background = QColor() ) override {
        ReportBuilder::addBlockElement( element, horizontalAlignment, background );
        report()->d->headerChanged();
    }
    /*reimp*/ void addVerticalSpacing( qreal space ) override {
        ReportBuilder::addVerticalSpacing( space );
        report()->d->headerChanged();
    }
};

/**
 * @internal
 */
class HeaderPrivate
{
public:
    explicit HeaderPrivate( KDReports::Report* report )
        : m_textDocument(),
          m_builder( m_textDocument.contentDocumentData(),
                     QTextCursor( &m_textDocument.contentDocument() ),
                     report ),
          m_report( report )
    {
    }
    KDReports::TextDocument m_textDocument;
    KDReports::HeaderReportBuilder m_builder;
    KDReports::Report* m_report;
};

}

#endif /* KDREPORTSHEADER_P_H */

