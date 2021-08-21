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

#include "KDReportsTableBreakingLogic_p.h"

using namespace KDReports;

TableBreakingLogic::TableBreakingLogic()
    : m_pages(1)
{
}

//@cond PRIVATE
void TableBreakingLogic::setColumnWidths(const WidthVector &widths)
{
    m_widths = widths;
}
//@endcond

void TableBreakingLogic::setPageCount(int pages)
{
    m_pages = pages;
}

QVector<int> TableBreakingLogic::columnsPerPage() const
{
    QVector<int> columnsForPage;
    if (m_pages == 0)
        return columnsForPage;
    const int numColumnsForPage = m_widths.count();
    qreal totalWidth = 0;
    for (int i = 0; i < numColumnsForPage; ++i) {
        totalWidth += m_widths[i];
    }
    const qreal idealPageWidth = totalWidth / m_pages;
    // qDebug() << "idealPageWidth=" << idealPageWidth;
    qreal currentWidth = 0;
    columnsForPage.resize(m_pages);
    int columnsInThisPage = 0;
    int pageNumber = 0;
    for (int i = 0; i < numColumnsForPage; ++i) {
        const qreal columnWidth = m_widths[i];
        const bool lastPage = pageNumber == m_pages - 1;
        if (currentWidth + columnWidth > idealPageWidth && !lastPage) {
            // Better to include or exclude the column?
            // That's the tricky part. If we exclude too much
            // the last page will be crammed, while if we include
            // a huge column this one might be crammed.
            // Let's have a threshold then: include it if we won't go over-board
            // more than 1/3rd of the page width.
            if (currentWidth + columnWidth < idealPageWidth * 4 / 3 || currentWidth == 0) { // Avoid infinite loop if huge column
                columnsInThisPage += 1;
            } else {
                // current column will be for next page.
                --i;
            }
            columnsForPage[pageNumber++] = columnsInThisPage;

            currentWidth = 0;
            columnsInThisPage = 0;
        } else {
            currentWidth += columnWidth;
            columnsInThisPage += 1;
        }
    }
    if (pageNumber < m_pages && columnsInThisPage > 0) {
        columnsForPage[pageNumber++] = columnsInThisPage;
    }
    columnsForPage.resize(pageNumber);
#ifdef DEBUG_TABLEBREAKINGLOGIC
    qDebug() << "Result of optimized table breaking:" << columnsForPage;
#endif
    return columnsForPage;
}

TableBreakingLogic::WidthVector TableBreakingLogic::widthPerPage(const QVector<int> &colPerPage) const
{
    Q_ASSERT(colPerPage.size() <= m_pages);
    int startColumn = 0;
    WidthVector widths;
    widths.resize(colPerPage.size());
    for (int page = 0; page < colPerPage.size(); ++page) {
        const int numColumnsForPage = colPerPage[page];
        qreal pageTotalWidth = 0;
        for (int i = startColumn; i < startColumn + numColumnsForPage; ++i) {
            pageTotalWidth += m_widths[i];
        }
        startColumn += numColumnsForPage;
        widths[page] = pageTotalWidth;
    }
    return widths;
}
