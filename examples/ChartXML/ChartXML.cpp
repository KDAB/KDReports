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

#include <QApplication>
#include <QDebug>

#include <KDReports>
#include <KDReportsChartElement.h>
#include <KDReportsXmlElementHandler.h>
#include <QPrintDialog>
#include <QMessageBox>
#include <QFile>
#include <TableModel.h>
#include <QDomElement>

#include <KDChartChart>
#include <KDChartPieDiagram>
#include <KDChartPolarCoordinatePlane>

class MyXmlElementHandler : public KDReports::XmlElementHandler
{
public:
    MyXmlElementHandler( KDChart::Chart* chart )
        : m_chart( chart )
    {}
    virtual bool chartElement( KDReports::ChartElement& chartElement, QDomElement& xmlElement )
    {
        qDebug() << "chartElement called";
        Q_UNUSED( xmlElement );
        chartElement.setChart( m_chart );
        return true;
    }
private:
    KDChart::Chart* m_chart;
};

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    QFile reportFile( ":/Chart.xml" );
    if( !reportFile.open( QIODevice::ReadOnly ) ) {
        QMessageBox::warning(
            0, QObject::tr( "Warning" ),
            QObject::tr( "Could not open report description file 'Chart.xml', which should have been compiled into the application." ) );
        return -1;
    }

    // Prepare chart model
    TableModel chartModel;
    chartModel.setDataHasVerticalHeaders( false );
    chartModel.loadFromCSV( ":/chart_model" );

    // Prepare chart
    KDChart::Chart chart;
    chart.replaceCoordinatePlane( new KDChart::PolarCoordinatePlane( &chart ) );
    KDChart::PieDiagram* diagram = new KDChart::PieDiagram();
    diagram->setModel( &chartModel );
    chart.coordinatePlane()->replaceDiagram( diagram );
    
    MyXmlElementHandler myHandler( &chart );
    report.setXmlElementHandler( &myHandler );

    KDReports::ErrorDetails details;
    if( !report.loadFromXML( &reportFile, &details ) ) {
        QMessageBox::warning( 0, QObject::tr( "Warning" ), QObject::tr( "Could not parse report description file:\n%1" ).arg(details.message()) );
        reportFile.close();
        return -2;
    }

    // show a print preview:
    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

