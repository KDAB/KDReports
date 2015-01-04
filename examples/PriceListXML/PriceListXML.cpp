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
#include <QAbstractTableModel>
#include <QDebug>
#include <TableModel.h>

// or simply: #include <KDReports>
#include <KDReportsReport.h>
#include <KDReportsTableElement.h>
#include <KDReportsChartElement.h>
#include <KDReportsTextElement.h>
#include <KDReportsHtmlElement.h>
#include <KDReportsPreviewDialog.h>
#include <QPrintDialog>
#include <QMessageBox>
#include <QFile>

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    // Set the content of a text field - this shows how xml files can be used as templates for reports,
    // not only as complete (generated) reports.
    report.associateTextValue( "title_element", "Price list example" );
    report.associateTextValue( "company_address", QString::fromUtf8("Klarälvdalens Datakonsult AB\n"
"Rysktorp\n"
"SE-68392 Hagfors\n"
"Sweden") );
    // Note how id="table1_title" is used twice in the xml, both places get the right value
    report.associateTextValue( "table1_title", "Network Peripherals" );
    report.associateTextValue( "table2_title", "Printer Cartridges" );

    report.associateImageValue( "image_system", QPixmap( ":/system.png" ) );

    // Create two table models which will be used by one table element each.
    TableModel table1;
    table1.setDataHasVerticalHeaders( false );
    table1.loadFromCSV( ":/table1" );
    report.associateModel( QLatin1String( "table1" ), &table1 );
    TableModel table2;
    table2.setDataHasVerticalHeaders( false );
    table2.loadFromCSV( ":/table2" );
    report.associateModel( QLatin1String( "table2" ), &table2 );

    QFile reportFile( ":/PriceList.xml" );
    if( !reportFile.open( QIODevice::ReadOnly ) ) {
        QMessageBox::warning( 0, QObject::tr( "Warning" ), QObject::tr( "Could not open report description file 'PriceList.xml'. Please start this program from the PriceListXML directory." ) );
        return -1;
    }

    KDReports::ErrorDetails details;
    if( !report.loadFromXML( &reportFile, &details ) ) {
        QMessageBox::warning( 0, QObject::tr( "Warning" ), QObject::tr( "Could not parse report description file:\n%1" ).arg(details.message()) );
        reportFile.close();
        return -2;
    }

    // To show a print preview:
    KDReports::PreviewDialog preview( &report );
    return preview.exec();

    //return app.exec();
}

