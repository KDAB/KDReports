/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#include "KDReportsPreviewWidget.h"
#include "KDReportsReport.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QPointer>
#include <QPrintDialog>
#include <QProgressDialog>
#include <QShortcut>
#include <QTime>
#include <QTimer>
#include <qmath.h> // qCeil

#include "ui_previewdialogbase.h"

enum { PreviewSize = 200 };

/// @internal
/// The widget showing the large preview on the right
class PagePreviewWidget : public QWidget
{
    Q_OBJECT
public:
    PagePreviewWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        // For link hovered functionality
        setMouseTracking(true);
    }
    void setPixmap(const QPixmap &pixmap)
    {
        m_pixmap = pixmap;
        update();
    }
    int pixmapWidth() const { return m_pixmap.width(); }
    int pixmapHeight() const { return m_pixmap.height(); }
    QSize pixmapSize() const { return m_pixmap.size(); }

Q_SIGNALS:
    void mouseMoved(QPoint pos);
    void mouseClicked(QPoint pos);

protected:
    QPoint pixmapOffset() const
    {
        return QPoint((width() - m_pixmap.width()) / 2, (height() - m_pixmap.height()) / 2);
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        // painter.fillRect( event->rect(), QColor(224,224,224) );
        painter.drawPixmap(pixmapOffset(), m_pixmap);
    }
    /// \reimp
    void mouseMoveEvent(QMouseEvent *ev) override
    {
        Q_EMIT mouseMoved(ev->pos() - pixmapOffset());
    }
    /// \reimp
    void mouseReleaseEvent(QMouseEvent *ev) override
    {
        Q_EMIT mouseClicked(ev->pos() - pixmapOffset());
    }

private:
    QPixmap m_pixmap;
};

class KDReports::PreviewWidgetPrivate : public Ui::PreviewWidgetBase
{
public:
    PreviewWidgetPrivate(KDReports::PreviewWidget *w);

    // not in ctor because the init calls q->slotFoo which uses d->, so d must be set first.
    void init();

    void paintItem(QListWidgetItem *item, int index) const;
    QPixmap paintPreview(int index) const;
    void printSelectedPages();
    void setupComboBoxes();
    void pageCountChanged();
    void centerPreview();
    void zoomChanged();
    void fillZoomCombo();
    void updatePageButtons();
    void updatePreview();
    void pageNumberReturnPressed();
    void setReport(KDReports::Report *report);

    void handleMouseMove(QPoint pos);
    void handleMouseRelease(QPoint pos);
    void slotCurrentPageChanged();
    void slotFirstPage();
    void slotPrevPage();
    void slotNextPage();
    void slotLastPage();
    void slotPaperSizeActivated(int);
    void slotPaperOrientationActivated(int);
    void slotZoomIn();
    void slotZoomOut();
    void slotZoomChanged();
    void previewNextItems();

    PagePreviewWidget *m_previewWidget;
    QPrinter m_printer;
    qreal m_zoomFactor = 1.0;
    qreal m_endlessPrinterWidth = 114.0;
    KDReports::Report *m_report = nullptr;
    QTimer m_previewTimer;
    KDReports::PreviewWidget *q;
    int m_pageCount = 0;
    int m_firstDirtyPreviewItem = -1;
    bool m_eatPageNumberClick = false;
    bool m_onAnchor = false;
};

KDReports::PreviewWidgetPrivate::PreviewWidgetPrivate(KDReports::PreviewWidget *w)
    : m_previewWidget(new PagePreviewWidget)
    , q(w)
{
    QObject::connect(&m_previewTimer, &QTimer::timeout, q, [this]() { previewNextItems(); });
    QObject::connect(m_previewWidget, &PagePreviewWidget::mouseMoved, q, [this](QPoint pos) {
        handleMouseMove(pos);
    });
    QObject::connect(m_previewWidget, &PagePreviewWidget::mouseClicked, q, [this](QPoint pos) {
        handleMouseRelease(pos);
    });
}

