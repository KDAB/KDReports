/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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
using CellContentMap = QMap<QPair<int /*row*/, int /*column*/>, Cell>;
}

class KDReports::TableElementPrivate
{
public:
    void createCell(QTextTable *textTable, ReportBuilder &builder, int row, int column, const Cell &cell, QTextCharFormat charFormat) const;

    KDReports::CellContentMap m_cellContentMap;
    int m_rowCount = 0;
    int m_columnCount = 0;
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

int KDReports::TableElement::rowCount() const
{
    return d->m_rowCount;
}

int KDReports::TableElement::columnCount() const
{
    return d->m_columnCount;
}

KDReports::Cell &KDReports::TableElement::cell(int row, int column)
{
    d->m_rowCount = std::max(d->m_rowCount, row + 1);
    d->m_columnCount = std::max(d->m_columnCount, column + 1);

    const QPair<int, int> coord = qMakePair(row, column);
    return d->m_cellContentMap[coord]; // find or create
}

void KDReports::TableElementPrivate::createCell(QTextTable *textTable, ReportBuilder &builder, int row, int column, const Cell &cell, QTextCharFormat charFormat) const
{
    if (cell.columnSpan() > 1 || cell.rowSpan() > 1)
        textTable->mergeCells(row, column, cell.rowSpan(), cell.columnSpan());
    QTextTableCell tableCell = textTable->cellAt(row, column);
    Q_ASSERT(tableCell.isValid());
    QTextCursor cellCursor = tableCell.firstCursorPosition();
    QTextTableCellFormat tableCellFormat(charFormat.toTableCellFormat());
    if (cell.background().style() != Qt::NoBrush)
        tableCellFormat.setBackground(cell.background());
    tableCellFormat.setTableCellColumnSpan(cell.columnSpan());
    tableCellFormat.setTableCellRowSpan(cell.rowSpan());
    if (cell.verticalAlignment() != 0)
        tableCellFormat.setVerticalAlignment(ReportBuilder::toVerticalAlignment(cell.verticalAlignment()));
    if (auto func = cell.cellFormatFunction())
        func(row, column, tableCellFormat);
    tableCell.setFormat(tableCellFormat);
    cellCursor.setCharFormat(tableCellFormat);
    ReportBuilder cellBuilder(builder.currentDocumentData(), cellCursor, builder.report());
    cellBuilder.copyStateFrom(builder);
    cellBuilder.setDefaultFont(charFormat.font());
    cell.build(cellBuilder);
}

void KDReports::TableElement::build(ReportBuilder &builder) const
{
    if (d->m_cellContentMap.isEmpty())
        return;

    QTextCursor &textDocCursor = builder.cursor();

    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount(d->m_headerRowCount);
    tableFormat.setProperty(KDReports::HeaderColumnsProperty, d->m_headerColumnCount);
    tableFormat.setAlignment(textDocCursor.blockFormat().alignment());
    tableFormat.setBackground(background());
    tableFormat.setCellPadding(2); // Qt 6.8 changes the default to 4, enforce 2
    tableFormat.setBorderCollapse(false); // Qt 6.8 changes the default to true, enforce false
    fillTableFormat(tableFormat, textDocCursor);
    QTextCharFormat charFormat = textDocCursor.charFormat();

    QTextTable *textTable = textDocCursor.insertTable(d->m_rowCount, d->m_columnCount, tableFormat);

    CellContentMap::const_iterator it = d->m_cellContentMap.constBegin();
    for (; it != d->m_cellContentMap.constEnd(); ++it) {
        const int row = it.key().first;
        const int column = it.key().second;
        const Cell &cell = it.value();
        d->createCell(textTable, builder, row, column, cell, charFormat);
    }

    textDocCursor.movePosition(QTextCursor::End);

    builder.currentDocumentData().registerTable(textTable);
}
