"""
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

from PySide2.QtCore import QAbstractTableModel, QFile, Qt, QIODevice

def resizeArray(array, newSize):
    newArray = array
    for i in range(newSize - len(array)):
        newArray.append(0)
    return newArray

class TableModel(QAbstractTableModel):

    def __init__(self, parent = None):
        super().__init__(parent)
        self.m_dataHasHorizontalHeaders = True
        self.m_dataHasVerticalHeaders = True
        self.m_supplyHeaderData = True
        self.m_rows = []
        self.m_horizontalHeaderData = []
        self.m_verticalHeaderData = []

    def rowCount(self, parentIndex):
        return len(self.m_rows)

    def columnCount(self, parentIndex):
        if len(self.m_rows) == 0:
            return 0
        else:
            return len(self.m_rows[0])
          
    def data(self, index, role):
        # FIXME kdchart queries (-1, -1) for empty models
        if (index.row() == -1 or index.column() == -1):
            print("TableModel::data: row: ", index.row(), ", column: ", index.column(), ", rowCount: ", self.rowCount(), ", columnCount: ", self.columnCount())
            print("TableModel::data: FIXME fix kdchart views to not query model data for invalid indices!")
            return None

        if (role == Qt.DisplayRole or role == Qt.EditRole):
            return self.m_rows[index.row()][index.column()]
        else:
            return None

    def headerData(self, section, orientation, role):
        result = None

        if (role in [Qt.DisplayRole, Qt.EditRole]):
            if (self.m_supplyHeaderData):
              if (orientation == Qt.Horizontal): # column header data:
                  if (section < len(self.m_horizontalHeaderData)):
                      result = self.m_horizontalHeaderData[section]
                  else:
                      # row header data:
                      if (section < len(self.m_verticalHeaderData.count)):
                          result = self.m_verticalHeaderData[section];
        return result

    def setData(self, index, value, role):
        if (role == Qt.EditRole):
            self.m_rows[index.row()][index.column()] = value
            self.dataChanged.emit(index, index)
            return True
        return False

    def loadFromCSV(self, filename):
        file = QFile(filename)
        data = []

        if (file.exists() and file.open(QIODevice.ReadOnly)):
            self.beginResetModel()
            while file.atEnd() == False:
                line = file.readLine()
                data.append(line)

            if len(data) > 0:
                self.m_rows = resizeArray(self.m_rows, len(data) -1) 
                for row in range(len(data)):
                    parts = data[row].split(',')

                    values = resizeArray([], len(parts))
                    for column  in range(len(parts)):
                        # get rid of trailing or leading whitespaces and quotes:
                        cell = parts[column].simplified()

                        if cell.startsWith("\""):
                            cell = cell[1:]

                        if cell.endsWith("\""):
                            cell = cell[:-1]


                        if (row == 0 and self.m_dataHasHorizontalHeaders): # interpret the first row as column headers:
                            self.m_horizontalHeaderData.append(cell)
                        else:
                            if (column == 0 and self.m_dataHasVerticalHeaders): # interpret first column as row headers:
                                self.m_verticalHeaderData.append(cell)
                            else:
                                value = cell
                                try:
                                    # interpret cell values as floating point:
                                    value = float(cell)
                                except ValueError:
                                    value = cell

                                if self.m_dataHasVerticalHeaders:
                                    destColumn = column - 1
                                else:
                                    destColumn = column

                                values[destColumn] = value
                    if (row > 0):
                        if self.m_dataHasHorizontalHeaders:
                            destRow = row - 1
                        else:
                            destRow = row
                        self.m_rows[destRow] = values
            else:
                self.m_rows = []

            self.endResetModel()
            return True
        else:
            print("TableModel::loadFromCSV: file", filename, "does not exist / or could not be opened")
            return False

    def clear(self):
        self.beginResetModel()
        self.m_rows = []
        self.endResetModel()

    def setDataHasHorizontalHeaders(self, value):
        self.m_dataHasHorizontalHeaders = value

    def setDataHasVerticalHeaders(self, value):
        self.m_dataHasVerticalHeaders = value

    def setSupplyHeaderData(self, value):
        self.m_supplyHeaderData = value

