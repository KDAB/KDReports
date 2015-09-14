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

#include "KDReportsPreviewWidget.h"
#include <QTime>
#include <QDebug>
#include <QShortcut>
#include <QPrintDialog>
#include <QKeyEvent>
#include <QProgressDialog>
#include "KDReportsReport.h"
#include <QTimer>
#include <QPointer>
#include <QPainter>
#include <qmath.h> // qCeil

#include "ui_previewdialogbase.h"

enum { PreviewSize = 200 };

/// @internal
/// The widget showing the large preview on the right
class PagePreviewWidget : public QWidget
{
public:
    PagePreviewWidget( QWidget *parent = 0 ) : QWidget( parent ) {}
    void setPixmap( const QPixmap &pixmap ) {
        m_pixmap = pixmap;
        update();
    }
    int pixmapWidth() const { return m_pixmap.width(); }
    int pixmapHeight() const { return m_pixmap.height(); }
    QSize pixmapSize() const { return m_pixmap.size(); }

protected:
    void paintEvent( QPaintEvent * ) {
        QPainter painter(this);
        const QPoint offset( ( width() - m_pixmap.width() ) / 2,
                             ( height() - m_pixmap.height() ) / 2 );
        //painter.fillRect( event->rect(), QColor(224,224,224) );
        painter.drawPixmap( offset, m_pixmap );
    }
private:
    QPixmap m_pixmap;
};


class KDReports::PreviewWidgetPrivate : public Ui::PreviewWidgetBase
{
public:
    PreviewWidgetPrivate( KDReports::PreviewWidget* q );

    // not in ctor because the init calls q->slotFoo which uses d->, so d must be set first.
    void init();

    void paintItem( QListWidgetItem *item, int index );
    QPixmap paintPreview( int index );
    void printSelectedPages();
    void setupComboBoxes();
    void pageCountChanged();
    void centerPreview();
    void zoomChanged();
    void fillZoomCombo();
    void updatePageButtons();
    void updatePreview();
    void pageNumberReturnPressed();
    void setReport( KDReports::Report* report );

    void _kd_slotCurrentPageChanged();
    void _kd_slotFirstPage();
    void _kd_slotPrevPage();
    void _kd_slotNextPage();
    void _kd_slotLastPage();
    void _kd_slotPaperSizeActivated( int );
    void _kd_slotPaperOrientationActivated( int );
    void _kd_slotZoomIn();
    void _kd_slotZoomOut();
    void _kd_slotZoomChanged();
    void _kd_previewNextItems();

    PagePreviewWidget* m_previewWidget;
    QPrinter m_printer;
    qreal m_zoomFactor;
    qreal m_endlessPrinterWidth;
    KDReports::Report* m_report;
    QTimer m_previewTimer;
    KDReports::PreviewWidget* q;
    int m_pageCount;
    int m_firstDirtyPreviewItem;
    bool m_eatPageNumberClick;
};

KDReports::PreviewWidgetPrivate::PreviewWidgetPrivate( KDReports::PreviewWidget* w )
    : m_previewWidget( new PagePreviewWidget ),
      m_printer(),
      m_zoomFactor( 1.0 ),
      m_endlessPrinterWidth( 114.0 ),
      m_report( 0 ),
      q( w ),
      m_pageCount( 0 ),
      m_firstDirtyPreviewItem( -1 ),
      m_eatPageNumberClick( false )
{
    QObject::connect( &m_previewTimer, SIGNAL(timeout()), q, SLOT(_kd_previewNextItems()) );
}

