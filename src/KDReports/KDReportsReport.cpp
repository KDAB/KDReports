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

#include "KDReportsReport.h"
#include "KDReportsReport_p.h"
#include "KDReportsElement.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsHeader.h"
#include "KDReportsXmlParser_p.h"
#include "KDReportsMainTable.h"
#include "KDReportsTextDocReportLayout_p.h"
#include "KDReportsSpreadsheetReportLayout_p.h"
#include <QXmlInputSource>
#include <QThread>
#include <QProgressDialog>
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QPrintDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>
#include <QApplication>
#include <QPointer>
#include <QStyle>
#include <QStyleOption>

QT_BEGIN_NAMESPACE
Q_GUI_EXPORT extern int qt_defaultDpi(); // This is what QTextDocument uses...
QT_END_NAMESPACE

KDReports::ReportPrivate::ReportPrivate( Report* report )
    : m_layoutWidth( 0 ),
      m_endlessPrinterWidth( 0 ),
      m_orientation( QPrinter::Portrait ),
      m_pageSize( QPrinter::A4 ),
      m_marginTop( 20.0 ), // warning, defaults are duplicated in KDReportsXmlParser.cpp
      m_marginLeft( 20.0 ),
      m_marginBottom( 20.0 ),
      m_marginRight( 20.0 ),
      m_headerBodySpacing( 0 ),
      m_footerBodySpacing( 0 ),
      m_headers(),
      m_footers(),
      m_watermarkRotation( 0 ),
      m_watermarkColor( QColor( 204, 204, 204 ) ),
      m_watermarkFont( QFont( QLatin1String( "Helvetica" ), 48 ) ),
      m_watermarkImage(),
#if 0
      m_numHorizontalPages( 1 ),
      m_numVerticalPages( 0 ),
      m_scaleFontsBy( 1.0 ),
      m_autoScale( false ),
      m_tableBreakingPageOrder( KDReports::Report::DownThenRight ),
#endif
      m_firstPageNumber( 1 ),
      m_pageContentSizeDirty( true ),
      m_xmlElementHandler( 0 ),
      m_currentRow( -1 ),
      m_currentModel( 0 ),
      m_reportMode( KDReports::Report::WordProcessing ),
      m_layout( new TextDocReportLayout(report) ),
      m_mainTable( new MainTable ),
      q( report )
{
}

KDReports::ReportPrivate::~ReportPrivate()
{
    delete m_layout;
    delete m_mainTable;
}

bool KDReports::ReportPrivate::wantEndlessPrinting() const
{
    return m_layoutWidth > 0;
}

// Copy of Qt's qt_paperSizes
static const float kdr_paperSizes[][2] = {
    {210, 297}, // A4
    {176, 250}, // B5
    {215.9f, 279.4f}, // Letter
    {215.9f, 355.6f}, // Legal
    {190.5f, 254}, // Executive
    {841, 1189}, // A0
    {594, 841}, // A1
    {420, 594}, // A2
    {297, 420}, // A3
    {148, 210}, // A5
    {105, 148}, // A6
    {74, 105}, // A7
    {52, 74}, // A8
    {37, 52}, // A8
    {1000, 1414}, // B0
    {707, 1000}, // B1
    {31, 44}, // B10
    {500, 707}, // B2
    {353, 500}, // B3
    {250, 353}, // B4
    {125, 176}, // B6
    {88, 125}, // B7
    {62, 88}, // B8
    {33, 62}, // B9
    {163, 229}, // C5E
    {105, 241}, // US Common
    {110, 220}, // DLE
    {210, 330}, // Folio
    {431.8f, 279.4f}, // Ledger
    {279.4f, 431.8f} // Tabloid
};

// "raw" because it does not swap for Landscape
static QSizeF rawPaperSize(QPrinter::PageSize pageSize) {
    return QSizeF(KDReports::mmToPixels(kdr_paperSizes[pageSize][0]),
                  KDReports::mmToPixels(kdr_paperSizes[pageSize][1]));
}

QSizeF KDReports::ReportPrivate::paperSize() const
{
    // determine m_paperSize from m_pageSize if needed
    if (m_paperSize.isEmpty()) {
        int width_index = 0;
        int height_index = 1;
        if (m_orientation == QPrinter::Landscape) {
            width_index = 1;
            height_index = 0;
        }
        m_paperSize = QSizeF(mmToPixels(kdr_paperSizes[m_pageSize][width_index]),
                             mmToPixels(kdr_paperSizes[m_pageSize][height_index]));
    }
    //qDebug() << "m_paperSize=" << m_paperSize;
    return m_paperSize;
}

