#
# This module finds if KDChart is installed.
#
# KDChart_FOUND       - Set to TRUE if KDChart was found.
# KDChart_LIBRARIES   - Path to KDChart libraries.
# KDChart_INCLUDE_DIR - Path to the KDChart include directory.
#
# SPDX-FileCopyrightText: 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

include(FindPackageHandleStandardArgs)

find_library(KDChart_LIBRARIES
    NAMES KDChart kdchart kdchart2 kdchartd2 NAMES_PER_DIR
    HINTS ${KDChart_DIR} ENV KDChart_DIR
    PATH_SUFFIXES lib)

find_path(KDChart_INCLUDE_DIR
    NAMES KDChartGlobal.h
    HINTS ${KDChart_DIR} ENV KDChart_DIR
    PATH_SUFFIXES include include/KDChart)

mark_as_advanced(KDChart_LIBRARIES KDChart_INCLUDE_DIR)

find_package_handle_standard_args(KDChart DEFAULT_MSG KDChart_LIBRARIES KDChart_INCLUDE_DIR)

if(KDChart_FOUND)
    add_library(KDChart UNKNOWN IMPORTED)
    set_target_properties(KDChart PROPERTIES
        IMPORTED_LOCATION ${KDChart_LIBRARIES}
        INTERFACE_INCLUDE_DIRECTORIES ${KDChart_INCLUDE_DIR})
endif()
