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
    KDReports::Header& header = report.header( KDReports::FirstPage );
    header.addElement( KDReports::TextElement( "A tall image made to fit into the page" ) );

    KDReports::Header& header2 = report.header( KDReports::EvenPages );
    header2.addElement( KDReports::TextElement( "A wide image made to fit into the page" ) );

    KDReports::Footer& footer = report.footer();
    footer.addElement( KDReports::TextElement( "Page" ), Qt::AlignRight );
    footer.addInlineElement( KDReports::TextElement( " " ) );
    footer.addVariable( KDReports::PageNumber );
    footer.addInlineElement( KDReports::TextElement( "/" ) );
    footer.addVariable( KDReports::PageCount );

    KDReports::ImageElement imageElement( QPixmap( ":/imageTall.jpg" ) );
    imageElement.setFitToPage();
    report.addElement( imageElement );

    KDReports::ImageElement imageElement2( QPixmap( ":/image.jpg" ) );
    imageElement2.setFitToPage();
    report.addElement( imageElement2 );

    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}
