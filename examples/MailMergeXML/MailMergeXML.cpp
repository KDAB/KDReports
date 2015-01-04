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
#include <QDebug>

#include <KDReportsReport.h>
#include <KDReportsTextElement.h>
#include <KDReportsPreviewDialog.h>
#include <QPrintDialog>
#include <QMessageBox>
#include <QFile>
#include <TableModel.h>

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    QFile reportFile( ":/MailMerge.xml" );
    if( !reportFile.open( QIODevice::ReadOnly ) ) {
        QMessageBox::warning(
            0, QObject::tr( "Warning" ),
            QObject::tr( "Could not open report description file 'MailMerge.xml'. "
                         "Please start this program from the HelloWorldXML directory." ) );
        return -1;
    }

    TableModel members;
    members.setDataHasVerticalHeaders( false );
    members.loadFromCSV( ":/members.csv" );
    report.associateModel( QLatin1String( "members" ), &members);
  
    for (int i = 0; i < members.rowCount(); i++ ) {
        report.setCurrentRow(&members, i);
        KDReports::ErrorDetails details;
        if( !report.loadFromXML( &reportFile, &details ) ) {
            QMessageBox::warning( 0, QObject::tr( "Warning" ), QObject::tr( "Could not parse report description file:\n%1" ).arg(details.message()) );
            reportFile.close();
            return -2;
        }
        
        if ( i < members.rowCount() - 1 )
            report.addPageBreak();

        // show a print preview:
    }
    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

