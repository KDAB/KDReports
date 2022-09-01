/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSHEADER_H
#define KDREPORTSHEADER_H

#include "KDReportsGlobal.h"
#include "KDReportsVariableType.h"
#include <QTextOption>
#include <QtCore/Qt>
#include <memory>

QT_BEGIN_NAMESPACE
template<class T>
class QList;
class QFont;
class QTextDocument;
class HeaderData;
QT_END_NAMESPACE

namespace KDReports {

class HeaderPrivate;
class XmlParser;
class Element;
class HeaderMap;
class Report;
class ReportPrivate;
class TextDocument;

/**
 * This class presents a header or footer in a report.
 * To add an element to a header or footer, first get hold of the
 * right header/footer using Report::header() or Report::footer(),
 * then add elements to it.
 */
class KDREPORTS_EXPORT Header
{
public:
    /**
     * Sets the default font used for text in this header
     * \since 1.1
     */
    void setDefaultFont(const QFont &font);

    /**
     * \return the default font used for text in this header
     * \since 1.4
     */
    QFont defaultFont() const;

    /**
     * Adds an element to the report, next to the previous element, in the same paragraph.
     */
    void addInlineElement(const Element &element);

    /**
     * Adds an element to the report, creating a new paragraph for it.
     * You can specify the alignment of that paragraph.
     */
    void addElement(const Element &element, Qt::AlignmentFlag horizontalAlignment = Qt::AlignLeft);

    /**
     * Adds a variable in the text of the current paragraph.
     * Use addInlineElement before and/or after to complete the text of the paragraph.
     * Example: "Page 1 / 3":
     * @code
        header.addInlineElement( KDReports::TextElement( "Page " ) );
        header.addVariable( KDReports::PageNumber );
        header.addInlineElement( KDReports::TextElement( " / " ) );
        header.addVariable( KDReports::PageCount );
     * @endcode
     */
    void addVariable(VariableType variable);

    /**
     * Adds vertical spacing between paragraphs.
     * Make sure to call addElement after that, not addInlineElement.
     * @param space the space in millimeters
     */
    void addVerticalSpacing(qreal space);

    /**
     * Set the list of tabs (tabulations) to use when adding block elements.
     * Those tabs will be set for any block element that is added from now on,
     * in this header.
     * For more details see Report::setTabPositions.
     *
     * \since 1.2
     */
    void setTabPositions(const QList<QTextOption::Tab> &tabs);

    /**
     * \internal Returns the current builder cursor position.
     */
    int currentPosition() const;

private:
    friend class XmlParser;
    friend class Report;
    friend class ReportPrivate;
    explicit Header(Report *report);
    ~Header();
    friend class Test;
    friend class HeaderMap;
    friend class ::HeaderData;
    TextDocument &doc();
    void preparePaintingPage(int pageNumber);

    Q_DISABLE_COPY(Header)

    std::unique_ptr<HeaderPrivate> d;
};

typedef Header Footer;

}

#endif /* KDREPORTSHEADER_H */