void KDReports::PreviewWidgetPrivate::init()
{
    setupUi( q );
    actionBar->setEnabled(false );

    m_printer.setFullPage( true );

    pageList->setIconSize( QSize(PreviewSize, PreviewSize) );
    pageList->setViewMode( QListView::IconMode );
    pageList->setMovement( QListView::Static );
    previewArea->setWidget( m_previewWidget );
    setupComboBoxes();
    previewArea->setFocus();
    fillZoomCombo();

    //m_tableBreakingButton = buttonBox->addButton( tr("Table Breaking / Font Scaling..." ), QDialogButtonBox::ActionRole );
    QObject::connect( tableBreakingButton, SIGNAL(clicked()), q, SIGNAL(tableSettingsClicked()) );

    QObject::connect( firstPage, SIGNAL(clicked()), q, SLOT(_kd_slotFirstPage()) );
    QObject::connect( prevPage, SIGNAL(clicked()), q, SLOT(_kd_slotPrevPage()) );
    QObject::connect( nextPage, SIGNAL(clicked()), q, SLOT(_kd_slotNextPage()) );
    QObject::connect( lastPage, SIGNAL(clicked()), q, SLOT(_kd_slotLastPage()) );
    QObject::connect( zoomIn, SIGNAL(clicked()), q, SLOT(_kd_slotZoomIn()) );
    QObject::connect( zoomOut, SIGNAL(clicked()), q, SLOT(_kd_slotZoomOut()) );
    QObject::connect( zoomCombo, SIGNAL(activated(QString)), q, SLOT(_kd_slotZoomChanged()) );
    QObject::connect( pageList, SIGNAL(currentRowChanged(int)), q, SLOT(_kd_slotCurrentPageChanged()) );
    QObject::connect( paperSizeCombo, SIGNAL(activated(int)), q, SLOT(_kd_slotPaperSizeActivated(int)) );
    QObject::connect( paperOrientationCombo, SIGNAL(activated(int)), q, SLOT(_kd_slotPaperOrientationActivated(int)) );

    QShortcut* nextPageShortcut = new QShortcut( q );
    nextPageShortcut->setKey( Qt::CTRL + Qt::Key_PageDown );
    QObject::connect( nextPageShortcut, SIGNAL(activated()), q, SLOT(_kd_slotNextPage()) );
    QShortcut* prevPageShortcut = new QShortcut( q );
    prevPageShortcut->setKey( Qt::CTRL + Qt::Key_PageUp );
    QObject::connect( prevPageShortcut, SIGNAL(activated()), q, SLOT(_kd_slotPrevPage()) );

    pageNumber->setValidator( new QIntValidator( 1, 100000, pageNumber ) );
    pageNumber->installEventFilter(q);
}

void KDReports::PreviewWidgetPrivate::paintItem( QListWidgetItem *item, int index )
{
    //qDebug() << "paintItem" << index;

    // Use a QImage so that the raster paint engine is used.
    // Gives a 7.7 times speedup (!) compared to X11.
    QImage img( PreviewSize, PreviewSize, QImage::Format_ARGB32_Premultiplied );
    const QSizeF paperSize = m_report->paperSize();
    const qreal longestSide = qMax(paperSize.width(),
                                   paperSize.height());
    qreal width = img.width() * paperSize.width()/longestSide;
    qreal height = img.height() * paperSize.height()/longestSide;
    //img.fill( QColor( Qt::white ).rgb() );
    img.fill( qRgba( 0, 0, 0, 0 ) ); // transparent
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate((img.width() - width)/2,
                      (img.height() - height)/2);
    painter.fillRect( QRectF(0, 0, width, height), QBrush(Qt::white) );
    painter.scale( img.width() / longestSide, img.height() / longestSide );
    m_report->paintPage( index, painter );
    painter.setPen( QPen(1) );
    painter.drawRect( QRectF( 0, 0, paperSize.width(), paperSize.height() ) );

    item->setIcon( QIcon(QPixmap::fromImage(img)) );
}

void KDReports::PreviewWidgetPrivate::_kd_previewNextItems()
{
    if ( m_firstDirtyPreviewItem == -1 || m_firstDirtyPreviewItem >= m_pageCount ) {
        m_previewTimer.stop();
        m_firstDirtyPreviewItem = -1;
    } else {
        // Do next preview
        paintItem( pageList->item( m_firstDirtyPreviewItem ), m_firstDirtyPreviewItem );
        ++m_firstDirtyPreviewItem;
    }
}

