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

QString TimeTakenTo0BasedTime(QString a_TimeTaken)
{
    QRegExp fixableRE("^[0-9]{1,4}:[0-9]{2,2}$");
    if (!fixableRE.exactMatch(a_TimeTaken))
        return a_TimeTaken;

    QString secs = a_TimeTaken.right(2);
    long mins = a_TimeTaken.left(a_TimeTaken.length()-3).toLong();
    long hours = mins/60;
    mins %= 60;

    QString result = QString("%1:%2:%3")
        .arg(hours,2,10, QChar('0'))
        .arg(mins,2,10, QChar('0'))
        .arg(secs);
    return result;
}

QString CssAsHtmlAttr(QString a_Css)
{
    QString res;
    if (!a_Css.isEmpty())
        res = "style=\"" + a_Css + "\"";
    return res;
}

//***************************************************************************
//** Autor: Jürgen Ehms
//**
//** Filename: crc529.c
//**
//** Description: Programm to generate 16 BIT CRC
//**
//** Return values: 16 BIT CRC
//**
//** Errormessages: none
//**
//** Version    last change    description
//**
//** 1.00       07.09.2004
//***************************************************************************


#define POLYNOM 0x8005


unsigned int crc(unsigned int uiCount,unsigned char *pucDat)
{
short int iTmp;
unsigned short int uiTmp,uiTmp1,uiVal;
unsigned char *pucTmpDat;

if (uiCount < 2) return(0);        // response value is "0" for none or one data byte
pucTmpDat = pucDat;

uiTmp1 = *pucTmpDat++;
uiTmp1 = (uiTmp1<<8) + *pucTmpDat++;

if (uiCount == 2) return(uiTmp1);   // response value is CRC for two data bytes
for (iTmp=(int)(uiCount>>1);iTmp>0;iTmp--)
{

if (iTmp>1)
{
  uiVal = *pucTmpDat++;
  uiVal= (uiVal<<8) + *pucTmpDat++;
}
else
{
  if (uiCount&1)               // odd number of data bytes, complete with "0"
  {
    uiVal = *pucTmpDat;
    uiVal= (uiVal<<8);
  }
  else
  {
    uiVal=0; //letzte Werte mit 0
  }
}

for (uiTmp=0;uiTmp<16;uiTmp++)
{
   if (uiTmp1 & 0x8000)
   {
      uiTmp1  <<= 1;
      if (uiVal & 0x8000)uiTmp1++;
      uiTmp1 ^= POLYNOM;
   }
   else
   {
      uiTmp1  <<= 1;
      if (uiVal & 0x8000)uiTmp1++;
   }
   uiVal <<= 1;
 }
}
return(uiTmp1);
}
