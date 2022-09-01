/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsHLineElement.h"
#include "KDReportsHLineTextObject_p.h"
#include "KDReportsLayoutHelper_p.h" // mmToPixels
#include "KDReportsReportBuilder_p.h"
#include <QDebug>

class KDReports::HLineElementPrivate
{
public:
    QString m_id;
    QColor m_color;

    qreal m_thickness;
    int m_margin;
};

KDReports::HLineElement::HLineElement()
    : Element()
    , d(new HLineElementPrivate)
{
    d->m_color = Qt::gray;
    d->m_thickness = 2;
    d->m_margin = 6;
}

KDReports::HLineElement::HLineElement(const HLineElement &other)
    : Element(other)
    , d(new HLineElementPrivate(*other.d))
{
}

KDReports::HLineElement &KDReports::HLineElement::operator=(const HLineElement &other)
{
    if (&other == this)
        return *this;
    Element::operator=(other);
    *d = *other.d;
    return *this;
}

KDReports::HLineElement::~HLineElement()
{
}

void KDReports::HLineElement::setColor(const QColor &color)
{
    d->m_color = color;
}

QColor KDReports::HLineElement::color() const
{
    return d->m_color;
}

void KDReports::HLineElement::setThickness(qreal t)
{
    d->m_thickness = t;
}

qreal KDReports::HLineElement::thickness() const
{
    return d->m_thickness;
}

void KDReports::HLineElement::setMargin(int m)
{
    d->m_margin = m; // in mm
}

int KDReports::HLineElement::margin() const
{
    return d->m_margin;
}

void KDReports::HLineElement::build(ReportBuilder &builder) const
{
    QTextCursor &cursor = builder.cursor();

    QTextCharFormat fmt;

    fmt.setObjectType(HLineTextObject::HLineTextFormat);
    fmt.setProperty(HLineTextObject::Color, d->m_color);
    fmt.setProperty(HLineTextObject::Thickness, d->m_thickness);
    // qDebug() << "Setting Margin to" << d->m_margin << "mm" << mmToPixels( d->m_margin ) << "px";
    fmt.setProperty(HLineTextObject::Margin, double(mmToPixels(d->m_margin)));

    cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
}

KDReports::Element *KDReports::HLineElement::clone() const
{
    return new HLineElement(*this);
}
