/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#include "KDReportsHeader_p.h" // for variableValue and setVariableMarker
#include "KDReportsLayoutHelper_p.h" // mmToPixels
#include "KDReportsReportBuilder_p.h"
#include "KDReportsTextElement.h"

#include <QDebug>
#include <QTextBlock>

//@cond PRIVATE
KDReports::ReportBuilder::ReportBuilder(KDReports::TextDocumentData &contentDocument, const QTextCursor &cursor, KDReports::Report *report)
    : m_contentDocument(contentDocument)
    , m_cursor(cursor)
    , m_report(report)
{
}
//@endcond

void KDReports::ReportBuilder::addInlineElement(const Element &element)
{
    QTextCursor &cursor = this->cursor();
    cursor.beginEditBlock();
    // Save/restore the char format, otherwise insertHtml("<font size=\"+8\">My Title</font>")
    // leaves us with a large-font char format, which would affect subsequent insertText().
    // So: char format change must be done inside a single html element, not across.
    // This is also true for charts and images, and variables, which all set things into the char format.
    const QTextCharFormat origCharFormat = cursor.charFormat();
    element.build(*this);
    cursor.setCharFormat(origCharFormat);
    cursor.endEditBlock();
}

void KDReports::ReportBuilder::addBlockElement(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor)
{
    QTextCursor &cursor = this->cursor();
    cursor.beginEditBlock();

    if (!m_first) {
        cursor.insertBlock();
    } else {
        m_first = false;
    }

    // Set the default font again, the previous block should have no effect on this one
    QTextCharFormat charFormat = cursor.charFormat();
    charFormat.setFont(m_defaultFont);
    cursor.setCharFormat(charFormat);

    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(horizontalAlignment);
    setupBlockFormat(blockFormat);

    if (backgroundColor.isValid())
        blockFormat.setBackground(backgroundColor);

    cursor.setBlockFormat(blockFormat);

    element.build(*this);

    cursor.setCharFormat(charFormat); // restore, we don't want addElement(bold text) + addInline(normal text) to make the normal text bold
    cursor.endEditBlock();

#if 0 // DEBUG CODE for tab positions
    if (!m_tabPositions.isEmpty()) {
        QTextBlock block = cursor.document()->firstBlock();
        do {
            qDebug() << "addBlock: Looking at block" << block.blockNumber() << "tabs:" << block.blockFormat().tabPositions().count();
            block = block.next();
        } while ( block.isValid() );
    }
#endif
}

void KDReports::ReportBuilder::addVariable(KDReports::VariableType variable)
{
    QTextCursor &cursor = this->cursor();
    const int charPosition = cursor.position();
    // Don't ask for the value of PageCount yet -- it would create a documentlayout
    // which would make any later insertion into the textdocument much, much slower.
    const QString value = variable == KDReports::PageCount ? QStringLiteral("UNKNOWN YET") : variableValue(0 /*pageNumber*/, m_report, variable);
    KDReports::TextElement element(value);

    const QTextCharFormat origCharFormat = cursor.charFormat();

    // Keep the current font (KDRE-91).
    QTextCursor docCursor(&currentDocument());
    docCursor.setPosition(charPosition);
    cursor.setCharFormat(docCursor.charFormat());

    cursor.beginEditBlock();
    element.build(*this);
    cursor.endEditBlock();

    setVariableMarker(currentDocument(), charPosition, variable, value.length());

    cursor.setCharFormat(origCharFormat); // restore the orig format
}

void KDReports::ReportBuilder::addVerticalSpacing(qreal space)
{
    QTextCursor &cursor = this->cursor();
    cursor.beginEditBlock();

    if (!m_first) {
        cursor.insertBlock();
    } else {
        m_first = false;
    }

    QTextBlockFormat blockFormat;
    blockFormat.setTopMargin(KDReports::mmToPixels(space / 2));
    blockFormat.setBottomMargin(KDReports::mmToPixels(space / 2));
    cursor.setBlockFormat(blockFormat);
    QTextCharFormat charFormat;
    charFormat.setFontPointSize(1);
    cursor.setCharFormat(charFormat);
    cursor.insertText(QStringLiteral(" ")); // this ensures the point size 1 is actually used, making the paragraph much smaller
    cursor.endEditBlock();
}

