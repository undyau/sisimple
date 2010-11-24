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
#ifndef ALTERDIALOG_H
#define ALTERDIALOG_H

#include <QDialog>
#include <QStringList>

class CResult;

namespace Ui {
    class AlterDialog;
}

class AlterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlterDialog(QWidget *parent, QStringList& a_Courses, CResult* a_Result);
    ~AlterDialog();

signals:
    void club(QString a_Club);
    void name(QString a_Name);
    void course(QString a_Course);

private slots:
    virtual void accept(); // OK button pressed

private:
    Ui::AlterDialog *ui;
    QStringList& m_Courses;
    CResult* m_Result;
};

#endif // ALTERDIALOG_H
