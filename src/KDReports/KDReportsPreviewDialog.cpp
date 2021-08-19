/****************************************************************************
**
** This file is part of the KD Reports library.
**
** SPDX-FileCopyrightText: 2007-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#include "KDReportsPreviewDialog.h"
#include "KDReportsLayoutHelper_p.h"
#include "KDReportsPreviewWidget.h"
#include "KDReportsReport.h"
#include "KDReportsReport_p.h"
#include "KDReportsTableBreakingSettingsDialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

class KDReports::PreviewDialogPrivate
{
public:
    PreviewDialogPrivate(KDReports::PreviewDialog *q)
        : q(q)
        , m_previewWidget(nullptr)
        , m_buttonBox(nullptr)
        , m_quickPrintButton(nullptr)
        , m_dirBrowsingEnabled(true)
    {
    }

    void slotTableBreakingDialog();
    void slotPrintWithDialog();
    void slotQuickPrint();
    void slotSave();

    KDReports::PreviewDialog *q;
    KDReports::PreviewWidget *m_previewWidget;
    QDialogButtonBox *m_buttonBox;
    QPushButton *m_quickPrintButton;
    QString m_quickPrinterName;
    QString m_defaultSaveDirectory;
    QString m_savedFileName;
    bool m_dirBrowsingEnabled;
};

KDReports::PreviewDialog::PreviewDialog(KDReports::Report *report, QWidget *parent)
    : QDialog(parent)
    , d(new PreviewDialogPrivate(this))
{
    d->m_previewWidget = new KDReports::PreviewWidget(this);
    d->m_previewWidget->setReport(report);
    QVBoxLayout *topLayout = new QVBoxLayout(this);
    topLayout->addWidget(d->m_previewWidget);
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    topLayout->addLayout(bottomLayout);

    connect(d->m_previewWidget, &KDReports::PreviewWidget::tableSettingsClicked, this, [&]() { d->slotTableBreakingDialog(); } );
    connect(d->m_previewWidget, &KDReports::PreviewWidget::linkActivated, this, &KDReports::PreviewDialog::linkActivated);

    d->m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    bottomLayout->addWidget(d->m_buttonBox);

    QPushButton *printWithDialogButton = new QPushButton(tr("&Print..."), this);
    d->m_buttonBox->addButton(printWithDialogButton, QDialogButtonBox::ActionRole);
    connect(printWithDialogButton, &QPushButton::clicked, this, [&]() { d->slotPrintWithDialog(); } );

    d->m_quickPrintButton = new QPushButton(this); // create it here for the ordering
    d->m_buttonBox->addButton(d->m_quickPrintButton, QDialogButtonBox::ActionRole);

    QPushButton *saveButton = new QPushButton(tr("&Save..."), this);
    d->m_buttonBox->addButton(saveButton, QDialogButtonBox::ActionRole);
    connect(saveButton, &QPushButton::clicked, this, [&]() { d->slotSave(); } );

    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    d->m_buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    d->m_quickPrintButton->hide();
}

KDReports::PreviewDialog::~PreviewDialog()
{
    delete d;
}

void KDReports::PreviewDialog::setQuickPrinterName(const QString &printerName)
{
    if (!printerName.isEmpty()) {
        // QPrinter::setPrinterName has no effect on the QPrintDialog which uses the CUPS default printer anyway...
        // So we only use this for the separate Print button.
        // This is why it's not called setDefaultPrinterName.
        d->m_quickPrinterName = printerName;
        d->m_quickPrintButton->setText(tr("Print &with %1").arg(printerName));
        d->m_quickPrintButton->show();
        connect(d->m_quickPrintButton, &QPushButton::clicked, this, [&]() { d->slotQuickPrint(); } );
    }
}

void KDReports::PreviewDialog::setDefaultSaveDirectory(const QString &path)
{
    d->m_defaultSaveDirectory = path;
}

void KDReports::PreviewDialog::setDirectoryBrowsingEnabled(bool allowed)
{
    d->m_dirBrowsingEnabled = allowed;
}

bool KDReports::PreviewDialog::showTableSettingsDialog(KDReports::Report *report)
{
    KDReports::TableBreakingSettingsDialog dialog(report);
    return dialog.exec();
}

void KDReports::PreviewDialogPrivate::slotTableBreakingDialog()
{
    if (q->showTableSettingsDialog(m_previewWidget->report())) {
        m_previewWidget->repaint();
    }
}

void KDReports::PreviewDialogPrivate::slotPrintWithDialog()
{
    if (m_previewWidget->printWithDialog()) {
        q->setResult(KDReports::PreviewDialog::Printed);
        q->accept();
    }
}

void KDReports::PreviewDialogPrivate::slotQuickPrint()
{
    KDReports::Report *report = m_previewWidget->report();
    QPrinter printer;
    report->setupPrinter(&printer);
    printer.setPrinterName(m_quickPrinterName);
    report->print(&printer, q);
    q->setResult(KDReports::PreviewDialog::Printed);
    q->accept();
}

void KDReports::PreviewDialogPrivate::slotSave()
{
    KDReports::Report *report = m_previewWidget->report();
    QString file;
    if (m_dirBrowsingEnabled) {
        file = QFileDialog::getSaveFileName(q, q->tr("Save Report as PDF"), m_defaultSaveDirectory, q->tr("PDF Files (*.pdf)"));
    } else {
        bool ok;
        Q_FOREVER {
            const QString text = q->tr("Saving as PDF in %1\n\nEnter the file name:").arg(m_defaultSaveDirectory);
            QString fileName = QInputDialog::getText(q, q->tr("Save Report as PDF"), text, QLineEdit::Normal, report->documentName() + QLatin1String(".pdf"), &ok);
            if (!fileName.endsWith(QLatin1String(".pdf"), Qt::CaseInsensitive)) {
                fileName += QLatin1String(".pdf");
            }
            if (!ok || fileName.isEmpty())
                return;
            file = m_defaultSaveDirectory + QLatin1Char('/') + fileName;
            if (QFile::exists(file)) {
                const QString msg = q->tr("%1 already exists. Do you want to replace it?").arg(fileName);
                if (QMessageBox::warning(q, q->tr("Overwrite?"), msg, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
                    break; // overwrite
                }
            } else {
                // All OK
                break;
            }
        }
    }
    if (!file.isEmpty()) {
        QPrinter printer;
        report->setupPrinter(&printer);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(file);
        m_savedFileName = file;
        report->print(&printer, q);
        if (QFile::exists(file)) {
            q->setResult(KDReports::PreviewDialog::SavedSuccessfully);
        } else {
            q->setResult(KDReports::PreviewDialog::SaveError);
        }
        q->accept();
    }
}

void KDReports::PreviewDialog::setPageSizeChangeAllowed(bool b)
{
    d->m_previewWidget->setPageSizeChangeAllowed(b);
}

void KDReports::PreviewDialog::setShowTableSettingsDialog(bool b)
{
    d->m_previewWidget->setShowTableSettingsDialog(b);
}

void KDReports::PreviewDialog::setWidthForEndlessPrinter(qreal widthMM)
{
    d->m_previewWidget->setWidthForEndlessPrinter(widthMM);
}

bool KDReports::PreviewDialog::isSelected(int pageNumber) const
{
    return d->m_previewWidget->isSelected(pageNumber);
}

void KDReports::PreviewDialog::accept()
{
    QDialog::accept();
}

void KDReports::PreviewDialog::reject()
{
    QDialog::reject();
}

KDReports::PreviewWidget *KDReports::PreviewDialog::previewWidget()
{
    return d->m_previewWidget;
}

QString KDReports::PreviewDialog::savedFileName() const
{
    return d->m_savedFileName;
}

#include "moc_KDReportsPreviewDialog.cpp"