void KDReports::PreviewWidgetPrivate::init()
{
    setupUi(q);
    actionBar->setEnabled(false);

    m_printer.setFullPage(true);

    pageList->setIconSize(QSize(PreviewSize, PreviewSize));
    pageList->setViewMode(QListView::IconMode);
    pageList->setMovement(QListView::Static);
    previewArea->setWidget(m_previewWidget);
    setupComboBoxes();
    previewArea->setFocus();
    fillZoomCombo();

    // m_tableBreakingButton = buttonBox->addButton( tr("Table Breaking / Font Scaling..." ), QDialogButtonBox::ActionRole );
    QObject::connect(tableBreakingButton, &QAbstractButton::clicked, q, &PreviewWidget::tableSettingsClicked);

    QObject::connect(firstPage, &QAbstractButton::clicked, q, [this]() { slotFirstPage(); });
    QObject::connect(prevPage, &QAbstractButton::clicked, q, [this]() { slotPrevPage(); });
    QObject::connect(nextPage, &QAbstractButton::clicked, q, [this]() { slotNextPage(); });
    QObject::connect(lastPage, &QAbstractButton::clicked, q, [this]() { slotLastPage(); });
    QObject::connect(zoomIn, &QAbstractButton::clicked, q, [this]() { slotZoomIn(); });
    QObject::connect(zoomOut, &QAbstractButton::clicked, q, [this]() { slotZoomOut(); });
    QObject::connect(zoomCombo, QOverload<int>::of(&QComboBox::activated), q, [this]() { slotZoomChanged(); });
    QObject::connect(pageList, &QListWidget::currentRowChanged, q, [this]() { slotCurrentPageChanged(); });
    QObject::connect(paperSizeCombo, QOverload<int>::of(&QComboBox::activated), q, [this](int idx) { slotPaperSizeActivated(idx); });
    QObject::connect(paperOrientationCombo, QOverload<int>::of(&QComboBox::activated), q, [this](int idx) { slotPaperOrientationActivated(idx); });

    auto *nextPageShortcut = new QShortcut(q);
    nextPageShortcut->setKey(Qt::CTRL | Qt::Key_PageDown);
    QObject::connect(nextPageShortcut, &QShortcut::activated, q, [this]() { slotNextPage(); });
    auto *prevPageShortcut = new QShortcut(q);
    prevPageShortcut->setKey(Qt::CTRL | Qt::Key_PageUp);
    QObject::connect(prevPageShortcut, &QShortcut::activated, q, [this]() { slotPrevPage(); });
    pageNumber->setValidator(new QIntValidator(1, 100000, pageNumber));
    pageNumber->installEventFilter(q);
}

void KDReports::PreviewWidgetPrivate::paintItem(QListWidgetItem *item, int index) const
{
    // qDebug() << "paintItem" << index;

    // Use a QImage so that the raster paint engine is used.
    // Gives a 7.7 times speedup (!) compared to X11.
    QImage img(PreviewSize, PreviewSize, QImage::Format_ARGB32_Premultiplied);
    const QSizeF paperSize = m_report->paperSize();
    const qreal longestSide = qMax(paperSize.width(), paperSize.height());
    qreal width = img.width() * paperSize.width() / longestSide;
    qreal height = img.height() * paperSize.height() / longestSide;
    // img.fill( QColor( Qt::white ).rgb() );
    img.fill(qRgba(0, 0, 0, 0)); // transparent
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate((img.width() - width) / 2, (img.height() - height) / 2);
    painter.fillRect(QRectF(0, 0, width, height), QBrush(Qt::white));
    painter.scale(img.width() / longestSide, img.height() / longestSide);
    m_report->paintPage(index, painter);
    painter.setPen(QPen(1));
    painter.drawRect(QRectF(0, 0, paperSize.width(), paperSize.height()));

    item->setIcon(QIcon(QPixmap::fromImage(img)));
}

