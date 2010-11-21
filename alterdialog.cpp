#include "alterdialog.h"
#include "ui_alterdialog.h"
#include "CResult.h"
#include "CCourse.h"
//#include <QDebug>

AlterDialog::AlterDialog(QWidget *parent, QStringList& a_Courses, CResult* a_Result) :
    QDialog(parent), ui(new Ui::AlterDialog), m_Courses(a_Courses), m_Result(a_Result)

{
    ui->setupUi(this);
    ui->classComboBox->addItems(m_Courses);
    int index;
    if ((index = ui->classComboBox->findText(m_Result->GetCourse()->GetName())) >= 0)
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
