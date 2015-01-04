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

#ifndef CHARTTEXTOBJECT_H
#define CHARTTEXTOBJECT_H

#ifdef HAVE_KDCHART

#include <QTextObjectInterface>

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
/* QMetaType must be told about
 * the existence of KDChart::Chart*
 * type in order to allow its usage
 * with QVariants
 */
namespace KDChart {
    class Chart;
}

Q_DECLARE_METATYPE(KDChart::Chart*)
#endif

namespace KDReports {

class ChartTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    enum { ChartObjectTextFormat = QTextFormat::UserObject + 2 };
    enum { 
        ChartObject = 1, 
        Size = 2, 
        Unit = 3 
    };

    static void registerChartTextObjectHandler(QTextDocument *doc);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                    const QTextFormat &format);

    void drawObject(QPainter *painter, const QRectF &rect,
                    QTextDocument *doc, int posInDocument,
                    const QTextFormat &format);

};

} // namespace

#endif

#endif /* CHARTTEXTOBJECT_H */
