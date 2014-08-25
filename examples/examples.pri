# This file is included by all of the examples' *.pro files.

# KDREPORTSLIB and KDREPORTSTESTTOOLSLIB are defined by the toplevel kdreports.pro and stored into .qmake.cache

# KDREPORTS_STATICLIB needs to be defined for KDReportsGlobal.h
static {
    DEFINES += KDREPORTS_STATICLIB
    DEFINES += TESTTOOLS_STATICLIB
}

# For more qt4/qt5 compatibility
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

contains( $$list($$[QT_VERSION]), 5.* ): QT += widgets printsupport

# The src/KDReports include path is only for the use of private classes in unittests
INCLUDEPATH += \
            $$TOP_SOURCE_DIR/src/KDReports \
            $$TOP_SOURCE_DIR/include/KDReports \
            $$TOP_SOURCE_DIR/examples/tools
DEPENDPATH += \
            $$TOP_SOURCE_DIR/src/KDReports \
            $$TOP_SOURCE_DIR/include/KDReports \
            $$TOP_SOURCE_DIR/examples/tools
LIBS        += -L$${TOP_BUILD_DIR}/lib -l$$KDREPORTSLIB -l$$KDREPORTSTESTTOOLSLIB
!isEmpty(QMAKE_LFLAGS_RPATH):LIBS += $$QMAKE_LFLAGS_RPATH$${TOP_BUILD_DIR}/lib

DESTDIR = $${TOP_BUILD_DIR}/bin

# The dependencies need to be linked explicitly, at least on Mac OS X, for instance libkdchart.
# This is all done by kdchart.pri
include (../kdchart.pri)
QT += xml

include(../variables.pri)
DEFINES -= QT_NO_CAST_FROM_ASCII
