/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsAutoTableElement.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsReport_p.h" // modelForKey
#include <QAbstractItemModel>
#include <QBitArray>
#include <QDateTime>
#include <QDebug>
#include <QIcon>
#include <QTextCursor>
#include <QTextTableCell>
#include <QUrl>
#include <QVector>

class KDReports::AutoTableElementPrivate
{
public:
    void fillCellFromHeaderData(int section, Qt::Orientation orientation, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable, ReportBuilder &builder) const;
    QSize fillTableCell(int row, int column, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable, ReportBuilder &builder) const;

    QAbstractItemModel *m_tableModel = nullptr;
    QString m_modelKey;
    bool m_verticalHeaderVisible = true;
    bool m_horizontalHeaderVisible = true;
    QBrush m_headerBackground = QColor(218, 218, 218);
    QSize m_iconSize = QSize(32, 32);
};

// Helper for fillCellFromHeaderData and fillTableCell
class FillCellHelper
{
public:
    FillCellHelper(QAbstractItemModel *tableModel, int section, Qt::Orientation orientation, QSize iconSz)
        : iconSize(iconSz)
        , cellDecoration(tableModel->headerData(section, orientation, Qt::DecorationRole))
        , cellFont(tableModel->headerData(section, orientation, Qt::FontRole))
        , cellText(tableModel->headerData(section, orientation, Qt::DisplayRole).toString())
        , foreground(tableModel->headerData(section, orientation, Qt::ForegroundRole))
        , background(tableModel->headerData(section, orientation, Qt::BackgroundRole))
        , alignment(Qt::Alignment(tableModel->headerData(section, orientation, Qt::TextAlignmentRole).toInt()))
        , decorationAlignment(tableModel->headerData(section, orientation, KDReports::AutoTableElement::DecorationAlignmentRole))
        , nonBreakableLines(tableModel->headerData(section, orientation, KDReports::AutoTableElement::NonBreakableLinesRole).toBool())
        , span(1, 1)
    {
    }
    FillCellHelper(QAbstractItemModel *tableModel, const QModelIndex &index, QSize _span, QSize iconSz)
        : iconSize(iconSz)
        , cellDecoration(tableModel->data(index, Qt::DecorationRole))
        , cellFont(tableModel->data(index, Qt::FontRole))
        , cellText(displayText(tableModel->data(index, Qt::DisplayRole)))
        , foreground(tableModel->data(index, Qt::ForegroundRole))
        , background(tableModel->data(index, Qt::BackgroundRole))
        , alignment(Qt::Alignment(tableModel->data(index, Qt::TextAlignmentRole).toInt()))
        , decorationAlignment(tableModel->data(index, KDReports::AutoTableElement::DecorationAlignmentRole))
        , nonBreakableLines(tableModel->data(index, KDReports::AutoTableElement::NonBreakableLinesRole).toBool())
        , span(_span)
    {
    }
    void fill(QTextTable *textTable, KDReports::ReportBuilder &builder, QTextDocument &textDoc, QTextTableCell &cell);

private:
    void insertDecoration(KDReports::ReportBuilder &builder, QTextDocument &textDoc);
    static QString displayText(const QVariant &value);

    QSize iconSize;
    QVariant cellDecoration;
    QVariant cellFont;
    QString cellText;
    QVariant foreground;
    QVariant background;
    Qt::Alignment alignment;
    QVariant decorationAlignment;
    bool nonBreakableLines;
    QSize span;

    QTextCursor cellCursor;
};

static QTextCharFormat::VerticalAlignment toVerticalAlignment(Qt::Alignment alignment)
{
    switch (alignment & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
        return QTextCharFormat::AlignTop;
    case Qt::AlignBottom:
        return QTextCharFormat::AlignBottom;
    case Qt::AlignVCenter:
        return QTextCharFormat::AlignMiddle;
    case Qt::AlignBaseline:
        return QTextCharFormat::AlignBaseline;
    }
    return QTextCharFormat::AlignNormal;
}

