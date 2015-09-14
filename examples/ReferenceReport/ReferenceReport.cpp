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

#include <QFile>
#include "ResultModel.h"
#include <QStandardItemModel>
#include <QApplication>
#include <TableModel.h>
#ifdef HAVE_KDCHART
#include <KDChartChart>
#include <KDChartPieDiagram>
#include <KDChartPolarCoordinatePlane>
#endif
#include <KDReports.h>
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QDir>

static void fillEvenPagesHeader(KDReports::Header& evenPagesHeader)
{
    evenPagesHeader.addElement( KDReports::TextElement( "This header should be on even pages, and contain a 2x2 table with blue borders" ) );
    KDReports::TableElement table;
    table.setBorder(1); // if you see no borders in this table, upgrade to Qt >= 4.3.1
    table.setBorderBrush( Qt::blue );
    table.setHeaderRowCount(0);
    table.cell(0, 0).addInlineElement( KDReports::TextElement( "1" ) );
    table.cell(0, 1).addInlineElement( KDReports::TextElement( "2" ) );
    KDReports::Cell& cell = table.cell(1, 0);
    cell.addElement( KDReports::TextElement( "This should say 2/2: " ) );
    cell.addVariable( KDReports::PageNumber );
    cell.addInlineElement( KDReports::TextElement( "/" ) );
    cell.addVariable( KDReports::PageCount );
    evenPagesHeader.addInlineElement( table );
}

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    KDReports::Report report;
    report.setDocumentName( "Reference report" );

    report.setWatermarkPixmap( QPixmap( ":/background.jpg" ) );
    report.setWatermarkText( QString() );

    KDReports::Header& header = report.header( KDReports::FirstPage );
    QPixmap kdab( ":/kdab_small.jpg" );
    QPainter painter( &kdab );
    painter.drawRect( 0, 0, kdab.width() - 1, kdab.height() - 1 );
    KDReports::ImageElement imageElement( kdab );
    imageElement.setWidth( 50 ); // mm
    header.addElement( imageElement );
    header.addElement( KDReports::TextElement( "This header should be on the first page. The kdab logo should be 50mm wide and 24mm high." ) );

    header.addElement( KDReports::TextElement( "This should say 1/2: " ) );
    header.addVariable( KDReports::PageNumber );
    header.addInlineElement( KDReports::TextElement( "/" ) );
    header.addVariable( KDReports::PageCount );

    header.addInlineElement( KDReports::TextElement( "\t\tDate: " ) );
    header.addVariable( KDReports::DefaultLocaleLongDate );

    KDReports::Header& evenPagesHeader = report.header( KDReports::EvenPages );
    fillEvenPagesHeader(evenPagesHeader);

    KDReports::Footer& footer = report.footer();
    footer.addElement( KDReports::TextElement( "This should be right-aligned" ), Qt::AlignRight );

    KDReports::TextElement titleElement;
    titleElement << "This text is blue, bold, centered and has a point size of 14";
    titleElement.setBold( true );
    titleElement.setPointSize( 14 );
    titleElement.setTextColor( Qt::blue );
    report.addElement( titleElement, Qt::AlignHCenter );

    report.addVerticalSpacing( 15 );
    report.addElement( KDReports::TextElement( "There should be 15mm above this paragraph" ) );
    report.addElement( KDReports::TextElement( "There should be an image as watermark behind this text" ) );
    report.addElement( KDReports::HLineElement() );
    report.addElement( KDReports::TextElement( "There should be a horizontal line above this paragraph" ) );

    {
        QList<QTextOption::Tab> tabs;
        tabs.append( report.middleAlignedTab() );
        tabs.append( report.rightAlignedTab() );
        report.setTabPositions( tabs );
    }

    report.addElement( KDReports::TextElement( "Left-aligned text\tMiddle\tRight-aligned text" ) );

    report.setParagraphMargins( 30, 10, 50, 10 );
    report.addElement( KDReports::TextElement( "This paragraph has margins on all sides; especially visible when its text is long enough so that it wraps onto multiple lines."));
    report.setParagraphMargins( 0, 0, 0, 0 );

    //report.addElement( KDReports::TextElement( "Left again" ) );
    //report.addElement( KDReports::TextElement( "Right" ), Qt::AlignRight ); // Fails when done here! Must be a Qt bug ###

    report.addPageBreak();

    // Define tab positions for decimal-point alignment
    {
        QList<QTextOption::Tab> tabs;
        QTextOption::Tab tab;
        tab.position = 50; // in mm
        tab.type = QTextOption::DelimiterTab;
        tab.delimiter = QLatin1Char( '.' );
        tabs.append( tab );
        report.setTabPositions( tabs );
    }

    // table
    KDReports::TableElement tableElement;
    tableElement.setHeaderRowCount( 2 );
    tableElement.setPadding( 3 );
    QColor headerColor( "#DADADA" );
    // Merged header in row 0
    KDReports::Cell& topHeader = tableElement.cell( 0, 0 );
    topHeader.setColumnSpan( 2 );
    topHeader.setBackground( headerColor );
    topHeader.addElement( KDReports::TextElement( "This header should be gray and span over two columns" ), Qt::AlignHCenter );

    // Normal header in row 1
    KDReports::Cell& headerCell1 = tableElement.cell( 1, 0 );
    headerCell1.setBackground( headerColor );
    // This would look better if centered vertically. This feature is only available since
    // Qt-4.3 though (QTextCharFormat::AlignMiddle)
    QPixmap systemPixmap( ":/system.png" );
    headerCell1.addElement( KDReports::ImageElement( systemPixmap ) );
    headerCell1.addInlineElement( KDReports::TextElement( " Header with computer icon" ) );

    KDReports::Cell& headerCell2 = tableElement.cell( 1, 1 );
    headerCell2.setBackground( headerColor );
    KDReports::TextElement expected( "Italic with dark gray background" );
    expected.setItalic( true );
    expected.setBackground( QColor("#999999") ); // note that this background only applies to this element
    headerCell2.addElement( expected );
    headerCell2.addInlineElement( KDReports::TextElement( " Normal text" ) );

    // Data in rows 2 and 3
    tableElement.cell( 2, 0 ).addElement( KDReports::TextElement( "Price:\t250.2" ) );
    tableElement.cell( 2, 1 ).addElement( KDReports::TextElement( "Price:\t1088.5" ) );
    tableElement.cell( 3, 0 ).addElement( KDReports::TextElement( "Reduced price:\t68.52" ) );
    tableElement.cell( 3, 1 ).addElement( KDReports::TextElement( "Reduced price:\t88.584" ) );

    report.addElement( tableElement );

    report.addVerticalSpacing( 5 );


    // Auto table test

    QStandardItemModel model;
    QStandardItem* firstHeaderItem = new QStandardItem( QObject::tr( "<html>This auto-table uses <b>QStandardItemModel</b>" ) );
    firstHeaderItem->setTextAlignment( Qt::AlignCenter ); // works
    model.setHorizontalHeaderItem( 0, firstHeaderItem );
    QStandardItem* secondHeaderItem = new QStandardItem( QObject::tr( "Icon on the left in this header" ) );
    secondHeaderItem->setData( qVariantFromValue( QPixmap( ":/system.png" ) ), Qt::DecorationRole );
    //secondHeaderItem->setTextAlignment( Qt::AlignCenter ); // doesn't work. QTBUG-20041
    model.setHorizontalHeaderItem( 1, secondHeaderItem );
    QStandardItem* thirdHeaderItem = new QStandardItem( QObject::tr( "Small pixmap on the right" ) );
    thirdHeaderItem->setIcon( QPixmap( ":/system.png" ) ); // size determined by setIconSize below
    thirdHeaderItem->setData( Qt::AlignRight, KDReports::AutoTableElement::DecorationAlignmentRole );
    model.setHorizontalHeaderItem( 2, thirdHeaderItem );

    QStandardItem* firstCellItem = new QStandardItem( QObject::tr( "<html>This is <b>bold</b> text" ) );
    model.setItem( 0, 0, firstCellItem );

    QStandardItem* secondCellItem = new QStandardItem( QObject::tr( "Icon on the left in this cell" ) );
    secondCellItem->setData( qVariantFromValue( QPixmap( ":/system.png" ) ), Qt::DecorationRole );
    model.setItem( 0, 1, secondCellItem );

    QStandardItem* thirdCellItem = new QStandardItem( QObject::tr( "Small pixmap on the right" ) );
    thirdCellItem->setIcon( QPixmap( ":/system.png" ) ); // size determined by setIconSize below
    thirdCellItem->setData( Qt::AlignRight, KDReports::AutoTableElement::DecorationAlignmentRole );
    model.setItem( 0, 2, thirdCellItem );

    QStandardItem* italicItem = new QStandardItem( QObject::tr( "10 pt italic blue text" ) );
    italicItem->setFont( QFont( "Arial", 10, -1, true /*italic*/ ) );
    italicItem->setForeground( Qt::blue );
    model.setItem( 1, 0, italicItem );

    QStandardItem* yellowItem = new QStandardItem( QObject::tr( "Yellow background" ) );
    yellowItem->setBackground( Qt::yellow );
    model.setItem( 1, 1, yellowItem );

    QStandardItem* alignedItem = new QStandardItem( QObject::tr( "Right-aligned" ) );
    alignedItem->setTextAlignment( Qt::AlignRight );
    model.setItem( 1, 2, alignedItem );

    KDReports::AutoTableElement autoTable( &model );
    autoTable.setIconSize( QSize( 16, 16 ) );
    autoTable.setHeaderBackground( QBrush() );
    autoTable.setVerticalHeaderVisible( false );
    report.addElement( autoTable );

    report.addElement( KDReports::TextElement() );

    ResultModel resultModel;
    KDReports::AutoTableElement autoTableResults( &resultModel );
    autoTableResults.setVerticalHeaderVisible( false );
    report.addElement( autoTableResults );

    // Chart test
#ifdef HAVE_KDCHART
    report.addElement( KDReports::TextElement() );
    report.addElement( KDReports::TextElement("The chart below is made with KDChart:") );

    TableModel chartModel;
    chartModel.setDataHasVerticalHeaders( false );
    chartModel.loadFromCSV( ":/chart_model" );
    KDChart::Chart chart;
    chart.replaceCoordinatePlane( new KDChart::PolarCoordinatePlane( &chart ) );
    KDChart::PieDiagram* diagram = new KDChart::PieDiagram();
    diagram->setModel( &chartModel );
    chart.coordinatePlane()->replaceDiagram( diagram );
    KDReports::ChartElement chartElement( &chart );
    chartElement.setSize( 50, 50 ); // in mm
    report.addElement(chartElement);
#endif

    // Hyperlink test
    report.addElement( KDReports::HtmlElement("<a href=\"http://www.kdab.com\">click here to open http://www.kdab.com</a>") );

    //report.exportToHtml( "out.html" );

    KDReports::PreviewDialog preview( &report );
    //preview.setQuickPrinterName( "hp-LaserJet-1300" );
    preview.setDefaultSaveDirectory( QDir::homePath() );
    //preview.setDirectoryBrowsingEnabled( false );

    return preview.exec();
}
