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
class HeaderReportBuilder : public KDReports::ReportBuilder
{
public:
    HeaderReportBuilder( REPORTBUILDER_CTOR_ARGDEFS )  //krazy:exclude=explicit
        : ReportBuilder( REPORTBUILDER_CTOR_ARGNAMES ) {}

    /*reimp*/ void addInlineElement( const KDReports::Element& element ) {
        ReportBuilder::addInlineElement( element );
        report()->d->headerChanged();
    }
    /*reimp*/ void addBlockElement( const KDReports::Element& element, Qt::AlignmentFlag horizontalAlignment, const QColor& background = QColor() ) {
        ReportBuilder::addBlockElement( element, horizontalAlignment, background );
        report()->d->headerChanged();
    }
    /*reimp*/ void addVerticalSpacing( qreal space ) {
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