void KDReports::PreviewWidgetPrivate::previewNextItems()
{
    if (m_firstDirtyPreviewItem == -1 || m_firstDirtyPreviewItem >= m_pageCount) {
        m_previewTimer.stop();
        m_firstDirtyPreviewItem = -1;
    } else {
        // Do next preview
        paintItem(pageList->item(m_firstDirtyPreviewItem), m_firstDirtyPreviewItem);
        ++m_firstDirtyPreviewItem;
    }
}

QPixmap KDReports::PreviewWidgetPrivate::paintPreview(int index) const
{
    const QSizeF paperSize = m_report->paperSize();
    const int width = qCeil(paperSize.width() * m_zoomFactor);
    const int height = qCeil(paperSize.height() * m_zoomFactor);

    QPixmap pixmap(width, height);
    // qDebug() << width << "," << height;

    // qDebug() << "paintPreview " << index;

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(QRectF(0, 0, width, height), QBrush(Qt::white));
    painter.scale(m_zoomFactor, m_zoomFactor);
    m_report->paintPage(index, painter);
    painter.setPen(QPen(1));
    painter.drawRect(QRectF(0, 0, paperSize.width(), paperSize.height()));

    return pixmap;
}

void KDReports::PreviewWidgetPrivate::handleMouseMove(QPoint pos)
{
    const QPoint unscaledPos = pos / m_zoomFactor;
    const QString link = m_report->anchorAt(pageList->currentRow(), unscaledPos);
    if (link.isEmpty()) { // restore cursor
        q->unsetCursor();
        m_onAnchor = false;
    } else if (!m_onAnchor) {
        q->setCursor(Qt::PointingHandCursor);
        m_onAnchor = true;
    }
}

void KDReports::PreviewWidgetPrivate::handleMouseRelease(QPoint pos)
{
    const QPoint unscaledPos = pos / m_zoomFactor;
    const QString link = m_report->anchorAt(pageList->currentRow(), unscaledPos);
    if (!link.isEmpty()) {
        Q_EMIT q->linkActivated(link);
    }
}

void KDReports::PreviewWidgetPrivate::printSelectedPages()
{
    // Well, the user can modify the page size in the printer dialog too - ensure layout matches
    // qDebug() << "pageSize: " << m_printer.pageSize();
    m_report->setPageSize(m_printer.pageLayout().pageSize());
    m_report->setPageOrientation(m_printer.pageLayout().orientation());
    pageCountChanged();

    // ### But how do we match "marked pages" from a previous layout into the new layout?
    // ### Hardly makes sense...

    int markedPages = 0;
    for (int pageIndex = 0; pageIndex < pageList->count(); ++pageIndex) {
        if (pageList->item(pageIndex)->checkState() == Qt::Checked)
            ++markedPages;
    }

    QProgressDialog dialog(QObject::tr("Printing"), QObject::tr("Cancel"), 0, markedPages, q);
    dialog.setWindowModality(Qt::ApplicationModal);

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

            m_report->paintPage(pageIndex, painter);
            dialog.setValue(++printed);
            firstPage = false;
        }
    }
    painter.end();
}

