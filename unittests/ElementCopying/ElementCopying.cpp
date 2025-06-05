/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <KDReportsAutoTableElement.h>
#include <KDReportsHLineElement.h>
#include <KDReportsTableElement.h>
#include <KDReportsTextElement.h>
#include <QStandardItemModel>
#include <QTest>

using namespace KDReports;

class TestCopying : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
private slots:
    void testCopyingTextElement()
    {
        TextElement elem1(QStringLiteral("foo"));
        QBrush brush(Qt::red);
        elem1.setBackground(brush);
        QCOMPARE(elem1.background(), brush);
        TextElement elem2(elem1); // copy ctor
        QCOMPARE(elem2.background(), brush);
        TextElement elem3;
        elem3 = elem2; // operator=
        QCOMPARE(elem3.background(), brush);
    }
    void testCopyingHLineElement()
    {
        HLineElement elem1;
        QColor color(Qt::red);
        elem1.setColor(color);
        QCOMPARE(elem1.color(), color);
        HLineElement elem2(elem1); // copy ctor
        QCOMPARE(elem2.color(), color);
        HLineElement elem3;
        elem3 = elem2; // operator=
        QCOMPARE(elem3.color(), color);
    }
    void testCopyingTableElement()
    {
        TableElement elem1;
        QBrush brush(Qt::red);
        elem1.setBackground(brush);
        QCOMPARE(elem1.background(), brush);
        TableElement elem2(elem1); // copy ctor
        QCOMPARE(elem2.background(), brush);
        TableElement elem3;
        elem3 = elem2; // operator=
        QCOMPARE(elem3.background(), brush);
    }
    void testCopyingAutoTableElement()
    {
        QStandardItemModel model;
        AutoTableElement elem1(&model);
        QBrush brush(Qt::red);
        elem1.setBackground(brush);
        QCOMPARE(elem1.background(), brush);
        AutoTableElement elem2(elem1); // copy ctor
        QCOMPARE(elem2.background(), brush);
        AutoTableElement elem3(&model);
        elem3 = elem2; // operator=
        QCOMPARE(elem3.background(), brush);
    }
    void testClone()
    {
        TextElement elem1(QStringLiteral("foo"));
        QBrush brush(Qt::red);
        elem1.setBackground(brush);
        TableElement elem2;
        elem2.setPadding(42);
        QBrush tableBrush(Qt::green);
        elem2.setBackground(tableBrush);
        const QList<Element *> elemList {elem1.clone(), elem2.clone()};
        Element *baseElem1 = elemList[0];
        Element *baseElem2 = elemList[1];
        QCOMPARE(baseElem1->background(), brush);
        QCOMPARE(baseElem2->background(), tableBrush);
        // check no slicing:
        QVERIFY(dynamic_cast<TextElement *>(baseElem1));
        auto *clonedTableElement = dynamic_cast<TableElement *>(baseElem2);
        QVERIFY(clonedTableElement);
        // check properties were copied
        QCOMPARE(int(clonedTableElement->padding()), 42);
        delete baseElem1;
        delete baseElem2;
    }
};

QTEST_MAIN(TestCopying)

#include "ElementCopying.moc"
