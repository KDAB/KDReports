/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsMainTable.h"
#include "KDReportsAutoTableElement.h"
#include "KDReportsSpreadsheetReportLayout_p.h"
#include <QDebug>

class KDReports::MainTablePrivate
{
public:
    MainTablePrivate()
        : m_element(nullptr)
        , m_layout(nullptr)
    {
    }
    AutoTableElement *m_element;
    SpreadsheetReportLayout *m_layout;
};

KDReports::MainTable::MainTable()
    : d(new MainTablePrivate)
{
}

KDReports::MainTable::~MainTable()
{
    delete d->m_element;
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

KDReports::AutoTableElement *KDReports::MainTable::autoTableElement() const
{
    return d->m_element;
}

void KDReports::MainTable::setTableBreakingPageOrder(Report::TableBreakingPageOrder pageOrder)
{
    d->m_layout->setTableBreakingPageOrder(pageOrder);
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
