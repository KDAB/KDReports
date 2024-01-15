/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <QAbstractTableModel>
#include <QApplication>
#include <QDebug>
#include <TableModel.h>

// or simply: #include <KDReports>
#include <KDReportsChartElement.h>
#include <KDReportsHtmlElement.h>
#include <KDReportsPreviewDialog.h>
#include <KDReportsReport.h>
#include <KDReportsTableElement.h>
#include <KDReportsTextElement.h>
#include <QFile>
#include <QMessageBox>
#include <QPrintDialog>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Create a report
    KDReports::Report report;

    // Set the content of a text field - this shows how xml files can be used as templates for reports,
    // not only as complete (generated) reports.
    report.associateTextValue("title_element", "Price list example");
    report.associateTextValue("company_address",
                              QString::fromUtf8("Klarälvdalens Datakonsult AB\n"
                                                "Rysktorp\n"
                                                "SE-68392 Hagfors\n"
                                                "Sweden"));
    // Note how id="table1_title" is used twice in the xml, both places get the right value
    report.associateTextValue("table1_title", "Network Peripherals");
    report.associateTextValue("table2_title", "Printer Cartridges");

    report.associateImageValue("image_system", QPixmap(":/system.png"));

    // Create two table models which will be used by one table element each.
    TableModel table1;
    table1.setDataHasVerticalHeaders(false);
    table1.loadFromCSV(":/table1");
    report.associateModel(QLatin1String("table1"), &table1);
    TableModel table2;
    table2.setDataHasVerticalHeaders(false);
    table2.loadFromCSV(":/table2");
    report.associateModel(QLatin1String("table2"), &table2);

    QFile reportFile(":/PriceList.xml");
    if (!reportFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Could not open report description file 'PriceList.xml'. Please start this program from the PriceListXML directory."));
        return -1;
    }

    KDReports::ErrorDetails details;
    if (!report.loadFromXML(&reportFile, &details)) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Could not parse report description file:\n%1").arg(details.message()));
        reportFile.close();
        return -2;
    }

    // To show a print preview:
    KDReports::PreviewDialog preview(&report);
    return preview.exec();

    // return app.exec();
}
