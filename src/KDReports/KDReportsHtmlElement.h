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
    explicit HtmlElement( const QString& html = QString() );
    /**
     * Destroys this element.
     */
    virtual ~HtmlElement();

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
    void setHtml( const QString& html );

    /**
     * \return the full HTML text in this element.
     * \since 1.4
     */
    QString html() const;

    /**
     * Adds more text to the HTML in this element.
     */
    HtmlElement& operator<<( const QString& );

    /**
     * Set the ID associated with this text element.
     * \since 1.2
     */
    void setId( const QString& id );

    /**
     * @return the ID associated with this text element.
     * \since 1.2
     */
    QString id() const;

    /**
     * @internal
     * @reimp
     */
    void build( ReportBuilder& builder ) const;
    /**
     * @internal
     * @reimp
     */
    Element* clone() const;

private:
    HtmlElementPrivate* const d;
};

}

#endif /* KDREPORTSHTMLELEMENT_H */

