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

#ifndef KDREPORTSXMLPARSER_H
#define KDREPORTSXMLPARSER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Reports API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#include "KDReportsGlobal.h"
#include "KDReportsReport.h"
#include "KDReportsErrorDetails.h"
#include <QtCore/QHash>
#include <QPixmap>

class QDomDocument;
class QDomNode;

namespace KDReports {
class TableElement;
class XmlElementHandler;
class AbstractTableElement;

/**
 * @internal
 * XML parser for Report
 */
class XmlParser
{
public:
    XmlParser( const QHash<QString, QString>& textValues,
               const QHash<QString, QImage>& imageValues,
               XmlElementHandler* xmlHandler,
               KDReports::Report* report,
               ErrorDetails* details )
        : m_textValues( textValues ),
          m_imageValues( imageValues ),
          m_xmlElementHandler( xmlHandler ),
          m_report( report ),
          m_errorDetails( details )
    {}

    bool processDocument( const QDomDocument& document, KDReports::ReportBuilder* builder );
private:
    bool processNode( const QDomNode& node, KDReports::ReportBuilder* builder, bool inHeader, bool inFooter );
    void addElement( KDReports::Element& reportElement, KDReports::ReportBuilder* builder, const QDomElement& element );
    bool parseTableContents( KDReports::TableElement& table, const QDomNode& tableNode, KDReports::ReportBuilder& builder,
                             bool inHeader, bool inFooter );
    void parseCommonTableAttributes( KDReports::AbstractTableElement& tableElement, QDomElement& element );
    void parseTabs( KDReports::ReportBuilder* builder, const QDomElement& element );
    void parseParagraphMargins( KDReports::ReportBuilder* builder, const QDomElement& element );
    QString extractText( const QDomElement& element, QString* id, const QAbstractItemModel *currentModel = 0, int currentRow = -1 ) const;
    QImage extractImage( const QDomElement& element, QString* pId ) const;
    bool testForErrorAndFillErrorDetails();
    void error( const QString& errorString );

    QHash<QString, QString> m_textValues;
    QHash<QString, QImage> m_imageValues;
    XmlElementHandler* m_xmlElementHandler;
    KDReports::Report* m_report;
    ErrorDetails* m_errorDetails;
};

}

#endif
