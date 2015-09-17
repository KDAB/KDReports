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
#include <QStandardItemModel>
#include <QDebug>
#include <KDReportsFrame.h>

// or simply: #include <KDReports>
#include <KDReportsReport.h>
#include <KDReportsTableElement.h>
#include <KDReportsTextElement.h>
#include <KDReportsCell.h>
#include <KDReportsPreviewDialog.h>
#include <QPrintDialog>

static qreal inchToMM(qreal inch) { return inch * 25.4; }

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    QFont font;
    font.setPointSize(10);
    report.setDefaultFont(font);

    // This example shows how to print address labels
    // on the popular Avery label size of 2.625 in x 1 in which is the white label #5160.
    // It is available as 30 labels per page and is used for addressing and mailing
    // purposes.

    // These labels fit in a Letter (8.5 in x 11 in) format like this:
    // 0.31 + 2.625 * 3 + 0.31 =~ 8.5 horizontally

    const qreal pageWidth = inchToMM(8.5);
    const qreal pageHeight = inchToMM(11);
    const qreal cellWidth = inchToMM(2.625); // = 66.675 mm
    const qreal cellHeight = inchToMM(1); // = 25.4 mm
    const int rowCount = 10;
    const int columnCount = 3;
    const qreal verticalPageMargin = (pageHeight - cellHeight * rowCount) / 2; // the 0.31 above
    const qreal horizontalPageMargin = (pageWidth - cellWidth * columnCount) / 2;
    report.setMargins( verticalPageMargin, horizontalPageMargin, verticalPageMargin, horizontalPageMargin );

    KDReports::TableElement table;
    table.setWidth( 100, KDReports::Percent );
    table.setBorder( 0 );

//    qDebug() << "cell size" << cellWidth << "x" << cellHeight << "mm";
//    qDebug() << "table width" << cellWidth * columnCount << "mm";
//    qDebug() << "table height" << cellHeight * rowCount << "mm. Page height=" << inchToMM(11);

    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {

            KDReports::Cell &cell = table.cell(row, column);

            KDReports::TextElement address;
            address  << QString::fromUtf8("Klarälvdalens Datakonsult AB\n")
                     << "Rysktorp\n"
                     << "Sweden\n";

            KDReports::Frame frame;
            //frame.setWidth( cellWidth ); // width comes from table column widths
            frame.setHeight( cellHeight );
            frame.setPadding( 0 );
            frame.setBorder( 0 );
            frame.addElement( address, Qt::AlignCenter );

            cell.addElement( frame, Qt::AlignCenter );
        }
    }

    report.addElement( table );
    report.setPageSize( QPrinter::Letter );

    // To show a print preview:
    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

