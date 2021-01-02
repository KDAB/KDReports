/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#include "KDReportsHeader.h"
#include "KDReportsHeader_p.h"
#include "KDReportsReport.h"
#include "KDReportsReport_p.h"
#include "KDReportsTextElement.h"
#include "KDReportsAbstractReportLayout_p.h"
#include "KDReportsLayoutHelper_p.h"
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QLocale>
#include <QTextBlock>

KDReports::Header::Header( KDReports::Report* report )
    : d( new HeaderPrivate( report ) )
{
    d->m_textDocument.contentDocument().setDefaultFont( report->d->m_layout->defaultFont() );
}

KDReports::Header::~Header()
{
    delete d;
}

void KDReports::Header::addInlineElement( const Element& element )
{
    d->m_builder.addInlineElementPublic( element );
    // don't add anything else here, it won't be called from the xml parser
}

void KDReports::Header::addElement( const Element& element, Qt::AlignmentFlag horizontalAlignment )
{
    d->m_builder.addBlockElementPublic( element, horizontalAlignment );
    // don't add anything else here, it won't be called from the xml parser
}

KDReports::TextDocument& KDReports::Header::doc()
{
    return d->m_textDocument;
}

static const int VariableTypeProperty = QTextFormat::UserProperty + 246;
static const int VariableLengthProperty = QTextFormat::UserProperty + 247;

namespace KDReports {
QString variableValue( int pageNumber, KDReports::Report* report, VariableType type )
{
    switch ( type ) {
    case PageNumber:
        return QString::number( pageNumber + 1 );
    case PageCount:
        return QString::number( report->numberOfPages() );
    case TextDate:
        return QDate::currentDate().toString( Qt::TextDate );
    case ISODate:
        return QDate::currentDate().toString( Qt::ISODate );
    case SystemLocaleShortDate:
        return QLocale::system().toString(QDate::currentDate(), QLocale::ShortFormat);
    case SystemLocaleLongDate:
        return QLocale::system().toString(QDate::currentDate(), QLocale::LongFormat);
    case LocaleDate:
    case DefaultLocaleShortDate:
        return QLocale().toString(QDate::currentDate(), QLocale::ShortFormat);
    case DefaultLocaleLongDate:
        return QLocale().toString(QDate::currentDate(), QLocale::LongFormat);
    case TextTime:
        return QTime::currentTime().toString( Qt::TextDate );
    case ISOTime:
        return QTime::currentTime().toString( Qt::ISODate );
    case LocaleTime:
        return QLocale().toString(QTime::currentTime(), QLocale::ShortFormat);
    default:
        qWarning() << "Program error, variable" << type << "not implemented";
    }
    return QString();
}
}

void KDReports::setVariableMarker( QTextDocument& textDoc, int pos, KDReports::VariableType variableType, int valueLength )
{
    QTextCursor c( &textDoc );
    c.setPosition( pos );
    c.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
    QTextCharFormat charFormat = c.charFormat();
    charFormat.setProperty( ResizableImageProperty, QVariant() );
    charFormat.setProperty( VariableTypeProperty, variableType );
    charFormat.setProperty( VariableLengthProperty, valueLength );
    c.setCharFormat( charFormat );
}

void KDReports::cleanupVariableProperties( QTextCharFormat &charFormat )
{
    charFormat.setProperty( ResizableImageProperty, QVariant() );
    charFormat.setProperty( VariableTypeProperty, QVariant() );
    charFormat.setProperty( VariableLengthProperty, QVariant() );
}

void KDReports::Header::addVariable( VariableType variable )
{
    d->m_builder.addVariablePublic( variable );
}

void KDReports::Header::addVerticalSpacing( qreal space )
{
    d->m_builder.addVerticalSpacingPublic( space );
}

void KDReports::Header::preparePaintingPage( int pageNumber )
{
    //qDebug() << "preparePaintingPage" << pageNumber;
    QTextCursor c( &d->m_textDocument.contentDocument() );
    do {
        c.movePosition( QTextCursor::NextCharacter );
        //qDebug() << c.block().text() << c.position();
        QTextCharFormat format = c.charFormat();
        if ( format.hasProperty( VariableTypeProperty ) ) {
            // go back one char, due to charFormat() being the format of the character -before- the cursor!
            c.movePosition( QTextCursor::PreviousCharacter );
            const VariableType variableType = static_cast<VariableType>(format.property(VariableTypeProperty).toInt());
            const int oldLength = format.property(VariableLengthProperty).toInt();
            //qDebug() << "Found variable type" << variableType << "length" << oldLength << "at pos" << c.position();
            const QString value = KDReports::variableValue( pageNumber, d->m_report, variableType );
            const int startPos = c.position();
            c.setPosition( c.position() + oldLength, QTextCursor::KeepAnchor );
            //qDebug() << "inserting text" << value << "with format" << c.charFormat().font();
            c.insertText( value ); // update variable value
            // update marker
            setVariableMarker( d->m_textDocument.contentDocument(), startPos, variableType, value.length() );
        }
    } while ( !c.atEnd() );
}

void KDReports::Header::setDefaultFont( const QFont& font )
{
    QFont f( font );
    f.setStyleStrategy( QFont::ForceOutline ); // bitmap fonts look awful in printed documents
    d->m_textDocument.contentDocument().setDefaultFont( f );
    d->m_builder.setDefaultFont( f );
}

QFont KDReports::Header::defaultFont() const
{
    return d->m_textDocument.contentDocument().defaultFont();
}

void KDReports::Header::setTabPositions( const QList<QTextOption::Tab>& tabs )
{
    d->m_builder.setTabPositions( tabs );
}

int KDReports::Header::currentPosition() const
{
    return d->m_builder.currentPosition();
}
