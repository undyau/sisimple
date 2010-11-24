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
#include "coursedialog.h"
#include "ui_coursedialog.h"
#include "CCourse.h"
#include <QPushButton>

courseDialog::courseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::courseDialog), m_New(true), m_Course(NULL)
    {
    init();
    }


courseDialog::courseDialog(QWidget *parent, CCourse* a_Course) :
    QDialog(parent),
    ui(new Ui::courseDialog), m_New(false), m_Course(a_Course)
{
    init();

    ui->nameEdit->setText(a_Course->GetName());
    ui->lengthEdit->setText(a_Course->GetLength());
    QStringList controls;
    a_Course->GetControls(controls);
    QString s;
    for (QStringList::Iterator i = controls.begin(); i != controls.end(); i++)
        s += (*i + ',');
    ui->controlsText->insertPlainText(s.left(s.length() - 1));
}

void courseDialog::init()
    {
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->controlsText,SIGNAL(textChanged()),this,SLOT(enableCtrls()));
    connect(ui->nameEdit,SIGNAL(textChanged(QString)), this, SLOT(enableCtrls()));

    enableCtrls();
    }

courseDialog::~courseDialog()
{
    delete ui;
}

void courseDialog::accept()
    {
    m_Name = ui->nameEdit->text();
    m_Length = ui->lengthEdit->text();
    m_Climb = ui->climbEdit->text();
    m_Controls = ui->controlsText->toPlainText();

    QDialog::accept();
    }

void courseDialog::enableCtrls()
    {
    bool Ok(true);
    QString name = ui->nameEdit->text();
    name = name.trimmed();
    Ok = !name.isEmpty();

    QString controls = ui->controlsText->toPlainText();
    QRegExp re("^[[\\s]*[0-9]{1,3}[\\s]*,]*[\\s]*[0-9]{1,3}[\\s]*");
    Ok |= (re.exactMatch(controls) && re.matchedLength() == controls.length());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(Ok);
    }