QPixmap KDReports::PreviewWidgetPrivate::paintPreview( int index )
{
    const QSizeF paperSize = m_report->paperSize();
    const int width = qCeil( paperSize.width() * m_zoomFactor );
    const int height = qCeil( paperSize.height() * m_zoomFactor );

    QPixmap pixmap( width, height );
    //qDebug() << width << "," << height;

    //qDebug() << "paintPreview " << index;

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.fillRect( QRectF(0, 0, width, height), QBrush(Qt::white) );
    painter.scale( m_zoomFactor, m_zoomFactor );
    m_report->paintPage( index, painter );
    painter.setPen( QPen(1) );
    painter.drawRect( QRectF( 0, 0, paperSize.width(), paperSize.height() ) );

    return pixmap;
}

void KDReports::PreviewWidgetPrivate::printSelectedPages()
{
    // Well, the user can modify the page size in the printer dialog too - ensure layout matches
    //qDebug() << "pageSize: " << m_printer.pageSize();
    m_report->setPageSize( m_printer.pageSize() );
    m_report->setOrientation( m_printer.orientation() );
    pageCountChanged();

    // ### But how do we match "marked pages" from a previous layout into the new layout?
    // ### Hardly makes sense...

    int markedPages = 0;
    for ( int pageIndex = 0; pageIndex < pageList->count(); ++pageIndex ) {
        if ( pageList->item(pageIndex)->checkState() == Qt::Checked )
            ++markedPages;
    }

    QProgressDialog dialog( QObject::tr( "Printing" ), QObject::tr( "Cancel" ), 0, markedPages, q );
    dialog.setWindowModality( Qt::ApplicationModal );

    QPainter painter;
    painter.begin(&m_printer);

    int printed = 0;
    bool firstPage = true;
    for (int pageIndex = 0; pageIndex < m_pageCount; ++pageIndex) {

        // OK because the dialog is modal
        qApp->processEvents();

        if (dialog.wasCanceled())
            break;

        if (q->isSelected(pageIndex)) {
            // Print all pages that are marked
            if (!firstPage)
                m_printer.newPage();

            m_report->paintPage( pageIndex, painter );
            dialog.setValue( ++printed );
            firstPage = false;
        }
    }
    painter.end();
}

