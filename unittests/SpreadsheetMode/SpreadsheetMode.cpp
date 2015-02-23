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
#include <KDReportsFontScaler_p.h>
#include <QStandardItemModel>
#ifdef Q_WS_X11
#include <QX11Info>
#endif

// Was Helvetica, but this resolves to Nimbus Sans L on many linux machines,
// and when it actually resolves to Helvetica, we get very different metrics...
static const char s_fontName[] = "Nimbus Sans L";
static bool fontFound;

using namespace KDReports;
namespace KDReports { class Test; }

#define FUZZYCOMPARE(a, b) \
 QVERIFY( a >= b - 4 ); QVERIFY( a <= b + 4 );

class KDReports::Test : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Force the DPI do the same value as on Windows
        // (mostly for testFontScaling)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qApp->setAttribute(Qt::AA_Use96Dpi, true);
#else
#ifdef Q_WS_X11
        QX11Info::setAppDpiX(0, 96);
        QX11Info::setAppDpiY(0, 96);
#endif
#endif
        QFont font( s_fontName, 48 );
        QFontInfo info( font );
        fontFound = (info.family() == s_fontName);
        qDebug() << "Setting DPI to 96. " << s_fontName << "48 has a size of" << QFontMetrics( font ).height();
        qDebug() << font << info.family();
        // Unfortunately, Qt on Mac OSX hardcodes 72 DPI, so we'll always have different results there.
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#define SKIP_IF_FONT_NOT_FOUND \
    if (!fontFound) \
        QSKIP(QString("Font %1 not found").arg(s_fontName).toLatin1(), SkipAll);
#else
#define SKIP_IF_FONT_NOT_FOUND \
    if (!fontFound) \
        QSKIP(QString("Font %1 not found").arg(s_fontName).toLatin1());
#endif

    // Tests

    void testOnePageLayout()
    {
        SKIP_IF_FONT_NOT_FOUND
        fillModel(4, 8);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 14));
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        QCOMPARE(report.numberOfPages(), 1);
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>() << QRect(0,0,4,8));
        QCOMPARE(report.mainTable()->lastAutoFontScalingFactor(), 1.0);
    }

    void testFontScaling()
    {
        SKIP_IF_FONT_NOT_FOUND

        fillModel(2, 25);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 48));
        AutoTableElement tableElement(&m_model);
        tableElement.setHorizontalHeaderVisible(false);
        tableElement.setVerticalHeaderVisible(false);
        report.mainTable()->setAutoTableElement(tableElement);
        QCOMPARE(report.numberOfPages(), 2);
        QCOMPARE(report.mainTable()->pageRects().count(), 2);

#ifndef Q_OS_MAC // Somehow the mac gets different font size for the headers, even compared to linux with DPI 72.
        const int rowsFirstPage = report.mainTable()->pageRects()[0].height();
        QVERIFY(rowsFirstPage <= 20);
        QVERIFY(rowsFirstPage >= 18);
        QCOMPARE(report.mainTable()->pageRects()[0], QRect(0,0,2,rowsFirstPage));
        QCOMPARE(report.mainTable()->pageRects()[1], QRect(0,rowsFirstPage,2,m_model.rowCount()-rowsFirstPage));

        // Now show a huge horizontal header, and check we have less rows per page
        tableElement.setHorizontalHeaderVisible(true);
        report.mainTable()->setAutoTableElement(tableElement);
        report.mainTable()->setHorizontalHeaderFont(QFont(QLatin1String(s_fontName), 48));
        QCOMPARE(report.numberOfPages(), 2);
        QVERIFY(report.mainTable()->pageRects()[0].height() < rowsFirstPage);

        // Now force a very small font scaling factor
        report.setFontScalingFactor(0.2);
        QCOMPARE(report.numberOfPages(), 1);
        QCOMPARE(report.mainTable()->lastAutoFontScalingFactor(), 0.2);
        QCOMPARE(report.mainTable()->pageRects()[0], QRect(0,0,2,m_model.rowCount()));
#endif
    }

