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

#ifndef RESULTMODEL_H
#define RESULTMODEL_H

#include <QSize>
#include <QStringList>
#include <QVector>
#include <QAbstractTableModel>

/**
 * Test Model for the "cell span" feature in AutoTableElement.
 */
class ResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ResultModel( QObject* parent=0 );

    int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;

    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    QSize span( const QModelIndex& index ) const;

private:
    struct Data
    {
        Data( const QString& _param, qreal _value,
              const QString& _unit )
        {
            list << _param << QString::number( _value ) << _unit;
            merged = false;
        }
        Data( const QString& _param, const QString& _error )
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

