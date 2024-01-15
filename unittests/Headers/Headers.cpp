/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <KDReports>
#include <KDReportsReport_p.h>
#include <KDReportsTextDocument_p.h>
#include <QTest>

using namespace KDReports;
namespace KDReports {
class Test;
}

class KDReports::Test : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
private slots:
    void testNoHeaders()
    {
        Report report;
        HeaderMap &headers = report.d->m_headers;
        QVERIFY(headers.isEmpty());
        QCOMPARE(headers.headerForPage(1, 2), ( Header * )nullptr);
        QCOMPARE(headers.headerForPage(2, 2), ( Header * )nullptr);
    }

    void testSingleHeader()
    {
        Report report;
        Header &theHeader = report.header();
        HeaderMap &headers = report.d->m_headers;
        QVERIFY(!headers.isEmpty());
        QCOMPARE(headers.headerForPage(1, 2), &theHeader);
        QCOMPARE(headers.headerForPage(2, 2), &theHeader);
    }

    void testEvenOdd()
    {
        Report report;
        Header &evenHeader = report.header(EvenPages);
        Header &oddHeader = report.header(OddPages);
        HeaderMap &headers = report.d->m_headers;
        QVERIFY(!headers.isEmpty());
        QCOMPARE(headers.headerForPage(1, 2), &oddHeader);
        QCOMPARE(headers.headerForPage(2, 2), &evenHeader);
        QCOMPARE(headers.headerForPage(1, 1), &oddHeader);
        QCOMPARE(headers.headerForPage(4, 4), &evenHeader);
    }

    void testFirstLast()
    {
        Report report;
        Header &firstHeader = report.header(FirstPage);
        HeaderMap &headers = report.d->m_headers;
        QVERIFY(!headers.isEmpty());
        QCOMPARE(headers.headerForPage(1, 1), &firstHeader);
        QCOMPARE(headers.headerForPage(1, 2), &firstHeader);
        QCOMPARE(headers.headerForPage(2, 2), ( Header * )nullptr);
        Header &lastHeader = report.header(LastPage);
        QCOMPARE(headers.headerForPage(1, 1), &firstHeader); // only one page -> use first header
        QCOMPARE(headers.headerForPage(1, 2), &firstHeader); // unchanged
        QCOMPARE(headers.headerForPage(2, 2), &lastHeader);
        QCOMPARE(headers.headerForPage(2, 3), ( Header * )nullptr);
        Header &normalHeader = report.header(AllPages);
        QCOMPARE(headers.headerForPage(1, 1), &firstHeader); // unchanged
        QCOMPARE(headers.headerForPage(2, 2), &lastHeader); // unchanged
        QCOMPARE(headers.headerForPage(3, 3), &lastHeader);
        QCOMPARE(headers.headerForPage(2, 3), &normalHeader);
    }

    void testFirstLastEvenOdd()
    {
        Report report;
        Header &firstHeader = report.header(FirstPage);
        Header &lastHeader = report.header(LastPage);
        Header &evenHeader = report.header(EvenPages);
        Header &oddHeader = report.header(OddPages);
        HeaderMap &headers = report.d->m_headers;
        QCOMPARE(headers.headerForPage(1, 1), &firstHeader); // only one page -> use first header
        QCOMPARE(headers.headerForPage(1, 2), &firstHeader);
        QCOMPARE(headers.headerForPage(2, 2), &lastHeader);
        QCOMPARE(headers.headerForPage(2, 3), &evenHeader);
        QCOMPARE(headers.headerForPage(3, 4), &oddHeader);
        QCOMPARE(headers.headerForPage(4, 6), &evenHeader);
        QCOMPARE(headers.headerForPage(5, 6), &oddHeader);
    }

    void testPageBreaksAndFooter()
    {
        // There was a bug where the addition of a footer would lose the page breaks
        Report report;
        const int numPages = 3;
        for (int i = 0; i < numPages; ++i) {
            report.addElement(KDReports::TextElement("foo"));
            if (i + 1 != numPages)
                report.addPageBreak();
        }
        QCOMPARE(report.numberOfPages(), numPages);
        // report.dump(); // should contain "page-break-after:always" twice
        KDReports::Footer &footer = report.footer(KDReports::AllPages);
        QCOMPARE(report.numberOfPages(), numPages);
        KDReports::TextElement text("foo");
        footer.addElement(text, Qt::AlignCenter);
        // report.dump(); // should contain "page-break-after:always" twice
        QCOMPARE(report.numberOfPages(), numPages);
    }

    void testResizableImageAndVariable()
    {
        // Test code that triggered the assert below in kdreports-1.4's PriceList example
        // ASSERT: "format.isImageFormat()" in file KDReports/KDReportsTextDocumentData.cpp, line 144
        Report report;
        KDReports::Header &header = report.header();
        QPixmap kdab(":/kdab_small.jpg");
        KDReports::ImageElement imageElement(kdab);
        imageElement.setWidth(40, KDReports::Percent);
        header.addElement(imageElement);
        header.addVariable(KDReports::PageNumber);
        QCOMPARE(report.numberOfPages(), 1);
    }
};

QTEST_MAIN(Test) // Report needs QPrinter needs a QApplication

#include "Headers.moc"
