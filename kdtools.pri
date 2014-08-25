  # copy from environment:
  isEmpty( KDTOOLSDIR ):KDTOOLSDIR=$$(KDTOOLSDIR)
  !isEmpty( KDTOOLSDIR ) {
    unix:isEmpty(QMAKE_EXTENSION_SHLIB) {
      static:QMAKE_EXTENSION_SHLIB=a
      else {
        macx:QMAKE_EXTENSION_SHLIB=dylib
        else:QMAKE_EXTENSION_SHLIB=so
      }
    }
    unix:!exists( $$KDTOOLSDIR/lib/libKDToolsGui.$$QMAKE_EXTENSION_SHLIB ):error( "Cannot find libKDToolsGui.$$QMAKE_EXTENSION_SHLIB in $$KDTOOLSDIR/lib" )

    KDCHART_LIB_TYPE_SUFFIX = ""
    win32 {
        CONFIG(debug, debug|release) {
            KDTOOLS_LIB_TYPE_SUFFIX="d"
        }
    }

    LIBS += -L$$KDTOOLSDIR/lib
    win32 {
      LIBS += -lKDToolsGui$$KDTOOLS_LIB_TYPE_SUFFIX
      unittests:LIBS += -lKDUnitTest$$KDTOOLS_LIB_TYPE_SUFFIX
    } else {
      !isEmpty(QMAKE_LFLAGS_RPATH):LIBS += $$QMAKE_LFLAGS_RPATH$$KDTOOLSDIR/lib
      LIBS += -lKDToolsGui
      unittests:LIBS += -lKDUnitTest
    }

    INCLUDEPATH += $$KDTOOLSDIR/include
    DEPENDPATH += $$KDTOOLSDIR/include

    CONFIG += have_kdtools
    DEFINES += HAVE_KDTOOLS

  } else:equals( builddir, $$top_builddir ) {
    message( "WARNING: kdtools not found. Please set KDTOOLSDIR either as an environment variable or on the qmake command line if you want kdtools support")
  }
