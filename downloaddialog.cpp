#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QSerialPortInfo>
#include <QSerialPort>
//#include <QColorGroup>
#include "CEvent.h"

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog),m_Dumper(NULL),m_CardCount(0)
{
    ui->setupUi((QDialog*)this);
    ui->lcdNumber->setSegmentStyle( QLCDNumber::Filled );
    QPalette palette = ui->lcdNumber->palette();
   // palette.setColor(QPalette::Normal, QPalette::Foreground, Qt::red);
   // palette.setColor(QPalette::Normal, QPalette::Background, Qt::black);
    palette.setColor(QPalette::Normal, QPalette::Light, Qt::green);
    palette.setColor(QPalette::Normal, QPalette::Dark, Qt::darkGreen);
    ui->lcdNumber->setPalette(palette);

    ui->comboBox->setModel(&m_SerialPorts);
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(tryDownload()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
    connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(setSerialPort(QString)));

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::iterator i;
    for (i = ports.begin(); i != ports.end(); i++)
        addDevice(*i);

    connect (this, SIGNAL(DownloadDataRead(QStringList)), CEvent::Event(), SLOT(NewDownloadData(QStringList)));
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::addDevice(const QSerialPortInfo & info)
{
    QString description = QString("%1 - %2").arg(info.portName() +" ", info.description());
    ui->comboBox->addItem(description);
    if (ui->comboBox->count() == 1)
        setSerialPort(info.portName());
}

void DownloadDialog::removeDevice(const QSerialPortInfo & info)
{
    int i;
    QString description = QString("%1 - %2").arg(info.portName() +" ", info.description());
    if ((i = ui->comboBox->findText(description)) >= 0)
        ui->comboBox->removeItem(i);
}

void DownloadDialog::setSerialPort(const QString& a_SerialPort)
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::iterator i;
    for (i = ports.begin(); i != ports.end(); i++)
        {
        QString description = QString("%1 - %2").arg((*i).portName() +" ", (*i).description());
        if (description == a_SerialPort)
            {
            m_SerialPort = (*i).portName();
            break;
            }
        }
}

void DownloadDialog::tryDownload()
{
    if (m_SerialPort.isEmpty())
        m_SerialPort = ui->comboBox->currentText();

    if (!m_SerialPort.isEmpty())
        {
        if (m_Dumper == NULL)
            {
            m_DownloadOK = false;
            ui->closeButton->setEnabled(false);
            ui->downloadButton->setEnabled(false);
            m_Dumper = new CSIDumper();
            m_Dumper->SetSerialPort(m_SerialPort);
            connect(m_Dumper, SIGNAL(CardCsv(QString)), this, SLOT(processCardCsv()));
            connect(m_Dumper, SIGNAL(Finished(int, QString)), this, SLOT(dumperFinished(int, QString)));
            connect(m_Dumper, SIGNAL(StatusUpdate(QString)), ui->labelStatus, SLOT(setText(QString)));
            connect(m_Dumper, SIGNAL(ErrorOcurred(QString)), this, SLOT(dumperError(QString)));
            m_Dumper->tryDownload();
            }
        }
}

void DownloadDialog::dumperFinished(int a_Count, QString a_Summary)
{
    // send out the data and close the dialog
    ui->labelStatus->setText(a_Summary);
    ui->closeButton->setEnabled(true);
    ui->downloadButton->setEnabled(true);
    m_DownloadOK = a_Count > 0;

}

void DownloadDialog::dumperError(QString a_Summary)
{
    // send out the data and close the dialog
    ui->labelStatus->setText(a_Summary);
    ui->closeButton->setEnabled(true);
    ui->downloadButton->setEnabled(true);
    m_DownloadOK = false;
}

void DownloadDialog::processCardCsv()
{
    m_CardCount++;
    ui->lcdNumber->display(m_CardCount);
}

void DownloadDialog::closeDialog()
{
    if (m_DownloadOK && m_Dumper)
        {
        hide();
        QStringList rawdata = m_Dumper->GetAllDataCsv();
        emit DownloadDataRead(rawdata);
        }
    delete m_Dumper;
    m_Dumper = NULL;
    done(1);
}
