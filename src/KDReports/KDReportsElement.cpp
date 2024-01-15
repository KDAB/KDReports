/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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
