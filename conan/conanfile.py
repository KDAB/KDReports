###############################################################################
## Copyright (C) 2007-2019 Klaralvdalens Datakonsult AB.  All rights reserved.
##
## This file is part of the KD Reports library.
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
##
## This file may be distributed and/or modified under the terms of the
## GNU Lesser General Public License version 2.1 and version 3 as published by the
## Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
##
## This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
## WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
##
## Contact info@kdab.com if any conditions of this licensing are not
## clear to you.
##
###############################################################################

from conans import ConanFile, CMake, tools


class KdreportsConan(ConanFile):
    name = "KDReports"
    version = "1.8.0"
    license = ("https://raw.githubusercontent.com/KDAB/KDReports/kdreports-{0}-release/LICENSE.txt"
               "https://raw.githubusercontent.com/KDAB/KDReports/kdreports-{0}-release/LICENSE.GPL.txt,"
               "https://raw.githubusercontent.com/KDAB/KDReports/kdreports-{0}-release/LICENSE.LGPL.txt").format(version)
    author = "Klaralvdalens Datakonsult AB (KDAB) info@kdab.com"
    url = "https://github.com/KDAB/KDReports.git"
    description = "KD Reports is a Qt tool that lets you easily create printable reports by providing all of the necessary features for a variety of applications."
    generators = "cmake"

    def requirements(self):
        # Kdab uses a custom qt build
        # To build qt with with extra modules enabled use qt package from:
        #   https://github.com/bincrafters/conan-qt
        # Build the command:
        #   conan create -ks -o qt:qtsvg=True -o qt:qtdeclarative=True -o qt:qtremoteobjects=True -o qt:qtscxml=True . 5.13.2@kdab/stable
        self.requires("qt/5.13.2@kdab/stable")
        self.requires("KDChart/2.6.50@kdab/stable")

    def source(self):
        git = tools.Git(folder="")
        git.clone(self.url)
        git.checkout("kdreports-%s-release"%self.version)

    def configure(self):
        # Use kdab flags to match qt package hash
        # ~$ conan create -ks -o qt:qttools=True -o qt:qtsvg=True -o qt:qtdeclarative=True -o qt:qtremoteobjects=True -o qt:qtscxml=True . 5.13.2@kdab/stable
        self.options["qt"].qtsvg = True
        self.options["qt"].qtdeclarative = True
        self.options["qt"].qtremoteobjects = True
        self.options["qt"].qtscxml = True
        self.options["qt"].qttools = True

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.env_info.CMAKE_PREFIX_PATH.append(self.package_folder)
