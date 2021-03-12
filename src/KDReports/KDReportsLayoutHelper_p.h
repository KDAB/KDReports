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

#ifndef KDREPORTSLAYOUTHELPER_H
#define KDREPORTSLAYOUTHELPER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Reports API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#include "KDReportsGlobal.h"
#include <QTextFormat>
#include <QtCore/QtGlobal> // qreal

namespace KDReports {

/**
 * @internal
 */
KDREPORTS_EXPORT qreal mmToPixels(qreal mm);
KDREPORTS_EXPORT qreal mmToPixels(qreal mm, double dpi);
KDREPORTS_EXPORT qreal pixelsToPointsMultiplier(double resolution);

static const int ResizableImageProperty = QTextFormat::UserProperty + 5984;

}

#endif /* KDREPORTSLAYOUTHELPER_H */
