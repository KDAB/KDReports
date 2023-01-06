/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <KDReports>
#include <KDReportsReport_p.h>
#include <KDReportsTextDocument_p.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTest>
#include <QTextTableCell>

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
    void testSimplePagination()
    {
        Report report;
        TextElement elem(QStringLiteral("foo"));
        for (int i = 0; i < 100; ++i) {
            report.addElement(elem);
        }
        QVERIFY(report.numberOfPages() > 1);
    }

    void testPageBreak()
    {
        Report report;
        report.addElement(KDReports::TextElement("First page"));
        report.addPageBreak();
        report.addElement(KDReports::TextElement("Second page"));
        QCOMPARE(report.numberOfPages(), 2);
        report.addPageBreak();
        QCOMPARE(report.numberOfPages(), 3);
        report.addPageBreak();
        QCOMPARE(report.numberOfPages(), 3); // yep, it's a flag, can only be set once; documented.
        report.addElement(KDReports::TextElement(QString()));
        report.addPageBreak();
        QCOMPARE(report.numberOfPages(), 4);
    }

    void testOrientation()
    {
        Report report;
        report.setPageSize(QPageSize::A5);
        report.addElement(KDReports::TextElement("Foo"));
        QCOMPARE(report.numberOfPages(), 1);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCOMPARE(report.pageSize(), QPrinter::A5);
#else
        QCOMPARE(report.pageSize().id(), QPageSize::A5);
#endif
        QCOMPARE(report.pageOrientation(), QPageLayout::Portrait);
        QSizeF paperSize = report.paperSize();
        report.setPageOrientation(QPageLayout::Landscape);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCOMPARE(report.pageSize(), QPrinter::A5);
#else
        QCOMPARE(report.pageSize().id(), QPageSize::A5);
#endif

        QCOMPARE(report.pageOrientation(), QPageLayout::Landscape);
        paperSize.transpose(); // swap width and height
        QCOMPARE(report.paperSize(), paperSize);
    }

    void testEndlessPrinter()
    {
        Report report;
        TextElement elem(QStringLiteral("foo"));
        for (int i = 0; i < 400; ++i) {
            report.addElement(elem);
        }
        report.setWidthForEndlessPrinter(114.0);
        QCOMPARE(report.numberOfPages(), 1);
        // Switching back to normal pagination
        report.setWidthForEndlessPrinter(0);
        report.setPageSize(QPageSize::A6);
        QVERIFY(report.numberOfPages() > 1);
    }

    void testEndlessPrinterWithPageBreak()
    {
        Report report;
        TextElement elem(QStringLiteral("foo"));
        for (int i = 0; i < 200; ++i) {
            report.addElement(elem);
        }
        report.addPageBreak();
        report.addElement(elem);
        report.setWidthForEndlessPrinter(114.0);
        QCOMPARE(report.numberOfPages(), 1);
    }

    void testEndlessPrinterBug()
    {
        KDReports::Report report;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:");
        QVERIFY(db.open());

        // fill the DB with some test data
        QSqlQuery query;
        query.exec("create table airlines (id int primary key, "
                   "name varchar(20), homecountry varchar(2))");
        for (int i = 0; i < 500; i++)
            query.exec(QString("insert into airlines values(%0, 'Test%0', 'T%0')").arg(i));

        QSqlTableModel tableModel(nullptr, db);
        tableModel.setTable("airlines");
        tableModel.select();

        KDReports::AutoTableElement tableElement(&tableModel);
        tableElement.setVerticalHeaderVisible(false);
        report.addElement(tableElement);

        report.setWidthForEndlessPrinter(105.0);

        QCOMPARE(report.numberOfPages(), 1);
    }

    void testTableFont()
    {
        KDReports::Report report;
        int rows = 2;
        int columns = 2;
        const QString cellText = QStringLiteral("HELLO WORLD table %1x%2").arg(rows).arg(columns);
        TableElement tableElement;
        tableElement.setDefaultFont(QFont("Arial", 11));
        tableElement.setBorder(1);
        for (int row = 0; row < rows; ++row) {
            for (int column = 0; column < columns; ++column) {
                tableElement.cell(0, column).addElement(KDReports::TextElement(cellText));
            }
        }
        report.addElement(tableElement);
        // trigger a layout
        QCOMPARE(report.numberOfPages(), 1);
        QTextCursor c(report.mainTextDocument());
        c.movePosition(QTextCursor::NextCharacter);
        QTextTable *table = c.currentTable();
        QVERIFY(table);

        QTextTableCell firstCell = table->cellAt(0, 0);
        QVERIFY(firstCell.isValid());
        QTextCursor cc = firstCell.firstCursorPosition();
        QCOMPARE(cc.charFormat().font().pointSize(), 11);
    }

#if 0 // not available in WordProcessingMode anymore
    void testScaleNoTables()
    {
        Report report;
        report.setDefaultFont( QFont( QLatin1String( "Helvetica" ), 48 ) );
        TextElement elem( QString::fromLatin1( "foo" ) );
        while ( report.numberOfPages() < 3 ) {
            report.addElement( elem );
        }
        QCOMPARE( report.numberOfPages(), 3 );
        report.scaleTo( 0 /*doesn't make sense, but scaleTo corrects it*/, 2 );
        QCOMPARE( report.numberOfPages(), 2 );
        report.scaleTo( 1, 2 );
        QCOMPARE( report.numberOfPages(), 2 );
        report.scaleTo( 2, 2 );
        QCOMPARE( report.numberOfPages(), 2 ); // there are no tables, therefore it must fit in 2 pages, can't use the other 2.
        report.scaleTo( 1, 1 );
        QCOMPARE( report.numberOfPages(), 1 );
        report.scaleTo( 1, 0 /*no max limit*/ );
        QCOMPARE( report.numberOfPages(), 3 );

        // Now using font scaling directly.
        QCOMPARE( report.fontScalingFactor(), 1.0 );
        report.setTableBreakingEnabled( false );
        report.setFontScalingFactor( 0.1 );
        QCOMPARE( report.fontScalingFactor(), 0.1 );
        QCOMPARE( report.numberOfPages(), 1 );
    }
#endif
};

QTEST_MAIN(Test) // Report needs QPrinter needs a QApplication

#include "PageLayout.moc"
