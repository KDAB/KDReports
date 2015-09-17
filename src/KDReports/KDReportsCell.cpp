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

#include "KDReportsCell.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsElement.h"
#include "KDReportsElementData_p.h"

class KDReports::CellPrivate
{
public:
    CellPrivate()
        : m_columnSpan( 1 ),
          m_rowSpan( 1 )
    {}
    ~CellPrivate() {
    }
    QList<ElementData> m_elements;
    int m_columnSpan;
    int m_rowSpan;
};

KDReports::Cell::Cell()
    : d( new CellPrivate )
{
}

KDReports::Cell::Cell(const Cell &other)
    : Element( other ), d( new CellPrivate( *other.d ) )
{
}

KDReports::Cell & KDReports::Cell::operator=(const Cell &other)
{
    if ( &other == this )
        return *this;
    Element::operator=( other );
    *d = *other.d;
    return *this;
}

KDReports::Cell::~Cell()
{
    delete d;
}

void KDReports::Cell::setColumnSpan( int columnSpan )
{
    d->m_columnSpan = columnSpan;
}

int KDReports::Cell::columnSpan() const
{
    return d->m_columnSpan;
}

void KDReports::Cell::setRowSpan( int rowSpan )
{
    d->m_rowSpan = rowSpan;
}

int KDReports::Cell::rowSpan() const
{
    return d->m_rowSpan;
}

void KDReports::Cell::addInlineElement( const Element& element )
{
    d->m_elements.append( KDReports::ElementData( element.clone() ) );
}

void KDReports::Cell::addElement( const Element& element, Qt::AlignmentFlag horizontalAlignment )
{
    d->m_elements.append( KDReports::ElementData( element.clone(), horizontalAlignment ) );
}

void KDReports::Cell::addVariable( VariableType variable )
{
    d->m_elements.append( KDReports::ElementData( variable ) );
}

void KDReports::Cell::build( ReportBuilder& builder ) const
{
    foreach( const KDReports::ElementData& ed, d->m_elements )
    {
        switch ( ed.m_type ) {
        case KDReports::ElementData::Inline:
            builder.addInlineElement( *ed.m_element );
            break;
        case KDReports::ElementData::Block:
            builder.addBlockElement( *ed.m_element, ed.m_align );
            break;
        case KDReports::ElementData::Variable:
            builder.addVariable( ed.m_variableType );
            break;
        }
    }
}