void KDReports::ReportPrivate::ensureLayouted()
{
    // We need to do a layout if
    // m_pageContentSizeDirty is true, i.e. page size has changed etc.
    if (m_pageContentSizeDirty) {
        if (!wantEndlessPrinting()) {
            setPaperSizeFromPrinter(paperSize());
        } else {
            // Get the document to fit into one page
            Q_ASSERT(m_layoutWidth != 0);
            qreal textDocWidth = m_layoutWidth - mmToPixels(m_marginLeft + m_marginRight);
            m_paperSize = layoutAsOnePage(textDocWidth);

            qDebug() << "setPaperSizeFromPrinter: endless printer. m_layoutWidth=" << m_layoutWidth
              << "textDocWidth=" << textDocWidth
              << "single page has size" << m_paperSize << "pixels";

            Q_ASSERT(m_layout->numberOfPages() == 1);
        }
        // at this point m_pageContentSizeDirty has been set to false in all cases
    }

    m_layout->ensureLayouted();
}

// The height of the text doc, by calculation. Adjusted by caller, if negative.
qreal KDReports::ReportPrivate::rawMainTextDocHeight() const
{
    qreal textDocHeight = paperSize().height() - mmToPixels( m_marginTop + m_marginBottom );
    const qreal headerHeight = m_headers.height();
    textDocHeight -= headerHeight;
    textDocHeight -= mmToPixels( m_headerBodySpacing );
    const qreal footerHeight = m_footers.height();
    textDocHeight -= mmToPixels( m_footerBodySpacing );
    textDocHeight -= footerHeight;
    //qDebug() << "pageContent height (pixels): paper size" << m_paperSize.height() << "minus margins" << mmToPixels( m_marginTop + m_marginBottom )
    //        << "minus headerHeight" << headerHeight << "minus footerHeight" << footerHeight << "and spacings =" << textDocHeight;
    return textDocHeight;
}

bool KDReports::ReportPrivate::skipHeadersFooters() const
{
    const bool skip = rawMainTextDocHeight() <= 0;
    if ( skip ) {
        qDebug() << "Not enough height for headers and footers in this page size, hiding headers and footers.";
    }
    return skip;
}

qreal KDReports::ReportPrivate::mainTextDocHeight() const
{
    const qreal height = rawMainTextDocHeight();
    const bool skip = height <= 0;
    if ( skip ) {
        qreal textDocHeight = paperSize().height() - mmToPixels( m_marginTop + m_marginBottom );
        textDocHeight -= mmToPixels( m_headerBodySpacing );
        textDocHeight -= mmToPixels( m_footerBodySpacing );
        return textDocHeight;
    }
    return height;
}

/*
   [top margin]
   [header]
   [m_headerBodySpacing]
   [body]
   [m_footerBodySpacing]
   [footer]
   [bottom margin]
 */
void KDReports::ReportPrivate::setPaperSizeFromPrinter( const QSizeF& paperSize )
{
    Q_ASSERT( !wantEndlessPrinting() ); // call ensureLayouted instead!

    m_paperSize = paperSize;
    const qreal marginsInPixels = mmToPixels( m_marginLeft + m_marginRight );
    qreal textDocWidth = m_paperSize.width() - marginsInPixels;

    m_headers.layoutWithTextWidth( textDocWidth );
    m_footers.layoutWithTextWidth( textDocWidth );

    const qreal textDocHeight = mainTextDocHeight();

    // Font scaling
    // Problem: how to re-implement this without a layout document?
    // We would risk cumulating rounding problems...?
    // ### 2nd problem: what about fonts in headers and footers? shouldn't they scale too?
    //if ( m_scaleFontsBy != 1.0 )
    //    m_textDocument.scaleFontsBy( m_scaleFontsBy );

    m_layout->setPageContentSize( QSizeF( textDocWidth, textDocHeight ) );

    m_pageContentSizeDirty = false;
}

KDReports::Header* KDReports::HeaderMap::headerForPage( int pageNumber /* 1-based */, int pageCount ) const
{
    Header* firstPageHeader = 0;
    Header* lastPageHeader = 0;
    Header* evenPagesHeader = 0;
    Header* oddPagesHeader = 0;
    for ( const_iterator it = begin(); it != end(); ++it ) {
        const KDReports::HeaderLocations loc = it.key();
        Header* const h = it.value();
        if ( loc & KDReports::FirstPage ) firstPageHeader = h;
        if ( loc & KDReports::LastPage ) lastPageHeader = h;
        if ( loc & KDReports::EvenPages ) evenPagesHeader = h;
        if ( loc & KDReports::OddPages ) oddPagesHeader = h;
    }
    if ( pageNumber == 1 && firstPageHeader )
        return firstPageHeader;
    if ( pageNumber == pageCount && lastPageHeader )
        return lastPageHeader;
    if ( pageNumber & 1 ) // odd
        return oddPagesHeader;
    else // even
        return evenPagesHeader;
}