void KDReports::PreviewWidgetPrivate::setupComboBoxes()
{
    paperSizeCombo->addItem(PreviewWidget::tr("A0 (841 x 1189 mm)"), QPageSize::A0);
    paperSizeCombo->addItem(PreviewWidget::tr("A1 (594 x 841 mm)"), QPageSize::A1);
    paperSizeCombo->addItem(PreviewWidget::tr("A2 (420 x 594 mm)"), QPageSize::A2);
    paperSizeCombo->addItem(PreviewWidget::tr("A3 (297 x 420 mm)"), QPageSize::A3);
    paperSizeCombo->addItem(PreviewWidget::tr("A4 (210 x 297 mm)"), QPageSize::A4);
    paperSizeCombo->addItem(PreviewWidget::tr("A5 (148 x 210 mm)"), QPageSize::A5);
    paperSizeCombo->addItem(PreviewWidget::tr("A6 (105 x 148 mm)"), QPageSize::A6);
    paperSizeCombo->addItem(PreviewWidget::tr("A7 (74 x 105 mm)"), QPageSize::A7);
    paperSizeCombo->addItem(PreviewWidget::tr("A8 (52 x 74 mm)"), QPageSize::A8);
    // paperSizeCombo->addItem(PreviewWidget::tr("A9 (37 x 52 mm)"), QPageSize::A9);
    paperSizeCombo->addItem(PreviewWidget::tr("B0 (1000 x 1414 mm)"), QPageSize::B0);
    paperSizeCombo->addItem(PreviewWidget::tr("B1 (707 x 1000 mm)"), QPageSize::B1);
    paperSizeCombo->addItem(PreviewWidget::tr("B2 (500 x 707 mm)"), QPageSize::B2);
    paperSizeCombo->addItem(PreviewWidget::tr("B3 (353 x 500 mm)"), QPageSize::B3);
    paperSizeCombo->addItem(PreviewWidget::tr("B4 (250 x 353 mm)"), QPageSize::B4);
    paperSizeCombo->addItem(PreviewWidget::tr("B5 (176 x 250 mm)"), QPageSize::B5);
    paperSizeCombo->addItem(PreviewWidget::tr("B6 (125 x 176 mm)"), QPageSize::B6);
    paperSizeCombo->addItem(PreviewWidget::tr("B7 (88 x 125 mm)"), QPageSize::B7);
    paperSizeCombo->addItem(PreviewWidget::tr("B8 (62 x 88 mm)"), QPageSize::B8);
    // paperSizeCombo->addItem(PreviewWidget::tr("B9 (44 x 62 mm)"), QPageSize::B9);
    // paperSizeCombo->addItem(PreviewWidget::tr("B10 (31 x 44 mm)"), QPageSize::B10);
    paperSizeCombo->addItem(PreviewWidget::tr("C5E (163 x 229 mm)"), QPageSize::C5E);
    paperSizeCombo->addItem(PreviewWidget::tr("DLE (110 x 220 mm)"), QPageSize::DLE);
    paperSizeCombo->addItem(PreviewWidget::tr("Executive (7.5 x 10 inches)"), QPageSize::Executive);
    paperSizeCombo->addItem(PreviewWidget::tr("Folio (210 x 330 mm)"), QPageSize::Folio);
    paperSizeCombo->addItem(PreviewWidget::tr("Ledger (432 x 279 mm)"), QPageSize::Ledger);
    paperSizeCombo->addItem(PreviewWidget::tr("Legal (8.5 x 14 inches)"), QPageSize::Legal);
    paperSizeCombo->addItem(PreviewWidget::tr("Letter (8.5 x 11 inches)"), QPageSize::Letter);
    paperSizeCombo->addItem(PreviewWidget::tr("Tabloid (279 x 432 mm)"), QPageSize::Tabloid);
    paperSizeCombo->addItem(PreviewWidget::tr("US #10 Envelope (105 x 241 mm)"), QPageSize::Comm10E);
    paperSizeCombo->addItem(PreviewWidget::tr("Endless printer (%1 mm wide)").arg(m_endlessPrinterWidth), QPageSize::Custom);

    paperOrientationCombo->addItem(PreviewWidget::tr("Portrait"), QPageLayout::Portrait);
    paperOrientationCombo->addItem(PreviewWidget::tr("Landscape"), QPageLayout::Landscape);
}

void KDReports::PreviewWidgetPrivate::slotCurrentPageChanged()
{
    updatePreview();
    updatePageButtons();
}

void KDReports::PreviewWidgetPrivate::updatePageButtons()
{
    prevPage->setEnabled(pageList->currentRow() > 0);
    nextPage->setEnabled(pageList->currentRow() < pageList->count() - 1);
    pageNumber->setText(QString::number(pageList->currentRow() + 1));
}

void KDReports::PreviewWidgetPrivate::updatePreview()
{
    if (!pageList->currentItem())
        return;
    const QSize oldSize = m_previewWidget->pixmapSize();
    const QPixmap pixmap = paintPreview(pageList->currentRow());
    m_previewWidget->setPixmap(pixmap);
    if (pixmap.size() != oldSize) {
        centerPreview();
    }
}

