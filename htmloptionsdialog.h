#ifndef HTMLOPTIONSDIALOG_H
#define HTMLOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
    class HtmlOptionsDialog;
}

class HtmlOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HtmlOptionsDialog(QWidget *parent = 0);
    ~HtmlOptionsDialog();

private:
    Ui::HtmlOptionsDialog *ui;

private slots:
    void accept();
};

#endif // HTMLOPTIONSDIALOG_H
