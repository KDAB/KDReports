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
enum VariableType {
    PageNumber,  ///< Page number
    PageCount,   ///< Page count
    TextDate,    ///< Current date in text format like "Thu Aug 18 2005", with translated
                 ///  names but unlocalized order of names and numbers
    ISODate,     ///< Current date in ISO 8601 format like "2005-08-18"
    LocaleDate,  ///< Current date in locale-dependent format, deprecated in favour of SystemLocaleShortDate or SystemLocaleLongDate.
    TextTime,    ///< Current time in text format like "13:42:59"
    ISOTime,     ///< Current time in ISO 8601 format like "13:42:59"
    LocaleTime,  ///< Current time in locale-dependent format

    SystemLocaleShortDate, ///< Current date formatted according to the system locale, short format, see Qt::SystemLocaleShortDate
    SystemLocaleLongDate, ///< Current date formatted according to the system locale, long format, see Qt::SystemLocaleLongDate
    DefaultLocaleShortDate, ///< Current date formatted according to the application locale, short format, see Qt::DefaultLocaleShortDate
    DefaultLocaleLongDate, ///< Current date formatted according to the application locale, long format, see Qt::DefaultLocaleLongDate

    NVariableTypes ///< @internal
};

}

#endif /* KDREPORTSVARIABLETYPE_H */
