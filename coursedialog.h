#ifndef COURSEDIALOG_H
#define COURSEDIALOG_H

#include <QDialog>
class Course

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

private:
    Ui::courseDialog *ui;
    bool m_New;
    CCourse* m_Course;
};

#endif // COURSEDIALOG_H
