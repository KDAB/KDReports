/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2022-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSWINDOW_H
#define KDREPORTSWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class KDReportsWindow;
}
QT_END_NAMESPACE

class KDReportsWindow : public QMainWindow
{
    Q_OBJECT

public:
    KDReportsWindow(QWidget *parent = nullptr);
    ~KDReportsWindow();

    void referenceReport();
    void letterReport();
    void mailMergeXml();
    void priceListXml();

private:
    Ui::KDReportsWindow *ui;
};
#endif // KDREPORTSWINDOW_H
