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

#include "KDReportsImageElement.h"
#include "KDReportsReport.h"
#include "KDReportsReport_p.h"
#include "KDReportsLayoutHelper_p.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QTextDocument>
#include <QUrl>

class KDReports::ImageElementPrivate
{
public:
    ImageElementPrivate() : m_width(0), m_height(0),
                m_fitToPage(false),
                m_unit(KDReports::Millimeters)
    {}
    QVariant m_pixmap; // pixmap or image, can't use QPixmap directly in threads
    QSize m_pixmapSize; // = m_pixmap.size()

    // size in final document, chosen by the user:
    qreal m_width;
    qreal m_height;
    bool m_fitToPage;
    KDReports::Unit m_unit;
    QString m_id;
};

KDReports::ImageElement::ImageElement( const QPixmap& pixmap )
    : d( new ImageElementPrivate )
{
    setPixmap( pixmap );
}

KDReports::ImageElement::ImageElement( const QImage& image )
    : d( new ImageElementPrivate )
{
    setImage( image );
}

KDReports::ImageElement::ImageElement(const ImageElement &other)
    : Element(other), d( new ImageElementPrivate( *other.d ) )
{
}

KDReports::ImageElement & KDReports::ImageElement::operator=(const ImageElement &other)
{
    if ( &other == this )
        return *this;
    Element::operator=( other );
    *d = *other.d;
    return *this;
}

KDReports::ImageElement::~ImageElement()
{
    delete d;
}

void KDReports::ImageElement::setPixmap( const QPixmap& pixmap )
{
    d->m_pixmap = QVariant::fromValue(pixmap);
    d->m_pixmapSize = pixmap.size();
}

QPixmap KDReports::ImageElement::pixmap() const
{
    return d->m_pixmap.value<QPixmap>();
}

void KDReports::ImageElement::setImage(const QImage &image)
{
    d->m_pixmap = QVariant::fromValue(image);
    d->m_pixmapSize = image.size();
}

QImage KDReports::ImageElement::image() const
{
    return d->m_pixmap.value<QImage>();
}

void KDReports::ImageElement::setWidth( qreal width, Unit unit )
{
    d->m_width = width;
    d->m_unit = unit;
    d->m_height = 0; // mutually exclusive!
    d->m_fitToPage = false; // "
}

qreal KDReports::ImageElement::width() const
{
    return d->m_width;
}

void KDReports::ImageElement::setHeight( qreal height, Unit unit )
{
    d->m_height = height;
    d->m_unit = unit;
    d->m_width = 0; // mutually exclusive!
    d->m_fitToPage = false; // "
}

qreal KDReports::ImageElement::height() const
{
    return d->m_height;
}

void KDReports::ImageElement::setFitToPage()
{
    d->m_width = 0; // mutually exclusive!
    d->m_height = 0; // mutually exclusive!
    d->m_fitToPage = true;
}

bool KDReports::ImageElement::fitToPage() const
{
    return d->m_fitToPage;
}

void KDReports::ImageElement::build( ReportBuilder& builder ) const
{
#if 0
    if ( d->m_width && d->m_unit == Millimeters ) {
        const QSize sz( qRound( mmToPixels( d->m_width.width() ) ),
                        qRound( mmToPixels( d->m_width.height() ) ) );
        d->m_pixmap = d->m_pixmap.scaled( sz, Qt::KeepAspectRatio, Qt::SmoothTransformation );
        qDebug() << "ImageElement: m_width (mm) =" << d->m_width << " sz (pixels) =" << sz;
    }
#endif

    if ( d->m_pixmapSize.isNull() )
        return;

    static int imageNumber = 0;
    const QString name = QString::fromLatin1( "image%1.png" ).arg( ++imageNumber );
    builder.currentDocument().addResource( QTextDocument::ImageResource, QUrl( name ), d->m_pixmap );
    builder.currentDocumentData().addResourceName( name );

    QTextImageFormat imageFormat;
    imageFormat.setName( name );
    imageFormat.setWidth( d->m_pixmapSize.width() );
    imageFormat.setHeight( d->m_pixmapSize.height() );

    if ( d->m_width ) {
        if ( d->m_unit == Millimeters ) {
            const int pixelWidth = qRound( mmToPixels( d->m_width ) );
            const int pixelHeight = pixelWidth * imageFormat.height() / imageFormat.width();
            imageFormat.setWidth( pixelWidth );
            imageFormat.setHeight( pixelHeight );
        } else {
            imageFormat.setProperty( ResizableImageProperty, QString( QLatin1Char( 'W' ) + QString::number( d->m_width ) ) );
            KDReports::TextDocumentData::updatePercentSize( imageFormat, QSizeF( builder.report()->d->textDocumentWidth(), -1 /*unknown*/ ) );
        }
    } else if ( d->m_height ) {
        if ( d->m_unit == Millimeters ) {
            const int pixelHeight = qRound( mmToPixels( d->m_height ) );
            const int pixelWidth = pixelHeight * imageFormat.width() / imageFormat.height();
            imageFormat.setHeight( pixelHeight );
            imageFormat.setWidth( pixelWidth );
        } else {
            imageFormat.setProperty( ResizableImageProperty, QString( QLatin1Char( 'H' ) + QString::number( d->m_height ) ) );
            // can't calc size yet, will be done at layouting time... hopefully.
        }
    } else if ( d->m_fitToPage ) {
        imageFormat.setProperty( ResizableImageProperty, QString( QLatin1Char( 'T' ) ) );
    }

    QTextCursor& cursor = builder.cursor();
    cursor.insertImage( imageFormat );
}

void KDReports::ImageElement::setId( const QString& id )
{
    d->m_id = id;
}

QString KDReports::ImageElement::id() const
{
    return d->m_id;
}

KDReports::Element* KDReports::ImageElement::clone() const
{
    return new ImageElement( *this );
}

KDReports::Unit KDReports::ImageElement::unit() const
{
    return d->m_unit;
}

void KDReports::ImageElement::setUnit(KDReports::Unit unit)
{
    d->m_unit = unit;
}