void KDReports::PreviewWidgetPrivate::setupComboBoxes()
{
    paperSizeCombo->addItem(q->tr("A0 (841 x 1189 mm)"), QPrinter::A0);
    paperSizeCombo->addItem(q->tr("A1 (594 x 841 mm)"), QPrinter::A1);
    paperSizeCombo->addItem(q->tr("A2 (420 x 594 mm)"), QPrinter::A2);
    paperSizeCombo->addItem(q->tr("A3 (297 x 420 mm)"), QPrinter::A3);
    paperSizeCombo->addItem(q->tr("A4 (210 x 297 mm)"), QPrinter::A4);
    paperSizeCombo->addItem(q->tr("A5 (148 x 210 mm)"), QPrinter::A5);
    paperSizeCombo->addItem(q->tr("A6 (105 x 148 mm)"), QPrinter::A6);
    paperSizeCombo->addItem(q->tr("A7 (74 x 105 mm)"), QPrinter::A7);
    paperSizeCombo->addItem(q->tr("A8 (52 x 74 mm)"), QPrinter::A8);
    //paperSizeCombo->addItem(q->tr("A9 (37 x 52 mm)"), QPrinter::A9);
    paperSizeCombo->addItem(q->tr("B0 (1000 x 1414 mm)"), QPrinter::B0);
    paperSizeCombo->addItem(q->tr("B1 (707 x 1000 mm)"), QPrinter::B1);
    paperSizeCombo->addItem(q->tr("B2 (500 x 707 mm)"), QPrinter::B2);
    paperSizeCombo->addItem(q->tr("B3 (353 x 500 mm)"), QPrinter::B3);
    paperSizeCombo->addItem(q->tr("B4 (250 x 353 mm)"), QPrinter::B4);
    paperSizeCombo->addItem(q->tr("B5 (176 x 250 mm)"), QPrinter::B5);
    paperSizeCombo->addItem(q->tr("B6 (125 x 176 mm)"), QPrinter::B6);
    paperSizeCombo->addItem(q->tr("B7 (88 x 125 mm)"), QPrinter::B7);
    paperSizeCombo->addItem(q->tr("B8 (62 x 88 mm)"), QPrinter::B8);
    //paperSizeCombo->addItem(q->tr("B9 (44 x 62 mm)"), QPrinter::B9);
    //paperSizeCombo->addItem(q->tr("B10 (31 x 44 mm)"), QPrinter::B10);
    paperSizeCombo->addItem(q->tr("C5E (163 x 229 mm)"), QPrinter::C5E);
    paperSizeCombo->addItem(q->tr("DLE (110 x 220 mm)"), QPrinter::DLE);
    paperSizeCombo->addItem(q->tr("Executive (7.5 x 10 inches)"), QPrinter::Executive);
    paperSizeCombo->addItem(q->tr("Folio (210 x 330 mm)"), QPrinter::Folio);
    paperSizeCombo->addItem(q->tr("Ledger (432 x 279 mm)"), QPrinter::Ledger);
    paperSizeCombo->addItem(q->tr("Legal (8.5 x 14 inches)"), QPrinter::Legal);
    paperSizeCombo->addItem(q->tr("Letter (8.5 x 11 inches)"), QPrinter::Letter);
    paperSizeCombo->addItem(q->tr("Tabloid (279 x 432 mm)"), QPrinter::Tabloid);
    paperSizeCombo->addItem(q->tr("US #10 Envelope (105 x 241 mm)"), QPrinter::Comm10E);
    paperSizeCombo->addItem(q->tr("Endless printer (%1 mm wide)").arg(m_endlessPrinterWidth), QPrinter::Custom);

    paperOrientationCombo->addItem(q->tr("Portrait"), QPrinter::Portrait);
    paperOrientationCombo->addItem(q->tr("Landscape"), QPrinter::Landscape);
}

void KDReports::PreviewWidgetPrivate::_kd_slotCurrentPageChanged()
{
    updatePreview();
    updatePageButtons();
}

void KDReports::PreviewWidgetPrivate::updatePageButtons()
{
    prevPage->setEnabled( pageList->currentRow() > 0 );
    nextPage->setEnabled( pageList->currentRow() < pageList->count() - 1 );
    pageNumber->setText( QString::number( pageList->currentRow() + 1 ) );
}

void KDReports::PreviewWidgetPrivate::updatePreview()
{
    if ( !pageList->currentItem() )
        return;
    const QSize oldSize = m_previewWidget->pixmapSize();
    const QPixmap pixmap = paintPreview( pageList->currentRow() );
    m_previewWidget->setPixmap( pixmap );
    if (pixmap.size() != oldSize) {
        centerPreview();
    }
}

void KDReports::PreviewWidgetPrivate::pageNumberReturnPressed()
{
    bool ok;
    const int newPageNumber = pageNumber->text().toInt(&ok) - 1;
    if ( !ok || newPageNumber < 0 || newPageNumber > pageList->count() - 1 )
        return;
    pageList->setCurrentRow( newPageNumber );
}

void KDReports::PreviewWidgetPrivate::_kd_slotFirstPage()
{
    pageList->setCurrentRow( 0 );
}

void KDReports::PreviewWidgetPrivate::_kd_slotPrevPage()
{
    if ( !pageList->currentItem() || pageList->currentRow() == 0 )
        return;
    pageList->setCurrentRow( pageList->currentRow() - 1 );
}

void KDReports::PreviewWidgetPrivate::_kd_slotNextPage()
{
    if ( !pageList->currentItem() || pageList->currentRow() >= pageList->count() - 1 )
        return;
    pageList->setCurrentRow( pageList->currentRow() + 1 );
}

