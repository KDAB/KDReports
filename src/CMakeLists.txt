set(QT_USE_QTXML TRUE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(FORMS
  KDReports/previewdialogbase.ui
  KDReports/tablebreakingdialogbase.ui
)

qt5_wrap_ui(KDREPORTS_UI ${FORMS})

set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../pics/pics.qrc)
qt5_add_resources(KDREPORTS_RES ${RESOURCES})

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/KDReports)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/KDReports)
endif()

include(ECMGenerateHeaders)
ecm_generate_headers(kdreports_HEADERS
  ORIGINAL
    CAMELCASE
  HEADER_NAMES
    KDReportsReport
    KDReportsElement
    KDReportsChartElement
    KDReportsTextElement
    KDReportsHtmlElement
    KDReportsTableElement
    KDReportsAutoTableElement
    KDReportsAbstractTableElement
    KDReportsImageElement
    KDReportsHLineElement
    KDReportsPreviewDialog
    KDReportsPreviewWidget
    KDReportsHeader
    KDReportsCell
    KDReportsFrame
    KDReportsGlobal
    KDReportsXmlElementHandler
    KDReportsXmlElementHandlerV2
    KDReportsVariableType
    KDReportsUnit
    KDReportsTableBreakingSettingsDialog
    KDReportsMainTable
    KDReportsXmlHelper
    KDReportsErrorDetails
  OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports
  RELATIVE
    KDReports
  COMMON_HEADER
    KDReports
)
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReports "#include \"KDReports.h\"\n")

install(FILES
  ${kdreports_HEADERS}
  KDReports/KDReportsReport.h
  KDReports/KDReportsElement.h
  KDReports/KDReportsChartElement.h
  KDReports/KDReportsTextElement.h
  KDReports/KDReportsHtmlElement.h
  KDReports/KDReportsTableElement.h
  KDReports/KDReportsAutoTableElement.h
  KDReports/KDReportsAbstractTableElement.h
  KDReports/KDReportsImageElement.h
  KDReports/KDReportsHLineElement.h
  KDReports/KDReportsPreviewDialog.h
  KDReports/KDReportsPreviewWidget.h
  KDReports/KDReportsHeader.h
  KDReports/KDReportsCell.h
  KDReports/KDReportsFrame.h
  KDReports/KDReportsGlobal.h
  KDReports/KDReportsXmlElementHandler.h
  KDReports/KDReportsXmlElementHandlerV2.h
  KDReports/KDReportsVariableType.h
  KDReports/KDReportsUnit.h
  KDReports/KDReportsTableBreakingSettingsDialog.h
  KDReports/KDReportsMainTable.h
  KDReports/KDReportsXmlHelper.h
  KDReports/KDReportsErrorDetails.h
  KDReports/KDReports.h
DESTINATION ${INSTALL_INCLUDE_DIR}/KDReports
)

set(SOURCES
  KDReports/KDReportsReport.cpp
  KDReports/KDReportsElement.cpp
  KDReports/KDReportsChartElement.cpp
  KDReports/KDReportsTextElement.cpp
  KDReports/KDReportsHtmlElement.cpp
  KDReports/KDReportsTableElement.cpp
  KDReports/KDReportsAutoTableElement.cpp
  KDReports/KDReportsAbstractTableElement.cpp
  KDReports/KDReportsImageElement.cpp
  KDReports/KDReportsReportBuilder.cpp
  KDReports/KDReportsPreviewDialog.cpp
  KDReports/KDReportsPreviewWidget.cpp
  KDReports/KDReportsLayoutHelper.cpp
  KDReports/KDReportsHeader.cpp
  KDReports/KDReportsTextDocument.cpp
  KDReports/KDReportsTextDocumentData.cpp
  KDReports/KDReportsCell.cpp
  KDReports/KDReportsFrame.cpp
  KDReports/KDReportsXmlParser.cpp
  KDReports/KDReportsTableBreakingSettingsDialog.cpp
  KDReports/KDReportsXmlElementHandler.cpp
  KDReports/KDReportsErrorDetails.cpp
  KDReports/KDReportsTableBreakingLogic.cpp
  KDReports/KDReportsFontScaler.cpp
  KDReports/KDReportsHLineTextObject.cpp
  KDReports/KDReportsHLineElement.cpp
  KDReports/KDReportsChartTextObject.cpp
  KDReports/KDReportsMainTable.cpp
  KDReports/KDReportsAbstractReportLayout.cpp
  KDReports/KDReportsTextDocReportLayout.cpp
  KDReports/KDReportsSpreadsheetReportLayout.cpp
  KDReports/KDReportsTableLayout.cpp
  KDReports/KDReportsXmlElementHandlerV2.cpp
  KDReports/KDReportsXmlHelper.cpp
)

add_library(kdreports ${KDReports_LIBRARY_MODE} ${SOURCES} ${KDREPORTS_UI} ${KDREPORTS_RES})
if(${PROJECT_NAME}_STATIC)
  target_compile_definitions(kdreports PUBLIC KDREPORTS_STATICLIB)
else()
  target_compile_definitions(kdreports PRIVATE -DKDREPORTS_BUILD_KDREPORTS_LIB)
endif()

target_link_libraries(kdreports ${QT_LIBRARIES})
if (KDChart_FOUND)
    target_link_libraries(kdreports KDChart::kdchart)
endif()
set_target_properties(kdreports PROPERTIES DEFINE_SYMBOL KDREPORTS_BUILD_KDREPORTS_LIB)
set_target_properties(kdreports PROPERTIES
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  VERSION ${${PROJECT_NAME}_VERSION}
)
target_include_directories(kdreports INTERFACE "$<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/KDReports/>")

#version libraries on Windows
if(WIN32)
  set(postfix ${${PROJECT_NAME}_SOVERSION})
  set(CMAKE_RELEASE_POSTFIX ${postfix})
  set_target_properties(kdreports PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
  #append 'd' to debug libraries
  string(CONCAT postfix ${postfix} "d")
  set(CMAKE_DEBUG_POSTFIX ${postfix})
  set_target_properties(kdreports PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endif()

install(TARGETS kdreports EXPORT KDReportsTargets
  RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
  LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
  ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR}
)
if(MSVC AND NOT KDReports_STATIC)
  install(FILES "$<TARGET_PDB_FILE_DIR:kdreports>/$<TARGET_PDB_FILE_NAME:kdreports>" DESTINATION ${INSTALL_LIBRARY_DIR} CONFIGURATIONS Debug RelWithDebInfo)
endif()
