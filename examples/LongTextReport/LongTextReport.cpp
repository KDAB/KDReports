/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#include <QApplication>
#include <QDebug>

#include <KDReportsPreviewDialog.h>
#include <KDReportsReport.h>
#include <KDReportsTextDocument_p.h>
#include <KDReportsTextElement.h>
#include <QElapsedTimer>
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

    // add many, many paragraphs of text
    QElapsedTimer timer;
    timer.start();
    report.beginEdit();
    for (int i = 0; i < 10000; ++i) {
        KDReports::TextElement textElement(QObject::tr("This is a report generated with KDReports"));
        report.addElement(textElement, Qt::AlignLeft);
    }
    report.endEdit();
    qDebug() << timer.elapsed() / 1000.0 << "s for adding the elements to the report";
    timer.start();

    // show a print preview
    timer.start();
    KDReports::PreviewDialog preview(&report);
    qDebug() << timer.elapsed() / 1000.0 << "s for layouting";

    return preview.exec();
}
