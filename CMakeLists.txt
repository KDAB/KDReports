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
# -DKDReports_DOCS=[true|false]
#  Build the API documentation. Enables the 'docs' build target.
#  Default=false
#
# -DKDReports_PYTHON_BINDINGS=[true|false]
#  Build/Generate python bindings.  Always false for Debug builds
#  (If your shiboken or pyside is installed in a non-standard locations
#  try passing the SHIBOKEN_CUSTOM_PREFIX and PYSIDE_CUSTOM_PREFIX variables.)
#  Default=false
#
# -DKDReports_PYTHON_BINDINGS_INSTALL_PREFIX=[path]
#  Alternative install path for Python bindings.
#  Default=CMAKE_INSTALL_PREFIX
#
cmake_minimum_required(VERSION 3.3)
if(POLICY CMP0071)
  # Silence warning about automoc/autouic skipping generated files
  cmake_policy(SET CMP0071 NEW)
endif()
if(POLICY CMP0074)
  # Since 3.12 tells find_package to look for <Package>_ROOT variables.
  cmake_policy(SET CMP0074 NEW)
endif()

# Allow using a non-KDAB install location.
set(KDAB_INSTALL True CACHE INTERNAL "Install to default KDAB Location")
if(DEFINED CMAKE_INSTALL_PREFIX)
  if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(KDAB_INSTALL False CACHE INTERNAL "Install to non-KDAB Location")
  endif()
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  project(KDReports LANGUAGES CXX)
else()
  project(KDReports
          DESCRIPTION "A Qt-based library for generating printable and exportable reports from code and XML descriptions."
          HOMEPAGE_URL "https://github.com/KDAB/KDReports"
          LANGUAGES CXX)
endif()

set(${PROJECT_NAME}_VERSION_MAJOR 2)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(${PROJECT_NAME}_VERSION_PATCH 0)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})
set(PROJECT_VERSION ${${PROJECT_NAME}_VERSION}) #needed for ECM
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

include(FeatureSummary)

option(${PROJECT_NAME}_QT6 "Build against Qt 6" OFF)
option(${PROJECT_NAME}_STATIC "Build statically" OFF)
option(${PROJECT_NAME}_TESTS "Build the tests" ON)
option(${PROJECT_NAME}_EXAMPLES "Build the examples" ON)
option(${PROJECT_NAME}_DOCS "Build the API documentation" OFF)
option(${PROJECT_NAME}_PYTHON_BINDINGS "Build python bindings" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/KDAB/modules)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to ${default_build_type} as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(${PROJECT_NAME}_QT6)
  set(Qt_VERSION_MAJOR 6)
  set(QT_MIN_VERSION "6.0.0")
  find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets PrintSupport Xml)
  list(APPEND QT_LIBRARIES Qt6::Core Qt6::Widgets Qt6::PrintSupport Qt6::Xml)
  set(${PROJECT_NAME}_LIBRARY_QTID "-qt6")
else()
  set(Qt_VERSION_MAJOR 5)
  set(QT_MIN_VERSION 5.9.0)
  find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets PrintSupport Xml)
  list(APPEND QT_LIBRARIES Qt5::Core Qt5::Widgets Qt5::PrintSupport Qt5::Xml)
  set(${PROJECT_NAME}_LIBRARY_QTID "")
endif()
include(KDQtInstallPaths) #to set QT_INSTALL_FOO variables

