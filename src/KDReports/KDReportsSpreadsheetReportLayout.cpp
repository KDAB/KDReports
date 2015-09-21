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

#include "KDReportsSpreadsheetReportLayout_p.h"
#include "KDReportsTableBreakingLogic_p.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsLayoutHelper_p.h"
#include <QPainter>
#include <QAbstractItemModel>
#include <QDebug>
#include <QIcon>
#include <QBitArray>
#include <qmath.h> // qCeil

KDReports::SpreadsheetReportLayout::SpreadsheetReportLayout(KDReports::Report *report)
    : m_tableBreakingPageOrder( Report::DownThenRight ),
      m_numHorizontalPages( 1 ),
      m_numVerticalPages( 0 ),
      m_layoutDirty( true ),
      m_userRequestedFontScalingFactor( 1.0 ),
      m_tableSettings()

{
    Q_UNUSED(report); // for later
}

void KDReports::SpreadsheetReportLayout::setLayoutDirty()
{
    m_layoutDirty = true;
}

void KDReports::SpreadsheetReportLayout::setDefaultFont(const QFont &font)
{
    m_tableLayout.m_cellFont = font;
    m_tableLayout.m_horizontalHeaderFont = font;
    m_tableLayout.m_verticalHeaderFont = font;
    setLayoutDirty();
}

QFont KDReports::SpreadsheetReportLayout::defaultFont() const
{
    return m_tableLayout.m_cellFont;
}

qreal KDReports::SpreadsheetReportLayout::paintTableVerticalHeader( qreal x, qreal y, QPainter& painter, int row )
{
    QAbstractItemModel* model = m_tableLayout.m_model;

    const QRectF cellRect( x, y, m_tableLayout.vHeaderWidth(), m_tableLayout.rowHeight() );

    painter.setFont( m_tableLayout.verticalHeaderScaledFont() );
    painter.fillRect( cellRect, m_tableSettings.m_headerBackground );
    drawBorder( cellRect, painter );

    const QColor foreground = qvariant_cast<QColor>( model->headerData( row, Qt::Vertical, Qt::ForegroundRole ) );
    if ( foreground.isValid() )
        painter.setPen( foreground );

    const QString cellText = model->headerData( row, Qt::Vertical ).toString();
    const qreal padding = m_tableLayout.scaledCellPadding();
    const Qt::Alignment alignment( model->headerData( row, Qt::Vertical, Qt::TextAlignmentRole ).toInt() );
    const QVariant cellDecoration = model->headerData( row, Qt::Vertical, Qt::DecorationRole );
    const QVariant decorationAlignment = model->headerData( row, Qt::Vertical, KDReports::AutoTableElement::DecorationAlignmentRole );

    const QRectF cellContentsRect = cellRect.adjusted( padding, padding, -padding, -padding );
    //painter.drawText( cellContentsRect, alignment, cellText );
    paintTextAndIcon( painter, cellContentsRect, cellText, cellDecoration, decorationAlignment, alignment );

    if ( foreground.isValid() )
        painter.setPen( Qt::black );

    x += cellRect.width();
    return x;
}

