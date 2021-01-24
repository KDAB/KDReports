/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#ifndef KDREPORTSTEXTDOCREPORTLAYOUT_H
#define KDREPORTSTEXTDOCREPORTLAYOUT_H

#include "KDReportsAbstractReportLayout_p.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsTextDocument_p.h"

namespace KDReports {

class TextDocReportLayout : public AbstractReportLayout
{
public:
    explicit TextDocReportLayout(KDReports::Report *report);

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
    void setPageContentSize(const QSizeF &size) override;
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
    QString toHtml() const override;
    /// \reimp
    void finishHtmlExport() override;

    // for unittests only
    TextDocument &textDocument() { return m_textDocument; }

    ReportBuilder *builder() { return &m_builder; }

private:
    TextDocument m_textDocument;
    ReportBuilder m_builder;
};

}

#endif // KDREPORTSTEXTDOCREPORTLAYOUT_H
