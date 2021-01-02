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

#include <QApplication>
#include <QDebug>

#include <KDReportsReport.h>
#include <KDReportsTextElement.h>
#include <KDReportsPreviewDialog.h>
#include <QPrintDialog>

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    // Add a text element for the title
    KDReports::TextElement titleElement( QObject::tr( "Hello World!" ) );
    titleElement.setPointSize( 18 );
    report.addElement( titleElement, Qt::AlignHCenter );

    // add 20 mm of vertical space:
    report.addVerticalSpacing( 20 );

    // add some more text
    KDReports::TextElement textElement( QObject::tr( "This is a report generated with KDReports" ) );
    report.addElement( textElement, Qt::AlignLeft );

    // show a print preview
    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

