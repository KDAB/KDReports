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

#include "KDReportsTextElement.h"
#include "KDReportsReportBuilder_p.h"
#include <QDebug>

class KDReports::TextElementPrivate
{
public:
    QString m_string;
    QString m_id;
    QTextCharFormat m_charFormat;
};

KDReports::TextElement::TextElement( const QString& string )
    : Element(), d( new TextElementPrivate )
{
    d->m_string = string;
}

KDReports::TextElement::TextElement( const TextElement &other )
    : Element(other), d( new TextElementPrivate( *other.d ) )
{
}

KDReports::TextElement & KDReports::TextElement::operator=( const TextElement &other )
{
    if ( &other == this )
        return *this;
    Element::operator=( other );
    *d = *other.d;
    return *this;
}

KDReports::TextElement::~TextElement()
{
    delete d;
}

void KDReports::TextElement::build( ReportBuilder& builder ) const
{
    QTextCursor& cursor = builder.cursor();
    const int charPosition = cursor.position();
    d->m_charFormat.setBackground( background() );
    cursor.setCharFormat( d->m_charFormat );
    cursor.insertText( d->m_string );
    if ( !d->m_id.isEmpty() )
        builder.currentDocumentData().setTextValueMarker( charPosition, d->m_id, d->m_string.length(), false );
}

KDReports::TextElement& KDReports::TextElement::operator<<( const QString& str )
{
    d->m_string.append( str );
    return *this;
}

void KDReports::TextElement::setText( const QString& text )
{
    d->m_string = text;
}

void KDReports::TextElement::setFontFamily( const QString& family )
{
    d->m_charFormat.setFontFamily( family );
}

void KDReports::TextElement::setBold( bool bold )
{
    d->m_charFormat.setFontWeight( bold ? QFont::Bold : QFont::Normal );
}

void KDReports::TextElement::setItalic( bool italic )
{
    d->m_charFormat.setFontItalic( italic );
}

void KDReports::TextElement::setUnderline( bool underline )
{
    d->m_charFormat.setFontUnderline( underline );
}

void KDReports::TextElement::setStrikeOut( bool strikeout )
{
    d->m_charFormat.setFontStrikeOut( strikeout );
}

void KDReports::TextElement::setPointSize( qreal size )
{
    d->m_charFormat.setFontPointSize( size );
}

void KDReports::TextElement::setFont( const QFont& font )
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    d->m_charFormat.setFont( font, QTextCharFormat::FontPropertiesSpecifiedOnly );
#else
    d->m_charFormat.setFont( font );
#endif
}

void KDReports::TextElement::setTextColor( const QColor& color )
{
    d->m_charFormat.setForeground( color );
}

QColor KDReports::TextElement::textColor() const
{
    return d->m_charFormat.foreground().color();
}

KDReports::Element* KDReports::TextElement::clone() const
{
    return new TextElement( *this );
}

void KDReports::TextElement::setId( const QString& id )
{
    d->m_id = id;
}

QString KDReports::TextElement::id() const
{
    return d->m_id;
}

QString KDReports::TextElement::text() const
{
    return d->m_string;
}

QFont KDReports::TextElement::font() const
{
    return d->m_charFormat.font();
}

void KDReports::TextElement::setCharFormat( const QTextCharFormat& format )
{
    d->m_charFormat = format;
}