void KDReports::PreviewWidgetPrivate::pageNumberReturnPressed()
{
    bool ok;
    const int newPageNumber = pageNumber->text().toInt(&ok) - 1;
    if (!ok || newPageNumber < 0 || newPageNumber > pageList->count() - 1)
        return;
    pageList->setCurrentRow(newPageNumber);
}

void KDReports::PreviewWidgetPrivate::slotFirstPage()
{
    pageList->setCurrentRow(0);
}

void KDReports::PreviewWidgetPrivate::slotPrevPage()
{
    if (!pageList->currentItem() || pageList->currentRow() == 0)
        return;
    pageList->setCurrentRow(pageList->currentRow() - 1);
}

void KDReports::PreviewWidgetPrivate::slotNextPage()
{
    if (!pageList->currentItem() || pageList->currentRow() >= pageList->count() - 1)
        return;
    pageList->setCurrentRow(pageList->currentRow() + 1);
}

void KDReports::PreviewWidgetPrivate::slotLastPage()
{
    if (pageList->count() == 0) // can't happen
        return;
    pageList->setCurrentRow(pageList->count() - 1);
}

void KDReports::PreviewWidgetPrivate::slotPaperSizeActivated(int index)
{
    const QPageSize qPageSize(static_cast<QPageSize::PageSizeId>(paperSizeCombo->itemData(index).toInt()));
    m_printer.setPageSize(qPageSize);
    if (qPageSize.id() == QPageSize::Custom) {
        m_report->setWidthForEndlessPrinter(m_endlessPrinterWidth);
    } else {
        m_report->setWidthForEndlessPrinter(0);
        m_report->setPageSize(qPageSize);
    }
    pageCountChanged();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    emit q->pageSizeChanged(static_cast<QPrinter::PageSize>(qPageSize.id()));
#else
    emit q->pageSizeChanged(qPageSize);
#endif
}

void KDReports::PreviewWidgetPrivate::slotPaperOrientationActivated(int index)
{
    const QPageLayout::Orientation orientation = static_cast<QPageLayout::Orientation>(paperOrientationCombo->itemData(index).toInt());
    m_printer.setPageOrientation(orientation);
    m_report->setPageOrientation(orientation);
    pageCountChanged();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    emit q->orientationChanged(static_cast<QPrinter::Orientation>(orientation));
#else
    emit q->orientationChanged(orientation);
#endif
}

