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

#ifndef KDREPORTSTEXTDOCREPORTLAYOUT_H
#define KDREPORTSTEXTDOCREPORTLAYOUT_H

#include "KDReportsTextDocument_p.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsAbstractReportLayout_p.h"

namespace KDReports
{

class TextDocReportLayout : public AbstractReportLayout
{
public:
    explicit TextDocReportLayout(KDReports::Report* report);

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

    // for unittests only
    TextDocument& textDocument() { return m_textDocument; }

    ReportBuilder* builder() {return &m_builder;}

private:
    TextDocument m_textDocument;
    ReportBuilder m_builder;
};

}

#endif // KDREPORTSTEXTDOCREPORTLAYOUT_H