void KDReports::SpreadsheetReportLayout::paintTableHorizontalHeader( const QRectF& cellRect, QPainter& painter, int col )
{
    QAbstractItemModel* model = m_tableLayout.m_model;

    painter.setFont( m_tableLayout.horizontalHeaderScaledFont() );
    // adjust rect so that it's really under the lines; we get some white space otherwise, sometimes
    painter.fillRect( cellRect.adjusted( -0.5, -0.5, -0.5, -0.5 ), m_tableSettings.m_headerBackground );
    drawBorder( cellRect, painter );

    const QColor foreground = qvariant_cast<QColor>( model->headerData( col, Qt::Horizontal, Qt::ForegroundRole ) );
    if ( foreground.isValid() )
        painter.setPen( foreground );

    const QString cellText = model->headerData( col, Qt::Horizontal ).toString();
    const qreal padding = m_tableLayout.scaledCellPadding();
    const Qt::Alignment alignment( model->headerData( col, Qt::Horizontal, Qt::TextAlignmentRole ).toInt() );
    const QVariant cellDecoration = model->headerData( col, Qt::Horizontal, Qt::DecorationRole );
    const QVariant decorationAlignment = model->headerData( col, Qt::Horizontal, KDReports::AutoTableElement::DecorationAlignmentRole );

    const QRectF cellContentsRect = cellRect.adjusted( padding, padding, -padding, -padding );
    //painter.drawText( cellContentsRect, alignment, cellText );
    paintTextAndIcon( painter, cellContentsRect, cellText, cellDecoration, decorationAlignment, alignment );

    if ( foreground.isValid() )
        painter.setPen( Qt::black );
}

void KDReports::SpreadsheetReportLayout::paintIcon( QPainter& painter, const QRectF& cellContentsRect, const QVariant& cellDecoration ) const
{
    QPixmap pix = qvariant_cast<QPixmap>( cellDecoration );
    if ( pix.isNull() ) {
        pix = qvariant_cast<QIcon>( cellDecoration ).pixmap( m_tableLayout.m_iconSize );
    }
    qreal height = pix.height();
    QImage img;
    if ( pix.isNull() ) {
        img = qvariant_cast<QImage>( cellDecoration );
        if ( img.isNull() )
            return;
        height = img.height();
    }

    // Now either img or pix is set.

    // Apply scaling factor
    if ( m_tableLayout.scalingFactor() != 1. ) {
        if ( !pix.isNull() ) {
            pix = pix.scaledToWidth( pix.width() * m_tableLayout.scalingFactor() );
            height = pix.height();
        } else {
            img = img.scaledToWidth( img.width() * m_tableLayout.scalingFactor() );
            height = img.height();
        }
    }

    // Vertical centering
    const qreal y = qMax( qreal( 0 ), qreal( ( cellContentsRect.height() - height ) / 2.0 ) );
    if ( !img.isNull() ) {
        // Draw img
        painter.drawImage( cellContentsRect.x(), cellContentsRect.y() + y, img );
    } else {
        // Draw pix
        painter.drawPixmap( cellContentsRect.x(), cellContentsRect.y() + y, pix );
    }
}

// We could use QItemDelegate::paint instead, but it does so much more, it looks slow.
void KDReports::SpreadsheetReportLayout::paintTextAndIcon( QPainter& painter, const QRectF& cellContentsRect, const QString& cellText, const QVariant& cellDecoration, const QVariant& decorationAlignment, Qt::Alignment alignment )
{
    QRectF textRect = cellContentsRect;

    const bool hasIcon = !cellDecoration.isNull();
    int iconWidth = 0;
    const bool iconAfterText = decorationAlignment.isValid() && ( decorationAlignment.toInt() & Qt::AlignRight );
    if ( hasIcon ) {
        iconWidth = m_tableLayout.decorationSize( cellDecoration ).width() * m_tableLayout.scalingFactor();
        //qDebug() << "row" << row << "hasIcon=" << hasIcon << "iconAfterText=" << iconAfterText << "iconWidth=" << iconWidth;
        if ( !iconAfterText ) {
            paintIcon( painter, cellContentsRect, cellDecoration );
            textRect.setLeft( textRect.left() + iconWidth + 2 );
        } else {
            textRect.setWidth( textRect.width() - iconWidth - 2 );
        }
    }

    //qDebug() << "Drawing text in" << textRect;
    painter.drawText( textRect, alignment, cellText );

    if ( hasIcon && iconAfterText ) {
        QRectF iconRect = cellContentsRect;
        iconRect.setLeft( iconRect.right() - iconWidth );
        //qDebug() << "iconRect=" << iconRect;
        paintIcon( painter, iconRect, cellDecoration );
    }
}


