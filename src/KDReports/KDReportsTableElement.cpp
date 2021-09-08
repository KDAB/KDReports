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

#include "KDReportsTableElement.h"
#include "KDReportsCell.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsReport.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsTextDocument_p.h"
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QPainter>
#include <QTextCursor>
#include <QTextTableCell>

namespace KDReports {
class CellContentMap : public QMap<QPair<int /*row*/, int /*column*/>, Cell>
{
public:
    CellContentMap() { }
    void getSize(int &rows, int &columns) const
    {
        rows = 0;
        columns = 0;
        for (const_iterator it = begin(); it != end(); ++it) {
            rows = qMax(rows, it.key().first + 1);
            columns = qMax(columns, it.key().second + 1);
        }
    }
};
}

class KDReports::TableElementPrivate
{
public:
    KDReports::CellContentMap m_cellContentMap;
    int m_headerRowCount = 0;
    int m_headerColumnCount = 0;
};

////

KDReports::TableElement::TableElement()
    : d(new TableElementPrivate)
{
}

KDReports::TableElement::TableElement(const TableElement &other)
    : AbstractTableElement(other)
    , d(new TableElementPrivate(*other.d))
{
}

KDReports::TableElement &KDReports::TableElement::operator=(const TableElement &other)
{
    if (&other == this)
        return *this;
    AbstractTableElement::operator=(other);
    *d = *other.d;
    return *this;
}

KDReports::TableElement::~TableElement()
{
    delete d;
}

KDReports::Element *KDReports::TableElement::clone() const
{
    // never used at the moment
    return new TableElement(*this);
}

void KDReports::TableElement::setHeaderRowCount(int count)
{
    d->m_headerRowCount = count;
}

int KDReports::TableElement::headerRowCount() const
{
    return d->m_headerRowCount;
}

void KDReports::TableElement::setHeaderColumnCount(int count)
{
    d->m_headerColumnCount = count;
}

int KDReports::TableElement::headerColumnCount() const
{
    return d->m_headerColumnCount;
}

KDReports::Cell &KDReports::TableElement::cell(int row, int column)
{
    const QPair<int, int> coord = qMakePair(row, column);
    return d->m_cellContentMap[coord]; // find or create
}

void KDReports::TableElement::build(ReportBuilder &builder) const
{
    if (d->m_cellContentMap.isEmpty())
        return;

    QTextCursor &textDocCursor = builder.cursor();

    int rowCount;
    int columnCount;
    d->m_cellContentMap.getSize(rowCount, columnCount);

    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount(d->m_headerRowCount);
    tableFormat.setProperty(KDReports::HeaderColumnsProperty, d->m_headerColumnCount);
    tableFormat.setAlignment(textDocCursor.blockFormat().alignment());
    tableFormat.setBackground(background());
    fillTableFormat(tableFormat, textDocCursor);
    QTextCharFormat charFormat = textDocCursor.charFormat();

    QTextTable *textTable = textDocCursor.insertTable(rowCount, columnCount, tableFormat);

    CellContentMap::const_iterator it = d->m_cellContentMap.constBegin();
    for (; it != d->m_cellContentMap.constEnd(); ++it) {
        const int row = it.key().first;
        const int column = it.key().second;
        const Cell &cell = it.value();
        if (cell.columnSpan() > 1 || cell.rowSpan() > 1)
            textTable->mergeCells(row, column, cell.rowSpan(), cell.columnSpan());
        QTextTableCell tableCell = textTable->cellAt(row, column);
        Q_ASSERT(tableCell.isValid());
        QTextCursor cellCursor = tableCell.firstCursorPosition();
        QTextCharFormat tableCellFormat = charFormat;
        tableCellFormat.setBackground(cell.background());
        tableCellFormat.setTableCellColumnSpan(cell.columnSpan());
        tableCellFormat.setTableCellRowSpan(cell.rowSpan());
        tableCell.setFormat(tableCellFormat);
        cellCursor.setCharFormat(tableCellFormat);
        ReportBuilder cellBuilder(builder.currentDocumentData(), cellCursor, builder.report());
        cellBuilder.copyStateFrom(builder);
        cellBuilder.setDefaultFont(charFormat.font());
        cell.build(cellBuilder);
    }

    textDocCursor.movePosition(QTextCursor::End);

    builder.currentDocumentData().registerTable(textTable);
}
