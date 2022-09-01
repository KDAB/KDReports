#!/usr/bin/env python

#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

''' Show a Print Preview Dialog Example '''

# pylint: disable=missing-class-docstring,missing-function-docstring

import sys

from PySide2.QtCore import Qt
from PySide2.QtWidgets import QApplication
from PyKDReports.KDReports import PreviewDialog, Report, TextElement


def main():
    QApplication(sys.argv)

    # create a report
    report = Report()

    # Add a text element for the title
    titleElement = TextElement("Hello World!")
    titleElement.setPointSize(18)
    report.addElement(titleElement, Qt.AlignHCenter)

    # add 20 mm of vertical space:
    report.addVerticalSpacing(20)

    # add some more text
    textElement = TextElement("This is a report generated with KDReports")
    report.addElement(textElement, Qt.AlignLeft)

    # show a print preview
    preview = PreviewDialog(report)
    return preview.exec_()


if __name__ == "__main__":
    main()
