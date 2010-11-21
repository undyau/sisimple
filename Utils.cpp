#include "Utils.h"
#include <QMessageBox>
#include <QDir>
#include <QStringList>
//#include <QDebug>

int SIMessageBox(const QString&  a_Message,
                 QMessageBox::Icon a_Icon,
                 QMessageBox::StandardButtons a_Buttons,
                 const QString&  a_Caption,
                 QWidget*  a_Parent
                  )
{
    QMessageBox dlg(a_Icon, a_Caption, a_Message, a_Buttons, a_Parent);
    return dlg.exec();
}

QString SimplifyPath(QString a_Path)
{
    QString docdir =  QDir::homePath();
    if (a_Path.left(docdir.length() + 1) ==  docdir + QDir::separator())
        return a_Path.mid(docdir.length() + 1);
    else
        return a_Path;
}

long ToLong(const QString& a_Value)
{
    long result;
    bool ok(false);
    result = a_Value.toLong(&ok);
    if (ok)
        return result;
    else
        return 0;
}

QDateTime ToDateTime(const QString& a_Value)
{
    QTime time;
    QDateTime result;

    if (!a_Value.isEmpty())
        {
        time = QTime::fromString(a_Value, "hh:mm:ss");
        result.setTime(time);
        result.setDate(QDate::currentDate());
        }
    return result;
}

int DOWToInt(const QString& a_DOW)
{
    if (a_DOW.isEmpty())
        return -1;
    if (a_DOW == "Su") return 0;
    if (a_DOW == "Mo") return 1;
    if (a_DOW == "Tu") return 2;
    if (a_DOW == "We") return 3;
    if (a_DOW == "Th") return 4;
    if (a_DOW == "Fr") return 5;
    if (a_DOW == "Sa") return 6;
    return -2;
}

QString FormatTimeTaken(long a_Secs)
{
    if (a_Secs < 0)
        return "";

    short minutes = a_Secs /60;
    a_Secs = a_Secs - (minutes * 60);

    QString s = QString("%1:%2").arg(minutes).arg(a_Secs, 2, 10, QLatin1Char('0'));
    return s;
}
