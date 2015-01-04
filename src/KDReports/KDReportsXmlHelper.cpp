/****************************************************************************
** Copyright (C) 2011-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#include "KDReportsXmlHelper.h"
#include <QDebug>

Qt::AlignmentFlag KDReports::XmlHelper::stringToAlignment( const QString& str )
{
    if( str == QLatin1String( "left" ) )
        return Qt::AlignLeft;
    else if( str == QLatin1String( "right" ) )
        return Qt::AlignRight;
    else if( str == QLatin1String( "hcenter" ) )
        return Qt::AlignHCenter;
    else
        qWarning( "Unexpected alignment flag in KDReports::Report::stringToAlignment(): %s", qPrintable( str ) );
    return Qt::AlignLeft;
}

KDReports::VariableType KDReports::XmlHelper::stringToVariableType( const QString& type )
{
    if( type == QLatin1String( "pagenumber" ) )
        return KDReports::PageNumber;
    else if( type == QLatin1String( "pagecount" ) )
        return KDReports::PageCount;
    else if( type == QLatin1String( "textdate" ) )
        return KDReports::TextDate;
    else if( type == QLatin1String( "isodate" ) )
        return KDReports::ISODate;
    else if( type == QLatin1String( "localedate" ) )
        return KDReports::LocaleDate;
    else if( type == QLatin1String( "texttime" ) )
        return KDReports::TextTime;
    else if( type == QLatin1String( "isotime" ) )
        return KDReports::ISOTime;
    else if( type == QLatin1String( "localetime" ) )
        return KDReports::LocaleTime;
    qWarning( "Unexpected variable type: %s", qPrintable( type ) );
    return KDReports::PageNumber;
}

QColor KDReports::XmlHelper::readColor( const QDomElement& element, const char* attributeName )
{
    QColor ret;
    const QString name = element.attribute( QLatin1String( attributeName ) );
    if ( !name.isEmpty() ) {
        ret = QColor( name );
    }
    return ret;
}

QColor KDReports::XmlHelper::readBackground( const QDomElement& element )
{
    QColor ret;
    if ( element.hasAttribute( QLatin1String( "background" ) ) ) {
        const QString name = element.attribute( QLatin1String( "background" ) );
        ret = QColor( name );
    } else if( element.hasAttribute( QLatin1String( "bgred" ) ) &&
        element.hasAttribute( QLatin1String( "bggreen" ) ) &&
        element.hasAttribute( QLatin1String( "bgblue" ) ) ) {
        int red = 0, green = 0, blue = 0;
        bool ok = true;
        red = element.attribute( QLatin1String( "bgred" ) ).toInt( &ok );
        if( ok ) {
            green = element.attribute( QLatin1String( "bggreen" ) ).toInt( &ok );
            if( ok ) {
                blue = element.attribute( QLatin1String( "bgblue" ) ).toInt( &ok );
                if( ok ) {
                    ret.setRed( red );
                    ret.setGreen( green );
                    ret.setBlue( blue );
                }
            }
        }
    }
    return ret;
}

KDReports::HeaderLocations KDReports::XmlHelper::parseHeaderLocation( const QString& xmlAttr )
{
    if ( xmlAttr.isEmpty() )
        return KDReports::AllPages;
    KDReports::HeaderLocations loc;
    const QStringList tokens = xmlAttr.toLower().split( QLatin1Char( ',' ) );
    foreach( const QString& s, tokens ) {
        QString token = s.trimmed();
        if ( token == QLatin1String( "first" ) )
            loc |= KDReports::FirstPage;
        else if ( token == QLatin1String( "last" ) )
            loc |= KDReports::LastPage;
        else if ( token == QLatin1String( "odd" ) )
            loc |= KDReports::OddPages;
        else if ( token == QLatin1String( "even" ) )
            loc |= KDReports::EvenPages;
        else if ( token == QLatin1String( "all" ) )
            loc |= KDReports::AllPages;
        else
            qWarning() << "Found unexpected token in header location attribute:" << token;
    }
    return loc;
}
