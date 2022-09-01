/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsLayoutHelper_p.h"

QT_BEGIN_NAMESPACE
Q_GUI_EXPORT extern int qt_defaultDpi(); // This is what QTextDocument uses...
QT_END_NAMESPACE

qreal KDReports::mmToPixels(qreal mm)
{
    return mm * 0.039370147 // mm to inch
        * qt_defaultDpi(); // inch to pixels
}

qreal KDReports::pixelsToPointsMultiplier(double resolution)
{
    // See Q_GUI_EXPORT qreal qt_pixelMultiplier(int resolution) in Qt
    return resolution <= 0 ? 1.0 : 72.0 / resolution;
}
