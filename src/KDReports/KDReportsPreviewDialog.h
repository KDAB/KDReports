/****************************************************************************
** Copyright (C) 2007-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#ifndef KDREPORTSPREVIEWDIALOG_H
#define KDREPORTSPREVIEWDIALOG_H

#include "KDReportsGlobal.h"
#include <QDialog>
#include <QPrinter>

namespace KDReports {
class Report;
class PreviewWidget;

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
    explicit PreviewDialog( KDReports::Report* report, QWidget *parent = 0 );

    ~PreviewDialog();

    /**
     * Return true if the page has been selected (checked) by the user.
     */
    bool isSelected( int pageNumber ) const;

    /**
     * Set to false if you want to prevent the user from changing the page size
     * in the preview dialog.
     * Set to true if you want to allow the user to change the page size;
     * note that this leads to a call to Report::setPageSize().
     * Changing the page size is allowed by default.
     */
    void setPageSizeChangeAllowed( bool b );

    /**
     * Show or hide the button for configuring table settings and font scaling.
     * The button is shown by default.
     * \since 1.1
     */
    void setShowTableSettingsDialog( bool b );

    /**
     * Sets the width of the endless printer, in case the user selects that item
     * from the page selection combobox.
     */
    void setWidthForEndlessPrinter( qreal widthMM );

    // maybe setZoomFactor() to set the initial value?

    /**
     * Shows the table settings (and font scaling) dialog.
     * Called when the user clicks on the corresponding button.
     * This is a virtual method so that the dialog can be replaced
     * or subclassed.
     * \since 1.1
     */
    virtual bool showTableSettingsDialog( KDReports::Report* report );

    /**
     * Returns the preview widget used in this dialog.
     * Can be used for fine tuning, for instance setShowPageListWidget(false).
     * \since 1.4
     */
    KDReports::PreviewWidget* previewWidget();

Q_SIGNALS:
    /// Emitted when the user changes the page size.
    void pageSizeChanged( QPrinter::PageSize pageSize );
    /// Emitted when the user changes the page orientation.
    void orientationChanged( QPrinter::Orientation orientation );

private Q_SLOTS:
    /// \reimp
    void accept();
    /// \reimp
    void reject();

private:
    Q_PRIVATE_SLOT( d, void _kd_slotTableBreakingDialog() )
    class Private;
    friend class Private;
    Private* const d;
};

}

#endif
