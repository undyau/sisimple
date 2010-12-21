/*
Copyright 2010 Andy Simpson

This file is part of SI Simple.

SI Simple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SI Simple is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SI Simple.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "importsidialog.h"
#include "ui_importsidialog.h"
#include "Utils.h"
#include <QUrl>
#include <QFileInfo>

ImportSIDialog::ImportSIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportSIDialog)
{
    ui->setupUi(this);
    connect(ui->fileEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui->webEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    EnableCtrls();
}

ImportSIDialog::~ImportSIDialog()
{
    delete ui;
}

void ImportSIDialog::accept()
{
    if (ui->radioButtonWeb->isChecked())
        {
        Download();
        }
    if (ui->radioButtonFile->isChecked())
        {
        ReadFile();
        }
}

void ImportSIDialog::Download()
{

}

void ImportSIDialog::ReadFile()
{

}

void ImportSIDialog::textChanged(QString)
{
    EnableCtrls();
}

void ImportSIDialog::EnableCtrls()
{
    if (ui->radioButtonWeb->isChecked())
        {
        QUrl url(ui->webEdit->text());
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(url.isValid());
        }
    if (ui->radioButtonFile->isChecked())
        {
        QFileInfo file(ui->fileEdit->text());
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(file.exists());
        }
}
