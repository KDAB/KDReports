# - Find KDChart
# This module finds if KDChart is installed.
#
# KDChart_FOUND          - Set to TRUE if KDChart was found.
# KDChart_LIBRARIES      - Path to KDChart libraries.
# KDChart_INCLUDE_DIR    - Path to the KDChart include directory.
#
# Copyright (C) 2015-2016 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

# Redistribution and use is allowed according to the terms of the BSD license

include(FindPackageHandleStandardArgs)

find_library(KDChart_LIBRARIES
    NAMES KDChart kdchart kdchart2
    HINTS ${KDChart_DIR}/lib $ENV{KDChart_DIR}/lib
    PATH_SUFFIXES lib)
find_path(KDChart_INCLUDE_DIR
    NAMES KDChartGlobal.h
    HINTS ${KDChart_DIR}/include $ENV{KDChart_DIR}/include
    PATH_SUFFIXES KDChart)

mark_as_advanced(KDChart_LIBRARIES KDChart_INCLUDE_DIR)

find_package_handle_standard_args(KDChart DEFAULT_MSG KDChart_LIBRARIES KDChart_INCLUDE_DIR)
