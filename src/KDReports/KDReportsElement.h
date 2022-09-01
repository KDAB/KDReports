/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSELEMENT_H
#define KDREPORTSELEMENT_H

#include "KDReportsGlobal.h"
#include <memory>

QT_BEGIN_NAMESPACE
class QBrush;
QT_END_NAMESPACE

namespace KDReports {
class ReportBuilder;
class ElementPrivate;

/**
 * The KDReports::Element class is the base for all elements.
 *
 * An element defines what is to be added to the report. Calling
 * Report::addElement or Report::addInlineElement leads to new content
 * in the report. The report doesn't store the element, though.
 * For this reason, any changes to the element after insertion will have no
 * effect on the report. Set all settings in the element before calling addElement.
 */
class KDREPORTS_EXPORT Element
{
public:
    /**
     * Destroys this element.
     */
    virtual ~Element();

    /**
     * Sets the background color for this element.
     */
    void setBackground(const QBrush &brush);
    /**
     * Returns the background color for this element.
     */
    QBrush background() const;

    /**
     * @internal
     */
    virtual void build(ReportBuilder &) const = 0;

    /**
     * @internal
     */
    virtual Element *clone() const = 0;

protected:
    /**
     * Constructor - used by subclasses.
     */
    Element();
    /**
     * Copy constructor - used by subclasses.
     */
    Element(const Element &other);
    /**
     * Assignment operator - used by subclasses.
     */
    Element &operator=(const Element &other);

private:
    std::unique_ptr<ElementPrivate> d;
};

}

#endif /* KDREPORTSELEMENT_H */
