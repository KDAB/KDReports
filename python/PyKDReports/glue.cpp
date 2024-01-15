/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

// clang-format off
// Don't reformat this snippet

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

// clang-format on
