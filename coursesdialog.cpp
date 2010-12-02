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
#include "coursesdialog.h"
#include "ui_coursesdialog.h"
#include "CEvent.h"
#include "coursedialog.h"
#include "CCourse.h"
#include <QMessageBox>
#include <QDebug>
#include <set>

coursesdialog::coursesdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::coursesdialog)
{
    ui->setupUi(this);

    QStringList names;
    CEvent::Event()->GetCourseNames(names);
    ui->coursesList->addItems(names);

    connect(ui->newButton, SIGNAL(clicked()), this, SLOT(runNewCourseDialog()));
    connect(ui->EditButton, SIGNAL(clicked()), this, SLOT(runCourseDialog()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedCourses()));
    connect(ui->coursesList,SIGNAL(itemSelectionChanged()), this, SLOT(enableCtrls()));
    connect(this, SIGNAL(newCourse(CCourse*)), CEvent::Event(), SLOT(newCourse(CCourse*)));
    connect(this, SIGNAL(deleteCourse(CCourse*)), CEvent::Event(), SLOT(deleteCourse(CCourse*)));
    connect(CEvent::Event(), SIGNAL(newCourseExists(QString)), this, SLOT(addNewCourseName(QString)));
    connect(CEvent::Event(), SIGNAL(deletedCourse(QString)), this, SLOT(deleteCourseName(QString)));
    enableCtrls();
}

coursesdialog::~coursesdialog()
{
    delete ui;
}

void coursesdialog::runNewCourseDialog()
{
    courseDialog dlg(this);
    if (dlg.exec())
        {
        if (ui->coursesList->findItems(dlg.m_Name, Qt::MatchFixedString).count() > 0)
            {
            QMessageBox msg;
            msg.setText("Course already exists " + dlg.m_Name );
            msg.exec();
            return;
            }

        QString s = dlg.m_Controls;
        s.remove(" ");
        QStringList sl = s.split(",");
        CCourse* course = new CCourse(dlg.m_Name, dlg.m_Length, dlg.m_Climb, sl);
        emit newCourse(course);
        }
}

void coursesdialog::runCourseDialog()
    {
    CCourse* course = CEvent::Event()->CourseFromName(ui->coursesList->selectedItems()[0]->text());
    if (course)
        {
        courseDialog dlg(this, course);
        if (dlg.exec())
            {
            if (dlg.m_Name != course->GetName())
                {
                if (ui->coursesList->findItems(dlg.m_Name, Qt::MatchFixedString).count() > 0)
                    {
                    QMessageBox msg;
                    msg.setText("Course already exists " + dlg.m_Name );
                    msg.exec();
                    return;
                    }
                else
                    {
                    ui->coursesList->selectedItems()[0]->setText(dlg.m_Name);
                    }
                }
            QString s = dlg.m_Controls;
            s.remove(" ");
            QStringList sl = s.split(",");
            course->Alter(dlg.m_Name,
                                dlg.m_Length,
                                dlg.m_Climb,
                                sl);
            }
        }
    }

void coursesdialog::enableCtrls()
{
    int count = ui->coursesList->selectedItems().count();

    ui->EditButton->setEnabled(count == 1);
    ui->deleteButton->setEnabled(count > 0);
}

void coursesdialog::deleteSelectedCourses()
{
    std::set<CCourse*> victims;
    for (int i = 0; i < ui->coursesList->selectedItems().size(); i++)
        {
        QString name = ui->coursesList->selectedItems()[i]->text();
        CCourse* course = CEvent::Event()->CourseFromName(name);
        victims.insert(course);
        }

    for (std::set<CCourse*>::iterator c = victims.begin(); c != victims.end(); c++)
        emit (deleteCourse(*c));
}

void coursesdialog::deleteCourseName(QString a_Course)
{
for(int i = 0, len = ui->coursesList->count();i < len; ++i)
    if(ui->coursesList->item(i)->text() == a_Course)
        {
        delete ui->coursesList->takeItem(i);
        break;
        }
}

void coursesdialog::addNewCourseName(QString a_Course)
{
   ui->coursesList->addItem(a_Course);
}
