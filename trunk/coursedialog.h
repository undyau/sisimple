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
#ifndef COURSEDIALOG_H
#define COURSEDIALOG_H

#include <QDialog>
class CCourse;

namespace Ui {
    class courseDialog;
}

class courseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit courseDialog(QWidget *parent);
    courseDialog(QWidget *parent, CCourse* a_Course);
    ~courseDialog();
    QString m_Name;
    QString m_Length;
    QString m_Climb;
    QString m_Controls;

private slots:
    virtual void accept();
    virtual void enableCtrls();

private:
    Ui::courseDialog *ui;
    bool m_New;
    CCourse* m_Course;

    void init();
};

#endif // COURSEDIALOG_H
