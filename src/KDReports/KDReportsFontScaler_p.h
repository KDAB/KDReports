/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef FONTSCALER_P_H
#define FONTSCALER_P_H

#include "KDReportsGlobal.h"
#include <QFont>
#include <QFontMetricsF>

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
    explicit FontScaler(const QFont &initialFont);

    void setFontAndScalingFactor(const QFont &font, qreal scalingFactor);

    void setFactorForHeight(qreal wantedHeight);
    void setFactorForWidth(qreal wantedFactor, const QString &sampleText);
    void applyAdditionalScalingFactor(qreal factor);

    QFont font() const
    {
        return m_font;
    }
    QFontMetricsF fontMetrics() const
    {
        return m_fontMetrics;
    }
    qreal scalingFactor() const
    {
        return m_scalingFactor;
    }
    QFontMetricsF initialFontMetrics() const
    {
        return m_initialFontMetrics;
    }
    qreal textWidth(const QString &text) const;

private:
    QFont m_font;
    QFontMetricsF m_fontMetrics;
    QFontMetricsF m_initialFontMetrics;
    qreal m_scalingFactor;
};

}

#endif /* FONTSCALER_P_H */
