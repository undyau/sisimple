#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include "qextserialenumerator.h"
#include "qextserialport.h"
#include <QColorGroup>

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog),m_CardCount(0)
{
    ui->setupUi(this);
    ui->lcdNumber->setSegmentStyle( QLCDNumber::Filled );
    QPalette palette = ui->lcdNumber->palette();
   // palette.setColor(QPalette::Normal, QPalette::Foreground, Qt::red);
   // palette.setColor(QPalette::Normal, QPalette::Background, Qt::black);
    palette.setColor(QPalette::Normal, QPalette::Light, Qt::green);
    palette.setColor(QPalette::Normal, QPalette::Dark, Qt::darkGreen);
    ui->lcdNumber->setPalette(palette);

    ui->comboBox->setModel(&m_SerialPorts);
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(tryDownload()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(setSerialPort(QString)));

    m_Sdw = new QextSerialEnumerator();
    connect(m_Sdw, SIGNAL(deviceDiscovered(QextPortInfo)), this, SLOT(addDevice(QextPortInfo)));
    connect(m_Sdw, SIGNAL(deviceRemoved(QextPortInfo)), this, SLOT(removeDevice(QextPortInfo)));
    m_Sdw->setUpNotifications( );
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::addDevice(const QextPortInfo & info)
{
    QString description = QString("%1 (%2)").arg(info.portName).arg(info.friendName);
    ui->comboBox->addItem(info.friendName);
    if (ui->comboBox->count() == 1)
        setSerialPort(info.friendName);
}

void DownloadDialog::removeDevice(const QextPortInfo & info)
{
    int i;
    if ((i = ui->comboBox->findText(info.friendName)) >= 0)
        ui->comboBox->removeItem(i);
}

void DownloadDialog::setSerialPort(const QString& a_SerialPort)
{
    QList<QextPortInfo> ports = m_Sdw->getPorts();
    QList<QextPortInfo>::iterator i;
    for (i = ports.begin(); i != ports.end(); i++)
        if ((*i).friendName == a_SerialPort)
            {
            m_SerialPort = (*i).portName;
            break;
            }
}

void DownloadDialog::tryDownload()
{
    if (!m_SerialPort.isEmpty())
        {
        if (m_Dumper == NULL)
            {
            m_Dumper = new CSIDumper();
            m_Dumper->SetSerialPort(m_SerialPort);
            connect(m_Dumper, SIGNAL(CardCsv(QString)), this, SLOT(processCardCsv(QString)));
            connect(m_Dumper, SIGNAL(Finished(QString)), this, SLOT(dumperFinished(QString)));
            m_Dumper->tryDownload();
            }
        }
}

void DownloadDialog::dumperFinished(int a_Count, QString a_Summary)
{
    // send out the data and close the dialog
    delete m_Dumper;
    m_Dumper = NULL;
}

void DownloadDialog::processCardCsv(QString a_CardData)
{
    m_CardCount++;
    ui->lcdNumber->display(m_CardCount);
}
