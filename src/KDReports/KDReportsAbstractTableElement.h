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

#ifndef KDREPORTSABSTRACTTABLEELEMENT_H
#define KDREPORTSABSTRACTTABLEELEMENT_H

#include "KDReportsElement.h"
#include "KDReportsUnit.h"

template <typename T> class QList;
class QFont;
class QTextTableFormat;
class QTextCursor;
namespace KDReports {

/**
 * Base class for TableElement and AutoTableElement.
 *
 */
class KDREPORTS_EXPORT AbstractTableElement : public Element
{
public:

    /**
     * Specifies the width of the border in pixels.
     * The default border has a width of 1.
     * Set it to 0 for a table without borders.
     */
    void setBorder( qreal border );

    /**
     * Returns the width of the table border
     */
    qreal border() const;

    /**
     * Specifies the color (more generally, the brush) of the border.
     * \since 1.2
     */
    void setBorderBrush( const QBrush& brush );

    /**
     * Returns the color (brush) of the table border
     * \since 1.2
     */
    QBrush borderBrush() const;

    /**
     * Specifies the size of the padding in millimeters.
     * Padding is the distance between the contents of the cell and the cell border,
     * all around the contents (above, below, on the left and on the right).
     * The default padding has a size of 0.5 mm.
     */
    void setPadding( qreal padding );

    /**
     * Returns the width of the cell's internal padding in millimeters.
     */
    qreal padding() const;

    /**
     * Sets the width of the table, in millimeters or as a percentage of the page width.
     *
     * For instance, for a table that should stretch to the full width of the page, call setWidth(100, KDReports::Percent).
     *
     * This is only available in WordProcessing mode.
     */
    void setWidth( qreal width, Unit unit = Millimeters );

    /**
     * \return the width of the table, in millimeters or as a percentage of the page width.
     * \see unit
     * \since 1.4
     */
    qreal width() const;

    /**
     * \return the width unit of the table.
     * \since 1.4
     */
    Unit unit() const;

    /**
     * Sets the default font used for text in this table.
     * In WordProcessing mode, per-cell fonts can override this.
     * \since 1.3
     */
    void setDefaultFont( const QFont& font );

    /**
     * Copies a table element.
     * The model and settings (and cell elements, when using addElement) are copied over.
     */
    AbstractTableElement(const AbstractTableElement &other);
    /**
     * Copies the table and settings from another table element.
     */
    AbstractTableElement &operator=(const AbstractTableElement &other);

protected:
    /**
     * Protected constructor, for derived classes only.
     */
    AbstractTableElement();

    /**
     * Destructor. Deletes internal data.
     */
    ~AbstractTableElement();

    /**
     * @internal for derived classes only.
     */
    void fillTableFormat( QTextTableFormat& tableFormat, QTextCursor& textDocCursor ) const;

private:
    class Private;
    Private* const d;
};

}

#endif /* KDREPORTSABSTRACTTABLEELEMENT_H */

