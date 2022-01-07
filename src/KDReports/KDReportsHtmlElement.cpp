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

#include "KDReportsHtmlElement.h"
#include "KDReportsReportBuilder_p.h"
#include <QDebug>

class KDReports::HtmlElementPrivate
{
public:
    QString m_html;
    QString m_id;
};

KDReports::HtmlElement::HtmlElement(const QString &html)
    : Element()
    , d(new HtmlElementPrivate)
{
    d->m_html = html;
}

KDReports::HtmlElement::HtmlElement(const HtmlElement &other)
    : Element(other)
    , d(new HtmlElementPrivate(*other.d))
{
}

KDReports::HtmlElement &KDReports::HtmlElement::operator=(const HtmlElement &other)
{
    if (&other == this)
        return *this;
    Element::operator=(other);
    *d = *other.d;
    return *this;
}

KDReports::HtmlElement::~HtmlElement()
{
    delete d;
}

void KDReports::HtmlElement::build(ReportBuilder &builder) const
{
    QTextCursor &cursor = builder.cursor();
    const int charPosition = cursor.position();
    cursor.insertHtml(d->m_html);
    if (!d->m_id.isEmpty())
        builder.currentDocumentData().setTextValueMarker(charPosition, d->m_id, cursor.position(), false);
}

KDReports::HtmlElement &KDReports::HtmlElement::operator<<(const QString &str)
{
    d->m_html.append(str);
    return *this;
}

void KDReports::HtmlElement::setHtml(const QString &html)
{
    d->m_html = html;
}

KDReports::Element *KDReports::HtmlElement::clone() const
{
    return new HtmlElement(*this);
}

void KDReports::HtmlElement::setId(const QString &id)
{
    d->m_id = id;
}

QString KDReports::HtmlElement::id() const
{
    return d->m_id;
}

QString KDReports::HtmlElement::html() const
{
    return d->m_html;
}
