/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include <QAbstractTableModel>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QThread>

#include <KDReports>
#include <QPrintDialog>

class HelloWorldTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    using QAbstractTableModel::QAbstractTableModel;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return 3;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return 8;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole)
            return QString::number((index.row() + 1) * (index.column() + 1));
        else
            return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return QString("Column %1").arg(section + 1);
        else
            return QVariant();
    }
};

class Thread : public QThread
{
    Q_OBJECT
public:
    using QThread::QThread;

protected:
    void run() override
    {

        // Create a report
        KDReports::Report report;

        report.setWatermarkImage(QImage(":/background.jpg"));
        report.setWatermarkText(QString());

        KDReports::ImageElement imageElement(QImage(":/logo.png"));
        report.header().addElement(imageElement);

        // Add a text element for the title
        KDReports::TextElement titleElement(QObject::tr("KD Reports Hello world example"));
        titleElement.setPointSize(18);
        report.addElement(titleElement, Qt::AlignHCenter);

        // A bit of spacing (10 mm)
        report.addVerticalSpacing(10);

        // Add another text element, demonstrating "<<" operator
        KDReports::TextElement textElement;
        textElement << QObject::tr("This is a sample report produced with KD Reports.");
        textElement << "\n";
        textElement << QObject::tr("Klarälvdalens Datakonsult AB, Platform-independent software solutions");
        textElement << "\n";
        report.addElement(textElement);

        // Create a table model, which will be used by the table element
        HelloWorldTableModel tableModel;

        // A bit of spacing (10 mm)
        report.addVerticalSpacing(10);

        // Add an "auto table" (a table that shows the contents of a model)
        KDReports::AutoTableElement tableElement(&tableModel);
        tableElement.setBorder(1);
        report.addElement(tableElement);

        // Add many paragraphs, to demonstrate page breaking
        for (int i = 1; i < 100; ++i) {
            report.addElement(KDReports::HtmlElement(QStringLiteral("<b>Customer</b> <em>%1</em>").arg(i)), Qt::AlignLeft);
            report.addInlineElement(KDReports::TextElement("  - 2006"));
        }

        // Add a page break
        report.addPageBreak();

        report.addElement(KDReports::TextElement("This is the last page"));

        // To export to an image file:
        // qDebug() << "Exporting to output.png";
        report.exportToImage(QSize(200, 1200), "output.png", "PNG"); // deleted by main() below

        // To export to a PDF file:
        // qDebug() << "Exporting to output.pdf";
        report.exportToFile("output.pdf"); // deleted by main() below
    }
};

class XmlThread : public QThread
{
    Q_OBJECT
public:
    using QThread::QThread;

protected:
    void run() override
    {
        KDReports::Report report;
        report.associateTextValue("title_element", "Price list example");
        report.associateImageValue("image_system", QImage(":/system.png"));
        QFile reportFile(":/PriceList.xml");
        if (!reportFile.open(QIODevice::ReadOnly)) {
            Q_ASSERT(false);
        }
        KDReports::ErrorDetails details;
        if (!report.loadFromXML(&reportFile, &details)) {
            qWarning("ERROR: %s", qPrintable(details.message()));
        }
        report.exportToImage(QSize(800, 600), "xml.png", "PNG"); // deleted by main() below
        report.exportToFile("xml.pdf"); // deleted by main() below
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Thread thr;
    thr.start();
    thr.wait();
    Q_ASSERT(QFile::exists("output.pdf"));
    Q_ASSERT(QFile::exists("output.png"));
    const bool keep = argc > 1 && QByteArray(argv[1]) == "-k"; // -k == keep generated files
    if (!keep) {
        QFile::remove("output.pdf");
        QFile::remove("output.png");
    }

    XmlThread xmlThr;
    xmlThr.start();
    xmlThr.wait();
    Q_ASSERT(QFile::exists("xml.pdf"));
    Q_ASSERT(QFile::exists("xml.png"));
    if (!keep) {
        QFile::remove("xml.pdf");
        QFile::remove("xml.png");
    }

    return 0; // app.exec();
}

#include "InThread.moc"
