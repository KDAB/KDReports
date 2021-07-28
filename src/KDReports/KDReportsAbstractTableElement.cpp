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

#include "KDReportsAbstractTableElement.h"
#include "KDReportsLayoutHelper_p.h"
#include <QDebug>
#include <QTextCursor>
#include <QTextTableFormat>

class KDReports::AbstractTableElementPrivate
{
public:
    AbstractTableElementPrivate()
        : m_border(1.0)
        , m_borderBrush(Qt::darkGray /*like Qt does*/)
        , m_padding(0.5)
        , m_width(0)
        , m_unit(KDReports::Millimeters)
        , m_fontSpecified(false)
    {
    }
    qreal m_border;
    QBrush m_borderBrush;
    qreal m_padding;
    qreal m_width;
    KDReports::Unit m_unit;
    QFont m_defaultFont;
    bool m_fontSpecified;
    QVector<KDReports::AbstractTableElement::ColumnConstraint> m_constraints;
};

KDReports::AbstractTableElement::AbstractTableElement()
    : Element()
    , d(new AbstractTableElementPrivate)
{
}

KDReports::AbstractTableElement::AbstractTableElement(const AbstractTableElement &other)
    : Element(other)
    , d(new AbstractTableElementPrivate(*other.d))
{
}

KDReports::AbstractTableElement &KDReports::AbstractTableElement::operator=(const AbstractTableElement &other)
{
    if (&other == this)
        return *this;
    Element::operator=(other);
    *d = *other.d;
    return *this;
}

void KDReports::AbstractTableElement::setColumnConstraints(const QVector<KDReports::AbstractTableElement::ColumnConstraint> &constraints)
{
    d->m_constraints = constraints;
}

KDReports::AbstractTableElement::~AbstractTableElement()
{
    delete d;
}

void KDReports::AbstractTableElement::setBorder(qreal border)
{
    d->m_border = border;
}

qreal KDReports::AbstractTableElement::border() const
{
    return d->m_border;
}

void KDReports::AbstractTableElement::setBorderBrush(const QBrush &brush)
{
    d->m_borderBrush = brush;
}

QBrush KDReports::AbstractTableElement::borderBrush() const
{
    return d->m_borderBrush;
}

void KDReports::AbstractTableElement::setPadding(qreal padding)
{
    d->m_padding = padding;
}

qreal KDReports::AbstractTableElement::padding() const
{
    return d->m_padding;
}

void KDReports::AbstractTableElement::setWidth(qreal width, Unit unit)
{
    d->m_width = width;
    d->m_unit = unit;
}

qreal KDReports::AbstractTableElement::width() const
{
    return d->m_width;
}

KDReports::Unit KDReports::AbstractTableElement::unit() const
{
    return d->m_unit;
}

void KDReports::AbstractTableElement::setDefaultFont(const QFont &font)
{
    d->m_defaultFont = font;
    d->m_fontSpecified = true;
}

QFont KDReports::AbstractTableElement::defaultFont(bool *isSet) const
{
    *isSet = d->m_fontSpecified;
    return d->m_defaultFont;
}

void KDReports::AbstractTableElement::fillTableFormat(QTextTableFormat &tableFormat, QTextCursor &textDocCursor) const
{
    if (d->m_width) {
        if (d->m_unit == Millimeters) {
            tableFormat.setWidth(QTextLength(QTextLength::FixedLength, mmToPixels(d->m_width)));
        } else {
            tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, d->m_width));
        }
    }

    if (!d->m_constraints.isEmpty()) {
        QVector<QTextLength> constraints;
        constraints.reserve(d->m_constraints.size());
        for (const auto &c : qAsConst(d->m_constraints)) {
            QTextLength length;
            if (c.unit == Millimeters) {
                length = QTextLength(QTextLength::FixedLength, mmToPixels(c.width));
            } else {
                length = QTextLength(QTextLength::PercentageLength, c.width);
            }
            constraints.append(length);
        }
        tableFormat.setColumnWidthConstraints(constraints);
    }

    tableFormat.setBorder(border());
    tableFormat.setBorderBrush(borderBrush());
    tableFormat.setCellPadding(mmToPixels(padding()));
    tableFormat.setCellSpacing(0); // HTML-like table borders look so old century
    if (d->m_fontSpecified) {
        QTextCharFormat charFormat = textDocCursor.charFormat();
        charFormat.setFont(d->m_defaultFont);
        textDocCursor.setCharFormat(charFormat);
    }
}
