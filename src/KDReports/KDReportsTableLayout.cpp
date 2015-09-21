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

#include "KDReportsTableLayout_p.h"
#include "KDReportsLayoutHelper_p.h" // mmToPixels
#include <QDebug>
#include <QFontMetrics>
#include <QAbstractItemModel>

using namespace KDReports;

TableLayout::TableLayout()
    : m_model( 0 ),
      m_cellFont(),
      m_horizontalHeaderFont(),
      m_verticalHeaderFont(),
      m_horizontalHeaderVisible( true ),
      m_verticalHeaderVisible( true ),
      m_cellPadding( KDReports::mmToPixels( 0.5 ) ),
      m_fixedRowHeight( 0 ),
      m_iconSize( 32, 32 ),
      m_rowHeight( 0 ),
      m_vHeaderWidth( 0 ),
      m_hHeaderHeight( 0 ),
      m_cellFontScaler( m_cellFont ),
      m_horizontalHeaderFontScaler( m_horizontalHeaderFont ),
      m_verticalHeaderFontScaler( m_verticalHeaderFont )
{
}

void TableLayout::setInitialFontScalingFactor( qreal fontScalingFactor )
{
    m_cellFontScaler.setFontAndScalingFactor( m_cellFont, fontScalingFactor );
    if ( m_horizontalHeaderVisible ) {
        m_horizontalHeaderFontScaler.setFontAndScalingFactor( m_horizontalHeaderFont, fontScalingFactor );
    }
    if ( m_verticalHeaderVisible ) {
        m_verticalHeaderFontScaler.setFontAndScalingFactor( m_verticalHeaderFont, fontScalingFactor );
    }
    updateRowHeight();
#ifdef DEBUG_LAYOUT
    qDebug() << "initial m_rowHeight=" << m_rowHeight;
#endif
}

void TableLayout::updateRowHeight()
{
    if (m_fixedRowHeight > 0) {
        m_rowHeight = m_fixedRowHeight;
        return;
    }
    m_rowHeight = m_cellFontScaler.fontMetrics().height() + 2.0 * scaledCellPadding();
    if ( m_horizontalHeaderVisible ) {
        m_hHeaderHeight = m_horizontalHeaderFontScaler.fontMetrics().height() + 2.0 * scaledCellPadding();
    }
    if ( m_verticalHeaderVisible ) {
        qreal vHeaderHeight = m_verticalHeaderFontScaler.fontMetrics().height() + 2.0 * scaledCellPadding();
#ifdef DEBUG_LAYOUT
         qDebug() << "cells say rowHeight=" << m_rowHeight << "vHeader says height=" << vHeaderHeight;
#endif
        m_rowHeight = qMax( m_rowHeight, vHeaderHeight );
    }
}

