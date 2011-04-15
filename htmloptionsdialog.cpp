#include "htmloptionsdialog.h"
#include "ui_htmloptionsdialog.h"
#include "chtmloptions.h"

HtmlOptionsDialog::HtmlOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HtmlOptionsDialog)
{
    ui->setupUi(this);
    CHtmlOptions options;
    ui->wholePageCheckBox->setChecked(options.getWholePage());
    ui->ShowNameCheck->setChecked(options.getShowNameTooltip());
    ui->BehindCssEdit->setText(options.getBehindCss());
    ui->ElapsedCssEdit->setText(options.getElapsedCss());
    ui->FastCssEdit->setText(options.getFastCss());
    ui->FastestCssEdit->setText(options.getFastestCss());
    ui->HeaderCssEdit->setText(options.getHeaderCss());
    ui->LegCssEdit->setText(options.getLegCss());
    ui->SlowCssEdit->setText(options.getSlowCss());

}

HtmlOptionsDialog::~HtmlOptionsDialog()
{
    delete ui;
}

void HtmlOptionsDialog::accept()
{
    CHtmlOptions options;
    options.setWholePage(ui->wholePageCheckBox->isChecked());
    options.setShowNameTooltip(ui->ShowNameCheck->isChecked());
    options.setBehindCss(ui->BehindCssEdit->text());
    options.setElapsedCss(ui->ElapsedCssEdit->text());
    options.setFastCss(ui->FastCssEdit->text());
    options.setFastestCss(ui->FastestCssEdit->text());
    options.setHeaderCss(ui->HeaderCssEdit->text());
    options.setLegCss(ui->LegCssEdit->text());
    options.setSlowCss(ui->SlowCssEdit->text());
    QDialog::accept();
}
