#ifndef coursesdialog_H
#define coursesdialog_H

#include <QDialog>

namespace Ui {
    class coursesdialog;
}

class coursesdialog : public QDialog
{
    Q_OBJECT

public:
    explicit coursesdialog(QWidget *parent = 0);
    ~coursesdialog();

private slots:
    void runCourseDialog();
    void runNewCourseDialog();

private:
    Ui::coursesdialog *ui;
};

#endif // coursesdialog_H
