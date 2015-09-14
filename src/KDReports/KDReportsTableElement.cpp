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

#include "KDReportsTableElement.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsReport.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsTextDocument_p.h"
#include "KDReportsCell.h"
#include <QPainter>
#include <QTextCursor>
#include <QTextTableCell>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

namespace KDReports
{
class CellContentMap : public QMap<QPair<int /*row*/, int /*column*/>, Cell>
{
public:
    CellContentMap() {}
    void getSize( int& rows, int& columns ) const {
        rows = 0;
        columns = 0;
        for ( const_iterator it = begin(); it != end(); ++it ) {
            rows = qMax( rows, it.key().first + 1 );
            columns = qMax( columns, it.key().second + 1 );
        }
    }
};
}

class KDReports::TableElementPrivate
{
public:
    TableElementPrivate()
        : m_headerRowCount( 0 ),
          m_headerColumnCount( 0 )
    {}
    ~TableElementPrivate() {}

    KDReports::CellContentMap m_cellContentMap;
    int m_headerRowCount;
    int m_headerColumnCount;
};

////

KDReports::TableElement::TableElement()
    : d( new TableElementPrivate )
{
}

KDReports::TableElement::TableElement(const TableElement &other)
    : AbstractTableElement( other ), d( new TableElementPrivate( *other.d ) )
{
}

KDReports::TableElement & KDReports::TableElement::operator=(const TableElement &other)
{
    if ( &other == this )
        return *this;
    AbstractTableElement::operator=( other );
    *d = *other.d;
    return *this;
}

KDReports::TableElement::~TableElement()
{
    delete d;
}

KDReports::Element* KDReports::TableElement::clone() const
{
    // never used at the moment
    return new TableElement( *this );
}

void KDReports::TableElement::setHeaderRowCount( int count )
{
    d->m_headerRowCount = count;
}

int KDReports::TableElement::headerRowCount() const
{
    return d->m_headerRowCount;
}

void KDReports::TableElement::setHeaderColumnCount( int count )
{
    d->m_headerColumnCount = count;
}

int KDReports::TableElement::headerColumnCount() const
{
    return d->m_headerColumnCount;
}

KDReports::Cell& KDReports::TableElement::cell( int row, int column )
{
    const QPair<int, int> coord = qMakePair( row, column );
    return d->m_cellContentMap[ coord ]; // find or create
}

void KDReports::TableElement::build( ReportBuilder& builder ) const
{
    if ( d->m_cellContentMap.isEmpty() )
        return;

    QTextCursor& textDocCursor = builder.cursor();

    int rowCount, columnCount;
    d->m_cellContentMap.getSize( rowCount, columnCount );

    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount( d->m_headerRowCount );
    tableFormat.setProperty( KDReports::HeaderColumnsProperty, d->m_headerColumnCount );
    tableFormat.setAlignment( textDocCursor.blockFormat().alignment() );
    tableFormat.setBackground(background());
    fillTableFormat( tableFormat, textDocCursor );

    QTextTable* textTable = textDocCursor.insertTable( rowCount, columnCount,
                                                       tableFormat );

    CellContentMap::const_iterator it = d->m_cellContentMap.constBegin();
    for ( ; it != d->m_cellContentMap.constEnd() ; ++it ) {
        const int row = it.key().first;
        const int column = it.key().second;
        const Cell& cell = it.value();
        if ( cell.columnSpan() > 1 || cell.rowSpan() > 1 )
            textTable->mergeCells( row, column, cell.rowSpan(), cell.columnSpan() );
        QTextTableCell tableCell = textTable->cellAt( row, column );
        Q_ASSERT( tableCell.isValid() );
        QTextCursor cellCursor = tableCell.firstCursorPosition();
        QTextCharFormat tableCellFormat;
        tableCellFormat.setBackground( cell.background() );
        tableCellFormat.setTableCellColumnSpan( cell.columnSpan() );
        tableCellFormat.setTableCellRowSpan( cell.rowSpan() );
        tableCell.setFormat( tableCellFormat );
        cellCursor.setCharFormat( tableCellFormat );
        ReportBuilder cellBuilder( builder.currentDocumentData(),
                                   cellCursor, builder.report() );
        cellBuilder.copyStateFrom( builder );
        cell.build( cellBuilder );
    }

    textDocCursor.movePosition( QTextCursor::End );

    builder.currentDocumentData().registerTable( textTable );
}
