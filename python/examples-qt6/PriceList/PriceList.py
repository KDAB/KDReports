#!/usr/bin/env python

#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

''' PriceList Example '''

# pylint: disable=missing-class-docstring,missing-function-docstring

import sys

from TableModel import TableModel

from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap, QColor
from PySide6.QtWidgets import QApplication
from PyKDReportsQt6.KDReports import PreviewDialog, TextElement, TableElement, AutoTableElement
from PyKDReportsQt6.KDReports import Report, HeaderLocation, Unit, VariableType, ImageElement

try:
    import rc_price_list  # pylint: disable=unused-import
except ImportError:
    sys.exit("Oops.. rc_price_list needs to be generated first.\n"
             "Please run: rcc -g python PriceList.qrc -o rc_price_list.py\n"
             "(Make sure to use the rcc from the Qt version used to generate the bindings!)")


def main():
    QApplication(sys.argv)

    report = Report()
    report.setHeaderBodySpacing(10)  # mm
    report.setFooterBodySpacing(10)  # mm

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
    evenPagesHeader.addVariable(VariableType.PageNumber)
    evenPagesHeader.addInlineElement(TextElement(" / "))
    evenPagesHeader.addVariable(VariableType.PageCount)

    footer = report.footer()
    companyAddressElement = TextElement("Klarälvdalens Datakonsult AB\nRysktorp\nSE-68392 Hagfors\nSweden")
    footer.addElement(companyAddressElement, Qt.AlignRight)

    titleElement = TextElement("Price list example")
    titleElement.setPointSize(18)
    report.addElement(titleElement, Qt.AlignHCenter)

    report.addVerticalSpacing(10)  # 1 cm

    titleElementColor = QColor(204, 204, 255)

    # A text element with the title above a table
    # Note that the background color is not just behind the text (as setBackground() would do),
    # but behind the whole paragraph, up until the right margin of the page.
    tableTitleElement = TextElement("Network Peripherals")
    tableTitleElement.setBold(True)
    report.addElement(tableTitleElement, Qt.AlignLeft, titleElementColor)

    table1 = TableModel()
    table1.setDataHasVerticalHeaders(False)
    table1.loadFromCSV(":/table1")
    autoTableElement1 = AutoTableElement(table1)
    autoTableElement1.setWidth(100, Unit.Percent)
    report.addElement(autoTableElement1)

    report.addVerticalSpacing(5)

    # Notice how elements can be copied and modified
    # This way, we use the same font attributes for all title elements
    tableTitleElement2 = tableTitleElement
    tableTitleElement2.setText("Printer Cartridges")
    report.addElement(tableTitleElement2, Qt.AlignLeft, titleElementColor)

    table2 = TableModel()
    table2.setDataHasVerticalHeaders(False)
    table2.loadFromCSV(":/table2")
    autoTableElement2 = AutoTableElement(table2)
    autoTableElement2.setWidth(100, Unit.Percent)
    report.addElement(autoTableElement2)

    # and again, on the second page
    report.addPageBreak()
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
    headerColor = QColor(218, 218, 218)
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
    headerCell1.addElement(ImageElement(systemPixmap))
    headerCell1.addInlineElement(TextElement(" Item"))
    headerCell2 = tableElement.cell(1, 1)
    headerCell2.setBackground(headerColor)
    expected = TextElement("Expected")
    expected.setItalic(True)
    expected.setBackground(QColor(153, 153, 153))  # note that this background only applies to this element
    headerCell2.addElement(expected)
    headerCell2.addInlineElement(TextElement(" shipping time"))

    # Data in rows 2 and 3
    tableElement.cell(2, 0).addElement(TextElement("Network Peripherals"))
    tableElement.cell(2, 1).addElement(TextElement("4 days"))
    tableElement.cell(3, 0).addElement(TextElement("Printer Cartridges"))
    tableElement.cell(3, 1).addElement(TextElement("3 days"))

    report.addElement(tableElement)

    preview = PreviewDialog(report)
    return preview.exec()


if __name__ == "__main__":
    main()
