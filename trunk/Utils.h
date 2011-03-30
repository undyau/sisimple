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
/* Utility functons */

#ifndef __UTILS_H__
#define __UTILS_H__
#include <QString>
#include <QDateTime>
#include <QMessageBox>

int SIMessageBox(const QString&  a_Message,
                QMessageBox::Icon a_Icon = QMessageBox::NoIcon,
                QMessageBox::StandardButtons a_Buttons = QMessageBox::Ok,
                const QString&  a_Caption = QString("SI Simple"),
                QWidget*  a_Parent = NULL
                );

QString SimplifyPath(QString a_Path);

long ToLong(const QString& a_Value);
QDateTime ToDateTime(const QString& a_Value);
int DOWToInt(const QString& a_DOW);
//QString FormatTimeTaken(long a_Secs, bool a_NullOK = false);
QString FormatTimeTaken(long a_Secs);
QString TimeTakenTo0BasedTime(QString a_TimeTaken);

unsigned int crc(unsigned int uiCount,unsigned char *pucDat);    // Supplied by SI
#endif
