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

#include "KDReportsErrorDetails.h"
#include "KDReportsErrorDetails_p.h"
#include <QObject>

KDReports::ErrorDetails::ErrorDetails()
    : d( new ErrorDetailsPrivate )
{
}

KDReports::ErrorDetails::ErrorDetails( const QString& message )
    : d( new ErrorDetailsPrivate )
{
    setDriverMessage( message );
}


QString KDReports::ErrorDetails::message() const
{
    if ( !hasError() )
        return QString();

    if ( d->m_line != -1 )
        return QObject::tr("Error on line %1, column %2: %3").arg(line()).arg(column()).arg(driverMessage());

    else
        return driverMessage();
}

int KDReports::ErrorDetails::line() const
{
    return d->m_line;
}

int KDReports::ErrorDetails::column() const
{
    return d->m_column;
}

bool KDReports::ErrorDetails::hasError() const
{
    return d->m_hasError;
}

QString KDReports::ErrorDetails::driverMessage() const
{
    return d->m_message;
}

void KDReports::ErrorDetails::setLine( int line )
{
    d->m_hasError = true;
    d->m_line = line;
}

void KDReports::ErrorDetails::setColumn( int column )
{
    d->m_hasError = true;
    d->m_column = column;
}

void KDReports::ErrorDetails::setDriverMessage( const QString& message )
{
    d->m_hasError = true;
    d->m_message = message;
}

KDReports::ErrorDetails::~ErrorDetails()
{
    delete d;
}

KDReports::ErrorDetails::ErrorDetails( const ErrorDetails& other )
    : d( new ErrorDetailsPrivate( *other.d ) )
{
}

