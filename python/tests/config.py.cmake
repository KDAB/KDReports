# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

import os
import sys

class TstConfig(object):
    bindingsNamespace = "PyKDReports"

    def initLibraryPath():
        if sys.platform == 'win32' and sys.version_info[0] == 3 and sys.version_info[1] >= 8:
            os.add_dll_directory("@CMAKE_RUNTIME_OUTPUT_DIRECTORY@")
