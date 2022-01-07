## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2021-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

import unittest
import importlib
import inspect

from config import TstConfig

class TestImportModules(unittest.TestCase):
    def test_importModules(self):
        m = importlib.import_module(TstConfig.bindingsNamespace + '.KDReports')
        moduleSymbols = []
        for t in inspect.getmembers(m):
            moduleSymbols.append(t[0])

        symbols = ['ChartElement', 'PageCount', 'Report']
        for symbol in symbols:
            self.assertIn(symbol, moduleSymbols)

if __name__ == '__main__':
    TstConfig.initLibraryPath()
    unittest.main()
