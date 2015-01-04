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

#include <QTextTableCell>
#include <QtTest/QtTest>
#include <KDReports>
#include <KDReportsTextDocument_p.h>
#include <QStandardItemModel>
#include <QTextCursor>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

using namespace KDReports;
namespace KDReports { class Test; }

// copied from KDReportsHeader
static const int VariableTypeProperty = QTextFormat::UserProperty + 246;
static const int VariableLengthProperty = QTextFormat::UserProperty + 247;
static void setVariableMarker( QTextDocument& textDoc, int pos, VariableType variableType, int valueLength )
{
    QTextCursor c( &textDoc );
    c.setPosition( pos );
    bool ok = c.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
    Q_UNUSED( ok ); // in release mode
    Q_ASSERT( ok );
    QTextCharFormat charFormat;
    charFormat.setProperty( VariableTypeProperty, variableType );
    charFormat.setProperty( VariableLengthProperty, valueLength );
    c.setCharFormat( charFormat );
}

class KDReports::Test : public QObject {
    Q_OBJECT
private slots:
    void testTextDocumentInitialState()
    {
        QTextDocument textDoc;
        QTextCursor cursor( &textDoc );
        QCOMPARE( textDoc.toPlainText(), QString() );
        QCOMPARE( textDoc.blockCount(), 1 ); // there's one block upfront
        // we can set alignment on it
        QTextBlockFormat blockFormat;
        blockFormat.setAlignment( Qt::AlignRight );
        cursor.setBlockFormat( blockFormat );
        cursor.insertText( "foo" );
        QCOMPARE( textDoc.toPlainText(), QString("foo") ); // no \n
        QCOMPARE( textDoc.blockCount(), 1 );
        // the above is why ReportBuilder::addBlockElement only inserts a block
        // if the document isn't empty.
    }
    void testSetVariableMarker()
    {
        QTextDocument textDoc;
        QTextCursor c( &textDoc );
        c.insertText( "VARHERE..." );
        setVariableMarker( textDoc, 0, PageNumber, 7 );
        // Tricky, the format for char 0 is available when the cursor is at char 1,
        // since "charFormat() is the format of the character before"
        c.setPosition( 1 );
        QVERIFY( c.charFormat().hasProperty( VariableTypeProperty ) );
        QVERIFY( c.charFormat().hasProperty( VariableLengthProperty ) );
        c.setPosition( 0 );
#if QT_VERSION < 0x040300
        // This was the behavior in Qt-4.2.2, but it changed in Qt-4.3.0
        QVERIFY( !c.charFormat().hasProperty( VariableTypeProperty ) );
        QVERIFY( !c.charFormat().hasProperty( VariableLengthProperty ) );
#else
        QVERIFY( c.charFormat().hasProperty( VariableTypeProperty ) );
        QVERIFY( c.charFormat().hasProperty( VariableLengthProperty ) );
#endif
    }
    void testEmptyVariable()
    {
        QTextDocument textDoc;
        QTextCursor c( &textDoc );
        c.insertText( "BEFORE" );
        const int charPosition = c.position() - 1; // works with -1, but not without it (can't move right)
        setVariableMarker( textDoc, charPosition, PageNumber, 0 );
        c.insertText( "AFTER" );
        // Tricky, the format for char 0 is available when the cursor is at char 1,
        // since "charFormat() is the format of the character before"
        c.setPosition( charPosition + 1 );
        QVERIFY( c.charFormat().hasProperty( VariableTypeProperty ) );
        QVERIFY( c.charFormat().hasProperty( VariableLengthProperty ) );
    }
    void testUpdatePercentSizes()
    {
        QTextDocument textDoc;
        QTextCursor c( &textDoc );
        c.insertText( "test" );
        c.setPosition( 1 );
        c.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
        QTextCharFormat charFormat;
        charFormat.setProperty( VariableLengthProperty, 42 );
        c.setCharFormat( charFormat );
        c.movePosition( QTextCursor::NextCharacter );
        QVERIFY( c.charFormat().hasProperty( VariableLengthProperty ) );
        c.movePosition( QTextCursor::NextCharacter );
        QVERIFY( !c.charFormat().hasProperty( VariableLengthProperty ) );
    }