KDReports::HeaderLocations KDReports::HeaderMap::headerLocation( Header *header ) const
{
    for ( const_iterator it = begin(); it != end(); ++it ) {
        const KDReports::HeaderLocations loc = it.key();
        Header* const h = it.value();
        if(h == header){
            return loc;
        }
    }
    KDReports::HeaderLocations loc;
    return loc;
}

void KDReports::ReportPrivate::paintPage( int pageNumber, QPainter& painter )
{
    ensureLayouted();

    const int pageCount = m_layout->numberOfPages();
    KDReports::Header* header = m_headers.headerForPage( pageNumber + 1, pageCount );
    if ( header ) {
        header->preparePaintingPage( pageNumber + m_firstPageNumber - 1 );
    }
    KDReports::Header* footer = m_footers.headerForPage( pageNumber + 1, pageCount );
    if ( footer ) {
        footer->preparePaintingPage( pageNumber + m_firstPageNumber - 1 );
    }

    const qreal textDocWidth = m_paperSize.width() - mmToPixels( m_marginLeft + m_marginRight );
    const qreal textDocHeight = mainTextDocHeight();

    const int left = qRound( mmToPixels( m_marginLeft ) );
    const int top = qRound( mmToPixels( m_marginTop ) );
    //const int right = qRound( mmToPixels( m_marginRight ) );
    const int bottom = qRound( mmToPixels( m_marginBottom ) );
    const bool skipHeadersFooters = this->skipHeadersFooters();
    const int headerHeightWithSpacing = qRound( ( skipHeadersFooters ? 0 : m_headers.height() ) + mmToPixels( m_headerBodySpacing ) );
    const int footerHeight = skipHeadersFooters ? 0 : qRound( m_footers.height() );
    //const int footerHeightWithspacing = qRound( m_footers.height() + mmToPixels( m_footerBodySpacing ) );
    //const QRect paperRect( 0, 0, qRound( m_paperSize.width() ), qRound( m_paperSize.height() ) );
    //const QRect textDocRect = paperRect.adjusted( left, top + headerHeightWithSpacing,
    //                                              -right, - bottom - footerHeightWithspacing);
    const QRect textDocRect( left, top + headerHeightWithSpacing,
                             textDocWidth, textDocHeight );

    /*qDebug() << "paintPage: in pixels: top=" << top << " headerHeight=" << headerHeightWithSpacing
             << " textDoc size:" << textDocRect.size()
             << " bottom=" << bottom << " footerHeight=" << footerHeight;*/

    if( !m_watermarkText.isEmpty() ) {
        painter.save();
        painter.translate( textDocRect.center() );
        painter.rotate( m_watermarkRotation );
        painter.setFont( m_watermarkFont );
        painter.setPen( m_watermarkColor );
        const QSize textSize( painter.fontMetrics().size( Qt::TextSingleLine, m_watermarkText ) );
        const QRect textRect( -textSize.width() / 2, -textSize.height()/2, textSize.width(), textSize.height() );
        painter.drawText( textRect, Qt::AlignCenter, m_watermarkText );
        painter.restore();
    }

    if( !m_watermarkImage.isNull() ) {
        // We paint it without scaling it, for better quality.
        // But this means the actual size depends on the zoom level or printer resolution...
        //
        // It also means the image could end up being bigger than the page, and we don't want that.
        // So we scale down if necessary. But never up.
        QImage img = m_watermarkImage;
        if ( m_watermarkImage.width() > textDocRect.width() ||
             m_watermarkImage.height() > textDocRect.height() ) {
            // should probably be cached?
            img = m_watermarkImage.scaled( textDocRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
        }
        const QRect imageRect = QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, img.size(), textDocRect );
        //qDebug() << "textDocRect=" << textDocRect << "size=" << img.size() << "-> imageRect=" << imageRect;
        painter.drawImage( imageRect.topLeft(), img );
    }


    painter.save();
    //painter.setClipRect( textDocRect, Qt::IntersectClip ); // triggers a Qt-Windows bug when printing
    painter.setClipRect( textDocRect );
    painter.translate( left, top + headerHeightWithSpacing );
    m_layout->paintPageContent( pageNumber, painter );
    painter.restore();

    if ( header && !skipHeadersFooters ) {
        painter.save();
        painter.translate( left, top );
        header->doc().contentDocument().drawContents( &painter, painter.clipRegion().boundingRect() );
        painter.restore();
    }
    if ( footer && !skipHeadersFooters ) {
        painter.save();
        painter.translate( left, m_paperSize.height() - bottom - footerHeight );
        footer->doc().contentDocument().drawContents( &painter, painter.clipRegion().boundingRect() );
        painter.restore();
    }
}

