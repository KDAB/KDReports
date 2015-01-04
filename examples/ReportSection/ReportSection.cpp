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

#include <QApplication>

#include <KDReports>
#include <QDebug>

static const int numColumns = 8;
static const int numRows = 300;

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
        headerCell.addElement( KDReports::TextElement(QString("Column %1").arg(i)) );
    }

    // Vertical header
    for (int i = 1; i < numRows; ++i) {
        KDReports::Cell& headerCell = tableElement.cell( i, 0 );
        headerCell.setBackground( headerColor );
        headerCell.addElement( KDReports::TextElement(QString("Row %1").arg(i)) );
    }

    for( int column = 1; column < numColumns; ++column ) {
        for ( int row = 1; row < numRows; ++row ) {
            const QString text = QString::number((column-1) + numColumns * (row-1) + 1);
            tableElement.cell( row, column ).addElement( KDReports::TextElement( text ) );
        }
    }
    return tableElement;
}

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    KDReports::Report report;
    report.setDefaultFont( QFont( "Arial", 8 ) );

    report.setFirstPageNumber( 100 );

    KDReports::Header& header = report.header( KDReports::FirstPage );
    header.addElement( KDReports::TextElement( "This is a test for generating one section of a larger report." ) );

    KDReports::Footer& footer = report.footer();
    footer.addElement( KDReports::TextElement( "We are generating pages 100-109 out of 500." ) );
    footer.addElement( KDReports::TextElement( "Page" ), Qt::AlignRight );
    footer.addInlineElement( KDReports::TextElement( " " ) );
    footer.addVariable( KDReports::PageNumber );
    footer.addInlineElement( KDReports::TextElement( " / " ) );
    // As an example for combining reports together, we'll set the value of
    // the total page count later, when it's known.
    KDReports::TextElement pageCountElement;
    pageCountElement.setId( QLatin1String("id_page_count") );
    footer.addInlineElement( pageCountElement );

    QList<QTextOption::Tab> tabs;
    tabs.append( report.rightAlignedTab() );
    report.setTabPositions( tabs );

    KDReports::TextElement tocElement("<Table of Contents will go here>");
    tocElement.setId( QLatin1String("id_toc") );
    report.addElement( tocElement );

    report.addVerticalSpacing( 10 );

    report.setTabPositions( QList<QTextOption::Tab>() ); // for performance

    KDReports::TableElement largeTableElement = largeTable();
    report.addElement( largeTableElement );

    // Let's assume we know the total page count is 1000 now, let's set it.
    report.associateTextValue( QLatin1String("id_page_count"), QString::number(500) );

    // Fill in the table of contents
    QStringList tocList;
    tocList << QString::fromLatin1("Chapter one\tpage 1");
    tocList << QString::fromLatin1("Chapter two\tpage 3");
    tocList << QString::fromLatin1("Last chapter\tpage 6");
    QString tocText = tocList.join( QString::fromLatin1("\n") );
    report.associateTextValue( QLatin1String("id_toc"), tocText );

    KDReports::PreviewDialog preview( &report );
    return preview.exec();

    // For performance testing...
    //report.exportToFile( "testout.pdf" );
    //return 0;
}
