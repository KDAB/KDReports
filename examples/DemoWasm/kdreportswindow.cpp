#include "kdreportswindow.h"
#include "ui_kdreportswindow.h"

#include "ResultModel.h"
#include <QApplication>
#include <QFile>
#include <QStandardItemModel>
#include <TableModel.h>
#ifdef HAVE_KDCHART
#include <KDChartChart>
#include <KDChartPieDiagram>
#include <KDChartPolarCoordinatePlane>
#endif
#include <KDReports.h>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QPainter>

KDReportsWindow::KDReportsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KDReportsWindow)
{
    ui->setupUi(this);
    connect(ui->actionReference_Report, &QAction::triggered, this, &KDReportsWindow::referenceReport);
    connect(ui->actionLetter, &QAction::triggered, this, &KDReportsWindow::letterReport);
    connect(ui->actionMain_Merge, &QAction::triggered, this, &KDReportsWindow::mailMergeXml);
    connect(ui->actionPrice_List_XML, &QAction::triggered, this, &KDReportsWindow::priceListXml);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, ui->tabWidget, &QTabWidget::removeTab);
    ui->menubar->setVisible(false);
    ui->tabWidget->setTabsClosable(false);

    letterReport();
    referenceReport();
    mailMergeXml();
    priceListXml();
    ui->tabWidget->setCurrentIndex(0);
}

KDReportsWindow::~KDReportsWindow()
{
    delete ui;
}

static void fillEvenPagesHeader(KDReports::Header &evenPagesHeader)
{
    evenPagesHeader.addElement(KDReports::TextElement("This header should be on even pages, and contain a 2x2 table with blue borders"));
    KDReports::TableElement table;
    table.setBorder(1); // if you see no borders in this table, upgrade to Qt >= 4.3.1
    table.setBorderBrush(Qt::blue);
    table.setHeaderRowCount(0);
    table.cell(0, 0).addInlineElement(KDReports::TextElement("1"));
    table.cell(0, 1).addInlineElement(KDReports::TextElement("2"));
    KDReports::Cell &cell = table.cell(1, 0);
    cell.addElement(KDReports::TextElement("This should say 2/2: "));
    cell.addVariable(KDReports::PageNumber);
    cell.addInlineElement(KDReports::TextElement("/"));
    cell.addVariable(KDReports::PageCount);
    evenPagesHeader.addInlineElement(table);
}