void FillCellHelper::fill(QTextTable *textTable, KDReports::ReportBuilder &builder, QTextDocument &textDoc, QTextTableCell &cell)
{
    cellCursor = cell.firstCursorPosition();
    QTextCharFormat cellFormat = cell.format();
    if (background.canConvert<QBrush>()) {
        cellFormat.setBackground(qvariant_cast<QBrush>(background));
    }
    cellFormat.setVerticalAlignment(toVerticalAlignment(alignment));
    cell.setFormat(cellFormat);

    QTextBlockFormat blockFormat = cellCursor.blockFormat();
    blockFormat.setAlignment(alignment);
    blockFormat.setNonBreakableLines(nonBreakableLines);
    builder.setupBlockFormat(blockFormat);

    cellCursor.setBlockFormat(blockFormat);

    const bool hasIcon = !cellDecoration.isNull();
    const bool iconAfterText = decorationAlignment.isValid() && (decorationAlignment.toInt() & Qt::AlignRight);
    if (hasIcon && !iconAfterText) {
        insertDecoration(builder, textDoc);
    }

    QTextCharFormat charFormat = cellCursor.charFormat();
    if (cellFont.isValid()) {
        QFont cellQFont = qvariant_cast<QFont>(cellFont);
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        charFormat.setFont(cellQFont, QTextCharFormat::FontPropertiesSpecifiedOnly);
#else
        charFormat.setFont(cellQFont);
#endif
    } else {
        charFormat.setFont(builder.defaultFont());
    }
    if (foreground.canConvert<QBrush>()) {
        charFormat.setForeground(qvariant_cast<QBrush>(foreground));
    }
    cellCursor.setCharFormat(charFormat);

    if (hasIcon && !iconAfterText) {
        cellCursor.insertText(QChar::fromLatin1(' ')); // spacing between icon and text
    }

    // qDebug() << cellText;
    if (cellText.startsWith(QLatin1String("<qt>")) || cellText.startsWith(QLatin1String("<html>")))
        cellCursor.insertHtml(cellText);
    else
        cellCursor.insertText(cellText);

    if (hasIcon && iconAfterText) {
        cellCursor.insertText(QChar::fromLatin1(' ')); // spacing between icon and text
        insertDecoration(builder, textDoc);
    }

    if (span.width() > 1 || span.height() > 1)
        textTable->mergeCells(cell.row(), cell.column(), span.height(), span.width());
}

void FillCellHelper::insertDecoration(KDReports::ReportBuilder &builder, QTextDocument &textDoc)
{
    QImage img = qvariant_cast<QImage>(cellDecoration);
    if (img.isNull()) {
        img = qvariant_cast<QIcon>(cellDecoration).pixmap(iconSize).toImage();
    }
    if (!img.isNull()) {
        static int imageNumber = 0;
        const QString name = QStringLiteral("cell-image%1.png").arg(++imageNumber);
        textDoc.addResource(QTextDocument::ImageResource, QUrl(name), img);
        builder.currentDocumentData().addResourceName(name);
        cellCursor.insertImage(name);
    }
}

QString FillCellHelper::displayText(const QVariant &value)
{
    QLocale locale; // in QStyledItemDelegate this is configurable, it comes from QWidget::locale()...
    QString text;
    switch (value.userType()) {
    case QMetaType::Float:
    case QVariant::Double:
        text = locale.toString(value.toReal());
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        text = locale.toString(value.toLongLong());
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        text = locale.toString(value.toULongLong());
        break;
    case QVariant::Date:
        text = locale.toString(value.toDate(), QLocale::ShortFormat);
        break;
    case QVariant::Time:
        text = locale.toString(value.toTime(), QLocale::ShortFormat);
        break;
    case QVariant::DateTime:
        text = locale.toString(value.toDateTime().date(), QLocale::ShortFormat);
        text += QLatin1Char(' ');
        text += locale.toString(value.toDateTime().time(), QLocale::ShortFormat);
        break;
    default:
        text = value.toString();
        break;
    }
    return text;
}

////

KDReports::AutoTableElement::AutoTableElement(QAbstractItemModel *tableModel)
    : d(new AutoTableElementPrivate)
{
    d->m_tableModel = tableModel;
}

KDReports::AutoTableElement::AutoTableElement(const QString &modelKey)
    : d(new AutoTableElementPrivate)
{
    d->m_tableModel = KDReports::modelForKey(modelKey);
}

KDReports::AutoTableElement::AutoTableElement(const AutoTableElement &other)
    : AbstractTableElement(other)
    , d(new AutoTableElementPrivate(*other.d))
{
}

KDReports::AutoTableElement &KDReports::AutoTableElement::operator=(const AutoTableElement &other)
{
    if (&other == this)
        return *this;
    AbstractTableElement::operator=(other);
    *d = *other.d;
    return *this;
}

KDReports::AutoTableElement::~AutoTableElement()
{
}

void KDReports::AutoTableElementPrivate::fillCellFromHeaderData(int section, Qt::Orientation orientation, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable,
                                                                ReportBuilder &builder) const
{
    FillCellHelper helper(m_tableModel, section, orientation, m_iconSize);
    helper.fill(textTable, builder, textDoc, cell);
}

QSize KDReports::AutoTableElementPrivate::fillTableCell(int row, int column, QTextTableCell &cell, QTextDocument &textDoc, QTextTable *textTable, ReportBuilder &builder) const
{
    const QModelIndex index = m_tableModel->index(row, column);
    const QSize span = m_tableModel->span(index);
    FillCellHelper helper(m_tableModel, index, span, m_iconSize);
    helper.fill(textTable, builder, textDoc, cell);
    return span;
}

