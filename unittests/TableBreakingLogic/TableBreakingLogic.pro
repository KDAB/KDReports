KDREPORTS_PATH = $$PWD/../..

include( $$KDREPORTS_PATH/unittests/unittests.pri )

DESTDIR = .
HEADERS = 
SOURCES = TableBreakingTest.cpp

# Shared lib: class not exported, compile it again into the unit test
# Static lib: class accessible, compiling it again would lead to "duplicate symbol" on Mac OS X
!static: SOURCES += $$KDREPORTS_PATH/src/KDReports/KDReportsTableBreakingLogic.cpp

test.target = test
test.commands = ./$(TARGET)
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test
