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
                    const QTextFormat &format) override;

    void drawObject(QPainter *painter, const QRectF &rect,
                    QTextDocument *doc, int posInDocument,
                    const QTextFormat &format) override;

};

} // namespace

#endif

#endif /* CHARTTEXTOBJECT_H */
