/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef CHARTTEXTOBJECT_H
#define CHARTTEXTOBJECT_H

#ifdef HAVE_KDCHART

#include <QTextObjectInterface>

namespace KDReports {

class ChartTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    enum
    {
        ChartObjectTextFormat = QTextFormat::UserObject + 2
    };
    enum
    {
        ChartObject = 1,
        Size = 2,
        Unit = 3
    };

    static void registerChartTextObjectHandler(QTextDocument *doc);

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) override;
};

} // namespace

#endif

#endif /* CHARTTEXTOBJECT_H */