    void testUpdateFormats()
    {
        // How to scale all fonts in the document by a given amount? (research for TextDocument::scaleFontsBy)
        // I keep this Qt-only code here, can be useful for Qt bug reports for instance.
        QTextDocument textDoc;
        fillDocWithVariousSizes( textDoc );

        // Collect initial sizes, for comparison
        QList<qreal> initialPointSizes;
        {
            QTextCursor cursor( &textDoc );
            Q_FOREVER {
                qreal cursorFontPointSize = cursor.charFormat().fontPointSize();
                if ( cursorFontPointSize == 0 )
                    cursorFontPointSize = textDoc.defaultFont().pointSize();
                initialPointSizes << cursorFontPointSize;
                if ( cursor.atEnd() )
                    break;
                cursor.movePosition( QTextCursor::NextCharacter );
            }
        }
        //qDebug() << initialPointSizes;

        // Now scale
        {
            QTextCursor cursor( &textDoc );
            qreal currentPointSize = -1.0;
            QTextCursor lastCursor( &textDoc );
            Q_FOREVER {
                qreal cursorFontPointSize = cursor.charFormat().fontPointSize();
                //qDebug() << cursorFontPointSize << "last=" << currentPointSize << cursor.block().text() << "position=" << cursor.position();
                if ( cursorFontPointSize != currentPointSize ) {
                    if ( currentPointSize != -1.0 ) {
                        setFontSizeHelper( lastCursor, cursor.position() - 1, currentPointSize, 1.2 );
                        lastCursor.setPosition( cursor.position() - 1, QTextCursor::MoveAnchor );
                    }
                    currentPointSize = cursorFontPointSize;
                }
                if ( cursor.atEnd() )
                    break;
                cursor.movePosition( QTextCursor::NextCharacter );
            }
            if ( currentPointSize != -1.0 ) {
                setFontSizeHelper( lastCursor, cursor.position(), currentPointSize, 1.2 );
            }
        }

        // Now check
        {
            QCOMPARE( textDoc.toPlainText(), QString("i128\n1618") ); // it better not have changed :)

            QTextCursor cursor( &textDoc );
            int i = 0;
            Q_FOREVER {
                //qDebug() << i << cursor.charFormat().fontPointSize();
#if QT_VERSION < 0x040300
                // Skip checking position 0 with Qt-4.2, it worked differently then
                if ( i > 0 )
#endif
                QCOMPARE( cursor.charFormat().fontPointSize(), initialPointSizes[i] * 1.2 );
                if ( cursor.atEnd() )
                    break;
                cursor.movePosition( QTextCursor::NextCharacter );
                ++i;
            }
        }

        // Now call the actual KDReports::TextDocument implementation
        KDReports::TextDocument myTextDoc;
        fillDocWithVariousSizes(myTextDoc.contentDocument());
        //myTextDoc.prepareForLayout();
        myTextDoc.scaleFontsBy(1.2);

        // Now check that KDReports itself works
        {
            QCOMPARE( myTextDoc.contentDocument().toPlainText(), QString("i128\n1618") ); // it better not have changed :)

            QTextCursor cursor( &myTextDoc.contentDocument() );
            int i = 0;
            Q_FOREVER {
                //qDebug() << i << cursor.charFormat().fontPointSize();
#if QT_VERSION < 0x040300
                // Skip checking position 0 with Qt-4.2, it worked differently then
                if ( i > 0 )
#endif
                QCOMPARE( cursor.charFormat().fontPointSize(), initialPointSizes[i] * 1.2 );
                if ( cursor.atEnd() )
                    break;
                cursor.movePosition( QTextCursor::NextCharacter );
                ++i;
            }
        }

    }