QSizeF KDReports::ReportPrivate::layoutAsOnePage( qreal docWidth )
{
    m_headers.layoutWithTextWidth( docWidth );
    m_footers.layoutWithTextWidth( docWidth );

    const qreal docHeight = m_layout->layoutAsOnePage(docWidth);

    qreal pageWidth = docWidth + mmToPixels( m_marginLeft + m_marginRight );
    qreal pageHeight = docHeight + mmToPixels( m_marginTop + m_marginBottom );
    pageHeight += m_headers.height();
    pageHeight += m_footers.height();

    m_pageContentSizeDirty = false;

    //qDebug() << "One-page document is" << pageWidth << "x" << pageHeight;
    return QSizeF( pageWidth, pageHeight );
}

void KDReports::ReportPrivate::headerChanged()
{
    m_pageContentSizeDirty = true;
}

bool KDReports::ReportPrivate::doPrint( QPrinter* printer, QWidget* parent )
{
    // caller has to ensure that we have been layouted for this printer already
    const int pageCount = m_layout->numberOfPages();
    QProgressDialog* dialog = 0;
    if ( QThread::currentThread() == qApp->thread() ) {
        dialog = new QProgressDialog( QObject::tr( "Printing" ), QObject::tr( "Cancel" ), 0, pageCount, parent );
        dialog->setWindowModality( Qt::ApplicationModal );
    }
    QPainter painter( printer );

    int fromPage = 0;
    int toPage = pageCount;
    if ( printer->printRange() == QPrinter::PageRange ) {
        fromPage = printer->fromPage() - 1; // it starts at 1
        toPage = printer->toPage(); // -1 because it starts at 1, and +1 because of '<'
        if ( toPage == 0 )
            toPage = pageCount;
    }

    bool firstPage = true;
    for ( int pageIndex = fromPage; pageIndex < toPage; ++pageIndex ) {
        if ( dialog ) {
            dialog->setValue( pageIndex );
            if ( dialog->wasCanceled() )
                break;
        }

        if ( !firstPage )
            printer->newPage();

        paintPage( pageIndex, painter );
        firstPage = false;
    }

    delete dialog;
    return true;
}

#ifndef NDEBUG
void KDReports::ReportPrivate::debugLayoutToPdf( const char* fileName )
{
    // for calling from gdb

    QFile html( QFile::decodeName( fileName ) + QLatin1String( ".html" ) );
    Q_ASSERT( html.open( QIODevice::WriteOnly ) );
    const QString htmlText = m_layout->toHtml();
    html.write( htmlText.toUtf8() );
    html.close();

    bool oldLayoutDirty = true;
    m_pageContentSizeDirty = false;
    QPrinter printer;
    q->setupPrinter( &printer );
    printer.setOutputFileName( QFile::decodeName( fileName ) );
    doPrint( &printer, 0 );
    printer.setOutputFileName( QString() );
    m_pageContentSizeDirty = oldLayoutDirty;
}
#endif

typedef QMap<QString,QAbstractItemModel*> ModelMap;
Q_GLOBAL_STATIC( ModelMap, globalModelMap )

QAbstractItemModel* KDReports::modelForKey( const QString& key )
{
    return globalModelMap()->value( key, 0 );
}

qreal KDReports::ReportPrivate::textDocumentWidth() const
{
    return paperSize().width() - mmToPixels( m_marginLeft + m_marginRight );
}

KDReports::ReportBuilder* KDReports::ReportPrivate::builder()
{
    if (m_reportMode == KDReports::Report::WordProcessing)
        return static_cast<TextDocReportLayout *>(m_layout)->builder();
    return 0;
}

////

KDReports::Report::Report( QObject* parent )
    : QObject( parent ), d( new ReportPrivate( this ) )
{
    setPageSize( QPrinter::A4 );
}

KDReports::Report::~Report()
{
    delete d;
}

void KDReports::Report::addInlineElement( const Element& element )
{
    if (d->m_reportMode != WordProcessing) {
        qWarning("KDReports: addInlineElement is only supported in WordProcessing mode");
    } else {
        d->builder()->addInlineElementPublic( element );
        // don't add anything else here, it won't be called from the xml parser
    }
}

void KDReports::Report::addElement( const Element& element, Qt::AlignmentFlag horizontalAlignment, const QColor& backgroundColor )
{
    if (d->m_reportMode != WordProcessing) {
        qWarning("KDReports: addElement is only supported in WordProcessing mode");
    } else {
        d->builder()->addBlockElementPublic( element, horizontalAlignment, backgroundColor );
        // don't add anything else here, it won't be called from the xml parser
    }
}

void KDReports::Report::addVerticalSpacing( qreal space )
{
    if (d->m_reportMode != WordProcessing) {
        qWarning("KDReports: addVerticalSpacing is only supported in WordProcessing mode");
    } else {
        d->builder()->addVerticalSpacingPublic( space );
    }
}

