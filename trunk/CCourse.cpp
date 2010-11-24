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

#include "CCourse.h" // class's header file
#include <QStringList>
#include "Utils.h"
#include "CLeg.h"

// class constructor
CCourse::CCourse(QString& a_Data)
    {
    if (!ValidData(a_Data))
            return;
    QStringList courseParts;
    courseParts = a_Data.split("=");

    m_Name = courseParts[0];  // course name can be anything
    QString temp = courseParts[1];

    QStringList controls = temp.split(",");
    long lastCN(-1);
    long nextCN;
    int i(0);
    while (i < controls.count())
        {
        nextCN = ToLong(controls[i]);
        m_Controls.push_back(new CLeg(lastCN, nextCN));
        lastCN = nextCN;
        i++;
        }
    m_Controls.push_back(new CLeg(lastCN, -2));

    m_Length = courseParts[2];
    if (courseParts.count() > 3)
        m_Climb = courseParts[3];
    }

CCourse::CCourse(QString& a_Name, QString& a_Length, QString& a_Climb, QStringList& a_Controls)
    {
    Alter(a_Name, a_Length, a_Climb, a_Controls);
    }

// class destructor
CCourse::~CCourse()
    {

    }

bool CCourse::ValidData(QString& a_Data)
    {
// Course data is delimted by '=' signs
// First part is course name, then comma separated list of control codes, distance, climb
// Climb is optional
    QStringList courseParts;
    courseParts = a_Data.split("=");

    if (courseParts.count() < 2 || courseParts.count() > 4)
            return false;

    QString temp = courseParts[1];
    QStringList controls = temp.split(",");

    QRegExp reControl("[0-9]+");
    for (int i = 0; i < controls.count(); i++)
        {
        if (!reControl.exactMatch(controls[i]))
            return false;
        }

    QRegExp reLength("[0-9]+\\.?[0-9]*");
    if (!reLength.exactMatch(courseParts[2]))
        return false;

    if(courseParts.count() > 3)
        {
        QRegExp reClimb("[0-9]+");
        if (!reClimb.exactMatch(courseParts[3]))
            return false;
        }
    return true;
    }


void CCourse::SetOptionalLeg(int a_Index)
    {
    if (a_Index >  (int) m_Controls.size())
        {
        QString s("Cannot make leg %1 optional, course only has %2 legs");
        s = s.arg(a_Index).arg(m_Controls.size());
        SIMessageBox(s, QMessageBox::Warning);
        return;
        }

    CLeg* leg = m_Controls[a_Index];
    if (leg->GetUntimed())
        {
        QString s("Cannot make leg %1 optional, it is an \"untimed\" leg");
        s = s.arg(a_Index);
        SIMessageBox(s, QMessageBox::Warning);
        return;
        }

    leg->SetOptional(true);
    }

bool CCourse::GetOptionalLeg(int a_Index)
    {
    if (a_Index > (int) m_Controls.size())
        return false;

    return m_Controls[a_Index]->GetOptional();
    }

void CCourse::SetUntimedLeg(int a_Index)
    {
    if (a_Index > (long) m_Controls.size())
        {
        QString s("Cannot make leg %1 untimed, course only has %2 legs");
        s = s.arg(a_Index).arg(m_Controls.size());
        SIMessageBox(s, QMessageBox::Warning);
        return;
        }
    CLeg* leg = m_Controls[a_Index];

    if (leg->GetOptional())
        {
        QString s("Cannot make leg %1 untimed, it is optional on this course");
        s = s.arg(a_Index);
        SIMessageBox(s, QMessageBox::Warning);
        return;
        }

    leg->SetUntimed(true);
    }

bool CCourse::GetUntimedLeg(int a_Index)
    {
    if (a_Index > (int) m_Controls.size())
        return false;

    return m_Controls[a_Index]->GetUntimed();
    }

void CCourse::CompulsoryControls(std::list<long>& a_Controls)
    {
    for (unsigned int i = 0; i < m_Controls.size() - 1; i++) // skip finish leg
        if (!m_Controls[i]->GetOptional())
            a_Controls.push_back(m_Controls[i]->GetEndCN());
    }

/*void CCourse::UntimedLegs(std::set<long>& a_Legs)
    {
    a_Controls = m_UntimedControls;
    }*/
CLeg CCourse::GetLeg(int a_Index)
    {
    return *m_Controls[a_Index];
    }

QString CCourse::TextDescStr()
{
    QString s;
    if (m_Controls.size() > 1)
        {
        if (!m_Climb.isEmpty())
            {
            s = "%1 Course: %2 controls %3 km %4 m";
            s = s.arg(m_Name).arg(m_Controls.size() -1).arg(m_Length).arg(m_Climb);
            }
        else
            {
            s = "%1 Course: %2 controls %3 km";
            s = s.arg(m_Name).arg(m_Controls.size() -1).arg(m_Length);
            }
        }
    else
        {
        s = "%s Course: %s km";
        s = s.arg(m_Name).arg(m_Length);
        }
    return s;
}

QString CCourse::TextSplitHdrStr()
{
    QString result("  # Name                         Result   Start   ");
    QString s;
    for (int i = 0; i < (int)m_Controls.size() -1; i++)
        {
        s = " %1.(%2) ";
        s = s.arg(i+1, 2).arg(m_Controls[i]->GetEndCN(), 3);
        result += s;
        }
    result += "  Finish.           min/km";
    return result;
}

void CCourse::GetControls(QStringList& a_Controls) const
{
    QString s;
    for (int i = 0; i < (int)m_Controls.size() -1; i++)
        {
        s = QString("%1").arg(m_Controls[i]->GetEndCN());
        a_Controls.append(s);
        }
}

void CCourse::Alter(QString& a_Name, QString& a_Length, QString& a_Climb, QStringList a_Controls)
    {
    m_Name = a_Name;
    m_Length = a_Length;
    m_Climb = a_Climb;
    m_Controls.clear();

    long lastCN(-1);
    long nextCN;
    int i(0);
    while (i < a_Controls.count())
        {
        nextCN = ToLong(a_Controls[i]);
        m_Controls.push_back(new CLeg(lastCN, nextCN));
        lastCN = nextCN;
        i++;
        }
    m_Controls.push_back(new CLeg(lastCN, -2));
    }
