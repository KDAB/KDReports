/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

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

#include "KDReportsAutoTableElement.h"
#include "KDReportsGlobal.h"
#include "KDReportsReport.h"
#include "KDReportsTextDocumentData_p.h"
#include <QFont>
#include <QMap>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFormat>

QT_BEGIN_NAMESPACE
class QTextImageFormat;
class QTextTable;
QT_END_NAMESPACE

namespace KDReports {
enum
{
    HeaderColumnsProperty = QTextFormat::UserProperty + 248
};

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
    void layoutWithTextWidth(qreal w);

    // like QTextDocument::setPageSize but also takes care of objects with % sizes
    void setPageSize(QSizeF size);

#if 0
    // add user property to char format
    void setMarker( int pos, int propertyId, const QVariant& value );
#endif

    void regenerateAutoTables();
    void regenerateAutoTableForModel(QAbstractItemModel *model);

    void updateTextValue(const QString &id, const QString &newValue);

    void scaleFontsBy(qreal factor);

    QFont defaultFont() const;
    QTextDocument &contentDocument();
    TextDocumentData &contentDocumentData();

    QList<KDReports::AutoTableElement *> autoTableElements();

    QString asHtml() const;
    QString toStandaloneHtml();
    void preciseDump();

private:
    TextDocumentData m_contentDocument;
};

}

#endif /* KDREPORTSTEXTDOCUMENT_H */
