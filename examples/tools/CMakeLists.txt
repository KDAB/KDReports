##
## This file is part of the KD Reports library.
##
## SPDX-FileCopyrightText: 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
##
## SPDX-License-Identifier: ((LGPL-2.1-only OR LGPL-3.0-only) AND (GPL-2.0-only OR GPL-3.0-only)) OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
##
## Licensees holding valid commercial KD Reports licenses may use this file in
## accordance with the KD Reports Commercial License Agreement provided with
## the Software.
##
## Contact info@kdab.com if any conditions of this licensing are not clear to you.
##


add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x000000)

set(SOURCES
    TableModel.cpp
)

add_library(kdreporttesttools ${KDReports_LIBRARY_MODE} ${SOURCES})
if(${PROJECT_NAME}_STATIC)
  target_compile_definitions(kdreporttesttools PUBLIC TESTTOOLS_STATICLIB)
else()
  target_compile_definitions(kdreporttesttools PRIVATE -DKDREPORT_BUILD_TESTTOOLS_LIB)
endif()

target_link_libraries(kdreporttesttools ${QT_LIBRARIES})
set_target_properties(kdreporttesttools PROPERTIES DEFINE_SYMBOL KDREPORT_BUILD_TESTTOOLS_LIB)
set_target_properties(kdreporttesttools PROPERTIES
  SOVERSION ${${PROJECT_NAME}_SOVERSION}
  VERSION ${${PROJECT_NAME}_VERSION}
)
