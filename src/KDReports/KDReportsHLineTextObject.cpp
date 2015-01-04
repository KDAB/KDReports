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

#include "KDReportsHLineTextObject_p.h"

#include <QTextDocument>
#include <QTextFormat>
#include <QTextFrame>
#include <QPainter>
#include <QRectF>
#include <QColor>

#include <QDebug>

namespace KDReports {

Q_GLOBAL_STATIC(HLineTextObject, globalHLineInterface)

void HLineTextObject::registerHLineObjectHandler(QTextDocument *doc)
{
    HLineTextObject *hLineInterface = globalHLineInterface();

     //This assert is here because a bad build environment can cause this to fail. There is a note
    // in the Qt source that indicates an error should be output, but there is no such output.
    Q_ASSERT(qobject_cast<QTextObjectInterface *>(hLineInterface));

    doc->documentLayout()->registerHandler( HLineTextObject::HLineTextFormat, hLineInterface );
}


QSizeF HLineTextObject::intrinsicSize(QTextDocument *doc, int posInDocument,
            const QTextFormat &format)
{
    Q_UNUSED(posInDocument);

    QTextFrameFormat f = doc->rootFrame()->frameFormat();
    const qreal width = doc->pageSize().width() - (f.leftMargin() + f.rightMargin());

    const double thickness = format.doubleProperty( Thickness );
    const double margin = format.doubleProperty( Margin );
    //qDebug() << "intrinsicSize: thickness=" << thickness << "margin=" << margin;

    return QSizeF(width, thickness + 2 * margin);
}

void HLineTextObject::drawObject(QPainter *painter, const QRectF &r,
            QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);

    const QColor color = qvariant_cast<QColor>( format.property(Color) );

    QPen pen;
    pen.setWidthF( format.doubleProperty( Thickness ) );

    pen.setColor( color );
    painter->setPen( pen );

    const qreal y = r.top() + ( r.height() / 2 );
    QLineF line( r.left(), y, r.right(), y );

    painter->drawLine(line);
}

}