void KDReportsWindow::referenceReport()
{
    auto report = new KDReports::Report;
    report->setDocumentName("Reference report");

    report->setWatermarkPixmap(QPixmap(":/background.jpg"));
    report->setWatermarkText(QString());

    KDReports::Header &header = report->header(KDReports::FirstPage);
    QPixmap kdab(":/kdab_small.jpg");
    QPainter painter(&kdab);
    painter.drawRect(0, 0, kdab.width() - 1, kdab.height() - 1);
    KDReports::ImageElement imageElement(kdab);
    imageElement.setWidth(50); // mm
    header.addElement(imageElement);
    header.addElement(KDReports::TextElement("This header should be on the first page. The kdab logo should be 50mm wide and 24mm high."));

    header.addElement(KDReports::TextElement("This should say 1/2: "));
    header.addVariable(KDReports::PageNumber);
    header.addInlineElement(KDReports::TextElement("/"));
    header.addVariable(KDReports::PageCount);

    header.addInlineElement(KDReports::TextElement("\t\tDate: "));
    header.addVariable(KDReports::DefaultLocaleLongDate);

    KDReports::Header &evenPagesHeader = report->header(KDReports::EvenPages);
    fillEvenPagesHeader(evenPagesHeader);

    KDReports::Footer &footer = report->footer();
    footer.addElement(KDReports::TextElement("This should be right-aligned"), Qt::AlignRight);

    KDReports::TextElement titleElement;
    titleElement << "This text is blue, bold, centered and has a point size of 14";
    titleElement.setBold(true);
    titleElement.setPointSize(14);
    titleElement.setTextColor(Qt::blue);
    report->addElement(titleElement, Qt::AlignHCenter);

    report->addVerticalSpacing(15);
    report->addElement(KDReports::TextElement("There should be 15mm above this paragraph"));
    report->addElement(KDReports::TextElement("There should be an image as watermark behind this text"));
    report->addElement(KDReports::HLineElement());
    report->addElement(KDReports::TextElement("There should be a horizontal line above this paragraph"));

    {
        QList<QTextOption::Tab> tabs;
        tabs.append(report->middleAlignedTab());
        tabs.append(report->rightAlignedTab());
        report->setTabPositions(tabs);
    }

    report->addElement(KDReports::TextElement("Left-aligned text\tMiddle\tRight-aligned text"));

    report->setParagraphMargins(30, 10, 50, 10);
    report->addElement(KDReports::TextElement("This paragraph has margins on all sides; especially visible when its text is long enough so that it wraps onto multiple lines."));
    report->setParagraphMargins(0, 0, 0, 0);

    // report->addElement( KDReports::TextElement( "Left again" ) );
    // report->addElement( KDReports::TextElement( "Right" ), Qt::AlignRight ); // Fails when done here! Must be a Qt bug ###

    report->addPageBreak();

    // Define tab positions for decimal-point alignment
    {
        QList<QTextOption::Tab> tabs;
        QTextOption::Tab tab;
        tab.position = 50; // in mm
        tab.type = QTextOption::DelimiterTab;
        tab.delimiter = QLatin1Char('.');
        tabs.append(tab);
        report->setTabPositions(tabs);
    }

    // table
    KDReports::TableElement tableElement;
    tableElement.setHeaderRowCount(2);
    tableElement.setPadding(3);
    QColor headerColor("#DADADA");
    // Merged header in row 0
    KDReports::Cell &topHeader = tableElement.cell(0, 0);
    topHeader.setColumnSpan(2);
    topHeader.setBackground(headerColor);
    topHeader.addElement(KDReports::TextElement("This header should be gray and span over two columns"), Qt::AlignHCenter);

    // Normal header in row 1
    KDReports::Cell &headerCell1 = tableElement.cell(1, 0);
    headerCell1.setBackground(headerColor);
    // This would look better if centered vertically. This feature is only available since
    // Qt-4.3 though (QTextCharFormat::AlignMiddle)
    QPixmap systemPixmap(":/system.png");
    headerCell1.addElement(KDReports::ImageElement(systemPixmap));
    headerCell1.addInlineElement(KDReports::TextElement(" Header with computer icon"));

    KDReports::Cell &headerCell2 = tableElement.cell(1, 1);
    headerCell2.setBackground(headerColor);
    KDReports::TextElement expected("Italic with dark gray background");
    expected.setItalic(true);
    expected.setBackground(QColor("#999999")); // note that this background only applies to this element
    headerCell2.addElement(expected);
    headerCell2.addInlineElement(KDReports::TextElement(" Normal text"));

    // Data in rows 2 and 3
    tableElement.cell(2, 0).addElement(KDReports::TextElement("Price:\t250.2"));
    tableElement.cell(2, 1).addElement(KDReports::TextElement("Price:\t1088.5"));
    tableElement.cell(3, 0).addElement(KDReports::TextElement("Reduced price:\t68.52"));
    tableElement.cell(3, 1).addElement(KDReports::TextElement("Reduced price:\t88.584"));

    report->addElement(tableElement);

    report->addVerticalSpacing(5);

    // Auto table test

    QStandardItemModel model;
    QStandardItem *firstHeaderItem = new QStandardItem(QObject::tr("<html>This auto-table uses <b>QStandardItemModel</b>"));
    firstHeaderItem->setTextAlignment(Qt::AlignCenter); // works
    model.setHorizontalHeaderItem(0, firstHeaderItem);
    QStandardItem *secondHeaderItem = new QStandardItem(QObject::tr("Icon on the left in this header"));
    secondHeaderItem->setData(QVariant::fromValue(QPixmap(":/system.png")), Qt::DecorationRole);
    // secondHeaderItem->setTextAlignment( Qt::AlignCenter ); // doesn't work. QTBUG-20041
    model.setHorizontalHeaderItem(1, secondHeaderItem);
    QStandardItem *thirdHeaderItem = new QStandardItem(QObject::tr("Small pixmap on the right"));
    thirdHeaderItem->setIcon(QPixmap(":/system.png")); // size determined by setIconSize below
    thirdHeaderItem->setData(Qt::AlignRight, KDReports::AutoTableElement::DecorationAlignmentRole);
    model.setHorizontalHeaderItem(2, thirdHeaderItem);

    QStandardItem *firstCellItem = new QStandardItem(QObject::tr("<html>This is <b>bold</b> text"));
    model.setItem(0, 0, firstCellItem);

    QStandardItem *secondCellItem = new QStandardItem(QObject::tr("Icon on the left in this cell"));
    secondCellItem->setData(QVariant::fromValue(QPixmap(":/system.png")), Qt::DecorationRole);
    model.setItem(0, 1, secondCellItem);

    QStandardItem *thirdCellItem = new QStandardItem(QObject::tr("Small pixmap on the right"));
    thirdCellItem->setIcon(QPixmap(":/system.png")); // size determined by setIconSize below
    thirdCellItem->setData(Qt::AlignRight, KDReports::AutoTableElement::DecorationAlignmentRole);
    model.setItem(0, 2, thirdCellItem);

    QStandardItem *italicItem = new QStandardItem(QObject::tr("10 pt italic blue text"));
    italicItem->setFont(QFont("Arial", 10, -1, true /*italic*/));
    italicItem->setForeground(Qt::blue);
    model.setItem(1, 0, italicItem);

    QStandardItem *yellowItem = new QStandardItem(QObject::tr("Yellow background"));
    yellowItem->setBackground(Qt::yellow);
    model.setItem(1, 1, yellowItem);

    QStandardItem *alignedItem = new QStandardItem(QObject::tr("Right-aligned"));
    alignedItem->setTextAlignment(Qt::AlignRight);
    model.setItem(1, 2, alignedItem);

    KDReports::AutoTableElement autoTable(&model);
    autoTable.setIconSize(QSize(16, 16));
    autoTable.setHeaderBackground(QBrush());
    autoTable.setVerticalHeaderVisible(false);
    report->addElement(autoTable);

    report->addElement(KDReports::TextElement());

    ResultModel resultModel;
    KDReports::AutoTableElement autoTableResults(&resultModel);
    autoTableResults.setVerticalHeaderVisible(false);
    report->addElement(autoTableResults);

    // Chart test
#ifdef HAVE_KDCHART
    report->addElement(KDReports::TextElement());
    report->addElement(KDReports::TextElement("The chart below is made with KDChart:"));

    TableModel chartModel;
    chartModel.setDataHasVerticalHeaders(false);
    chartModel.loadFromCSV(":/chart_model");
    KDChart::Chart chart;
    chart.replaceCoordinatePlane(new KDChart::PolarCoordinatePlane(&chart));
    KDChart::PieDiagram *diagram = new KDChart::PieDiagram();
    diagram->setModel(&chartModel);
    chart.coordinatePlane()->replaceDiagram(diagram);
    KDReports::ChartElement chartElement(&chart);
    chartElement.setSize(50, 50); // in mm
    report->addElement(chartElement);
#endif

    // Hyperlink test
    report->addElement(KDReports::HtmlElement("<a href=\"https://www.kdab.com\">click here to open https://www.kdab.com</a>"));

    // report->exportToHtml( "out.html" );

    auto preview = new KDReports::PreviewWidget(this);
    preview->setReport(report);
    int idx = ui->tabWidget->addTab(preview, "Reference Report");
    ui->tabWidget->setCurrentIndex(idx);
}

