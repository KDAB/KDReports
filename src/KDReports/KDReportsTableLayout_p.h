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

#ifndef KDREPORTSTABLELAYOUT_H
#define KDREPORTSTABLELAYOUT_H

#include "KDReportsFontScaler_p.h"
#include <QVector>
#include <QFont>
class QAbstractItemModel;

namespace KDReports {

class TableLayout
{
public:
    TableLayout();

    void setInitialFontScalingFactor( qreal fontScalingFactor );

    // Determine "ideal" column widths, based on contents
    void updateColumnWidths();
    // Return row height (determined during call to columnWidths), padding included
    qreal rowHeight() const { return m_rowHeight; }
    // Return the width of the vertical header, 0 if not shown
    qreal vHeaderWidth() const { return m_vHeaderWidth; }
    // Return the height of the horizontal header, 0 if not shown
    qreal hHeaderHeight() const { return m_hHeaderHeight; }

    //void updateColumnWidthsByFactor( qreal factor );

    QFont scaledFont() const { return m_cellFontScaler.font(); }
    QFont horizontalHeaderScaledFont() const { return m_horizontalHeaderFontScaler.font(); }
    QFont verticalHeaderScaledFont() const { return m_verticalHeaderFontScaler.font(); }

    //QFontMetricsF scaledFontMetrics() const { return m_cellFontScaler.fontMetrics(); }
    qreal scalingFactor() const { return m_cellFontScaler.scalingFactor(); }
    void ensureScalingFactorForWidth( qreal scalingFactor );
    void ensureScalingFactorForHeight( qreal maxRowHeight );
    qreal scaledCellPadding() const { return m_cellPadding * scalingFactor(); }

    QSize decorationSize( const QVariant& cellDecoration ) const;

    QAbstractItemModel* m_model;

    QFont m_cellFont;
    QFont m_horizontalHeaderFont;
    QFont m_verticalHeaderFont;

    bool m_horizontalHeaderVisible;
    bool m_verticalHeaderVisible;

    // All measurements below are in pixels
    qreal m_cellPadding;
    QVector<qreal> m_columnWidths;
    qreal m_fixedRowHeight;

    QVector<QString> m_widestTextPerColumn;

    QSize m_iconSize;

private:
    qreal addIconWidth( qreal textWidth, const QVariant& cellDecoration ) const;
    void updateRowHeight();

    qreal m_rowHeight;
    qreal m_vHeaderWidth;
    qreal m_hHeaderHeight;

    FontScaler m_cellFontScaler;
    FontScaler m_horizontalHeaderFontScaler;
    FontScaler m_verticalHeaderFontScaler;
};

}

#endif /* KDREPORTSTABLELAYOUT_H */
