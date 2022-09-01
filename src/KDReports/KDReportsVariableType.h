/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#ifndef KDREPORTSVARIABLETYPE_H
#define KDREPORTSVARIABLETYPE_H

/** @namespace KDReports
 * This is the namespace that contains all %KDReports classes and some enums
 * that do not belong to a particular class.
 */

namespace KDReports {
/**
 * Type of variable
 */
enum VariableType
{
    PageNumber, ///< Page number
    PageCount, ///< Page count
    TextDate, ///< Current date in text format like "Thu Aug 18 2005", with translated
    ///  names but unlocalized order of names and numbers
    ISODate, ///< Current date in ISO 8601 format like "2005-08-18"
    LocaleDate, ///< Current date in locale-dependent format, deprecated in favour of SystemLocaleShortDate or SystemLocaleLongDate.
    TextTime, ///< Current time in text format like "13:42:59"
    ISOTime, ///< Current time in ISO 8601 format like "13:42:59"
    LocaleTime, ///< Current time in locale-dependent format

    SystemLocaleShortDate, ///< Current date formatted according to the system locale, short format, see Qt::SystemLocaleShortDate
    SystemLocaleLongDate, ///< Current date formatted according to the system locale, long format, see Qt::SystemLocaleLongDate
    DefaultLocaleShortDate, ///< Current date formatted according to the application locale, short format, see Qt::DefaultLocaleShortDate
    DefaultLocaleLongDate, ///< Current date formatted according to the application locale, long format, see Qt::DefaultLocaleLongDate

    NVariableTypes ///< @internal
};
}

#endif /* KDREPORTSVARIABLETYPE_H */
