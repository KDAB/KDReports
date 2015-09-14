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

#ifndef KDREPORTSTABLEBREAKINGSETTINGSDIALOG_H
#define KDREPORTSTABLEBREAKINGSETTINGSDIALOG_H

#include "KDReportsGlobal.h"
#include <QDialog>

namespace KDReports {
class TableBreakingSettingsDialogPrivate;
class Report;

/**
 * The TableBreakingSettingsDialog class provides a dialog
 * for the user to change settings related to table layout
 * and font scaling. Consult the short source code of this class
 * or an instance of the dialog for full details.
 * @see Report::setTableBreakingPageOrder, Report::setFontScalingFactor,
 *      AutoTable::setHorizontalHeaderVisible, ...
 *      
 * \since 1.1
 */
class KDREPORTS_EXPORT TableBreakingSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructs a settings dialog for \a report as a child window of \a parent.
     */
    explicit TableBreakingSettingsDialog( KDReports::Report* report, QWidget *parent = 0 );

    ~TableBreakingSettingsDialog();

private Q_SLOTS:
    /// \reimp
    void accept();
    /// \reimp
    void reject();

private:
    Q_PRIVATE_SLOT(d, void _kd_slotBreakTablesToggled(bool))
    TableBreakingSettingsDialogPrivate* const d;
};

}

#endif
