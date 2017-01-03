/****************************************************************************
** Copyright (C) 2007-2017 Klaralvdalens Datakonsult AB.  All rights reserved.
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
#include "KDReportsHeader_p.h" // for cleanupVariableProperties
#include <QDebug>

class KDReports::TextElementPrivate
{
public:
    TextElementPrivate()
        : m_bold(false),
        m_boldIsSet(false),
        m_italic(false),
        m_italicIsSet(false),
        m_underline(false),
        m_underlineIsSet(false),
        m_strikeout(false),
        m_strikeoutIsSet(false),
        m_fontSet(false),
        m_pointSize(0)
    {}

    QString m_string;
    QString m_id;

    QString m_fontFamily;
    bool m_bold;
    bool m_boldIsSet;
    bool m_italic;
    bool m_italicIsSet;
    bool m_underline;
    bool m_underlineIsSet;
    bool m_strikeout;
    bool m_strikeoutIsSet;
    bool m_fontSet;
    qreal m_pointSize;
    QColor m_foreground;
    QFont m_font;

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
    QTextCharFormat charFormat = cursor.charFormat();
    KDReports::cleanupVariableProperties( charFormat );
    if ( d->m_fontSet ) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        charFormat.setFont( d->m_font, QTextCharFormat::FontPropertiesSpecifiedOnly );
#else
        charFormat.setFont( d->m_font );
#endif
    }
    if ( d->m_boldIsSet )
        charFormat.setFontWeight( d->m_bold ? QFont::Bold : QFont::Normal );
    if ( d->m_italicIsSet )
        charFormat.setFontItalic( d->m_italic );
    if ( d->m_underlineIsSet )
        charFormat.setFontUnderline( d->m_underline );
    if ( d->m_strikeoutIsSet )
        charFormat.setFontStrikeOut( d->m_strikeout );
    if ( d->m_pointSize > 0 )
        charFormat.setFontPointSize( d->m_pointSize );
    if ( !d->m_fontFamily.isEmpty() )
        charFormat.setFontFamily( d->m_fontFamily );
    if ( d->m_foreground.isValid() )
        charFormat.setForeground( d->m_foreground );
    charFormat.setBackground( background() );
    cursor.setCharFormat( charFormat );
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
    d->m_fontFamily = family;
}

void KDReports::TextElement::setBold( bool bold )
{
    d->m_bold = bold;
    d->m_boldIsSet = true;
}

void KDReports::TextElement::setItalic( bool italic )
{
    d->m_italic = italic;
    d->m_italicIsSet = true;
}

void KDReports::TextElement::setUnderline( bool underline )
{
    d->m_underline = underline;
    d->m_underlineIsSet = true;
}

void KDReports::TextElement::setStrikeOut( bool strikeout )
{
    d->m_strikeout = strikeout;
    d->m_strikeoutIsSet = true;
}

void KDReports::TextElement::setPointSize( qreal size )
{
    d->m_pointSize = size;
}

void KDReports::TextElement::setFont( const QFont& font )
{
    d->m_font = font;
    d->m_fontSet = true;
}

void KDReports::TextElement::setTextColor( const QColor& color )
{
    d->m_foreground = color;
}

QColor KDReports::TextElement::textColor() const
{
    return d->m_foreground;
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
    return d->m_font;
}
