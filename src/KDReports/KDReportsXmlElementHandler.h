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

#ifndef KDREPORTSXMLELEMENTHANDLER_H
#define KDREPORTSXMLELEMENTHANDLER_H

#include "KDReportsGlobal.h"
#include "KDReportsErrorDetails.h"
class QDomElement;

namespace KDReports {

class Report;
class TextElement;
class HtmlElement;
class TableElement;
class AutoTableElement;
class ChartElement;
class ImageElement;
class HLineElement;
class Header;
typedef Header Footer;
class Cell;

/**
 * This handler is called for each XML element being parsed
 * from an XML file.
 * It allows you to customize the XML dynamically at runtime,
 * by deriving from the XmlElementHandler interface and reimplementing
 * some or all of the hooks. Each hook (virtual method) is called after the
 * XML element has been parsed into a KDReports element, and before the
 * KDReports element is added to the report. After that, both the XML element
 * and the KDReports element are discarded, which is why this customization
 * has to happen during loading.
 *
 * Example:
 * <code>
 * bool CustomElementHandler::textElement( KDReports::TextElement& textElement, const QDomElement& xmlElement )
 * {
 *     const QString id = xmlElement.attribute( "id" );
 *     if ( id == "customerName" )
 *         textElement.setTextColor( Qt::blue );
 *     return true;
 * }
 * </code>
 *
 * All virtual methods return a boolean which can be used to skip an element:
 * if the method returns false, the KDReports element will not be added to the report.
 *
 * Note that you can even insert new elements into the report from any of the
 * handler methods. Those additional elements will be inserted before the currently
 * parsed element.
 *
 * It is possible to signal an error, during parsing, by calling
 * setErrorDetails. This will result in parsing being terminated, and the
 * error being reported to the user.
 *
 * To set up the handler so that it is used during parsing, use
 * <code>
 *   handler = new CustomElementHandler;
 *   report.setXmlElementHandler( handler );
 * </code>
 */
class KDREPORTS_EXPORT XmlElementHandler //krazy:exclude=dpointer TODO fix this
{
public:
    XmlElementHandler();
    virtual ~XmlElementHandler();

    /**
     * Called after parsing "report" and its attributes.
     * Returning false aborts the loading of the report.
     */
    virtual bool startReport( KDReports::Report& report, QDomElement& xmlElement );

    /**
     * Called after parsing "header" and its attributes, but before
     * parsing its contents.
     * Returning false skips this header.
     */
    virtual bool startHeader( KDReports::Header& header, QDomElement& xmlElement );

    /**
     * Called after parsing "header" and its contents.
     */
    virtual void endHeader( KDReports::Header& header, const QDomElement& xmlElement );

    /**
     * Called after parsing "footer" and its attributes, but before
     * parsing its contents.
     * Returning false skips this footer.
     */
    virtual bool startFooter( KDReports::Footer& footer, QDomElement& xmlElement );

    /**
     * Called after parsing "footer" and its contents.
     */
    virtual void endFooter( KDReports::Footer& footer, const QDomElement& xmlElement );

    /**
     * Called after parsing "text", its attributes and the text content.
     * Changes to those attributes must be done on @p textElement.
     *
     * As a special case, changes to the alignment and paragraph-background for
     * block elements must done using <code>xmlElement.setAttribute()</code>,
     * since they are not properties of KDReports::TextElement.
     */
    virtual bool textElement( KDReports::TextElement& textElement, QDomElement& xmlElement );

    /**
     * Called after parsing "html", its attributes and the html content.
     * Changes to those attributes must be done on @p htmlElement.
     *
     * As a special case, changes to the alignment and paragraph-background for
     * block elements must done using <code>xmlElement.setAttribute()</code>,
     * since they are not properties of KDReports::HtmlElement.
     */
    virtual bool htmlElement( KDReports::HtmlElement& htmlElement, QDomElement& xmlElement );

    /**
     * Called after parsing "table" (without a model) and its attributes,
     * but before parsing the cells in the table.
     * Changes to those attributes must be done on @p tableElement.
     *
     * As a special case, changes to the alignment for block elements must
     * be done using <code>xmlElement.setAttribute()</code>.
     * Returning false skips the whole table.
     */
    virtual bool startTableElement( KDReports::TableElement& tableElement, QDomElement& xmlElement );

    /**
     * Called after parsing a "table" (without a model) and its cells.
     * Returning false skips the whole table.
     */
    virtual bool endTableElement( KDReports::TableElement& tableElement, QDomElement& xmlElement );

    /**
     * Called after parsing a "cell" element inside a table, but before parsing the
     * cell contents.
     * Returning false makes the cell empty.
     */
    virtual bool startCell( KDReports::Cell& cell, QDomElement& xmlElement );

    /**
     * Called after parsing each "cell" element inside a table and its contents.
     * Returning false makes the cell empty.
     */
    virtual bool endCell( KDReports::Cell& cell, QDomElement& xmlElement );

    /**
     * Called after parsing a "table" element (when a model is set) and its attributes,
     * but before parsing the cells in the table.
     * Changes to those attributes must be done on @p tableElement.
     *
     * As a special case, changes to the alignment for block elements must
     * be done using <code>xmlElement.setAttribute()</code>.
     */
    virtual bool autoTableElement( KDReports::AutoTableElement& tableElement, QDomElement& xmlElement );

    /**
     * Called after parsing "chart" and its attributes.
     * Changes to those attributes must be done on @p chartElement.
     *
     * As a special case, changes to the alignment for block elements must
     * be done using <code>xmlElement.setAttribute()</code>.
     */
    virtual bool chartElement( KDReports::ChartElement& chartElement, QDomElement& xmlElement );

    /**
     * Called after parsing "image" and its attributes.
     * Changes to those attributes must be done on @p imageElement.
     *
     * As a special case, changes to the alignment for block elements must
     * be done using <code>xmlElement.setAttribute()</code>.
     */
    virtual bool imageElement( KDReports::ImageElement& imageElement, QDomElement& xmlElement );

    /**
     * Called after parsing "page-break".
     * Can be used to skip the page break depending on some condition.
     */
    virtual bool pageBreak( QDomElement& xmlElement );

#ifdef KDREPORTS_ALLOW_BINARY_INCOMPATIBILITY
    /**
     * Called after parsing a "hline".
     * Returning false skips the whole line. Can be used to change
     * hline attributes.
     *
     * This method is only defined and called if -DKDREPORTS_ALLOW_BINARY_INCOMPATIBILITY
     * is set while compiling both kdreports and your application.
     */
    virtual bool hLineElement( KDReports::HLineElement& hLineElement, QDomElement& xmlElement );
#endif

    /**
     * Called after parsing "custom", whose only purpose is to call this method.
     * This allows you to define special places in the XML file where this method
     * should be called, for instance to insert dynamic content into the report.
     */
    virtual void customElement( const QDomElement& xmlElement );

    /**
     * Called at the end of the parsing of the whole report.
     * The "report" element is passed again here.
     */
    virtual void endReport( KDReports::Report& report, const QDomElement& xmlElement );

    /**
     * @return an ErrorDetails instance.
     */
    KDReports::ErrorDetails errorDetails();

    /**
     * When parsing the XML code, errors may be reported by setting an ErrorDetail using this method.
     * Parsing will be stopped, and the error reported back to the user.
     */
    void setErrorDetails( const KDReports::ErrorDetails& details );

private:
    KDReports::ErrorDetails m_details;

    // BIC TODO: add d pointer
};

}

#endif /* KDREPORTSXMLELEMENTHANDLER_H */
