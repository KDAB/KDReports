/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORTSTABLEELEMENT_H
#define KDREPORTSTABLEELEMENT_H

#include "KDReportsAbstractTableElement.h"

namespace KDReports {
class Cell;
class TableElementPrivate;

/**
 * The KDReports::TableElement class represents a table in the report.
 * Use cell() to provide the contents for each cell of the table.
 *
 * Use AutoTableElement instead if you have the data in a QAbstractItemModel.
 */
class KDREPORTS_EXPORT TableElement : public AbstractTableElement
{
public:
    /**
     * Creates an empty table.
     * Text and other content can be added to the table cells using
     * the cell() method.
     * This must be done before the table is added to the report.
     */
    TableElement();

    /**
     * Destructor. Deletes internal data.
     */
    ~TableElement() override;

    /**
     * Copies a table element.
     * Settings and cell elements are copied over.
     */
    TableElement(const TableElement &other);
    /**
     * Copies the settings and cell elements from another table element.
     */
    TableElement &operator=(const TableElement &other);

    /**
     * Declares the first @p count rows of the table as table header.
     * The table header rows get repeated when a table is broken across a page boundary.
     * The default number of header rows is 0.
     */
    void setHeaderRowCount( int count );

    /**
     * \return table header rows count.
     * \since 1.4
     */
    int headerRowCount() const;

    /**
     * Declares the first @p count columns of the table as table header.
     * The table header columns get repeated when a table is broken
     * into multiple pages horizontally (see Report::setTableBreakingEnabled).
     * The default number of header rows is 0.
     * \since 1.1
     */
    void setHeaderColumnCount( int count );

    /**
     * \return table header columns count.
     * \since 1.4
     */
    int headerColumnCount() const;

    /**
     * Returns the reference to a cell in the table.
     * @param row number, starting from 0
     * @param column number, starting from 0
     */
    Cell& cell( int row, int column );

    /**
     * @internal
     * @reimp
     */
    void build( ReportBuilder& ) const override;
    /**
     * @internal
     * @reimp
     */
    Element* clone() const override;

private:
    TableElementPrivate* const d;
};

}


#endif /* KDREPORTSTABLEELEMENT_H */

