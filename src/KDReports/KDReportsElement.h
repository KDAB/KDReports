/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#ifndef KDREPORTSELEMENT_H
#define KDREPORTSELEMENT_H

#include "KDReportsGlobal.h"

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
    ElementPrivate *const d;
};

}

#endif /* KDREPORTSELEMENT_H */
