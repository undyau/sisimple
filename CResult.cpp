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

#include "CResult.h" // class's header file
#include "CSidetails.h"
#include "CEvent.h"
#include "Utils.h"
#include "CCourse.h"
#include "CLegstat.h"
#include <QStringList>
#include <QDebug>
#include <QSettings>
#include "chtmloptions.h"

// class constructor
CResult::CResult(QString& a_RawData) : m_RawData(a_RawData), m_ProcessedResult(false),
    m_Invalid(false), m_Finished(true), m_Pos(0), m_FinishedOverride(false), m_FinishedOverrideSet(false),
    m_Course(NULL), m_Altered(false), m_Disqualified(false)
    {
    QStringList array = m_RawData.split(',');

    m_RawIndex = array[0].toLong();
    m_SINumber = array[2].toLong();

    CSiDetails* detail;
    if ((detail = CEvent::Event()->GetSIData(m_SINumber)) != NULL)
        {
        m_Name = detail->GetName();
        m_Club = detail->GetClub();
        }

    if (!array[5].isEmpty() && m_Name.isEmpty())
        m_Name = array[5] + ' ' + array[6];

    if (!array[7].isEmpty() && m_Club.isEmpty())
        m_Club = array[7].left(3);

    m_Clear.SetData(ToLong(array[16]), array[17], ToDateTime(array[18]));
    m_Check.SetData(ToLong(array[19]), array[20], ToDateTime(array[21]));
    m_Start.SetData(-1, array[23], ToDateTime(array[24]));
    m_Finish.SetData(-2, array[26], ToDateTime(array[27]));

    // Now try to find all the punch data
    unsigned long punches = ToLong(array[28]);
    for (unsigned long i = 0; i < punches; i++)
        m_Punches.push_back(new CPunch(ToLong(array[(i*3)+29]), array[(i*3)+30], ToDateTime(array[(i*3)+31])));

    DoTimeSanityCheck();
    }

CResult::CResult(long a_RawIndex, long a_SINumber, QString a_Name, QString a_Club, QString a_Time,
                 QString m_Status, QStringList& a_Controls, QStringList& a_Splits) :
                 m_ProcessedResult(false), m_Invalid(false), m_Pos(0),
                 m_FinishedOverride(false),
                 m_FinishedOverrideSet(false), m_Course(NULL), m_Altered(false),
                 m_Disqualified(false),  m_SINumber(a_SINumber),
                 m_RawIndex(a_RawIndex), m_Name(a_Name), m_Club(a_Club)
   {

    m_RawData = QString("%1%2%3%4").arg(a_SINumber).arg(a_Name).arg(a_Club).arg(a_Time);

    m_Clear.SetData(10, "", ToDateTime("00:00:00"));
    m_Check.SetData(20, "", ToDateTime("00:00:00"));
    m_Start.SetData(-1, "", ToDateTime("00:00:00"));
    m_Finish.SetData(-2, "", ToDateTime(a_Time));

    unsigned long punches = a_Controls.size();
    for (unsigned long i = 0; i < punches; i++)
        m_Punches.push_back(new CPunch(ToLong(a_Controls.at(i)), "", ToDateTime(a_Splits.at(i))));

    m_Finished = m_Status == "OK";
    m_Invalid = (m_Status == "DidNotFinish" || m_Status == "MisPunch");
    m_Disqualified = m_Status == "Disqualified";
    }

// class destructor
CResult::~CResult()
    {
    for (std::list<CPunch*>::iterator i = m_Punches.begin(); i != m_Punches.end(); i++)
        delete (*i);

    ClearLegStats();
    }

bool CResult::ValidData(QString& a_Data)
    {
    QStringList tokens = a_Data.split(',');
    if (tokens.count() < 28)
        return false;

    if (tokens[0] == "No.") // header record
        return false;

    QString temp = tokens[2];
    bool ok(false);
    temp.toLong(&ok);
    return ok;
    }

