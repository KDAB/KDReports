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

#include "KDReportsMainTable.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsSpreadsheetReportLayout_p.h"
#include <QDebug>

class KDReports::MainTablePrivate
{
public:
    MainTablePrivate() : m_element(0), m_layout(0) {}
    AutoTableElement* m_element;
    SpreadsheetReportLayout* m_layout;
};

KDReports::MainTable::MainTable()
    : d(new MainTablePrivate)
{
}

KDReports::MainTable::~MainTable()
{
    delete d->m_element;
    delete d;
}

void KDReports::MainTable::setAutoTableElement(const AutoTableElement &element)
{
    delete d->m_element;
    d->m_element = new AutoTableElement(element);

    d->m_layout->setModel(element.tableModel());
    d->m_layout->setVerticalHeaderVisible(element.isVerticalHeaderVisible());
    d->m_layout->setHorizontalHeaderVisible(element.isHorizontalHeaderVisible());
    d->m_layout->setCellPadding(element.padding()); // in mm
    d->m_layout->setIconSize(element.iconSize());
    d->m_layout->setCellBorder(element.border(), element.borderBrush());
    d->m_layout->setHeaderBackground(element.headerBackground());
}

KDReports::AutoTableElement* KDReports::MainTable::autoTableElement() const
{
    return d->m_element;
}

void KDReports::MainTable::setTableBreakingPageOrder(Report::TableBreakingPageOrder pageOrder)
{
    d->m_layout->setTableBreakingPageOrder( pageOrder );
}

KDReports::Report::TableBreakingPageOrder KDReports::MainTable::tableBreakingPageOrder() const
{
    return d->m_layout->tableBreakingPageOrder();
}

void KDReports::MainTable::setLayout(SpreadsheetReportLayout *layout)
{
    d->m_layout = layout;
}

void KDReports::MainTable::setHorizontalHeaderFont(const QFont &font)
{
    d->m_layout->setHorizontalHeaderFont(font);
}

void KDReports::MainTable::setVerticalHeaderFont(const QFont &font)
{
    d->m_layout->setVerticalHeaderFont(font);
}

QList<QRect> KDReports::MainTable::pageRects() const
{
   d->m_layout->ensureLayouted();
   return d->m_layout->m_pageRects;
}

qreal KDReports::MainTable::lastAutoFontScalingFactor() const
{
    return d->m_layout->m_tableLayout.scalingFactor();
}
