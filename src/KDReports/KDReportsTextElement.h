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

#ifndef KDREPORTSTEXTELEMENT_H
#define KDREPORTSTEXTELEMENT_H

#include "KDReportsElement.h"
#include <QtCore/QString>
#include <QTextCharFormat>

class QColor;
class QFont;

namespace KDReports {
class TextElementPrivate;

/**
 * The KDReports::TextElement class represents text in the report.
 * This is one way to insert rich text into the report (the other way is with HtmlElement).
 */
class KDREPORTS_EXPORT TextElement : public Element
{
public:
    /**
     * Creates a text element.
     */
    explicit TextElement( const QString& string = QString() );
    /**
     * Destroys this text element.
     */
    virtual ~TextElement();

    /**
     * Copies a text element.
     * The text and settings are copied over.
     */
    TextElement(const TextElement &other);
    /**
     * Copies the text and settings from another text element.
     */
    TextElement &operator=(const TextElement &other);

    /**
     * Sets the full text in this element.
     */
    void setText( const QString& text );

    /**
     * Adds more text to the text in this element.
     */
    TextElement& operator<<( const QString& );

    /// Set font attribute: family.
    void setFontFamily( const QString& family );
    /// Set font attribute: bold.
    void setBold( bool bold );
    /// Set font attribute: italic.
    void setItalic( bool italic );
    /// Set font attribute: underline.
    void setUnderline( bool underline );
    /// Set font attribute: strike out.
    void setStrikeOut( bool strikeout );
    /// Set font attribute: size in points. Can be integer or decimal.
    void setPointSize( qreal size );

    /**
     * Set multiple font attributes with a single call.
     *
     * Note that (since KDReports 1.6), when Qt is 5.3 or more, the font attributes that are not
     * explicitly specified in this font, will be resolved against the report's default font,
     * rather than being resolved against the application's font.
     */
    void setFont( const QFont& );

    /**
      * \return all font attribute
      * \since 1.4
      */
    QFont font() const;

    /**
     * Set the text color.
     * \since 1.1
     */
    void setTextColor( const QColor& color );

    /**
     * \return the text color.
     * \since 1.4
     */
    QColor textColor() const;

    /**
     * Set the ID associated with this text element.
     * \since 1.2
     */
    void setId( const QString& id );

    /**
     * \return the ID associated with this text element.
     * \since 1.2
     */
    QString id() const;

    /**
     * \return the text associated with this text element.
     * \since 1.4
     */
    QString text() const;

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

    friend class ReportBuilder;
    void setCharFormat( const QTextCharFormat& format );

    TextElementPrivate* const d;
};

}

#endif /* KDREPORTSTEXTELEMENT_H */
