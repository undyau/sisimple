#include "htmloptionsdialog.h"
#include "ui_htmloptionsdialog.h"
#include "QSettings"

HtmlOptionsDialog::HtmlOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HtmlOptionsDialog)
{
    ui->setupUi(this);
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("HTML Options");
    ui->wholePageCheckBox->setChecked(settings.value("WholePage",false).toBool());
    ui->preSplitsHeaderLineEdit->setText(settings.value("preSplitsHeader","<div>").toString());
    ui->postSplitsHeaderLineEdit->setText(settings.value("postSplitsHeader","</div>").toString());
    ui->preSplitsEvenLineEdit->setText(settings.value("preSplitsEven","<div title=\"{competitor}\">").toString());
    ui->postSplitsEvenLineEdit->setText(settings.value("postSplitsEven","</div>").toString());
    ui->preSplitsOddLineEdit->setText(settings.value("preSplitsEven","<div title=\"{competitor}\">").toString());
    ui->postSplitsOddLineEdit->setText(settings.value("postSplitsEven","</div>").toString());
    settings.endGroup();
}

HtmlOptionsDialog::~HtmlOptionsDialog()
{
    delete ui;
}

void HtmlOptionsDialog::accept()
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("HTML Options");
    settings.setValue("WholePage", ui->wholePageCheckBox->isChecked());
    settings.setValue("preSplitsHeader", ui->preSplitsHeaderLineEdit->text());
    settings.setValue("postSplitsHeader",ui->postSplitsHeaderLineEdit->text());
    settings.setValue("preSplitsEven",ui->preSplitsEvenLineEdit->text());
    settings.setValue("postSplitsEven",ui->postSplitsEvenLineEdit->text());
    settings.setValue("preSplitsEven",ui->preSplitsOddLineEdit->text());
    settings.setValue("postSplitsEven",ui->postSplitsOddLineEdit->text());
    settings.endGroup();
}
