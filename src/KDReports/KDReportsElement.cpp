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

#include "KDReportsElement.h"
#include <QBrush>

class KDReports::ElementPrivate
{
public:
    QBrush m_background;
};

KDReports::Element::Element()
    : d( new ElementPrivate )
{
}

KDReports::Element::Element(const Element &other)
    : d( new ElementPrivate( *other.d ) )
{
}

KDReports::Element & KDReports::Element::operator=(const Element &other)
{
    if ( &other == this )
        return *this;
    *d = *other.d;
    return *this;
}

KDReports::Element::~Element()
{
    delete d;
}

void KDReports::Element::setBackground( const QBrush& brush )
{
    d->m_background = brush;
}

QBrush KDReports::Element::background() const
{
    return d->m_background;
}