    void testFindingTables()
    {
        // How to find all the tables in a QTextDocument?
        QTextDocument textDoc;
        QTextCursor c( &textDoc );
        QTextTable* firstTable = c.insertTable( 2, 2 );
        QTextTableCell bottomRight = firstTable->cellAt( 1, 1 );
        QTextTable* secondTable = bottomRight.firstCursorPosition().insertTable( 3, 3 ); // a nested table
        c.movePosition( QTextCursor::End );
        QTextTable* thirdTable = c.insertTable( 1, 1 );
        thirdTable->firstCursorPosition().insertText( "in table" );
        c.insertText( "Foo" );
        QList<QTextTable *> origTables;
        origTables << firstTable << secondTable << thirdTable;

        // A generic and slow solution is
        //    curs.currentTable() && !tablesFound.contains(curs.currentTable())
        // for each cursor position. Surely there's better.
        // We could jump to currentFrame().lastCursorPosition() but then it would skip
        // nested tables.
        QTextDocument* clonedDoc = textDoc.clone();
        QSet<QTextTable *> tablesFound;
        {
            QTextCursor curs(clonedDoc);
            while (!curs.atEnd()) {
                QTextTable* currentTable = curs.currentTable();
                if ( currentTable && !tablesFound.contains(currentTable) ) {
                    tablesFound.insert( currentTable );
                }
                curs.movePosition( QTextCursor::NextCharacter );
            }
            QCOMPARE( tablesFound.size(), 3 );
        }

        // Let's do something else then, let's find them by cursor position
        QList<QTextTable *> tablesByPos;
        {
            // first test
            const int firstPos = firstTable->firstCursorPosition().position();
            QTextCursor curs( clonedDoc );
            curs.setPosition( firstPos );
            QVERIFY( curs.currentTable() );

            // generic loop. works this approach is in TextDocument::breakTables now.
            Q_FOREACH( QTextTable* origTable, origTables ) {
                QTextCursor curs( clonedDoc );
                curs.setPosition( origTable->firstCursorPosition().position() );
                tablesByPos.append( curs.currentTable() );
            }
            QCOMPARE( tablesByPos.size(), 3 );
            QCOMPARE( tablesByPos.toSet(), tablesFound );
        }

        delete clonedDoc;
    }

    // Now for actual KDReports tests

    void testTextElement()
    {
        QVERIFY(qApp);

        Report report;
        QCOMPARE( report.doc().contentDocument().blockCount(), 1 );
        TextElement elem1( QString::fromLatin1( "foo" ) );
        report.addElement( elem1 );
        QTextDocument& doc = report.doc().contentDocument();
        QCOMPARE( doc.blockCount(), 1 );
        QCOMPARE( doc.toPlainText(), QString("foo") );
        TextElement elem2( QString::fromLatin1( "bar" ) );
        report.addElement( elem2 );
        QCOMPARE( doc.blockCount(), 2 );
        QCOMPARE( doc.toPlainText(), QString("foo\nbar") );
    }

    void testAddVerticalSpacing()
    {
        Report report;
        report.addVerticalSpacing( 1 );
        report.addElement( TextElement( "foo" ) );
        report.addVerticalSpacing( 10 );
        report.addElement( TextElement( "bar" ) );
        // We can't QCOMPARE anything here, but at least we checked that it doesn't crash
    }

