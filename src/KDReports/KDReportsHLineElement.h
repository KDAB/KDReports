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

#ifndef KDREPORTSHLINEELEMENT_H
#define KDREPORTSHLINEELEMENT_H

#include "KDReportsElement.h"
#include <QtCore/QString>

class QColor;
class QFont;
class QTextDocument;

namespace KDReports {
class HLineElementPrivate;

/**
 * The KDReports::HLineElement class represents an horizontal line in the report.
 * \since 1.3
 */
class KDREPORTS_EXPORT HLineElement : public Element
{
public:
    /**
     * Creates a hline element.
     */
    HLineElement();
    /**
     * Destroys this hline element.
     */
    virtual ~HLineElement();

    /**
     * Copies a hline element.
     * The settings are copied over.
     */
    HLineElement(const HLineElement &other);
    /**
     * Copies the hline and settings from another hline element.
     */
    HLineElement &operator=(const HLineElement &other);

    /**
     * Set the line color.
     */
    void setColor( const QColor& color );

    /**
     * Returns the hline color
     */
    QColor color() const;

    /**
     * Sets the line thickness
     */
    void setThickness(int w);

    /**
     * Returns the hline thickness
     */
    int thickness() const;

    /**
     * Sets the margin (empty space) used above and below the line, in mm.
     */
    void setMargin(int m);

    /**
     * Returns the margin (empty space) used above and below the line, in mm.
     */
    int margin() const;

    void build( ReportBuilder& builder ) const;
    /**
     * @internal
     * @reimp
     */
    Element* clone() const;

private:

    HLineElementPrivate* const d;
};

}

#endif /* KDREPORTSHLINELEMENT_H */

