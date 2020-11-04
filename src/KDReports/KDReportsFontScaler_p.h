/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#ifndef FONTSCALER_P_H
#define FONTSCALER_P_H

#include <QFontMetricsF>
#include <QFont>
#include "KDReportsGlobal.h"

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


// Enable this to get debug output about the layouting
// (font scaling, table layout, spreadsheet layout)
//#define DEBUG_LAYOUT

namespace KDReports {

class KDREPORTS_EXPORT FontScaler
{
public:
    explicit FontScaler( const QFont& initialFont );

    void setFontAndScalingFactor( const QFont& font, qreal scalingFactor );

    void setFactorForHeight( qreal wantedHeight );
    void setFactorForWidth( qreal wantedFactor, const QString& usingText );
    void applyAdditionalScalingFactor( qreal factor );

    QFont font() const { return m_font; }
    QFontMetricsF fontMetrics() const { return m_fontMetrics; }
    qreal scalingFactor() const { return m_scalingFactor; }
    QFontMetricsF initialFontMetrics() const { return m_initialFontMetrics; }

private:
    QFont m_font;
    QFontMetricsF m_fontMetrics;
    QFontMetricsF m_initialFontMetrics;
    qreal m_scalingFactor;
};

}

#endif /* FONTSCALER_P_H */
