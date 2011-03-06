#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
class CSIDumper;
class QextSerialEnumerator;
class QextPortInfo;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
 void addDevice(const QextPortInfo & info );
 void removeDevice(const QextPortInfo & info );
 void tryDownload();
 void setSerialPort(const QString& a_SerialPort);
 void processCardCsv(QString);
 void dumperFinished(QString);

private:
    QString m_SerialPort;
    Ui::MainWindow *ui;
    QStringListModel m_SerialPorts;
    QextSerialEnumerator *m_Sdw;
    CSIDumper* m_Dumper;
    QStringList m_Cards;
};

#endif // MAINWINDOW_H
