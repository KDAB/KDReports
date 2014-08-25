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

