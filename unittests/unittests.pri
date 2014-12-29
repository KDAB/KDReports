#    Note: KDREPORTS_PATH is set in the calling .pro file
include (../examples/examples.pri)

contains( $$list($$[QT_VERSION]), 5.* ): QT += testlib
else: CONFIG += qtestlib