void KDReportsWindow::letterReport()
{
    // Create a report
    auto report = new KDReports::Report;

    // create a table:
    QStandardItemModel model;
    model.setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Product")));
    model.setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Release Date")));
    int row = 0;
    model.setItem(row, 0, new QStandardItem(QObject::tr("KD Reports 1.0")));
    model.setItem(row++, 1, new QStandardItem(QObject::tr("August 21, 2007")));
    model.setItem(row, 0, new QStandardItem(QObject::tr("KD Reports 1.1")));
    model.setItem(row++, 1, new QStandardItem(QObject::tr("November 25, 2008")));
    model.setItem(row, 0, new QStandardItem(QObject::tr("KD Reports 1.2")));
    model.setItem(row++, 1, new QStandardItem(QObject::tr("October, 2009")));

    // Add From: and To: information:
    KDReports::TextElement fromElement;
    fromElement << "From:\n"
                << QString::fromUtf8("Klarälvdalens Datakonsult AB\n") << "Rysktorp\n"
                << "Sweden\n";
    report->addElement(fromElement);
    report->addVerticalSpacing(10);
    KDReports::TextElement toElement;
    toElement << "To:\n"
              << "All Qt Users Out There In The World\n";
    report->addElement(toElement);
    report->addVerticalSpacing(10);

    // Add a text element for the title
    KDReports::TextElement titleElement(QObject::tr("Ever wanted to create a printed report from within your Qt application?\nLook no further!"));
    titleElement.setPointSize(14);
    report->addElement(titleElement, Qt::AlignCenter);

    // A bit of spacing (10 mm)
    report->addVerticalSpacing(10);

    // Add another text element, demonstrating "<<" operator
    KDReports::TextElement bodyElement;
    bodyElement.setPointSize(10);
    bodyElement << QObject::tr("Dear KDAB Customers,\n");
    bodyElement << "we are happy to introduce the newest member of KDAB's line of industry leading software"
                   " products: KD Reports. KD Reports is the Qt tool to easily create printable reports. It "
                   "provides all necessary features for a variety of applications:\n"
                   "Reports can be created programmatically, using an easy to use C++ API, or data-driven, "
                   "creating complete reports from XML sources. Report data "
                   "can be retrieved from SQL data sources. If KDAB's KDChart package is available, reports "
                   "can be garnished with different chart types. Watermarks, headers and footers are there "
                   "as well. Reports can be previewed manually, directly send to be printer, or saved as "
                   "PDF.\n";
    report->addElement(bodyElement, Qt::AlignJustify);

    // release date table:
    KDReports::AutoTableElement tableElement(&model);
    tableElement.setVerticalHeaderVisible(false);
    tableElement.setBorder(1);
    report->addElement(tableElement, Qt::AlignCenter);
    report->addVerticalSpacing(6);

    KDReports::TextElement body2Element;
    body2Element.setPointSize(10);
    body2Element << "Reporting is a rather general feature, and it seems a nice package providing this kind "
                    "of functionality to complement Qt was looked for by many. We at KDAB hope to make the "
                    "life of our customers more enjoyable with it. Let us know if we were successful!\n";
    report->addElement(body2Element, Qt::AlignJustify);
    report->addVerticalSpacing(30);
    KDReports::TextElement signatureElement;
    signatureElement << QObject::tr("Cheers,\n") << QObject::tr("Klarälvdalens Datakonsult AB, Platform-independent software solutions");

    report->addElement(signatureElement);

    // add footer with contact information:
    report->footer().addElement(KDReports::HLineElement());
    KDReports::TextElement footerText;
    footerText << "www.kdab.com | email: info@kdab.com | +46-563-540090";
    footerText.setPointSize(8);
    report->footer().addElement(footerText, Qt::AlignCenter);

    auto preview = new KDReports::PreviewWidget(this);
    preview->setReport(report);
    int idx = ui->tabWidget->addTab(preview, "What is KDReports?");
    ui->tabWidget->setCurrentIndex(idx);
}

