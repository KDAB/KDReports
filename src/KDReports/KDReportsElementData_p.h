/****************************************************************************
** Copyright (C) 2007-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
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
        enum Type { Inline, Block, Variable };
        // inline
        ElementData( Element* elem )
            : m_element( elem ), m_type( Inline ) {} // m_align not used
        // block
        ElementData( Element* elem, Qt::AlignmentFlag a )
            : m_element( elem ), m_type( Block ), m_align( a ) {}
        // inline variable
        ElementData( KDReports::VariableType variable )
            : m_element( 0 ), m_type( Variable ), m_variableType( variable ) {}
        // copy ctor
        ElementData(const ElementData &other) {
            operator=(other);
        }
        ElementData &operator=(const ElementData &other) {
            m_element = other.m_element ? other.m_element->clone() : 0;
            m_type = other.m_type;
            m_variableType = other.m_variableType;
            m_align = other.m_align;
            return *this;
        }
        ~ElementData() { delete m_element; }

        Element* m_element;
        Type m_type : 3;
        union {
            KDReports::VariableType m_variableType;
            Qt::AlignmentFlag m_align;
        };
    };

}

#endif
