/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
**
** SPDX-License-Identifier: MIT
**
****************************************************************************/

#pragma once

// Make "signals:", "slots:" visible as access specifiers
#define QT_ANNOTATE_ACCESS_SPECIFIER(a) __attribute__((annotate(#a)))

#include <QAbstractItemModel>
#include <QTextCursor>
#include <QTextDocumentFragment>

#include <KDReports/KDReports.h>
#include <KDReports/KDReportsAbstractTableElement.h>
#include <KDReports/KDReportsAutoTableElement.h>
#include <KDReports/KDReportsCell.h>
#include <KDReports/KDReportsChartElement.h>
#include <KDReports/KDReportsElement.h>
#include <KDReports/KDReportsErrorDetails.h>
#include <KDReports/KDReportsFrame.h>
#include <KDReports/KDReportsGlobal.h>
#include <KDReports/KDReportsHLineElement.h>
#include <KDReports/KDReportsHeader.h>
#include <KDReports/KDReportsHtmlElement.h>
#include <KDReports/KDReportsImageElement.h>
#include <KDReports/KDReportsMainTable.h>
#include <KDReports/KDReportsPreviewDialog.h>
#include <KDReports/KDReportsPreviewWidget.h>
#include <KDReports/KDReportsReport.h>
#include <KDReports/KDReportsTableBreakingSettingsDialog.h>
#include <KDReports/KDReportsTableElement.h>
#include <KDReports/KDReportsTextElement.h>
#include <KDReports/KDReportsUnit.h>
#include <KDReports/KDReportsVariableType.h>
#include <KDReports/KDReportsXmlElementHandler.h>
#include <KDReports/KDReportsXmlHelper.h>
// Private
#include <KDReports/KDReportsReportBuilder_p.h>
