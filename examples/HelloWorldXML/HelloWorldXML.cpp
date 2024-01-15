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

#include <KDReportsPreviewDialog.h>
#include <KDReportsReport.h>
#include <KDReportsTextElement.h>
#include <QFile>
#include <QMessageBox>
#include <QPrintDialog>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Create a report
    KDReports::Report report;

    QFile reportFile(":/HelloWorld.xml");
    if (!reportFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"),
                             QObject::tr("Could not open report description file 'HelloWorld.xml'. "
                                         "Please start this program from the HelloWorldXML directory."));
        return -1;
    }

    KDReports::ErrorDetails details;
    if (!report.loadFromXML(&reportFile, &details)) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Could not parse report description file:\n%1").arg(details.message()));
        reportFile.close();
        return -2;
    }

    // show a print preview:
    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}