void KDReports::SpreadsheetReportLayout::drawBorder( const QRectF cellRect, QPainter &painter )
{
    if ( m_tableSettings.m_border > 0 ) {
        const QPen oldPen = painter.pen();
        // QTextDocumentLayoutPrivate::drawBorder draws 4 rects using a brush
        // But that's more work, and it doesn't always look good anyway.
        // So we just use it as a color for now.
        painter.setPen( QPen( m_tableSettings.m_borderBrush.color(), m_tableSettings.m_border ) );
        painter.drawRect( cellRect );
        painter.setPen( oldPen );
    }
}

void KDReports::SpreadsheetReportLayout::paintPageContent(int pageNumber, QPainter &painter)
{
    //qDebug() << "painting with" << m_tableLayout.scaledFont();
    QAbstractItemModel* model = m_tableLayout.m_model;
    const qreal padding = m_tableLayout.scaledCellPadding();
    const QRect cellCoords = m_pageRects[pageNumber];
    //qDebug() << "painting page" << pageNumber << "cellCoords=" << cellCoords;
    qreal y = 0 /*m_topMargin*/; // in pixels
    const qreal rowHeight = m_tableLayout.rowHeight();

    if ( m_tableLayout.m_horizontalHeaderVisible ) {
        qreal x = 0 /*m_leftMargin*/;
        if ( m_tableLayout.m_verticalHeaderVisible ) {
            x += m_tableLayout.vHeaderWidth();
        }
        for ( int col = cellCoords.left(); col <= cellCoords.right(); ++col )
        {
            const QRectF cellRect( x, y, m_tableLayout.m_columnWidths[ col ], m_tableLayout.hHeaderHeight() );
            paintTableHorizontalHeader( cellRect, painter, col );
            x += cellRect.width();
        }
        y += m_tableLayout.hHeaderHeight();
    }

    const int firstRow = cellCoords.top();
    const int firstColumn = cellCoords.left();
    const int numRows = cellCoords.height();
    const int numColumns = cellCoords.width();

    // This won't work across page breaks....
    QVector<QBitArray> coveredCells;
    coveredCells.resize( numRows );
    for ( int row = firstRow; row <= cellCoords.bottom(); ++row )
        coveredCells[row - firstRow].resize( numColumns );

    for ( int row = firstRow; row <= cellCoords.bottom(); ++row )
    {
        qreal x = 0 /*m_leftMargin*/;
        if ( m_tableLayout.m_verticalHeaderVisible ) {
            x = paintTableVerticalHeader( x, y, painter, row );
        }
        painter.setFont( m_tableLayout.scaledFont() );
        for ( int col = cellCoords.left(); col <= cellCoords.right(); ++col )
        {
            if (coveredCells[row - firstRow].testBit(col - firstColumn)) {
                x += m_tableLayout.m_columnWidths[ col ];
                continue;
            }

            const QModelIndex index = model->index( row, col );

            const QSize span = model->span( index );
            if (span.isValid()) {
                for (int r = row; r < row + span.height() && r < numRows; ++r) {
                    for (int c = col; c < col + span.width() && c < numColumns; ++c) {
                        coveredCells[r - firstRow].setBit(c - firstColumn);
                    }
                }
            }

            const QRectF cellRect( x, y, cellWidth( col, span.width() ), qMax(1, span.height()) * rowHeight );
            const QRectF cellContentsRect = cellRect.adjusted( padding, padding, -padding, -padding );
            //qDebug() << "cell" << row << col << "rect=" << cellRect;

            const QString cellText = model->data( index, Qt::DisplayRole ).toString();
            const QColor foreground = qvariant_cast<QColor>( model->data( index, Qt::ForegroundRole ) );
            const QColor background = qvariant_cast<QColor>( model->data( index, Qt::BackgroundRole ) );
            const Qt::Alignment alignment( model->data( index, Qt::TextAlignmentRole ).toInt() );
            const QVariant decorationAlignment( model->data( index, KDReports::AutoTableElement::DecorationAlignmentRole ) );
            const QVariant cellDecoration( model->data( index, Qt::DecorationRole ) );

            if ( background.isValid() ) {
                painter.fillRect( cellRect, QBrush( background ) );
            } else if ( span.isValid() ) {
                painter.fillRect( cellRect, Qt::white );
            }
            drawBorder(cellRect, painter);

            // Per-cell font is not supported, on purpose. All rows use the same font,
            // otherwise the calculations for making things fit into a number of pages
            // become quite complex and slow.
            //const QVariant cellFont = model->data( index, Qt::FontRole );
            //if ( cellFont.isValid() )
            //    painter.setFont( qvariant_cast<QFont>( cellFont ) );
            //else
            //    painter.setFont( scaledFont );

            if ( foreground.isValid() )
                painter.setPen( foreground );

            paintTextAndIcon( painter, cellContentsRect, cellText, cellDecoration, decorationAlignment, alignment );

            if ( foreground.isValid() )
                painter.setPen( Qt::black );

            x += m_tableLayout.m_columnWidths[ col ];
        }
        y += rowHeight;
    }
}

