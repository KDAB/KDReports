/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <KDReportsFrame.h>
#include <QAbstractTableModel>
#include <QApplication>
#include <QDebug>

// or simply: #include <KDReports>
#include <KDReportsAutoTableElement.h>
#include <KDReportsCell.h>
#include <KDReportsMainTable.h>
#include <KDReportsPreviewDialog.h>
#include <KDReportsReport.h>
#include <KDReportsTableElement.h>
#include <KDReportsTextElement.h>
#include <QMessageBox>
#include <QPrintDialog>

static qreal inchToMM(qreal inch)
{
    return inch * 25.4;
}

class LabelModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * @brief Creates a LabelModel
     * @param cellWidth the width of the label, in mm
     * @param cellHeight the height of the label, in mm
     * @param parent the QObject parent
     */
    LabelModel(qreal cellWidth, qreal cellHeight, QObject *parent = nullptr)
        : QAbstractTableModel(parent)
        , m_cellWidth(cellWidth)
        , m_cellHeight(cellHeight)
    {
    }

    int rowCount(const QModelIndex &parent) const override
    {
        if (parent.isValid())
            return 0;
        return 10;
    }

    int columnCount(const QModelIndex &parent) const override
    {
        if (parent.isValid())
            return 0;
        return 3;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        Q_UNUSED(index);
        switch (role) {
        case Qt::DisplayRole:
            return QString::fromUtf8("Klarälvdalens Datakonsult AB\n") + "Rysktorp\n" + "Sweden\n";
        case Qt::SizeHintRole:
            return QSizeF(m_cellWidth, m_cellHeight);
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        default:
            break;
        }
        return QVariant();
    }

private:
    qreal m_cellWidth;
    qreal m_cellHeight;
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // This example shows how to print address labels
    // on the popular Avery label size of 2.625 in x 1 in which is the white label #5160.
    // It is available as 30 labels per page and is used for addressing and mailing
    // purposes.

    const qreal pageWidth = inchToMM(8.5);
    const qreal pageHeight = inchToMM(11);
    const qreal cellWidth = inchToMM(2.625); // = 66.675 mm
    const qreal cellHeight = inchToMM(1); // = 25.4 mm
    const int rowCount = 10;
    const int columnCount = 3;

    KDReports::Report report;
    report.setReportMode(KDReports::Report::SpreadSheet);
    report.setPageSize(QPageSize::Letter);
    KDReports::MainTable *mainTable = report.mainTable();

    LabelModel model(cellWidth, cellHeight);
    KDReports::AutoTableElement autoTable(&model);
    autoTable.setHorizontalHeaderVisible(false);
    autoTable.setVerticalHeaderVisible(false);
    autoTable.setBorder(0);
    mainTable->setAutoTableElement(autoTable);

    QFont font;
    font.setPointSize(10);
    report.setDefaultFont(font);

    // Margins calculation
    // These labels fit in a Letter (8.5 in x 11 in) format like this:
    // 0.31 + 2.625 * 3 + 0.31 =~ 8.5 horizontally
    const qreal verticalPageMargin = (pageHeight - cellHeight * rowCount) / 2 - 0.01;
    const qreal horizontalPageMargin = (pageWidth - cellWidth * columnCount) / 2; // the 0.31" above
    report.setMargins(verticalPageMargin, horizontalPageMargin, verticalPageMargin, horizontalPageMargin);
    report.setFixedRowHeight(cellHeight);

    //    qDebug() << "verticalPageMargin=" << verticalPageMargin;
    //    qDebug() << "cell size" << cellWidth << "x" << cellHeight << "mm";
    //    qDebug() << "table width" << cellWidth * columnCount << "mm";
    //    qDebug() << "table height" << cellHeight * rowCount << "mm. Page height=" << inchToMM(11);

    // To show a print preview:
    KDReports::PreviewDialog preview(&report);
    if (preview.exec()) {
        switch (preview.result()) {
        case KDReports::PreviewDialog::SavedSuccessfully:
            QMessageBox::information(nullptr, QString("Report saved"), QString("Success saving to %1").arg(preview.savedFileName()));
            break;
        case KDReports::PreviewDialog::SaveError:
            QMessageBox::information(nullptr, QString("Error"), QString("Error while saving to %1").arg(preview.savedFileName()));
            break;
        default:
            break;
        }
    }
    return 0;
}

#include "Labels.moc"