void KDReports::PreviewWidgetPrivate::pageCountChanged()
{
    qApp->setOverrideCursor(Qt::WaitCursor); // layouting could take a long time
    if (m_printer.pageLayout().pageSize().id() == QPageSize::Custom) {
        // Printing without page breaks -> only one page
        m_pageCount = 1;
    } else {
        m_pageCount = m_report->numberOfPages();
        // qDebug() << "Report has" << m_pageCount << "pages";
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    int numberWidth = 20 + pageNumber->fontMetrics().width(QString::number(m_pageCount));
#else
    int numberWidth = 20 + pageNumber->fontMetrics().horizontalAdvance(QString::number(m_pageCount));
#endif
    pageNumber->setMinimumWidth(numberWidth);
    pageNumber->setMaximumWidth(numberWidth);
    pageCount->setText(QStringLiteral(" / ") + QString::number(m_pageCount));

    // Ensure that the treewidget has the right number of items
    int numberOfItems = pageList->count();
    while (numberOfItems < m_pageCount) {
        auto *item = new QListWidgetItem(pageList);
        item->setText(QString::number(numberOfItems + 1));
        item->setCheckState(Qt::Checked);
        ++numberOfItems;
    }
    if (pageList->currentRow() >= m_pageCount) { // avoid crash
        // qDebug() << "Adjusting current row to" << m_pageCount - 1;
        pageList->setCurrentRow(m_pageCount - 1);
    }
    while (numberOfItems > m_pageCount) {
        delete pageList->item(numberOfItems - 1);
        --numberOfItems;
    }

    QPixmap whitePixmap(PreviewSize, PreviewSize);
    whitePixmap.fill(Qt::white);

    for (int index = 0; index < m_pageCount; ++index)
        pageList->item(index)->setIcon(whitePixmap);

    m_firstDirtyPreviewItem = 0;
// #define PAINT_ALL_PREVIEWS
#ifdef PAINT_ALL_PREVIEWS
    QTime dt;
    dt.start();
    while (m_firstDirtyPreviewItem != -1) {
        previewNextItems();
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
    int width = qMax(m_previewWidget->pixmapWidth(), previewArea->viewport()->width());
    int height = qMax(m_previewWidget->pixmapHeight(), previewArea->viewport()->height());
    m_previewWidget->resize(width, height);
}

void KDReports::PreviewWidgetPrivate::slotZoomIn()
{
    if (m_zoomFactor > 1.99)
        m_zoomFactor = qMin<qreal>(4.0, m_zoomFactor + 0.5);
    else if (m_zoomFactor > 0.99)
        m_zoomFactor += 0.2;
    else
        m_zoomFactor += 0.1;
    fillZoomCombo();
    zoomChanged();
}

void KDReports::PreviewWidgetPrivate::slotZoomOut()
{
    if (m_zoomFactor > 1.99)
        m_zoomFactor -= 0.5;
    else if (m_zoomFactor > 0.99)
        m_zoomFactor -= 0.2;
    else if (m_zoomFactor >= 0.2)
        m_zoomFactor -= 0.1;
    fillZoomCombo();
    zoomChanged();
}

void KDReports::PreviewWidgetPrivate::slotZoomChanged()
{
    QString str = zoomCombo->currentText();
    m_zoomFactor = str.remove(str.indexOf(QChar::fromLatin1('%')), 1).toDouble() / 100.0;
    zoomChanged();
}

void KDReports::PreviewWidgetPrivate::zoomChanged()
{
    // qDebug() << m_zoomFactor;
    updatePreview();
    centerPreview();
}

void KDReports::PreviewWidgetPrivate::fillZoomCombo()
{
    static const double s_zoomFactors[] = {0.125, 0.25, 0.333, 0.5, 0.667, 0.75, 1, 1.25, 1.50, 2, 4};

    zoomCombo->clear();
    bool currentZoomFound = false;
    for (const qreal val : s_zoomFactors) {
        if (!currentZoomFound && m_zoomFactor <= val - 0.001) {
            zoomCombo->addItem(QString::number(m_zoomFactor * 100) + QChar::fromLatin1('%'), QVariant(val));
            zoomCombo->setCurrentIndex(zoomCombo->count() - 1);
            currentZoomFound = true;
        }
        zoomCombo->addItem(QString::number(val * 100) + QChar::fromLatin1('%'), QVariant(val));
        if (m_zoomFactor > val - 0.001 && m_zoomFactor < val + 0.001) {
            zoomCombo->setCurrentIndex(zoomCombo->count() - 1);
            currentZoomFound = true;
        }
    }
}

////

KDReports::PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent)
    , d(new PreviewWidgetPrivate(this))
{
    d->init();
}

KDReports::PreviewWidget::~PreviewWidget()
{
    delete d;
}

bool KDReports::PreviewWidget::isSelected(int pageNumber) const
{
    if (pageNumber >= 0 && pageNumber < d->pageList->count())
        return (d->pageList->item(pageNumber)->checkState() == Qt::Checked);
    else
        return false;
}

void KDReports::PreviewWidget::resizeEvent(QResizeEvent *)
{
    d->centerPreview();
}

void KDReports::PreviewWidget::setPageSizeChangeAllowed(bool b)
{
    d->paperSizeCombo->setVisible(b);
    d->paperOrientationCombo->setVisible(b);
    d->labelPaperSize->setVisible(b);
    d->labelOrientation->setVisible(b);
}

void KDReports::PreviewWidget::setShowTableSettingsDialog(bool b)
{
    d->tableBreakingButton->setVisible(b);
}

bool KDReports::PreviewWidget::eventFilter(QObject *obj, QEvent *ev)
{
    // Watch for Return in the pageNumber lineedit.
    // We could just connect to returnPressed(), but the dialog's OK button would still trigger.
    if (obj == d->pageNumber) {
        if (ev->type() == QEvent::KeyPress) {
            auto *keyev = static_cast<QKeyEvent *>(ev);
            if (keyev->key() == Qt::Key_Enter || keyev->key() == Qt::Key_Return) {
                d->pageNumberReturnPressed();
                keyev->accept();
                d->previewArea->setFocus();
                return true;
            }
        } else if (ev->type() == QEvent::FocusIn) {
            d->m_eatPageNumberClick = true;
            d->pageNumber->selectAll();
        } else if (ev->type() == QEvent::MouseButtonPress && d->m_eatPageNumberClick) {
            d->m_eatPageNumberClick = false;
            return true;
        }
    }

    return QWidget::eventFilter(obj, ev);
}

void KDReports::PreviewWidget::setWidthForEndlessPrinter(qreal widthMM)
{
    d->m_endlessPrinterWidth = widthMM;
}

KDReports::Report *KDReports::PreviewWidget::report() const
{
    return d->m_report;
}

bool KDReports::PreviewWidget::printWithDialog()
{
    QPointer<QPrintDialog> dialog = new QPrintDialog(&d->m_printer, this);
    dialog->setOption(QAbstractPrintDialog::PrintSelection, true);
    dialog->setOption(QAbstractPrintDialog::PrintPageRange, true);
    bool ok = false;
    /* cppcheck-suppress nullPointerRedundantCheck */
    if (dialog->exec() == QDialog::Accepted && dialog) {
        if (dialog->printRange() == QAbstractPrintDialog::AllPages) {
            // Select all pages
            for (int pageIndex = 0; pageIndex < d->pageList->count(); ++pageIndex) {
                d->pageList->item(pageIndex)->setCheckState(Qt::Checked);
            }
        } else if (dialog->printRange() == QAbstractPrintDialog::PageRange) {
            const int fromPage = dialog->fromPage() - 1; // dialog is 1 based
            const int toPage = dialog->toPage() - 1;
            // Select only pages from <fromPage> to <toPage>
            for (int pageIndex = 0; pageIndex < d->pageList->count(); ++pageIndex) {
                const bool checked = (pageIndex >= fromPage && pageIndex <= toPage);
                d->pageList->item(pageIndex)->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
            }
        }
        d->printSelectedPages();
        ok = true;
    }
    delete dialog;
    return ok;
}

void KDReports::PreviewWidget::setReport(KDReports::Report *report)
{
    d->setReport(report);
}

void KDReports::PreviewWidgetPrivate::setReport(KDReports::Report *report)
{
    Q_ASSERT(report);
    m_report = report;
    actionBar->setEnabled(true);

    // initialize combos from report
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    paperSizeCombo->setCurrentIndex(paperSizeCombo->findData(m_report->pageSize()));
#else
    paperSizeCombo->setCurrentIndex(paperSizeCombo->findData(m_report->pageSize().id()));
#endif
    paperOrientationCombo->setCurrentIndex(paperOrientationCombo->findData(m_report->pageOrientation()));

    tableBreakingButton->setVisible(m_report->reportMode() == KDReports::Report::SpreadSheet);

    m_report->setupPrinter(&m_printer);
    pageCountChanged();
    if (!pageList->currentItem()) {
        // No page selected yet - select the first one
        pageList->setCurrentItem(pageList->item(0));
    }
    slotCurrentPageChanged(); // update preview and buttons
    pageList->scrollToTop();
}

void KDReports::PreviewWidget::setShowPageListWidget(bool show)
{
    d->pageList->setVisible(show);
}

void KDReports::PreviewWidget::repaint()
{
    d->pageCountChanged();
    d->slotCurrentPageChanged(); // update preview and buttons
}

QSize KDReports::PreviewWidget::sizeHint() const
{
    return QSize(800, 600);
}

#include "KDReportsPreviewWidget.moc"
