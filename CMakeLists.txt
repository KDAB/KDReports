# This file is part of the KD Reports library.
#
# SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: MIT
#

# Pass the following variables to cmake to control the build:
# (See INSTALL.txt for more information)
#
# -DKDReports_QT6=[true|false]
#  Build against Qt6 rather than Qt5
#  Default=true
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
cmake_minimum_required(VERSION 3.12)
if(POLICY CMP0071)
    # Silence warning about automoc/autouic skipping generated files
    cmake_policy(SET CMP0071 NEW)
endif()
if(POLICY CMP0074)
    # Since 3.12 tells find_package to look for <Package>_ROOT variables.
    cmake_policy(SET CMP0074 NEW)
endif()

# Allow using a non-KDAB install location.
set(KDAB_INSTALL
    True
    CACHE INTERNAL "Install to default KDAB Location"
)
if(DEFINED CMAKE_INSTALL_PREFIX)
    if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
        set(KDAB_INSTALL
            False
            CACHE INTERNAL "Install to non-KDAB Location"
        )
    endif()
endif()

project(
    KDReports
    DESCRIPTION "A Qt-based library for generating printable and exportable reports from code and XML descriptions."
    HOMEPAGE_URL "https://github.com/KDAB/KDReports"
    LANGUAGES CXX
)

set(${PROJECT_NAME}_VERSION_MAJOR 2)
set(${PROJECT_NAME}_VERSION_MINOR 3)
set(${PROJECT_NAME}_VERSION_PATCH 95)
set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}
)
set(PROJECT_VERSION ${${PROJECT_NAME}_VERSION}) #needed for ECM
set(${PROJECT_NAME}_SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

include(FeatureSummary)

option(${PROJECT_NAME}_QT6 "Build against Qt 6" ON)
option(${PROJECT_NAME}_STATIC "Build statically" OFF)
option(${PROJECT_NAME}_TESTS "Build the tests" ON)
option(${PROJECT_NAME}_EXAMPLES "Build the examples" ON)
option(${PROJECT_NAME}_DOCS "Build the API documentation" OFF)
option(${PROJECT_NAME}_PYTHON_BINDINGS "Build python bindings" OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ECM/modules)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/KDAB/modules)

include(ECMEnableSanitizers)

# Set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(default_build_type "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to ${default_build_type} as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${default_build_type}"
        CACHE STRING "Choose the type of build." FORCE
    )
    # Set the possible values of build type for cmake-gui
    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
                 "Debug"
                 "Release"
                 "MinSizeRel"
                 "RelWithDebInfo"
    )
endif()

if(CMAKE_COMPILE_WARNING_AS_ERROR)
    # These -Wno-error flags are here to enable -Werror for clazy in CI
    # it doesn't mean we endorse those warnings.
    # MSVC is untested and probably will have a few hundered errors
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wno-error=deprecated-declarations)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            add_compile_options(-Wno-error=documentation -Wno-error=unused-private-field -Wno-error=c++20-extensions)
        endif()
    endif()
endif()

if(${PROJECT_NAME}_QT6)
    set(QT_VERSION_MAJOR 6)
    set(QT_MIN_VERSION "6.0.0")
    find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets PrintSupport Xml)
    list(
        APPEND
        QT_LIBRARIES
        Qt6::Core
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Xml
    )
    set(${PROJECT_NAME}_LIBRARY_QTID "-qt6")
else()
    set(QT_VERSION_MAJOR 5)
    set(QT_MIN_VERSION 5.9.0)
    find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets PrintSupport Xml)
    list(
        APPEND
        QT_LIBRARIES
        Qt5::Core
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Xml
    )
    set(${PROJECT_NAME}_LIBRARY_QTID "")
endif()
include(KDQtInstallPaths) #to set QT_INSTALL_FOO variables

# look for KDChart
if(TARGET KDChart::kdchart)
    # already defined => it's probably in another gitsubmodule in the same overall cmake project
    set(KDChart_FOUND TRUE)
else()
    set(KDChartPackageName KDChart${${PROJECT_NAME}_LIBRARY_QTID})
    find_package(${KDChartPackageName} CONFIG)
    set(KDChart_FOUND ${${KDChartPackageName}_FOUND})
