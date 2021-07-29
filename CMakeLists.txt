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

# Pass the following variables to cmake to control the build:
# (See INSTALL.txt for more information)
#
# -DKDReports_QT6=[true|false]
#  Build against Qt6 rather than Qt5
#  Default=false (Qt5 will be used even if Qt6 is available)
#
# -DKDReports_STATIC=[true|false]
#  Build static libraries
#  Default=false
#
# -DKDReports_TESTS=[true|false]
#  Build the test harness.
#  Default=false
#
# -DKDReports_EXAMPLES=[true|false]
#  Build the examples.
#  Default=true
#
# -DKDReports_PYTHON_BINDINGS=[true|false]
#  Build/Generate python bindings.  Always false for Debug builds
#  (If your shiboken or pyside is installed in a non-standard locations
#  try passing the SHIBOKEN_CUSTOM_PREFIX and PYSIDE2_CUSTOM_PREFIX variables.)
#  Default=false
#
# -DKDReports_PYTHON_BINDINGS_INSTALL_PREFIX=[path]
#  Alternative install path for Python bindings.
#  Default=CMAKE_INSTALL_PREFIX
#
cmake_minimum_required(VERSION 3.1)
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()
if(POLICY CMP0071)
  # Silence warning about automoc/autouic skipping generated files
  cmake_policy(SET CMP0071 NEW)
endif()
if(POLICY CMP0074)
  # Since 3.12 tells find_package to look for <Package>_ROOT variables.
  cmake_policy(SET CMP0074 NEW)
endif()

# Just to fix warnings with --warn-uninitialized
if(NOT DEFINED USE_DEFAULT_INSTALL_LOCATION)
  set(USE_DEFAULT_INSTALL_LOCATION FALSE)
endif()
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "")
endif()

# Allow using a non-KDAB install location
set(KDAB_INSTALL True)
if((NOT DEFINED ${USE_DEFAULT_INSTALL_LOCATION}) OR (NOT ${USE_DEFAULT_INSTALL_LOCATION}))
  if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(KDAB_INSTALL False)
  endif()
  set(USE_DEFAULT_INSTALL_LOCATION ${KDAB_INSTALL} CACHE INTERNAL "Install to default KDAB Location" FORCE)
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  project(KDReports LANGUAGES CXX)
else()
  project(KDReports
          DESCRIPTION "A Qt-based library for generating printable and exportable reports from code and XML descriptions."
          HOMEPAGE_URL "https://github.com/KDAB/KDReports"
          LANGUAGES CXX)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/modules)

include(FeatureSummary)

option(${PROJECT_NAME}_STATIC "Build statically" OFF)
option(${PROJECT_NAME}_TESTS "Build the tests" ON)
option(${PROJECT_NAME}_EXAMPLES "Build the examples" ON)
option(${PROJECT_NAME}_QT6 "Build against Qt 6" OFF)
option(${PROJECT_NAME}_PYTHON_BINDINGS "Build python bindings" OFF)
if(${PROJECT_NAME}_PYTHON_BINDINGS AND (CMAKE_BUILD_TYPE MATCHES "^[Dd]eb" OR ${PROJECT_NAME}_STATIC))
  message(FATAL_ERROR "** Python Bindings are disabled in debug or static builds.")
endif()

set(${PROJECT_NAME}_VERSION_MAJOR 1)
set(${PROJECT_NAME}_VERSION_MINOR 95)
set(${PROJECT_NAME}_VERSION_PATCH 0)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})
set(PROJECT_VERSION ${${PROJECT_NAME}_VERSION}) #needed for ECM
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

if(${PROJECT_NAME}_QT6)
  find_package(Qt6 6.0.0 CONFIG REQUIRED COMPONENTS Core Widgets PrintSupport Xml Test)
  set(QT_MAJOR_VERSION 6)
  list(APPEND QT_LIBRARIES Qt6::Core Qt6::PrintSupport Qt6::Xml)
  set(${PROJECT_NAME}_LIBRARY_QTID "-qt6")
else()
  set(REQUIRED_QT_VERSION 5.9.0)
  find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets PrintSupport Xml Test)
  set(QT_MAJOR_VERSION 5)
  list(APPEND QT_LIBRARIES Qt5::Core Qt5::PrintSupport Qt5::Xml)
  set(${PROJECT_NAME}_LIBRARY_QTID "")
endif()

# setup default install locations
include(InstallLocation)

if(${PROJECT_NAME}_TESTS)
  find_package(Qt${QT_MAJOR_VERSION}Test REQUIRED)
endif()
if(NOT EMSCRIPTEN)
  find_package(Qt${QT_MAJOR_VERSION}Sql REQUIRED)
  list(APPEND QT_LIBRARIES Qt${QT_MAJOR_VERSION}::Sql)
