TEMPLATE = lib

include(../../variables.pri)

TARGET = $${KDREPORTSTESTTOOLSLIB}

# TESTTOOLS_STATICLIB needs to be defined for tools_export.h
staticlib {
  DEFINES += TESTTOOLS_STATICLIB
}

# For QAIM::reset()
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

DEFINES += KDREPORT_BUILD_TESTTOOLS_LIB
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += TableModel.h
SOURCES += TableModel.cpp
