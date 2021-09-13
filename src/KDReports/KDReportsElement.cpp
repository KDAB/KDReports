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

#include "KDReportsElement.h"
#include <QBrush>

class KDReports::ElementPrivate
{
public:
    QBrush m_background;
};

KDReports::Element::Element()
    : d(new ElementPrivate)
{
}

KDReports::Element::Element(const Element &other)
    : d(new ElementPrivate(*other.d))
{
}

KDReports::Element &KDReports::Element::operator=(const Element &other)
{
    if (&other == this)
        return *this;
    *d = *other.d;
    return *this;
}

KDReports::Element::~Element()
{
}

void KDReports::Element::setBackground(const QBrush &brush)
{
    d->m_background = brush;
}

QBrush KDReports::Element::background() const
{
    return d->m_background;
}