#ifndef Q_OS_MAC // disabled on Mac due to a different DPI value. The code should be portable anyway :)
    void fontScalerShouldScaleForHeight()
    {
        SKIP_IF_FONT_NOT_FOUND

        QFont f(s_fontName);
        f.setPixelSize(86);
        FontScaler scaler(f);
        const QFontMetricsF fm(f);
        const QString text = "hello";
        const qreal initialHeight = fm.height();
        qDebug() << f << "height=" << initialHeight;
        FUZZYCOMPARE(initialHeight, 107.0); // DPI-dependent (fixed by initTestCase) and font-dependent...

        const qreal wantedHeight = 50;
        scaler.setFactorForHeight(wantedHeight);
        const qreal factor = scaler.scalingFactor();
        QVERIFY2(factor > 0.42, qPrintable(QString::number(factor)));
        QVERIFY2(factor < 0.49, qPrintable(QString::number(factor)));

        {
            const qreal scaledHeight = scaler.fontMetrics().height();
            QVERIFY2(scaledHeight <= wantedHeight, qPrintable(QString::number(scaledHeight)));
            QVERIFY2(scaledHeight > wantedHeight - 6, qPrintable(QString::number(scaledHeight)));
        }

#ifndef Q_OS_WIN
        // Testing a really small wanted height
        // Seems that on linux we can't go down to 2.7, the min is 3.0
        // Maybe because ascent=1, descent=1, height=1+1+1=3. Well, sometimes 4.
        // On Windows we can get 2, though.
        scaler.setFactorForHeight(2.9);
        QVERIFY2(scaler.fontMetrics().height() <= 4, qPrintable(QString::number(scaler.fontMetrics().height())));
#endif
    }

    void fontScalerShouldScaleForWidth()
    {
        SKIP_IF_FONT_NOT_FOUND

        QFont f(s_fontName);
        f.setPixelSize(86);
        FontScaler scaler(f);
        const QFontMetricsF fm(f);
        const QString text = "hello";
        const qreal initialWidth = fm.width(text);
        FUZZYCOMPARE(initialWidth, 182.0); // DPI-dependent (fixed by initTestCase) and font-dependent...

        const qreal wantedWidth = 50;
        const qreal wantedHFactor = wantedWidth / initialWidth; // i.e. 0.274
        scaler.setFactorForWidth(wantedHFactor, "Sample text");
        const qreal factor = scaler.scalingFactor();
        QVERIFY2(factor > 0.25, qPrintable(QString::number(factor)));
        QVERIFY2(factor < 0.29, qPrintable(QString::number(factor)));

        {
            const qreal scaledWidth = scaler.fontMetrics().width(text);
            QVERIFY2(scaledWidth <= wantedWidth, qPrintable(QString::number(scaledWidth)));
            QVERIFY2(scaledWidth > wantedWidth - 6, qPrintable(QString::number(scaledWidth)));
        }
    }

    void testFontScalerFontIssues()
    {
        QFont f(s_fontName, 8);
        FontScaler scaler(f);
        QFontMetricsF fm(f);
        const QString text = "Column 1";
        const qreal initialWidth = fm.width(text);
        const qreal wantedHFactor = 0.867968;
        const qreal expectedWidth = initialWidth * wantedHFactor;
        scaler.setFactorForWidth(wantedHFactor, text); // passing "text" here is the fix for this issue
        fm = scaler.fontMetrics();
        const qreal finalWidth = fm.width(text);
        QVERIFY2(finalWidth <= expectedWidth, QString("finalWidth=%1 bigger than expectedWidth=%2").arg(finalWidth).arg(expectedWidth).toLatin1().constData());
    }

