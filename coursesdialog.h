#ifndef coursesdialog_H
#define coursesdialog_H

#include <QDialog>
class CCourse;

namespace Ui {
    class coursesdialog;
}

class coursesdialog : public QDialog
{
    Q_OBJECT

public:
    explicit coursesdialog(QWidget *parent = 0);
    ~coursesdialog();

signals:
    void newCourse(CCourse*);
    void deleteCourse(CCourse*);


private slots:
    void runCourseDialog();
    void runNewCourseDialog();
    void enableCtrls();
    void deleteSelectedCourse();
    void deleteCourseName(QString a_Course);
    void addNewCourseName(QString a_Course);

private:
    Ui::coursesdialog *ui;
};

#endif // coursesdialog_H
