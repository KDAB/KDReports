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

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QVariant>
#include <QVector>
#include <QAbstractTableModel>
#include "tools_export.h"
#include <QStringList>

/** TableModel uses a simple rectangular vector of vectors to represent a data
    table that can be displayed in regular Qt Interview views.
    Additionally, it provides a method to load CSV files exported by
    OpenOffice Calc in the default configuration. This allows to prepare test
    data using spreadsheet software.

    It expects the CSV files in the subfolder ./modeldata. If the application
    is started from another location, it will ask for the location of the
    model data files.
*/

class TESTTOOLS_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel ( QObject * parent = 0 );
    ~TableModel ();

    /** Return header data from the model.
        The model will use the first data row and the first data column of the
        physical data as source of column and row header data. This data isnot
        exposed as model data, that means, the first model row and column will
        start at index (1,1).
    */
    QVariant headerData ( int section, Qt::Orientation orientation,
                          int role = Qt::DisplayRole ) const ;

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    /** Load the table from a comma separated file.
     *
     * The files are supposed to be Unicode (UTF8), have commas (',') as
     * delimiters, and quotes ('"') as text delimiters. All lines are expected
     * to provide the same number of fields. HINT: This is the default way
     * OO.o-Calc export CSV files.
     * The cell data is expected to be floating point values, except for the
     * first row and the first column, where string values are exected (those
     * will be used as axis descriptors). If values cannot be converted to
     * doubles, there string representation will be used.
     *
     * @returns true if successful, false otherwise
     */
    bool loadFromCSV ( const QString& filename );

    /** Make the model invalid, that is, provide no data. */
    void clear();

    /**
     * Set to false if the data has no horizontal header
     */
    void setDataHasHorizontalHeaders( bool value ) {
        m_dataHasHorizontalHeaders = value;
    }
    /**
     * Set to false if the data has no vertical header
     */
    void setDataHasVerticalHeaders( bool value ) {
        m_dataHasVerticalHeaders = value;
    }
    /**
     * setSupplyHeaderData(false) allows to prevent the model from supplying header data,
     * even if parsing found any
     */
    void setSupplyHeaderData( bool value ) {
        m_supplyHeaderData = value;
    }

private:
    // the vector of rows:
    QVector< QVector<QVariant> > m_rows;
    // the header data:
    QStringList m_horizontalHeaderData;
    QStringList m_verticalHeaderData;
    bool m_dataHasHorizontalHeaders;
    bool m_dataHasVerticalHeaders;
    bool m_supplyHeaderData;
};


#endif
