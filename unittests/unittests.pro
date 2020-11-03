##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

TEMPLATE = subdirs
SUBDIRS += ElementCopying TextDocument Headers PageLayout XmlParser TableBreakingLogic SpreadsheetMode InThread

test.target=test
unix:!macx {
    LIB_PATH=$$TOP_BUILD_DIR/lib:\$\$LD_LIBRARY_PATH
    test.commands=for d in $${SUBDIRS}; do cd "\$$d" && LD_LIBRARY_PATH=$$LIB_PATH $(MAKE) test && cd .. || exit 1; done
}
unix:macx {
    LIB_PATH=$$TOP_BUILD_DIR/lib:\$\$DYLD_LIBRARY_PATH
    test.commands=for d in $${SUBDIRS}; do ( cd "\$$d" && DYLD_LIBRARY_PATH=$$LIB_PATH $(MAKE) test ) || exit 1; done
}
win32 {
    WIN_BINDIR=
    debug_and_release {
        WIN_BINDIR=release
    }

    RUNTEST=$${TOP_SOURCE_DIR}/unittests/runTest.bat
    RUNTEST=$$replace(RUNTEST, /, \\)
    LIB_PATH=$$TOP_BUILD_DIR/lib
    LIB_PATH=$$replace(LIB_PATH, /, \\)
    test.commands=for %d in ($${SUBDIRS}); do $$RUNTEST $$LIB_PATH "%d" $$WIN_BINDIR || exit 1; done
}

unix:test.commands=for d in $${SUBDIRS}; do cd "\$$d" && $(MAKE) test && cd .. || exit 1; done
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test

