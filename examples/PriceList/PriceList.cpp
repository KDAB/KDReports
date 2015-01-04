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
#include <TableModel.h>

#include <KDReports>
#include <QDebug>
#include <QPainter>


int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    KDReports::Report report;

    report.setHeaderBodySpacing( 10 ); // mm
    report.setFooterBodySpacing( 10 ); // mm

//     report.setWatermarkPixmap( QPixmap( ":/kdab.jpg" ) );
//     report.setWatermarkText( QString::null );

    KDReports::Header& header = report.header( KDReports::OddPages );
    QPixmap kdab( ":/kdab_small.jpg" );
    // add a border around the pixmap, mostly for debugging
    //QPainter painter( &kdab );
    //painter.drawRect( 0, 0, kdab.width() - 1, kdab.height() - 1 );
    KDReports::ImageElement imageElement( kdab );
    // The image size can be set in mm or in percent of the page width (without margins)
    // imageElement.setWidth( 50 ); // mm
    imageElement.setWidth( 40, KDReports::Percent );

    header.addElement( imageElement );
    header.addVariable( KDReports::PageNumber );
    header.addInlineElement( KDReports::TextElement( " / " ) );
    header.addVariable( KDReports::PageCount );
    header.addInlineElement( KDReports::TextElement( ", Date: " ) );
    header.addVariable( KDReports::TextDate );
    header.addInlineElement( KDReports::TextElement( ", Time: " ) );
    header.addVariable( KDReports::TextTime );

    KDReports::Header& evenPagesHeader = report.header( KDReports::EvenPages );
    evenPagesHeader.addElement( imageElement );
    evenPagesHeader.addInlineElement( KDReports::TextElement( "Even pages header: " ) );
    evenPagesHeader.addVariable( KDReports::PageNumber );
    evenPagesHeader.addInlineElement( KDReports::TextElement( " / " ) );
    evenPagesHeader.addVariable( KDReports::PageCount );

    KDReports::Footer& footer = report.footer();
    KDReports::TextElement companyAddressElement( QString::fromUtf8( "Klarälvdalens Datakonsult AB\nRysktorp\nSE-68392 Hagfors\nSweden" ) );
    footer.addElement( companyAddressElement, Qt::AlignRight );

    KDReports::TextElement titleElement;
    titleElement << "Price list example";
    titleElement.setPointSize( 18 );
    report.addElement( titleElement, Qt::AlignHCenter );

    report.addVerticalSpacing( 10 ); // 1 cm

    const QColor titleElementColor( 204, 204, 255 );

    // A text element with the title above a table
    // Note that the background color is not just behind the text (as setBackground() would do),
    // but behind the whole paragraph, up until the right margin of the page.
    KDReports::TextElement tableTitleElement( "Network Peripherals" );
    tableTitleElement.setBold( true );
    report.addElement( tableTitleElement, Qt::AlignLeft, titleElementColor );

    TableModel table1;
    table1.setDataHasVerticalHeaders( false );
    table1.loadFromCSV( ":/table1" );
    // Q_ASSERT( table1.headerData( 0, Qt::Vertical ).toString() == "10/100 Mbps switch" );
    //Q_ASSERT( table1.data( table1.index( 0, 0 ) ).toString() == "Product" );
    KDReports::AutoTableElement autoTableElement1( &table1 );
    autoTableElement1.setWidth( 100, KDReports::Percent );
    report.addElement( autoTableElement1 );

    report.addVerticalSpacing( 5 );

    // Notice how elements can be copied and modified
    // This way, we use the same font attributes for all title elements
    KDReports::TextElement tableTitleElement2 = tableTitleElement;
    tableTitleElement2.setText( "Printer Cartridges" );
    report.addElement( tableTitleElement2, Qt::AlignLeft, titleElementColor );

    TableModel table2;
    table2.setDataHasVerticalHeaders( false );
    table2.loadFromCSV( ":/table2" );
    KDReports::AutoTableElement autoTableElement2( &table2 );
    autoTableElement2.setWidth( 100, KDReports::Percent );
    report.addElement( autoTableElement2 );

    // and again, on the second page
    report.addPageBreak();
    report.addElement( tableTitleElement, Qt::AlignLeft, titleElementColor );
    report.addElement( autoTableElement1 );
    report.addVerticalSpacing( 5 );
    report.addElement( tableTitleElement2, Qt::AlignLeft, titleElementColor );
    report.addElement( autoTableElement2 );

    report.addVerticalSpacing( 5 );

    // ===========================================================================
    // Another kind of table, where the data comes from code and not from a model:
    // ===========================================================================
    KDReports::TableElement tableElement;
    tableElement.setHeaderRowCount( 2 );
    tableElement.setPadding( 3 );
    QColor headerColor( "#DADADA" );
    // Merged header in row 0
    KDReports::Cell& topHeader = tableElement.cell( 0, 0 );
    topHeader.setColumnSpan( 2 );
    topHeader.setBackground( headerColor );
    topHeader.addElement( KDReports::TextElement( "TableElement example" ), Qt::AlignHCenter );

    // Normal header in row 1
    KDReports::Cell& headerCell1 = tableElement.cell( 1, 0 );
    headerCell1.setBackground( headerColor );
    // This would look better if centered vertically. This feature is only available since
    // Qt-4.3 though (QTextCharFormat::AlignMiddle)
    QPixmap systemPixmap( ":/system.png" );
    headerCell1.addElement( KDReports::ImageElement( systemPixmap ) );
    headerCell1.addInlineElement( KDReports::TextElement( " Item" ) );
    KDReports::Cell& headerCell2 = tableElement.cell( 1, 1 );
    headerCell2.setBackground( headerColor );
    KDReports::TextElement expected( "Expected" );
    expected.setItalic( true );
    expected.setBackground( QColor("#999999") ); // note that this background only applies to this element
    headerCell2.addElement( expected );
    headerCell2.addInlineElement( KDReports::TextElement( " shipping time" ) );

    // Data in rows 2 and 3
    tableElement.cell( 2, 0 ).addElement( KDReports::TextElement( "Network Peripherals" ) );
    tableElement.cell( 2, 1 ).addElement( KDReports::TextElement( "4 days" ) );
    tableElement.cell( 3, 0 ).addElement( KDReports::TextElement( "Printer Cartridges" ) );
    tableElement.cell( 3, 1 ).addElement( KDReports::TextElement( "3 days" ) );

    report.addElement( tableElement );

    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

