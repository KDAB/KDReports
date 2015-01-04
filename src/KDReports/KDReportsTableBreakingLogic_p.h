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

#ifndef TABLEBREAKINGLOGIC_H
#define TABLEBREAKINGLOGIC_H

#include <QVector>

namespace KDReports
{

/// @internal
/// Table breaking in the horizontal direction
class TableBreakingLogic
{
public:
    TableBreakingLogic();

    typedef QVector<qreal> WidthVector;
    void setColumnWidths( const WidthVector& widths );
    void setPageCount( int pages );

    /// Performs the optimization calculation and
    /// returns: the number of columns per page
    /// This does NOT take into account the vertical header,
    /// since it will anyway have to be on every page.
    QVector<int> columnsPerPage() const;

    /// Simple transformation of the result of columnsPerPage,
    /// returns the sum of the column widths for a given page
    WidthVector widthPerPage( const QVector<int>& colPerPage ) const;

private:
    WidthVector m_widths;
    int m_pages;
};

} // namespace KDReports

#endif /* TABLEBREAKINGLOGIC_H */

