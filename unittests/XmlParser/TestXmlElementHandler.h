/****************************************************************************
** Copyright (C) 2007-2019 Klaralvdalens Datakonsult AB.  All rights reserved.
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

#ifndef TESTXMLELEMENTHANDLER_H
#define TESTXMLELEMENTHANDLER_H

#include <QStringList>
#include "KDReportsXmlElementHandlerV2.h"

class TestXmlElementHandler : public KDReports::XmlElementHandlerV2
{
public:
    explicit TestXmlElementHandler( KDReports::Report& report );
    QStringList callbacks() const { return cb; }

    bool startReport( KDReports::Report& report, QDomElement& xmlElement ) override;
    bool startHeader( KDReports::Header& header, QDomElement& xmlElement ) override;
    void endHeader( KDReports::Header& header, const QDomElement& xmlElement ) override;
    bool startFooter( KDReports::Footer& footer, QDomElement& xmlElement ) override;
    void endFooter( KDReports::Footer& footer, const QDomElement& xmlElement ) override;
    bool textElement( KDReports::TextElement& textElement, QDomElement& xmlElement ) override;
    bool htmlElement( KDReports::HtmlElement& htmlElement, QDomElement& xmlElement ) override;
    bool startTableElement( KDReports::TableElement& tableElement, QDomElement& xmlElement ) override;
    bool endTableElement( KDReports::TableElement& tableElement, QDomElement& xmlElement ) override;
    bool startCell( KDReports::Cell& cell, QDomElement& xmlElement ) override;
    bool endCell( KDReports::Cell& cell, QDomElement& xmlElement ) override;
    bool autoTableElement( KDReports::AutoTableElement& tableElement, QDomElement& xmlElement ) override;
    bool chartElement( KDReports::ChartElement& chartElement, QDomElement& xmlElement ) override;
    bool imageElement( KDReports::ImageElement& imageElement, QDomElement& xmlElement ) override;
    bool pageBreak( QDomElement& xmlElement ) override;
    bool hLineElement( KDReports::HLineElement& textElement, QDomElement& xmlElement ) override;
    void customElement( const QDomElement& xmlElement ) override;
    bool variable(KDReports::VariableType &type, QDomElement &xmlElement) override;
    bool vspace(int &size, QDomElement &xmlElement) override;
    bool paragraphMargin(qreal &left, qreal &top, qreal &right, qreal &bottom, const QDomElement& xmlElement ) override;
    bool tabs(QList<QTextOption::Tab> &tabs, const QDomElement& xmlElement ) override;
    void endReport( KDReports::Report& report, const QDomElement& xmlElement ) override;
private:
    QStringList cb;
    KDReports::Report& mReport;
};

#endif /* TESTXMLELEMENTHANDLER_H */

