/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <QApplication>
#include <QIdentityProxyModel>
#include <QStandardItemModel>
#include <QTableView>

#include <KDReports>
#include <QDebug>

// #define USE_CUSTOM_ROLES

class ProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    using QIdentityProxyModel::QIdentityProxyModel;
    QSize span(const QModelIndex &index) const override
    {
        // Row 2 Column 4 should span over 3 rows and 4 columns.
        if (index.row() == 1 && index.column() == 3) {
            return QSize(4, 3);
        }
        return QSize();
    }
    QVariant data(const QModelIndex &proxyIndex, int role) const override
    {
        if (proxyIndex.row() == 1 && proxyIndex.column() == 3 && role == Qt::DisplayRole)
            return "This cell spans multiple columns!";
        return QIdentityProxyModel::data(proxyIndex, role);
    }
};

class MyReportGenerator
{
public:
    KDReports::AutoTableElement largeAutoTable();

private:
    QStandardItemModel m_model;
    ProxyModel m_proxyModel;
};

KDReports::AutoTableElement MyReportGenerator::largeAutoTable()
{
#ifdef USE_CUSTOM_ROLES
    QColor colorRed(119, 112, 112);
    QColor colorGreen(112, 192, 112);
    QColor colorBlue(112, 112, 192);
    QPixmap pix(16, 16);
    pix.fill(Qt::green);
#endif

    const int numColumns = 10;
    const int numRows = 300;

    for (int column = 0; column < numColumns; ++column) {
        for (int row = 0; row < numRows; ++row) {
            const QString text = QString::number(column + numColumns * row + 1);
            auto *item = new QStandardItem(text);
#ifdef USE_CUSTOM_ROLES
            if (row == 2) {
                item->setForeground(colorRed);
                item->setBackground(colorBlue);
                item->setTextAlignment(Qt::AlignRight);
            }
            if (row >= 5) {
                item->setIcon(QIcon(pix));
                if ((row % 2) == 0) {
                    item->setData(Qt::AlignRight, KDReports::AutoTableElement::DecorationAlignmentRole);
                }
                if ((row % 3) == 0) {
                    item->setTextAlignment(Qt::AlignRight);
                }
            }
#endif
            m_model.setItem(row, column, item);
        }
    }

    // Horizontal header
    for (int i = 0; i < numColumns; ++i) {
        auto *item = new QStandardItem(QString("Column %1").arg(i + 1));
#ifdef USE_CUSTOM_ROLES
        item->setIcon(QIcon(pix));
#endif
        m_model.setHorizontalHeaderItem(i, item);
    }

    // Vertical header
    for (int i = 0; i < numRows; ++i) {
        auto *item = new QStandardItem(QString("Row %1").arg(i + 1));
#ifdef USE_CUSTOM_ROLES
        item->setIcon(QIcon(pix));
#endif
        m_model.setVerticalHeaderItem(i, item);
    }

    m_proxyModel.setSourceModel(&m_model);
    KDReports::AutoTableElement tableElement(&m_proxyModel);
    tableElement.setHorizontalHeaderVisible(true);
    tableElement.setVerticalHeaderVisible(true);
    tableElement.setPadding(2);
    tableElement.setIconSize(QSize(16, 16));
    // tableElement.setBorder( 1 );
    return tableElement;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KDReports::Report report;
    report.setReportMode(KDReports::Report::SpreadSheet);
    report.setDefaultFont(QFont("Arial", 8));
    report.setTableBreakingPageOrder(KDReports::Report::RightThenDown);
    // report.scaleTo(1, 10);

    report.setFirstPageNumber(100); // for testing this feature

    KDReports::Header &header = report.header(KDReports::FirstPage);
    header.addElement(KDReports::TextElement("Here is a long table."));

    KDReports::Footer &footer = report.footer();
    footer.addElement(KDReports::TextElement("Page"), Qt::AlignRight);
    footer.addInlineElement(KDReports::TextElement(" "));
    footer.addVariable(KDReports::PageNumber);
    footer.addInlineElement(KDReports::TextElement(" (starting at 100, for testing)"));
    // footer.addInlineElement( KDReports::TextElement( "/" ) );
    // footer.addVariable( KDReports::PageCount );

    MyReportGenerator generator;
    KDReports::AutoTableElement largeTableElement = generator.largeAutoTable();

    // This example shows the importance of a large enough font initially.
    // The scaling to 1-page-wide will make sure everything fits anyway, but it will
    // look much better when starting with a font of size 12 or 18 than with a font of size 8.
    // Well, the difference is that it reduces the padding around the text, in proportion.
    // So effectively another solution for better looking text (with many columns) is to reduce the padding.
    // To see what I mean, try what happens in this example with setPadding(3) instead of 2 above,
    // and a default font of 8...
    largeTableElement.setDefaultFont(QFont("Arial", 12));

    report.mainTable()->setAutoTableElement(largeTableElement);

    KDReports::PreviewDialog preview(&report);
    return preview.exec();

    // For performance testing...
    // report.exportToFile( "testout.pdf" );
    // return 0;
}

#include "SpreadsheetAutoTable.moc"
