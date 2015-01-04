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

#include "KDReportsTextDocumentData_p.h"
#include <QUrl>
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsReportBuilder_p.h"
#include "KDReportsHLineTextObject_p.h"
#include "KDReportsChartTextObject_p.h"
#include <QTextTable>
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QTextDocument>

KDReports::TextDocumentData::TextDocumentData()
    : m_usesTabPositions(false)
{
    m_document = new QTextDocument;
    m_document->setUseDesignMetrics( true );

    HLineTextObject::registerHLineObjectHandler(m_document);
#ifdef HAVE_KDCHART
    ChartTextObject::registerChartTextObjectHandler(m_document);
#endif
}

KDReports::TextDocumentData::~TextDocumentData()
{
    delete m_document;
}

void KDReports::TextDocumentData::dumpTextValueCursors() const
{
    qDebug() << "Text value cursors:  (document size=" << m_document->characterCount() << ")";
    QMultiMap<QString, TextValueData>::const_iterator it = m_textValueCursors.begin();
    while ( it != m_textValueCursors.end() ) {
        const TextValueData& data = *it;
        if (data.cursor.isNull()) {
            qDebug() << it.key() << "unresolved cursor at pos" << data.initialPosition;
        } else {
            qDebug() << it.key() << "QTextCursor currently at pos" << data.cursor.position() << "length" << data.valueLength;
        }
        ++it;
    }
}

void KDReports::TextDocumentData::aboutToModifyContents(ModificationMode mode)
{
    resolveCursorPositions(mode);
}

void KDReports::TextDocumentData::resolveCursorPositions(ModificationMode mode)
{
    // We have to use QTextCursor in TextValueData so that it gets updated when
    // we modify the document later on, but we can't just store the QTextCursor
    // at insertion time; that cursor would be moved to the end of the document
    // while the insertion keeps happening...
    QMap<QString, TextValueData>::iterator it = m_textValueCursors.begin();
    for ( ; it != m_textValueCursors.end(); ++it ) {
        TextValueData& data = *it;
        if ( data.cursor.isNull()) {
            // When appending, leave cursors "at end of document" unresolved.
            // Otherwise they'll keep moving with insertions.
            if (mode == Append && data.initialPosition >= m_document->characterCount() - 1) {
                continue;
            }
            data.cursor = QTextCursor( m_document );
            data.cursor.setPosition( data.initialPosition );
            //qDebug() << "Cursor for" << it.key() << "resolved at position" << data.initialPosition;
        }
    }
    //dumpTextValueCursors();
}

void KDReports::TextDocumentData::setTextValueMarker( int pos, const QString& id, int valueLength, bool html )
{
    //qDebug() << "setTextValueMarker" << pos << id << valueLength << "in doc" << m_document;
    TextValueData val;
    val.valueLength = valueLength;
    val.elementType = html ? ElementTypeHtml : ElementTypeText;
    val.initialPosition = pos;
    m_textValueCursors.insert( id, val );
}

void KDReports::TextDocumentData::updateTextValue( const QString& id, const QString& newValue )
{
    aboutToModifyContents(Modify);

    //qDebug() << "updateTextValue: looking for id" << id << "in doc" << m_document;

    QMultiMap<QString, TextValueData>::iterator it = m_textValueCursors.find( id );
    while ( it != m_textValueCursors.end() && it.key() == id ) {
        TextValueData& data = *it;
        //qDebug() << "Found at position" << data.cursor.position() << "length" << data.valueLength << "replacing with new value" << newValue;

        QTextCursor c( data.cursor );
        const int oldPos = data.cursor.position();
        c.setPosition( oldPos + data.valueLength, QTextCursor::KeepAnchor );
        const bool html = data.elementType == ElementTypeHtml;
        if ( html )
            c.insertHtml( newValue );
        else
            c.insertText( newValue );
        // update data
        data.valueLength = c.position() - oldPos;
        data.cursor.setPosition( oldPos );
        //qDebug() << " stored new length" << data.valueLength;

        ++it;
    }

    //dumpTextValueCursors();
}

