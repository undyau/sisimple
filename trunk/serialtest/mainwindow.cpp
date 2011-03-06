#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include <QMessageBox>
#include <Qtdebug>
#include "sidumprecord.h"
#include "csidumper.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_SerialPorts(this), m_Dumper(NULL)
{
    ui->setupUi(this);
    ui->comboBox->setModel(&m_SerialPorts);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(tryDownload()));
    connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(setSerialPort(QString)));

    m_Sdw = new QextSerialEnumerator();
    connect(m_Sdw, SIGNAL(deviceDiscovered(QextPortInfo)), this, SLOT(addDevice(QextPortInfo)));
    connect(m_Sdw, SIGNAL(deviceRemoved(QextPortInfo)), this, SLOT(removeDevice(QextPortInfo)));
    m_Sdw->setUpNotifications( );
}

MainWindow::~MainWindow()
{
    delete m_Sdw;
    delete ui;
    if (m_Dumper)
        delete m_Dumper;
}

void MainWindow::addDevice(const QextPortInfo & info)
{
    QString description = QString("%1 (%2)").arg(info.portName).arg(info.friendName);
    ui->comboBox->addItem(info.friendName);
    if (ui->comboBox->count() == 1)
        setSerialPort(info.friendName);
}

void MainWindow::removeDevice(const QextPortInfo & info)
{
    int i;
    if ((i = ui->comboBox->findText(info.friendName)) >= 0)
        ui->comboBox->removeItem(i);
}

void MainWindow::setSerialPort(const QString& a_SerialPort)
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

void MainWindow::tryDownload()
{
    if (!m_SerialPort.isEmpty())
        {
        if (m_Dumper == NULL)
            {
            m_Cards.empty();
            m_Dumper = new CSIDumper();
            m_Dumper->SetSerialPort(m_SerialPort);
            connect(m_Dumper, SIGNAL(CardCsv(QString)), this, SLOT(processCardCsv(QString)));
            connect(m_Dumper, SIGNAL(Finished(QString)), this, SLOT(dumperFinished(QString)));
            connect(m_Dumper, SIGNAL(Finished(QString)), ui->textEdit, SLOT(setText(QString)));
            connect(m_Dumper, SIGNAL(CardCsv(QString)), ui->textEdit, SLOT(setText(QString)));
            m_Dumper->tryDownload();
            }
        }
}

void MainWindow::dumperFinished(QString)
{
    delete m_Dumper;
    m_Dumper = NULL;
}

void MainWindow::processCardCsv(QString a_CardData)
{
    m_Cards.append(a_CardData);
}