#endif

    // Test that we don't hit an infinite loop when trying to use setFactorForWidth
    // with a very small number.
    void testFontScalerVerySmall()
    {
        QFont f(s_fontName);
        f.setPointSize(10);
        FontScaler scaler(f);
        scaler.setFactorForWidth(0.09, "Hello world");
        qreal factor = scaler.scalingFactor();
        QVERIFY( factor < 0.1 );
    }

    void testSimpleScaleTo()
    {
        SKIP_IF_FONT_NOT_FOUND

        fillModel(4, 8);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 14));
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(1, 4);
        QCOMPARE(report.numberOfPages(), 1); // it fits
        QCOMPARE(report.mainTable()->lastAutoFontScalingFactor(), 1.0);
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>() << QRect(0,0,4,8));
        report.scaleTo(4, 1);
        QCOMPARE(report.numberOfPages(), 4); // one column per page
        QCOMPARE(report.mainTable()->lastAutoFontScalingFactor(), 1.0);
        QCOMPARE(report.mainTable()->pageRects()[0], QRect(0,0,1,8));
        QCOMPARE(report.mainTable()->pageRects()[1], QRect(1,0,1,8));
        QCOMPARE(report.mainTable()->pageRects()[2], QRect(2,0,1,8));
        QCOMPARE(report.mainTable()->pageRects()[3], QRect(3,0,1,8));
    }

    void testVerticalScaling() // when scaleTo doesn't have enough room vertically
    {
        fillModel(1, 40);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(1, 2); // must cram 40 rows into 2 page vertically
        QFont font = QFont(QLatin1String(s_fontName));
        font.setPointSize(86);
        report.setDefaultFont(font);
        QCOMPARE(report.numberOfPages(), 2);
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() < 0.8);
        QCOMPARE(report.mainTable()->pageRects()[0], QRect(0,0,1,20));
        QCOMPARE(report.mainTable()->pageRects()[1], QRect(0,20,1,20));
    }

    void testHorizontalScaling()
    {
        fillModel(10, 4); // 10 columns
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(2, 1); // must fit in two pages horizontally
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 48));
        QCOMPARE(report.numberOfPages(), 2);
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() < 0.8);
        //qDebug() << report.mainTable()->pageRects();
        const int columns = report.mainTable()->pageRects()[0].width();
        Q_ASSERT(columns == 5 || columns == 6); // I get 6 on linux, but 5 on Windows
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>() << QRect(0,0,columns,4) << QRect(columns,0,10-columns,4));
    }

    void testVertAndHorizScaling()
    {
        fillModel(4, 20); // 20 rows
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(2, 1); // must cram 20 rows into 1 page vertically
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 48));
        QCOMPARE(report.numberOfPages(), 2);
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() < 0.8);
        //qDebug() << report.mainTable()->pageRects();
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>() << QRect(0,0,2,20) << QRect(2,0,2,20));
    }

    void testHorizForcedBreaking()
    {
        SKIP_IF_FONT_NOT_FOUND

        // There would be room for a single page horizontally, but the user forced 2
        fillModel(2, 10);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(2, 1);
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 48));
        QCOMPARE(report.numberOfPages(), 2);
        QCOMPARE(report.mainTable()->lastAutoFontScalingFactor(), 1.0);
        //qDebug() << report.mainTable()->pageRects();
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>() << QRect(0,0,1,10) << QRect(1,0,1,10));
    }

    void testScaleToHuge()
    {
        SKIP_IF_FONT_NOT_FOUND

        fillModel(2, 2);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(4, 4); // that's much more than we need
        // Expected result: columns are split on the 2 pages horizontally, but rows are kept together
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 48));
        QCOMPARE(report.numberOfPages(), 2);
        QCOMPARE(report.mainTable()->lastAutoFontScalingFactor(), 1.0);
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>() << QRect(0,0,1,2) << QRect(1,0,1,2));
    }

    void test4000SmallCells()
    {
        fillModel(40, 100, true /*small cells*/);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        AutoTableElement tableElement(&m_model);
        tableElement.setVerticalHeaderVisible(false);
        tableElement.setPadding(3);
        report.mainTable()->setAutoTableElement(tableElement);
        QFont font(QLatin1String("Arial"));
        font.setPixelSize(19);
        report.setDefaultFont(font);
        report.scaleTo(2, 50);
        report.mainTable()->setTableBreakingPageOrder(Report::RightThenDown);
        QCOMPARE(report.numberOfPages(), 6);
        // With point size 10, initially I was getting 0.583037. With FontScaler: 0.524504.
        // With pixel sizes: 0.478, on both machines.
        // Autobuild on linux gets 37 rows instead of 43, though (taller font, by one pixel...)
        qDebug() << report.mainTable()->lastAutoFontScalingFactor();
#ifdef Q_OS_MAC
        // Bah, fonts change too much.
        return;
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() >= 0.37);
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() <= 0.39);
#else
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() >= 0.45);
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() <= 0.50);
#endif
        const int rows = report.mainTable()->pageRects()[0].height();
        qDebug() << rows << "rows";
        QVERIFY(rows >= 37);
        QVERIFY(rows <= 48);
        const int columns = report.mainTable()->pageRects()[0].width();
        QVERIFY(columns >= 20);
        QVERIFY(columns <= 21);

        //qDebug() << report.mainTable()->pageRects();
        QCOMPARE(report.mainTable()->pageRects()[0], QRect(0,0,columns,rows));
        QCOMPARE(report.mainTable()->pageRects()[1], QRect(columns,0,40-columns,rows));
        QCOMPARE(report.mainTable()->pageRects()[2], QRect(0,rows,columns,rows));
        QCOMPARE(report.mainTable()->pageRects()[3], QRect(columns,rows,40-columns,rows));
        QCOMPARE(report.mainTable()->pageRects()[4], QRect(0,2*rows,columns,100-2*rows));
        QCOMPARE(report.mainTable()->pageRects()[5], QRect(columns,2*rows,40-columns,100-2*rows));

        report.setTableBreakingPageOrder(Report::DownThenRight);
        //qDebug() << report.mainTable()->pageRects();
        QCOMPARE(report.mainTable()->pageRects(), QList<QRect>()
                  << QRect(0,0,columns,rows)
                  << QRect(0,rows,columns,rows)
                  << QRect(0,2*rows,columns,100-2*rows)
                  << QRect(columns,0,40-columns,rows)
                  << QRect(columns,rows,40-columns,rows)
                  << QRect(columns,2*rows,40-columns,100-2*rows));
    }

    // Various tests for table-breaking with scaleTo()
    void testMaxVerticPages_data()
    {
        QTest::addColumn<int>("numColumns");
        QTest::addColumn<int>("numRows");
        QTest::addColumn<int>("maxHorizPages");
        QTest::addColumn<int>("maxVerticPages");
        QTest::addColumn<int>("minVerticPages"); // to tolerate a bit of overzealous down-scaling
        QTest::addColumn<int>("verticalHeaderFontSize");
        QTest::addColumn<int>("horizontalHeaderFontSize");

#ifdef Q_WS_X11 // Not available with Qt5
        // This test needs 72x72 DPI to actually hit the rounding problem.
        QX11Info::setAppDpiX(0, 72);
        QX11Info::setAppDpiY(0, 72);

        // We allow to go down to 2*70 pages, because asking for a font of height "2.38" and getting 2, is ok. Quite small.
        QTest::newRow("40x10000, 2*84, was a rounding problem")
            << 40 /*cols*/ << 10000 /*rows*/ << 2 << 84 /*pages, vertically*/ << 70 /*min pages, vertically*/ << 10 << 0;
#endif

        // The same thing with a more reasonable font size
        QTest::newRow("10x1000, 2*30")
            << 10 /*cols*/ << 1000 /*rows*/ << 2 << 30 /*pages, vertically*/ << 25 /*min pages, vertically*/ << 10 << 0;
        QTest::newRow("6x1000, 1*35, large font in vHeader")
            << 6  /*cols*/ << 1000  /*rows*/ << 1 << 35 /*pages*/ << 34 <<  30 << 8;
        QTest::newRow("8x10000, 2*50")
            << 8 /*cols*/ << 10000 /*rows*/ << 2 << 50 /*pages*/ << 44 <<  10 << 0;
    }

    void testMaxVerticPages()
    {
        QFETCH(int, numColumns);
        QFETCH(int, numRows);
        QFETCH(int, maxHorizPages);
        QFETCH(int, maxVerticPages);
        QFETCH(int, minVerticPages);
        QFETCH(int, verticalHeaderFontSize);
        QFETCH(int, horizontalHeaderFontSize);
        fillModel(numColumns, numRows, true /*small cells*/);
        for (int i = 0; i < numRows; ++i) {
            m_model.setVerticalHeaderItem(i, new QStandardItem(QString::fromLatin1("Row_%1").arg(i)));
        }
        Report report;
        report.setReportMode(Report::SpreadSheet);
        const QString fontName = QString::fromLatin1("Arial");
        QFont font(fontName);
        font.setPointSize(10);
        report.setDefaultFont(font);
        report.scaleTo(maxHorizPages, maxVerticPages);
        AutoTableElement tableElement(&m_model);
        tableElement.setHorizontalHeaderVisible(horizontalHeaderFontSize > 0);
        tableElement.setVerticalHeaderVisible(verticalHeaderFontSize > 0);
        tableElement.setPadding(3);
        report.mainTable()->setAutoTableElement(tableElement);
        if (horizontalHeaderFontSize > 0)
            report.mainTable()->setHorizontalHeaderFont(QFont(fontName, horizontalHeaderFontSize));
        if (verticalHeaderFontSize > 0) {
            QFont vHeaderFont(fontName, verticalHeaderFontSize);
            qDebug() << "vHeaderFont:" << vHeaderFont << QFontInfo( vHeaderFont ).family() << QFontMetricsF( vHeaderFont ).height();
            report.mainTable()->setVerticalHeaderFont(vHeaderFont);
        }
        report.setTableBreakingPageOrder(Report::RightThenDown);
        qDebug() << "numberOfPages=" << report.numberOfPages();
        // If this is >, then KDReports is not respecting the scaleTo API.
        QVERIFY(report.numberOfPages() <= maxHorizPages*maxVerticPages);

#ifndef Q_OS_WIN
        qDebug() << maxHorizPages << "*" << minVerticPages << "=" << maxHorizPages*minVerticPages;
        // If the DPI is known, we can even check that we're not down-scaling too much.
        QVERIFY(report.numberOfPages() >= maxHorizPages*minVerticPages);
#else
        Q_UNUSED(minVerticPages);
#endif
    }

    void testVerticalHeader()
    {
        // Check that the font for vertical headers is taken into account
        // in the layouting.
        // For that, we make it huge, and we check that the report takes two pages.
        fillModel(4, 20);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 8));
        report.mainTable()->setVerticalHeaderFont(QFont(QLatin1String("courier"), 30));
        AutoTableElement tableElement(&m_model);
        tableElement.setVerticalHeaderVisible(true);
        tableElement.setPadding(3);
        report.mainTable()->setAutoTableElement(tableElement);
        QCOMPARE(report.numberOfPages(), 2);
    }

    void testWithHeaders()
    {
        fillModel(4, 20);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.setDefaultFont(QFont(QLatin1String(s_fontName), 8));
        // A page header with a huge font
        KDReports::Header& header = report.header( KDReports::FirstPage );
        header.setDefaultFont(QFont(QLatin1String("courier"), 40));
        header.addElement( KDReports::TextElement( "This is the page header.\nIt's big.\nVery\nbig.\nHuge." ) );
        AutoTableElement tableElement(&m_model);
        tableElement.setVerticalHeaderVisible(true);
        tableElement.setPadding(3);
        report.mainTable()->setAutoTableElement(tableElement);
        //report.exportToFile( "testWithHeaders.pdf" ); // for debugging
#ifndef Q_OS_MAC
        QCOMPARE(report.numberOfPages(), 2);
#endif
    }

    void testScaleTables()
    {
        qWarning("test temporarily disabled");
#if 0
        // Note that this font is huge; each column is in fact larger than the page,
        // so it wraps. Interesting testcase :)
        QFont defaultFont( QLatin1String( s_fontName ), 48 );
        int columns = 0;
        int numOfPages = 0;
        do {
            // Add tables with as any columns as necessary to span 3 pages horizontally
            // The API doesn't allow to keep adding cells to a single table, so we
            // try this into a new report every time
            // TODO: can be done with an autotable now
            ++columns;
            Report testReport;
            testReport.setReportMode(Report::SpreadSheet);
            testReport.setDefaultFont( defaultFont );
            testReport.setTableBreakingEnabled( true );
            addTable( testReport, 1, columns );
            numOfPages = testReport.numberOfPages();
        } while ( numOfPages < 3 );

        //qDebug() << "The table has 1 row and" << columns << "columns.";
        QVERIFY( columns >= 3 );
        {
            Report report;
            report.setDefaultFont( defaultFont );
            report.setTableBreakingEnabled( true );
            addTable( report, 1, columns );
            QCOMPARE( report.numberOfPages(), 3 );
            // Right now we're 1 | 2 | 3, let's try scaling down to 1 | 2
            report.scaleTo( 2, 1 );
            //report.exportToFile( "2-1.pdf" ); // for debugging
            QCOMPARE( report.numberOfPages(), 2 );
            report.scaleTo( 2, 10 /* doesn't matter */ );
            //report.exportToFile( "2-10.pdf" ); // for debugging
            QCOMPARE( report.numberOfPages(), 2 );
            report.scaleTo( 3, 1 ); // back to orig
            QCOMPARE( report.numberOfPages(), 3 );
            report.scaleTo( 1, 1 ); // squeeze it into a single page
            QCOMPARE( report.numberOfPages(), 1 );
            report.scaleTo( 1, 10 ); // squeeze it into a single page
            // ## how to check that all columns are visible, and not truncated?
            QCOMPARE( report.numberOfPages(), 1 );
        }
        // Now add tables with more rows
        int rows = 0;
        do {
            ++rows;
            Report testReport;
            testReport.setDefaultFont( defaultFont );
            testReport.setTableBreakingEnabled( true );
            addTable( testReport, rows, columns );
            numOfPages = testReport.numberOfPages();
        } while ( numOfPages < 9 );
        //qDebug() << "Making report with a big table: " << rows << "rows and" << columns << "columns.";
        {
            Report report;
            report.setDefaultFont( defaultFont );
            report.scaleTo( 3, 3 );
            addTable( report, rows, columns );
            //report.exportToFile( "3-3.pdf" ); // for debugging
            // So now we have a table big enough for 3x3
            QCOMPARE( report.numberOfPages(), 9 );
        }
#endif
    }

    void testExportAsSinglePage()
    {
        // Same testcase as testVerticalScaling, for now
        fillModel(1, 40);
        Report report;
        report.setReportMode(Report::SpreadSheet);
        report.mainTable()->setAutoTableElement(AutoTableElement(&m_model));
        report.scaleTo(1, 2); // must cram 40 rows into 2 page vertically
        QFont font = QFont(QLatin1String(s_fontName));
        font.setPointSize(86); // huge :)
        report.setDefaultFont(font);
        //QCOMPARE(report.numberOfPages(), 2);

        //report.exportToFile( "testExportAsSinglePage.pdf" ); // for debugging

        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        const QString filename = tempFile.fileName();
        tempFile.close();
        const QSize size(1000, 2000);
        bool ok = report.exportToImage( size, filename, "PNG" );
        QVERIFY(ok);
        QVERIFY(QFile::exists(filename));
        QPixmap pix;
        QVERIFY(pix.load(filename));
        QCOMPARE(pix.size(), size);

        QCOMPARE(report.mainTable()->pageRects()[0], QRect(0,0,1,40));
        QVERIFY(report.mainTable()->lastAutoFontScalingFactor() > 0.9999);
        // The only way to truly validate that it worked, though, is to open test-export.jpg and check...

        QFile::remove(filename);
    }

    void testBreakSimpleTable() // No constraints, no known number of pages. Not so "simple".
    {
        qWarning("temporarily disabled");
#if 0
        Report report;
        report.setTableBreakingEnabled( true );
        makeSimpleTable( report );
        //report.exportToFile( "testBreakSimpleTable.pdf" ); // for debugging
        QCOMPARE( report.numberOfPages(), 3 ); // Was and should be 4. Now 3 with the new algo, see TODO in breakTables.
#endif
    }

private:
    void fillModel(int width, int height, bool smallCells = false)
    {
        m_model.clear();
        m_model.setColumnCount(width);
        m_model.setRowCount(height);
        for (int row = 0; row < height; ++row) {
            for (int column = 0; column < width; ++column) {
                QString text;
                if (smallCells)
                    text = QString::number(column + width * row + 1);
                else
                    text = "My number is " + QString::number(column) + ',' + QString::number(row);
                m_model.setItem(row, column, new QStandardItem(text));
            }
        }
    }
    QStandardItemModel m_model;
};

QTEST_MAIN(Test) // Report needs QPrinter needs a QApplication

#include "SpreadsheetMode.moc"