void KDReports::PreviewWidgetPrivate::_kd_slotLastPage()
{
    if ( pageList->count() == 0 ) // can't happen
        return;
    pageList->setCurrentRow( pageList->count() - 1 );
}

void KDReports::PreviewWidgetPrivate::_kd_slotPaperSizeActivated( int index )
{
    QPrinter::PageSize qPageSize( static_cast<QPrinter::PageSize>( paperSizeCombo->itemData(index).toInt() ) );
    m_printer.setPageSize( qPageSize );
    if ( qPageSize == QPrinter::Custom ) {
        m_report->setWidthForEndlessPrinter( m_endlessPrinterWidth );
    } else {
        m_report->setWidthForEndlessPrinter( 0 );
        m_report->setPageSize( qPageSize );
    }
    pageCountChanged();
    emit q->pageSizeChanged( qPageSize );
}

void KDReports::PreviewWidgetPrivate::_kd_slotPaperOrientationActivated( int index )
{
    m_printer.setOrientation( QPrinter::Orientation(paperOrientationCombo->itemData(index).toInt()) );
    m_report->setOrientation( m_printer.orientation() );
    pageCountChanged();
    emit q->orientationChanged( m_printer.orientation() );
}

void KDReports::PreviewWidgetPrivate::pageCountChanged()
{
    qApp->setOverrideCursor( Qt::WaitCursor ); // layouting could take a long time
    if ( m_printer.pageSize() == QPrinter::Custom ) {
        // Printing without page breaks -> only one page
        m_pageCount = 1;
    } else {
        m_pageCount = m_report->numberOfPages();
        //qDebug() << "Report has" << m_pageCount << "pages";
    }

    int numberWidth = 20 + pageNumber->fontMetrics().width( QString::number( m_pageCount ) );
    pageNumber->setMinimumWidth( numberWidth );
    pageNumber->setMaximumWidth( numberWidth );
    pageCount->setText( QString::fromLatin1( " / " ) + QString::number( m_pageCount ) );

    // Ensure that the treewidget has the right number of items
    int numberOfItems = pageList->count();
    while ( numberOfItems < m_pageCount ) {
        QListWidgetItem *item = new QListWidgetItem(pageList);
        item->setText( QString::number( numberOfItems + 1 ) );
        item->setCheckState( Qt::Checked );
        ++numberOfItems;
    }
    if ( pageList->currentRow() >= m_pageCount ) { // avoid crash
        //qDebug() << "Adjusting current row to" << m_pageCount - 1;
        pageList->setCurrentRow( m_pageCount - 1 );
    }
    while ( numberOfItems > m_pageCount ) {
        delete pageList->item( numberOfItems - 1 );
        --numberOfItems;
    }

    QPixmap whitePixmap( PreviewSize, PreviewSize );
    whitePixmap.fill( Qt::white );

    for (int index = 0; index < m_pageCount; ++index)
        pageList->item(index)->setIcon( whitePixmap );

    m_firstDirtyPreviewItem = 0;
// #define PAINT_ALL_PREVIEWS
#ifdef PAINT_ALL_PREVIEWS
    QTime dt;
    dt.start();
    while ( m_firstDirtyPreviewItem != -1 ) {
        _kd_previewNextItems();
    }
    // Debug mode:
    // 500 rows, Qt-4.5 or 4.6, no antialiasing, T500 laptop, -O0 or -O2: 42 previews in 8.2s to 8.8s
    qDebug() << m_pageCount << "previews in" << dt.elapsed() / 1000.0 << "s";
#else
    m_previewTimer.start(0);
#endif

    updatePageButtons();
    updatePreview();
    centerPreview();
    qApp->restoreOverrideCursor();
}

void KDReports::PreviewWidgetPrivate::centerPreview()
{
#if 0 // extreme flicker, due to QScrollArea doing a move(0,0) every time (issue N152085)
    // TT suggested as workaround to use QAbstractScrollArea instead, didn't try yet.
    const QPoint offset( ( previewArea->width() - m_previewWidget->width() ) / 2,
                         ( previewArea->height() - m_previewWidget->height() ) / 2 );
    qDebug() << "Moving to " << offset;
    m_previewWidget->move( offset );
#endif
    // So: make it big, instead. At least as big as the viewport.
    int width = qMax( m_previewWidget->pixmapWidth(), previewArea->viewport()->width() );
    int height = qMax( m_previewWidget->pixmapHeight(), previewArea->viewport()->height() );
    m_previewWidget->resize( width, height );
}

