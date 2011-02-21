#include "clockerrorsdialog.h"
#include "ui_clockerrorsdialog.h"
#include "CControlAdjustments.h"
#include <map>
#include "CCourse.h"
#include <QMessageBox>

clockerrorsdialog::clockerrorsdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::clockerrorsdialog), m_Changed(false)
{
    ui->setupUi(this);
    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(clickedSetButton()));

    CControlAdjustments::ControlAdjustments()->GetAllAdjustments(m_Adjustments);
    setsummary();

    QString note = QString(tr("Use -1 as start control number, -2 as finish control number\r\n"));
    note += tr("To reset adjustment for a control, set its adjustment time to 0");

    ui->notesLabel->setText(note);
}


clockerrorsdialog::~clockerrorsdialog()
{
    delete ui;
}

void clockerrorsdialog::setsummary()
{
    ui->listAdjustments->clear();
    for (std::map<int,int>::iterator i = m_Adjustments.begin(); i != m_Adjustments.end(); i++)
        {
        QString line = QString(tr("%1 adjusted by %2 seconds")).arg(i->first).arg(i->second);
        ui->listAdjustments->append(line);
        }
}

void clockerrorsdialog::clickedSetButton()
{
    bool ok;
    int control = ui->controlEdit->text().toInt(&ok);
    if (!ok)
        return;
    int seconds = ui->secondsEdit->text().toInt(&ok);
    if (!ok)
        return;

    m_Changed = true;
    m_Adjustments[control] = seconds;
    setsummary();
}

void clockerrorsdialog::accept()
    {
    if (m_Changed)
        {
        CControlAdjustments::ControlAdjustments()->SetAllAdjustments(m_Adjustments);
        QMessageBox msg;
        msg.setText(tr("You need to (re)load the competitor SI times.\r\nYou can keep or reload the courses."));
        msg.exec();
        }
    QDialog::accept();
    }
