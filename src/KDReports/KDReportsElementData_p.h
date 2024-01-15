/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSELEMENTDATA_P_H
#define KDREPORTSELEMENTDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Reports API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#include "KDReportsElement.h"
#include "KDReportsVariableType.h"

namespace KDReports {

/**
 * @internal
 * Storage for Cell and Frame contents
 */
struct ElementData
{
    enum Type
    {
        Inline,
        Block,
        Variable,
        VerticalSpacing
    };
    // inline
    ElementData(Element *elem)
        : m_element(elem)
        , m_type(Inline)
    {
    } // m_align not used
    // block
    ElementData(Element *elem, Qt::AlignmentFlag a)
        : m_element(elem)
        , m_type(Block)
        , m_align(a)
    {
    }
    // inline variable
    ElementData(KDReports::VariableType variable)
        : m_element(nullptr)
        , m_type(Variable)
        , m_variableType(variable)
    {
    }
    // vertical spacing
    ElementData(Type type, qreal value)
        : m_element(nullptr)
        , m_type(type)
        , m_value(value)
    {
    }

    // copy ctor
    ElementData(const ElementData &other)
    {
        operator=(other);
    }
    ElementData &operator=(const ElementData &other)
    {
        m_element = other.m_element ? other.m_element->clone() : nullptr;
        m_type = other.m_type;
        switch (m_type) {
        case Block:
            m_align = other.m_align;
            break;
        case Inline:
            break;
        case Variable:
            m_variableType = other.m_variableType;
            break;
        case VerticalSpacing:
            m_value = other.m_value;
            break;
        }
        return *this;
    }
    ~ElementData()
    {
        delete m_element;
    }

    Element *m_element;
    Type m_type : 3;
    union {
        KDReports::VariableType m_variableType;
        Qt::AlignmentFlag m_align;
        qreal m_value;
    };
};
}

#endif
