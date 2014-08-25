/****************************************************************************
** Copyright (C) 2007-2014 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Reports library.
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2.1 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#include "KDReportsReport.h"
#include "KDReportsReport_p.h"
#include "KDReportsPreviewDialog.h"
#include <QPushButton>
#include "KDReportsPreviewWidget.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "KDReportsTableBreakingSettingsDialog.h"
#include "KDReportsLayoutHelper_p.h"

class KDReports::PreviewDialog::Private
{
public:
    Private( KDReports::PreviewDialog* q) : q( q ) {}

    void _kd_slotTableBreakingDialog();

    KDReports::PreviewWidget* m_previewWidget;
    KDReports::PreviewDialog* q;
};

KDReports::PreviewDialog::PreviewDialog( KDReports::Report* report, QWidget *parent )
    : QDialog( parent ), d( new Private( this ) )
{
    d->m_previewWidget = new KDReports::PreviewWidget( this );
    d->m_previewWidget->setReport( report );
    QVBoxLayout* topLayout = new QVBoxLayout( this );
    topLayout->addWidget( d->m_previewWidget );
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    topLayout->addLayout( bottomLayout );

    connect( d->m_previewWidget, SIGNAL(tableSettingsClicked()), this, SLOT(_kd_slotTableBreakingDialog()) );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                        Qt::Horizontal,
                                                        this );
    bottomLayout->addWidget( buttonBox );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

    QPushButton* okButton = buttonBox->button( QDialogButtonBox::Ok );
    okButton->setText(tr("&Print..."));

}

KDReports::PreviewDialog::~PreviewDialog()
{
    delete d;
}

bool KDReports::PreviewDialog::showTableSettingsDialog( KDReports::Report* report )
{
    KDReports::TableBreakingSettingsDialog dialog( report );
    return dialog.exec();
}

void KDReports::PreviewDialog::Private::_kd_slotTableBreakingDialog()
{
    if ( q->showTableSettingsDialog( m_previewWidget->report() ) ) {
        m_previewWidget->repaint();
    }
}

void KDReports::PreviewDialog::setPageSizeChangeAllowed( bool b )
{
    d->m_previewWidget->setPageSizeChangeAllowed( b );    
}

void KDReports::PreviewDialog::setShowTableSettingsDialog( bool b )
{
    d->m_previewWidget->setShowTableSettingsDialog( b );
}

void KDReports::PreviewDialog::setWidthForEndlessPrinter( qreal widthMM )
{
    d->m_previewWidget->setWidthForEndlessPrinter( widthMM );
}

bool KDReports::PreviewDialog::isSelected( int pageNumber ) const
{
    return d->m_previewWidget->isSelected( pageNumber );
}

void KDReports::PreviewDialog::accept()
{
    if ( d->m_previewWidget->printWithDialog() ) {
        QDialog::accept();
    }
}

void KDReports::PreviewDialog::reject()
{
    QDialog::reject();
}

KDReports::PreviewWidget * KDReports::PreviewDialog::previewWidget()
{
    return d->m_previewWidget;
}

#include "moc_KDReportsPreviewDialog.cpp"