    void testVariableInHeader()
    {
        Report report;
        Header& header = report.header();
        KDReports::TextElement text( "Page " );
        text.setPointSize( 18 );
        header.addInlineElement( text );
        header.addVariable( KDReports::PageNumber );
        //header.doc().prepareForLayout();
        // Check preparePaintingPage() when the variable is at the last position
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("Page 1") );
        header.preparePaintingPage( 1 );
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("Page 2") );

        // Check that the formatting of the variable reused the formatting of the text (KDRE-91)
        QTextCursor cursor(& header.doc().contentDocument() );
        cursor.setPosition( 6 );
        QCOMPARE( cursor.charFormat().font().pointSize(), 18 );

        // Check preparePaintingPage() when the variable isn't last.
        header.addInlineElement( KDReports::TextElement( "." ) );
        //header.doc().prepareForLayout();
        header.preparePaintingPage( 1 );
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("Page 2.") );
        header.preparePaintingPage( 2 );
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("Page 3.") );
    }

    void testVariableInHeaderAtFirstPosition()
    {
        Report report;
        Header& header = report.header();
        header.addVariable( KDReports::PageNumber );
        //header.doc().prepareForLayout();
        // Check preparePaintingPage() when the variable is at the first position
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("1") );
        header.preparePaintingPage( 1 );
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("2") );
    }

    void testVariableInTableCell()
    {
        Report report;
        Header& header = report.header();
        TableElement table;
        Cell& cell = table.cell(0, 0);
        cell.addInlineElement( KDReports::TextElement( "Page " ) );
        cell.addVariable( KDReports::PageNumber );
        header.addElement(table);
        //header.doc().prepareForLayout();
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("\nPage 1\n") );
        header.preparePaintingPage( 1 );
        QCOMPARE( header.doc().contentDocument().toPlainText(), QString("\nPage 2\n") );
    }

    void testAutoTable()
    {
        Report report;
        report.setDefaultFont(QFont("Arial", 18));
        QStandardItemModel model(2, 2);
        QStandardItem* headerItem = new QStandardItem( QLatin1String( "Header1" ) );
        headerItem->setBackground( Qt::red );
        model.setHorizontalHeaderItem( 0, headerItem );
        model.setItem( 0, 0, new QStandardItem( QLatin1String( "TopLeft" ) ) );
        QStandardItem *topRight = new QStandardItem( QLatin1String( "TopRight" ) );
        QFont font;
        font.setBold(true);
        topRight->setFont(font);
        model.setItem( 0, 1, topRight );
        model.setItem( 1, 0, new QStandardItem( QLatin1String( "BottomLeft" ) ) );
        model.setItem( 1, 1, new QStandardItem( QLatin1String( "<html><b>BottomRight</b>" ) ) );

        AutoTableElement tableElem( &model );
        tableElem.setVerticalHeaderVisible(false);
        tableElem.setHorizontalHeaderVisible(true);
        report.addElement(tableElem);

        // trigger a layout
        QCOMPARE( report.numberOfPages(), 1 );

        report.addPageBreak();
        report.addElement( TextElement( "page 2" ) );
        QCOMPARE( report.numberOfPages(), 2 );

        // for debugging
        //KDReports::PreviewDialog preview( &report );
        //preview.exec();

        QTextCursor c( &report.doc().contentDocument() );
        c.movePosition( QTextCursor::NextCharacter );
        QTextTable* table = c.currentTable();
        QVERIFY(table);

        QTextTableCell headerCell = table->cellAt(0, 0);
        QVERIFY(headerCell.isValid());
        QTextCursor cc = headerCell.firstCursorPosition();
        QCOMPARE(cc.block().text(), QString::fromLatin1("Header1"));
        QCOMPARE(cc.charFormat().background().color().name(), QColor(Qt::red).name() );

        QTextTableCell topLeftCell = table->cellAt(1, 0);
        QVERIFY(topLeftCell.isValid());
        cc = topLeftCell.firstCursorPosition();
        QCOMPARE(cc.block().text(), QString::fromLatin1("TopLeft"));

        QTextTableCell topRightCell = table->cellAt(1, 1);
        QVERIFY(topRightCell.isValid());
        cc = topRightCell.firstCursorPosition();
        QCOMPARE(cc.block().text(), QString::fromLatin1("TopRight"));
        QVERIFY(cc.charFormat().font().bold());
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        QCOMPARE(cc.charFormat().font().pointSize(), 18);
#endif

        QTextTableCell bottomRightCell = table->cellAt(2, 1);
        QVERIFY(bottomRightCell.isValid());
        cc = bottomRightCell.firstCursorPosition();
        QCOMPARE(cc.block().text(), QString::fromLatin1("BottomRight"));

        cc.movePosition( QTextCursor::NextCharacter );
        QCOMPARE(cc.charFormat().fontWeight(), int(QFont::Bold));
        QVERIFY(cc.charFormat().font().bold());
        QCOMPARE(cc.charFormat().font().pointSize(), 18);

        // Now check if we can regenerate the autotable
        model.setItem( 0, 0, new QStandardItem( QLatin1String( "MODIFIED" ) ) );
        report.regenerateAutoTables();

        // trigger a layout again
        QCOMPARE( report.numberOfPages(), 2 );

        // for debugging
        //KDReports::PreviewDialog preview( &report );
        //preview.exec();

        c = QTextCursor( &report.doc().contentDocument() );
        c.movePosition( QTextCursor::NextCharacter );
        table = c.currentTable();
        QVERIFY(table);
        topLeftCell = table->cellAt(1, 0);
        QVERIFY(topLeftCell.isValid());
        cc = topLeftCell.firstCursorPosition();
        QCOMPARE(cc.block().text(), QString::fromLatin1("MODIFIED"));

        // Regenerate again using the new regenerateAutoTableForModel
        model.setItem( 0, 0, new QStandardItem( QLatin1String( "MODIFIEDAGAIN" ) ) );
        report.regenerateAutoTableForModel( &model );
        QCOMPARE( report.numberOfPages(), 2 );

        c = QTextCursor( &report.doc().contentDocument() );
        c.movePosition( QTextCursor::NextCharacter );
        table = c.currentTable();
        QVERIFY(table);
        topLeftCell = table->cellAt(1, 0);
        QVERIFY(topLeftCell.isValid());
        cc = topLeftCell.firstCursorPosition();
        QCOMPARE(cc.block().text(), QString::fromLatin1("MODIFIEDAGAIN"));
    }

    void testAutoTableWithFetchMore()
    {
        // open a DB connection to an in-memory database
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:");
        if ( !db.open() ) {
            qWarning("Could not use QSQLITE!");
            QVERIFY(0);
        }
        // fill the DB with some test data
        QSqlQuery query;
        query.exec("create table airlines (id int primary key, "
                   "name varchar(20), homecountry varchar(2))");
        const int numRows = 400;
        for (int i = 0; i < numRows; i++)
            query.exec(QString("insert into airlines values(%0, 'Test%0', 'T%0')").arg(i));

        QSqlTableModel tableModel( 0, db );
        tableModel.setTable( "airlines" );
        tableModel.select();

        KDReports::Report report;
        KDReports::AutoTableElement tableElement( &tableModel );
        report.addElement( tableElement );

        //report.exportToFile( "testAutoTableWithFetchMore.pdf" ); // for debugging

        // trigger a layout
        QVERIFY( report.numberOfPages() >= 5 );

        QTextCursor c( &report.doc().contentDocument() );
        c.movePosition( QTextCursor::NextCharacter );
        QTextTable* table = c.currentTable();
        QVERIFY(table);

        QCOMPARE(table->rows(), numRows + 1 /*header*/);
    }

    void testBigImage()
    {
        Report report;
        QPixmap pix( 200, 1500 );
        pix.fill( Qt::black );
        KDReports::ImageElement imageElement( pix );
        imageElement.setHeight( 100, KDReports::Percent );
        report.addElement( imageElement );
        QCOMPARE( report.numberOfPages(), 1 );
    }

    void testSetFontFullyQualified()
    {
        Report report;
        TextElement elem1( QString::fromLatin1( "foo" ) );
        elem1.setFont(QFont("Arial", 18));
        report.addElement( elem1 );
        QTextDocument& doc = report.doc().contentDocument();
        QTextCursor c( &doc );
        c.setPosition( 1 );
        QCOMPARE( c.charFormat().font().pointSize(), 18 );
    }

    void testSetFontNeedsResolving()
    {
        Report report;
        report.setDefaultFont(QFont("Arial", 18));
        TextElement elem1( QString::fromLatin1( "foo" ) );
        QFont font;
        font.setBold(true);
        elem1.setFont(font);
        report.addElement( elem1 );
        QTextDocument& doc = report.doc().contentDocument();
        QTextCursor c( &doc );
        c.setPosition( 1 );
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        QCOMPARE( c.charFormat().font().pointSize(), 18 );
#endif
        QVERIFY( c.charFormat().font().bold() );
    }


