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

#ifndef KDREPORTSTEXTDOCUMENTDATA_P_H
#define KDREPORTSTEXTDOCUMENTDATA_P_H
#include "KDReportsAutoTableElement.h"
#include "KDReportsReport.h"
#include <QMultiMap>
#include <QTextCursor>
#include <QTextDocument>

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

namespace KDReports {

/**
 * @internal
 * Contains a QTextDocument and its associated data.
 */
class KDREPORTS_EXPORT TextDocumentData
{
public:
    explicit TextDocumentData();
    ~TextDocumentData();

    TextDocumentData(const TextDocumentData &) = delete;
    TextDocumentData &operator=(const TextDocumentData &) = delete;

    QTextDocument &document()
    {
        return m_document;
    }
    const QTextDocument &document() const
    {
        return m_document;
    }

    void setUsesTabPositions(bool usesTabs);
    void saveResourcesToFiles();
    enum ModificationMode
    {
        Append,
        Modify
    };
    void aboutToModifyContents(ModificationMode mode);
    void updateTextValue(const QString &id, const QString &newValue);
    void layoutWithTextWidth(qreal w);
    void setPageSize(QSizeF size);
    void scaleFontsBy(qreal factor); // TODO remove?
    void updatePercentSizes(QSizeF size);
    void setTextValueMarker(int pos, const QString &id, int valueLength, bool html);
    /// Break all tables in the document
    /// @returns the number of horizontal pages used
    // int breakTables( const QSizeF& textDocPageSize, int numHorizontalPages, KDReports::Report::TableBreakingPageOrder pageOrder );
    // We need to know about all tables in order to implement table-breaking
    void registerTable(QTextTable *table);
    QString asHtml() const;
    /// For autotables, let's also remember the AutoTableElement, to be able
    /// to regenerate them (when modifying options in the table breaking dialog)
    void registerAutoTable(QTextTable *table, const KDReports::AutoTableElement *element);
    QList<KDReports::AutoTableElement *> autoTableElements();
    void regenerateAutoTables();
    void regenerateAutoTableForModel(QAbstractItemModel *model);
    void addResourceName(const QString &resourceName);
    void setHasResizableImages();

    static void updatePercentSize(QTextImageFormat &format, QSizeF size);

private:
    void resolveCursorPositions(ModificationMode mode);
    void setFontSizeHelper(QTextCursor &lastCursor, int endPosition, qreal pointSize, qreal factor);
    void regenerateOneTable(const KDReports::AutoTableElement &tableElement, QTextTable *table);
    void dumpTextValueCursors() const;

    QTextDocument m_document;
    enum ElementType
    {
        ElementTypeText,
        ElementTypeHtml
    };
    struct TextValueData
    {
        int valueLength;
        ElementType elementType;
        QTextCursor cursor;
        int initialPosition;
    };
    QMultiMap<QString, TextValueData> m_textValueCursors;

    QList<QTextTable *> m_tables;

    typedef QHash<QTextTable *, KDReports::AutoTableElement> AutoTablesMaps;
    AutoTablesMaps m_autoTables;
    QList<QString> m_resourceNames;
    bool m_usesTabPositions;
    bool m_hasResizableImages = false;
};

}

#endif /* KDREPORTSTEXTDOCUMENTDATA_P_H */