void KDReports::TextDocumentData::updatePercentSizes( const QSizeF& size )
{
    QTextCursor c( m_document );
    c.beginEditBlock();
    // TODO only if we inserted resizable images
    do {
        c.movePosition( QTextCursor::NextCharacter );
        QTextCharFormat format = c.charFormat();
        if ( format.hasProperty( ResizableImageProperty ) ) {
            Q_ASSERT( format.isImageFormat() );
            QTextImageFormat imageFormat = format.toImageFormat();
            updatePercentSize( imageFormat, size );
            //qDebug() << "updatePercentSizes: setting image to " << imageFormat.width() << "," << imageFormat.height();
            c.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
            c.setCharFormat( imageFormat );
            c.movePosition( QTextCursor::NextCharacter );
        }
    } while ( !c.atEnd() );

    if (m_usesTabPositions) {
        QTextFrameFormat rootFrameFormat = m_document->rootFrame()->frameFormat();
        const int rootFrameMargins = rootFrameFormat.leftMargin() + rootFrameFormat.rightMargin();
        QTextBlock block = m_document->firstBlock();
        do {
            QTextBlockFormat blockFormat = block.blockFormat();
            QList<QTextOption::Tab> tabs = blockFormat.tabPositions();
            //qDebug() << "Looking at block" << block.blockNumber() << "tabs:" << tabs.count();
            if (!tabs.isEmpty()) {
                for (int i = 0; i < tabs.count(); ++i) {
                    QTextOption::Tab& tab = tabs[i];
                    if ( tab.delimiter == QLatin1Char('P') /* means Page -- see rightAlignedTab*/) {
                        if ( tab.type == QTextOption::RightTab ) {
                            //qDebug() << "Adjusted RightTab from" << tab.position << "to" << size.width();
                            tab.position = size.width() - rootFrameMargins;
                        } else if ( tab.type == QTextOption::CenterTab ) {
                            tab.position = ( size.width() - rootFrameMargins ) / 2;
                        }
                    }
                }
                blockFormat.setTabPositions( tabs );
                //qDebug() << "Adjusted tabs:" << tabs;
                c.setPosition( block.position() );
                c.setBlockFormat( blockFormat );
            }
            block = block.next();
        } while ( block.isValid() );
    }
    c.endEditBlock();
}

void KDReports::TextDocumentData::layoutWithTextWidth( qreal w )
{
    if ( w != m_document->textWidth() ) {
        //qDebug() << "setTextWidth" << w;
        m_document->setTextWidth( w );
        updatePercentSizes( m_document->size() );
    }
}

void KDReports::TextDocumentData::setPageSize( const QSizeF& size )
{
    if ( size != m_document->pageSize() ) {
        //qDebug() << "setPageSize" << size;
        m_document->setPageSize( size );
        updatePercentSizes( size );
    }
}

