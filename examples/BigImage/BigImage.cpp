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
#include <TableModel.h>

#include <KDReports>
#include <QDebug>
#include <QPainter>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KDReports::Report report;
    KDReports::Header &header = report.header(KDReports::FirstPage);
    header.addElement(KDReports::TextElement("A tall image made to fit into the page"));

    KDReports::Header &header2 = report.header(KDReports::EvenPages);
    header2.addElement(KDReports::TextElement("A wide image made to fit into the page"));

    KDReports::Footer &footer = report.footer();
    footer.addElement(KDReports::TextElement("Page"), Qt::AlignRight);
    footer.addInlineElement(KDReports::TextElement(" "));
    footer.addVariable(KDReports::PageNumber);
    footer.addInlineElement(KDReports::TextElement("/"));
    footer.addVariable(KDReports::PageCount);

    KDReports::ImageElement imageElement(QPixmap(":/imageTall.jpg"));
    imageElement.setFitToPage();
    report.addElement(imageElement);

    KDReports::ImageElement imageElement2(QPixmap(":/image.jpg"));
    imageElement2.setFitToPage();
    report.addElement(imageElement2);

    KDReports::PreviewDialog preview(&report);
    return preview.exec();
}
