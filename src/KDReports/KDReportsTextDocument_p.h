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

#ifndef KDREPORTSTEXTDOCUMENT_H
#define KDREPORTSTEXTDOCUMENT_H

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

#include "KDReportsTextDocumentData_p.h"
#include <QTextCursor>
#include <QMap>
#include <QTextDocument>
#include <QTextFormat>
#include "KDReportsGlobal.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsReport.h"
#include <QFont>

class QTextImageFormat;
class QTextTable;

namespace KDReports
{
enum { HeaderColumnsProperty = QTextFormat::UserProperty + 248 };


/**
 * @internal  (exported for unit tests)
 * There is one instance of TextDocument in the report,
 * but also one in each header/footer.
 */
class KDREPORTS_EXPORT TextDocument
{
public:
    explicit TextDocument();
    ~TextDocument();

    // like QTextDocument::setTextWidth but also takes care of objects with % sizes
    void layoutWithTextWidth( qreal w );

    // like QTextDocument::setPageSize but also takes care of objects with % sizes
    void setPageSize( const QSizeF& size );

#if 0
    // add user property to char format
    void setMarker( int pos, int propertyId, const QVariant& value );
#endif

    void regenerateAutoTables();
    void regenerateAutoTableForModel( QAbstractItemModel* model );

    void updateTextValue( const QString& id, const QString& newValue );

    void scaleFontsBy( qreal factor );

    QFont defaultFont() const;
    QTextDocument& contentDocument();
    TextDocumentData& contentDocumentData();

    QList<KDReports::AutoTableElement *> autoTableElements();

    QString asHtml() const;
    void preciseDump();

private:
    TextDocumentData m_contentDocument;
};

}

#endif /* KDREPORTSTEXTDOCUMENT_H */

