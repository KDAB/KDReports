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

#include "KDReportsTextDocReportLayout_p.h"
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QPainter>
#include <QTextBlock>

KDReports::TextDocReportLayout::TextDocReportLayout(KDReports::Report *report)
    : m_textDocument()
    , m_builder(m_textDocument.contentDocumentData(), QTextCursor(&m_textDocument.contentDocument()), report)

{
}

void KDReports::TextDocReportLayout::setLayoutDirty() { }

void KDReports::TextDocReportLayout::paintPageContent(int pageNumber, QPainter &painter)
{
    painter.translate(0, -pageNumber * m_textDocument.contentDocument().pageSize().height());

    // Instead of using drawContents directly, we have to fork it in order to fix the palette (to avoid white-on-white in dark color schemes)
    // m_textDocument.contentDocument().drawContents(&painter, painter.clipRegion().boundingRect());
    // This even allows us to optimize it a bit (painter clip rect already set)

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = painter.clipRegion().boundingRect();
    ctx.palette.setColor(QPalette::Text, Qt::black);
    m_textDocument.contentDocument().documentLayout()->draw(&painter, ctx);
}

int KDReports::TextDocReportLayout::numberOfPages()
{
    // qDebug() << "page height" << m_textDocument.contentDocument().pageSize().height();
    // qDebug() << "doc height" << m_textDocument.contentDocument().size().height();
    return m_textDocument.contentDocument().pageCount();
}

void KDReports::TextDocReportLayout::setPageContentSize(const QSizeF &size)
{
    m_textDocument.setPageSize(size);
}

void KDReports::TextDocReportLayout::ensureLayouted() { }

QString KDReports::TextDocReportLayout::toHtml() const
{
    return m_textDocument.asHtml();
}

qreal KDReports::TextDocReportLayout::layoutAsOnePage(qreal docWidth)
{
    m_textDocument.layoutWithTextWidth(docWidth);
    qreal docHeight = m_textDocument.contentDocument().size().height();

    // We need to get rid of all page breaks...
    // Unittest: PageLayout::testEndlessPrinterWithPageBreaks()
    QTextCursor c(&m_textDocument.contentDocument());
    c.beginEditBlock();
    QTextBlock block = m_textDocument.contentDocument().firstBlock();
    do {
        // qDebug() << "addBlock: Looking at block" << block.blockNumber();
        QTextBlockFormat format = block.blockFormat();
        if (format.pageBreakPolicy() != QTextBlockFormat::PageBreak_Auto)
            format.setPageBreakPolicy(QTextBlockFormat::PageBreak_Auto);
        c.setPosition(block.position());
        c.setBlockFormat(format);
        block = block.next();
    } while (block.isValid());
    c.endEditBlock();

    setPageContentSize(QSizeF(docWidth, docHeight));
    qDebug() << "m_textDocument.layoutDocument().setPageSize" << docWidth << "x" << docHeight << numberOfPages() << "pages";
    qreal newDocHeight = m_textDocument.contentDocument().size().height();
    if (newDocHeight > docHeight) {
        // QTextDocument is playing tricks on us; it was able to layout as docWidth x docHeight
        // but once we set that as the page size, we end up with more height...
        // Unittest: PageLayout::testEndlessPrinterBug()
        qDebug() << "newDocHeight=" << newDocHeight << "expected" << docHeight;
        setPageContentSize(QSizeF(docWidth, newDocHeight));
        newDocHeight = m_textDocument.contentDocument().size().height();
        qDebug() << "final newDocHeight=" << newDocHeight << numberOfPages() << "pages";
    }

    Q_ASSERT(numberOfPages() == 1);
    return newDocHeight;
}

void KDReports::TextDocReportLayout::finishHtmlExport()
{
    m_textDocument.contentDocumentData().saveResourcesToFiles();
}

void KDReports::TextDocReportLayout::setDefaultFont(const QFont &font)
{
    m_textDocument.contentDocument().setDefaultFont(font);
    m_builder.setDefaultFont(font);
}

QFont KDReports::TextDocReportLayout::defaultFont() const
{
    return m_textDocument.defaultFont();
}

void KDReports::TextDocReportLayout::updateTextValue(const QString &id, const QString &newValue)
{
    m_textDocument.updateTextValue(id, newValue);
}

qreal KDReports::TextDocReportLayout::idealWidth()
{
    // See Database example for the +1, the right border was missing otherwise.
    return m_textDocument.contentDocument().idealWidth() + 1; // in pixels
}

bool KDReports::TextDocReportLayout::scaleTo(int numPagesHorizontally, int numPagesVertically)
{
    Q_UNUSED(numPagesHorizontally);
    Q_UNUSED(numPagesVertically);
    qWarning("scaleTo is only implemented in Spreadsheet mode currently");
    return false;
}

void KDReports::TextDocReportLayout::setFixedRowHeight(qreal height)
{
    Q_UNUSED(height);
    qWarning("fixed row height is only implemened in Spreadsheet mode");
}

int KDReports::TextDocReportLayout::maximumNumberOfPagesForHorizontalScaling() const
{
    return 1;
}

int KDReports::TextDocReportLayout::maximumNumberOfPagesForVerticalScaling() const
{
    return 1; // not implemented
}

void KDReports::TextDocReportLayout::setUserRequestedFontScalingFactor(qreal factor)
{
    Q_UNUSED(factor);
    qWarning("font scaling is only implemented in Spreadsheet mode currently");
}

qreal KDReports::TextDocReportLayout::userRequestedFontScalingFactor() const
{
    return 1; // not implemented
}
