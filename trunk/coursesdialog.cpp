#include "coursesdialog.h"
#include "ui_coursesdialog.h"
#include "CEvent.h"
#include "coursedialog.h"

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
    courseDialog dlg(this/*, course*/);
    dlg.exec();
}
