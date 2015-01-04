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

#include "TestXmlElementHandlerV1.h"
#include <QDebug>
#include <QtXml/QDomElement>
#include "KDReportsReport.h"
#include "KDReportsTextElement.h"
#include "KDReportsHtmlElement.h"
#include "KDReportsImageElement.h"
#include "KDReportsChartElement.h"
#include "KDReportsHeader.h"
#include "KDReportsTableElement.h"
#include "KDReportsHLineElement.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsCell.h"

TestXmlElementHandlerV1::TestXmlElementHandlerV1( KDReports::Report& report )
    : mReport( report )
{
}

bool TestXmlElementHandlerV1::startReport( KDReports::Report& report, QDomElement& xmlElement )
{
    Q_UNUSED( report ); // unused in release mode
    Q_ASSERT( &report == &mReport );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "report" );
    cb << "startReport";
    return true;
}

bool TestXmlElementHandlerV1::startHeader( KDReports::Header& header, QDomElement& xmlElement )
{
    Q_UNUSED( header );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "header" );
    cb << "startHeader";
    return true;
}

void TestXmlElementHandlerV1::endHeader( KDReports::Header& header, const QDomElement& xmlElement )
{
    Q_UNUSED( header );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "header" );
    cb << "endHeader";
}

bool TestXmlElementHandlerV1::startFooter( KDReports::Footer& footer, QDomElement& xmlElement )
{
    Q_UNUSED( footer );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "footer" );
    cb << "startFooter";
    return true;
}

void TestXmlElementHandlerV1::endFooter( KDReports::Footer& footer, const QDomElement& xmlElement )
{
    Q_UNUSED( footer );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "footer" );
    cb << "endFooter";
}

bool TestXmlElementHandlerV1::textElement( KDReports::TextElement& textElement, QDomElement& xmlElement )
{
    Q_UNUSED( textElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "text" );
    cb << "textElement";
    if (textElement.id() == "testModification")
        textElement.setId("testModified");
    return true;
}

bool TestXmlElementHandlerV1::htmlElement( KDReports::HtmlElement& htmlElement, QDomElement& xmlElement )
{
    Q_UNUSED( htmlElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "html" );
    cb << "htmlElement";
    return true;
}

bool TestXmlElementHandlerV1::startTableElement( KDReports::TableElement& tableElement, QDomElement& xmlElement )
{
    Q_UNUSED( tableElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "table" );
    cb << "startTableElement";
    return true;
}

bool TestXmlElementHandlerV1::endTableElement( KDReports::TableElement& tableElement, QDomElement& xmlElement )
{
    Q_UNUSED( tableElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "table" );
    cb << "endTableElement";
    return true;
}

bool TestXmlElementHandlerV1::startCell( KDReports::Cell& cell, QDomElement& xmlElement )
{
    Q_UNUSED( cell );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "cell" );
    cb << "startCell";
    return true;
}

bool TestXmlElementHandlerV1::endCell( KDReports::Cell& cell, QDomElement& xmlElement )
{
    Q_UNUSED( cell );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "cell" );
    cb << "endCell";
    return true;
}

bool TestXmlElementHandlerV1::autoTableElement( KDReports::AutoTableElement& tableElement, QDomElement& xmlElement )
{
    Q_UNUSED( tableElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "table" );
    cb << "autoTableElement";
    return true;
}

bool TestXmlElementHandlerV1::chartElement( KDReports::ChartElement& chartElement, QDomElement& xmlElement )
{
    Q_UNUSED( chartElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "chart" );
    cb << "chartElement";
    return true;
}

bool TestXmlElementHandlerV1::imageElement( KDReports::ImageElement& imageElement, QDomElement& xmlElement )
{
    Q_UNUSED( imageElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "image" );

    if ( xmlElement.hasAttribute("broken") ) {
        KDReports::ErrorDetails details;
        details.setDriverMessage( QString("PC LOAD LETTER") );
        setErrorDetails( details );
        return false;
    }
    cb << "imageElement";
    return true;
}

bool TestXmlElementHandlerV1::pageBreak( QDomElement& xmlElement )
{
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "page-break" );
    cb << "pageBreak";
    return true;
}

void TestXmlElementHandlerV1::customElement( const QDomElement& xmlElement )
{
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "custom" );
    cb << "customElement";
    // Test inserting elements from the XML element handler
    if ( xmlElement.attribute( "id" ) == "lastParagraph" )
        mReport.addElement( KDReports::TextElement( "Custom element" ) );
}

#ifdef KDREPORTS_ALLOW_BINARY_INCOMPATIBILITY
bool TestXmlElementHandlerV1::hLineElement( KDReports::HLineElement& hLineElement, QDomElement& xmlElement )
{
    Q_UNUSED( hLineElement );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "hline" );
    cb << "hLineElement";
    return true;
}
#endif

void TestXmlElementHandlerV1::endReport( KDReports::Report& report, const QDomElement& xmlElement )
{
    Q_UNUSED( report ); // unused in release mode
    Q_ASSERT( &report == &mReport );
    Q_UNUSED( xmlElement ); // unused in release mode
    Q_ASSERT( xmlElement.tagName() == "report" );
    cb << "endReport";
}

