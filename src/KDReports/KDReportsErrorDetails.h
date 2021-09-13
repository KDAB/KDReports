/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORTSERRORDETAILS_H
#define KDREPORTSERRORDETAILS_H

#include "KDReportsGlobal.h"
#include <QtAlgorithms>
#include <memory>

namespace KDReports {
class ErrorDetailsPrivate;
class XmlElementHandler;

/**
 * The ErrorDetails class is used for transferring errors when parsing XML
 */
class KDREPORTS_EXPORT ErrorDetails
{
public:
    ErrorDetails();
    explicit ErrorDetails(const QString &message);
    ErrorDetails(const ErrorDetails &other);

    ~ErrorDetails();
    inline ErrorDetails &operator=(const ErrorDetails &other);

    /**
     * @return a message including line and column number if available.
     */
    QString message() const;

    void setLine(int line);
    void setColumn(int column);

    /**
     * Set the lowlevel message. This excludes line and/or column numbers.
     */
    void setDriverMessage(const QString &message);

    int line() const;
    int column() const;

    /**
     * @return the lowlevel message, without any line or column numbers included.
     */
    QString driverMessage() const;

    bool hasError() const;

    inline void swap(ErrorDetails &other); // krazy:exclude=inline
private:
    friend class XmlElementHandler;
    std::unique_ptr<ErrorDetailsPrivate> d;
};

void ErrorDetails::swap(ErrorDetails &other)
{
    qSwap(d, other.d);
}

ErrorDetails &ErrorDetails::operator=(const ErrorDetails &other)
{
    ErrorDetails copy(other);
    swap(copy);
    return *this;
}

} // namespace KDReports

QT_BEGIN_NAMESPACE
template <>
inline void qSwap(KDReports::ErrorDetails &lhs, KDReports::ErrorDetails &rhs)
{
    lhs.swap(rhs);
}
QT_END_NAMESPACE

#endif /* KDREPORTSERRORDETAILS_H */
