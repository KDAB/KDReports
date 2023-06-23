/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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
    void fillConstraints(QTextTableFormat *tableFormat) const;

    qreal m_border = 1.0;
    QBrush m_borderBrush = QBrush(Qt::darkGray /*like Qt does*/);
    qreal m_padding = 0.5;
    qreal m_width = 0;
    KDReports::Unit m_unit = KDReports::Millimeters;
    QFont m_defaultFont;
    bool m_fontSpecified = false;
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

void KDReports::AbstractTableElementPrivate::fillConstraints(QTextTableFormat *tableFormat) const
{
    if (!m_constraints.isEmpty()) {
        QVector<QTextLength> constraints;
        constraints.reserve(m_constraints.size());
        for (const auto &c : qAsConst(m_constraints)) {
            QTextLength length; // Variable by default
            switch (c.unit) {
            case Millimeters:
                length = QTextLength(QTextLength::FixedLength, mmToPixels(c.width));
                break;
            case Percent:
                length = QTextLength(QTextLength::PercentageLength, c.width);
                break;
            }
            constraints.append(length);
        }
        tableFormat->setColumnWidthConstraints(constraints);
    }
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

    d->fillConstraints(&tableFormat);

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
