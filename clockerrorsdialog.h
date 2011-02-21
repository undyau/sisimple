#ifndef CLOCKERRORSDIALOG_H
#define CLOCKERRORSDIALOG_H

#include <QDialog>
#include <map>

namespace Ui {
    class clockerrorsdialog;
}

class clockerrorsdialog : public QDialog
{
    Q_OBJECT

public:
    explicit clockerrorsdialog(QWidget *parent = 0);
    ~clockerrorsdialog();

private slots:
    void clickedSetButton();
    virtual void accept();

private:
    Ui::clockerrorsdialog *ui;
    std::map<int, int>m_Adjustments;
    void setsummary();
    bool m_Changed;
};

#endif // CLOCKERRORSDIALOG_H