//@cond PRIVATE
void KDReports::ReportBuilder::addInlineElementPublic(const Element &element)
{
    addInlineElement(element);
}

void KDReports::ReportBuilder::addBlockElementPublic(const Element &element, Qt::AlignmentFlag horizontalAlignment, const QColor &backgroundColor)
{
    addBlockElement(element, horizontalAlignment, backgroundColor);
}

void KDReports::ReportBuilder::addVariablePublic(KDReports::VariableType variable)
{
    addVariable(variable);
}

void KDReports::ReportBuilder::addVerticalSpacingPublic(qreal space)
{
    addVerticalSpacing(space);
}
//@endcond

void KDReports::ReportBuilder::addPageBreakPublic()
{
    QTextBlockFormat blockFormat = m_cursor.blockFormat();
    blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    m_cursor.setBlockFormat(blockFormat);
}

void KDReports::ReportBuilder::insertFragmentPublic(const QTextDocumentFragment &fragment)
{
    m_cursor.insertFragment(fragment);
}

void KDReports::ReportBuilder::setTabPositions(const QList<QTextOption::Tab> &tabs)
{
    m_tabPositions = tabs;
    for (QTextOption::Tab &tab : m_tabPositions) {
        tab.position = mmToPixels(tab.position);
    }
    m_contentDocument.setUsesTabPositions(true);
}

void KDReports::ReportBuilder::copyStateFrom(const ReportBuilder &parentBuilder)
{
    m_tabPositions = parentBuilder.m_tabPositions;
    m_leftMargin = parentBuilder.m_leftMargin;
    m_rightMargin = parentBuilder.m_rightMargin;
    m_topMargin = parentBuilder.m_topMargin;
    m_bottomMargin = parentBuilder.m_bottomMargin;
    m_defaultFont = parentBuilder.m_defaultFont;
}

QDebug operator<<(QDebug &dbg, QTextOption::Tab tab) // clazy says: pass by value, small enough
{
    static const char *types[] = {"LeftTab", "RightTab", "CenterTab", "DelimiterTab"};
    dbg.space() << '(' << types[tab.type] << tab.position << "px" << ')';
    return dbg;
}

//@cond PRIVATE
void KDReports::ReportBuilder::setParagraphMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    m_leftMargin = mmToPixels(left);
    m_topMargin = mmToPixels(top);
    m_rightMargin = mmToPixels(right);
    m_bottomMargin = mmToPixels(bottom);
}
//@endcond

void KDReports::ReportBuilder::setupBlockFormat(QTextBlockFormat &blockFormat) const
{
    blockFormat.setTabPositions(m_tabPositions);
    blockFormat.setLeftMargin(m_leftMargin);
    blockFormat.setRightMargin(m_rightMargin);
    blockFormat.setTopMargin(m_topMargin);
    blockFormat.setBottomMargin(m_bottomMargin);
}

int KDReports::ReportBuilder::currentPosition()
{
    return m_cursor.position();
}

QTextCharFormat::VerticalAlignment KDReports::ReportBuilder::toVerticalAlignment(Qt::Alignment alignment)
{
    switch (alignment & Qt::AlignVertical_Mask) {
    case Qt::AlignTop:
        return QTextCharFormat::AlignTop;
    case Qt::AlignBottom:
        return QTextCharFormat::AlignBottom;
    case Qt::AlignVCenter:
        return QTextCharFormat::AlignMiddle;
    case Qt::AlignBaseline:
        return QTextCharFormat::AlignBaseline;
    }
    return QTextCharFormat::AlignNormal;
}
