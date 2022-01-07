##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

set(FORMS
  KDReports/previewdialogbase.ui
  KDReports/tablebreakingdialogbase.ui
)
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../pics/pics.qrc)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/KDReports)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/KDReports)
endif()

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
  KDReports/KDReportsXmlHelper.cpp
)

add_library(kdreports ${KDReports_LIBRARY_MODE} ${SOURCES})
set_target_properties(kdreports PROPERTIES OUTPUT_NAME "kdreports${${PROJECT_NAME}_LIBRARY_QTID}")
if(${PROJECT_NAME}_STATIC)
  target_compile_definitions(kdreports PUBLIC KDREPORTS_STATICLIB)
else()
  target_compile_definitions(kdreports PRIVATE -DKDREPORTS_BUILD_KDREPORTS_LIB)
endif()

target_link_libraries(kdreports ${QT_LIBRARIES})
if(KDChart_FOUND)
  target_link_libraries(kdreports KDChart::kdchart)
endif()
set_target_properties(kdreports PROPERTIES DEFINE_SYMBOL KDREPORTS_BUILD_KDREPORTS_LIB)
set_target_properties(kdreports PROPERTIES
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  VERSION ${${PROJECT_NAME}_VERSION}
)
target_include_directories(kdreports INTERFACE "$<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/KDReports${${PROJECT_NAME}_LIBRARY_QTID}/>")

#version libraries on Windows
if(WIN32)
  set(postfix ${${PROJECT_NAME}_SOVERSION})
  string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_BUILD_TYPE)
  if(${UPPER_BUILD_TYPE} MATCHES "^DEBUG")
    string(CONCAT postfix ${postfix} "d")
    set_target_properties(kdreports PROPERTIES DEBUG_POSTFIX ${postfix})
  else()
    set_target_properties(kdreports PROPERTIES ${UPPER_BUILD_TYPE}_POSTFIX ${postfix})
  endif()
endif()

# Generate library version files
include(ECMSetupVersion)
ecm_setup_version(
  ${${PROJECT_NAME}_VERSION}
  VARIABLE_PREFIX KDREPORTS
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/KDReports/KDReportsVersion.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReports${${PROJECT_NAME}_LIBRARY_QTID}ConfigVersion.cmake"
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/KDReports/KDReportsVersion.h" DESTINATION ${INSTALL_INCLUDE_DIR}/KDReports${${PROJECT_NAME}_LIBRARY_QTID})

if(${PROJECT_NAME}_IS_ROOT_PROJECT)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KDReportsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/install/KDReports${${PROJECT_NAME}_LIBRARY_QTID}Config.cmake"
    INSTALL_DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports${${PROJECT_NAME}_LIBRARY_QTID}"
    PATH_VARS INSTALL_INCLUDE_DIR
  )

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/install/KDReports${${PROJECT_NAME}_LIBRARY_QTID}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReports${${PROJECT_NAME}_LIBRARY_QTID}ConfigVersion.cmake"
    DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports${${PROJECT_NAME}_LIBRARY_QTID}"
  )

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
    KDReports/KDReportsVariableType.h
    KDReports/KDReportsUnit.h
    KDReports/KDReportsTableBreakingSettingsDialog.h
    KDReports/KDReportsMainTable.h
    KDReports/KDReportsXmlHelper.h
    KDReports/KDReportsErrorDetails.h
    KDReports/KDReports.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/KDReports${${PROJECT_NAME}_LIBRARY_QTID}
  )

  install(TARGETS kdreports EXPORT KDReportsTargets
    RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
    LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
    ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR}
  )

  if(MSVC AND NOT KDReports_STATIC)
    install(FILES "$<TARGET_PDB_FILE_DIR:kdreports>/$<TARGET_PDB_FILE_NAME:kdreports>" DESTINATION ${INSTALL_LIBRARY_DIR} CONFIGURATIONS Debug RelWithDebInfo)
  endif()
endif()
