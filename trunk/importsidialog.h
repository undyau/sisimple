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

#ifndef IMPORTSIDIALOG_H
#define IMPORTSIDIALOG_H

#include <QDialog>

class QNetworkReply;
class QNetworkAccessManager;

namespace Ui {
    class ImportSIDialog;
}

class ImportSIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportSIDialog(QWidget *parent = 0);
    ~ImportSIDialog();

private slots:
    void accept(); // Load the SI data
    void textChanged(QString); // Some text changed in an edit box
    void replyFinished(QNetworkReply* reply); // got a reply from web

private:
    Ui::ImportSIDialog *ui;
    QNetworkAccessManager* m_Manager;
    void Download();
    void ReadFile();
    void EnableCtrls();
};

#endif // IMPORTSIDIALOG_H
