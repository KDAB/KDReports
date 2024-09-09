/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
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

    enum
    {
        HLineTextFormat = QTextFormat::UserObject + 1
    };
    enum
    {
        Color = QTextFormat::UserProperty + 1,
        Thickness = QTextFormat::UserProperty + 2,
        Margin = QTextFormat::UserProperty + 3,
    };

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
