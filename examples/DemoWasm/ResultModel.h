/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef RESULTMODEL_H
#define RESULTMODEL_H

#include <QAbstractTableModel>
#include <QSize>
#include <QStringList>
#include <QVector>

/**
 * Test Model for the "cell span" feature in AutoTableElement.
 */
class ResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ResultModel(QObject *parent = 0);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QSize span(const QModelIndex &index) const override;

private:
    struct Data
    {
        Data(const QString &_param, qreal _value, const QString &_unit)
        {
            list << _param << QString::number(_value) << _unit;
            merged = false;
        }
        Data(const QString &_param, const QString &_error)
        {
            list << _param << _error << QString("hidden");
            merged = true;
        }
        Data()
        {
            merged = false;
        }

        QStringList list;
        bool merged;
    };
    QVector<Data> m_data;
};

#endif /* RESULTMODEL_H */