endif()

find_package(KDChart CONFIG) # First try the cmake file provided by kdchart, ideal solution
if(NOT KDChart_FOUND)
  find_package(KDChart MODULE) # use FindKDChart.cmake in case kdchart was built with qmake
endif()
if(KDChart_FOUND)
  message(STATUS "Enabling KDChart support")
  add_definitions(-DHAVE_KDCHART)
  if(NOT TARGET KDChart::kdchart)
    add_library(KDChart::kdchart SHARED IMPORTED)
    set_target_properties(KDChart::kdchart PROPERTIES
      IMPORTED_LOCATION "${KDChart_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${KDChart_INCLUDE_DIR}")
  endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

add_definitions(-DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DUSE_EXCEPTIONS -DQT_FATAL_ASSERT)
if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

if(${PROJECT_NAME}_STATIC)
  set(${PROJECT_NAME}_LIBRARY_MODE "STATIC")
else()
  set(${PROJECT_NAME}_LIBRARY_MODE "SHARED")
endif()

if(NOT DEFINED SHIBOKEN_CUSTOM_PREFIX) #look for shiboken in a custom location
  set(SHIBOKEN_CUSTOM_PREFIX "")
endif()
if(NOT DEFINED PYSIDE2_CUSTOM_PREFIX) #look for pyside in a custom location
  set(PYSIDE2_CUSTOM_PREFIX "")
endif()

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(${PROJECT_NAME}_IS_ROOT_PROJECT TRUE)

  if(CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DNDEBUG)
  endif()

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
  endif()

  if(USE_DEFAULT_INSTALL_LOCATION)
    if(UNIX)
      set(CMAKE_INSTALL_PREFIX "/usr/local/KDAB/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}")
    elseif(WIN32)
      set(CMAKE_INSTALL_PREFIX "C:\\KDAB\\${PROJECT_NAME}-$$VERSION")
    endif()
  endif()

  message(STATUS "Building ${PROJECT_NAME} ${${PROJECT_NAME}_VERSION} in ${CMAKE_BUILD_TYPE} mode. Installing to ${CMAKE_INSTALL_PREFIX}")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

  install(FILES README.txt README-commercial.txt kdreports.pri DESTINATION ${INSTALL_DOC_DIR})
  install(DIRECTORY LICENSES DESTINATION ${INSTALL_DOC_DIR})

  include(ECMSetupVersion)
  ecm_setup_version(
    ${${PROJECT_NAME}_VERSION}
    VARIABLE_PREFIX ${PROJECT_NAME}
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/KDReports/KDReportsVersion.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReportsConfigVersion.cmake"
    SOVERSION ${${PROJECT_NAME}_SOVERSION}
    COMPATIBILITY AnyNewerVersion
  )
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/KDReports/KDReportsVersion.h" DESTINATION ${INSTALL_INCLUDE_DIR}/KDReports)

  include(CMakePackageConfigHelpers)

  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KDReportsConfig-buildtree.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReportsConfig.cmake"
    @ONLY
  )
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KDReportsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/install/KDReportsConfig.cmake"
    INSTALL_DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports"
    PATH_VARS INSTALL_INCLUDE_DIR
  )

  install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/install/KDReportsConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReportsConfigVersion.cmake"
    DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports"
  )
  install(EXPORT KDReportsTargets NAMESPACE KDReports::
    DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports"
  )

  # Generate .pri file for qmake users
  #TODO: ECM does not support Qt6 yet
  if(QT_MAJOR_VERSION EQUAL 5 AND
    CMAKE_VERSION VERSION_GREATER "3.11.99" AND NOT CMAKE_CONFIGURATION_TYPES) # Not working with VS generator or older cmake versions
    include(ECMGeneratePriFile)
    ecm_generate_pri_file(BASE_NAME KDReports
                          LIB_NAME KDReports
                          # DEPS "${PRI_DEPS}" # TODO for kdchart
                          FILENAME_VAR pri_filename
    )
    install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
  endif()
endif()

add_subdirectory(src)

if(${PROJECT_NAME}_PYTHON_BINDINGS)
  if(MSVC)
    set(CMAKE_CXX_STANDARD 14)
  endif()
  add_subdirectory(python)
endif()

if(${PROJECT_NAME}_IS_ROOT_PROJECT)
  export(TARGETS kdreports NAMESPACE KDReports::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReportsTargets.cmake"
  )

  if(${PROJECT_NAME}_TESTS)
    enable_testing()
    add_subdirectory(unittests)
  endif()
  if(${PROJECT_NAME}_EXAMPLES)
    add_subdirectory(examples)
  endif()
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
