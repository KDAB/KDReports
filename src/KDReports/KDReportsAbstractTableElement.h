/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSABSTRACTTABLEELEMENT_H
#define KDREPORTSABSTRACTTABLEELEMENT_H

#include "KDReportsElement.h"
#include "KDReportsUnit.h"

QT_BEGIN_NAMESPACE
template<typename T>
class QList;
class QFont;
class QTextTableFormat;
class QTextCursor;
QT_END_NAMESPACE

namespace KDReports {
class AbstractTableElementPrivate;

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
    void setBorder(qreal border);

    /**
     * Returns the width of the table border
     */
    qreal border() const;

    /**
     * Specifies the color (more generally, the brush) of the border.
     * \since 1.2
     */
    void setBorderBrush(const QBrush &brush);

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
    void setPadding(qreal padding);

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
    void setWidth(qreal width, Unit unit = Millimeters);

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
    void setDefaultFont(const QFont &font);

    /**
     * \return the default font set by setDefaultFont(), if any
     * \param isSet set to true if a default font was set, false otherwise
     * \since 1.8
     */
    QFont defaultFont(bool *isSet) const;

    struct ColumnConstraint
    {
        qreal width;
        Unit unit;
    };
    /**
     * Sets constraints on the column widths (fixed or proportional)
     * \param constraints vector of constraints
     * \since 1.9
     */
    void setColumnConstraints(const QVector<ColumnConstraint> &constraints);

protected:
    /**
     * Protected constructor, for derived classes only.
     */
    AbstractTableElement();

    /**
     * Copies a table element.
     * The model and settings (and cell elements, when using addElement) are copied over.
     */
    AbstractTableElement(const AbstractTableElement &other);
    /**
     * Copies the table and settings from another table element.
     */
    AbstractTableElement &operator=(const AbstractTableElement &other);

    /**
     * Destructor. Deletes internal data.
     */
    ~AbstractTableElement() override;

    /**
     * @internal for derived classes only.
     */
    void fillTableFormat(QTextTableFormat &tableFormat, QTextCursor &textDocCursor) const;

private:
    std::unique_ptr<AbstractTableElementPrivate> d;
};

}

#endif /* KDREPORTSABSTRACTTABLEELEMENT_H */
