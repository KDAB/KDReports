"""
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
"""
import sys

from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QApplication
from PyKDReportsQt6.KDReports import PreviewDialog, Report, TextElement, TableElement, AutoTableElement, HeaderLocation, Unit, VariableType

import rc_price_list

import TableModel

def main():
    app = QApplication(sys.argv)

    report = Report()
    report.setHeaderBodySpacing(10) # mm
    report.setFooterBodySpacing(10) # mm

    header = report.header(HeaderLocation.OddPages)
    kdab = QPixmap(":/kdab_small.jpg")
    imageElement = ImageElement(kdab)
    imageElement.setWidth(40, Unit.Percent)

    header.addElement(imageElement)
    header.addVariable(VariableType.PageNumber)
    header.addInlineElement(TextElement(" / "))
    header.addVariable(VariableType.PageCount)
    header.addInlineElement(TextElement(", Date: "))
    header.addVariable(VariableType.TextDate)
    header.addInlineElement(TextElement(", Time: "))
    header.addVariable(VariableType.TextTime)

    evenPagesHeader = report.header(HeaderLocation.EvenPages)
    evenPagesHeader.addElement(imageElement)
    evenPagesHeader.addInlineElement(TextElement("Even pages header: "))
    evenPagesHeader.addVariable(VariableType.PageNumber);
    evenPagesHeader.addInlineElement(TextElement(" / "))
    evenPagesHeader.addVariable(VariableType.PageCount)

    footer = report.footer()
    companyAddressElement = TextElement("Klarälvdalens Datakonsult AB\nRysktorp\nSE-68392 Hagfors\nSweden")
    footer.addElement(companyAddressElement, Qt.AlignRight)

    titleElement = TextElement("Price list example")
    titleElement.setPointSize(18)
    report.addElement(titleElement, Qt.AlignHCenter)

    report.addVerticalSpacing(10) # 1 cm

    titleElementColor = QColor(204, 204, 255)

    # A text element with the title above a table
    # Note that the background color is not just behind the text (as setBackground() would do),
    # but behind the whole paragraph, up until the right margin of the page.
    tableTitleElement = TextElement("Network Peripherals")
    tableTitleElement.setBold(true)
    report.addElement(tableTitleElement, Qt.AlignLeft, titleElementColor)

    table1 = TableModel()
    table1.setDataHasVerticalHeaders(false)
    table1.loadFromCSV(":/table1")
    autoTableElement1 = AutoTableElement(table1)
    autoTableElement1.setWidth(100, Unit.Percent)
    report.addElement(autoTableElement1)

    report.addVerticalSpacing(5)

    # Notice how elements can be copied and modified
    # This way, we use the same font attributes for all title elements
    tableTitleElement2 = tableTitleElement;
    tableTitleElement2.setText("Printer Cartridges")
    report.addElement(tableTitleElement2, Qt.AlignLeft, titleElementColor)

    table2 = TableModel;
    table2.setDataHasVerticalHeaders(false)
    table2.loadFromCSV(":/table2")
    autoTableElement2 = AutoTableElement(table2)
    autoTableElement2.setWidth(100, Unit.Percent)
    report.addElement(autoTableElement2)

    # and again, on the second page
    report.addPageBreak();
    report.addElement(tableTitleElement, Qt.AlignLeft, titleElementColor)
    report.addElement(autoTableElement1)
    report.addVerticalSpacing(5)
    report.addElement(tableTitleElement2, Qt.AlignLeft, titleElementColor)
    report.addElement(autoTableElement2)

    report.addVerticalSpacing(5)

    # ===========================================================================
    # Another kind of table, where the data comes from code and not from a model:
    # ===========================================================================
    tableElement = TableElement()
    tableElement.setHeaderRowCount(2)
    tableElement.setPadding(3)
    headerColor = QColor("#DADADA")
    # Merged header in row 0
    topHeader = tableElement.cell(0, 0)
    topHeader.setColumnSpan(2)
    topHeader.setBackground(headerColor)
    topHeader.addElement(TextElement("TableElement example"), Qt.AlignHCenter)

    # Normal header in row 1
    headerCell1 = tableElement.cell(1, 0)
    headerCell1.setBackground(headerColor)
    # This would look better if centered vertically. This feature is only available since
    # Qt-4.3 though (QTextCharFormat::AlignMiddle)
    systemPixmap = QPixmap(":/system.png")
    headerCell1.addElement(ImageElement(systemPixmap));
    headerCell1.addInlineElement(TextElement(" Item"))
    headerCell2 = tableElement.cell(1, 1);
    headerCell2.setBackground(headerColor);
    expected = TextElement("Expected")
    expected.setItalic(True)
    expected.setBackground(QColor("#999999")) # note that this background only applies to this element
    headerCell2.addElement(expected)
    headerCell2.addInlineElement(TextElement(" shipping time"))

    # Data in rows 2 and 3
    tableElement.cell(2, 0).addElement(TextElement("Network Peripherals"))
    tableElement.cell(2, 1).addElement(TextElement("4 days"))
    tableElement.cell(3, 0).addElement(TextElement("Printer Cartridges"))
    tableElement.cell(3, 1).addElement(TextElement("3 days"))

    report.addElement(tableElement);

    preview = PreviewDialog(report)
    return preview.exec_()

if __name__ == "__main__":
    main()