endif()
# First try the cmake file provided by kdchart, ideal solution
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
        set_target_properties(
            KDChart::kdchart PROPERTIES IMPORTED_LOCATION "${KDChart_LIBRARIES}" INTERFACE_INCLUDE_DIRECTORIES
                                                                                 "${KDChart_INCLUDE_DIR}"
        )
    endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_LINK_DEPENDS_NO_SHARED TRUE) # don't relink dependencies when a shared lib changes

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# Default to hidden visibility for symbols
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(KDCompilerFlags)

add_definitions(-DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_URL_CAST_FROM_STRING)
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
        set(CMAKE_INSTALL_PREFIX
            "/usr/local/KDAB/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}"
            CACHE INTERNAL "Install to default KDAB Location"
        )
    elseif(WIN32)
        set(CMAKE_INSTALL_PREFIX
            "C:\\KDAB\\${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}"
            CACHE INTERNAL "Install to default KDAB Location"
        )
    endif()
endif()

# setup default install locations
include(KDInstallLocation)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(${PROJECT_NAME}_IS_ROOT_PROJECT TRUE)

    message(STATUS "Building ${PROJECT_NAME} ${${PROJECT_NAME}_VERSION} in ${CMAKE_BUILD_TYPE} mode. "
                   "Installing to ${CMAKE_INSTALL_PREFIX}"
    )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

    install(FILES README.md DESTINATION ${INSTALL_DOC_DIR})
    if(NOT ${PROJECT_NAME}_QT6)
        install(FILES kdreports.pri DESTINATION ${INSTALL_DOC_DIR})
    endif()
    install(DIRECTORY LICENSES DESTINATION ${INSTALL_DOC_DIR})

    install(
        EXPORT KDReportsTargets
        NAMESPACE KDReports::
        DESTINATION "${INSTALL_LIBRARY_DIR}/cmake/KDReports${${PROJECT_NAME}_LIBRARY_QTID}"
    )

    # Generate .pri file for qmake users (except when using the VS generator)
    if(NOT CMAKE_CONFIGURATION_TYPES)
        if(QT_VERSION_MAJOR EQUAL 5 OR (QT_VERSION_MAJOR EQUAL 6 AND Qt6Core_VERSION VERSION_GREATER "6.2"))
            include(ECMGeneratePriFile) #not available for Qt6.2 or lower
            set(PROJECT_VERSION_STRING ${${PROJECT_NAME}_VERSION})
            ecm_generate_pri_file(
                BASE_NAME
                ${PROJECT_NAME}
                LIB_NAME
                kdreports${${PROJECT_NAME}_LIBRARY_QTID}
                FILENAME_VAR
                pri_filename
                INCLUDE_INSTALL_DIR
                ${INSTALL_INCLUDE_DIR}
            )
            install(FILES ${pri_filename} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
        endif()
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
    if(QT_VERSION_MAJOR EQUAL 5 AND Qt5Core_VERSION VERSION_LESS 5.12)
        message(WARNING "** Disabling Python Bindings. Qt version is too old and unsupported.")
        set(${PROJECT_NAME}_PYTHON_BINDINGS OFF)
    elseif(QT_VERSION_MAJOR EQUAL 6 AND Qt6Core_VERSION VERSION_LESS 6.2)
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
        find_package(Qt${QT_VERSION_MAJOR}Sql)
    endif()
endif()

if(${PROJECT_NAME}_TESTS)
    find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
    add_subdirectory(unittests)
endif()

if(${PROJECT_NAME}_EXAMPLES)
    add_subdirectory(examples)
endif()

if(${PROJECT_NAME}_DOCS)
    add_subdirectory(docs) # needs to go last, in case there are build source files
endif()

if(${PROJECT_NAME}_IS_ROOT_PROJECT)
    # Add uninstall target (not for submodules since parent projects typically have uninstall too)
    include(ECMUninstallTarget)

    feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()

# TODO once cppcheck is passing change --error-exitcode=1 so CI step fails on new errors
add_custom_target(
    cppcheck
    COMMENT "Run cppcheck on sources"
    USES_TERMINAL
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMAND
        ${CMAKE_COMMAND} -E env cppcheck --project=${PROJECT_BINARY_DIR}/compile_commands.json --enable=all
        --error-exitcode=0 --language=c++ --inline-suppr --quiet --disable=missingInclude,unusedFunction
        --check-level=exhaustive --library=qt.cfg -i3rdParty/ --suppress=*:*.moc --suppress=*:*moc_*.cpp -i/cmake/ECM/
        -i/cmake/KDAB/
)
