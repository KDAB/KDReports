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

#include "KDReportsFontScaler_p.h"
#include <QDebug>
#include <QFontMetricsF>
#include <QFont>

using namespace KDReports;

FontScaler::FontScaler(const QFont& initialFont)
    : m_font( initialFont ),
      m_fontMetrics( m_font ),
      m_initialFontMetrics( m_fontMetrics ),
      m_scalingFactor( 1.0 )
{
}

void FontScaler::setFontAndScalingFactor( const QFont& font, qreal scalingFactor )
{
#ifdef DEBUG_LAYOUT
    //qDebug() << "setFontAndScalingFactor scalingFactor=" << scalingFactor;
#endif
    m_font = font;
    m_scalingFactor = scalingFactor;
    if ( m_font.pixelSize() == -1 )
        m_font.setPointSizeF( m_font.pointSizeF() * scalingFactor );
    else
        m_font.setPixelSize( m_font.pixelSize() * scalingFactor );
    m_fontMetrics = QFontMetricsF( m_font );
    m_initialFontMetrics = m_fontMetrics;
}

void FontScaler::applyAdditionalScalingFactor( qreal factor )
{
    // TODO this should be calculated in the end, using finalWidth/initialWidth. It's currently messed up by the -0.1 below for instance.
    m_scalingFactor *= factor;

    if ( m_font.pixelSize() == -1 ) {
        if ( factor > 0.99 && factor < 1.000 ) // applying 0.999 forever can take a very long time ;)
            m_font.setPointSizeF( m_font.pointSizeF() - 0.1 );
        else
            m_font.setPointSizeF( m_font.pointSizeF() * factor );
    } else {
        if ( m_font.pixelSize() > 2 && factor > 0.99 && factor < 1.000 )
            m_font.setPixelSize( m_font.pixelSize() - 1 );
        else
            m_font.setPixelSize( m_font.pixelSize() * factor );
    }
#ifdef DEBUG_LAYOUT
    qDebug() << " applyAdditionalScalingFactor" << factor << "combined factor:" << m_scalingFactor
             << "pointSize:" << m_font.pointSizeF() << "pixelSize:" << m_font.pixelSize();
#endif
    m_fontMetrics = QFontMetricsF( m_font );
}

void FontScaler::setFactorForHeight( qreal wantedHeight )
{
#ifdef DEBUG_LAYOUT
    qDebug() << " FontScaler::setFactorForHeight" << wantedHeight;
#endif
    Q_ASSERT( wantedHeight > 0 );
    qreal height = m_fontMetrics.height();
    int iterations = 0;
    qreal prevHeight = height;

    while ( height > wantedHeight && height > 3.0 /* seems to be the min we can get */ ) {
        const qreal factor = wantedHeight / height;
        applyAdditionalScalingFactor( factor );
        prevHeight = height;
        height = m_fontMetrics.height();
#ifdef DEBUG_LAYOUT
        qDebug() << " FontScaler: height=" << height << factor << m_scalingFactor;
#endif
        if ( ++iterations > 10 && height == prevHeight ) {
            // We're not getting anywhere, probably we're hitting a minimum width when
            // trying to use a very very small font (see unittest testFontScalerVerySmall)
            break;
        }
        if ( m_font.pixelSize() == 1 ) {
            break;
        }
    }
}

void FontScaler::setFactorForWidth( qreal wantedFactor, const QString& sampleText )
{
#ifdef DEBUG_LAYOUT
    qDebug() << " FontScaler::setFactorForWidth" << wantedFactor;
#endif
    // Just applying that scaling factor for the font size isn't enough,
    // fonts do not scale proportionnally. We need do this like
    // "scale the font so that this text fits into this width"
    const qreal initialWidth = m_initialFontMetrics.width( sampleText );
    const qreal wantedWidth = initialWidth * wantedFactor;
    qreal width = m_fontMetrics.width( sampleText );
#ifdef DEBUG_LAYOUT
    qDebug() << " FontScaler: sampleText with initialFontMetrics:" << initialWidth << "with current fontMetrics:" << width << "wanted:" << wantedWidth;
#endif

    int iterations = 0;
    qreal prevWidth = width;

    while ( width > wantedWidth ) {
        qreal factor = wantedWidth / width;
        applyAdditionalScalingFactor( factor );
        prevWidth = width;
        width = m_fontMetrics.width( sampleText );
#ifdef DEBUG_LAYOUT
        qDebug() << "  FontScaler: width=" << width << "factor=" << factor << "m_scalingFactor=" << m_scalingFactor;
#endif
        if ( ++iterations > 10 && width == prevWidth ) {
            // We're not getting anywhere, probably we're hitting a minimum width when
            // trying to use a very very small font (see unittest testFontScalerVerySmall)
            break;
        }
    }
}
