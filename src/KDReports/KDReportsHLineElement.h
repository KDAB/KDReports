/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORTSHLINEELEMENT_H
#define KDREPORTSHLINEELEMENT_H

#include "KDReportsElement.h"
#include <QtCore/QString>

QT_BEGIN_NAMESPACE
class QColor;
class QFont;
class QTextDocument;
QT_END_NAMESPACE

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
    ~HLineElement() override;

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

    void build( ReportBuilder& builder ) const override;
    /**
     * @internal
     * @reimp
     */
    Element* clone() const override;

private:

    HLineElementPrivate* const d;
};

}

#endif /* KDREPORTSHLINELEMENT_H */