# look for KDChart
find_package(KDChart${${PROJECT_NAME}_LIBRARY_QTID} CONFIG) # First try the cmake file provided by kdchart, ideal solution
if(NOT ${PROJECT_NAME}_QT6)
  if(NOT KDChart_FOUND)
    find_package(KDChart MODULE) # use FindKDChart.cmake in case kdchart was built with qmake
  endif()
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
set(CMAKE_AUTORCC TRUE)
set(CMAKE_AUTOUIC TRUE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(TARGET Qt6::Core)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Default to hidden visibility for symbols
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(KDCompilerFlags)

add_definitions(-DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DUSE_EXCEPTIONS -DQT_FATAL_ASSERT)
if(MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()
if(CMAKE_BUILD_TYPE MATCHES "Release")
  add_definitions(-DNDEBUG)
endif()

if(${PROJECT_NAME}_STATIC)
  set(${PROJECT_NAME}_LIBRARY_MODE "STATIC")
else()
  set(${PROJECT_NAME}_LIBRARY_MODE "SHARED")
endif()

if(KDAB_INSTALL)
  if(UNIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local/KDAB/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}" CACHE INTERNAL "Install to default KDAB Location")
  elseif(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:\\KDAB\\${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}" CACHE INTERNAL "Install to default KDAB Location")
  endif()
endif()

# setup default install locations
include(InstallLocation)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(${PROJECT_NAME}_IS_ROOT_PROJECT TRUE)

  message(STATUS "Building ${PROJECT_NAME} ${${PROJECT_NAME}_VERSION} in ${CMAKE_BUILD_TYPE} mode. Installing to ${CMAKE_INSTALL_PREFIX}")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

  install(FILES README.md README-commercial.txt DESTINATION ${INSTALL_DOC_DIR})
  if(NOT ${PROJECT_NAME}_QT6)
    install(FILES kdreports.pri DESTINATION ${INSTALL_DOC_DIR})
  endif()
  install(DIRECTORY LICENSES DESTINATION ${INSTALL_DOC_DIR})

  install(EXPORT KDReportsTargets NAMESPACE KDReports::
    DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports${${PROJECT_NAME}_LIBRARY_QTID}"
  )

  # Generate .pri file for qmake users
  #TODO: ECM does not support Qt6 yet
  if(Qt_VERSION_MAJOR EQUAL 5 AND
    CMAKE_VERSION VERSION_GREATER "3.11.99" AND NOT CMAKE_CONFIGURATION_TYPES) # Not working with VS generator or older cmake versions
    include(ECMGeneratePriFile)
    ecm_generate_pri_file(BASE_NAME KDReports
                          LIB_NAME KDReports
                          # DEPS "${PRI_DEPS}" # TODO for kdchart
                          FILENAME_VAR pri_filename
    )
    install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
  endif()
else()
  #Always disable tests, examples, docs when used as a submodule
  set(${PROJECT_NAME}_IS_ROOT_PROJECT FALSE)
  set(${PROJECT_NAME}_TESTS FALSE)
  set(${PROJECT_NAME}_EXAMPLES FALSE)
  set(${PROJECT_NAME}_DOCS FALSE)
endif()

if(${PROJECT_NAME}_TESTS)
  enable_testing()
endif()

add_subdirectory(src)

if(${PROJECT_NAME}_PYTHON_BINDINGS)
  if(Qt_VERSION_MAJOR EQUAL 5 AND Qt5Core_VERSION VERSION_LESS 5.12)
    message(WARNING "** Disabling Python Bindings. Qt version is too old and unsupported.")
    set(${PROJECT_NAME}_PYTHON_BINDINGS OFF)
  elseif(Qt_VERSION_MAJOR EQUAL 6 AND Qt6Core_VERSION VERSION_LESS 6.2)
    message(WARNING "** Disabling Python Bindings. shiboken6 is too old and unsupported")
    set(${PROJECT_NAME}_PYTHON_BINDINGS OFF)
  elseif(CMAKE_BUILD_TYPE MATCHES "^[Dd]eb" OR ${PROJECT_NAME}_STATIC)
    message(FATAL_ERROR "** Python Bindings are disabled in debug or static builds.")
  endif()
endif()
if(${PROJECT_NAME}_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()

if(${PROJECT_NAME}_TESTS OR ${PROJECT_NAME}_EXAMPLES)
  if(NOT EMSCRIPTEN)
    find_package(Qt${Qt_VERSION_MAJOR}Sql)
  endif()
endif()

if(${PROJECT_NAME}_TESTS)
  find_package(Qt${Qt_VERSION_MAJOR}Test REQUIRED)
  add_subdirectory(unittests)
endif()

if(${PROJECT_NAME}_EXAMPLES)
  add_subdirectory(examples)
endif()

if(${PROJECT_NAME}_DOCS)
  add_subdirectory(docs) # needs to go last, in case there are build source files
endif()

if(${PROJECT_NAME}_IS_ROOT_PROJECT)
  if(NOT ${PROJECT_NAME}_DOCS)
    add_custom_target(docs
      COMMAND ${CMAKE_COMMAND} -E echo "Sorry, there is no docs target since KDReports_DOCS=OFF."
    "Re-run cmake with the -DKDReports_DOCS=True option if you want to generate the documentation.")
  endif()
  # Add uninstall target (not for submodules since parent projects typically have uninstall too)
  include(ECMUninstallTarget)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