void KDReports::TextDocumentData::updatePercentSize( QTextImageFormat& imageFormat, const QSizeF& size )
{
    // "W50" means W=50%.  "H60" means H=60%.
    QString prop = imageFormat.property( ResizableImageProperty ).toString();
    const qreal imageRatio = imageFormat.height() / imageFormat.width();
    const qreal pageWidth = size.width();
    const qreal pageHeight = size.height();
    const qreal pageRatio = pageWidth ? pageHeight / pageWidth : 0;
    if ( prop[0] == QLatin1Char( 'T' ) ) {
        //qDebug() << "updatePercentSize fitToPage" << imageRatio << pageRatio;
        if ( imageRatio < pageRatio ) {
            prop = QString::fromLatin1( "W100" );
        } else {
            prop = QString::fromLatin1( "H100" );
        }
    }
    const qreal percent = prop.mid( 1 ).toDouble();
    switch ( prop[0].toLatin1() ) {
    case 'W':
        {
        const qreal newWidth = pageWidth * percent / 100.0;
        imageFormat.setWidth( newWidth );
        imageFormat.setHeight( newWidth * imageRatio );
        // ### I needed to add this -2 here for 100%-width images to fit in
        if ( percent == 100.0 )
            imageFormat.setWidth( imageFormat.width() - 2 );
        }
        break;
    case 'H':
        imageFormat.setHeight( pageHeight * percent / 100.0 );
        // ### I needed to add -6 here for 100%-height images to fit in (with Qt-4.4)
        // and it became -9 with Qt-4.5, and even QtSw doesn't know why.
        // Task number 241890
        if ( percent == 100.0 )
            imageFormat.setHeight( imageFormat.height() - 10 );
        imageFormat.setWidth( imageRatio ? imageFormat.height() / imageRatio : 0 );
        //qDebug() << "updatePercentSize" << size << "->" << imageFormat.width() << "x" << imageFormat.height();
        break;
    default:
        qWarning( "Unhandled image format property type - internal error" );
    }
}


void KDReports::TextDocumentData::registerTable( QTextTable* table )
{
    m_tables.append( table );
}

void KDReports::TextDocumentData::scaleFontsBy( qreal factor )
{
    QTextCursor cursor( m_document );
    qreal currentPointSize = -1.0;
    QTextCursor lastCursor( m_document );
    Q_FOREVER {
        qreal cursorFontPointSize = cursor.charFormat().fontPointSize();
        //qDebug() << cursorFontPointSize << "last=" << currentPointSize << cursor.block().text() << "position=" << cursor.position();
        if ( cursorFontPointSize != currentPointSize ) {
            if ( currentPointSize != -1.0 ) {
                setFontSizeHelper( lastCursor, cursor.position() - 1, currentPointSize, factor );
                lastCursor.setPosition( cursor.position() - 1, QTextCursor::MoveAnchor );
            }
            currentPointSize = cursorFontPointSize;
        }
        if ( cursor.atEnd() )
            break;
        cursor.movePosition( QTextCursor::NextCharacter );
    }
    if ( currentPointSize != -1.0 ) {
        setFontSizeHelper( lastCursor, cursor.position(), currentPointSize, factor );
    }

    // Also adjust the padding in the cells so that it remains proportional,
    // and the column constraints.
    Q_FOREACH( QTextTable* table, m_tables ) {
        QTextTableFormat format = table->format();
        format.setCellPadding( format.cellPadding() * factor );

        QVector<QTextLength> constraints = format.columnWidthConstraints();
        for ( int i = 0; i < constraints.size(); ++i ) {
            if ( constraints[i].type() == QTextLength::FixedLength ) {
                constraints[i] = QTextLength( QTextLength::FixedLength, constraints[i].rawValue() * factor );
            }
        }
        format.setColumnWidthConstraints( constraints );

        table->setFormat( format );
    }
}

void KDReports::TextDocumentData::setFontSizeHelper( QTextCursor& lastCursor, int endPosition, qreal pointSize, qreal factor )
{
    if ( pointSize == 0 ) {
        pointSize = m_document->defaultFont().pointSize();
    }
    pointSize *= factor;
    QTextCharFormat newFormat;
    newFormat.setFontPointSize( pointSize );
    //qDebug() << "Applying" << pointSize << "from" << lastCursor.position() << "to" << endPosition;
    lastCursor.setPosition( endPosition, QTextCursor::KeepAnchor );
    lastCursor.mergeCharFormat( newFormat );
}

QString KDReports::TextDocumentData::asHtml() const
{
    QString htmlText = m_document->toHtml("utf-8");
    htmlText.remove( QLatin1String( "margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; " ) );
    htmlText.remove( QLatin1String( "-qt-block-indent:0; " ) );
    htmlText.remove( QLatin1String( "text-indent:0px;" ) );
    htmlText.remove( QLatin1String( "style=\"\"" ) );
    htmlText.remove( QLatin1String( "style=\" \"" ) );
    return htmlText;
}

