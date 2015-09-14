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

#ifndef KDREPORTSCELL_H
#define KDREPORTSCELL_H

#include "KDReportsElement.h"
#include "KDReportsVariableType.h"
#include <QtCore/Qt>
#include <QMap>
#include <QPair>

namespace KDReports
{
class ReportBuilder;
class Element;
class CellContentMap;
class TableElement;
class CellPrivate;

/**
 * This class presents a cell in a table.
 * To add an element to a cell, first get hold of the
 * right cell using Table::cell(), then add elements to it.
 */
class KDREPORTS_EXPORT Cell : public Element
{
public:
    /**
     * Set the number of columns that this cell will span.
     * The default is 1.
     */
    void setColumnSpan( int columnSpan );
    /**
     * Returns the number of columns that this cell will span.
     */
    int columnSpan() const;

    /**
     * Set the number of rows that this cell will span.
     * The default is 1.
     */
    void setRowSpan( int rowSpan );
    /**
     * Returns the number of rows that this cell will span.
     */
    int rowSpan() const;

    /**
     * Adds an element to the cell, next to the previous element, in the same paragraph.
     */
    void addInlineElement( const Element& element );

    /**
     * Adds an element to the cell, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     */
    void addElement( const Element& element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft );

    /**
     * Adds an variable in the text of the current paragraph.
     * @see KDReportsHeader::addVariable
     */
    void addVariable( VariableType variable );

    /**
     * @internal
     * @reimp
     */
    void build( ReportBuilder& builder ) const;

    /**
     * @internal - not supported
     */
    Element* clone() const { return 0; } //krazy:exclude=inline

private:
    // TableElement needs to create/destroy/copy cells
    friend class TableElement;
    friend class QMap<QPair<int, int>, Cell>;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    friend struct QMapData<QPair<int, int>, Cell>;
    friend struct QMapNode<QPair<int, int>, Cell>;
#endif
    Cell();
    ~Cell();
    Cell(const Cell &other);
    Cell &operator=(const Cell &other);

    CellPrivate* const d;
};

}

#endif /* KDREPORTSCELL_H */
