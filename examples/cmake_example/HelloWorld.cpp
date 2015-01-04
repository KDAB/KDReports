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

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    // Add a text element for the title
    KDReports::TextElement titleElement( QObject::tr( "Hello World!" ) );
    titleElement.setPointSize( 18 );
    report.addElement( titleElement, Qt::AlignHCenter );

    // add 20 mm of vertical space:
    report.addVerticalSpacing( 20 );

    // add some more text
    KDReports::TextElement textElement( QObject::tr( "This is a report generated with KDReports" ) );
    report.addElement( textElement, Qt::AlignLeft );

    // show a print preview
    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

