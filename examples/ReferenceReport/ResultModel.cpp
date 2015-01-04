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

#include "ResultModel.h"

ResultModel::ResultModel( QObject* parent)
    : QAbstractTableModel( parent )
{
    m_data << Data( "Wind", 69, "km/h" );
    m_data << Data( "Earthquakes", "none" ); // merged=true
    m_data << Data( "Min Temperature", 10, "deg C" );
    m_data << Data( "Max Temperature", 40, "deg C" );
}

int ResultModel::columnCount( const QModelIndex& parent ) const
{
    if ( parent.isValid() )
        return 0;
    return 3; // by definition
}

int ResultModel::rowCount( const QModelIndex& parent ) const
{
    if ( parent.isValid() )
        return 0;
    return m_data.count();
}

QVariant ResultModel::data( const QModelIndex& index, int role) const
{
    if( !index.isValid() )
        return QVariant();

    if( role == Qt::DisplayRole || role == Qt::EditRole ) {
        return m_data[index.row()].list.at( index.column() );
    }
    if ( role == Qt::TextAlignmentRole ) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

QVariant ResultModel::headerData( int section, Qt::Orientation orientation, int role) const
{
    if( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        switch( section ) {
        case 0:
            return "Parameter";
        case 1:
            return "Value";
        case 2:
            return "Unit";
        default:
            break;
        }
    }
    return QVariant();
}

QSize ResultModel::span( const QModelIndex& index ) const
{
    if ( !index.isValid() )
        return QSize();

    const int row = index.row();
    const int column = index.column();

    if ( column == 1 && m_data[row].merged )
        return QSize( 2, 1 );

    if ( row < rowCount()-1 && m_data[row].list.at(column) == m_data[row+1].list.at(column))
        return QSize( 1, 2 );

    return QSize();
}
