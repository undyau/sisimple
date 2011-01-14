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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include "CEvent.h"
#include <QSettings>
#include <QDebug>
#include <QFileDialog>

ImportSIDialog::ImportSIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportSIDialog), m_Manager(NULL)
{
    ui->setupUi(this);
    connect(ui->fileEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui->webEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(this, SIGNAL(SIDataRead(QString)),CEvent::Event(), SLOT(newSIData(QString)));
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(ui->radioButtonFile, SIGNAL(clicked()), this, SLOT(choiceChanged()));
    connect(ui->radioButtonWeb, SIGNAL(clicked()), this, SLOT(choiceChanged()));

    QSettings settings(QSettings::IniFormat,  QSettings::SystemScope, "undy","SI Simple");
    settings.beginGroup("General");
    ui->fileEdit->setText(settings.value("SIGlobalFile","").toString());
    ui->webEdit->setText(settings.value("SIGlobalWeb","").toString());
    settings.endGroup();

    EnableCtrls();
}

ImportSIDialog::~ImportSIDialog()
{
    if (m_Manager != NULL)
        delete m_Manager;
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
    if (m_Manager == NULL)
        m_Manager = new QNetworkAccessManager(this);
    connect(m_Manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    setCursor(Qt::BusyCursor);
    setEnabled(false);
    m_Manager->get(QNetworkRequest(QUrl(ui->webEdit->text())));

}

void ImportSIDialog::replyFinished(QNetworkReply* reply)
{
    setCursor(Qt::ArrowCursor);
    setEnabled(true);
    if (reply->error() != QNetworkReply::NoError)
        {
        SIMessageBox(tr("Unable to read ") + reply->url().toString() + tr(". Error: ") + reply->errorString());
        return;
        }
    else
        {
        QSettings settings("undy","SI Simple");
        settings.beginGroup("General");
        settings.setValue("SIGlobalWeb",reply->url().toString());
        settings.endGroup();
        QByteArray bytes = reply->readAll();  // bytes
        QString string(bytes); // string
        emit SIDataRead(string);
        QDialog::accept();
        }
}

void ImportSIDialog::ReadFile()
{
    QSettings settings(QSettings::IniFormat,  QSettings::SystemScope, "undy","SI Simple");
    settings.beginGroup("General");
    settings.setValue("SIGlobalFile",ui->fileEdit->text());
    settings.endGroup();

 // Load new file
    QFile tfile(ui->fileEdit->text());
    tfile.open(QIODevice::ReadOnly);
    QByteArray bytes = tfile.readAll();
    QString string(bytes); // string
    emit SIDataRead(string);
    QDialog::accept();
}

void ImportSIDialog::textChanged(QString)
{
    EnableCtrls();
}

void ImportSIDialog::choiceChanged()
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

void ImportSIDialog::chooseFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select SI data file"), CEvent::Event()->Directory());

    if (file.isEmpty())
        return;

    ui->fileEdit->setText(file);
}
