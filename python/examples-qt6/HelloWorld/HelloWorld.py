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
from PySide6.QtWidgets import QApplication
from PyKDReportsQt6.KDReports import PreviewDialog, Report, TextElement


def main():
    app = QApplication(sys.argv)

    # create a report
    report = Report();

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

