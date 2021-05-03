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

__all__ = ['KDReports']

# Preload PySide2 libraries to avoid missing libraries while loading KDReports
try:
    from PySide@PYSIDE_MAJOR_VERSION@ import QtCore
    # Create a alias for PySide module so we can use a single import in source files
    import PySide@PYSIDE_MAJOR_VERSION@
    sys.modules["PySide"] = PySide@PYSIDE_MAJOR_VERSION@
except Exception:
    print("Failed to load PySide")
    raise

