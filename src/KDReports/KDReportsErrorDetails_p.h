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

#ifndef KDREPORTSERRORDETAILS_P_H
#define KDREPORTSERRORDETAILS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the KD Reports API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QString>

namespace KDReports {
class HLineElementHandler;

class ErrorDetailsPrivate
{
public:
    ErrorDetailsPrivate()
        : m_hasError(false)
        , m_line(-1)
        , m_column(-1)
        , m_hlineHandler(nullptr)
    {
    }

    bool m_hasError;
    int m_line;
    int m_column;
    QString m_message;

    // Hack: storage for XmlElementHandler which lacks a d pointer
    HLineElementHandler *m_hlineHandler;
};

}

#endif /* KDREPORTSERRORDETAILS_P_H */
