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

#include "KDReportsHLineElement.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsHLineTextObject_p.h"
#include "KDReportsLayoutHelper_p.h" // mmToPixels
#include <QDebug>

class KDReports::HLineElementPrivate
{
public:

    QString m_id;
    QColor m_color;
    
    int m_thickness;
    int m_margin;
};

KDReports::HLineElement::HLineElement()
    : Element(), d( new HLineElementPrivate )
{
    d->m_color = Qt::gray;
    d->m_thickness = 2;
    d->m_margin = 6;
}

KDReports::HLineElement::HLineElement( const HLineElement &other )
    : Element(other), d( new HLineElementPrivate( *other.d ) )
{
}

KDReports::HLineElement & KDReports::HLineElement::operator=( const HLineElement &other )
{
    if ( &other == this )
        return *this;
    Element::operator=( other );
    *d = *other.d;
    return *this;
}

KDReports::HLineElement::~HLineElement()
{
    delete d;
}

void KDReports::HLineElement::setColor( const QColor& color )
{
    d->m_color = color;
}

QColor KDReports::HLineElement::color() const
{
    return d->m_color;
}

void KDReports::HLineElement::setThickness( int t )
{
    d->m_thickness = t;
}

int KDReports::HLineElement::thickness() const
{
    return d->m_thickness;
}

void KDReports::HLineElement::setMargin(int m)
{
    d->m_margin = m; // in mm
}

int KDReports::HLineElement::margin() const
{
    return d->m_margin;
}

void KDReports::HLineElement::build( ReportBuilder& builder ) const
{
    QTextCursor& cursor = builder.cursor();

    QTextCharFormat fmt;

    fmt.setObjectType( HLineTextObject::HLineTextFormat );
    fmt.setProperty( HLineTextObject::Color, d->m_color );
    fmt.setProperty( HLineTextObject::Thickness, double( d->m_thickness ) );
    //qDebug() << "Setting Margin to" << d->m_margin << "mm" << mmToPixels( d->m_margin ) << "px";
    fmt.setProperty( HLineTextObject::Margin, double( mmToPixels( d->m_margin ) ) );

    cursor.insertText( QString(QChar::ObjectReplacementCharacter), fmt);
}

KDReports::Element* KDReports::HLineElement::clone() const
{
    return new HLineElement( *this );
}

