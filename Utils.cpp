/*
Copyright 2010 Andy Simpson

This file is part of SI Simple.

SI Simple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SI Simple is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SI Simple.  If not, see <http://www.gnu.org/licenses/>.
*/

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