void CResult::DoTimeSanityCheck()
    {
    QDateTime timeMark = m_Start.GetWhen();
    bool timeAdjusted(false);
    bool ignoredPunches(false);

    if (!m_Start.GetWhen().isValid())
        {
        CEvent::Event()->LogResultProblem(this, "No start time, marked invalid");
        m_Invalid = true;
        }


    for (std::list<CPunch*>::iterator i = m_Punches.begin(); i != m_Punches.end(); i++)
        {
        if ((*i)->GetWhen() < timeMark)
            {
            // Check for date rollover
            if (timeMark > (*i)->GetWhen().addSecs(6*3600) && timeMark < (*i)->GetWhen().addSecs(12*3600))
                {
                (*i)->SetWhen((*i)->GetWhen().addSecs(12*3600));
                timeAdjusted = true;
                }
            else
                {
                // skip control if last valid control was start ?
                ignoredPunches = true;
                (*i)->SetIgnore(true);
                }
            }
        else
            {
            timeMark = (*i)->GetWhen();
            }
        }

    if (!m_Finish.GetWhen().isValid())
        {
        CEvent::Event()->LogResultProblem(this, "No finish time, marked invalid");
        m_Invalid = true;
        }
    else
        {
        if (m_Finish.GetWhen() < timeMark)
            {
            QDateTime plus6(timeMark);
            QDateTime plus12(timeMark);
            plus6.addSecs(6*3600);
            plus12.addSecs(12*3600);

            // Check for date rollover
            if (timeMark > m_Finish.GetWhen().addSecs(6*3600) && timeMark < m_Finish.GetWhen().addSecs(12*3600))
                {
                m_Finish.SetWhen(m_Finish.GetWhen().addSecs(12*3600));
                timeAdjusted = true;
                }
            else
                {
                // go backwards and ignore any controls punched after the finish, if that leaves any
                int j(0);
                for (std::list<CPunch*>::iterator i = m_Punches.begin(); i != m_Punches.end(); i++)
                    {
                    if ((*i)->GetWhen() > m_Finish.GetWhen())
                        (*i)->SetIgnore(true);
                    j++;
                    ignoredPunches = true;
                    }
                if (j == 0)
                    {
                    CEvent::Event()->LogResultProblem(this, "Finish time looks too early, marked invalid");
                    m_Invalid = true;
                    }
                }
            }
        }

    if (timeAdjusted && !m_Invalid)
        CEvent::Event()->LogResultProblem(this, "Adjusted punch time(s) - clock may have rolled over");

    if (ignoredPunches && !m_Invalid)
        CEvent::Event()->LogResultProblem(this, "Ignored some punches - maybe runner didn't clear or or punched after finish");
    }


void CResult::PunchedControls(std::list<CPunch>& a_Controls, bool a_IncludeStartFinish)
    {
    if (a_IncludeStartFinish && m_Start.GetWhen().isValid())
        a_Controls.push_back(m_Start);
    for (std::list<CPunch*>::iterator i = m_Punches.begin(); i != m_Punches.end(); i++)
        if (!(*i)->GetIgnore())
            a_Controls.push_back(*(*i));
    if (a_IncludeStartFinish && m_Finish.GetWhen().isValid())
        a_Controls.push_back(m_Finish);
    }

QString CResult::DebugStr()
    {
    QString s = QString("SI: %1, Clear CN: %2, Clear Time: %3");
    return s.arg(m_SINumber).arg(m_Clear.GetCN()).arg(m_Clear.GetWhen().toString());
    }

long CResult::TimeTaken()
    {
    long result;
    if (m_Invalid || m_Disqualified || !m_Finish.GetWhen().isValid() || !m_Start.GetWhen().isValid())
        return -1;

    result = m_Start.GetWhen().secsTo(m_Finish.GetWhen());

    // Check for any untimed legs
    for (int i = 0; m_Course && i < m_Course->GetLegCount(); i++)
        if (m_Course->GetUntimedLeg(i) && m_LegStats.find(i) != m_LegStats.end())
            {
            result -= m_LegStats[i]->m_LegTime;
            }
    return result;
    }

QString CResult::TextResultStr()
    {
    QString s;
    if (!m_Invalid && GetFinished() && m_Pos > 0)
        {
        s = QString("%1 %2 %3 %4").arg(m_Pos, 3).arg(GetName(), -26).arg(m_Club.left(3), 3)
            .arg(FormatTimeTaken(TimeTaken()));
        }
    else
        {
        s = QString("    %1 %2 %3")
            .arg(GetName(), -26)
            .arg(m_Club.left(3), 3)
        .arg(m_Disqualified ? "DSQ" : "DNF");
        }
    return s;
    }

