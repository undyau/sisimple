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

#include "alterdialog.h"
#include "ui_alterdialog.h"
#include "CResult.h"
#include "CCourse.h"
#include <QDebug>

AlterDialog::AlterDialog(QWidget *parent, QStringList& a_Courses, CResult* a_Result) :
    QDialog(parent), ui(new Ui::AlterDialog), m_Courses(a_Courses), m_Result(a_Result)

{
    ui->setupUi(this);
    ui->classComboBox->addItems(m_Courses);

    int index;
    if (m_Result->GetCourse() && (index = ui->classComboBox->findText(m_Result->GetCourse()->GetName())) >= 0)
        ui->classComboBox->setCurrentIndex(index);

    ui->nameEdit->setText(m_Result->GetName());
    ui->clubEdit->setText(m_Result->GetClub());

    connect(this,SIGNAL(club(QString)), m_Result, SLOT(SetClub(QString)));
    connect(this,SIGNAL(name(QString)), m_Result, SLOT(SetName(QString)));
    connect(this,SIGNAL(course(QString)), m_Result, SLOT(SetCourse(QString)));
}

AlterDialog::~AlterDialog()
{
    delete ui;
}

void AlterDialog::accept()
{
    emit club(ui->clubEdit->text());
    emit name(ui->nameEdit->text());
    emit course(ui->classComboBox->currentText());
    QDialog::accept();
}
