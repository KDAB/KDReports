/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsFrame.h"
#include "KDReportsElement.h"
#include "KDReportsElementData_p.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsReportBuilder_p.h"

#include <QDebug>
#include <QTextFrame>

class KDReports::FramePrivate
{
public:
    FramePrivate()
        : m_width(0)
        , m_height(0)
        , m_padding(0.5)
        , m_border(1.0)
        , m_widthUnit(KDReports::Millimeters)
        , m_heightUnit(KDReports::Millimeters)
    {
    }
    ~FramePrivate()
    {
    }
    QList<KDReports::ElementData> m_elements;
    qreal m_width;
    qreal m_height;
    qreal m_padding;
    qreal m_border;
    KDReports::Unit m_widthUnit;
    KDReports::Unit m_heightUnit;
    KDReports::Frame::Position m_position = KDReports::Frame::InFlow;
};

KDReports::Frame::Frame()
    : d(new FramePrivate)
{
}

KDReports::Frame::Frame(const Frame &other)
    : Element(other)
    , d(new FramePrivate(*other.d))
{
}

KDReports::Frame &KDReports::Frame::operator=(const Frame &other)
{
    if (&other == this)
        return *this;
    Element::operator=(other);
    *d = *other.d;
    return *this;
}

void KDReports::Frame::setWidth(qreal width, KDReports::Unit unit)
{
    d->m_width = width;
    d->m_widthUnit = unit;
}

void KDReports::Frame::setHeight(qreal height, KDReports::Unit unit)
{
    d->m_height = height;
    d->m_heightUnit = unit;
}

void KDReports::Frame::setPadding(qreal padding)
{
    d->m_padding = padding;
}

qreal KDReports::Frame::padding() const
{
    return d->m_padding;
}

void KDReports::Frame::setBorder(qreal border)
{
    d->m_border = border;
}

qreal KDReports::Frame::border() const
{
    return d->m_border;
}

void KDReports::Frame::setPosition(Position pos)
{
    d->m_position = pos;
}

KDReports::Frame::Position KDReports::Frame::position() const
{
    return d->m_position;
}

KDReports::Frame::~Frame()
{
}

void KDReports::Frame::addInlineElement(const Element &element)
{
    d->m_elements.append(KDReports::ElementData(element.clone()));
}

void KDReports::Frame::addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment)
{
    d->m_elements.append(KDReports::ElementData(element.clone(), horizontalAlignment));
}

void KDReports::Frame::addVariable(VariableType variable)
{
    d->m_elements.append(KDReports::ElementData(variable));
}

void KDReports::Frame::build(ReportBuilder &builder) const
{
    // prepare the frame
    QTextFrameFormat format;
    if (d->m_width) {
        if (d->m_widthUnit == Millimeters) {
            format.setWidth(QTextLength(QTextLength::FixedLength, mmToPixels(d->m_width)));
        } else {
            format.setWidth(QTextLength(QTextLength::PercentageLength, d->m_width));
        }
    }
    if (d->m_height) {
        if (d->m_heightUnit == Millimeters) {
            format.setHeight(QTextLength(QTextLength::FixedLength, mmToPixels(d->m_height)));
        } else {
            format.setHeight(QTextLength(QTextLength::PercentageLength, d->m_height));
        }
    }

    format.setPadding(mmToPixels(padding()));
    format.setBorder(d->m_border);
    // TODO borderBrush like in AbstractTableElement
    format.setPosition(static_cast<QTextFrameFormat::Position>(d->m_position)); // those enums have the same value

    QTextCursor &textDocCursor = builder.cursor();

    QTextFrame *frame = textDocCursor.insertFrame(format);

    QTextCursor contentsCursor = frame->firstCursorPosition();

    ReportBuilder contentsBuilder(builder.currentDocumentData(), contentsCursor, builder.report());
    contentsBuilder.copyStateFrom(builder);

    foreach (const KDReports::ElementData &ed, d->m_elements) {
        switch (ed.m_type) {
        case KDReports::ElementData::Inline:
            contentsBuilder.addInlineElement(*ed.m_element);
            break;
        case KDReports::ElementData::Block:
            contentsBuilder.addBlockElement(*ed.m_element, ed.m_align);
            break;
        case KDReports::ElementData::Variable:
            contentsBuilder.addVariable(ed.m_variableType);
            break;
        case KDReports::ElementData::VerticalSpacing:
            contentsBuilder.addVerticalSpacing(ed.m_value);
            break;
        }
    }

    textDocCursor.movePosition(QTextCursor::End);
}

KDReports::Element *KDReports::Frame::clone() const
{
    return new Frame(*this);
}
