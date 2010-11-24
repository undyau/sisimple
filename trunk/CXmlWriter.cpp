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

#include "CXmlWriter.h" // class's header file
#include "Utils.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>

// class constructor
CXmlWriter::CXmlWriter(QString a_DocType, QString a_DtdName): m_GotValue(false)
{
    m_Output = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    m_Output += "\n<!DOCTYPE " + a_DocType +
                " SYSTEM " + '"' + a_DtdName + "\">";
    m_GotValue = true;
}

// class destructor
CXmlWriter::~CXmlWriter()
{
    // insert your code here
}

void CXmlWriter::StartElement(QString  a_ElementName, QString a_Attributes)
{
    if (!m_GotValue)
        m_Output += ">";

    m_Elements.push(a_ElementName);
    m_Output += "\n<" + a_ElementName;
    if (!a_Attributes.isEmpty())
        m_Output += " " + a_Attributes;
    m_GotValue = false;
    m_LastElement = a_ElementName;
}

void CXmlWriter::EndElement()
{
if (m_Elements.size() < 1)
    return;

if (m_GotValue == false && m_LastElement == m_Elements.top())
    m_Output += "/>";
else
    m_Output += "</" + m_Elements.top() + ">";

m_Elements.pop();
m_GotValue = true;
}

void CXmlWriter::AddValue(QString a_Value)
{
    if (!m_GotValue)
        m_Output += ">";
    m_GotValue = true;
    m_Output += a_Value;
}

void CXmlWriter::AddValue(long a_Value)
{
    if (!m_GotValue)
        m_Output += ">";
    m_GotValue = true;
    QString s = QString("%1").arg(a_Value);
    m_Output += s;
}

bool CXmlWriter::WriteToFile(QString a_File)
{
    QFile tfile(a_File);
    if (tfile.exists())
        {
        tfile.remove();
        if (tfile.exists())
            {
            SIMessageBox("Unable to clean up old file " + a_File + " to write XML.");
            return false;
            }
        }

    if (!tfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
        SIMessageBox("Unable to create file " + a_File + " to write XML.");
        return false;
        }

    // break m_Output by \n token and write to file line-wise

    QStringList lines(m_Output.split('\n'));
    QTextStream out(&tfile);
    for (QStringList::const_iterator i = lines.constBegin(); i != lines.constEnd(); i++)
        out << *i << "\n";

    tfile.close();
    return true;
}
