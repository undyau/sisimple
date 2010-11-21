#include "coursedialog.h"
#include "ui_coursedialog.h"
#include "CCourse.h"

courseDialog::courseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::courseDialog), m_New(true), m_Course(NULL)
{
    ui->setupUi(this);
}


courseDialog::courseDialog(QWidget *parent, CCourse* a_Course) :
    QDialog(parent),
    ui(new Ui::courseDialog), m_New(false), m_Course(a_Course)
{
    ui->setupUi(this);

    ui->nameEdit->setText(a_Course->GetName());
    ui->lengthEdit->setText(a_Course->GetLength());
    a_Course->
}

courseDialog::~courseDialog()
{
    delete ui;
}
