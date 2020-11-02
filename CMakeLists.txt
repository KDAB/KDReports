# This is the top-level CMakeLists.txt file for the KDReports project.
#
# Pass the following variables to cmake to control the build:
# (See INSTALL-cmake.txt for more information)
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

cmake_minimum_required(VERSION 2.8.12)
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()
if(POLICY CMP0074)
  # Since 3.12 tells find_package to look for <Package>_ROOT variables.
  cmake_policy(SET CMP0074 NEW)
endif()

set(KDAB_INSTALL True)
if((NOT DEFINED ${USE_DEFAULT_INSTALL_LOCATION}) OR (NOT ${USE_DEFAULT_INSTALL_LOCATION}))
  if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(KDAB_INSTALL False)
  endif()
  set(USE_DEFAULT_INSTALL_LOCATION ${KDAB_INSTALL} CACHE INTERNAL "Install to default KDAB Location" FORCE)
endif()

project(KDReports CXX)

option(${PROJECT_NAME}_STATIC "Build statically" OFF)
option(${PROJECT_NAME}_TESTS "Build the tests" ON)
option(${PROJECT_NAME}_EXAMPLES "Build the examples" ON)

set(${PROJECT_NAME}_VERSION_MAJOR 1)
set(${PROJECT_NAME}_VERSION_MINOR 8)
set(${PROJECT_NAME}_VERSION_PATCH 50)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# setup default install locations
include(InstallLocation)

find_package(Qt5Core QUIET)

if(Qt5Core_FOUND)
  find_package(Qt5 COMPONENTS Widgets PrintSupport Sql Xml Test REQUIRED)
  if(${PROJECT_NAME}_TESTS)
    find_package(Qt5 COMPONENTS Test REQUIRED)
  endif()
  set(QT_LIBRARIES Qt5::Core Qt5::PrintSupport Qt5::Sql Qt5::Xml)
  set(QT_USE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Qt5Portability.cmake")

  if(Qt5_POSITION_INDEPENDENT_CODE)
    if(NOT CMAKE_VERSION VERSION_LESS 2.8.11)
      if(Qt5Core_VERSION VERSION_LESS 5.1)
        set_property(TARGET Qt5::Core PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE ON)
      endif()
    else()
      set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()
  endif()
endif()

find_package(KDChart CONFIG) # First try the cmake file provided by kdchart, ideal solution
if (NOT KDChart_FOUND)
  find_package(KDChart MODULE) # use FindKDChart.cmake in case kdchart was built with qmake
endif()
if (KDChart_FOUND)
  message(STATUS "Enabling KDChart support")
  add_definitions(-DHAVE_KDCHART)
  # Hack for kdchart versions < kdchart-2.7.1 (when built with cmake)
  find_package(Qt5Svg CONFIG REQUIRED)
  if (NOT TARGET KDChart::kdchart)
    add_library(KDChart::kdchart SHARED IMPORTED)
    set_target_properties(KDChart::kdchart PROPERTIES
      IMPORTED_LOCATION "${KDChart_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${KDChart_INCLUDE_DIR}")
  endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)

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

  install(FILES LICENSE.GPL.txt LICENSE.US.txt LICENSE.txt LICENSE.LGPL.txt README.txt kdreports.pri DESTINATION ${INSTALL_DOC_DIR})

  include(CMakePackageConfigHelpers)

  write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReportsConfigVersion.cmake"
    VERSION ${${PROJECT_NAME}_VERSION}
    COMPATIBILITY AnyNewerVersion
  )
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
  include(ECMGeneratePriFile)
  set(PROJECT_VERSION_STRING ${${PROJECT_NAME}_VERSION})
  ecm_generate_pri_file(BASE_NAME KDReports
                        LIB_NAME KDReports
                        # DEPS "${PRI_DEPS}" # TODO for kdchart
                        FILENAME_VAR pri_filename)
  install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
endif()

add_subdirectory(src)

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
