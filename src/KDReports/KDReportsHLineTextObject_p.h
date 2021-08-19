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

#ifndef HLINETEXTOBJECT_H
#define HLINETEXTOBJECT_H

#include <QTextObjectInterface>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
QT_END_NAMESPACE

namespace KDReports {

class HLineTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    using QObject::QObject;

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

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) override;
};

}

#endif /* HLINETEXTOBJECT_H */
