/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsTextElement.h"
#include "KDReportsHeader_p.h" // for cleanupVariableProperties
#include "KDReportsReportBuilder_p.h"
#include <QDebug>

class KDReports::TextElementPrivate
{
public:
    QString m_string;
    QString m_id;

    QString m_fontFamily;
    bool m_bold = false;
    bool m_boldIsSet = false;
    bool m_italic = false;
    bool m_italicIsSet = false;
    bool m_underline = false;
    bool m_underlineIsSet = false;
    bool m_strikeout = false;
    bool m_strikeoutIsSet = false;
    bool m_fontSet = false;
    qreal m_pointSize = 0;
    QColor m_foreground;
    QFont m_font;
};

KDReports::TextElement::TextElement(const QString &string)
    : Element()
    , d(new TextElementPrivate)
{
    d->m_string = string;
}

KDReports::TextElement::TextElement(const TextElement &other)
    : Element(other)
    , d(new TextElementPrivate(*other.d))
{
}

KDReports::TextElement &KDReports::TextElement::operator=(const TextElement &other)
{
    if (&other == this)
        return *this;
    Element::operator=(other);
    *d = *other.d;
    return *this;
}

KDReports::TextElement::~TextElement()
{
}

void KDReports::TextElement::build(ReportBuilder &builder) const
{
    QTextCursor &cursor = builder.cursor();
    const int charPosition = cursor.position();
    QTextCharFormat charFormat = cursor.charFormat();
    KDReports::cleanupVariableProperties(charFormat);
    if (d->m_fontSet) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        charFormat.setFont(d->m_font, QTextCharFormat::FontPropertiesSpecifiedOnly);
#else
        charFormat.setFont(d->m_font);
#endif
    }
    if (d->m_boldIsSet)
        charFormat.setFontWeight(d->m_bold ? QFont::Bold : QFont::Normal);
    if (d->m_italicIsSet)
        charFormat.setFontItalic(d->m_italic);
    if (d->m_underlineIsSet)
        charFormat.setFontUnderline(d->m_underline);
    if (d->m_strikeoutIsSet)
        charFormat.setFontStrikeOut(d->m_strikeout);
    if (d->m_pointSize > 0)
        charFormat.setFontPointSize(d->m_pointSize);
    if (!d->m_fontFamily.isEmpty())
#if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
        charFormat.setFontFamilies({d->m_fontFamily});
#else
        charFormat.setFontFamily(d->m_fontFamily);
#endif
    if (d->m_foreground.isValid())
        charFormat.setForeground(d->m_foreground);
    else
        charFormat.clearForeground();
    if (background().style() != Qt::NoBrush)
        charFormat.setBackground(background());
    else
        charFormat.clearBackground();
    cursor.setCharFormat(charFormat);
    cursor.insertText(d->m_string);
    if (!d->m_id.isEmpty())
        builder.currentDocumentData().setTextValueMarker(charPosition, d->m_id, d->m_string.length(), false);
}

KDReports::TextElement &KDReports::TextElement::operator<<(const QString &str)
{
    d->m_string.append(str);
    return *this;
}

void KDReports::TextElement::setText(const QString &text)
{
    d->m_string = text;
}

void KDReports::TextElement::setFontFamily(const QString &family)
{
    d->m_fontFamily = family;
}

void KDReports::TextElement::setBold(bool bold)
{
    d->m_bold = bold;
    d->m_boldIsSet = true;
}

void KDReports::TextElement::setItalic(bool italic)
{
    d->m_italic = italic;
    d->m_italicIsSet = true;
}

void KDReports::TextElement::setUnderline(bool underline)
{
    d->m_underline = underline;
    d->m_underlineIsSet = true;
}

void KDReports::TextElement::setStrikeOut(bool strikeout)
{
    d->m_strikeout = strikeout;
    d->m_strikeoutIsSet = true;
}

void KDReports::TextElement::setPointSize(qreal size)
{
    d->m_pointSize = size;
}

void KDReports::TextElement::setFont(const QFont &font)
{
    d->m_font = font;
    d->m_fontSet = true;
}

void KDReports::TextElement::setTextColor(const QColor &color)
{
    d->m_foreground = color;
}

QColor KDReports::TextElement::textColor() const
{
    return d->m_foreground;
}

KDReports::Element *KDReports::TextElement::clone() const
{
    return new TextElement(*this);
}

void KDReports::TextElement::setId(const QString &id)
{
    d->m_id = id;
}

QString KDReports::TextElement::id() const
{
    return d->m_id;
}

QString KDReports::TextElement::text() const
{
    return d->m_string;
}

QFont KDReports::TextElement::font() const
{
    return d->m_font;
}
