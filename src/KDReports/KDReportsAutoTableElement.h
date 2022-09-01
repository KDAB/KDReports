/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSAUTOTABLEELEMENT_H
#define KDREPORTSAUTOTABLEELEMENT_H

#include "KDReportsAbstractTableElement.h"
#include <QtCore/QSize>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextTableCell;
class QAbstractItemModel;
QT_END_NAMESPACE

namespace KDReports {
class AutoTableElementPrivate;

/**
 * The KDReports::AutoTableElement class represents a table in the report,
 * whose data is provided by a QAbstractItemModel.
 * A header row is added if the QAbstractItemModel has horizontal header data,
 * and a header column is added if the QAbstractItemModel has vertical header data.
 * The header row is repeated on every page if the table is broken across page
 * boundaries.
 */
class KDREPORTS_EXPORT AutoTableElement : public AbstractTableElement
{
public:
    /**
     * Creates a table element from the given table model.
     * Note that the argument isn't const because KDReports will call fetchMore() on it,
     * if canFetchMore() returns true.
     */
    explicit AutoTableElement(QAbstractItemModel *tableModel);

    /**
     * Creates a table element that does not have an associated model
     * yet. The association will be done later using the model key.
     */
    explicit AutoTableElement(const QString &modelKey);

    /**
     * Destructor. Deletes internal data.
     */
    ~AutoTableElement() override;

    /**
     * Copies a table element.
     * The model and settings are copied over.
     */
    AutoTableElement(const AutoTableElement &other);
    /**
     * Copies the model and settings from another table element.
     */
    AutoTableElement &operator=(const AutoTableElement &other);

    /**
     * \return the model associated with this element
     * \since 1.2
     */
    QAbstractItemModel *tableModel() const;

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
     * Sets whether to show a vertical header (showing header data from the model,
     * or row numbers by default).
     * This is true by default, call setVerticalHeaderVisible(false) to hide the vertical header.
     */
    void setVerticalHeaderVisible(bool visible);

    /**
     * Sets whether to show a horizontal header, showing header data from the model.
     * This is true by default, call setHorizontalHeaderVisible(false) to hide the horizontal header.
     */
    void setHorizontalHeaderVisible(bool visible);

    /**
     * \return true if the vertical header will be visible
     * \since 1.1
     */
    bool isVerticalHeaderVisible() const;

    /**
     * \return true if the horizontal header will be visible
     * \since 1.1
     */
    bool isHorizontalHeaderVisible() const;

    /**
     * Sets the background color of the headers
     * The default color is gray.
     * Call setHeaderBackground(QBrush()) to disable the background color
     * and have transparent headers instead.
     */
    void setHeaderBackground(const QBrush &brush);

    /**
     * \return the background color of the headers
     * \since 1.4
     */
    QBrush headerBackground() const;

    /**
     * Sets the size of the decoration icons, in pixels.
     * This is used when setting the DecorationRole to a QIcon in the model.
     * Note that the model can also set it to a QPixmap or QImage, in which case
     * the size in the report will simply be the size of the pixmap or image.
     * \since 1.1
     */
    void setIconSize(QSize iconSize);

    /**
     * \return the size passed to setIconSize
     * \since 1.3
     */
    QSize iconSize() const;

    /**
     * @internal
     * @reimp
     */
    void build(ReportBuilder &) const override;
    /**
     * @internal
     * @reimp
     */
    Element *clone() const override;

    enum Role
    {
        DecorationAlignmentRole = 0x2D535FB1, ///< This model role allows to specify whether the icon should go before the text (Qt::AlignLeft) or after the text (Qt::AlignRight).
        NonBreakableLinesRole = 0x2D535FB2 ///< This model role allows to specify that line-breaking is not allowed in this table cell. \since 1.7.
    };

private:
    std::unique_ptr<AutoTableElementPrivate> d;
};

}

#endif /* KDREPORTSAUTOTABLEELEMENT_H */
