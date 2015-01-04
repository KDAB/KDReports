/****************************************************************************
** Copyright (C) 2007-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Reports library.
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2.1 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#include <QFile>
#include <QApplication>
#include <QAbstractTableModel>
#include <QDebug>
#include <QTextCodec>
#include <QThread>

#include <KDReports>
#include <QPrintDialog>

class HelloWorldTableModel : public QAbstractTableModel
{
public:
    int columnCount( const QModelIndex& parent = QModelIndex() ) const {
        Q_UNUSED( parent );
        return 3;
    }

    int rowCount( const QModelIndex& parent = QModelIndex() ) const {
        Q_UNUSED( parent );
        return 8;
    }

    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const
    {
        if ( role == Qt::DisplayRole )
            return QString::number( (index.row()+1) * (index.column()+1) );
        else
            return QVariant();
    }

    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
    {
        if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
            return QString( "Column %1" ).arg( section+1 );
        else
            return QVariant();
    }
};


class Thread : public QThread
{
    virtual void run() {

        // Create a report
        KDReports::Report report;

        report.setWatermarkImage( QImage( ":/background.jpg" ) );
        report.setWatermarkText( QString() );

        KDReports::ImageElement imageElement(QImage(":/logo.png"));
        report.header().addElement(imageElement);

        // Add a text element for the title
        KDReports::TextElement titleElement( QObject::tr( "KD Reports Hello world example" ) );
        titleElement.setPointSize( 18 );
        report.addElement( titleElement, Qt::AlignHCenter );

        // A bit of spacing (10 mm)
        report.addVerticalSpacing( 10 );

        // Add another text element, demonstrating "<<" operator
        KDReports::TextElement textElement;
        textElement << QObject::tr( "This is a sample report produced with KD Reports." );
        textElement << "\n";
        textElement << QObject::tr( "Klarälvdalens Datakonsult AB, Platform-independent software solutions" );
        textElement << "\n";
        report.addElement( textElement );

        // Create a table model, which will be used by the table element
        HelloWorldTableModel tableModel;

        // A bit of spacing (10 mm)
        report.addVerticalSpacing( 10 );

        // Add an "auto table" (a table that shows the contents of a model)
        KDReports::AutoTableElement tableElement( &tableModel );
        tableElement.setBorder( 1 );
        report.addElement( tableElement );

        // Add many paragraphs, to demonstrate page breaking
        for ( int i = 1; i < 100 ; ++i ) {
            report.addElement( KDReports::HtmlElement( QString::fromLatin1( "<b>Customer</b> <em>%1</em>" ).arg(i) ), Qt::AlignLeft );
            report.addInlineElement( KDReports::TextElement( "  - 2006" ) );
        }

        // Add a page break
        report.addPageBreak();

        report.addElement( KDReports::TextElement( "This is the last page" ) );

        // To export to an image file:
        //qDebug() << "Exporting to output.png";
        report.exportToImage( QSize(200, 1200), "output.png", "PNG" ); // deleted by main() below

        // To export to a PDF file:
        //qDebug() << "Exporting to output.pdf";
        report.exportToFile( "output.pdf" ); // deleted by main() below
    }
};

class XmlThread : public QThread
{
    virtual void run() {
        KDReports::Report report;
        report.associateTextValue( "title_element", "Price list example" );
        report.associateImageValue( "image_system", QImage( ":/system.png" ) );
        QFile reportFile( ":/PriceList.xml" );
        if( !reportFile.open( QIODevice::ReadOnly ) ) {
            Q_ASSERT( false );
        }
        KDReports::ErrorDetails details;
        if( !report.loadFromXML( &reportFile, &details ) ) {
            qWarning( "ERROR: %s", qPrintable( details.message() ) );
        }
        report.exportToImage( QSize(800, 600), "xml.png", "PNG" ); // deleted by main() below
        report.exportToFile( "xml.pdf" ); // deleted by main() below
    }
};

int main( int argc, char** argv ) {
    QApplication app( argc, argv );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
#endif

    Thread thr;
    thr.start();
    thr.wait();
    Q_ASSERT(QFile::exists("output.pdf"));
    Q_ASSERT(QFile::exists("output.png"));
    const bool keep = argc > 1 && QByteArray( argv[1] ) == "-k"; // -k == keep generated files
    if ( !keep ) {
        QFile::remove("output.pdf");
        QFile::remove("output.png");
    }

    XmlThread xmlThr;
    xmlThr.start();
    xmlThr.wait();
    Q_ASSERT(QFile::exists("xml.pdf"));
    Q_ASSERT(QFile::exists("xml.png"));
    if ( !keep ) {
        QFile::remove("xml.pdf");
        QFile::remove("xml.png");
    }

    return 0; // app.exec();
}

