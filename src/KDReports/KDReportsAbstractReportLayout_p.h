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

#ifndef KDREPORTSABSTRACTREPORTLAYOUT_H
#define KDREPORTSABSTRACTREPORTLAYOUT_H

#include <QString>
class QPainter;
class QSizeF;
class QWidget;
class QFont;

namespace KDReports
{

class AbstractReportLayout
{
public:
    AbstractReportLayout();
    virtual ~AbstractReportLayout();

    virtual void setLayoutDirty() = 0;
    virtual void setDefaultFont(const QFont& font) = 0;
    virtual QFont defaultFont() const = 0;
    virtual void paintPageContent(int pageNumber, QPainter& painter) = 0;
    virtual int numberOfPages() = 0; // not const, since it can trigger a layout
    /**
     * Sets the size of the "page content": i.e. the body of the report,
     * not including the page headers+footers.
     * The size is in pixels.
     */
    virtual void setPageContentSize(const QSizeF& size) = 0;
    virtual qreal layoutAsOnePage(qreal width) = 0;
    virtual bool scaleTo( int numPagesHorizontally, int numPagesVertically ) = 0;
    virtual void setFixedRowHeight(qreal height) = 0;
    virtual int maximumNumberOfPagesForHorizontalScaling() const = 0;
    virtual int maximumNumberOfPagesForVerticalScaling() const = 0;
    virtual void ensureLayouted() = 0;
    virtual void updateTextValue( const QString& id, const QString& newValue ) = 0;
    /**
     * Returns the width that could be used when exporting to an image, for instance.
     * Unrelated to any paper sizes, just from the contents.
     * The returned width is in pixels.
     */
    virtual qreal idealWidth() = 0;
    virtual void setUserRequestedFontScalingFactor(qreal factor) = 0;
    virtual qreal userRequestedFontScalingFactor() const = 0;

    virtual QString toHtml() const = 0;
    virtual void finishHtmlExport() = 0;
};

}

#endif // KDREPORTSABSTRACTREPORTLAYOUT_H
