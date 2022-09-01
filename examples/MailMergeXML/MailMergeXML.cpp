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
#include <QFile>
#include <QMessageBox>
#include <QPrintDialog>
#include <TableModel.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Create a report
    KDReports::Report report;

    QFile reportFile(":/MailMerge.xml");
    if (!reportFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"),
                             QObject::tr("Could not open report description file 'MailMerge.xml'. "
                                         "Please start this program from the HelloWorldXML directory."));
        return -1;
    }

    TableModel members;
    members.setDataHasVerticalHeaders(false);
    members.loadFromCSV(":/members.csv");
    report.associateModel(QLatin1String("members"), &members);

    for (int i = 0; i < members.rowCount(); i++) {
        report.setCurrentRow(&members, i);
        KDReports::ErrorDetails details;
        if (!report.loadFromXML(&reportFile, &details)) {
            QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Could not parse report description file:\n%1").arg(details.message()));
            reportFile.close();
            return -2;
        }

        if (i < members.rowCount() - 1)
            report.addPageBreak();

        // show a print preview:
    }
    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}
