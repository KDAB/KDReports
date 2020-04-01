/****************************************************************************
** Copyright (C) 2007-2020 Klaralvdalens Datakonsult AB.  All rights reserved.
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
    void setLayoutDirty() override;
    /// \reimp
    void setDefaultFont(const QFont& font) override;
    /// \reimp
    QFont defaultFont() const override;
    /// \reimp
    void paintPageContent(int pageNumber, QPainter& painter) override;
    /// \reimp
    int numberOfPages() override; // not const, since it can trigger a layout
    /// \reimp
    qreal idealWidth() override;
    /// \reimp
    void setPageContentSize(const QSizeF& size) override;
    /// \reimp
    void ensureLayouted() override;
    /// \reimp
    void updateTextValue( const QString& id, const QString& newValue ) override;
    /// \reimp
    qreal layoutAsOnePage(qreal width) override;
    /// \reimp
    bool scaleTo( int numPagesHorizontally, int numPagesVertically ) override;
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
    QString toHtml() const override;
    /// \reimp
    void finishHtmlExport() override;

    // for unittests only
    TextDocument& textDocument() { return m_textDocument; }

    ReportBuilder* builder() {return &m_builder;}

private:
    TextDocument m_textDocument;
    ReportBuilder m_builder;
};

}

#endif // KDREPORTSTEXTDOCREPORTLAYOUT_H
