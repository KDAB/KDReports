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

import os
import sys

class TstConfig(object):
    bindingsNamespace = "PyKDReports"

    def initLibraryPath():
        if sys.platform == 'win32' and sys.version_info[0] == 3 and sys.version_info[1] >= 8:
            os.add_dll_directory("@CMAKE_RUNTIME_OUTPUT_DIRECTORY@")
