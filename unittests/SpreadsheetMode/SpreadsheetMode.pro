KDREPORTS_PATH = $$PWD/../..

include( $$KDREPORTS_PATH/unittests/unittests.pri )

DESTDIR = .
HEADERS = 
SOURCES = SpreadsheetMode.cpp

test.target = test
test.commands = ./$(TARGET)
test.depends = $(TARGET)
QMAKE_EXTRA_TARGETS += test
