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
    connect(ui->coursesList,SIGNAL(itemSelectionChanged()), this, SLOT(enableCtrls()));
    enableCtrls();
}

coursesdialog::~coursesdialog()
{
    delete ui;
}

void coursesdialog::runNewCourseDialog()
{
    courseDialog dlg(this);
    dlg.exec();
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
            course->Alter(dlg.m_Name,
                                dlg.m_Length,
                                dlg.m_Climb,
                                dlg.m_Controls.split(","));
            }
        }
    }

void coursesdialog::enableCtrls()
{
    int count = ui->coursesList->selectedItems().count();

    ui->EditButton->setEnabled(count == 1);
    ui->deleteButton->setEnabled(count > 0);
}
