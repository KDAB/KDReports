# Not used by KDReports itself. This is for use in other projects.
# Include this file directly, or copy the file into your project.

# copy from environment:
  isEmpty( KDREPORTSDIR ):KDREPORTSDIR="$$(KDREPORTSDIR)"
  !isEmpty( KDREPORTSDIR ) {
    unix:isEmpty(QMAKE_EXTENSION_SHLIB):QMAKE_EXTENSION_SHLIB=so
    unix:!exists( $$KDREPORTSDIR/lib/libkdreports.$$QMAKE_EXTENSION_SHLIB ):error( "Cannot find libkdreports.$$QMAKE_EXTENSION_SHLIB in $KDREPORTSDIR/lib" )
    #win32:!exists( $$KDREPORTSDIR/lib/kdreports.lib ):error( "Cannot find kdreports.lib in $KDREPORTSDIR/lib" )
    unix:!exists( $$KDREPORTSDIR/include/KDReports ):error( "Cannot find KDReports in $KDREPORTSDIR/include/KDReports" )

    LIBS += -L$$KDREPORTSDIR/lib
    win32*:!static {
      CONFIG(debug, debug|release) {
        LIBS += -lkdreportsd1
      } else {
        LIBS += -lkdreports1
      }
    } else {
      !isEmpty(QMAKE_RPATH):LIBS += $$QMAKE_RPATH$$KDREPORTSDIR/lib
      LIBS += -lkdreports
    }

    INCLUDEPATH += $$KDREPORTSDIR/include $$KDREPORTSDIR/include/KDReports
    DEPENDPATH += $$KDREPORTSDIR/include

    CONFIG += have_kdreports
    DEFINES += HAVE_KDREPORTS

  } else:equals( builddir, $$top_builddir ) {
    message( "WARNING: kdreports not found. Please set KDREPORTSDIR either as an environment variable or on the qmake command line if you want kdreports support")
  }
