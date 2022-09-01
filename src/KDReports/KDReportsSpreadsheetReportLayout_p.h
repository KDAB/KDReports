/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSSPREADSHEETREPORTLAYOUT_H
#define KDREPORTSSPREADSHEETREPORTLAYOUT_H

#include "KDReportsAbstractReportLayout_p.h"
#include "KDReportsReport.h"
#include "KDReportsTableLayout_p.h"
#include <QBrush>

namespace KDReports {
class Report;
class MainTable;

class SpreadsheetReportLayout : public AbstractReportLayout
{
public:
    explicit SpreadsheetReportLayout(KDReports::Report *report);

    /// \reimp
    void setLayoutDirty() override;
    /// \reimp
    void setDefaultFont(const QFont &font) override;
    /// \reimp
    QFont defaultFont() const override;
    /// \reimp
    void paintPageContent(int pageNumber, QPainter &painter) override;
    /// \reimp
    int numberOfPages() override; // not const, since it can trigger a layout
    /// \reimp
    qreal idealWidth() override;
    /// \reimp
    void setPageContentSize(QSizeF size) override;
    /// \reimp
    void ensureLayouted() override;
    /// \reimp
    void updateTextValue(const QString &id, const QString &newValue) override;
    /// \reimp
    qreal layoutAsOnePage(qreal width) override;
    /// \reimp
    bool scaleTo(int numPagesHorizontally, int numPagesVertically) override;
    /// \reimp
    void setFixedRowHeight(qreal height) override;
    /// \reimp
    int maximumNumberOfPagesForHorizontalScaling() const override;
    /// \reimp
    int maximumNumberOfPagesForVerticalScaling() const override;
    /// \reimp
    void setUserRequestedFontScalingFactor(qreal factor) override;
    /// \reimp
    qreal userRequestedFontScalingFactor() const override;
    /// \reimp
    QString anchorAt(int pageNumber, QPoint pos) override;
    /// \reimp
    QString toHtml() const override;
    /// \reimp
    void finishHtmlExport() override;

    void setModel(QAbstractItemModel *model);
    void setVerticalHeaderVisible(bool visible);
    void setHorizontalHeaderVisible(bool visible);
    void setCellPadding(qreal padding);
    void setIconSize(QSize iconSize);
    void setCellBorder(qreal border, const QBrush &borderBrush);
    void setHeaderBackground(const QBrush &headerBackground);

    KDReports::Report::TableBreakingPageOrder tableBreakingPageOrder() const
    {
        return m_tableBreakingPageOrder;
    }
    void setTableBreakingPageOrder(KDReports::Report::TableBreakingPageOrder order);
    void setHorizontalHeaderFont(const QFont &font);
    void setVerticalHeaderFont(const QFont &font);

private:
    void drawBorder(const QRectF &cellRect, QPainter &painter) const;
    void breakHorizontally();
    // Return sum of m_tableLayout.m_columnWidths; caller must ensure updateColumnWidths was called before.
    qreal totalWidth() const;
    qreal cellWidth(int col, int horizSpan) const;
    qreal paintTableVerticalHeader(qreal x, qreal y, QPainter &painter, int row);
    void paintTableHorizontalHeader(const QRectF &cellRect, QPainter &painter, int col);
    void paintIcon(QPainter &painter, const QRectF &cellContentsRect, const QVariant &cellDecoration) const;
    void paintTextAndIcon(QPainter &painter, const QRectF &cellContentsRect, const QString &cellText, const QVariant &cellDecoration, const QVariant &decorationAlignment, Qt::Alignment alignment);

    KDReports::TableLayout m_tableLayout;
    KDReports::Report::TableBreakingPageOrder m_tableBreakingPageOrder;
    int m_numHorizontalPages; // for scaleTo(). 1 if not set.
    int m_numVerticalPages; // for scaleTo(). "Maximum" number of vertical pages. 0 if not set.
    bool m_layoutDirty;
    qreal m_userRequestedFontScalingFactor; // for scaleFontsBy(), 1.0 otherwise.
    QSizeF m_pageContentSize;

    // table-specific settings used during painting (and not during layouting)
    struct TableSettings
    {
        TableSettings()
            : m_border(1.0)
            , m_borderBrush(Qt::gray)
            , m_headerBackground(Qt::gray)
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