private:
    static void setFontSizeHelper( QTextCursor& lastCursor, int endPosition, qreal pointSize, qreal factor )
    {
        if ( pointSize == 0 ) {
            pointSize = lastCursor.block().document()->defaultFont().pointSize();
        }
        pointSize *= factor;
        QTextCharFormat newFormat;
        newFormat.setFontPointSize( pointSize );
        //qDebug() << "Applying" << pointSize << "from" << lastCursor.position() << "to" << endPosition;
        lastCursor.setPosition( endPosition, QTextCursor::KeepAnchor );
        lastCursor.mergeCharFormat( newFormat );
    }
    static void fillDocWithVariousSizes( QTextDocument& textDoc )
    {
        // Initial setup
        QTextCursor cursor( &textDoc );
        cursor.insertText( "i" ); // initial point size, default font.
        QTextCharFormat charFormat;
        charFormat.setFontPointSize( 12 );
        cursor.setCharFormat( charFormat );
        cursor.insertText( "12" );
        charFormat.setFontPointSize( 8 );
        cursor.setCharFormat( charFormat );
        cursor.insertText( "8" );

        QTextBlockFormat blockFormat;
        charFormat.setFontPointSize( 16 );
        cursor.setBlockFormat( blockFormat );
        cursor.insertBlock( blockFormat, charFormat );
        cursor.insertText( "16" );
        charFormat.setFontPointSize( 18 );
        cursor.setCharFormat( charFormat );
        cursor.insertText( "18" );

        QCOMPARE( textDoc.toPlainText(), QString("i128\n1618") );
    }
};

QTEST_MAIN(Test) // Report needs QPrinter needs a QApplication

#include "TextDocument.moc"
