#include "coursesdialog.h"
#include "ui_coursesdialog.h"
#include "CEvent.h"
#include "coursedialog.h"
#include "CCourse.h"
#include <QMessageBox>

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
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedCourse()));
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

void coursesdialog::deleteSelectedCourse()
{
CCourse* course = CEvent::Event()->CourseFromName(ui->coursesList->selectedItems()[0]->text());
emit (deleteCourse(course));
}

void coursesdialog::deleteCourseName(QString a_Course)
{
    QList<QListWidgetItem*> items = ui->coursesList->findItems(a_Course,Qt::MatchFixedString);
    if (items.count() == 1)
        ui->coursesList->removeItemWidget(items[0]);
}

void coursesdialog::addNewCourseName(QString a_Course)
{
   ui->coursesList->addItem(a_Course);
}
