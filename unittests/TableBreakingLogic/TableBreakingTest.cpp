/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <KDReportsTableBreakingLogic_p.h>
#include <QObject>
#include <QTest>

typedef QVector<int> ints;
Q_DECLARE_METATYPE(ints)
typedef QVector<qreal> qreals;
Q_DECLARE_METATYPE(qreals)

class Test : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
private slots:
    void testBreaking_data()
    {
        QTest::addColumn<qreals>("widths");
        QTest::addColumn<int>("pages");
        QTest::addColumn<ints>("expectedResult"); // columns per page
        QTest::addColumn<qreals>("expectedWidthPerPage");
        QTest::newRow("null") << qreals() << 0 << ints() << qreals();
        QTest::newRow("1 column 1 page") << (qreals() << 100) << 1 << (ints() << 1) << (qreals() << 100);
        QTest::newRow("2 columns 3 pages") << (qreals() << 100 << 100) << 3 << (ints() << 1 << 1) << (qreals() << 100 << 100);
        QTest::newRow("3 columns 3 pages") << (qreals() << 100 << 1 << 100) << 3 << (ints() << 1 << 1 << 1) << (qreals() << 100 << 1 << 100);
        QTest::newRow("3 columns 3 pages, very unequal") << (qreals() << 100 << 1 << 500)
                                                         << 3
                                                         // 1,1,1 (100,1,500) would be slightly better (minor, though).
                                                         // The huge column makes it think there's two pages worth of stuff
                                                         << (ints() << 2 << 1) << (qreals() << 101 << 500);
        QTest::newRow("complex") << (qreals() << 20 << 30 << 5 << 50 << 20)
                                 << 3
                                 // 1,2,2 gives (20,35,70)
                                 // 2,1,2 is kind of worse (50,5,70)
                                 // 2,2,1 is the best solution (50,55,20) - good :)
                                 << (ints() << 2 << 2 << 1) << (qreals() << 50 << 55 << 20);
        QTest::newRow("9 columns 3 pages") << (qreals() << 20 << 10 << 10 << 10 << 15 << 10 << 15 << 10 << 10) << 3 << (ints() << 3 << 3 << 3) << (qreals() << 40 << 35 << 35);
    }
    void testBreaking()
    {
        QFETCH(qreals, widths);
        QFETCH(int, pages);
        QFETCH(ints, expectedResult);
        QFETCH(qreals, expectedWidthPerPage);
        KDReports::TableBreakingLogic logic;
        logic.setColumnWidths(widths);
        logic.setPageCount(pages);
        const QVector<int> res = logic.columnsPerPage();
        if (res != expectedResult)
            qDebug() << "columnsPerPage:" << res << "expected" << expectedResult;
        QCOMPARE(res, expectedResult);
        const QVector<qreal> widthPerPage = logic.widthPerPage(res);
        if (widthPerPage != expectedWidthPerPage)
            qDebug() << "widthPerPage:" << widthPerPage << "expected" << expectedWidthPerPage;
        QCOMPARE(widthPerPage, expectedWidthPerPage);
    }
};

QTEST_MAIN(Test)

#include "TableBreakingTest.moc"
