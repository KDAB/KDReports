/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDAB-KDReports OR LicenseRef-KDAB-KDReports-US
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
** Contact info@kdab.com if any conditions of this licensing are not clear to you.
**
****************************************************************************/

// @snippet abstract_table_element_default_font

bool p;
auto retval = %CPPSELF.%FUNCTION_NAME(&p);
%PYARG_0 = PyTuple_New(2);
PyTuple_SET_ITEM(%PYARG_0, 0, %CONVERTTOPYTHON[QFont](retval));
PyTuple_SET_ITEM(%PYARG_0, 1, %CONVERTTOPYTHON[bool](p));

// @snippet abstract_table_element_default_font

// @snippet report_get_margins
qreal top, left, bottom, right;

%CPPSELF.%FUNCTION_NAME(&top, &left, &bottom, &right);
%PYARG_0 = PyTuple_New(4);
PyTuple_SET_ITEM(%PYARG_0, 0, %CONVERTTOPYTHON[qreal](top));
PyTuple_SET_ITEM(%PYARG_0, 1, %CONVERTTOPYTHON[qreal](left));
PyTuple_SET_ITEM(%PYARG_0, 2, %CONVERTTOPYTHON[qreal](bottom));
PyTuple_SET_ITEM(%PYARG_0, 3, %CONVERTTOPYTHON[qreal](right));

// @snippet report_get_margins