void KDReports::Report::setPageSize( const QPrinter::PageSize& size )
{
    d->m_pageSize = size;
    d->m_paperSize = QSizeF();
    d->m_pageContentSizeDirty = true;
}

void KDReports::Report::setPaperSize( const QSizeF & paperSize, QPrinter::Unit unit )
{
    qreal factor = 1.0;
    switch ( unit ) {
    case QPrinter::DevicePixel:
        break;
    case QPrinter::Millimeter:
        factor = mmToPixels( 1.0 );
        break;
    case QPrinter::Point:
        factor = 72.0 * qt_defaultDpi();
        break;
    case QPrinter::Inch:
        factor = qt_defaultDpi();
        break;
    default:
        qWarning( "Unsupported printer unit %d", unit );
    }
    d->m_paperSize = QSizeF( paperSize.width() * factor, paperSize.height() * factor );
    d->m_pageContentSizeDirty = true;
}

QPrinter::PageSize KDReports::Report::pageSize() const
{
    return d->m_pageSize;
}

void KDReports::Report::setOrientation( QPrinter::Orientation orientation )
{
    d->m_orientation = orientation;
    d->m_paperSize = QSizeF();
    d->m_pageContentSizeDirty = true;
}

QPrinter::Orientation KDReports::Report::orientation() const
{
    return d->m_orientation;
}

