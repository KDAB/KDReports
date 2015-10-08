project(KDReports)

option(
  ${PROJECT_NAME}_ENFORCE_QT4_BUILD
  "Enable if you want to enforce a build with Qt4"
  OFF
)

cmake_minimum_required(VERSION 2.8.11)

option(${PROJECT_NAME}_STATIC "Build statically" OFF)
option(${PROJECT_NAME}_TESTS "Build the tests" ON)

set(${PROJECT_NAME}_VERSION_MAJOR 1)
set(${PROJECT_NAME}_VERSION_MINOR 7)
set(${PROJECT_NAME}_VERSION_PATCH 0)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# setup default install locations
include(InstallLocation)

# try Qt5 first, and prefer that (if found), but only if not disabled via option
if(NOT ${PROJECT_NAME}_ENFORCE_QT4_BUILD)
  find_package(Qt5Core QUIET)
endif()

if(Qt5Core_FOUND)
  find_package(Qt5 COMPONENTS Widgets PrintSupport Sql Xml REQUIRED)
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
else()
  find_package(Qt4 4.7 QUIET REQUIRED QtCore QtMain QtGui)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)

add_definitions(-DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DUSE_EXCEPTIONS -DQT_FATAL_ASSERT)

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
endif()

add_subdirectory(src)

if(${PROJECT_NAME}_IS_ROOT_PROJECT)

  export(TARGETS kdreports NAMESPACE KDReports::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/KDReports/KDReportsTargets.cmake"
  )

#   add_subdirectory(features)
  if(${PROJECT_NAME}_TESTS)
    enable_testing()
    add_subdirectory(unittests)
  endif()
  add_subdirectory(examples)
endif()
