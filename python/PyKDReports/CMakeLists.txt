##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##

# Auto-Genereate files every class will have his cpp/h files
set(PyKDReports_SRC
    # individual classes
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_abstracttableelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_autotableelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_cell_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_chartelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_element_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_errordetails_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_frame_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_header_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_hlineelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_htmlelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_imageelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_maintable_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_previewdialog_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_previewwidget_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_report_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_reportbuilder_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_tablebreakingsettingsdialog_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_tableelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_textelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_xmlelementhandlerv2_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_xmlelementhandler_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_xmlhelper_wrapper.cpp

    # global module wrapper
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_module_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KDReports/kdreports_python.h
)

# includes necessary to parse and build the classes specified on typesystem
set(PyKDReports_include_paths
    $<JOIN:$<TARGET_PROPERTY:kdreports,INTERFACE_INCLUDE_DIRECTORIES>,${PATH_SEP}>
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/src/KDReports
)

# A list of paths where shiboken should look for typesystem
set(PyKDReports_typesystem_paths
    # PySide path, this variable was exposed by FindPySide2.cmake
    ${PYSIDE_TYPESYSTEMS}
)

# Include flags/path that will be set in 'target_include_directories'
set(PyKDReports_target_include_directories
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/src/KDReports
)

# Libraries that will be necessary to link the target, this will used in the command 'target_link_libraries'
set(PyKDReports_target_link_libraries
    kdreports
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
)

# changes on these files should trigger a new generation
set(PyKDReports_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/kdreports_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glue.cpp
)

create_python_bindings(
  "KDReports"
  "${PyKDReports_typesystem_paths}"
  "${PyKDReports_include_paths}"
  "${PyKDReports_SRC}"
  "${PyKDReports_target_include_directories}"
  "${PyKDReports_target_link_libraries}"
  ${CMAKE_CURRENT_SOURCE_DIR}/kdreports_global.h
  ${CMAKE_CURRENT_SOURCE_DIR}/typesystem_kdreports.xml
  "${PyKDReports_DEPENDS}"
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Make module import from build dir work
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py.cmake ${CMAKE_CURRENT_BINARY_DIR}/__init__.py @ONLY)