/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSPREVIEWWIDGET_H
#define KDREPORTSPREVIEWWIDGET_H

#include "KDReportsGlobal.h"
#include <QPrinter>
#include <QWidget>
#include <memory>

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
    explicit PreviewWidget(QWidget *parent = nullptr);

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /// Emitted when the user changes the page size.
    void pageSizeChanged(QPrinter::PageSize pageSize);
    /// Emitted when the user changes the page orientation.
    void orientationChanged(QPrinter::Orientation orientation);
#else
    /// Emitted when the user changes the page size.
    void pageSizeChanged(const QPageSize &pageSize);
    /// Emitted when the user changes the page orientation.
    void orientationChanged(QPageLayout::Orientation orientation);
#endif

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
    friend class PreviewWidgetPrivate;
    std::unique_ptr<PreviewWidgetPrivate> d;
};

}

#endif