QString CResult::TextElapsedStr()
    {
    if (m_Invalid)
        return "";

    QString result;
    QString pos = QString("%1");
    if (GetFinished())
        pos = pos.arg(m_Pos);
    else
        pos = " ";
    //result.Printf("%3s %-26.26s %7s  %8s ", pos.c_str(),
    //              GetName().c_str(), FormatTimeTaken(TimeTaken()).c_str(), m_Start.GetWhen().FormatTime().c_str());
    result = QString("%1 %2 %3  %4 ").arg(pos,3).arg(GetName().left(26),-26).arg(FormatTimeTaken(TimeTaken()),7)
        .arg(m_Start.GetWhen().time().toString(),8);
    QString s;
    for (int i = 0; i < m_Course->GetLegCount(); i++)
        {
        if (!GetLegStat(i))
            s = "  --:--   ";
        else
            {
            QString pre, post;
            if (CEvent::Event()->GetShowHTML() &&
                GetLegStat(i)->m_ElapsedPos == 1 &&
                CEvent::Event()->SavingResults())
                {
                CHtmlOptions options;
                QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
                settings.beginGroup("HTML Options");
                pre = "<span " + CssAsHtmlAttr(options.getFastestCss()) + ">";
                post = "</span>";
                }
            s = QString("%1%2%3%4").arg(pre).arg(FormatTimeTaken(GetLegStat(i)->m_ElapsedTime),7)
                .arg(GetLegStat(i)->m_ElapsedPos,3).arg(post);
            }
        result += s;
        }
    if (m_Finished && !m_Disqualified)
        {
        double speed, len;
        len = m_Course->GetLength().toDouble();
        speed = (double (TimeTaken()))/ (len * 60);
        s = QString("   %1   %2").arg(FormatTimeTaken(TimeTaken())).arg(speed, 6, 'f', 2);
        result += s;
        }
    return result;
    }

QString CResult::TextLegStr()
    {
    QString result;
    if (m_Invalid)
        return result;

    result.fill(' ',49);
    QString s;
    for (int i = 0; i < m_Course->GetLegCount(); i++)
        {

        if (!GetLegStat(i) || GetLegStat(i)->m_LegTime <= 0)
            {
            s.fill(' ',10);
           // s = "  --:--   ";
            }
        else
            {
            QString pre, post;
            if (CEvent::Event()->GetShowHTML() &&
                CEvent::Event()->SavingResults() &&
                GetLegStat(i)->m_LegPos == 1)
                {
                CHtmlOptions options;
                QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
                settings.beginGroup("HTML Options");
                pre = "<span " + CssAsHtmlAttr(options.getFastestCss()) + ">";
                post = "</span>";
                }
            s = QString("%1%2%3%4").arg(pre).arg(FormatTimeTaken(GetLegStat(i)->m_LegTime),7).arg(GetLegStat(i)->m_LegPos,3).arg(post);
            }
        result += s;
        }

    return result;
    }

QString CResult::TextLegBehindStr()
    {
    QString result;
    if (m_Invalid)
        return result;

    result.fill(' ',48);
    QString s;
    for (int i = 0; i < m_Course->GetLegCount(); i++)
        {
        if (!GetLegStat(i) || GetLegStat(i)->m_LegTime == 0)
            s.fill(' ',10);
        else
            {
            s = QString("%1  ").arg(FormatTimeTaken(GetLegStat(i)->m_LegBehind),8);
            }
        result += s;
        }
    return result;
    }


QString CResult::RawDataDisplayStr()
    {
    QString index = QString("(%1)");
    index = index.arg(m_RawIndex);

    QString punches;
    for (std::list<CPunch*>::iterator i = m_Punches.begin(); i != m_Punches.end(); i++)
        if (!(*i)->GetIgnore())
            {
            QString s = QString("%1 ").arg((*i)->GetCN(),3);
            punches += s;
            }

    QString totalTime;
    totalTime = FormatTimeTaken(m_Start.GetWhen().secsTo(m_Finish.GetWhen()));

    QString s;
    s = QString("%1 %2%3%4 %5%6 %7").arg(index,5)
    .arg(m_SINumber,-9)
    .arg(m_Name.left(26),-26)
    .arg(m_Course ? m_Course->GetName() : "[no course]",-11)
    .arg(m_Finished ? "+" : "-")
    .arg(totalTime)
    .arg(punches);
    return s;
    }

bool CResult::GetFinished()
    {
    if (m_FinishedOverrideSet)
        return m_FinishedOverride;
    else
        return m_Finished && !m_Disqualified;
    }

void CResult::SetFinishedOverride(bool a_Finished)
    {
    if (!GetInvalid())
        {
        m_FinishedOverride = a_Finished;
        m_FinishedOverrideSet = true;
        }
    }

void CResult::AddLegStat(CLegStat* a_LegStat)
    {
    m_LegStats[a_LegStat->m_Index] = a_LegStat;
    }

void CResult::ClearLegStats()
    {
    for (std::map<int, CLegStat*>::iterator i = m_LegStats.begin(); i != m_LegStats.end(); i++)
        delete i->second;
    m_LegStats.clear();
    }

CLegStat* CResult::GetLegStat(int a_Leg)
    {
    if (m_LegStats.find(a_Leg) == m_LegStats.end())
        return NULL;
    return m_LegStats[a_Leg];
    }