void KDReports::Report::setMargins( qreal top, qreal left, qreal bottom, qreal right )
{
    d->m_marginTop = top;
    d->m_marginLeft = left;
    d->m_marginBottom = bottom;
    d->m_marginRight = right;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

void KDReports::Report::getMargins( qreal* top, qreal* left, qreal* bottom, qreal* right ) const
{
    *top = d->m_marginTop;
    *left = d->m_marginLeft;
    *bottom = d->m_marginBottom;
    *right = d->m_marginRight;
}

void KDReports::Report::setLeftPageMargin(qreal left)
{
    d->m_marginLeft = left;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal KDReports::Report::leftPageMargins() const
{
    return d->m_marginLeft;
}

void KDReports::Report::setRightPageMargin(qreal right)
{
    d->m_marginRight = right;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal KDReports::Report::rightPageMargins() const
{
    return d->m_marginRight;
}

void KDReports::Report::setTopPageMargin(qreal top)
{
    d->m_marginTop = top;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal KDReports::Report::topPageMargins() const
{
    return d->m_marginTop;
}

void KDReports::Report::setBottomPageMargin(qreal bottom)
{
    d->m_marginBottom = bottom;

    // We'll need to call setPaperSizeFromPrinter, to update % sizes in all text documents.
    d->m_pageContentSizeDirty = true;
}

qreal KDReports::Report::bottomPageMargins() const
{
    return d->m_marginBottom;
}


void KDReports::Report::setWidthForEndlessPrinter( qreal widthMM )
{
    if ( widthMM ) {
        d->m_endlessPrinterWidth = widthMM;
        d->m_layoutWidth = mmToPixels( widthMM );
        d->m_pageContentSizeDirty = true;
        d->ensureLayouted();
    } else {
        d->m_layoutWidth = 0;
        d->m_pageContentSizeDirty = true;
        // caller will call setPageSize...
    }
}

void KDReports::Report::paintPage( int pageNumber, QPainter& painter )
{
    d->paintPage( pageNumber, painter );
}

int KDReports::Report::numberOfPages() const
{
    d->ensureLayouted();
    return d->m_layout->numberOfPages();
}

void KDReports::Report::dump() const
{
    qDebug() << asHtml();
}

QString KDReports::Report::asHtml() const
{
    return d->m_layout->toHtml();
}

bool KDReports::Report::printWithDialog( QWidget* parent )
{
    QPrinter printer;
    setupPrinter( &printer );
    QPointer<QPrintDialog> dialog = new QPrintDialog( &printer, parent );
    dialog->setMinMax( 1, numberOfPages() );
    bool ok = false;
    if ( dialog->exec() == QDialog::Accepted ) {
        // Well, the user can modify the page size in the printer dialog too - ensure layout matches
        d->ensureLayouted();
        ok = d->doPrint( &printer, parent );
    }
    delete dialog;
    return ok;
}

bool KDReports::Report::print( QPrinter* printer, QWidget* parent )
{
    // save the old page size
    QPrinter::PageSize savePageSize = pageSize();
    if ( d->wantEndlessPrinting() ) {
        // ensure that the printer is set up with the right size
        d->ensureLayouted();
        printer->setPaperSize( d->m_paperSize, QPrinter::DevicePixel );
    } else {
        // ensure that the layout matches the printer
        d->setPaperSizeFromPrinter( printer->paperRect().size() );
    }

    printer->setFullPage( true );

    // don't call ensureLayouted here, it would use the wrong printer!

    const bool ret = d->doPrint( printer, parent );

    // Reset the page size
    setPageSize( savePageSize );

    return ret;
}

bool KDReports::Report::exportToFile( const QString& fileName, QWidget* parent )
{
    d->ensureLayouted();
    QPrinter printer;
    setupPrinter( &printer );
    printer.setOutputFileName( fileName );
    const bool ret = d->doPrint( &printer, parent );
    printer.setOutputFileName( QString() );
    return ret;
}

bool KDReports::Report::exportToHtml( const QString& fileName )
{
    const QString html = asHtml();
    QFile file( fileName );
    if ( file.open( QIODevice::WriteOnly ) ) {
        file.write( html.toUtf8() );
        d->m_layout->finishHtmlExport();
        return true;
    }
    return false;
}

bool KDReports::Report::exportToImage( const QSize& size, const QString& fileName, const char* format )
{
    // Get the document to fit into one page

    QPrinter::PageSize savePageSize = pageSize();
    const qreal saveLayoutWidth = d->m_layoutWidth;
    d->m_layoutWidth = d->m_layout->idealWidth() + mmToPixels(d->m_marginLeft + d->m_marginRight);
    d->m_pageContentSizeDirty = true;
    d->ensureLayouted();

    const qreal zoomFactor = qMin( (qreal)size.width() / d->m_paperSize.width(),
                                   (qreal)size.height() / d->m_paperSize.height() );
    //qDebug() << "zoomFactor=" << zoomFactor;

    QImage image( size, QImage::Format_ARGB32_Premultiplied );
    image.fill( Qt::white );

    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.fillRect( QRectF(0, 0, size.width(), size.height()), QBrush(Qt::white) );
    painter.scale( zoomFactor, zoomFactor );
    d->paintPage( 0, painter );

    // restore textdoc size and header widths
    d->m_layoutWidth = saveLayoutWidth;
    setPageSize( savePageSize ); // redo layout
    qDebug() << "Saving pixmap" << image.size() << "into" << fileName << "with format" << format;
    return image.save( fileName, format );
}

KDReports::Header& KDReports::Report::header( HeaderLocations hl )
{
    if ( !d->m_headers.contains( hl ) )
        d->m_headers.insert( hl, new Header( this ) );
    return * d->m_headers.value( hl );
}

void KDReports::Report::setHeaderLocation( HeaderLocations hl, Header* header )
{
    // Remove old entry for this header
    HeaderLocations loc = d->m_headers.headerLocation(header);
    d->m_headers.remove( loc );
    d->m_headers.insert( hl, header );
}

KDReports::Header& KDReports::Report::footer( HeaderLocations hl )
{
    if ( !d->m_footers.contains( hl ) )
        d->m_footers.insert( hl, new Header( this ) );
    return * d->m_footers.value( hl );
}

void KDReports::Report::setFooterLocation( HeaderLocations hl, Footer* footer )
{
    // Remove old entry for this header
    HeaderLocations loc = d->m_footers.headerLocation(footer);
    d->m_footers.remove( loc );
    d->m_footers.insert( hl, footer);
}

bool KDReports::Report::loadFromXML( QIODevice* iodevice, ErrorDetails* details )
{
    QDomDocument doc;
    // Read document from the QIODevice, check for errors

    // We need to be able to see the space in <text> </text>, this is why
    // we activate the "report-whitespace-only-CharData" feature.
    // Unfortunately this leads to lots of whitespace text nodes in between real
    // elements in the rest of the document, watch out for that.
    if (iodevice->isOpen())
        iodevice->reset(); //need to do that to allow consecutive calls of loadFromXML()
    QXmlInputSource source( iodevice );
    QXmlSimpleReader reader;
    reader.setFeature( QLatin1String( "http://xml.org/sax/features/namespaces" ), false );
    reader.setFeature( QLatin1String( "http://xml.org/sax/features/namespace-prefixes" ), true );
    reader.setFeature( QLatin1String( "http://trolltech.com/xml/features/report-whitespace-only-CharData" ), true );

    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    bool ret = doc.setContent( &source, &reader, &errorMsg, &errorLine, &errorColumn );
    if( !ret ) {
        if ( details ) {
            details->setLine( errorLine );
            details->setColumn( errorColumn );
            details->setDriverMessage( errorMsg );
        }
        else
            qWarning( "Malformed XML read in KDReports::Report::loadFromXML(): error message = %s, error line = %d, error column = %d", qPrintable( errorMsg ), errorLine, errorColumn );
        return false;
    }
    return loadFromXML( doc, details );
}


bool KDReports::Report::loadFromXML( const QDomDocument& doc, ErrorDetails* details)
{
    XmlParser parser( d->m_textValues, d->m_imageValues, d->m_xmlElementHandler, this, details );
    d->m_pageContentSizeDirty = true;
    return parser.processDocument( doc, d->builder() );
}

void KDReports::Report::associateModel( const QString& modelKey, QAbstractItemModel* model )
{
    globalModelMap()->insert( modelKey, model );
}

KDReports::TextDocument& KDReports::Report::doc() const
{
    Q_ASSERT(d->m_reportMode == WordProcessing);
    return static_cast<TextDocReportLayout *>(d->m_layout)->textDocument();
}

void KDReports::Report::beginEdit()
{
    d->builder()->contentDocumentCursor().beginEditBlock();
}

void KDReports::Report::endEdit()
{
    d->builder()->contentDocumentCursor().endEditBlock();
}

void KDReports::Report::setWatermarkText( const QString& text,
                                          int rotation,
                                          const QColor& color,
                                          const QFont& font )
{
    d->m_watermarkText = text;
    d->m_watermarkRotation = rotation;
    d->m_watermarkColor = color;
    d->m_watermarkFont = font;
}

QString KDReports::Report::watermarkText() const
{
    return d->m_watermarkText;
}

int KDReports::Report::watermarkRotation() const
{
    return d->m_watermarkRotation;
}

QColor KDReports::Report::watermarkColor() const
{
    return d->m_watermarkColor;
}

QFont KDReports::Report::watermarkFont() const
{
    return d->m_watermarkFont;
}

void KDReports::Report::setWatermarkPixmap( const QPixmap& pixmap, bool autoGrayOut )
{
    QPixmap pix( pixmap );
    if( autoGrayOut ) {
        QStyleOption opt(0);
        opt.palette = QApplication::palette();
        pix = qApp->style()->generatedIconPixmap( QIcon::Disabled, pixmap, &opt );
    }
    setWatermarkImage( pix.toImage() );
}

QPixmap KDReports::Report::watermarkPixmap() const
{
    return QPixmap::fromImage( d->m_watermarkImage );
}

void KDReports::Report::setWatermarkImage( const QImage& image )
{
    d->m_watermarkImage = image;
}

QImage KDReports::Report::watermarkImage() const
{
    return d->m_watermarkImage;
}

void KDReports::Report::addPageBreak()
{
    d->builder()->addPageBreakPublic();
}

void KDReports::Report::associateTextValue( const QString& id, const QString& value )
{
    d->m_layout->updateTextValue( id, value ); // in case the document is built already
    d->m_headers.updateTextValue( id, value );
    d->m_footers.updateTextValue( id, value );
    d->m_textValues.insert( id, value ); // in case the document isn't built yet
}

void KDReports::Report::associateImageValue( const QString& id, const QPixmap& value )
{
    d->m_imageValues.insert( id, value.toImage() );
}

void KDReports::Report::associateImageValue( const QString& id, const QImage& value )
{
    d->m_imageValues.insert( id, value );
}

QSizeF KDReports::Report::paperSize() const
{
    return d->paperSize();
}

void KDReports::Report::addFragment( const QTextDocumentFragment& fragment )
{
    d->builder()->insertFragmentPublic( fragment );
}

void KDReports::Report::setDefaultFont( const QFont& font )
{
    d->m_layout->setDefaultFont(font);
    d->m_pageContentSizeDirty = true;
}

QFont KDReports::Report::defaultFont() const
{
    return d->m_layout->defaultFont();
}

void KDReports::Report::setHeaderBodySpacing( qreal spacing )
{
    d->m_headerBodySpacing = spacing;
    d->m_pageContentSizeDirty = true;
}

qreal KDReports::Report::headerBodySpacing() const
{
    return d->m_headerBodySpacing;
}

void KDReports::Report::setFooterBodySpacing( qreal spacing )
{
    d->m_footerBodySpacing = spacing;
    d->m_pageContentSizeDirty = true;
}

qreal KDReports::Report::footerBodySpacing() const
{
    return d->m_footerBodySpacing;
}

void KDReports::Report::scaleTo( int numPagesHorizontally, int numPagesVertically )
{
    d->m_layout->scaleTo(numPagesHorizontally, numPagesVertically);
}

int KDReports::Report::maximumNumberOfPagesForHorizontalScaling() const
{
    return d->m_layout->maximumNumberOfPagesForHorizontalScaling();
}

int KDReports::Report::maximumNumberOfPagesForVerticalScaling() const
{
    return d->m_layout->maximumNumberOfPagesForVerticalScaling();
}

void KDReports::Report::setFixedRowHeight(qreal mm)
{
    d->m_layout->setFixedRowHeight( mmToPixels(mm) );
}

void KDReports::Report::setFontScalingFactor( qreal factor )
{
    d->m_layout->setUserRequestedFontScalingFactor(factor);
}

qreal KDReports::Report::fontScalingFactor() const
{
    return d->m_layout->userRequestedFontScalingFactor();
}

void KDReports::Report::setTableBreakingEnabled( bool b )
{
    if (b)
      qWarning("setTableBreakingEnabled() is deprecated and has no effect anymore");
}

bool KDReports::Report::isTableBreakingEnabled() const
{
    return maximumNumberOfPagesForHorizontalScaling() != 1 || maximumNumberOfPagesForVerticalScaling() > 0;
}

void KDReports::Report::setTableBreakingPageOrder( TableBreakingPageOrder pageOrder )
{
    if (d->m_reportMode != SpreadSheet) {
        qWarning("setTableBreakingPageOrder is only supported in SpreadSheet mode");
    } else {
        mainTable()->setTableBreakingPageOrder(pageOrder);
    }
}

KDReports::Report::TableBreakingPageOrder KDReports::Report::tableBreakingPageOrder() const
{
    if (d->m_reportMode != SpreadSheet) {
        qWarning("tableBreakingPageOrder is only supported in SpreadSheet mode");
        return DownThenRight;
    } else {
        return mainTable()->tableBreakingPageOrder();
    }
}

void KDReports::Report::regenerateAutoTables()
{
    if (d->m_reportMode == WordProcessing)
        doc().regenerateAutoTables();
}

void KDReports::Report::regenerateAutoTableForModel( QAbstractItemModel* model )
{
    if (d->m_reportMode == WordProcessing)
        doc().regenerateAutoTableForModel( model );
}

QList<KDReports::AutoTableElement *> KDReports::Report::autoTableElements() const
{
    if (d->m_reportMode == WordProcessing)
        return doc().autoTableElements();
    return QList<KDReports::AutoTableElement *>();
}

void KDReports::Report::setXmlElementHandler( KDReports::XmlElementHandler* handler )
{
    d->m_xmlElementHandler = handler;
}

void KDReports::Report::setCurrentRow( const QAbstractItemModel *model, int row )
{
    d->m_currentModel = model;
    d->m_currentRow = row;
}

void KDReports::Report::setDocumentName(const QString &name)
{
    d->m_documentName = name;
}

QString KDReports::Report::documentName() const
{
    return d->m_documentName;
}

void KDReports::Report::setTabPositions( const QList<QTextOption::Tab>& tabs )
{
    d->builder()->setTabPositions( tabs);
}

void KDReports::Report::setParagraphMargins( qreal left, qreal top, qreal right, qreal bottom )
{
    d->builder()->setParagraphMargins( left, top, right, bottom );
}

void KDReports::Report::setReportMode(ReportMode reportMode)
{
    if (d->m_reportMode != reportMode) {
        d->m_reportMode = reportMode;
        delete d->m_layout;
        switch(reportMode) {
        case WordProcessing:
            d->m_layout = new TextDocReportLayout(this);
            break;
        case SpreadSheet:
            SpreadsheetReportLayout* sslayout = new SpreadsheetReportLayout(this);
            d->m_layout = sslayout;
            mainTable()->setLayout(sslayout);
            break;
        };
    }
}

KDReports::Report::ReportMode KDReports::Report::reportMode() const
{
    return d->m_reportMode;
}

KDReports::MainTable *KDReports::Report::mainTable() const
{
    Q_ASSERT(d->m_reportMode == SpreadSheet);
    return d->m_mainTable;
}

QTextOption::Tab KDReports::Report::rightAlignedTab() const
{
    QTextOption::Tab tab;
    tab.position = -1;
    tab.type = QTextOption::RightTab;
    tab.delimiter = QChar::fromLatin1('P'); // a bit hackish, but this is who we tell TextDocumentData::updatePercentSize
    return tab;
}

QTextOption::Tab KDReports::Report::middleAlignedTab() const
{
    QTextOption::Tab tab;
    tab.position = -1;
    tab.type = QTextOption::CenterTab;
    tab.delimiter = QChar::fromLatin1('P'); // a bit hackish, but this is who we tell TextDocumentData::updatePercentSize
    return tab;
}

KDReports::HeaderLocations KDReports::Report::headerLocation(KDReports::Header *header) const
{
    return d->m_headers.headerLocation(header);
}

KDReports::HeaderLocations KDReports::Report::footerLocation(KDReports::Footer *footer) const
{
    return d->m_footers.headerLocation(footer);
}

int KDReports::Report::currentPosition() const
{
    return d->builder()->currentPosition();
}

void KDReports::Report::setFirstPageNumber(int num)
{
    d->m_firstPageNumber = num;
}

int KDReports::Report::firstPageNumber() const
{
    return d->m_firstPageNumber;
}

void KDReports::Report::setupPrinter( QPrinter* printer )
{
    printer->setFullPage( true );
    printer->setOrientation( d->m_orientation );
    printer->setPaperSize( rawPaperSize(d->m_pageSize), QPrinter::DevicePixel );
    printer->setDocName( d->m_documentName );
}

#include "moc_KDReportsReport.cpp"
