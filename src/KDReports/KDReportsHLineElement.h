/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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
    void setColor(const QColor &color);

    /**
     * Returns the hline color
     */
    QColor color() const;

    /**
     * Sets the line thickness
     */
    void setThickness(qreal t);

    /**
     * Returns the hline thickness
     */
    qreal thickness() const;

    /**
     * Sets the margin (empty space) used above and below the line, in mm.
     */
    void setMargin(int m);

    /**
     * Returns the margin (empty space) used above and below the line, in mm.
     */
    int margin() const;

    void build(ReportBuilder &builder) const override;
    /**
     * @internal
     * @reimp
     */
    Element *clone() const override;

private:
    std::unique_ptr<HLineElementPrivate> d;
};

}

#endif /* KDREPORTSHLINELEMENT_H */
