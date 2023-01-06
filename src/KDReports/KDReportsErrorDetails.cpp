/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsErrorDetails.h"
#include "KDReportsErrorDetails_p.h"
#include <QObject>

KDReports::ErrorDetails::ErrorDetails()
    : d(new ErrorDetailsPrivate)
{
}

KDReports::ErrorDetails::ErrorDetails(const QString &message)
    : d(new ErrorDetailsPrivate)
{
    setDriverMessage(message);
}

QString KDReports::ErrorDetails::message() const
{
    if (!hasError()) {
        return QString();
    }

    if (d->m_line != -1) {
        return QObject::tr("Error on line %1, column %2: %3").arg(line()).arg(column()).arg(driverMessage());
    } else {
        return driverMessage();
    }
}

int KDReports::ErrorDetails::line() const
{
    return d->m_line;
}

int KDReports::ErrorDetails::column() const
{
    return d->m_column;
}

bool KDReports::ErrorDetails::hasError() const
{
    return d->m_hasError;
}

QString KDReports::ErrorDetails::driverMessage() const
{
    return d->m_message;
}

void KDReports::ErrorDetails::setLine(int line)
{
    d->m_hasError = true;
    d->m_line = line;
}

void KDReports::ErrorDetails::setColumn(int column)
{
    d->m_hasError = true;
    d->m_column = column;
}

void KDReports::ErrorDetails::setDriverMessage(const QString &message)
{
    d->m_hasError = true;
    d->m_message = message;
}

KDReports::ErrorDetails::~ErrorDetails()
{
}

KDReports::ErrorDetails::ErrorDetails(const ErrorDetails &other)
    : d(new ErrorDetailsPrivate(*other.d))
{
}

KDReports::ErrorDetails &KDReports::ErrorDetails::operator=(const ErrorDetails &other)
{
    ErrorDetails copy(other);
    swap(copy);
    return *this;
}

void KDReports::ErrorDetails::swap(ErrorDetails &other)
{
    qSwap(d, other.d);
}
