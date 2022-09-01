/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSMAINTABLE_H
#define KDREPORTSMAINTABLE_H

#include "KDReportsReport.h"

namespace KDReports {
class AutoTableElement;
class Report;
class Test;
class SpreadsheetReportLayout;
class MainTablePrivate;

/**
 * The MainTable class represents the main table of a report
 * in spread sheet mode.
 *
 * You can access it using Report::mainTable(). Example:
 * \code
 *   KDReports::Report report;
 *   report.setReportMode(Report::SpreadSheet);
 *   KDReports::AutoTableElement autoTable(myItemModel);
 *   report.mainTable()->setAutoTableElement(autoTable);
 * \endcode
 *
 * \since 1.3
 */
class KDREPORTS_EXPORT MainTable
{
public:
    /**
     * Sets the auto table element, which contains the definition of the main table.
     */
    void setAutoTableElement(const AutoTableElement &element);
    /**
     * \return the auto table element passed to setAutoTableElement
     */
    AutoTableElement *autoTableElement() const;

    /**
     * Sets the page order to use when breaking tables.
     * When a table is bigger than the page in both directions, horizontally
     * and vertically, then the table breaking can be done in two ways:
     * either the table goes "down then right", i.e. all rows appear first,
     * for the first few columns, and then other columns are printed,
     * or it goes "right then down", i.e. all columns appear first,
     * for the number of rows that fit into one page, and then
     * the bottom part of the table is printed.
     *
     * The default page order is DownThenRight.
     */
    void setTableBreakingPageOrder(Report::TableBreakingPageOrder pageOrder);

    /**
     * \return the table breaking page order given to setTableBreakingPageOrder
     */
    Report::TableBreakingPageOrder tableBreakingPageOrder() const;

    /**
     * Sets the font to use for the horizontal header of the table.
     * By default the font passed to report.setDefaultFont is used.
     */
    void setHorizontalHeaderFont(const QFont &font);

    /**
     * Sets the font to use for the vertical header of the table.
     * By default the font passed to report.setDefaultFont is used.
     */
    void setVerticalHeaderFont(const QFont &font);

private:
    friend class Report;
    friend class ReportPrivate;
    MainTable();
    ~MainTable();

    void setLayout(SpreadsheetReportLayout *layout);

    friend class Test;
    QList<QRect> pageRects() const; // for unittests
    qreal lastAutoFontScalingFactor() const; // for unittests

    Q_DISABLE_COPY(MainTable)
    std::unique_ptr<MainTablePrivate> d;
};

}

#endif // KDREPORTSMAINTABLE_H
