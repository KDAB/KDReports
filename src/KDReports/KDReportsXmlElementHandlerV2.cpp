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

#include "KDReportsXmlElementHandlerV2.h"
#ifdef Q_CC_MSVC
// Q_UNUSED requires the type to be known in msvc
#include "KDReportsHLineElement.h"
#include <QList>
#endif
#include <QDomElement>

KDReports::XmlElementHandlerV2::XmlElementHandlerV2()
{
}

KDReports::XmlElementHandlerV2::~XmlElementHandlerV2()
{
}

bool KDReports::XmlElementHandlerV2::vspace(int &size, QDomElement &xmlElement)
{
    Q_UNUSED( xmlElement );
    Q_UNUSED( size );
    return true;
}

bool KDReports::XmlElementHandlerV2::hLineElement(KDReports::HLineElement &hLineElement, QDomElement &xmlElement)
{
    Q_UNUSED( hLineElement );
    Q_UNUSED( xmlElement );
    return true;
}

bool KDReports::XmlElementHandlerV2::variable( KDReports::VariableType &type, QDomElement &xmlElement)
{
    Q_UNUSED( type );
    Q_UNUSED( xmlElement );
    return true;
}

bool KDReports::XmlElementHandlerV2::paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement& xmlElement )
{
    Q_UNUSED( left );
    Q_UNUSED( top );
    Q_UNUSED( right );
    Q_UNUSED( bottom );
    Q_UNUSED( xmlElement )
    return true;
}

bool KDReports::XmlElementHandlerV2::tabs(QList<QTextOption::Tab> &tabs, const QDomElement &xmlElement)
{
    Q_UNUSED( tabs );
    Q_UNUSED( xmlElement );
    return true;
}
