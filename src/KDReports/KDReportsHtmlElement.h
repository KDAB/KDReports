/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSHTMLELEMENT_H
#define KDREPORTSHTMLELEMENT_H

#include "KDReportsElement.h"
#include <QtCore/QString>

namespace KDReports {
class HtmlElementPrivate;

/**
 * The KDReports::HtmlElement class represents any part of the report that can be expressed in HTML.
 * This is one way to insert rich text into the report (the other way is with TextElement).
 * This can be used to create bullet-point lists and numbered lists.
 */
class KDREPORTS_EXPORT HtmlElement : public Element
{
public:
    /**
     * Creates an HTML report element.
     */
    explicit HtmlElement(const QString &html = QString());
    /**
     * Destroys this element.
     */
    ~HtmlElement() override;

    /**
     * Copies an HTML element.
     * The HTML and settings are copied over.
     */
    HtmlElement(const HtmlElement &other);
    /**
     * Copies the HTML and settings from another HTML element.
     */
    HtmlElement &operator=(const HtmlElement &other);

    /**
     * Sets the full HTML text in this element.
     */
    void setHtml(const QString &html);

    /**
     * \return the full HTML text in this element.
     * \since 1.4
     */
    QString html() const;

    /**
     * Adds more text to the HTML in this element.
     */
    HtmlElement &operator<<(const QString &);

    /**
     * Set the ID associated with this text element.
     * \since 1.2
     */
    void setId(const QString &id);

    /**
     * @return the ID associated with this text element.
     * \since 1.2
     */
    QString id() const;

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &builder) const override;
    /**
     * @internal
     * @reimp
     */
    Element *clone() const override;

private:
    std::unique_ptr<HtmlElementPrivate> d;
};

}

#endif /* KDREPORTSHTMLELEMENT_H */
