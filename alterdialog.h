#ifndef ALTERDIALOG_H
#define ALTERDIALOG_H

#include <QDialog>
#include <QStringList>

class CResult;

namespace Ui {
    class AlterDialog;
}

class AlterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlterDialog(QWidget *parent, QStringList& a_Courses, CResult* a_Result);
    ~AlterDialog();

signals:
    void club(QString a_Club);
    void name(QString a_Name);
    void course(QString a_Course);

private slots:
    virtual void accept(); // OK button pressed

private:
    Ui::AlterDialog *ui;
    QStringList& m_Courses;
    CResult* m_Result;
};

#endif // ALTERDIALOG_H
