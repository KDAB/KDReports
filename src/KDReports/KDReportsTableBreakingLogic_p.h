/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef TABLEBREAKINGLOGIC_H
#define TABLEBREAKINGLOGIC_H

#include <QVector>

namespace KDReports {

/// @internal
/// Table breaking in the horizontal direction
class TableBreakingLogic
{
public:
    TableBreakingLogic();

    typedef QVector<qreal> WidthVector;
    void setColumnWidths(const WidthVector &widths);
    void setPageCount(int pages);

    /// Performs the optimization calculation and
    /// returns: the number of columns per page
    /// This does NOT take into account the vertical header,
    /// since it will anyway have to be on every page.
    QVector<int> columnsPerPage() const;

    /// Simple transformation of the result of columnsPerPage,
    /// returns the sum of the column widths for a given page
    WidthVector widthPerPage(const QVector<int> &colPerPage) const;

private:
    WidthVector m_widths;
    int m_pages;
};

} // namespace KDReports

#endif /* TABLEBREAKINGLOGIC_H */
