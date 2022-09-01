#
# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2021-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

# dot should come with Doxygen find_package(Doxygen)
if(DOXYGEN_DOT_EXECUTABLE)
  set(HAVE_DOT "YES")
else()
  set(HAVE_DOT "NO")
  message(STATUS "Unable to provide inheritance diagrams for the API documentation. To fix, install the graphviz project from https://www.graphviz.org")
endif()

# latex
set(HAVE_LATEX "NO")
find_package(LATEX)
set_package_properties(LATEX PROPERTIES
  TYPE OPTIONAL
  DESCRIPTION "A document preparation system"
  URL "https://www.latex-project.org"
  PURPOSE "Provides for building a PS or PDF version of the API documentation"
)
if(LATEX_FOUND)
  if(MAKEINDEX_COMPILER)
    set(HAVE_LATEX "YES")
  else()
    message(STATUS "The LaTeX makeindex compiler could not be located. Unable to generate the buildsystem for LaTex documentation generation.")
  endif()
endif()

# qhelpgenerator
find_program(QHELPGEN_EXECUTABLE qhelpgenerator
  HINTS ${QT_INSTALL_BINS}
)
if(QHELPGEN_EXECUTABLE)
  set(HAVE_QHELPGEN "YES")
else()
  set(HAVE_QHELPGEN "NO")
  message(STATUS "Unable to generate the API documentation in qch format. To fix, install the qthelpgenerator program which comes with Qt.")
endif()

find_file(QDOC_QTCORE_TAG qtcore.tags
  HINTS ${QT_INSTALL_DOCS}/qtcore
  HINTS ${QT_INSTALL_DATA}/doc/qtcore
)
set(QDOC_TAG_DIR "<QDOC_TAG_DIR_not_found>")
if(QDOC_QTCORE_TAG)
  get_filename_component(QDOC_TAG_DIR ${QDOC_QTCORE_TAG} DIRECTORY)
  get_filename_component(QDOC_TAG_DIR ${QDOC_TAG_DIR} DIRECTORY)
endif()

set(KDCHART_TAG "")
if(KDChart_FOUND)
  #temporary hack to find the kdchart tags file
  get_filename_component(KDCHART_TAG ${KDChart_DIR} DIRECTORY)
  get_filename_component(KDCHART_TAG ${KDCHART_TAG} DIRECTORY) #up to lib
  get_filename_component(KDCHART_TAG ${KDCHART_TAG} DIRECTORY) #up to install_prefix
  set(KDCHART_TAG "${KDCHART_TAG}/share/doc/KDChart/kdchart.tags")
  set(KDCHART_TAG "${KDCHART_TAG}=https://docs.kdab.com/kdchart/latest/")
endif()

file(GLOB _dox_deps *.dox *.html)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

#apidox generation using doxygen
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
)

add_custom_command(
  OUTPUT ${DOXYGEN_OUTPUT_DIR}/qch/kdreports-api.qch
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile

  #copy some files by-hand that are referred to by the markdown README
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/INSTALL.txt ${DOXYGEN_OUTPUT_DIR}/html
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/LICENSES ${DOXYGEN_OUTPUT_DIR}/html/LICENSES
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples ${DOXYGEN_OUTPUT_DIR}/html/examples

  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}/html/docs
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}/html/docs/manual
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/docs/manual/kdreports.pdf ${DOXYGEN_OUTPUT_DIR}/html/docs/manual

  DEPENDS ${_dox_deps} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_custom_target(kdreports-api.qch ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/qch/kdreports-api.qch)
add_custom_target(docs DEPENDS kdreports-api.qch)

set(QCH_INSTALL_DIR ${INSTALL_DOC_DIR} CACHE STRING "Install location of Qt Assistant help files.")
install(FILES ${DOXYGEN_OUTPUT_DIR}/qch/kdreports-api.qch DESTINATION ${QCH_INSTALL_DIR})
install(FILES ${DOXYGEN_OUTPUT_DIR}/kdreports.tags DESTINATION ${INSTALL_DOC_DIR})
