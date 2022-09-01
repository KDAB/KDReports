/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSIMAGEELEMENT_H
#define KDREPORTSIMAGEELEMENT_H

#include "KDReportsElement.h"
#include "KDReportsUnit.h"
#include <QtCore/QtGlobal> // qreal

QT_BEGIN_NAMESPACE
class QPixmap;
class QImage;
QT_END_NAMESPACE

namespace KDReports {
class ImageElementPrivate;

/**
 * The KDReports::ImageElement class represents an image in the report.
 */
class KDREPORTS_EXPORT ImageElement : public Element
{
public:
    /**
     * Creates a image element from the given pixmap.
     * This constructor can only be used in the GUI thread.
     */
    explicit ImageElement(const QPixmap &pixmap);
    /**
     * Creates a image element from the given image.
     * This constructor can be used from secondary threads as well.
     * \since 1.3
     */
    explicit ImageElement(const QImage &image);
    /**
     * Destroys this image element.
     */
    ~ImageElement() override;

    /**
     * Sets the pixmap for this image element.
     * This replaces the pixmap or image set previously.
     */
    void setPixmap(const QPixmap &pixmap);

    /**
     * \return the pixmap.
     * \since 1.4
     */
    QPixmap pixmap() const;

    /**
     * Sets a new image for this image element.
     * This replaces the pixmap or image set previously.
     */
    void setImage(const QImage &image);

    /**
     * \return the image.
     * \since 1.4
     */
    QImage image() const;

    /**
     * Copies a image element.
     */
    ImageElement(const ImageElement &other);
    /**
     * Copies the data from another image element.
     */
    ImageElement &operator=(const ImageElement &other);

    /**
     * Sets the width of the image.
     * The height of the image is calculated so that the aspect ratio is preserved.
     *
     * The default size, if %setWidth is not called, is the pixmap's size in pixels,
     * which might lead to a different size depending on the resolution of the
     * output device but avoids any resizing.
     * On the other hand, setWidth makes sure that the layout of the document
     * is always the same by resizing the pixmap if necessary.
     *
     * For an image that should take half of the width of the page, call setWidth(50, KDReports::Percent).
     *
     * Note that images inside table cells still have their width relative to the page, not to the cell.
     * For instance setWidth(10, KDReports::Percent) means that the image width will be 10% of the page width,
     * not 10% of the containing cell's width.
     */
    void setWidth(qreal width, Unit unit = Millimeters);

    /**
     * \return the width of the image.
     * \since 1.4
     */
    qreal width() const;

    /**
     * Sets the height of the image.
     * The width of the image is calculated so that the aspect ratio is preserved.
     * This is mutually exclusive with setWidth().
     * \since 1.1
     */
    void setHeight(qreal height, Unit unit = Millimeters);

    /**
     * \return the height of the image.
     * \since 1.4
     */
    qreal height() const;

    /**
     * \return the unit for the image size (percent or millimeters).
     * \since 1.4
     */
    Unit unit() const;

    /**
     * Set the unit for the image size (percent or millimeters).
     * \since 1.5
     */
    void setUnit(Unit unit);

    /**
     * Requests that the image be scaled so that it fills the page as much as
     * possible, while still preserving aspect ratio.
     * This is mutually exclusive with setHeight/setWidth.
     * \since 1.1
     */
    void setFitToPage();

    /**
     * \return if the image be scaled to fills the page.
     * \since 1.4
     */
    bool fitToPage() const;

    /**
     * Set the ID associated with this image element.
     * \since 1.4
     */
    void setId(const QString &id);

    /**
     * \return the ID associated with this image element.
     * \since 1.4
     */
    QString id() const;

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

private:
    std::unique_ptr<ImageElementPrivate> d;
};

}

#endif /* KDREPORTSIMAGEELEMENT_H */
