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

#include "TableModel.h"

#include <QtGlobal>
#include <QtDebug>
#include <QFile>
#include <QByteArray>
#include <QString>
#include <QStringList>

TableModel::TableModel ( QObject * parent )
    : QAbstractTableModel ( parent ),
      m_dataHasHorizontalHeaders( true ),
      m_dataHasVerticalHeaders( true ),
      m_supplyHeaderData( true )
{
}

TableModel::~TableModel ()
{
}

int TableModel::rowCount ( const QModelIndex & ) const
{
    return m_rows.size();
}

int TableModel::columnCount ( const QModelIndex & ) const
{
    if ( m_rows.isEmpty() ) {
        return 0;
    } else {
        return m_rows[0].size();
    }
}

QVariant TableModel::data ( const QModelIndex & index, int role) const
{
    // FIXME kdchart queries (-1, -1) for empty models
    if ( index.row() == -1 || index.column() == -1 )
    {
        qDebug() << "TableModel::data: row: "
                 << index.row() << ", column: " << index.column()
                 << ", rowCount: " << rowCount() << ", columnCount: "
                 << columnCount() << endl
                 << "TableModel::data: FIXME fix kdchart views to not query"
            " model data for invalid indices!" << endl;
        return QVariant();
    }

//     qDebug () << "TableModel::data: row: "<< index.row() << ", column: "
//               << index.column() << endl;
    Q_ASSERT ( index.row() >= 0 && index.row() < rowCount() );
    Q_ASSERT ( index.column() >= 0 && index.column() < columnCount() );

    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        return m_rows[index.row()] [index.column()];
    } else {
        return QVariant();
    }
}

QVariant TableModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    switch ( role )
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    if ( m_supplyHeaderData )
    {
        if ( orientation == Qt::Horizontal )
        {   // column header data:
            if ( section < m_horizontalHeaderData.count() )
                result = m_horizontalHeaderData[section];
        } else {
            // row header data:
            if ( section < m_verticalHeaderData.count() )
                result = m_verticalHeaderData[section];
        }
    }
    break;
    case Qt::TextAlignmentRole:
//        result = QVariant ( Qt::AlignHCenter | Qt::AlignHCenter );
        break;
    case Qt::DecorationRole:
    case Qt::ToolTipRole:
        break;
    default:
//        qDebug () << "TableModel::headerData: unknown role " << role << "." << endl;
        break;
    }
    return result;
}


bool TableModel::setData ( const QModelIndex & index, const QVariant & value, int role/* = Qt::EditRole */ )
{
    Q_ASSERT ( index.row() >= 0 && index.row() < rowCount() );
    Q_ASSERT ( index.column() >= 0 && index.column() < columnCount() );

    if ( role == Qt::EditRole )
    {
        m_rows[index.row()] [index.column()] = value;
        emit dataChanged( index, index );
        return true;
    } else {
        return false;
    }
}

bool TableModel::loadFromCSV ( const QString& filename )
{
    QFile file ( filename );
    QStringList data;

    if ( file.exists() && file.open ( QIODevice::ReadOnly )  )
    {
        while ( !file.atEnd() )
        {
            QString line = QString::fromUtf8( file.readLine() );
            data.append ( line );
        }

        if ( data.size() > 0 )
        {
//             qDebug() << "TableModel::loadFromCSV: " << data.size()
//                      << " data rows found." << endl;

            m_rows.resize ( data.size() - 1 );

            // debugging code:
            //int previousColumnCount = 0;

            for ( int row = 0; row < data.size(); ++row )
            {
                const QStringList parts = data.at( row ).split ( QLatin1Char( ',' ) );

                //Q_ASSERT ( previousColumnCount == parts.size() || previousColumnCount == 0 );
                //previousColumnCount = parts.size();

                QVector<QVariant> values( parts.size() );

                for ( int column = 0; column < parts.size(); ++column )
                {
                    // get rid of trailing or leading whitespaces and quotes:
                    QString cell ( parts.at( column ).simplified() );

                    if ( cell.startsWith( QLatin1String("\"") ) )
                    {
                        cell.remove ( 0, 1 );
                    }
                    if ( cell.endsWith( QLatin1String("\"") ) )
                    {
                        cell.remove ( cell.length()-1, 1 );
                    }


                    if ( row == 0 && m_dataHasHorizontalHeaders )
                    {   // interpret the first row as column headers:
                        m_horizontalHeaderData.append( cell );
                        //setHeaderData ( column,  Qt::Horizontal, QVariant( cell ), Qt::DisplayRole );
                    } else {
                        if ( column == 0 && m_dataHasVerticalHeaders )
                        {   // interpret first column as row headers:
                            m_verticalHeaderData.append( cell );
                            //setHeaderData ( row,  Qt::Vertical, QVariant ( cell ) );
                        } else {
                            // interpret cell values as floating point:
                            bool convertedOk = false;
                            double value = cell.toDouble ( &convertedOk );
                            const int destColumn = m_dataHasVerticalHeaders ? column - 1 : column;
                            values[destColumn] = convertedOk ? QVariant( value ) : cell;
                        }
                    }
                }
                if ( row > 0 ) {
                    const int destRow = m_dataHasHorizontalHeaders ? row - 1 : row;
                    m_rows[destRow] = values;
                }
            }
        } else {
            m_rows.resize ( 0 );
        }

        reset();
        if ( m_rows.size () > 0 )
        {
//             qDebug() << "TableModel::loadFromCSV: table loaded, "
//                      << rowCount() << " rows, " << columnCount() << "columns."
//                      << endl;
        } else {
            qDebug() << "TableModel::loadFromCSV: table loaded, but no "
                "model data found." << endl;
        }
        return true;
    } else {
        qDebug() << "TableModel::loadFromCSV: file" << filename
                << "does not exist / or could not be opened" << endl;
        return false;
    }
}

void TableModel::clear()
{
    m_rows.resize( 0 );
    reset();
}
