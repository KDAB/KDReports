/****************************************************************************
** Copyright (C) 2007-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
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

class KDReports::Cell::Private
{
public:
    Private()
        : m_columnSpan( 1 ),
          m_rowSpan( 1 )
    {}
    ~Private() {
        foreach( const ElementData& ed, m_elements )
            delete ed.m_element;
    }
    struct ElementData
    {
        enum Type { Inline, Block, Variable };
        // inline
        ElementData( Element* elem )
            : m_element( elem ), m_type( Inline ) {} // m_align not used
        // block
        ElementData( Element* elem, Qt::AlignmentFlag a )
            : m_element( elem ), m_type( Block ), m_align( a ) {}
        // inline variable
        ElementData( KDReports::VariableType variable )
            : m_element( 0 ), m_type( Variable ), m_variableType( variable ) {}
        Element* m_element;
        Type m_type : 3;
        union {
            KDReports::VariableType m_variableType;
            Qt::AlignmentFlag m_align;
        };
    };
    QList<ElementData> m_elements;
    int m_columnSpan;
    int m_rowSpan;
};

KDReports::Cell::Cell()
    : d( new Private )
{
}

KDReports::Cell::Cell(const Cell &other)
    : Element( other ), d( new Private( *other.d ) )
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
    d->m_elements.append( Private::ElementData( element.clone() ) );
}

void KDReports::Cell::addElement( const Element& element, Qt::AlignmentFlag horizontalAlignment )
{
    d->m_elements.append( Private::ElementData( element.clone(), horizontalAlignment ) );
}

void KDReports::Cell::addVariable( VariableType variable )
{
    d->m_elements.append( Private::ElementData( variable ) );
}

void KDReports::Cell::build( ReportBuilder& builder ) const
{
    foreach( const Private::ElementData& ed, d->m_elements )
    {
        switch ( ed.m_type ) {
        case Private::ElementData::Inline:
            builder.addInlineElement( *ed.m_element );
            break;
        case Private::ElementData::Block:
            builder.addBlockElement( *ed.m_element, ed.m_align );
            break;
        case Private::ElementData::Variable:
            builder.addVariable( ed.m_variableType );
            break;
        }
    }
}
