include( $${TOP_SOURCE_DIR}/examples/examples.pri )
include( $${TOP_SOURCE_DIR}/kdchart.pri )

QT     += sql

HEADERS = ResultModel.h
SOURCES = ReferenceReport.cpp ResultModel.cpp
RESOURCES = ReferenceReport.qrc
