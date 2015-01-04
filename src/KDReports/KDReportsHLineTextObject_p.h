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

#ifndef HLINETEXTOBJECT_H
#define HLINETEXTOBJECT_H

#include <QTextObjectInterface>

class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;

namespace KDReports {

class HLineTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    enum { HLineTextFormat = QTextFormat::UserObject + 1 };
    enum { Color = 1 };
    enum { Thickness = 2 };
    enum { Margin = 3 };

    /**
     * Registers the HLineTextObject (a QTextObjectInterface)
     * handler with a QTextDocument in order to enable the
     * painting of HLineTextObjects
     */
    static void registerHLineObjectHandler(QTextDocument *doc);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                    const QTextFormat &format);

    void drawObject(QPainter *painter, const QRectF &rect,
                    QTextDocument *doc, int posInDocument,
                    const QTextFormat &format);
};


}

#endif /* HLINETEXTOBJECT_H */
