/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <QApplication>
#include <QDebug>
#include <QStandardItemModel>

// or simply: #include <KDReports>
#include <KDReportsAutoTableElement.h>
#include <KDReportsChartElement.h>
#include <KDReportsHLineElement.h>
#include <KDReportsHeader.h>
#include <KDReportsHtmlElement.h>
#include <KDReportsPreviewDialog.h>
#include <KDReportsReport.h>
#include <KDReportsTableElement.h>
#include <KDReportsTextElement.h>
#include <QPrintDialog>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Create a report
    KDReports::Report report;

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
    report.addElement(fromElement);
    report.addVerticalSpacing(10);
    KDReports::TextElement toElement;
    toElement << "To:\n"
              << "All Qt Users Out There In The World\n";
    report.addElement(toElement);
    report.addVerticalSpacing(10);

    // Add a text element for the title
    KDReports::TextElement titleElement(QObject::tr("Ever wanted to create a printed report from within your Qt application?\nLook no further!"));
    titleElement.setPointSize(14);
    report.addElement(titleElement, Qt::AlignCenter);

    // A bit of spacing (10 mm)
    report.addVerticalSpacing(10);

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
    report.addElement(bodyElement, Qt::AlignJustify);

    // release date table:
    KDReports::AutoTableElement tableElement(&model);
    tableElement.setVerticalHeaderVisible(false);
    tableElement.setBorder(1);
    report.addElement(tableElement, Qt::AlignCenter);
    report.addVerticalSpacing(6);

    KDReports::TextElement body2Element;
    body2Element.setPointSize(10);
    body2Element << "Reporting is a rather general feature, and it seems a nice package providing this kind "
                    "of functionality to complement Qt was looked for by many. We at KDAB hope to make the "
                    "life of our customers more enjoyable with it. Let us know if we were successful!\n";
    report.addElement(body2Element, Qt::AlignJustify);
    report.addVerticalSpacing(30);
    KDReports::TextElement signatureElement;
    signatureElement << QObject::tr("Cheers,\n") << QObject::tr("Klarälvdalens Datakonsult AB, Platform-independent software solutions");

    report.addElement(signatureElement);

    // add footer with contact information:
    report.footer().addElement(KDReports::HLineElement());
    KDReports::TextElement footerText;
    footerText << "www.kdab.com | email: info@kdab.com | +46-563-540090";
    footerText.setPointSize(8);
    report.footer().addElement(footerText, Qt::AlignCenter);

    // To show a print preview:
    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}
