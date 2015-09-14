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

#include "KDReportsChartElement.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsTextDocument_p.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsReport.h"
#include "KDReportsReport_p.h" // modelForKey
#include "KDReportsChartTextObject_p.h"
#ifdef HAVE_KDCHART
#include <KDChartChart>
#include <KDChartBarDiagram>
//#include <KDChartSerializer>
#endif
#include <QFile>
#include <QPainter>
#include <QDebug>
#include <QTextDocument>
#include <QUrl>
#include <QLabel>

class KDReports::ChartElementPrivate : public QSharedData
{
public:
    ChartElementPrivate() : m_tableModel( 0 ), m_size( 100, 100 ), m_unit( KDReports::Millimeters ),
#ifdef HAVE_KDCHART
                m_chart( 0 ),
#endif
                m_deleteChart( false ) {}

    ~ChartElementPrivate()
    {
#ifdef HAVE_KDCHART
        if ( m_deleteChart )
            delete m_chart;
#endif
    }
    QAbstractItemModel* m_tableModel;
    QSizeF m_size;
    KDReports::Unit m_unit;
#ifdef HAVE_KDCHART
    mutable KDChart::Chart* m_chart;
#endif
    mutable bool m_deleteChart;
};

KDReports::ChartElement::ChartElement( QAbstractItemModel* tableModel )
    : d( new ChartElementPrivate )
{
    d->m_tableModel = tableModel;
}

KDReports::ChartElement::ChartElement( const QString& modelKey ) :
    d( new ChartElementPrivate )
{
    if ( !modelKey.isEmpty() )
        d->m_tableModel = KDReports::modelForKey( modelKey );
}

KDReports::ChartElement::ChartElement( KDChart::Chart* chart ) :
    d( new ChartElementPrivate )
{
    d->m_tableModel = 0;
#ifdef HAVE_KDCHART
    d->m_chart = chart;
#else
    Q_UNUSED( chart );
#endif
}


KDReports::ChartElement::ChartElement(const ChartElement &other)
    : Element(other), d( other.d )
{
}

KDReports::ChartElement & KDReports::ChartElement::operator=(const ChartElement &other)
{
    if ( &other == this )
        return *this;
    Element::operator=( other );
    d = other.d;
    return *this;
}

KDReports::ChartElement::~ChartElement()
{
}

void KDReports::ChartElement::setChart( KDChart::Chart* chart )
{
#ifdef HAVE_KDCHART
    if ( d->m_deleteChart )
        delete d->m_chart;
    d->m_chart = chart;
    d->m_deleteChart = false;
#else
    Q_UNUSED( chart );
#endif
}

void KDReports::ChartElement::setSize( qreal width, qreal height, Unit unit )
{
    d->m_size = QSizeF( width, height );
    d->m_unit = unit;
}

void KDReports::ChartElement::build( ReportBuilder& builder ) const
{
#ifdef HAVE_KDCHART
    if( !d->m_tableModel && !d->m_chart )
        return; // no model has been associated yet - give up
#else
    if( !d->m_tableModel )
        return; // no model has been associated yet - give up
#endif

#ifdef HAVE_KDCHART

    if ( !d->m_chart ) {
        d->m_chart = new KDChart::Chart;
        d->m_deleteChart = true;
    }

    if ( d->m_chart->coordinatePlane()->diagram() == 0 ) {
        KDChart::BarDiagram* diagram = new KDChart::BarDiagram();
        diagram->setModel( d->m_tableModel );
        d->m_chart->coordinatePlane()->replaceDiagram( diagram );
    }

#if 0 // TODO, but not here...
    KDChart::Serializer serializer( d->m_chart, d->m_tableModel );
    QFile file("testSettings.xml");
    if ( file.open( QIODevice::ReadOnly ) ) {
        if ( serializer.read( &file ) ) {
            qDebug() << "Success loading XML for chart";
        }
    }
#endif

    QTextCursor& cursor = builder.cursor();
    QTextCharFormat fmt;

    fmt.setObjectType( ChartTextObject::ChartObjectTextFormat );
    fmt.setProperty( ChartTextObject::Size, d->m_size );
    fmt.setProperty( ChartTextObject::Unit, int( d->m_unit ) );
    fmt.setProperty( ChartTextObject::ChartObject, QVariant::fromValue( d->m_chart ) );

    cursor.insertText( QString( QChar::ObjectReplacementCharacter ), fmt );

#else
    Q_UNUSED( builder );
    qWarning("ChartElement: cannot create chart, KD Reports was compiled without KD Chart support");
#endif
}

KDReports::Element* KDReports::ChartElement::clone() const
{
    return new ChartElement( *this );
}

QSizeF KDReports::ChartElement::size() const
{
    return d->m_size;
}

KDReports::Unit KDReports::ChartElement::unit() const
{
    return d->m_unit;
}

void KDReports::ChartElement::setModelKey(const QString &modelKey)
{
    if ( !modelKey.isEmpty() )
        d->m_tableModel = KDReports::modelForKey( modelKey );
}

void KDReports::ChartElement::setTableModel(QAbstractItemModel *tableModel)
{
    d->m_tableModel = tableModel;
}
