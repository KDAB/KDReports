##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

  # copy from environment:
  isEmpty( KDCHARTDIR ):KDCHARTDIR=$$(KDCHARTDIR)
  !isEmpty( KDCHARTDIR ) {
    unix:isEmpty(QMAKE_EXTENSION_SHLIB) {
      static:QMAKE_EXTENSION_SHLIB=a
      else {
        macx:QMAKE_EXTENSION_SHLIB=dylib
        else:QMAKE_EXTENSION_SHLIB=so
      }
    }
    unix:!exists( $$KDCHARTDIR/lib/libkdchart.$$QMAKE_EXTENSION_SHLIB ):error( "Cannot find libkdchart.$$QMAKE_EXTENSION_SHLIB in $$KDCHARTDIR/lib" )
    unix:!exists( $$KDCHARTDIR/include/KDChart/KDChartGlobal ):error( "Cannot find KDChartGlobal in $$KDCHARTDIR/include/KDChart" )

    KDCHART_LIB_TYPE_SUFFIX = ""
    win32 {
        CONFIG(debug, debug|release) {
            KDCHART_LIB_TYPE_SUFFIX="d"
        }
    }

    LIBS += -L$$KDCHARTDIR/lib
    win32 {
      LIBS += -lkdchart$$KDCHART_LIB_TYPE_SUFFIX
    } else {
      !isEmpty(QMAKE_LFLAGS_RPATH):LIBS += $$QMAKE_LFLAGS_RPATH$$KDCHARTDIR/lib
      LIBS += -lkdchart
    }

    INCLUDEPATH += $$KDCHARTDIR/include/KDChart
    DEPENDPATH += $$KDCHARTDIR/include/KDChart

    CONFIG += have_kdchart
    DEFINES += HAVE_KDCHART

  } else:equals( WARN_IF_NOT_FOUND, "1" ) {
    message( "WARNING: kdchart not found. Please set KDCHARTDIR either as an environment variable or on the qmake command line if you want kdchart support")
  }
