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

#include "KDReportsTableBreakingLogic_p.h"

using namespace KDReports;

TableBreakingLogic::TableBreakingLogic()
    : m_pages( 1 )
{
}

void TableBreakingLogic::setColumnWidths( const WidthVector& widths )
{
    m_widths = widths;
}

void TableBreakingLogic::setPageCount( int pages )
{
    m_pages = pages;
}

QVector<int> TableBreakingLogic::columnsPerPage() const
{
    QVector<int> columnsForPage;
    if ( m_pages == 0 )
        return columnsForPage;
    const int numColumnsForPage = m_widths.count();
    qreal totalWidth = 0;
    for ( int i = 0; i < numColumnsForPage; ++i ) {
        totalWidth += m_widths[i];
    }
    // TODO handle m_pages==-1, i.e. the case where we don't care how many pages we get
    // horizontally, cf Report::setTableBreakingEnabled(true). But for this we need to
    // pass the page-content-width to this code here [in pixels]...
    const qreal idealPageWidth = totalWidth / m_pages;
    //qDebug() << "idealPageWidth=" << idealPageWidth;
    qreal currentWidth = 0;
    columnsForPage.resize( m_pages );
    int columnsInThisPage = 0;
    int pageNumber = 0;
    for ( int i = 0; i < numColumnsForPage; ++i ) {
        const qreal columnWidth = m_widths[i];
        const bool lastPage = pageNumber == m_pages - 1;
        if ( currentWidth + columnWidth > idealPageWidth && !lastPage ) {
            // Better to include or exclude the column?
            // That's the tricky part. If we exclude too much
            // the last page will be crammed, while if we include
            // a huge column this one might be crammed.
            // Let's have a threshold then: include it if we won't go over-board
            // more than 1/3rd of the page width.
            if ( currentWidth + columnWidth < idealPageWidth * 4 / 3
                || currentWidth == 0 ) {   // Avoid infinite loop if huge column
                columnsInThisPage += 1;
            } else {
                // current column will be for next page.
                --i;
            }
            columnsForPage[pageNumber++] = columnsInThisPage;

            currentWidth = 0;
            columnsInThisPage = 0;
        } else {
            currentWidth += columnWidth;
            columnsInThisPage += 1;
        }
    }
    if ( pageNumber < m_pages && columnsInThisPage > 0 ) {
        columnsForPage[pageNumber++] = columnsInThisPage;
    }
    columnsForPage.resize( pageNumber );
#ifdef DEBUG_TABLEBREAKINGLOGIC
    qDebug() << "Result of optimized table breaking:" << columnsForPage;
#endif
    return columnsForPage;
}


TableBreakingLogic::WidthVector TableBreakingLogic::widthPerPage( const QVector<int>& colPerPage ) const
{
    Q_ASSERT( colPerPage.size() <= m_pages );
    int startColumn = 0;
    WidthVector widths;
    widths.resize( colPerPage.size() );
    for ( int page = 0; page < colPerPage.size() ; ++page ) {
        const int numColumnsForPage = colPerPage[page];
        qreal pageTotalWidth = 0;
        for ( int i = startColumn; i < startColumn + numColumnsForPage; ++i ) {
            pageTotalWidth += m_widths[i];
        }
        startColumn += numColumnsForPage;
        widths[page] = pageTotalWidth;
    }
    return widths;
}
