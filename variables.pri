CONFIG += qt warn_on

exists( g++.pri ):include( g++.pri )

DEFINES += USE_EXCEPTIONS QT_FATAL_ASSERT

DEFINES += QT_NO_STL QT_NO_CAST_TO_ASCII QBA_NO_CAST_TO_VOID QBA_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

QT += xml

solaris-cc:DEFINES += SUN7

win32-msvc*:QMAKE_CXXFLAGS += /GR /EHsc /wd4251

unix:!macx:QMAKE_LFLAGS += -Wl,-no-undefined

CONFIG += depend_includepath

contains(TEMPLATE, lib) {
  DESTDIR = $${TOP_BUILD_DIR}/lib
}
# Putting all examples in the bin directory makes them easier to run (no PATH needed)
# but breaks the examples that load from .xml files. We could put them into a .qrc,
# but then people trying the binary version of kdreports wouldn't be able to edit the .xml
# to try out various features.
#contains(TEMPLATE, app) {
#  DESTDIR = $$PWD/bin
#}

static {
  DEFINES += KDREPORTS_STATICLIB
  DEFINES += KDCHART_STATICLIB
} else {
  DEFINES += KDCHART_SHAREDLIB
  contains(TEMPLATE, lib) {
    win32 {
      DLLDESTDIR = $${TOP_BUILD_DIR}/bin
      CONFIG += dll
    }
  }
}

unix {
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  UI_DIR = .ui
} else {
  MOC_DIR = _moc
  OBJECTS_DIR = _obj
  UI_DIR = _ui
}
