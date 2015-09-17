TEMPLATE = lib

# Use the filename "kdreportsd1.dll" (or "kdreportsd1.lib") on Windows
# to avoid name clashes between debug/non-debug versions of the
# KD Reports library:
TARGET = kdreports
CONFIG(debug, debug|release):!unix:TARGET = $${TARGET}d

WARN_IF_NOT_FOUND = 1
include($${TOP_SOURCE_DIR}/kdchart.pri)

# Workaround for visual studio integration
DESTDIR = $${TOP_BUILD_DIR}/lib
win32:DLLDESTDIR = $${TOP_BUILD_DIR}/bin
QT += xml

contains( $$list($$[QT_VERSION]), 5.* ): QT += widgets printsupport

include(../variables.pri)

FORMS = KDReports/previewdialogbase.ui \
    KDReports/tablebreakingdialogbase.ui
RESOURCES = $${TOP_SOURCE_DIR}/pics/pics.qrc
PUBLICHEADERS = KDReports/KDReportsReport.h \
    KDReports/KDReportsElement.h \
    KDReports/KDReportsChartElement.h \
    KDReports/KDReportsTextElement.h \
    KDReports/KDReportsHtmlElement.h \
    KDReports/KDReportsTableElement.h \
    KDReports/KDReportsAutoTableElement.h \
    KDReports/KDReportsAbstractTableElement.h \
    KDReports/KDReportsImageElement.h \
    KDReports/KDReportsHLineElement.h \
    KDReports/KDReportsPreviewDialog.h \
    KDReports/KDReportsPreviewWidget.h \
    KDReports/KDReportsHeader.h \
    KDReports/KDReportsCell.h \
    KDReports/KDReportsFrame.h \
    KDReports/KDReportsGlobal.h \
    KDReports/KDReportsXmlElementHandler.h \
    KDReports/KDReportsXmlElementHandlerV2.h \
    KDReports/KDReportsVariableType.h \
    KDReports/KDReportsUnit.h \
    KDReports/KDReportsTableBreakingSettingsDialog.h \
    KDReports/KDReportsMainTable.h \
    KDReports/KDReportsXmlHelper.h \
    KDReports/KDReportsErrorDetails.h

PRIVATEHEADERS = KDReports/KDReportsHeader_p.h \
    KDReports/KDReportsReport_p.h \
    KDReports/KDReportsXmlParser_p.h \
    KDReports/KDReportsErrorDetails_p.h \
    KDReports/KDReportsAbstractReportLayout_p.h \
    KDReports/KDReportsTextDocReportLayout_p.h \
    KDReports/KDReportsSpreadsheetReportLayout_p.h \
    KDReports/KDReportsHLineTextObject_p.h \
    KDReports/KDReportsChartTextObject_p.h \
    KDReports/KDReportsTableLayout_p.h

HEADERS = $$PUBLICHEADERS \
    $$PRIVATEHEADERS \

SOURCES = KDReports/KDReportsReport.cpp \
    KDReports/KDReportsElement.cpp \
    KDReports/KDReportsChartElement.cpp \
    KDReports/KDReportsTextElement.cpp \
    KDReports/KDReportsHtmlElement.cpp \
    KDReports/KDReportsTableElement.cpp \
    KDReports/KDReportsAutoTableElement.cpp \
    KDReports/KDReportsAbstractTableElement.cpp \
    KDReports/KDReportsImageElement.cpp \
    KDReports/KDReportsReportBuilder.cpp \
    KDReports/KDReportsPreviewDialog.cpp \
    KDReports/KDReportsPreviewWidget.cpp \
    KDReports/KDReportsLayoutHelper.cpp \
    KDReports/KDReportsHeader.cpp \
    KDReports/KDReportsTextDocument.cpp \
    KDReports/KDReportsTextDocumentData.cpp \
    KDReports/KDReportsCell.cpp \
    KDReports/KDReportsFrame.cpp \
    KDReports/KDReportsXmlParser.cpp \
    KDReports/KDReportsTableBreakingSettingsDialog.cpp \
    KDReports/KDReportsXmlElementHandler.cpp \
    KDReports/KDReportsErrorDetails.cpp \
    KDReports/KDReportsTableBreakingLogic.cpp \
    KDReports/KDReportsFontScaler.cpp \
    KDReports/KDReportsHLineTextObject.cpp \
    KDReports/KDReportsHLineElement.cpp \
    KDReports/KDReportsChartTextObject.cpp \
    KDReports/KDReportsMainTable.cpp \
    KDReports/KDReportsAbstractReportLayout.cpp \
    KDReports/KDReportsTextDocReportLayout.cpp \
    KDReports/KDReportsSpreadsheetReportLayout.cpp \
    KDReports/KDReportsTableLayout.cpp \
    KDReports/KDReportsXmlElementHandlerV2.cpp \
    KDReports/KDReportsXmlHelper.cpp
DEFINES += KDREPORTS_BUILD_KDREPORTS_LIB

target.path = $$INSTALL_PREFIX/lib$$LIB_SUFFIX
INSTALLS += target

# Translations

LANGUAGES = fr_FR
TRANSLATIONS = $$join( LANGUAGES, ".ts $${TOP_SOURCE_DIR}/translations/$${TARGET}_", "$${TOP_SOURCE_DIR}/translations/$${TARGET}_", ".ts" )
#qm.target = $$member( $$list( $$split(TRANSLATIONS) ) )
qm.target = qm
qm.depends = $$SOURCES $$HEADERS $$FORMS
qm.commands = lupdate $$qm.depends -ts $$TRANSLATIONS

QMAKE_EXTRA_TARGETS += qm
#POST_TARGETDEPS += $$qm.target
