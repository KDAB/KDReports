/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsCell.h"
#include "KDReportsElement.h"
#include "KDReportsElementData_p.h"
#include "KDReportsReportBuilder_p.h"

class KDReports::CellPrivate
{
public:
    QList<ElementData> m_elements;
    int m_columnSpan = 1;
    int m_rowSpan = 1;
};

KDReports::Cell::Cell()
    : d(new CellPrivate)
{
}

KDReports::Cell::Cell(const Cell &other)
    : Element(other)
    , d(new CellPrivate(*other.d))
{
}

KDReports::Cell &KDReports::Cell::operator=(const Cell &other)
{
    if (&other == this)
        return *this;
    Element::operator=(other);
    *d = *other.d;
    return *this;
}

KDReports::Cell::~Cell()
{
}

void KDReports::Cell::setColumnSpan(int columnSpan)
{
    d->m_columnSpan = columnSpan;
}

int KDReports::Cell::columnSpan() const
{
    return d->m_columnSpan;
}

void KDReports::Cell::setRowSpan(int rowSpan)
{
    d->m_rowSpan = rowSpan;
}

int KDReports::Cell::rowSpan() const
{
    return d->m_rowSpan;
}

void KDReports::Cell::addInlineElement(const Element &element)
{
    d->m_elements.append(KDReports::ElementData(element.clone()));
}

void KDReports::Cell::addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment)
{
    d->m_elements.append(KDReports::ElementData(element.clone(), horizontalAlignment));
}

void KDReports::Cell::addVariable(VariableType variable)
{
    d->m_elements.append(KDReports::ElementData(variable));
}

void KDReports::Cell::addVerticalSpacing(qreal space)
{
    d->m_elements.append(KDReports::ElementData(KDReports::ElementData::VerticalSpacing, space));
}

void KDReports::Cell::build(ReportBuilder &builder) const
{
    foreach (const KDReports::ElementData &ed, d->m_elements) {
        switch (ed.m_type) {
        case KDReports::ElementData::Inline:
            builder.addInlineElement(*ed.m_element);
            break;
        case KDReports::ElementData::Block:
            builder.addBlockElement(*ed.m_element, ed.m_align);
            break;
        case KDReports::ElementData::Variable:
            builder.addVariable(ed.m_variableType);
            break;
        case KDReports::ElementData::VerticalSpacing:
            builder.addVerticalSpacing(ed.m_value);
            break;
        }
    }
}
