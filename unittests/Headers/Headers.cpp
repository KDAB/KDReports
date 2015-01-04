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

#include <QtTest/QtTest>
#include <KDReports>
#include <KDReportsTextDocument_p.h>
#include <KDReportsReport_p.h>

using namespace KDReports;
namespace KDReports { class Test; }

class KDReports::Test : public QObject {
    Q_OBJECT
private slots:
    void testNoHeaders()
    {
        Report report;
        HeaderMap& headers = report.d->m_headers;
        QVERIFY( headers.isEmpty() );
        QCOMPARE( headers.headerForPage( 1, 2 ), (Header*)0 );
        QCOMPARE( headers.headerForPage( 2, 2 ), (Header*)0 );
    }

    void testSingleHeader()
    {
        Report report;
        Header& theHeader = report.header();
        HeaderMap& headers = report.d->m_headers;
        QVERIFY( !headers.isEmpty() );
        QCOMPARE( headers.headerForPage( 1, 2 ), &theHeader );
        QCOMPARE( headers.headerForPage( 2, 2 ), &theHeader );
    }

    void testEvenOdd()
    {
        Report report;
        Header& evenHeader = report.header( EvenPages );
        Header& oddHeader = report.header( OddPages );
        HeaderMap& headers = report.d->m_headers;
        QVERIFY( !headers.isEmpty() );
        QCOMPARE( headers.headerForPage( 1, 2 ), &oddHeader );
        QCOMPARE( headers.headerForPage( 2, 2 ), &evenHeader );
        QCOMPARE( headers.headerForPage( 1, 1 ), &oddHeader );
        QCOMPARE( headers.headerForPage( 4, 4 ), &evenHeader );
    }

    void testFirstLast()
    {
        Report report;
        Header& firstHeader = report.header( FirstPage );
        HeaderMap& headers = report.d->m_headers;
        QVERIFY( !headers.isEmpty() );
        QCOMPARE( headers.headerForPage( 1, 1 ), &firstHeader );
        QCOMPARE( headers.headerForPage( 1, 2 ), &firstHeader );
        QCOMPARE( headers.headerForPage( 2, 2 ), (Header*)0 );
        Header& lastHeader = report.header( LastPage );
        QCOMPARE( headers.headerForPage( 1, 1 ), &firstHeader ); // only one page -> use first header
        QCOMPARE( headers.headerForPage( 1, 2 ), &firstHeader ); // unchanged
        QCOMPARE( headers.headerForPage( 2, 2 ), &lastHeader );
        QCOMPARE( headers.headerForPage( 2, 3 ), (Header*)0 );
        Header& normalHeader = report.header( AllPages );
        QCOMPARE( headers.headerForPage( 1, 1 ), &firstHeader ); // unchanged
        QCOMPARE( headers.headerForPage( 2, 2 ), &lastHeader ); // unchanged
        QCOMPARE( headers.headerForPage( 3, 3 ), &lastHeader );
        QCOMPARE( headers.headerForPage( 2, 3 ), &normalHeader );
    }

    void testFirstLastEvenOdd()
    {
        Report report;
        Header& firstHeader = report.header( FirstPage );
        Header& lastHeader = report.header( LastPage );
        Header& evenHeader = report.header( EvenPages );
        Header& oddHeader = report.header( OddPages );
        HeaderMap& headers = report.d->m_headers;
        QCOMPARE( headers.headerForPage( 1, 1 ), &firstHeader ); // only one page -> use first header
        QCOMPARE( headers.headerForPage( 1, 2 ), &firstHeader );
        QCOMPARE( headers.headerForPage( 2, 2 ), &lastHeader );
        QCOMPARE( headers.headerForPage( 2, 3 ), &evenHeader );
        QCOMPARE( headers.headerForPage( 3, 4 ), &oddHeader );
        QCOMPARE( headers.headerForPage( 4, 6 ), &evenHeader );
        QCOMPARE( headers.headerForPage( 5, 6 ), &oddHeader );
    }

    void testPageBreaksAndFooter()
    {
        // There was a bug where the addition of a footer would lose the page breaks
        Report report;
        const int numPages = 3;
        for (int i=0; i<numPages; ++i)
        {
            report.addElement( KDReports::TextElement( "foo" ) );
            if( i+1 != numPages )
                report.addPageBreak();
        }
        QCOMPARE( report.numberOfPages(), numPages );
        //report.dump(); // should contain "page-break-after:always" twice
        KDReports::Footer& footer = report.footer( KDReports::AllPages );
        QCOMPARE( report.numberOfPages(), numPages );
        KDReports::TextElement text( "foo" );
        footer.addElement( text, Qt::AlignCenter );
        //report.dump(); // should contain "page-break-after:always" twice
        QCOMPARE( report.numberOfPages(), numPages );
    }

    void testResizableImageAndVariable()
    {
        // Test code that triggered the assert below in kdreports-1.4's PriceList example
        // ASSERT: "format.isImageFormat()" in file KDReports/KDReportsTextDocumentData.cpp, line 144
        Report report;
        KDReports::Header& header = report.header();
        QPixmap kdab( ":/kdab_small.jpg" );
        KDReports::ImageElement imageElement( kdab );
        imageElement.setWidth( 40, KDReports::Percent );
        header.addElement( imageElement );
        header.addVariable( KDReports::PageNumber );
        QCOMPARE( report.numberOfPages(), 1 );
    }

};

QTEST_MAIN(Test) // Report needs QPrinter needs a QApplication

#include "Headers.moc"