void KDReports::TextDocumentData::registerAutoTable( QTextTable* table, const KDReports::AutoTableElement* element )
{
    registerTable( table );
    m_autoTables.insert( table, *element ); // make copy of the AutoTableElement
}

QList<KDReports::AutoTableElement *> KDReports::TextDocumentData::autoTableElements()
{
    QList<KDReports::AutoTableElement *> lst;
    for ( AutoTablesMaps::iterator it = m_autoTables.begin(); it != m_autoTables.end(); ++it )
        lst.append( & it.value() );
    return lst;
}

void KDReports::TextDocumentData::regenerateAutoTables()
{
    //qDebug() << "regenerateAutoTables" << m_autoTables.count();
    if ( m_autoTables.isEmpty() )
        return;
    aboutToModifyContents(Modify);
    QTextCursor( m_document ).beginEditBlock();
    //preciseDump();
    AutoTablesMaps autoTables = m_autoTables; // make copy since it will be modified below.
    m_autoTables.clear();
    AutoTablesMaps::const_iterator it = autoTables.constBegin();
    for ( ; it != autoTables.constEnd() ; ++it ) {
        QTextTable* table = it.key();
        const KDReports::AutoTableElement& tableElement = it.value();
        regenerateOneTable( tableElement, table );
    }
    //preciseDump();
    QTextCursor( m_document ).endEditBlock();
}

void KDReports::TextDocumentData::regenerateAutoTableForModel( QAbstractItemModel* model )
{
    aboutToModifyContents(Modify);
    QTextCursor( m_document ).beginEditBlock();
    AutoTablesMaps::iterator it = m_autoTables.begin();
    for ( ; it != m_autoTables.end() ; ++it ) {
        KDReports::AutoTableElement tableElement = it.value();
        if ( tableElement.tableModel() == model ) {
            QTextTable* table = it.key();
            m_autoTables.erase( it );
            regenerateOneTable( tableElement, table );
            break;
        }
    }
    QTextCursor( m_document ).endEditBlock();
}

void KDReports::TextDocumentData::regenerateOneTable( const KDReports::AutoTableElement& tableElement, QTextTable* table )
{
    QTextCursor cursor = table->firstCursorPosition();
    cursor.beginEditBlock();
    cursor.movePosition( QTextCursor::PreviousCharacter );
    QTextCursor lastCurs = table->lastCursorPosition();
    lastCurs.setPosition( lastCurs.position() + 1 );
    QTextBlockFormat blockFormat = lastCurs.blockFormat(); // preserve page breaks
    cursor.setPosition( table->lastCursorPosition().position()+1, QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
    cursor.setBlockFormat(QTextBlockFormat()); // see preciseDump during TextDocument unittest
    m_tables.removeAll( table );

    ReportBuilder builder( *this,
                           cursor, 0 /* hack - assumes Report is not needed */ );
    tableElement.build( builder ); // this calls registerTable again

    cursor.setBlockFormat( blockFormat );
    cursor.endEditBlock();
}

void KDReports::TextDocumentData::saveResourcesToFiles()
{
    Q_FOREACH( const QString& name, m_resourceNames ) {
        const QVariant v = m_document->resource( QTextDocument::ImageResource, QUrl( name ) );
        QPixmap pix = v.value<QPixmap>();
        if ( !pix.isNull() ) {
            pix.save( name );
        }
    }
}

void KDReports::TextDocumentData::addResourceName( const QString& resourceName )
{
    m_resourceNames.append( resourceName );
}

void KDReports::TextDocumentData::setUsesTabPositions(bool usesTabs)
{
    m_usesTabPositions = usesTabs;
}
