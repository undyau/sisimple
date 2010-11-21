#ifndef COURSEDIALOG_H
#define COURSEDIALOG_H

#include <QDialog>
class CCourse;

namespace Ui {
    class courseDialog;
}

class courseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit courseDialog(QWidget *parent);
    courseDialog(QWidget *parent, CCourse* a_Course);
    ~courseDialog();
    QString m_Name;
    QString m_Length;
    QString m_Climb;
    QString m_Controls;

private slots:
    virtual void accept();
    virtual void enableCtrls();

private:
    Ui::courseDialog *ui;
    bool m_New;
    CCourse* m_Course;

    void init();
};

#endif // COURSEDIALOG_H
