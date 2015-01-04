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

namespace KDReports
{
class HLineElementHandler;

class ErrorDetailsPrivate
{
public:
    ErrorDetailsPrivate()
        : m_hasError( false ), m_line(-1), m_column(-1), m_hlineHandler(0)
    {
    }

    bool m_hasError;
    int m_line;
    int m_column;
    QString m_message;

    // Hack: storage for XmlElementHandler which lacks a d pointer
    HLineElementHandler* m_hlineHandler;
};

}


#endif /* KDREPORTSERRORDETAILS_P_H */

