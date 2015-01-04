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

#include <KDReports>
#include <QApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>


int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    KDReports::Report report;

    // open a DB connection to an in-memory database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if( !db.open() ) {
        QMessageBox::critical(0, QObject::tr("Cannot open database"),
                              QObject::tr("Cannot create connection to the requested database. Your Qt is probably lacking the QSQLITE driver. Please check your Qt installation." ), QMessageBox::Cancel );
        return false;
    }

    // fill the DB with some test data
    QSqlQuery query;
    query.exec("create table airlines (id int primary key, "
               "name varchar(20), homecountry varchar(2))");
    query.exec("insert into airlines values(1, 'Lufthansa', 'DE')");
    query.exec("insert into airlines values(2, 'SAS', 'SE')");
    query.exec("insert into airlines values(3, 'United', 'US')");
    query.exec("insert into airlines values(4, 'KLM', 'NL')");
    query.exec("insert into airlines values(5, 'Aeroflot', 'RU')");

    // Create a QSqlTableModel, connect to the previously created database, fill
    // the db with some data.
    QSqlTableModel tableModel( 0, db );
    tableModel.setTable( "airlines" );
    tableModel.select();
    tableModel.removeColumn( 0 );
    tableModel.setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    tableModel.setHeaderData(1, Qt::Horizontal, QObject::tr("Home country"));
    QFont font = app.font();
    font.setBold( true );
    tableModel.setHeaderData( 0, Qt::Horizontal, font, Qt::FontRole );
    tableModel.setHeaderData( 1, Qt::Horizontal, font, Qt::FontRole );

    KDReports::AutoTableElement tableElement( &tableModel );
    tableElement.setVerticalHeaderVisible( false );
    report.addElement( tableElement );

    // To export to an image file:
    //qDebug() << "Exporting to output.png";
    //report.exportToImage( QSize(300, 400), "output.png", "PNG" );


    KDReports::PreviewDialog preview( &report );
    return preview.exec();
}

