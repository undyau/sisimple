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
#endif
