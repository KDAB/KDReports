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
    explicit TableBreakingSettingsDialog(KDReports::Report *report, QWidget *parent = nullptr);

    ~TableBreakingSettingsDialog() override;

private Q_SLOTS:
    /// \reimp
    void accept() override;
    /// \reimp
    void reject() override;

private:
    TableBreakingSettingsDialogPrivate *const d;
};

}

#endif