void KDReports::PreviewWidgetPrivate::_kd_slotZoomIn()
{
    if ( m_zoomFactor > 1.99 )
        m_zoomFactor = qMin<qreal>( 4.0, m_zoomFactor + 0.5 );
    else if ( m_zoomFactor > 0.99 )
        m_zoomFactor += 0.2;
    else
        m_zoomFactor += 0.1;
    fillZoomCombo();
    zoomChanged();
}

void KDReports::PreviewWidgetPrivate::_kd_slotZoomOut()
{
    if ( m_zoomFactor > 1.99 )
        m_zoomFactor -= 0.5;
    else if ( m_zoomFactor > 0.99 )
        m_zoomFactor -= 0.2;
    else if ( m_zoomFactor >= 0.2 )
        m_zoomFactor -= 0.1;
    fillZoomCombo();
    zoomChanged();
}

void KDReports::PreviewWidgetPrivate::_kd_slotZoomChanged()
{
    QString str = zoomCombo->currentText();
    m_zoomFactor = str.remove( str.indexOf( QChar::fromLatin1('%') ), 1 ).toDouble() / 100.0;
    zoomChanged();
}

void KDReports::PreviewWidgetPrivate::zoomChanged()
{
    //qDebug() << m_zoomFactor;
    updatePreview();
    centerPreview();
}

void KDReports::PreviewWidgetPrivate::fillZoomCombo()
{
    static const double s_zoomFactors[] = {
        0.125, 0.25, 0.333, 0.5, 0.667, 0.75, 1, 1.25, 1.50, 2, 4, 0 /*end*/ };

    zoomCombo->clear();
    bool currentZoomFound = false;
    for ( int i = 0; s_zoomFactors[i]; ++i ) {
        const qreal val = s_zoomFactors[i];
        if ( !currentZoomFound && m_zoomFactor <= val - 0.001 ) {
            zoomCombo->addItem( QString::number( m_zoomFactor * 100 ) + QChar::fromLatin1('%'), QVariant(val) );
            zoomCombo->setCurrentIndex( zoomCombo->count() - 1 );
            currentZoomFound = true;
        }
        zoomCombo->addItem( QString::number( val * 100 ) + QChar::fromLatin1('%'), QVariant(val) );
        if ( m_zoomFactor > val - 0.001 && m_zoomFactor < val + 0.001 ) {
            zoomCombo->setCurrentIndex( zoomCombo->count() - 1 );
            currentZoomFound = true;
        }
    }
}

////

KDReports::PreviewWidget::PreviewWidget( QWidget *parent)
    : QWidget( parent ),
      d( new PreviewWidgetPrivate( this ) )
{
    d->init();
}

KDReports::PreviewWidget::~PreviewWidget()
{
    delete d;
}

bool KDReports::PreviewWidget::isSelected( int index ) const
{
    if (index >= 0 && index < d->pageList->count())
        return (d->pageList->item(index)->checkState() == Qt::Checked);
    else
        return false;
}

void KDReports::PreviewWidget::resizeEvent( QResizeEvent* )
{
    d->centerPreview();
}

void KDReports::PreviewWidget::setPageSizeChangeAllowed( bool b )
{
    d->paperSizeCombo->setVisible( b );
    d->paperOrientationCombo->setVisible( b );
    d->labelPaperSize->setVisible( b );
    d->labelOrientation->setVisible( b );
}

void KDReports::PreviewWidget::setShowTableSettingsDialog( bool b )
{
    d->tableBreakingButton->setVisible( b );
}

