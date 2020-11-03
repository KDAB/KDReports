/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#include <QtTest/QtTest>
#include <KDReportsTextElement.h>
#include <KDReportsAutoTableElement.h>
#include <KDReportsTableElement.h>
#include <KDReportsHLineElement.h>
#include <QStandardItemModel>

using namespace KDReports;

class TestCopying : public QObject {
    Q_OBJECT
private slots:
    void testCopyingTextElement()
    {
        TextElement elem1( QString::fromLatin1( "foo" ) );
        QBrush brush( Qt::red );
        elem1.setBackground( brush );
        QCOMPARE( elem1.background(), brush );
        TextElement elem2( elem1 ); // copy ctor
        QCOMPARE( elem2.background(), brush );
        TextElement elem3;
        elem3 = elem2; // operator=
        QCOMPARE( elem3.background(), brush );
    }
    void testCopyingHLineElement()
    {
        HLineElement elem1;
        QColor color( Qt::red );
        elem1.setColor( color );
        QCOMPARE( elem1.color(), color );
        HLineElement elem2( elem1 ); // copy ctor
        QCOMPARE( elem2.color(), color );
        HLineElement elem3;
        elem3 = elem2; // operator=
        QCOMPARE( elem3.color(), color );
    }
    void testCopyingTableElement()
    {
        TableElement elem1;
        QBrush brush( Qt::red );
        elem1.setBackground( brush );
        QCOMPARE( elem1.background(), brush );
        TableElement elem2( elem1 ); // copy ctor
        QCOMPARE( elem2.background(), brush );
        TableElement elem3;
        elem3 = elem2; // operator=
        QCOMPARE( elem3.background(), brush );
    }
    void testCopyingAutoTableElement()
    {
        QStandardItemModel model;
        AutoTableElement elem1( &model );
        QBrush brush( Qt::red );
        elem1.setBackground( brush );
        QCOMPARE( elem1.background(), brush );
        AutoTableElement elem2( elem1 ); // copy ctor
        QCOMPARE( elem2.background(), brush );
        AutoTableElement elem3( &model );
        elem3 = elem2; // operator=
        QCOMPARE( elem3.background(), brush );
    }
    void testClone()
    {
        TextElement elem1( QString::fromLatin1( "foo" ) );
        QBrush brush( Qt::red );
        elem1.setBackground( brush );
        TableElement elem2;
        QBrush tableBrush( Qt::green );
        elem2.setBackground( tableBrush );
        QList<Element *> elemList;
        elemList.append( elem1.clone() );
        elemList.append( elem2.clone() );
        Element* baseElem1 = elemList[0];
        Element* baseElem2 = elemList[1];
        QCOMPARE( baseElem1->background(), brush );
        QCOMPARE( baseElem2->background(), tableBrush );
        // check no slicing:
        QVERIFY( dynamic_cast<TextElement *>( baseElem1 ) );
        QVERIFY( dynamic_cast<TableElement *>( baseElem2 ) );
    }
};

QTEST_MAIN(TestCopying)

#include "ElementCopying.moc"
