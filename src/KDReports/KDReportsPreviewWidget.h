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

#ifndef KDREPORTSPREVIEWWIDGET_H
#define KDREPORTSPREVIEWWIDGET_H

#include "KDReportsGlobal.h"
#include <QWidget>
#include <QPrinter>

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
    explicit PreviewWidget( QWidget *parent = 0 );

    ~PreviewWidget();

    /**
     * Show a different report in the preview widget
     */
    void setReport( KDReports::Report* report );

    /**
     * Return true if the page has been selected (checked) by the user.
     */
    bool isSelected( int pageNumber ) const;

    /**
     * Set to false if you want to prevent the user from changing the page size
     * in the preview widget.
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
     * \return the report passed to the constructor or to setReport.
     */
    KDReports::Report* report() const;

    /**
     * Show the print dialog and print the selected pages
     * \return false if the dialog was cancelled
     */
    bool printWithDialog();

    /**
     * Show/hide the page list widget (with small previews) on the left.
     */
    void setShowPageListWidget( bool show );

    /**
     * Updates the preview. Call this after the report has changed.
     */
    void repaint();

    /**
     * \reimp
     */
    QSize sizeHint() const;

Q_SIGNALS:
    /// Emitted when the user changes the page size.
    void pageSizeChanged( QPrinter::PageSize pageSize );
    /// Emitted when the user changes the page orientation.
    void orientationChanged( QPrinter::Orientation orientation );
    /// Emitted when the table settings button has been clicked
    void tableSettingsClicked();

protected:
    /// \reimp
    void resizeEvent(QResizeEvent *);
    /// \reimp
    bool eventFilter(QObject*, QEvent*);

private:
    Q_PRIVATE_SLOT( d, void _kd_slotCurrentPageChanged() )
    Q_PRIVATE_SLOT( d, void _kd_slotFirstPage() )
    Q_PRIVATE_SLOT( d, void _kd_slotPrevPage() )
    Q_PRIVATE_SLOT( d, void _kd_slotNextPage() )
    Q_PRIVATE_SLOT( d, void _kd_slotLastPage() )
    Q_PRIVATE_SLOT( d, void _kd_slotPaperSizeActivated( int ) )
    Q_PRIVATE_SLOT( d, void _kd_slotPaperOrientationActivated( int ) )
    Q_PRIVATE_SLOT( d, void _kd_slotZoomIn() )
    Q_PRIVATE_SLOT( d, void _kd_slotZoomOut() )
    Q_PRIVATE_SLOT( d, void _kd_slotZoomChanged() )
    Q_PRIVATE_SLOT( d, void _kd_previewNextItems() )

    friend class PreviewWidgetPrivate;
    PreviewWidgetPrivate* const d;
};

}

#endif