void KDReportsWindow::mailMergeXml()
{
    auto report = new KDReports::Report;

    QFile reportFile(":/files/MailMerge.xml");
    if (!reportFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0, QObject::tr("Warning"),
                             QObject::tr("Could not open report description file 'MailMerge.xml'. "
                                         "Please start this program from the HelloWorldXML directory."));
        return;
    }

    TableModel members;
    members.setDataHasVerticalHeaders(false);
    members.loadFromCSV(":/files/members.csv");
    report->associateModel(QLatin1String("members"), &members);

    for (int i = 0; i < members.rowCount(); i++) {
        report->setCurrentRow(&members, i);
        KDReports::ErrorDetails details;
        if (!report->loadFromXML(&reportFile, &details)) {
            QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Could not parse report description file:\n%1").arg(details.message()));
            reportFile.close();
            return;
        }

        if (i < members.rowCount() - 1)
            report->addPageBreak();

        // show a print preview:
    }
    auto preview = new KDReports::PreviewWidget(this);
    preview->setReport(report);
    int idx = ui->tabWidget->addTab(preview, "Mail Merge");
    ui->tabWidget->setCurrentIndex(idx);
}

void KDReportsWindow::priceListXml()
{
    auto report = new KDReports::Report;

    // Set the content of a text field - this shows how xml files can be used as templates for reports,
    // not only as complete (generated) reports.
    report->associateTextValue("title_element", "Price list example");
    report->associateTextValue("company_address",
                              QString::fromUtf8("Klarälvdalens Datakonsult AB\n"
                                                "Rysktorp\n"
                                                "SE-68392 Hagfors\n"
                                                "Sweden"));
    // Note how id="table1_title" is used twice in the xml, both places get the right value
    report->associateTextValue("table1_title", "Network Peripherals");
    report->associateTextValue("table2_title", "Printer Cartridges");

    report->associateImageValue("image_system", QPixmap(":/system.png"));

    // Create two table models which will be used by one table element each.
    TableModel table1;
    table1.setDataHasVerticalHeaders(false);
    table1.loadFromCSV(":/table1");
    report->associateModel(QLatin1String("table1"), &table1);
    TableModel table2;
    table2.setDataHasVerticalHeaders(false);
    table2.loadFromCSV(":/table2");
    report->associateModel(QLatin1String("table2"), &table2);

    QFile reportFile(":/files/PriceList.xml");
    if (!reportFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Could not open report description file 'PriceList.xml'. Please start this program from the PriceListXML directory."));
        return;
    }

    KDReports::ErrorDetails details;
    if (!report->loadFromXML(&reportFile, &details)) {
        QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Could not parse report description file:\n%1").arg(details.message()));
        reportFile.close();
        return;
    }

    auto preview = new KDReports::PreviewWidget(this);
    preview->setReport(report);
    int idx = ui->tabWidget->addTab(preview, "Price List XML");
    ui->tabWidget->setCurrentIndex(idx);
}