int KDReports::SpreadsheetReportLayout::numberOfPages()
{
    ensureLayouted();
    return m_pageRects.count();
}

qreal KDReports::SpreadsheetReportLayout::idealWidth()
{
    m_tableLayout.setInitialFontScalingFactor( m_userRequestedFontScalingFactor );
    QAbstractItemModel* model = m_tableLayout.m_model;
    if ( !model )
        return -1;
    m_tableLayout.updateColumnWidths();
    const qreal total = totalWidth();
#ifdef DEBUG_LAYOUT
    qDebug() << "idealWidth:" << total << "pixels";
#endif
    return total;
}

qreal KDReports::SpreadsheetReportLayout::totalWidth() const
{
    qreal totalWidth = 0;
    for ( int col = 0; col < m_tableLayout.m_columnWidths.size(); ++col) {
        totalWidth += m_tableLayout.m_columnWidths[col];
    }
    if (m_tableLayout.m_verticalHeaderVisible)
        totalWidth += m_tableLayout.vHeaderWidth();
    return totalWidth;
}

qreal KDReports::SpreadsheetReportLayout::cellWidth(int col, int horizSpan) const
{
    qreal cellWidth = m_tableLayout.m_columnWidths[ col ];
    for ( int extraCol = 1; extraCol < horizSpan; ++extraCol ) {
        cellWidth += m_tableLayout.m_columnWidths[ col + extraCol ];
    }
    return cellWidth;
}

