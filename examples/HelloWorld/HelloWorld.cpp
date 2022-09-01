/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <QApplication>
#include <QDebug>

#include <KDReportsPreviewDialog.h>
#include <KDReportsReport.h>
#include <KDReportsTextElement.h>
#include <QPrintDialog>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Create a report
    KDReports::Report report;

    // Add a text element for the title
    KDReports::TextElement titleElement(QObject::tr("Hello World!"));
    titleElement.setPointSize(18);
    report.addElement(titleElement, Qt::AlignHCenter);

    // add 20 mm of vertical space:
    report.addVerticalSpacing(20);

    // add some more text
    KDReports::TextElement textElement(QObject::tr("This is a report generated with KDReports"));
    report.addElement(textElement, Qt::AlignLeft);

    // show a print preview
    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}
