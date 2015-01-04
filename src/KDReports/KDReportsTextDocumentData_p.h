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

#ifndef KDREPORTSTEXTDOCUMENTDATA_P_H
#define KDREPORTSTEXTDOCUMENTDATA_P_H
#include <QTextCursor>
#include "KDReportsReport.h"
#include "KDReportsAutoTableElement.h"
#include <QMultiMap>

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

namespace KDReports
{

/**
 * @internal
 * Contains a QTextDocument and its associated data.
 */
class KDREPORTS_EXPORT TextDocumentData
{
public:
    explicit TextDocumentData();
    ~TextDocumentData();

    QTextDocument& document() { return *m_document; }
    const QTextDocument& document() const { return *m_document; }

    void setUsesTabPositions(bool usesTabs);
    void saveResourcesToFiles();
    enum ModificationMode { Append, Modify };
    void aboutToModifyContents(ModificationMode mode);
    void updateTextValue( const QString& id, const QString& newValue );
    void layoutWithTextWidth( qreal w );
    void setPageSize( const QSizeF& size );
    void scaleFontsBy( qreal factor ); // TODO remove?
    void updatePercentSizes( const QSizeF& size );
    void setTextValueMarker( int pos, const QString& id, int valueLength, bool html );
    /// Break all tables in the document
    /// @returns the number of horizontal pages used
    //int breakTables( const QSizeF& textDocPageSize, int numHorizontalPages, KDReports::Report::TableBreakingPageOrder pageOrder );
    /// We need to know about all tables in order to implement table-breaking
    void registerTable( QTextTable* table );
    QString asHtml() const;
    /// For autotables, let's also remember the AutoTableElement, to be able
    /// to regenerate them (when modifying options in the table breaking dialog)
    void registerAutoTable( QTextTable* table, const KDReports::AutoTableElement* element );
    QList<KDReports::AutoTableElement *> autoTableElements();
    void regenerateAutoTables();
    void regenerateAutoTableForModel( QAbstractItemModel* model );
    void addResourceName( const QString& resourceName );

    static void updatePercentSize( QTextImageFormat& format, const QSizeF& size );

private:
    void resolveCursorPositions(ModificationMode mode);
    void setFontSizeHelper( QTextCursor& lastCursor, int endPosition, qreal pointSize, qreal factor );
    void regenerateOneTable( const KDReports::AutoTableElement& tableElement, QTextTable* table );
    void dumpTextValueCursors() const;

    QTextDocument* m_document; // a pointer because of the clone() API... TODO: cleanup
    enum ElementType { ElementTypeText, ElementTypeHtml };
    struct TextValueData
    {
        int valueLength;
        ElementType elementType;
        QTextCursor cursor;
        int initialPosition;
    };
    QMultiMap<QString, TextValueData> m_textValueCursors;

    QList<QTextTable*> m_tables;

    typedef QMap<QTextTable*, KDReports::AutoTableElement> AutoTablesMaps;
    AutoTablesMaps m_autoTables;
    QList<QString> m_resourceNames;
    bool m_usesTabPositions;
};

}

#endif /* KDREPORTSTEXTDOCUMENTDATA_P_H */
