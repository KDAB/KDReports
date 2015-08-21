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

#ifndef KDREPORTSERRORDETAILS_H
#define KDREPORTSERRORDETAILS_H

#include "KDReportsGlobal.h"
#include <QtAlgorithms>

namespace KDReports {
class ErrorDetailsPrivate;
class XmlElementHandler;

/**
 * The ErrorDetails class is used for transferring errors when parsing XML
 */
class KDREPORTS_EXPORT ErrorDetails
{
public:
    ErrorDetails();
    explicit ErrorDetails( const QString& message );
    ErrorDetails( const ErrorDetails& other );


    ~ErrorDetails();
    inline ErrorDetails& operator=( const ErrorDetails& other );

    /**
     * @return a message including line and column number if available.
     */
    QString message() const;

    void setLine( int line );
    void setColumn( int column );

    /**
     * Set the lowlevel message. This excludes line and/or column numbers.
     */
    void setDriverMessage( const QString& message );

    int line() const;
    int column() const;

    /**
     * @return the lowlevel message, without any line or column numbers included.
     */
    QString driverMessage() const;

    bool hasError() const;

    inline void swap( ErrorDetails & other ); //krazy:exclude=inline
private:
    friend class XmlElementHandler;
    ErrorDetailsPrivate* d; //krazy:exclude=dpointer make this const
};

void ErrorDetails::swap( ErrorDetails & other )
{
    qSwap( d, other.d );
}


ErrorDetails& ErrorDetails::operator=( const ErrorDetails& other )
{
    ErrorDetails copy( other );
    swap( copy );
    return *this;
}

} // namespace KDReports

template <> inline void qSwap( KDReports::ErrorDetails & lhs, KDReports::ErrorDetails & rhs )
{
    lhs.swap( rhs );
}

#endif /* KDREPORTSERRORDETAILS_H */
