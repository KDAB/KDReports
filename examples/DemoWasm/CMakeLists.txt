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

set(DEMO_SOURCES
    kdreports.qrc
    kdreportswindow.cpp
    kdreportswindow.h
    kdreportswindow.ui
    ResultModel.cpp
    ResultModel.h
    TableModel.cpp
    TableModel.h
    tools_export.h
    main.cpp
)

qt_add_executable(demo-wasm ${DEMO_SOURCES})

# Output wasm file to same location as .html
# file created by qt_add_executable
set_target_properties(demo-wasm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ""
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ""
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ""
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ""
)

target_link_libraries(
    demo-wasm PUBLIC ${QT_LIBRARIES} kdreports
)
