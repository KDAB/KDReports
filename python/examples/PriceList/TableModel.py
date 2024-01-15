#!/usr/bin/env python

#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

''' Table Model for the PriceList Example '''

# pylint: disable=missing-class-docstring,missing-function-docstring

from PySide2.QtCore import QAbstractTableModel, QFile, Qt, QIODevice


def resizeArray(array, newSize):
    newArray = array
    for _ in range(newSize - len(array)):
        newArray.append(0)
    return newArray


class TableModel(QAbstractTableModel):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.dataHasHorizontalHeaders = True
        self.dataHasVerticalHeaders = True
        self.supplyHeaderData = True
        self.rows = []
        self.horizontalHeaderData = []
        self.verticalHeaderData = []

    def rowCount(self, parentIndex):
        del parentIndex
        return len(self.rows)

    def columnCount(self, parentIndex):
        del parentIndex
        if len(self.rows) == 0:
            return 0
        return len(self.rows[0])

    def data(self, index, role):
        # FIXME kdchart queries (-1, -1) for empty models
        if (index.row() == -1 or index.column() == -1):
            print("TableModel::data: row: ", index.row(), ", column: ", index.column(),
                  ", rowCount: ", self.rowCount(None), ", columnCount: ", self.columnCount(None))
            print("TableModel::data: FIXME fix kdchart views to not query model data for invalid indices!")
            return None

        if role in [Qt.DisplayRole, Qt.EditRole]:
            return self.rows[index.row()][index.column()]
        return None

    def headerData(self, section, orientation, role):
        result = None

        if role in [Qt.DisplayRole, Qt.EditRole]:
            if self.supplyHeaderData:
                if orientation == Qt.Horizontal:  # column header data:
                    if section < len(self.horizontalHeaderData):
                        result = self.horizontalHeaderData[section]
                    else:
                        # row header data:
                        if section < len(self.verticalHeaderData.count):
                            result = self.verticalHeaderData[section]
        return result

    def setData(self, index, value, role):
        if role == Qt.EditRole:
            self.rows[index.row()][index.column()] = value
            self.dataChanged.emit(index, index)  # pylint: disable=no-member
            return True
        return False

    def loadFromCSV(self, filename):
        file = QFile(filename)
        data = []

        if file.exists() and file.open(QIODevice.ReadOnly):
            self.beginResetModel()
            while file.atEnd() is False:
                line = file.readLine()
                data.append(line)

            if len(data) > 0:
                self.rows = resizeArray(self.rows, len(data) - 1)
                for row in range(len(data)):  # pylint: disable=consider-using-enumerate
                    parts = data[row].split(',')

                    values = resizeArray([], len(parts))
                    for column in range(len(parts)):  # pylint: disable=consider-using-enumerate
                        # get rid of trailing or leading whitespaces and quotes:
                        cell = parts[column].simplified()

                        if cell.startsWith("\""):
                            cell = cell[1:]

                        if cell.endsWith("\""):
                            cell = cell[:-1]

                        if (row == 0 and self.dataHasHorizontalHeaders):  # interpret the first row as column headers:
                            self.horizontalHeaderData.append(cell)
                        else:
                            # interpret first column as row headers:
                            if (column == 0 and self.dataHasVerticalHeaders):
                                self.verticalHeaderData.append(cell)
                            else:
                                value = cell
                                try:
                                    # interpret cell values as floating point:
                                    value = float(cell)
                                except ValueError:
                                    value = cell

                                if self.dataHasVerticalHeaders:
                                    destColumn = column - 1
                                else:
                                    destColumn = column

                                values[destColumn] = value
                    if row > 0:
                        if self.dataHasHorizontalHeaders:
                            destRow = row - 1
                        else:
                            destRow = row
                        self.rows[destRow] = values
            else:
                self.rows = []

            self.endResetModel()
            return True
        print("TableModel::loadFromCSV: file", filename, "does not exist / or could not be opened")
        return False

    def clear(self):
        self.beginResetModel()
        self.rows = []
        self.endResetModel()

    def setDataHasHorizontalHeaders(self, value):
        self.dataHasHorizontalHeaders = value

    def setDataHasVerticalHeaders(self, value):
        self.dataHasVerticalHeaders = value

    def setSupplyHeaderData(self, value):
        self.supplyHeaderData = value
