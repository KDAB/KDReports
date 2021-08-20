/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#include "TestXmlElementHandler.h"
#include <KDReports>
#include <KDReportsTextDocument_p.h>
#include <QPainter>
#include <QStandardItemModel>
#include <QTextBlock>
#include <QTextCursor>
#include <QtTest/QtTest>

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
    void testSimpleXml()
    {
        QFile file(":/simple.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        QVERIFY(report.loadFromXML(&file));
        QCOMPARE(report.doc().contentDocument().blockCount(), 2);
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("Title\nTest used by XmlParser.cpp"));
    }

    void testTextId()
    {
        QFile file(":/textid.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        report.associateTextValue("1", "Ti");
        report.associateTextValue("2", "tle");
        report.associateTextValue("3", "textid.xml");
        QVERIFY(report.loadFromXML(&file));
        QCOMPARE(report.doc().contentDocument().blockCount(), 4);
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("Title .\ntextid.xml\n\n"));

        // Now test changing text values -after- loading.
        report.associateTextValue("1", "New Ti");
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("New Title .\ntextid.xml\n\n"));

        report.associateTextValue("3", "changed");
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("New Title .\nchanged\n\n"));
        report.associateTextValue("1", "Newer Ti");
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("Newer Title .\nchanged\n\n"));

        report.associateTextValue("5", "well");
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("Newer Title .\nchanged\n\nwell"));

        report.associateTextValue("4", "works");
        QCOMPARE(report.doc().contentDocument().toPlainText(), QString("Newer Title .\nchanged\nworks\nwell"));
    }

    void testHtmlId()
    {
        QFile file(":/htmlid.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        report.associateTextValue("1", "<b>Tit</b>");
        report.associateTextValue("2", "l<i>e</i>");
        report.associateTextValue("3", "htmlid.xml");
        QVERIFY(report.loadFromXML(&file));
        QTextDocument &doc = report.doc().contentDocument();
        QCOMPARE(doc.blockCount(), 2);
        QCOMPARE(doc.toPlainText(), QString("Title\nhtmlid.xml"));
        QTextCursor c(&doc);
        QVERIFY(c.charFormat().fontWeight() == QFont::Bold);
    }

    void testMargins()
    {
        QFile file(":/margins.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        QVERIFY(report.loadFromXML(&file));
        const QTextDocument &doc = report.doc().contentDocument();
        QCOMPARE(doc.blockCount(), 2);
        QCOMPARE(doc.toPlainText(), QString("Title\nTest used by XmlParser.cpp"));
        qreal top; qreal left; qreal bottom; qreal right;
        report.getMargins(&top, &left, &bottom, &right);
        QCOMPARE(static_cast<double>(top), 40.0);
        QCOMPARE(static_cast<double>(left), 0.0);
        QCOMPARE(static_cast<double>(right), 20.0);
        QCOMPARE(static_cast<double>(bottom), 20.0);
    }

    void testFonts()
    {
        QFile file(":/fonts.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        report.setDefaultFont(QFont("Fixed", 18));
        // Prepare model
        QStandardItemModel model;
        model.setItem(0, 0, new QStandardItem("0,0"));
        report.associateModel("table1", &model);

        QVERIFY(report.loadFromXML(&file));
        // For the page count variable to be updated, we need to simulate painting the page
        QPixmap pix(10, 10);
        QPainter painter(&pix);
        report.paintPage(0, painter);
        // Check the header's font
        Header &theHeader = report.header();
        const QTextDocument &doc = theHeader.doc().contentDocument();
        QCOMPARE(doc.blockCount(), 1);
        QCOMPARE(doc.toPlainText(), QString("Page 1 of 1"));
        QCOMPARE(doc.defaultFont().family(), QString("Arial"));
        QTextBlock block = doc.begin();
#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
        QCOMPARE(block.charFormat().fontFamily(), QString());
#else
        // What a weird API, fontFamilies returns a QVariant rather than a QStringList!
        QCOMPARE(block.charFormat().fontFamilies(), QVariant());
#endif
        QTextCursor cursor(block);
        while (!cursor.atBlockEnd()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 1, 0)
            QCOMPARE(cursor.charFormat().fontFamily(), QString("Arial"));
#else
            // What a weird API, fontFamilies returns a QVariant rather than a QStringList!
            QCOMPARE(cursor.charFormat().fontFamilies().toStringList(), QStringList{QString("Arial")});
#endif
            QCOMPARE(cursor.charFormat().font().pointSize(), 8);
            cursor.movePosition(QTextCursor::Right);
        }
        // Check the footer's font
        Footer &theFooter = report.footer();
        const QTextDocument &footerDoc = theFooter.doc().contentDocument();
        QCOMPARE(footerDoc.blockCount(), 1);
        QCOMPARE(footerDoc.toPlainText(), QString("footer"));
        QCOMPARE(footerDoc.defaultFont().pointSize(), 16);
        // Check the document's default font
        QCOMPARE(report.doc().contentDocument().defaultFont().pointSize(), 16);
        // Check the document's actual font (for each char, including the autotable)
        cursor = QTextCursor(report.doc().contentDocument().begin());
        while (!cursor.atEnd()) {
            int fontSize = cursor.charFormat().fontPointSize();
            QVERIFY(fontSize == 0 || fontSize == 16); // 0 gets resolved against the document font, so that's fine
            cursor.movePosition(QTextCursor::Right);
        }
    }

    void testTabs() // and paragraph margins
    {
        QFile file(":/tabs.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        QVERIFY(report.loadFromXML(&file));
        QTextDocument &doc = report.doc().contentDocument();
        QCOMPARE(doc.blockCount(), 3);
        QCOMPARE(doc.toPlainText(), QString("Left\tmiddle\tright\n0\t100\t200\nOnly a right margin"));
        QTextBlock block1 = doc.firstBlock();
        const QList<QTextOption::Tab> tabs1 = block1.blockFormat().tabPositions();
        QCOMPARE(tabs1.count(), 2);
        QCOMPARE(tabs1[0].type, QTextOption::CenterTab);
        QCOMPARE(tabs1[1].type, QTextOption::RightTab);
        QTextBlock block2 = block1.next();
        const QList<QTextOption::Tab> tabs2 = block2.blockFormat().tabPositions();
        QCOMPARE(tabs2.count(), 2);
        QCOMPARE(tabs2[0].type, QTextOption::LeftTab);
        QCOMPARE(tabs2[1].type, QTextOption::LeftTab);
        QVERIFY(block2.blockFormat().leftMargin() > 100);
        QCOMPARE(block2.blockFormat().rightMargin(), qreal(0));
        QTextBlock block3 = block2.next();
        QCOMPARE(block3.blockFormat().tabPositions(), tabs2);
        QCOMPARE(block3.blockFormat().leftMargin(), qreal(0));
        QVERIFY(block3.blockFormat().rightMargin() > 100);
    }

    void brokenXML()
    {
        QFile file(":/broken.xml");

        // Do not open file on purpose to test Report's loadFromXML when file is in closed state.

        Report report;
        KDReports::ErrorDetails details;
        QVERIFY(!report.loadFromXML(&file, &details));
        QCOMPARE(details.line(), 2);
        QCOMPARE(details.column(), 47);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCOMPARE(details.driverMessage(), QString("error occurred while parsing element"));
        QCOMPARE(details.message(), QString("Error on line 2, column 47: error occurred while parsing element"));
#else
        QCOMPARE(details.driverMessage(), QString("Expected '>' or '/', but got '\"'."));
        QCOMPARE(details.message(), QString("Error on line 2, column 47: Expected '>' or '/', but got '\"'."));
#endif
    }

    void wrongTopElement()
    {
        QFile file(":/wrongTopElement.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        KDReports::ErrorDetails details;
        QVERIFY(!report.loadFromXML(&file, &details));
        QCOMPARE(details.line(), -1);
        QCOMPARE(details.column(), -1);
        QCOMPARE(details.driverMessage(), QString("Expected \"<report>\" as the topmost element, but found \"<XreportX>\""));
        QCOMPARE(details.message(), QString("Expected \"<report>\" as the topmost element, but found \"<XreportX>\""));
    }

    void vspaceInHeader()
    {
        QFile file(":/vspaceInHeader.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        KDReports::ErrorDetails details;
        QVERIFY(!report.loadFromXML(&file, &details));
        QCOMPARE(details.line(), -1);
        QCOMPARE(details.column(), -1);
        QCOMPARE(details.driverMessage(), QString("<vspace> not allowed in headers, footers, or table cells"));
        QCOMPARE(details.message(), QString("<vspace> not allowed in headers, footers, or table cells"));
    }

    void testHandler()
    {
        QFile file(":/handler.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        TestXmlElementHandler handler(report);
        report.setXmlElementHandler(&handler);

        QStandardItemModel model;
        model.setItem(0, 0, new QStandardItem("modelCell"));
        // HTML cells need to start with "<html>" for KDReportsAutoTable
        // (not necessary if it's just for <html model="..." row="..." column="...">)
        model.setItem(0, 1, new QStandardItem("<html><b>htmlCell</b>"));
        report.associateModel("model1", &model);

        report.associateTextValue("testModified", "modified");

        QVERIFY(report.loadFromXML(&file));
        QVERIFY(!handler.errorDetails().hasError());
        QStringList expectedCallbacks;
        expectedCallbacks << "startReport"
                          << "startHeader"
                          << "imageElement"
                          << "variable"
                          << "endHeader"
                          << "startFooter"
                          << "textElement"
                          << "endFooter"
                          << "tabs"
                          << "paragraph-margins"
                          << "textElement"
                          << "textElement"
                          << "hLineElement"
                          << "vspace"
                          << "htmlElement"
                          << "textElement"
                          << "htmlElement"
                          << "startTableElement"
                          << "startCell"
                          << "textElement"
                          << "endCell"
                          << "endTableElement"
                          << "textElement"
                          << "customElement"
                          << "autoTableElement"
                          << "endReport";
        // qDebug() << handler.callbacks();
        QCOMPARE(handler.callbacks(), expectedCallbacks);
        QTextDocument &doc = report.doc().contentDocument();

        QString ptext = doc.toPlainText();

        // custom objects (such as HLineTextObject) are represented
        // as 0xFFFC
        ptext.replace(QChar(0xFFFC), '#');

        QCOMPARE(ptext, QString("Title\n#\n\nTest used by XmlParser.cpp\nmodelCell\nhtmlCell\n\nTableElement example\n\nmodified\nCustom element\n\nmodelCell\nhtmlCell\n"));
        QCOMPARE(report.doc().contentDocument().toPlainText(), doc.toPlainText());
    }

    void testHandlerWithError()
    {
        QFile file(":/handlerWithError.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        Report report;
        TestXmlElementHandler handler(report);
        report.setXmlElementHandler(&handler);

        QStandardItemModel model;
        model.setItem(0, 0, new QStandardItem("modelCell"));
        // HTML cells need to start with "<html>" for KDReportsAutoTable
        // (not necessary if it's just for <html model="..." row="..." column="...">)
        model.setItem(0, 1, new QStandardItem("<html><b>htmlCell</b>"));
        report.associateModel("model1", &model);

        QVERIFY(!report.loadFromXML(&file));
        KDReports::ErrorDetails details = handler.errorDetails();

        QCOMPARE(details.line(), -1);
        QCOMPARE(details.column(), -1);
        QCOMPARE(details.driverMessage(), QString("PC LOAD LETTER"));
        QCOMPARE(details.message(), QString("PC LOAD LETTER"));
    }

    void testSpreadSheetMode()
    {
        Report report;
        // TODO: make this part of the XML?
        report.setReportMode(Report::SpreadSheet);

        QStandardItemModel model;
        model.setItem(0, 0, new QStandardItem("cell1"));
        model.setItem(0, 1, new QStandardItem("cell2"));
        report.associateModel("table1", &model);

        QFile file(":/spreadsheet.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QVERIFY(report.loadFromXML(&file));

        QAbstractItemModel *baseModel = &model;
        QCOMPARE(report.mainTable()->autoTableElement()->tableModel(), baseModel);
        QCOMPARE(report.pageOrientation(), QPageLayout::Landscape);
    }

    void testSeparateHeaderAndFooter()
    {
        Report report;
        report.setReportMode(Report::SpreadSheet);

        QFile file(":/doc_header.xml");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QVERIFY(report.loadFromXML(&file));

        QStandardItemModel model;
        model.setItem(0, 0, new QStandardItem("cell1"));
        model.setItem(0, 1, new QStandardItem("cell2"));
        AutoTableElement autoTable(&model);
        report.mainTable()->setAutoTableElement(autoTable);

        QFile footerFile(":/doc_footer.xml");
        QVERIFY(footerFile.open(QIODevice::ReadOnly));
        QVERIFY(report.loadFromXML(&footerFile));

        // report.exportToFile( "testSeparateHeaderAndFooter.pdf" ); // for debugging

        // Check that we have header, table, and footer
        Header &header = report.header();
        header.preparePaintingPage(0);
        QCOMPARE(header.doc().contentDocument().toPlainText(), QString("Page 1 of 1"));
        QAbstractItemModel *baseModel = &model;
        QCOMPARE(report.mainTable()->autoTableElement()->tableModel(), baseModel);
        Footer &footer = report.footer();
        QCOMPARE(footer.doc().contentDocument().toPlainText(), QString("footer"));
    }
};

QTEST_MAIN(Test) // Report needs QPrinter needs a QApplication

#include "XmlParser.moc"
