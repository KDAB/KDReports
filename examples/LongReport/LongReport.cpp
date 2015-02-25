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

#include <QTableView>
#include <QStandardItemModel>
#include <QIdentityProxyModel>
#include <QApplication>

#include <KDReports>
#include <QDebug>

#define USE_AUTO_TABLE
//#define USE_CUSTOM_ROLES

QStandardItemModel model;
static const int numColumns = 10;
static const int numRows = 300;


#ifdef USE_AUTO_TABLE

class ProxyModel : public QIdentityProxyModel
{
public:
    QSize span(const QModelIndex & index) const {
        // Row 2 Column 4 should span over 3 rows and 4 columns.
        if (index.row() == 1 && index.column() == 3) {
            return QSize(4,3);
        }
        return QSize();
    }
    QVariant data(const QModelIndex &proxyIndex, int role) const {
        if (proxyIndex.row() == 1 && proxyIndex.column() == 3 && role == Qt::DisplayRole)
            return "This cell spans multiple columns!";
        return QIdentityProxyModel::data(proxyIndex, role);
    }
};
ProxyModel proxyModel;

static KDReports::AutoTableElement largeAutoTable()
{

    QColor colorRed( "#C07070" );
    QColor colorGreen( "#70C070" );
    QColor colorBlue( "#7070C0" );
    QPixmap pix( 16, 16 );
    pix.fill( Qt::green );

    for( int column = 0; column < numColumns; ++column ) {
        for ( int row = 0; row < numRows; ++row ) {
            const QString text = QString::number(column + numColumns * row + 1);
            QStandardItem* item = new QStandardItem( text );
#ifdef USE_CUSTOM_ROLES
            if ( row == 2 ) {
                item->setForeground(colorRed);
                item->setBackground(colorBlue);
                item->setTextAlignment(Qt::AlignRight);
            }
            if (row >= 5) {
                item->setIcon(QIcon(pix));
                if ((row % 2) == 0) {
                    item->setData( Qt::AlignRight, KDReports::AutoTableElement::DecorationAlignmentRole );
                }
                if ((row % 3) == 0) {
                    item->setTextAlignment(Qt::AlignRight);
                }
            }
#endif
            model.setItem( row, column,  item);
        }
    }

    // Horizontal header
    for (int i = 0; i < numColumns; ++i) {
        QStandardItem* item = new QStandardItem(QString("Column %1").arg(i+1));
#ifdef USE_CUSTOM_ROLES
        item->setIcon(QIcon(pix));
#endif
        model.setHorizontalHeaderItem( i, item );
    }

    // Vertical header
    for (int i = 0; i < numRows; ++i) {
        QStandardItem* item = new QStandardItem(QString("Row %1").arg(i+1));
#ifdef USE_CUSTOM_ROLES
        item->setIcon(QIcon(pix));
#endif
        model.setVerticalHeaderItem( i, item );
    }

    proxyModel.setSourceModel( &model );
    KDReports::AutoTableElement tableElement( &proxyModel );
    tableElement.setHorizontalHeaderVisible( true );
    tableElement.setVerticalHeaderVisible( true );
    tableElement.setPadding( 2 );
    tableElement.setIconSize( QSize( 16, 16 ) );
    //tableElement.setBorder( 1 );
    return tableElement;
}

#else // not using auto tables:

static KDReports::TableElement largeTable()
{
    KDReports::TableElement tableElement;
    tableElement.setHeaderRowCount( 1 );
    tableElement.setHeaderColumnCount( 1 );
    tableElement.setPadding( 3 );
    QColor headerColor( "#DADADA" );
    // Horizontal header
    for (int i = 1; i < numColumns; ++i) {
        KDReports::Cell& headerCell = tableElement.cell( 0, i );
        headerCell.setBackground( headerColor );
        headerCell.addElement( KDReports::TextElement(QString("Column %1").arg(i+1)) );
    }

    // Vertical header
    for (int i = 1; i < numRows; ++i) {
        KDReports::Cell& headerCell = tableElement.cell( i, 0 );
        headerCell.setBackground( headerColor );
        headerCell.addElement( KDReports::TextElement(QString("Row %1").arg(i+1)) );
    }

    for( int column = 1; column < numColumns; ++column ) {
        for ( int row = 1; row < numRows; ++row ) {
            const QString text = QString::number((column-1) + numColumns * (row-1) + 1);
            tableElement.cell( row, column ).addElement( KDReports::TextElement( text ) );
        }
    }
    return tableElement;
}
#endif

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    KDReports::Report report;
    report.setReportMode(KDReports::Report::SpreadSheet);
    report.setDefaultFont( QFont( "Arial", 8 ) );
    report.setTableBreakingPageOrder( KDReports::Report::RightThenDown );
    //report.scaleTo(1, 10);

    report.setFirstPageNumber( 100 ); // for testing this feature

    KDReports::Header& header = report.header( KDReports::FirstPage );
    header.addElement( KDReports::TextElement( "Here is a long table." ) );

    KDReports::Footer& footer = report.footer();
    footer.addElement( KDReports::TextElement( "Page" ), Qt::AlignRight );
    footer.addInlineElement( KDReports::TextElement( " " ) );
    footer.addVariable( KDReports::PageNumber );
    footer.addInlineElement( KDReports::TextElement( " (starting at 100, for testing)" ) );
    //footer.addInlineElement( KDReports::TextElement( "/" ) );
    //footer.addVariable( KDReports::PageCount );

#ifdef USE_AUTO_TABLE
    KDReports::AutoTableElement largeTableElement = largeAutoTable();

    // This example shows the importance of a large enough font initially.
    // The scaling to 1-page-wide will make sure everything fits anyway, but it will
    // look much better when starting with a font of size 12 or 18 than with a font of size 8.
    // Well, the difference is that it reduces the padding around the text, in proportion.
    // So effectively another solution for better looking text (with many columns) is to reduce the padding.
    // To see what I mean, try what happens in this example with setPadding(3) instead of 2 above,
    // and a default font of 8...
    largeTableElement.setDefaultFont(QFont("Arial", 12));

    report.mainTable()->setAutoTableElement(largeTableElement);
#else
    // not supported
    KDReports::TableElement largeTableElement = largeTable();
    report.mainTable()->setTableElement(largeTableElement);
#endif

    KDReports::PreviewDialog preview( &report );
    return preview.exec();

    // For performance testing...
    //report.exportToFile( "testout.pdf" );
    //return 0;
}
