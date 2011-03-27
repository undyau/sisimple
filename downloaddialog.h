#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include "qextserialenumerator.h"
#include <QStringListModel>
#include "csidumper.h"

namespace Ui {
    class DownloadDialog;
}

class DownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(QWidget *parent = 0);
    ~DownloadDialog();

public slots:
 void addDevice(const QextPortInfo & info );
 void removeDevice(const QextPortInfo & info );
 void processCardCsv(QString);
 void dumperFinished(int, QString);
 void tryDownload();
 void setSerialPort(const QString& a_SerialPort);
 void closeDialog();

signals:
    void DownloadDataRead(QStringList);

private:
    Ui::DownloadDialog *ui;
    QextSerialEnumerator* m_Sdw;
    QStringListModel m_SerialPorts;
    CSIDumper* m_Dumper;
    QString m_SerialPort;
    int m_CardCount;
    bool m_DownloadOK;
};

#endif // DOWNLOADDIALOG_H