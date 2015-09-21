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

#ifndef KDREPORTSSPREADSHEETREPORTLAYOUT_H
#define KDREPORTSSPREADSHEETREPORTLAYOUT_H

#include "KDReportsAbstractReportLayout_p.h"
#include "KDReportsTableLayout_p.h"
#include "KDReportsReport.h"
#include <QBrush>

namespace KDReports
{
class Report;
class MainTable;

class SpreadsheetReportLayout : public AbstractReportLayout
{
public:
    explicit SpreadsheetReportLayout(KDReports::Report* report);

    /// \reimp
    virtual void setLayoutDirty();
    /// \reimp
    virtual void setDefaultFont(const QFont& font);
    /// \reimp
    virtual QFont defaultFont() const;
    /// \reimp
    virtual void paintPageContent(int pageNumber, QPainter& painter);
    /// \reimp
    virtual int numberOfPages(); // not const, since it can trigger a layout
    /// \reimp
    virtual qreal idealWidth();
    /// \reimp
    virtual void setPageContentSize(const QSizeF& size);
    /// \reimp
    virtual void ensureLayouted();
    /// \reimp
    virtual void updateTextValue( const QString& id, const QString& newValue );
    /// \reimp
    virtual qreal layoutAsOnePage(qreal width);
    /// \reimp
    virtual bool scaleTo( int numPagesHorizontally, int numPagesVertically );
    /// \reimp
    virtual void setFixedRowHeight(qreal height);
    /// \reimp
    virtual int maximumNumberOfPagesForHorizontalScaling() const;
    /// \reimp
    virtual int maximumNumberOfPagesForVerticalScaling() const;
    /// \reimp
    virtual void setUserRequestedFontScalingFactor(qreal factor);
    /// \reimp
    virtual qreal userRequestedFontScalingFactor() const;
    /// \reimp
    virtual QString toHtml() const;
    /// \reimp
    virtual void finishHtmlExport();

    void setModel(QAbstractItemModel* model);
    void setVerticalHeaderVisible(bool visible);
    void setHorizontalHeaderVisible(bool visible);
    void setCellPadding(qreal padding);
    void setIconSize(const QSize& iconSize);
    void setCellBorder(qreal border, const QBrush &borderBrush);
    void setHeaderBackground(const QBrush &headerBackground);

    KDReports::Report::TableBreakingPageOrder tableBreakingPageOrder() const { return m_tableBreakingPageOrder; }
    void setTableBreakingPageOrder(KDReports::Report::TableBreakingPageOrder order);
    void setHorizontalHeaderFont(const QFont& font);
    void setVerticalHeaderFont(const QFont& font);

private:
    void drawBorder(const QRectF cellRect, QPainter &painter);
    void breakHorizontally();
    // Return sum of m_tableLayout.m_columnWidths; caller must ensure updateColumnWidths was called before.
    qreal totalWidth() const;
    qreal cellWidth( int col, int horizSpan ) const;
    qreal paintTableVerticalHeader( qreal x, qreal y, QPainter& painter, int row );
    void paintTableHorizontalHeader( const QRectF& cellRect, QPainter& painter, int col );
    void paintIcon( QPainter& painter, const QRectF& cellContentsRect, const QVariant& cellDecoration ) const;
    void paintTextAndIcon( QPainter& painter, const QRectF& cellContentsRect, const QString& cellText, const QVariant& cellDecoration, const QVariant& decorationAlignment, Qt::Alignment alignment );

    KDReports::TableLayout m_tableLayout;
    KDReports::Report::TableBreakingPageOrder m_tableBreakingPageOrder;
    int m_numHorizontalPages; // for scaleTo(). 1 if not set.
    int m_numVerticalPages;   // for scaleTo(). "Maximum" number of vertical pages. 0 if not set.
    bool m_layoutDirty;
    qreal m_userRequestedFontScalingFactor; // for scaleFontsBy(), 1.0 otherwise.
    QSizeF m_pageContentSize;

    // table-specific settings used during painting (and not during layouting)
    struct TableSettings
    {
        TableSettings()
            : m_border( 1.0 ),
              m_borderBrush( Qt::gray ),
              m_headerBackground( Qt::gray )
        {
        }
        qreal m_border;
        QBrush m_borderBrush;
        QBrush m_headerBackground;
    };
    TableSettings m_tableSettings;

    // The result of the layout:
    // a list of rects, each rect points to an "area" of the table
    // (in number of cells). For instance
    // Page 0 -> QRect(0,0,20,10), page 1 -> QRect(20,0,15,10)
    QList<QRect> m_pageRects;
    friend class MainTable;
};

}

#endif // KDREPORTSSPREADSHEETREPORTLAYOUT_H
