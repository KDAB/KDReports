#!/usr/bin/env python

#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

''' Labels Example '''

# pylint: disable=missing-class-docstring,missing-function-docstring

import sys

from PySide2.QtCore import Qt, QAbstractTableModel, QSizeF
from PySide2.QtGui import QFont
from PySide2.QtWidgets import QApplication, QMessageBox
from PyKDReports.KDReports import PreviewDialog, Report, AutoTableElement


def inchToMM(inch):
    return inch * 25.4


class LabelModel(QAbstractTableModel):
    def __init__(self, cellWidth, cellHeight, parent=None):
        super().__init__(parent)
        self.cellWidth = cellWidth
        self.cellHeight = cellHeight

    def rowCount(self, parent):  # pylint: disable=no-self-use
        if parent.isValid():
            return 0
        return 10

    def columnCount(self, parent):  # pylint: disable=no-self-use
        if parent.isValid():
            return 0
        return 3

    def data(self, index, role):
        del index
        if role == Qt.DisplayRole:
            return "Klarälvdalens Datakonsult AB\n" + "Rysktorp\n" + "Sweden\n"
        if role == Qt.SizeHintRole:
            return QSizeF(self.cellWidth, self.cellHeight)
        if role == Qt.TextAlignmentRole:
            return Qt.AlignCenter

        return None


def main():
    QApplication(sys.argv)

    # This example shows how to print address labels
    # on the popular Avery label size of 2.625 in x 1 in which is the white label #5160.
    # It is available as 30 labels per page and is used for addressing and mailing
    # purposes.

    pageWidth = inchToMM(8.5)
    pageHeight = inchToMM(11)
    cellWidth = inchToMM(2.625)
    cellHeight = inchToMM(1)
    rowCount = 10
    columnCount = 3

    report = Report()
    report.setReportMode(Report.SpreadSheet)
    mainTable = report.mainTable()

    model = LabelModel(cellWidth, cellHeight)
    autoTable = AutoTableElement(model)
    autoTable.setHorizontalHeaderVisible(False)
    autoTable.setVerticalHeaderVisible(False)
    autoTable.setBorder(0)
    mainTable.setAutoTableElement(autoTable)

    font = QFont()
    font.setPointSize(10)
    report.setDefaultFont(font)

    # Margins calculation
    # These labels fit in a Letter (8.5 in x 11 in) format like this:
    # 0.31 + 2.625 * 3 + 0.31 =~ 8.5 horizontally
    verticalPageMargin = (pageHeight - cellHeight * rowCount) / 2 - 0.01
    horizontalPageMargin = (pageWidth - cellWidth * columnCount) / 2
    report.setMargins(verticalPageMargin, horizontalPageMargin, verticalPageMargin, horizontalPageMargin)
    report.setFixedRowHeight(cellHeight)

    # To show a print preview:
    preview = PreviewDialog(report)
    if preview.exec_():
        result = preview.result()
        if result == PreviewDialog.SavedSuccessfully:
            QMessageBox.information(0, "Report saved", "Success saving to " + preview.savedFileName())
        elif result == PreviewDialog.SaveError:
            QMessageBox.information(0, "Error", "Error while saving to " + preview.savedFileName())

    return 0


if __name__ == "__main__":
    main()
