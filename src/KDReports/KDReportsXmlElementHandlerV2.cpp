/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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
