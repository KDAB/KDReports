/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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
class KDREPORTS_EXPORT TableElement : public AbstractTableElement // clazy:exclude=copyable-polymorphic
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
     * Returns the reference to a cell in the table.
     * This is the method to use in order to fill the table.
     * Cells are created on demand, you don't have to define a number of rows and columns before hand.
     *
     * @param row number, starting from 0
     * @param column number, starting from 0
     */
    Cell &cell(int row, int column);

    /**
     * Declares the first @p count rows of the table as table header.
     * The table header rows get repeated when a table is broken across a page boundary.
     * The default number of header rows is 0.
     */
    void setHeaderRowCount(int count);

    /**
     * \return table header rows count.
     * \since 1.4
     */
    int headerRowCount() const;

    /**
     * Declares the first @p count columns of the table as table header.
     * The table header columns get repeated when a table is broken
     * into multiple pages horizontally (see Report::scaleTo).
     * The default number of header rows is 0.
     * \since 1.1
     */
    void setHeaderColumnCount(int count);

    /**
     * \return table header columns count.
     * \since 1.4
     */
    int headerColumnCount() const;

    /**
     * \return number of rows, based on the calls to cell(row, column)
     * \since 2.3
     */
    int rowCount() const;
    /**
     * \return number of columns, based on the calls to cell(row, column)
     * \since 2.3
     */
    int columnCount() const;

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &) const override;
    /**
     * @internal
     * @reimp
     */
    Element *clone() const override;

private:
    std::unique_ptr<TableElementPrivate> d;
};

}

#endif /* KDREPORTSTABLEELEMENT_H */
