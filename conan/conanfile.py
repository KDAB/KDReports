######################################################################/
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2020-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

from conans import ConanFile, CMake, tools

class KdReportsConan(ConanFile):
    name = "kdreports"
    version = "2.1.1"
    license = ("https://raw.githubusercontent.com/KDAB/KDReports/master/LICENSES/GPL-2.0-only.txt,"
               "https://raw.githubusercontent.com/KDAB/KDReports/master/LICENSES/GPL-3.0-only.txt,"
               "https://raw.githubusercontent.com/KDAB/KDReports/master/LICENSES/LGPL-2.1-only.txt,"
               "https://raw.githubusercontent.com/KDAB/KDReports/master/LICENSES/LGPL-3.0-only.txt,")
    author = "Klaralvdalens Datakonsult AB (KDAB) info@kdab.com"
    url = "https://github.com/KDAB/KDReports.git"
    description = "A Qt library for creating printable reports"
    generators = "cmake"
    options = dict({
        "build_static": [True, False],
        "build_examples": [True, False],
        "build_tests": [True, False],
        "build_python_bindings": [True, False],
        "python_bindings_install_prefix": "ANY"
    })
    default_options = dict({
        "build_static": False,
        "build_examples": True,
        "build_tests": False,
        "build_python_bindings": False,
        "python_bindings_install_prefix": "ANY"
    })
    settings = "build_type"

    def source(self):
        git = tools.Git(folder="")
        git.clone(self.url)
        #We want cmake support, so use master for now
        #git.checkout("kdreports-%s-release"%self.version)

    def configure(self):
        self.options["kdreports"].shared = (not self.options.build_static)

    def build(self):
        self.cmake = CMake(self)
        self.cmake.definitions["KDReports_STATIC"] = self.options.build_static
        self.cmake.definitions["KDReports_EXAMPLES"] = self.options.build_examples
        self.cmake.definitions["KDReports_TESTS"] = self.options.build_tests
        self.cmake.definitions["KDReports_PYTHON_BINDINGS"] = self.options.build_python_bindings
        if self.options.python_bindings_install_dir:
            self.cmake.definitions["KDReports_PYTHON_BINDINGS_INSTALL_PREFIX"] = self.options.python_bindings_install_dir
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