void TableLayout::updateColumnWidths()
{
    if ( !m_model ) {
        return;
    }

    const QFontMetricsF fm = m_cellFontScaler.fontMetrics();
    m_hHeaderHeight = 0;
    const int colCount = m_model->columnCount();
    //qDebug() << "Starting layout of table" << colCount << "columns" << m_model->rowCount() << "rows";
    m_columnWidths.resize( colCount );
    m_widestTextPerColumn.resize( colCount );
    const int rowCount = m_model->rowCount();
    for ( int col = 0; col < colCount; ++col )
    {
        m_columnWidths[col] = 0;
        if ( m_horizontalHeaderVisible ) {
            const QString cellText = m_model->headerData( col, Qt::Horizontal ).toString();
            const qreal textWidth = m_horizontalHeaderFontScaler.fontMetrics().width(cellText);
            const qreal width = addIconWidth( textWidth, m_model->headerData( col, Qt::Horizontal, Qt::DecorationRole ) );
            m_columnWidths[col] = qMax( m_columnWidths[col], width );
            m_widestTextPerColumn[col] = cellText;
            m_hHeaderHeight = qMax( m_hHeaderHeight, m_horizontalHeaderFontScaler.fontMetrics().height() );
        }
        for ( int row = 0; row < rowCount; ++row )
        {
            const QModelIndex index = m_model->index( row, col );
            if ( m_model->span(index).width() > 1 ) {
                // Ignore spanned cells. Not ideal of course, but we'll have to assume
                // the other cells determine width, and this one just has to fit in.
                // I guess a two-pass algorithm is needed otherwise, checking every spanned cell
                // after the initial column width distribution? Urgh.
                continue;
            }
            qreal width;
            const QString cellText = m_model->data( index, Qt::DisplayRole ).toString();
            const QSizeF cellSize = m_model->data( index, Qt::SizeHintRole ).toSizeF();
            if (cellSize.isValid()) {
                width = mmToPixels(cellSize.width());
            } else {
                const qreal textWidth = fm.size(0 /*flags*/, cellText).width();
                width = addIconWidth( textWidth, m_model->data( index, Qt::DecorationRole ) );
            }
            if (width > m_columnWidths[col]) {
                m_columnWidths[col] = width;
                m_widestTextPerColumn[col] = cellText;
            }
        }
        //qDebug() << "Column" << col << "width" << m_columnWidths[col] << "+padding=" << m_columnWidths[col]+2*scaledCellPadding();
        m_columnWidths[col] += 2 * scaledCellPadding();
    }

    m_vHeaderWidth = 0;
    if ( m_verticalHeaderVisible ) {
        for ( int row = 0; row < rowCount; ++row )
        {
            const QString cellText = m_model->headerData( row, Qt::Vertical ).toString();
            const qreal textWidth = m_verticalHeaderFontScaler.fontMetrics().width(cellText);
            const qreal width = addIconWidth( textWidth, m_model->headerData( row, Qt::Vertical, Qt::DecorationRole ) );
            m_vHeaderWidth = qMax( m_vHeaderWidth, width );
        }
        m_vHeaderWidth += 2 * scaledCellPadding();
#ifdef DEBUG_LAYOUT
        //qDebug() << "m_vHeaderWidth=" << m_vHeaderWidth;
#endif
    }
    if ( m_horizontalHeaderVisible ) {
        m_hHeaderHeight += 2 * scaledCellPadding();
    }
}

#if 0
void TableLayout::updateColumnWidthsByFactor( qreal factor )
{
    const int colCount = m_model->columnCount();
    for ( int col = 0; col < colCount; ++col )
    {
        m_columnWidths[col] *= factor;
    }
    m_vHeaderWidth *= factor;
#ifdef DEBUG_LAYOUT
    qDebug() << "updateColumnWidthsByFactor: after factor" << factor << "m_vHeaderWidth=" << m_vHeaderWidth;
#endif

#ifdef DEBUG_LAYOUT
    qDebug() << "final scaling factors:" << m_cellFontScaler.scalingFactor()
             << m_horizontalHeaderFontScaler.scalingFactor()
             << m_verticalHeaderFontScaler.scalingFactor();
#endif
}
#endif

void TableLayout::ensureScalingFactorForWidth( qreal factor )
{
#ifdef DEBUG_LAYOUT
    qDebug() << "TableLayout::ensureScalingFactorForWidth" << factor;
#endif
    // ## fonts are definitely not proportional. QFontMetricsF::width for "Hello world"
    // jumps from 40 to 32 just by changing the font size from 6.6 to 6.3...
    // So instead of saying "applying this scaling factor", we should iterate over all
    // columns, and resize the font down until the column has the desired width, then choose
    // the min font size over all columns? Well, this might add up rounding problems, but
    // at least it will always fit...

    const int colCount = m_model->columnCount();
    QString textForScaling;
    for ( int col = 0; col < colCount; ++col ) {
        // Which column should we use as 'reference' for the scaling calculation?
        // The widest or the narrowest one? Chose narrowest, more rounding problems there.
        if (col == 0 || m_widestTextPerColumn[col].length() < textForScaling.length())
            textForScaling = m_widestTextPerColumn[col];
    }

    m_cellFontScaler.setFactorForWidth( factor, textForScaling );
    m_horizontalHeaderFontScaler.setFactorForWidth( factor, textForScaling );
    m_verticalHeaderFontScaler.setFactorForWidth( factor, textForScaling );
    updateRowHeight();
}

