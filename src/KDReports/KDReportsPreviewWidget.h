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

#ifndef KDREPORTSPREVIEWWIDGET_H
#define KDREPORTSPREVIEWWIDGET_H

#include "KDReportsGlobal.h"
#include <QPrinter>
#include <QWidget>

namespace KDReports {
class Report;
class PreviewWidgetPrivate;

/**
 * The PreviewWidget class provides a widget showing the report to the user,
 * typically used for "print preview" functionality.
 *
 * \since 1.2
 */
class KDREPORTS_EXPORT PreviewWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a preview widget.
     */
    explicit PreviewWidget(QWidget *parent = 0);

    ~PreviewWidget() override;

    /**
     * Show a different report in the preview widget
     */
    void setReport(KDReports::Report *report);

    /**
     * Return true if the page has been selected (checked) by the user.
     */
    bool isSelected(int pageNumber) const;

    /**
     * Set to false if you want to prevent the user from changing the page size
     * in the preview widget.
     * Set to true if you want to allow the user to change the page size;
     * note that this leads to a call to Report::setPageSize().
     * Changing the page size is allowed by default.
     */
    void setPageSizeChangeAllowed(bool b);

    /**
     * Show or hide the button for configuring table settings and font scaling.
     * The button is shown by default.
     * \since 1.1
     */
    void setShowTableSettingsDialog(bool b);

    /**
     * Sets the width of the endless printer, in case the user selects that item
     * from the page selection combobox.
     */
    void setWidthForEndlessPrinter(qreal widthMM);

    // maybe setZoomFactor() to set the initial value?

    /**
     * \return the report passed to the constructor or to setReport.
     */
    KDReports::Report *report() const;

    /**
     * Show the print dialog and print the selected pages
     * \return false if the dialog was cancelled
     */
    bool printWithDialog();

    /**
     * Show/hide the page list widget (with small previews) on the left.
     */
    void setShowPageListWidget(bool show);

    /**
     * Updates the preview. Call this after the report has changed.
     */
    void repaint();

    /**
     * \reimp
     */
    QSize sizeHint() const override;

Q_SIGNALS:
    /// Emitted when the user changes the page size.
    void pageSizeChanged(QPrinter::PageSize pageSize);
    /// Emitted when the user changes the page orientation (deprecated, use pageOrientationChanged)
    void orientationChanged(QPrinter::Orientation orientation);
    /// Emitted when the user changes the page orientation.
    void pageOrientationChanged(QPageLayout::Orientation orientation);
    /// Emitted when the table settings button has been clicked
    void tableSettingsClicked();
    /**
     * Emitted when the user clicked on a hyperlink
     * \since 2.0
     */
    void linkActivated(const QUrl &url);

protected:
    /// \reimp
    void resizeEvent(QResizeEvent *) override;
    /// \reimp
    bool eventFilter(QObject *, QEvent *) override;

private:
    Q_PRIVATE_SLOT(d, void _kd_slotCurrentPageChanged())
    Q_PRIVATE_SLOT(d, void _kd_slotFirstPage())
    Q_PRIVATE_SLOT(d, void _kd_slotPrevPage())
    Q_PRIVATE_SLOT(d, void _kd_slotNextPage())
    Q_PRIVATE_SLOT(d, void _kd_slotLastPage())
    Q_PRIVATE_SLOT(d, void _kd_slotPaperSizeActivated(int))
    Q_PRIVATE_SLOT(d, void _kd_slotPaperOrientationActivated(int))
    Q_PRIVATE_SLOT(d, void _kd_slotZoomIn())
    Q_PRIVATE_SLOT(d, void _kd_slotZoomOut())
    Q_PRIVATE_SLOT(d, void _kd_slotZoomChanged())
    Q_PRIVATE_SLOT(d, void _kd_previewNextItems())

    friend class PreviewWidgetPrivate;
    PreviewWidgetPrivate *const d;
};

}

#endif
