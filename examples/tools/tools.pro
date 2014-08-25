TEMPLATE = lib

TARGET = kdreporttesttools

CONFIG(debug, debug|release) {
    !unix: TARGET = $${TARGET}d
}

# TESTTOOLS_STATICLIB needs to be defined for tools_export.h
staticlib {
  DEFINES += TESTTOOLS_STATICLIB
}

# For QAIM::reset()
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

DESTDIR = $${TOP_BUILD_DIR}/lib
# Workaround for visual studio integration
win32: DLLDESTDIR = $${TOP_BUILD_DIR}/bin

DEFINES += KDREPORT_BUILD_TESTTOOLS_LIB
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += TableModel.h
SOURCES += TableModel.cpp

unix {
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  UI_DIR = .ui
} else {
  MOC_DIR = _moc
  OBJECTS_DIR = _obj
  UI_DIR = _ui
}