bool KDReports::PreviewWidget::eventFilter(QObject* obj, QEvent* ev)
{
    // Watch for Return in the pageNumber lineedit.
    // We could just connect to returnPressed(), but the dialog's OK button would still trigger.
    if ( obj == d->pageNumber ) {
        if ( ev->type() == QEvent::KeyPress ) {
            QKeyEvent* keyev = static_cast<QKeyEvent *>( ev );
            if (keyev->key() == Qt::Key_Enter || keyev->key() == Qt::Key_Return) {
                d->pageNumberReturnPressed();
                keyev->accept();
                d->previewArea->setFocus();
                return true;
            }
        } else if ( ev->type() == QEvent::FocusIn ) {
            d->m_eatPageNumberClick = true;
            d->pageNumber->selectAll();
        } else if ( ev->type() == QEvent::MouseButtonPress && d->m_eatPageNumberClick ) {
            d->m_eatPageNumberClick = false;
            return true;
        }
    }

    return QWidget::eventFilter( obj, ev );
}

void KDReports::PreviewWidget::setWidthForEndlessPrinter( qreal widthMM )
{
    d->m_endlessPrinterWidth = widthMM;
}

KDReports::Report* KDReports::PreviewWidget::report() const
{
    return d->m_report;
}

bool KDReports::PreviewWidget::printWithDialog()
{
    QPointer<QPrintDialog> dialog = new QPrintDialog( &d->m_printer, this );
#if QT_VERSION >= 0x040500
    dialog->setOption( QAbstractPrintDialog::PrintSelection, true );
    dialog->setOption( QAbstractPrintDialog::PrintPageRange, true );
#endif
    bool ok = false;
    if ( dialog->exec() == QDialog::Accepted && dialog ) {
        if ( dialog->printRange() == QAbstractPrintDialog::AllPages ) {
            // Select all pages
            for ( int pageIndex = 0; pageIndex < d->pageList->count(); ++pageIndex ) {
                d->pageList->item(pageIndex)->setCheckState( Qt::Checked );
            }
        } else if ( dialog->printRange() == QAbstractPrintDialog::PageRange ) {
            const int fromPage = dialog->fromPage() - 1; // dialog is 1 based
            const int toPage = dialog->toPage() - 1;
            // Select only pages from <fromPage> to <toPage>
            for ( int pageIndex = 0; pageIndex < d->pageList->count(); ++pageIndex ) {
                const bool checked = ( pageIndex >= fromPage && pageIndex <= toPage );
                d->pageList->item(pageIndex)->setCheckState( checked ? Qt::Checked : Qt::Unchecked );
            }
        }
        d->printSelectedPages();
        ok = true;
    }
    delete dialog;
    return ok;
}

void KDReports::PreviewWidget::setReport( KDReports::Report* report )
{
    d->setReport( report );
}

void KDReports::PreviewWidgetPrivate::setReport( KDReports::Report* report )
{
    Q_ASSERT(report);
    m_report = report;
    actionBar->setEnabled(true);

    // initialize combos from report
    paperSizeCombo->setCurrentIndex( paperSizeCombo->findData(m_report->pageSize()) );
    paperOrientationCombo->setCurrentIndex( paperOrientationCombo->findData(m_report->orientation()) );

    tableBreakingButton->setVisible(m_report->reportMode() == KDReports::Report::SpreadSheet);

    m_report->setupPrinter(&m_printer);
    pageCountChanged();
    if (!pageList->currentItem()) {
        // No page selected yet - select the first one
        pageList->setCurrentItem(pageList->item(0));
    }
    _kd_slotCurrentPageChanged(); // update preview and buttons
    pageList->scrollToTop();
}

void KDReports::PreviewWidget::setShowPageListWidget( bool show )
{
    d->pageList->setVisible( show );
}

void KDReports::PreviewWidget::repaint()
{
    d->pageCountChanged();
    d->_kd_slotCurrentPageChanged(); // update preview and buttons
}

QSize KDReports::PreviewWidget::sizeHint() const
{
    return QSize( 800, 600 );
}

#include "moc_KDReportsPreviewWidget.cpp"

