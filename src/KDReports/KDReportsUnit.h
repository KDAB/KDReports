/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSUNIT_H
#define KDREPORTSUNIT_H

namespace KDReports {

/**
 * Units used when setting a size.
 */
enum Unit
{
    Millimeters, ///< Millimeters (the default)
    Percent ///< Percentage of the text width, i.e. the page width minus margins
};

}

#endif /* KDREPORTSUNIT_H */
