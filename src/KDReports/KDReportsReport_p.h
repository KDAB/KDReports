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

#ifndef KDREPORTSREPORT_P_H
#define KDREPORTSREPORT_P_H

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

#include "KDReportsTextDocument_p.h"
#include "KDReportsReport.h"
#include "KDReportsHeader.h"
#include "KDReportsReportBuilder_p.h"
#include <QMap>
#include <QHash>

namespace KDReports
{
class XmlElementHandler;
class MainTable;
class AbstractReportLayout;

/**
 * @internal returns the model that is associated with a key.
 */
QAbstractItemModel* modelForKey( const QString& key );

class Header;
/**
 * @internal. Exported for unit tests only.
 */
class KDREPORTS_EXPORT HeaderMap : public QMap<HeaderLocations, Header*>
{
public:
    HeaderMap() {}
    ~HeaderMap() {
        for ( iterator it = begin(); it != end(); ++it ) {
            delete *it;
        }
    }

    void layoutWithTextWidth( qreal w ) {
        for ( const_iterator it = constBegin(); it != constEnd(); ++it ) {
            it.value()->doc().layoutWithTextWidth( w );
        }
    }

    void updateTextValue( const QString &id, const QString &newValue ) {
        for ( const_iterator it = constBegin(); it != constEnd(); ++it ) {
            it.value()->doc().updateTextValue( id, newValue );
        }
    }

    qreal height() const {
        qreal maxHeight = 0;
        for ( const_iterator it = begin(); it != end(); ++it ) {
            Header* header = it.value();
            maxHeight = qMax( maxHeight, header->doc().contentDocument().size().height() );
        }
        return maxHeight;
    }

    Header* headerForPage( int pageNumber, int pageCount ) const;
    KDReports::HeaderLocations headerLocation(Header *header) const;
};

class ReportPrivate
{
public:
    explicit ReportPrivate( Report* report );
    ~ReportPrivate();

    void setPaperSizeFromPrinter( const QSizeF& paperSize );
    void ensureLayouted();
    QSizeF paperSize() const;
    void paintPage( int pageNumber, QPainter& painter );
    bool doPrint( QPrinter* printer, QWidget* parent );
    QSizeF layoutAsOnePage( qreal docWidth );
    bool wantEndlessPrinting() const;
    bool hasNonLayoutedTextDocument() const;
    ReportBuilder* builder();

    void headerChanged(); // called by Header
    qreal textDocumentWidth() const; // called by ImageElement, ChartElement
    bool skipHeadersFooters() const;
    qreal rawMainTextDocHeight() const;
    qreal mainTextDocHeight() const;
#ifndef NDEBUG
    // for calling from gdb
    void debugLayoutToPdf( const char* fileName );
#endif

    qreal m_layoutWidth; // in pixels; used for layoutAsOnePage only
    qreal m_endlessPrinterWidth; // in mm
    mutable QSizeF m_paperSize; // in pixels
    QPrinter::Orientation m_orientation;
    QPrinter::PageSize m_pageSize;
    QString m_documentName;
    qreal m_marginTop;
    qreal m_marginLeft;
    qreal m_marginBottom;
    qreal m_marginRight;
    qreal m_headerBodySpacing;
    qreal m_footerBodySpacing;
    HeaderMap m_headers;
    HeaderMap m_footers;
    QString m_watermarkText;
    int m_watermarkRotation;
    QColor m_watermarkColor;
    QFont m_watermarkFont;
    QImage m_watermarkImage;
    int m_firstPageNumber;
    bool m_pageContentSizeDirty;

    //int m_numHorizontalPages; // for scaleTo() and setTableBreakingEnabled(). 1 if not set.
    //int m_numVerticalPages;   // for scaleTo(). 0 if not set.
    //qreal m_scaleFontsBy;     // for scaleFontsBy(), 1.0 otherwise.
    //bool m_autoScale;         // if scaleTo() was called.
    //KDReports::Report::TableBreakingPageOrder m_tableBreakingPageOrder;

    QHash<QString, QString> m_textValues;
    QHash<QString, QImage> m_imageValues;
    XmlElementHandler* m_xmlElementHandler;
    int m_currentRow;
    const QAbstractItemModel *m_currentModel;
    KDReports::Report::ReportMode m_reportMode;
    KDReports::AbstractReportLayout* m_layout;
    MainTable* m_mainTable;
    KDReports::Report *q;
};

}

#endif /* KDREPORTSREPORT_P_H */