void KDReports::AutoTableElement::build(ReportBuilder &builder) const
{
    if (!d->m_tableModel) {
        return;
    }
    QTextDocument &textDoc = builder.currentDocument();
    QTextCursor &textDocCursor = builder.cursor();
    textDocCursor.beginEditBlock();

    QTextTableFormat tableFormat;
    const int headerRowCount = d->m_horizontalHeaderVisible ? 1 : 0;
    const int headerColumnCount = d->m_verticalHeaderVisible ? 1 : 0;
    tableFormat.setHeaderRowCount(headerRowCount);
    tableFormat.setProperty(KDReports::HeaderColumnsProperty, headerColumnCount);

    tableFormat.setAlignment(textDocCursor.blockFormat().alignment());
    fillTableFormat(tableFormat, textDocCursor);

    while (d->m_tableModel->canFetchMore(QModelIndex()))
        d->m_tableModel->fetchMore(QModelIndex());

    const int rows = d->m_tableModel->rowCount();
    const int columns = d->m_tableModel->columnCount();

    QTextTable *textTable = textDocCursor.insertTable(rows + headerRowCount, columns + headerColumnCount, tableFormat);

    QTextCharFormat tableHeaderFormat;
    tableHeaderFormat.setBackground(d->m_headerBackground);
    // qDebug( "rows = %d, columns = %d", textTable->rows(), textTable->columns() );

    if (d->m_horizontalHeaderVisible) {
        for (int column = 0; column < columns; column++) {
            QTextTableCell cell = textTable->cellAt(0, column + headerColumnCount);
            Q_ASSERT(cell.isValid());
            cell.setFormat(tableHeaderFormat);
            d->fillCellFromHeaderData(column, Qt::Horizontal, cell, textDoc, textTable, builder);
        }
    }

    if (d->m_verticalHeaderVisible) {
        for (int row = 0; row < rows; row++) {
            QTextTableCell cell = textTable->cellAt(row + headerRowCount, 0);
            Q_ASSERT(cell.isValid());
            cell.setFormat(tableHeaderFormat);
            d->fillCellFromHeaderData(row, Qt::Vertical, cell, textDoc, textTable, builder);
        }
    }

    QVector<QBitArray> coveredCells;
    coveredCells.resize(rows);
    for (int row = 0; row < rows; row++)
        coveredCells[row].resize(columns);

    // The normal data
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            if (coveredCells[row].testBit(column))
                continue;
            QTextTableCell cell = textTable->cellAt(row + headerRowCount, column + headerColumnCount);
            Q_ASSERT(cell.isValid());
            const QSize span = d->fillTableCell(row, column, cell, textDoc, textTable, builder);
            if (span.isValid()) {
                for (int r = row; r < row + span.height() && r < rows; ++r) {
                    for (int c = column; c < column + span.width() && c < columns; ++c) {
                        coveredCells[r].setBit(c);
                    }
                }
            }
        }
    }

    textDocCursor.movePosition(QTextCursor::End);
    textDocCursor.endEditBlock();

    builder.currentDocumentData().registerAutoTable(textTable, this);
}

KDReports::Element *KDReports::AutoTableElement::clone() const
{
    // never used at the moment
    return new AutoTableElement(*this);
}

void KDReports::AutoTableElement::setVerticalHeaderVisible(bool visible)
{
    d->m_verticalHeaderVisible = visible;
}

void KDReports::AutoTableElement::setHorizontalHeaderVisible(bool visible)
{
    d->m_horizontalHeaderVisible = visible;
}

void KDReports::AutoTableElement::setHeaderBackground(const QBrush &brush)
{
    d->m_headerBackground = brush;
}

bool KDReports::AutoTableElement::isVerticalHeaderVisible() const
{
    return d->m_verticalHeaderVisible;
}

bool KDReports::AutoTableElement::isHorizontalHeaderVisible() const
{
    return d->m_horizontalHeaderVisible;
}

void KDReports::AutoTableElement::setIconSize(QSize iconSize)
{
    d->m_iconSize = iconSize;
}

QSize KDReports::AutoTableElement::iconSize() const
{
    return d->m_iconSize;
}

QAbstractItemModel *KDReports::AutoTableElement::tableModel() const
{
    return d->m_tableModel;
}

void KDReports::AutoTableElement::setTableModel(QAbstractItemModel *tableModel)
{
    d->m_tableModel = tableModel;
}

void KDReports::AutoTableElement::setModelKey(const QString &modelKey)
{
    d->m_tableModel = KDReports::modelForKey(modelKey);
}

QBrush KDReports::AutoTableElement::headerBackground() const
{
    return d->m_headerBackground;
}