QString CResult::GetDisplayName()
    {
    if (m_Name.isEmpty())
        {
        QString s = QString("(unknown) SI:%1").arg(m_SINumber);
        return s;
        }
    else
        return m_Name;
    }

void CResult::AddXML(CXmlWriter& a_Writer)
    {
    a_Writer.StartElement("PersonResult");
    a_Writer.StartElement("Person");
    a_Writer.StartElement("PersonName");
    QString safename(m_Name);
    safename.replace("&","+");
    QStringList names = safename.split(' ');

    a_Writer.StartElement("Family");
    if (names.count() == 0)
        a_Writer.AddValue("name");
    else
        a_Writer.AddValue(names[names.count() -1]);
    a_Writer.EndElement();
    for (int i = 0; i < ((int)names.count()) -1; i++)
        {
        a_Writer.StartElement("Given");
        a_Writer.AddValue(names[i]);
        a_Writer.EndElement();
        }
    if (names.size() < 2)
        {
        a_Writer.StartElement("Given");
        if (names.size() == 0)
            a_Writer.AddValue("no");
        else
            a_Writer.AddValue("someone");
        a_Writer.EndElement();
        }
    a_Writer.EndElement(); // PersonName

    a_Writer.StartElement("PersonId");
    a_Writer.AddValue(PersonID());
    a_Writer.EndElement();
    a_Writer.EndElement(); //Person

    a_Writer.StartElement("Club");
    a_Writer.StartElement("ClubId");
    a_Writer.AddValue(ClubID());
    a_Writer.EndElement();
    a_Writer.StartElement("ShortName");
    a_Writer.AddValue(ClubID());
    a_Writer.EndElement();
    a_Writer.StartElement("CountryId", "value=\"other\"");
    a_Writer.EndElement();
    a_Writer.EndElement();

    a_Writer.StartElement("Result");
    a_Writer.StartElement("CCard");
    a_Writer.StartElement("CCardId");
    a_Writer.AddValue(m_SINumber);
    a_Writer.EndElement();
    a_Writer.StartElement("PunchingUnitType", "value=\"SI\"");
    a_Writer.EndElement();
    a_Writer.EndElement();
    if (TimeTaken() > 0)
        {
        a_Writer.StartElement("Time");
                a_Writer.AddValue(FormatTimeTaken(TimeTaken()));
        a_Writer.EndElement();
        }

    if (GetPos() > 0)
        {
        QString temp = QString("%1").arg(GetPos());
        a_Writer.StartElement("ResultPosition");
        a_Writer.AddValue(temp);
        a_Writer.EndElement();
        }


    if (GetFinished() && TimeTaken() > 0)
        a_Writer.StartElement("CompetitorStatus", "value=\"OK\"");
    else if (GetDisqualified())
        a_Writer.StartElement("CompetitorStatus", "value=\"Disqualified\"");
    else if (GetInvalid())
        a_Writer.StartElement("CompetitorStatus", "value=\"DidNotFinish\"");
    else
        a_Writer.StartElement("CompetitorStatus", "value=\"MisPunch\"");
    a_Writer.EndElement();

    for (int i = 0; i < m_Course->GetLegCount() -1; i++)
        {
        QString temp = QString("sequence=\"%1\"").arg(i+1);
        a_Writer.StartElement("SplitTime", temp);

        a_Writer.StartElement("ControlCode");
        temp = QString("%1").arg(m_Course->GetLeg(i).GetEndCN());
        a_Writer.AddValue(temp);
        a_Writer.EndElement();

        a_Writer.StartElement("Time");
        if (GetLegStat(i) && GetLegStat(i)->m_LegTime != 0)
            a_Writer.AddValue(FormatTimeTaken(GetLegStat(i)->m_ElapsedTime));
        else
            a_Writer.AddValue("-----");
        a_Writer.EndElement();

        a_Writer.EndElement(); //SplitTime
        }

    a_Writer.EndElement(); // Result

    a_Writer.EndElement(); // PersonResult

    }

QString CResult::PersonID()
    {
    QString result = QString("%1-%2").arg(GetSINumber()).arg(GetRawIndex());
    return result;
    }

QString CResult::ClubID()
    {
    return m_Club;
    }

void CResult::SetCourse(QString a_Course)
    {
    CEvent::Event()->SetResultCourse(this, a_Course);
    }

void CResult::SetName(QString a_Name)
    {
    if (m_Name != a_Name)
        {
        m_Name = a_Name;
        m_Altered = true;
        }
    }

void CResult::SetClub(QString a_Club)
    {
    if (m_Club != a_Club)
        {
        m_Club = a_Club;
        m_Altered = true;
        }
    }