void TableLayout::ensureScalingFactorForHeight( qreal maxRowHeight )
{
    const qreal wantedRowHeightFactor = maxRowHeight / m_rowHeight;
    // Apply _final_ padding when determining the wanted text height, obviously.
    // Testcase with numbers: initial: 6+2*2=10. Wanted row height 5, so wanted factor 0.5
    //                        so wanted text height = 5 - 2*1 = 3.
    const qreal wantedTextHeight = maxRowHeight - 2.0 * wantedRowHeightFactor * scaledCellPadding();
#ifdef DEBUG_LAYOUT
    qDebug() << " ensureScalingFactorForHeight: wantedRowHeightFactor=" << wantedRowHeightFactor << "wantedTextHeight=" << wantedTextHeight << "after removing padding ( unscaled" << m_cellPadding << "current scaling" << scaledCellPadding() << "wanted" << wantedRowHeightFactor << "*" << m_cellPadding << "*" << scalingFactor() << "=" << wantedRowHeightFactor*scaledCellPadding() << ")";
#endif
    qreal additionalFactor = 0;

    // Let's see if the height is constrained by the cell font or by the vHeader font,
    // that's the one that has to determine the scaling factor
    if ( m_cellFontScaler.fontMetrics().height() >= m_verticalHeaderFontScaler.fontMetrics().height() ) {
#ifdef DEBUG_LAYOUT
        qDebug() << " Reducing the cell font to be wantedTextHeight=" << wantedTextHeight;
#endif
        const qreal initialCellFactor = m_cellFontScaler.scalingFactor();
        m_cellFontScaler.setFactorForHeight( wantedTextHeight );
        additionalFactor = m_cellFontScaler.scalingFactor() / initialCellFactor;
#ifdef DEBUG_LAYOUT
        //qDebug() << " applying factor to m_verticalHeaderFontScaler";
#endif
        m_verticalHeaderFontScaler.applyAdditionalScalingFactor( additionalFactor );
    } else {
        // bigger font in the vertical header
#ifdef DEBUG_LAYOUT
        qDebug() << "Reducing the vertical header font to be wantedTextHeight=" << wantedTextHeight;
#endif
        const qreal initialVerticHeaderFactor = m_verticalHeaderFontScaler.scalingFactor();
        m_verticalHeaderFontScaler.setFactorForHeight( wantedTextHeight );
        additionalFactor = m_verticalHeaderFontScaler.scalingFactor() / initialVerticHeaderFactor;
#ifdef DEBUG_LAYOUT
        //qDebug() << " applying factor to m_cellFontScaler";
#endif
        m_cellFontScaler.applyAdditionalScalingFactor( additionalFactor );
    }

#ifdef DEBUG_LAYOUT
    //qDebug() << " Now we have cellFontScaler:" << m_cellFontScaler.fontMetrics().height()
    //        << "verticalheaderfontscaler:" << m_verticalHeaderFontScaler.fontMetrics().height();
#endif

    m_horizontalHeaderFontScaler.applyAdditionalScalingFactor( additionalFactor );
    updateRowHeight();
    // With very small fonts, we can't get less than 3 pixels high for the text.
    m_rowHeight = qMin(maxRowHeight, m_rowHeight);

#ifdef DEBUG_LAYOUT
    qDebug() << " ensureScalingFactorForHeight: applied additional factor" << additionalFactor << "row height is now" << m_rowHeight;
#endif
}

QSize KDReports::TableLayout::decorationSize( const QVariant& cellDecoration ) const
{
    QImage img = qvariant_cast<QImage>( cellDecoration );
    if ( !img.isNull() ) {
        return img.size();
    }
    QPixmap pix = qvariant_cast<QPixmap>( cellDecoration );
    if ( !pix.isNull() ) {
        return pix.size();
    }
    return m_iconSize;
}

qreal KDReports::TableLayout::addIconWidth( qreal textWidth, const QVariant& cellDecoration ) const
{
    qreal width = textWidth;
    if ( !cellDecoration.isNull() )
        width += decorationSize( cellDecoration ).width() + 2 /*see textRect adjustments in SpreadsheetReportLayout::paintPageContent*/;
    return width;
}
