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

#ifndef KDREPORTSPREVIEWDIALOG_H
#define KDREPORTSPREVIEWDIALOG_H

#include "KDReportsGlobal.h"
#include <QDialog>
#include <QPrinter>

namespace KDReports {
class Report;
class PreviewWidget;
class PreviewDialogPrivate;

/**
 * The PreviewDialog class provides a dialog showing the report to the user,
 * typically used for "print preview" functionality.
 */
class KDREPORTS_EXPORT PreviewDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructs a preview dialog.
     */
    explicit PreviewDialog(KDReports::Report *report, QWidget *parent = 0);

    ~PreviewDialog() override;

    /**
     * Shows a [Print on <printer>] button, for quick printing without the print dialog
     * \param printerName the name of the printer for the quick print button to use.
     * Setting an empty printer name has no effect.
     *
     * \since 1.7
     */
    void setQuickPrinterName(const QString &printerName);

    /**
     * Sets the initial directory for the save dialog.
     * \param path the initial directory
     *
     * \since 1.7
     */
    void setDefaultSaveDirectory(const QString &path);

    /**
     * Allows or forbids the user from choosing the save directory.
     * If allowed (the default) the Save button shows a file dialog.
     * If forbidden, the Save button shows a text input field for choosing only the filename,
     *  and the directory will be the one set by setDefaultSaveDirectory().
     *
     * \since 1.7
     */
    void setDirectoryBrowsingEnabled(bool allowed);

    /**
     * Return true if the page has been selected (checked) by the user.
     */
    bool isSelected(int pageNumber) const;

    /**
     * Set to false if you want to prevent the user from changing the page size
     * in the preview dialog.
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
     * Shows the table settings (and font scaling) dialog.
     * Called when the user clicks on the corresponding button.
     * This is a virtual method so that the dialog can be replaced
     * or subclassed.
     * \since 1.1
     */
    virtual bool showTableSettingsDialog(KDReports::Report *report);

    /**
     * Returns the preview widget used in this dialog.
     * Can be used for fine tuning, for instance setShowPageListWidget(false).
     * \since 1.4
     */
    KDReports::PreviewWidget *previewWidget();

    /**
     * The Result enum describes result code accessible with QDialog::result()
     */
    enum Result { Printed = 10, SavedSuccessfully, SaveError };

    /**
     * The location where the report was saved, if the user saved it
     * Only set after exec returns.
     */
    QString savedFileName() const;

Q_SIGNALS:
    /// Emitted when the user changes the page size.
    void pageSizeChanged(QPrinter::PageSize pageSize);
    /// Emitted when the user changes the page orientation.
    void orientationChanged(QPrinter::Orientation orientation);
    /**
     * Emitted when the user clicked on a hyperlink
     * \since 2.0
     */
    void linkActivated(const QUrl &url);

public Q_SLOTS:
    /// \reimp
    void accept() override;
    /// \reimp
    void reject() override;

private:
    Q_PRIVATE_SLOT(d, void _kd_slotTableBreakingDialog())
    Q_PRIVATE_SLOT(d, void _kd_slotPrintWithDialog())
    Q_PRIVATE_SLOT(d, void _kd_slotQuickPrint())
    Q_PRIVATE_SLOT(d, void _kd_slotSave())
    PreviewDialogPrivate *const d;
};

}

#endif
