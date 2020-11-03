/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2011-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

#ifndef KDREPORTSXMLELEMENTHANDLERV2_H
#define KDREPORTSXMLELEMENTHANDLERV2_H

#include "KDReportsXmlElementHandler.h"
#include "KDReportsVariableType.h"
#include <QTextOption>

namespace KDReports {

class HLineElement;

/**
  * This handler handle more item than XmlElementHandler.
  * @see KDReports::XmlElementHandler
  * \since  1.4
  */
class KDREPORTS_EXPORT XmlElementHandlerV2 : public XmlElementHandler //krazy:exclude=dpointer
{
public:
    XmlElementHandlerV2();
    ~XmlElementHandlerV2() override;

    /**
     * Called after parsing "variable".
     */
    virtual bool variable( KDReports::VariableType &type, QDomElement& xmlElement );

    /**
     * Called after parsing "vspace"
     * Can be used to modify vertical space size
     * Returning false skip the vertical space
     */
    virtual bool vspace( int &size, QDomElement& xmlElement );

    /**
     * Called after parsing a "hline".
     * Returning false skips the whole line. Can be used to change
     * hline attributes.
     */
    virtual bool hLineElement( KDReports::HLineElement& hLineElement, QDomElement& xmlElement );

    /**
     * Called after parsing a "paragraph-margins".
     * Returning false skips the paragraph margin.
     */
    virtual bool paragraphMargin( qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement& xmlElement );

    /**
      * Called after parsing a "tabs"
      * Returning false skips tabs
      */
    virtual bool tabs(QList<QTextOption::Tab> &tabs, const QDomElement& xmlElement );
};

}

#endif // KDREPORTSXMLELEMENTHANDLERV2_H