void KDReports::SpreadsheetReportLayout::setPageContentSize(const QSizeF &size)
{
    m_pageContentSize = size;
#ifdef DEBUG_LAYOUT
    qDebug() << "page content size" << m_pageContentSize;
#endif
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::ensureLayouted()
{
    if ( !m_layoutDirty )
        return;
    if ( m_pageContentSize.isEmpty() ) {
        qWarning( "No paper size specified!" );
        return;
    }

    m_tableLayout.setInitialFontScalingFactor( m_userRequestedFontScalingFactor );
    m_pageRects.clear();

    QAbstractItemModel* model = m_tableLayout.m_model;
    if ( !model )
        return;

    // Here's the whole layouting logic

    // Step 1: determine "ideal" column widths, based on contents

    m_tableLayout.updateColumnWidths();

    // Step 2: based on that and the number of horiz pages wanted,
    //         determine actual column widths (horizontal table breaking)

    KDReports::TableBreakingLogic optimizer;
    optimizer.setColumnWidths( m_tableLayout.m_columnWidths );
    optimizer.setPageCount( m_numHorizontalPages );
    const QVector<int> columnsPerPage = optimizer.columnsPerPage();
    QVector<qreal> widthPerPage = optimizer.widthPerPage( columnsPerPage );
    const int horizPages = columnsPerPage.count();
    bool scaled = false;

    // Step 3: check everything fits horizontally, otherwise calculate font scaling factor for this

    const qreal horizMargins = 0 /*m_leftMargin*/ + 0 /*m_rightMargin*/;
    const qreal verticalMargins = 0 /*m_topMargin*/ + 0 /*m_bottomMargin*/;
    const qreal usablePageWidth = m_pageContentSize.width() - horizMargins;
    const qreal usablePageHeight = m_pageContentSize.height() - verticalMargins - m_tableLayout.hHeaderHeight();

#ifdef DEBUG_LAYOUT
    qDebug() << "usablePageHeight=" << m_pageContentSize.height() << "minus hHeaderHeight" << m_tableLayout.hHeaderHeight();
#endif

    // for each page, if (sum of column widths) > usablePageWidth,
    // then we need to scale everything (font and padding)
    // by the ratio of those two numbers.
    qreal bestScalingFactor = 1000000;
    for ( int page = 0; page < horizPages; ++page ) {
        const qreal width = widthPerPage[page] + m_tableLayout.vHeaderWidth();
        if ( width > usablePageWidth ) {
            const qreal scalingFactor = usablePageWidth / width;
#ifdef DEBUG_LAYOUT
            qDebug() << "page" << page << "sum of column widths:" << width
                     << "usablePageWidth=" << usablePageWidth;
            qDebug() << "scaling factor so that it fits horizontally:" << scalingFactor;
#endif
            bestScalingFactor = qMin(bestScalingFactor, scalingFactor);
            scaled = true;
        }
    }

    if (scaled) {
        m_tableLayout.ensureScalingFactorForWidth( bestScalingFactor );
    }

    // Step 4: check everything fits vertically, otherwise calculate font scaling factor for this

    const int rowCount = m_tableLayout.m_model->rowCount();
    if ( m_numVerticalPages > 0 ) {
        const qreal rowHeight = m_tableLayout.rowHeight();

        // We can't do a global division of heights, it assumes rows can be over page borders, partially truncated
        // const qreal maxTotalHeight = m_numVerticalPages  * usablePageHeight;
        // const qreal maxRowHeight = maxTotalHeight / rowCount;

        // Example: 5 rows over 2 pages, and the usablePageHeight is 100. What do you do?
        // 2.5 rows per page means truncation. 2 rows per page (as in the division above) is not enough.
        // The right solution is qCeil, i.e. max 3 rows per page, and maxRowHeight = 100 / 3 = 33.3.

        const int maxRowsPerPage = qCeil( static_cast<qreal>(rowCount) / m_numVerticalPages );
        const qreal maxRowHeight = usablePageHeight / maxRowsPerPage;
#ifdef DEBUG_LAYOUT
        qDebug() << "usablePageHeight=" << usablePageHeight
                 << "rowHeight=" << rowHeight
                 << "maxRowsPerPage=" << maxRowsPerPage
                 << "maxRowHeight=" << usablePageHeight << "/" << maxRowsPerPage << "=" << maxRowHeight;
#endif
        if ( rowHeight > maxRowHeight ) { // more than authorized maximum
            m_tableLayout.ensureScalingFactorForHeight( maxRowHeight );
            scaled = true;
        }
    }

    // Step 5: update font and calculations based on final font scaling
    if (scaled) {
#ifdef DEBUG_LAYOUT
        qDebug() << "final scaling factor" << m_tableLayout.scalingFactor();
        qDebug() << "final fonts: cells:" << m_tableLayout.scaledFont().pointSizeF()
                << "hHeader:" << m_tableLayout.horizontalHeaderScaledFont().pointSizeF()
                << "vHeader:" << m_tableLayout.verticalHeaderScaledFont().pointSizeF();
#endif
        // With this new scaling factor [when step 4 changed the factor], what should be the column widths?
        // If we just call
        // m_tableLayout.updateColumnWidthsByFactor( m_tableLayout.scalingFactor() / m_userRequestedFontScalingFactor );
        // then we risk truncating column text (because fonts are not proportional).
        // Testcase: LongReport with font size 8, padding 3, 10 columns, 300 rows, and scaleTo(1,10) (or none);
        m_tableLayout.updateColumnWidths();

#ifdef DEBUG_LAYOUT
        qDebug() << "New total width:" << totalWidth();
#endif

#if 0 // not used right now, but could be useful, especially if we want to goto step 3 again, to resize down
        // Update the widthPerPage array
        int column = 0;
        for ( int page = 0; page < horizPages; ++page ) {
            const int numColumnsInPage = columnsPerPage[page];
            widthPerPage[page] = 0;
            for ( int col = column; col < column + numColumnsInPage; ++col) {
                widthPerPage[page] += m_tableLayout.m_columnWidths[col];
            }

            const qreal width = widthPerPage[page] + m_tableLayout.vHeaderWidth();
            if ( width > usablePageWidth ) {
                qWarning() << "Too much width on page" << page;
            }

            column += numColumnsInPage;
        }
        qDebug() << "widthPerPage:" << widthPerPage;
#endif
    }

    const qreal rowHeight = m_tableLayout.rowHeight(); // do it now so that the scaling is included

    // Step 6: determine number of pages for all rows to fit

    const int maxRowsPerPage = qFloor(usablePageHeight / rowHeight); // no qCeil here, the last row would be truncated...
    int verticPages = qCeil( qreal( rowCount ) / qreal( maxRowsPerPage ) );

#ifdef DEBUG_LAYOUT
    qDebug() << "maxRowsPerPage=" << usablePageHeight << "/" << rowHeight << "=" <<  maxRowsPerPage;
    qDebug() << "pages:" << horizPages << "x" << verticPages;
    qDebug() << "verticPages = qCeil(" << rowCount << "/" << maxRowsPerPage << ") =" << verticPages;
#endif

    // avoid rounding problems (or the font not zooming down enough vertically),
    // obey m_numVerticalPages in all cases
    if ( m_numVerticalPages > 0 ) {
        Q_ASSERT( verticPages <= m_numVerticalPages );
        //    verticPages = qMin( m_numVerticalPages, verticPages );
    }

    // Step 7: now we can record all this in terms of cell areas

    if ( m_tableBreakingPageOrder == Report::RightThenDown ) {
        //qDebug() << "Doing right then down layout";
        int row = 0;
        for ( int y = 0; y < verticPages; ++y ) {
            int column = 0;
            const int numRowsInPage = qMin( maxRowsPerPage, rowCount - row );
            for ( int x = 0; x < horizPages; ++x ) {
                const int numColumnsInPage = columnsPerPage[x];
                m_pageRects.append( QRect( column, row, numColumnsInPage, numRowsInPage ) );
                column += numColumnsInPage;
            }
            row += maxRowsPerPage;
        }
    } else {
        //qDebug() << "Doing down then right layout";
        int column = 0;
        for ( int x = 0; x < horizPages; ++x ) {
            int row = 0;
            const int numColumnsInPage = columnsPerPage[x];
            for ( int y = 0; y < verticPages; ++y ) {
                const int numRowsInPage = qMin( maxRowsPerPage, rowCount - row );
                m_pageRects.append( QRect( column, row, numColumnsInPage, numRowsInPage ) );
                row += maxRowsPerPage;
            }
            column += numColumnsInPage;
        }
    }

    m_layoutDirty = false;
}

void KDReports::SpreadsheetReportLayout::updateTextValue(const QString &id, const QString &newValue)
{
    // Not implemented, there is no support for this in spreadsheet mode currently.
    Q_UNUSED(id);
    Q_UNUSED(newValue);
}

qreal KDReports::SpreadsheetReportLayout::layoutAsOnePage(qreal width)
{
    m_tableLayout.setInitialFontScalingFactor( m_userRequestedFontScalingFactor );
    const int rowCount = m_tableLayout.m_model->rowCount();
    const qreal usableTotalHeight = m_tableLayout.rowHeight() * rowCount;
    const qreal pageContentHeight = usableTotalHeight + 0 /*verticalMargins*/ + m_tableLayout.hHeaderHeight();

    m_pageContentSize = QSizeF(width, pageContentHeight);
#ifdef DEBUG_LAYOUT
    qDebug() << "layoutAsOnePage: page content size" << m_pageContentSize << rowCount << "*" << m_tableLayout.rowHeight() << "+" << m_tableLayout.hHeaderHeight();
#endif
    setLayoutDirty();
    return pageContentHeight;
}

QString KDReports::SpreadsheetReportLayout::toHtml() const
{
    return QString::fromLatin1("Not implemented");
}

void KDReports::SpreadsheetReportLayout::finishHtmlExport()
{
}

bool KDReports::SpreadsheetReportLayout::scaleTo(int numPagesHorizontally, int numPagesVertically)
{
    m_numHorizontalPages = numPagesHorizontally;
    m_numVerticalPages = numPagesVertically;
    setLayoutDirty();
    return true;
}

void KDReports::SpreadsheetReportLayout::setFixedRowHeight(qreal height)
{
    m_tableLayout.m_fixedRowHeight = height;
}

int KDReports::SpreadsheetReportLayout::maximumNumberOfPagesForHorizontalScaling() const
{
    return m_numHorizontalPages;
}

int KDReports::SpreadsheetReportLayout::maximumNumberOfPagesForVerticalScaling() const
{
    return m_numVerticalPages;
}

void KDReports::SpreadsheetReportLayout::setUserRequestedFontScalingFactor(qreal factor)
{
    m_userRequestedFontScalingFactor = factor;
    m_numHorizontalPages = 1;
    m_numVerticalPages = 0;
    setLayoutDirty();
}

qreal KDReports::SpreadsheetReportLayout::userRequestedFontScalingFactor() const
{
    return m_userRequestedFontScalingFactor;
}

void KDReports::SpreadsheetReportLayout::setTableBreakingPageOrder(KDReports::Report::TableBreakingPageOrder order)
{
    m_tableBreakingPageOrder = order;
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::setHorizontalHeaderFont(const QFont &font)
{
    m_tableLayout.m_horizontalHeaderFont = font;
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::setVerticalHeaderFont(const QFont &font)
{
    m_tableLayout.m_verticalHeaderFont = font;
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::setModel(QAbstractItemModel *model)
{
    m_tableLayout.m_model = model;
}

void KDReports::SpreadsheetReportLayout::setVerticalHeaderVisible(bool visible)
{
    m_tableLayout.m_verticalHeaderVisible = visible;
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::setHorizontalHeaderVisible(bool visible)
{
    m_tableLayout.m_horizontalHeaderVisible = visible;
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::setCellPadding(qreal padding)
{
    m_tableLayout.m_cellPadding = KDReports::mmToPixels(padding);
    //qDebug() << "setting cellpadding: mm=" << padding << "pixels=" << m_tableLayout.m_cellPadding;
    setLayoutDirty();
}

void KDReports::SpreadsheetReportLayout::setIconSize(const QSize& iconSize)
{
    m_tableLayout.m_iconSize = iconSize;
}

void KDReports::SpreadsheetReportLayout::setCellBorder(qreal border, const QBrush &borderBrush)
{
    m_tableSettings.m_border = border;
    m_tableSettings.m_borderBrush = borderBrush;
}

void KDReports::SpreadsheetReportLayout::setHeaderBackground(const QBrush &headerBackground)
{
    m_tableSettings.m_headerBackground = headerBackground;
}
