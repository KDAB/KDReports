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

#ifndef KDREPORTSCHARTELEMENT_H
#define KDREPORTSCHARTELEMENT_H

#include <QSharedDataPointer>
#include "KDReportsElement.h"
#include "KDReportsUnit.h"
#include <QtCore/QtGlobal> // qreal
#include <QSizeF>
class QAbstractItemModel;

namespace KDChart { class Chart; }

namespace KDReports {
class ChartElementPrivate;

/**
 * The KDReports::ChartElement class represents a chart in the report.
 * The data is provided by a QAbstractItemModel.
 */
class KDREPORTS_EXPORT ChartElement : public Element
{
public:
    /**
     * Creates a chart element from the given table model.
     */
    explicit ChartElement( QAbstractItemModel* tableModel );

    /**
     * Creates a chart element that does not have an associated model
     * yet. The association will be done later using the model key.
     * @see KDReports::Report::associateModel()
     */
    explicit ChartElement( const QString& modelKey );

    /**
     * Creates a chart element from the given chart.
     * 
     * The chart's ownership is NOT transferred. It is very important
     * that the chart exists at least as long as the report object exists.
     *
     * @since 1.3
     */
    explicit ChartElement( KDChart::Chart* chart );

    /**
     * Destroys this chart element.
     */
    virtual ~ChartElement();

    /**
     * Copies a chart element.
     */
    ChartElement(const ChartElement &other);
    /**
     * Copies the data from another chart element.
     */
    ChartElement &operator=(const ChartElement &other);

    /**
     * Sets the chart to use in this chart element.
     * Useful for associating a chart to a ChartElement from XmlElementHandler.
     * 
     * The chart's ownership is NOT transferred. It is very important
     * that the chart exists at least as long as the report object exists.
     *
     * @since 1.3
     */
    void setChart( KDChart::Chart* chart );

    /**
     * Sets the size of the chart.
     * The default size is a 100% of the page width, for both width and height.
     *
     * To set a chart size that adapts well to any page size, use Percent as unit.
     * setSize(50, 75, Percent) means that the width will be half of the page width (without margins),
     * and the height will always be 1.5 times the width (preserving the ratio between 50 and 75).
     * This feature (Percent) leads to slightly worse results on screen at 100% zoom, due to scaling
     * happening even at that size, but the printed report should be fine.
     */
    void setSize( qreal width, qreal height, Unit unit = Millimeters );

    /**
     * \return the size of the chart, use unit() to know the size unit
     * \since 1.4
     */
    QSizeF size() const;

    /**
      * \return the size unit
      * \since 1.4
      */
    Unit unit() const;

    /**
     * set the model associated with this element
     * \since 1.4
     */
    void setTableModel(QAbstractItemModel *tableModel);

    /**
     * set the model key associated with this element
     * \since 1.4
     */
    void setModelKey(const QString &modelKey);

    /**
     * @internal
     * @reimp
     */
    void build( ReportBuilder& ) const;
    /**
     * @internal
     * @reimp
     */
    Element* clone() const;

private:
    QSharedDataPointer<ChartElementPrivate> d;
};

}

#endif /* KDREPORTSCHARTELEMENT_H */
