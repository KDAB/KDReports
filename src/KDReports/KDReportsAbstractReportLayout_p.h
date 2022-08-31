/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORTSABSTRACTREPORTLAYOUT_H
#define KDREPORTSABSTRACTREPORTLAYOUT_H

#include <QString>

QT_BEGIN_NAMESPACE
class QFont;
class QPainter;
class QPoint;
class QSizeF;
class QWidget;
QT_END_NAMESPACE

namespace KDReports {

class AbstractReportLayout
{
public:
    AbstractReportLayout();
    virtual ~AbstractReportLayout();
    AbstractReportLayout(const AbstractReportLayout &other) = delete;
    AbstractReportLayout &operator=(const AbstractReportLayout &other) = delete;

    virtual void setLayoutDirty() = 0;
    virtual void setDefaultFont(const QFont &font) = 0;
    virtual QFont defaultFont() const = 0;
    virtual void paintPageContent(int pageNumber, QPainter &painter) = 0;
    virtual int numberOfPages() = 0; // not const, since it can trigger a layout
    /**
     * Sets the size of the "page content": i.e. the body of the report,
     * not including the page headers+footers.
     * The size is in pixels.
     */
    virtual void setPageContentSize(QSizeF size) = 0;
    virtual qreal layoutAsOnePage(qreal width) = 0;
    virtual bool scaleTo(int numPagesHorizontally, int numPagesVertically) = 0;
    virtual void setFixedRowHeight(qreal height) = 0;
    virtual int maximumNumberOfPagesForHorizontalScaling() const = 0;
    virtual int maximumNumberOfPagesForVerticalScaling() const = 0;
    virtual void ensureLayouted() = 0;
    virtual void updateTextValue(const QString &id, const QString &newValue) = 0;
    /**
     * Returns the width that could be used when exporting to an image, for instance.
     * Unrelated to any paper sizes, just from the contents.
     * The returned width is in pixels.
     */
    virtual qreal idealWidth() = 0;
    virtual void setUserRequestedFontScalingFactor(qreal factor) = 0;
    virtual qreal userRequestedFontScalingFactor() const = 0;

    virtual QString anchorAt(int pageNumber, QPoint pos) = 0;

    virtual QString toHtml() const = 0;
    virtual void finishHtmlExport() = 0;
};

}

#endif // KDREPORTSABSTRACTREPORTLAYOUT_H
