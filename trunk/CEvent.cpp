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

#include "CEvent.h" // class's header file
#include <QTime>
#include "Utils.h"
#include "CSidetails.h"
#include "CResult.h"
#include "CCourse.h"
#include "CUniquePunch.h"
#include "CLegstat.h"
#include <functional>
#include "CXmlWriter.h"
#include <QFileInfo>
#include <QFile>
#include <algorithm>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QDialogButtonBox>



// class constructor
CEvent::CEvent() : m_Changed(false), m_ShowSplits(true), m_SavingResults(false)
{
    // insert your code here
}

// class destructor
CEvent::~CEvent()
{
    for (std::vector<CCourse*>::iterator i = m_Courses.begin(); i != m_Courses.end(); i++)
        delete (*i);
    for (std::vector<CResult*>::iterator j = m_Results.begin(); j != m_Results.end(); j++)
        delete (*j);
    for (std::map<long, CSiDetails*>::iterator k = m_SiDetails.begin(); k != m_SiDetails.end(); k++)
        delete k->second;
}

CEvent* CEvent::Event()
{
    static CEvent    instance; // Guaranteed to be destroyed.
                               // Instantiated on first use.
    return &instance;
}

bool CEvent::SetDirectory(QString a_Dir)
{
    if (a_Dir != m_Dir && m_Changed)
        if (!CanClose())
            return false;

    m_Dir = a_Dir;
    m_GoodThreshold = 4; // if n people have done the same course, aways mark it legitimate
    m_Changed = false;
    m_CourseFile = FindCourseFile(a_Dir);
    m_RawDataFile = FindRawDataFile(a_Dir);
    m_SINamesFile = FindSINamesFile(a_Dir);
    QFileInfo dirInfo(a_Dir);
    m_SINamesGlobalFile = FindSINamesGlobalFile(dirInfo.absoluteFilePath());
    emit filesChanged(m_RawDataFile, m_CourseFile, m_SINamesFile, m_SINamesGlobalFile);
    LoadCourseData();
    LoadSIData();
    LoadRawData();
    DisplayRawData();

    RecalcResults();

    return true;
}

void CEvent::RecalcResults()
{
    std::vector<QString> lines;
    if (!m_SavingResults)
    {
        ;//Notify(NFY_CLEARDISPLAY);
    }
    CalcResults();
    if (m_ShowHTML)
        {
        lines.push_back("<html>");
        lines.push_back("<head>");
        lines.push_back( "<title>" + m_EventName + "</title>");
        lines.push_back( "</head>");
        lines.push_back( "<body>");
        lines.push_back( "<h1 align=\"center\">" + m_EventName + "</h1>");
        lines.push_back( "<pre>");
        }
    DisplayTextResults(lines);
    if (m_ShowSplits)
        DisplayTextSplits(lines);

    if (m_ShowHTML)
        {
        lines.push_back ("");
        lines.push_back ("</pre>");
        lines.push_back("</body>");
        lines.push_back("</html>");
        }

    WriteResults(lines);
}

void CEvent::LoadCourseData()
{
    // Clear out old data
    for (std::vector<CCourse*>::iterator i = m_Courses.begin(); i != m_Courses.end(); i++)
        delete (*i);
    m_Courses.clear();

    // Load new file
    QFile tfile(m_CourseFile);
    if (!tfile.open(QIODevice::ReadOnly))
        {
        QMessageBox msg;
        msg.setText("Unable to open course file " + m_CourseFile);
        msg.exec();
        return;
        }

    QString line;
    QTextStream input(&tfile);
    while (!input.atEnd())
        {
        line = input.readLine();
        m_Courses.push_back(new CCourse(line));
        }

    tfile.close();
}

void CEvent::LoadSIData()
{
    // Clear out old data
    for (std::map<long, CSiDetails*>::iterator i = m_SiDetails.begin(); i != m_SiDetails.end(); i++)
        delete i->second;
    m_SiDetails.clear();

    if (!m_SINamesGlobalFile.isEmpty())
        LoadSIFile(m_SINamesGlobalFile);
    if (!m_SINamesFile.isEmpty())
        LoadSIFile(m_SINamesFile);
}

void CEvent::LoadSIFile(QString& a_File)
{
    // Load new file
    QFile tfile(a_File);
    if (!tfile.open(QIODevice::ReadOnly))
        {
        QMessageBox msg;
        msg.setText("Unable to open SI file " + m_CourseFile);
        msg.exec();
        return;
        }

    QString str;
    QTextStream input(&tfile);
    while (!input.atEnd())
        {
        str = input.readLine();

        long number;
        QStringList split1 = str.split(" ");
        if (split1.count() > 1)
            {
            number = split1[0].toLong();
            str = str.mid(1 + split1[0].length());
            QStringList split2 = str.split(",");
            QString name = split2[0];
            QString club = split2.count() > 1 ? split2[1] : "";

            if (m_SiDetails.find(number) != m_SiDetails.end())
                {
                if (m_SiDetails[number]->GetName() != name)
                    {
                    QString msg("Duplicate entry in %1 for %2, using %3 instead of %4");
                    msg=msg.arg(SimplifyPath(a_File)).arg(number).arg(name).arg(m_SiDetails[number]->GetName());
                    LogMsg(msg);
                    m_SiDetails[number]->SetName(name);
                    m_SiDetails[number]->SetClub(club);
                    }
                else if (m_SiDetails[number]->GetClub() != club && !club.isEmpty() && !m_SiDetails[number]->GetClub().isEmpty())
                    {
                    QString msg("Duplicate entry in %1 for %2, using club%3 instead of %4");
                    msg=msg.arg(SimplifyPath(a_File)).arg(number).arg(club).arg(m_SiDetails[number]->GetClub());
                    LogMsg(msg);
                    m_SiDetails[number]->SetName(name);
                    m_SiDetails[number]->SetClub(club);
                    }
                }
            else
                {
                m_SiDetails[number] = new CSiDetails(number, name, club);
                }
            }
        }
    tfile.close();
}

void CEvent::LoadRawData()
{
    // Clear out old data
    for (std::vector<CResult*>::iterator i = m_Results.begin(); i != m_Results.end(); i++)
        delete (*i);
    m_Results.clear();

    // Load new file, marking duplicates
    QFile tfile(m_RawDataFile);
    if (!tfile.open(QIODevice::ReadOnly))
        {
        QMessageBox msg;
        msg.setText("Unable to open raw data file " + m_RawDataFile);
        msg.exec();
        return;
        }

    QString str;
    QTextStream input(&tfile);
    std::map<CUniquePunch, CResult*>uniqueList;
    std::map<long, int> usedSIs;
    while (!input.atEnd())
        {
        str = input.readLine();

        if (CResult::ValidData(str))
            {
            CResult* result = new CResult(str);
            if (result->GetName().isEmpty())
                LogResultProblem(result, "No name supplied to match SI number - please fix");
            SetCourse(result);
            if (uniqueList.find(CUniquePunch(result)) != uniqueList.end())
                {
                if (uniqueList[CUniquePunch(result)]->GetRawData().length() < result->GetRawData().length())
                    {
                    // want to replace entry in m_Results with new one
                    LogResultProblem(result, "Duplicate download found - replacing old data");
                    std::replace(m_Results.begin(),m_Results.end(), uniqueList[CUniquePunch(result)], result);
                    uniqueList[CUniquePunch(result)] = result;
                    }
                else // ignore new one, its a copy of something we have
                    {
                    //LogResultProblem(result, "Duplicate download found - ignored");
                    delete result;
                    }
                }
            else
                {
                uniqueList[CUniquePunch(result)] = result;
                m_Results.push_back(result);
                if (usedSIs.find(result->GetSINumber()) != usedSIs.end())
                    {
                    QString s = QString("SI number %1 has already been used by %2 %3");
                    s = s.arg(result->GetSINumber()).arg(usedSIs[result->GetSINumber()]);
                    s = s.arg(usedSIs[result->GetSINumber()] == 1 ? "runner" : "runners");
                    LogResultProblem(result, s);
                    usedSIs[result->GetSINumber()] += 1;
                    }
                else
                    usedSIs[result->GetSINumber()] = 1;
                }
            }
        }
}


CSiDetails* CEvent::GetSIData(long a_SINumber)
{
    std::map<long, CSiDetails*>::iterator i = m_SiDetails.find(a_SINumber);
    return i == m_SiDetails.end() ? NULL : i->second;
}


bool CEvent::CanClose()
{
QMessageBox msg;
msg.setText("OK to close current event " + m_Dir + " ?");
msg.setIcon(QMessageBox::Question);
msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
return msg.exec() == QMessageBox::Yes;
}

QString CEvent::FindCourseFile(QString a_Dir) const
{
    QStringList files;
    files.append("courses.txt");
    files.append("Courses.txt");
    files.append("course");
    files.append("Course");
    files.append("course.txt");
    files.append("Course.txt");

    return FindDataFile(files,a_Dir);
}

QString CEvent::FindRawDataFile(QString a_Dir) const
{
    QStringList files;
    files.append("RawData.csv");
    files.append("rawdata.csv");

    return FindDataFile(files,a_Dir);
}

QString CEvent::FindSINamesFile(QString a_Dir) const
{
    QStringList files;
    files.append("SINames");
    files.append("SINames.txt");
    files.append("SINamesEvent");
    files.append("SINamesEvent.txt");

    return FindDataFile(files,a_Dir);
}

QString CEvent::FindSINamesGlobalFile(QString a_Dir) const
{
    QStringList files;
    files.append("SINames");
    files.append("SINames.txt");
    files.append("SINamesGlobal");
    files.append("SINamesGlobal.txt");

    return FindDataFile(files,a_Dir);
}

QString CEvent::FindDataFile(QStringList& a_Candidates, QString a_Dir) const
{
    for (int i = 0; i < a_Candidates.count(); i++)
        {
        QDir dir(a_Dir);
        QFileInfo fname(dir, a_Candidates[i]);
        if (fname.exists() && fname.isReadable())
            return fname.filePath();
        }
    return "";
}

void CEvent::LogMsg(QString a_Msg)
{
    m_LastLogMsg = a_Msg;
    emit logMsg(a_Msg);
}


void CEvent::LogResultProblem(CResult* a_Result, QString a_Msg)
{
    QString s = QString("(%1) %2 %3 - %4")
    .arg(a_Result->GetRawIndex())
    .arg(a_Result->GetSINumber())
    .arg(a_Result->GetName())
    .arg(a_Msg);
    LogMsg(s);
}

void CEvent::DisplayMsg(QString a_Msg)
{
    m_LastDisplayMsg = a_Msg;
    emit displayMsg(a_Msg);
}

int CEvent::CompareCourseIndex(CCourse* a_Lhs, CCourse* a_Rhs)
{
    if (a_Lhs == a_Rhs)
        return 0;

    for (unsigned int i=0; i < m_Courses.size(); i++)
        if (m_Courses[i] == a_Lhs)
            return -1;
        else if (m_Courses[i] == a_Rhs)
            return 1;

    return 0;
}

bool FinishTimeLessThan(CResult* a_Lhs, CResult* a_Rhs)
{
    int courseCompare = CEvent::Event()->CompareCourseIndex(a_Lhs->GetCourse(), a_Rhs->GetCourse());
    if (courseCompare != 0)
        return courseCompare < 0;

    if (a_Lhs->GetInvalid() != a_Rhs->GetInvalid())
        return a_Rhs->GetInvalid();

    if (a_Lhs->GetInvalid())
        return false;

    if (a_Lhs->GetFinished() != a_Rhs->GetFinished())
        return a_Lhs->GetFinished();

    if (!a_Lhs->GetFinished())
        return false;

    return a_Lhs->TimeTaken() < a_Rhs->TimeTaken();
}


void CEvent::CalcResults()
{
    for (std::vector<CResult*>::iterator i = m_Results.begin(); i != m_Results.end(); i++)
        {
        SetCourse(*i);
        if ((*i)->GetCourse() != NULL)
            SetPunchTimes(*i);
        }
    // Now finish time has been established, can sort based on it
    std::vector<CResult*> results = m_Results;
    std::sort(results.begin(), results.end(), FinishTimeLessThan);
    SetPositions(results);
    SetLegStats(results);
}

void CEvent::SetPositions(std::vector<CResult*>& a_SortedResults)
{
    CResult* lastResult(NULL);
    long pos(0), runningCount(0);
    for (std::vector<CResult*>::iterator i = a_SortedResults.begin(); i != a_SortedResults.end(); i++)
        {
        // Handle course break
        if (!lastResult || (*i)->GetCourse() != lastResult->GetCourse())
            {
            lastResult = (*i);
            pos = 1;
            runningCount = 1;
            }
        else
        // Handle time break
            if (lastResult && FinishTimeLessThan(lastResult, *i))
                {
                ++runningCount;
                pos = runningCount;
                }
            else
                {
                ++runningCount;
                }

        lastResult = *i;
        if (!(*i)->GetInvalid() && (*i)->GetFinished())
            (*i)->SetPos(pos);
        else
            (*i)->SetPos(-1);
        }

}


void CEvent::SetLegStats(std::vector<CResult*>& a_SortedResults)
{
// Its possible to do this using Adaptable Binary Predicate for the two-argument search
// maybe read-up and do that one day. Meanwhile we'll use good old COBOL style sorted list
// processing

    std::vector<CResult*>::iterator startCourse = a_SortedResults.begin();
    CCourse* lastCourse(NULL);
    std::vector<CResult*>::iterator i;
    for (i = a_SortedResults.begin(); i != a_SortedResults.end(); i++)
        {
        // Handle course break
        if (lastCourse && (*i)->GetCourse() != lastCourse)
            {
            SetCourseLegStats(lastCourse, a_SortedResults, startCourse, i);
            startCourse = i;
            }
        lastCourse = (*i)->GetCourse();
        }
    if (lastCourse)
        {
        i = a_SortedResults.end();
        SetCourseLegStats(lastCourse, a_SortedResults, startCourse, i);
        }

}


bool legTimeLessThan(long a_Lhs, long a_Rhs)
{
    // the default unset value is 0, need to treat that as longest value.
    if (a_Lhs == a_Rhs)
        return false;

    if (a_Lhs == 0)
        return false;

    if (a_Rhs == 0)
        return true;

    return a_Lhs < a_Rhs;
}

void CEvent::SetCourseLegStats(CCourse* a_Course, std::vector<CResult*>& a_SortedResults, std::vector<CResult*>::iterator& a_Start, std::vector<CResult*>::iterator& a_End)
{
    // for each leg on course, get all leg times in a sorted array. Use as a look-up
    std::vector<long> legTimes;
    for (int i = 0; i < a_Course->GetLegCount(); i++)
        {
        for (std::vector<CResult*>::iterator j = a_Start; j != a_End; j++)
            if ((*j)->GetLegStat(i) != NULL && (*j)->GetLegStat(i)->m_LegTime > 0)
                legTimes.push_back((*j)->GetLegStat(i)->m_LegTime);
        if (legTimes.size() > 0)
            {
            std::sort(&legTimes[0], &legTimes[legTimes.size()],legTimeLessThan);
            for (std::vector<CResult*>::iterator j = a_Start; j != a_End; j++)
                if ((*j)->GetLegStat(i) != NULL)
                    {
                    CLegStat* stat = (*j)->GetLegStat(i);

                    stat->m_LegBehind = stat->m_LegTime - legTimes[0];
                    for (int k = 0; k < (int)legTimes.size(); k++)
                        if (stat->m_LegTime == legTimes[k])
                            stat->m_LegPos = k + 1;
                    }
            }
        legTimes.clear();
        }


    // for each leg on course, get all elapsed times in a sorted array. Use as a look-up
    for (int i = 0; i < a_Course->GetLegCount(); i++)
        {
        for (std::vector<CResult*>::iterator j = a_Start; j != a_End; j++)
            if ((*j)->GetLegStat(i) != NULL && (*j)->GetLegStat(i)->m_ElapsedTime > 0)
                legTimes.push_back((*j)->GetLegStat(i)->m_ElapsedTime);
        if (legTimes.size() > 0)
            {
            std::sort(&legTimes[0], &legTimes[legTimes.size()]);
            for (std::vector<CResult*>::iterator j = a_Start; j != a_End; j++)
                if ((*j)->GetLegStat(i) != NULL)
                    {
                    CLegStat* stat = (*j)->GetLegStat(i);
                    stat->m_ElapsedBehind = stat->m_ElapsedTime - legTimes[0];
                    for (int k = 0; k < (int)legTimes.size(); k++)
                        if (stat->m_ElapsedTime == legTimes[k])
                            stat->m_ElapsedPos = k + 1;
                    }
            }
        legTimes.clear();
        }
}

CResult* CEvent::GetResult(long a_RawIndex)
{
    // Brute force, but (so far) only called as one-off against specific index
    for (unsigned int i = 0; i < m_Results.size(); i++)
        if (m_Results[i]->GetRawIndex() == a_RawIndex)
            return m_Results[i];
    return NULL;
}

void CEvent::SetCourse(CResult* a_Result)
{
    if (m_Courses.size() == 0)
        {
        a_Result->SetCourse(NULL);
        return;
        }
    std::list<CPunch> visited;
    a_Result->PunchedControls(visited, false);
    bool finished(false);

// Loop through all courses and find best fit

    CCourse* bestCourse(NULL);
    int bestGoodPunches(0);
    for (std::vector<CCourse*>::iterator i = m_Courses.begin(); i != m_Courses.end(); i++)
        {
        int found(0);
        std::list<long> controls;
        (*i)->CompulsoryControls(controls);
        std::list<CPunch>::iterator progress = visited.begin();
        std::list<CPunch>::iterator res;

        for (std::list<long>::iterator control = controls.begin(); control != controls.end(); control++)
            {
            for (res = progress; res != visited.end(); res++)
                if ((*res).GetCN() == *control)
                    {
                    ++ found;
                    progress = ++res;
                    break;
                    }
            }
        if ( found == (int)controls.size())  // Finished !!
            {
            if (!finished || found > bestGoodPunches)
                {
                finished = true;
                bestGoodPunches = found;
                bestCourse = *i;
                }
            }
        else
            {
            if (found > bestGoodPunches && !finished)
                {
                bestGoodPunches = found;
                bestCourse = *i;
                }
            }
        }
    if (bestCourse != NULL)
        a_Result->SetCourse(bestCourse);

    if (m_Courses.size() == 1 && bestCourse == NULL)
        a_Result->SetCourse(m_Courses[0]);
    else
        if (bestCourse == NULL)
            {
            if (a_Result->GetCourse() == NULL)
                LogResultProblem(a_Result, "Couldn't guess course");
            else
                LogResultProblem(a_Result, "Couldn't guess course, left as " + a_Result->GetCourse()->GetName());
            }

}


void CEvent::SetPunchTimes(CResult* a_Result)
{
    if (a_Result->GetInvalid())
        {
        // ?
        }
    else
        {
        a_Result->ClearLegStats();
        std::list<CPunch> visited;
        a_Result->PunchedControls(visited, true);
        a_Result->SetFinished(true);
        std::list<CPunch>::iterator progress = visited.begin();
        std::list<CPunch>::iterator punchIter;

        // find all relevant punches, set elapsed times, leg times
        for (int i = 0; i < a_Result->GetCourse()->GetLegCount(); i++)
            {
            bool found(false);
            for (punchIter = progress; punchIter != visited.end(); punchIter++)
                if ((*punchIter).GetCN() == a_Result->GetCourse()->GetLeg(i).GetEndCN() ||
                    ((*punchIter) == a_Result->GetFinish() && a_Result->GetCourse()->GetLeg(i).GetEndCN() == -2))  // match
                    {
                    CLegStat* statistic = new CLegStat(a_Result, i, *punchIter); // been here
                    a_Result->AddLegStat(statistic);
                    statistic->m_ElapsedTime = a_Result->GetStart().GetWhen().secsTo((*punchIter).GetWhen());
                    if (i > 0 && a_Result->GetLegStat(i-1) != NULL)
                        statistic->m_LegTime = statistic->m_ElapsedTime - a_Result->GetLegStat(i-1)->m_ElapsedTime;
                    if (i == 0)
                        statistic->m_LegTime = statistic->m_ElapsedTime;
                    progress = ++punchIter;
                    found = true;
                    break;
                    }

            if (!found && !a_Result->GetCourse()->GetLeg(i).GetOptional())
                a_Result->SetFinished(false);
            }
        }

}


void CEvent::DisplayRawData()
{
    LogMsg("");
    for (std::vector<CResult*>::iterator i = m_Results.begin(); i != m_Results.end(); i++)
        LogMsg((*i)->RawDataDisplayStr());
}

void CEvent::DisplayTextResults(std::vector<QString>& a_Lines)
{
// Display header text
    a_Lines.push_back("");
    if (m_ShowHTML)
        a_Lines.push_back("<font size=\"4\"><b>Results</b></font>");
    else
        a_Lines.push_back("Results");
    a_Lines.push_back("");

    std::vector<CResult*> results = m_Results;
    std::sort(results.begin(), results.end(), FinishTimeLessThan);

// Process unknown course
    bool gotUnknown(false);
    CCourse* lastCourse(NULL);
    for (long i = 0; i < (long)results.size(); i++)
        {
        if (results[i]->GetCourse() == NULL)
            {
            if (!gotUnknown)
                {
                a_Lines.push_back("");
                a_Lines.push_back("Unknown course. Cannot guess course for the following runners:");
                a_Lines.push_back("");
                gotUnknown = true;
                }
            a_Lines.push_back(results[i]->GetDisplayName());
            }
        else
            {
            if (results[i]->GetCourse() != lastCourse)
                {
                lastCourse = results[i]->GetCourse();
                a_Lines.push_back("");
                if (m_ShowHTML)
                    a_Lines.push_back("<b>" + results[i]->GetCourse()->TextDescStr() + "</b>");
                else
                    a_Lines.push_back(results[i]->GetCourse()->TextDescStr());
                }
            a_Lines.push_back(results[i]->TextResultStr());
            }
        }
}

void CEvent::DisplayTextSplits(std::vector<QString>& a_Lines)
{
// Display header text
    a_Lines.push_back("");
    if (m_ShowHTML)
        a_Lines.push_back("<font size=\"4\"><b>Splits</b></font>");
    else
        a_Lines.push_back("Splits");
    a_Lines.push_back("");
    a_Lines.push_back("");

    std::vector<CResult*> results = m_Results;
    std::sort(results.begin(), results.end(), FinishTimeLessThan);

    CCourse* lastCourse(NULL);
    for (long i = 0; i < (long)results.size(); i++)
        {
        if (results[i]->GetCourse() != NULL)
            {
            if (results[i]->GetCourse() != lastCourse)
                {
                lastCourse = results[i]->GetCourse();
                a_Lines.push_back("");
                if (m_ShowHTML)
                    a_Lines.push_back("<b>" + results[i]->GetCourse()->TextDescStr() + "</b>");
                else
                    a_Lines.push_back(results[i]->GetCourse()->TextDescStr());
                a_Lines.push_back("");
                a_Lines.push_back(results[i]->GetCourse()->TextSplitHdrStr());
                }
            a_Lines.push_back(results[i]->TextElapsedStr());
            a_Lines.push_back(results[i]->TextLegStr());
            a_Lines.push_back(results[i]->TextLegBehindStr());
            }
        }

}

void CEvent::ExportXML(QString a_File)
{
    CXmlWriter xml("ResultList","IOFdata.dtd");
    xml.StartElement("ResultList","status=\"complete\"");

    xml.StartElement("EventId", "type=\"loc\" idManager=\"anyone\"");
    xml.AddValue(m_EventName);
    xml.EndElement();

    std::vector<CResult*> results = m_Results;
    std::sort(results.begin(), results.end(), FinishTimeLessThan);

    CCourse* lastCourse(NULL);
    for (long i = 0; i < (long)results.size(); i++)
        {
        if (results[i]->GetCourse() != NULL)
            {
            if (results[i]->GetCourse() != lastCourse)
                {
                if (lastCourse != NULL)
                    xml.EndElement();

                lastCourse = results[i]->GetCourse();
                xml.StartElement("ClassResult");
                xml.StartElement("ClassShortName");
                xml.AddValue(lastCourse->GetName());
                xml.EndElement();
                }
            results[i]->AddXML(xml);
            }
        }
    if (lastCourse != NULL)
        xml.EndElement();
    xml.EndElement();

    xml.WriteToFile(a_File);
}

void CEvent::SaveResults(QString a_File)
{
    QFile tfile(a_File);
    if (tfile.exists())
        {
        tfile.remove(a_File);
        }

    if (!tfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
        SIMessageBox("Unable to open file " + a_File + " to write results.");
        return;
        }

    m_SavingResults = true;
    m_SaveFile = &tfile;
    RecalcResults(); // only if results may have changed ?
    m_SavingResults = false;
    tfile.close();
    m_SaveFile = NULL;
}


void CEvent::GetCourseNames(QStringList& a_Names)
{
    a_Names.clear();
    for (unsigned int i = 0; i < m_Courses.size(); i++)
        a_Names.append(m_Courses[i]->GetName());
}

void CEvent::SetResultCourse(CResult* a_Result, QString a_CourseName)
{
    CCourse* course = CourseFromName(a_CourseName);

    if (course)
        {
        a_Result->SetCourse(course);
        SetPunchTimes(a_Result);
        return;
        }
}

bool CEvent::GetShowSplits()
{
    return m_ShowSplits;
}

bool CEvent::GetShowHTML()
{
    return m_ShowHTML;
}

void CEvent::SetShowSplits(bool a_Show)
{
    m_ShowSplits = a_Show;
}

void CEvent::SetShowHTML(bool a_Show)
{
    m_ShowHTML = a_Show;
}

QString CEvent::GetEventName()
{
    return m_EventName;
}

void CEvent::SetEventName(QString a_Name)
{
    m_EventName = a_Name;
}

void CEvent::WriteResults(std::vector<QString>& a_Lines)
{
    if (m_SavingResults)
        {
        QTextStream out(m_SaveFile);
        for (unsigned int i = 0; i < a_Lines.size(); i++)
            out << a_Lines[i] << endl;
        }
    else
        emit updatedResultsOutput(a_Lines);
}

bool CEvent::GetResultFinished(long a_Index)
{
    return ResultExists(a_Index) && GetResult(a_Index)->GetFinished();
}

bool CEvent::GetResultInvalid(long a_Index)
{
    return ResultExists(a_Index) && GetResult(a_Index)->GetInvalid();
}

bool CEvent::ResultExists(long a_Index)
{
    return (GetResult(a_Index) != NULL);
}

void CEvent::dnfResult(long a_Index)
{
    CResult* res = GetResult(a_Index);
    if (res)
        res->SetFinishedOverride(false);
}

void CEvent::reinstateResult(long a_Index) // Reinstate someone
{
    CResult* res = GetResult(a_Index);
    if (res)
        res->SetFinishedOverride(true);
}

CCourse* CEvent::CourseFromName(QString a_Name)
{
    for (unsigned int i = 0; i < m_Courses.size(); i++)
        if (m_Courses[i]->GetName() == a_Name)
            {
            return m_Courses[i];
            }
    return NULL;
}

void CEvent::newCourse(CCourse* a_Course)
{
    m_Courses.push_back(a_Course);
    emit newCourseExists(a_Course->GetName());
}

void CEvent::deleteCourse(CCourse* a_Course)
{
    for (unsigned int i = 0; i < m_Courses.size(); i++)
        if (m_Courses[i] == a_Course)
            {
            m_Courses.erase(m_Courses.begin() + i);
            emit deletedCourse(a_Course->GetName());
            delete a_Course;
            return;
            }
}

struct ltlonglist
{
  bool operator()(std::list<long> l1, std::list<long> l2) const
  {
    if (l1.size() != l2.size())
        return l1.size() < l2.size();

    std::list<long>::const_iterator x1,x2;
    for ( x1 = l1.begin(),x2 = l2.begin(); x1 != l1.end(); x1++, x2++)
        if (*x1 != *x2)
            return *x1 < *x2;

    return false;
  }
};


void CEvent::CollectPunchSequences(std::map<std::list<long>, int>& a_Sequences)
{
    a_Sequences.clear();

    for (unsigned int i = 0; i < m_Results.size(); i++)
        {
        std::list<CPunch> punches;
        m_Results[i]->PunchedControls(punches, false);
        std::list<long> plist;
        for (std::list<CPunch>::iterator y = punches.begin(); y != punches.end(); y++)
            {
            plist.push_back((*y).GetCN());
            }
        if (a_Sequences.find(plist) != a_Sequences.end())
            a_Sequences[plist] = a_Sequences[plist] + 1;
        else
            a_Sequences[plist] = 1;
        }
}

void CEvent::AddGuessedCourses(std::map<std::list<long>, int >& a_Sequences)
{
    std::list<QStringList> existingSequences;
    std::list<QString> existingNames;

    for (std::vector<CCourse*>::iterator x = m_Courses.begin(); x != m_Courses.end(); x++)
        {
        QStringList slExisting;
        (*x)->GetControls(slExisting);
        existingSequences.push_back(slExisting);
        existingNames.push_back((*x)->GetName());
        }
    int i(1);
    for (std::map<std::list<long>,int >::iterator x = a_Sequences.begin(); x != a_Sequences.end(); x++, i++)
        {
        QStringList sl;
        for (std::list<long>::const_iterator y = x->first.begin(); y != x->first.end(); y++)
            {
            QString t = QString("%1").arg(*y);
            sl.append(t);
            }

        if (std::find(existingSequences.begin(), existingSequences.end(), sl) == existingSequences.end())
            {
            QString stemName = QString(tr("Guess_Course_(%1 controls %2 to %2)")).arg(sl.size()).arg(sl.first()).arg(sl.last());
            QString name(stemName);
            while (std::find(existingNames.begin(), existingNames.end(), name) != existingNames.end())
                {
                name = QString(tr("%1%2")).arg(stemName).arg(++i);
                }
            QString len = tr("not set");
            QString climb = tr("not set");
            CCourse* course = new CCourse(name, len, climb, sl);
            m_Courses.push_back(course);
            emit newCourseExists(course->GetName());
            }
        }

}

bool CEvent::ContinueCourseLoad()
{
    if (m_Courses.size() > 0)
        {
        QMessageBox::StandardButtons btns = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
        int answer = SIMessageBox("Courses already exist - do you want to delete them and then guess the courses ?",
                     QMessageBox::Question,
                     btns);
        if (answer == QDialogButtonBox::Cancel)
            return false;

        if (answer == QDialogButtonBox::Yes)
            {
            std::vector<CCourse*> courses = m_Courses;
            for (unsigned int i = 0; i < m_Courses.size(); i++)
                deleteCourse(courses[i]);
            m_Courses.clear();
            }
        }
    return true;
}

int CEvent::ControlsDifferent(const std::list<long>& a_Good, const std::list<long>& a_Candidate)
    {
    int missing(0), extra(0);

    std::list<long>::iterator goodIter, candidateIter;
    // count missing
    for (std::list<long>::const_iterator goodIter = a_Good.begin(); goodIter != a_Good.end(); goodIter++)
        {
        if (std::find(a_Candidate.begin(),a_Candidate.end(), *goodIter) == a_Candidate.end())
            missing++;
        }
    for (std::list<long>::const_iterator x = a_Candidate.begin(); x != a_Candidate.end(); x++)
        {
        if (std::find(a_Good.begin(),a_Good.end(), *x) == a_Good.end())
            extra++;
        }

    return missing + extra;
    }

bool CEvent::courseIsSubset(const std::list<long>& a_Good, const std::list<long>& a_Candidate)
{
// Ignoring sequence, check if all controls on candidate are in good course
    for (std::list<long>::const_iterator x = a_Candidate.begin(); x != a_Candidate.end(); x++)
        {
        if (std::find(a_Good.begin(),a_Good.end(), *x) == a_Good.end())
            return false;
        }
    return true;
}

void CEvent::EliminateMispunchSequences(std::map<std::list<long>, int> &a_Sequences)
{

    std::vector<std::list<long> > eliminate;

    for (std::map<std::list<long>,int >::iterator x = a_Sequences.begin(); x != a_Sequences.end(); x++)
        {
        for (std::map<std::list<long>,int >::iterator y = a_Sequences.begin(); y != a_Sequences.end(); x++)
            {
            if ((*y).first != (*x).first && (*y).second > (*x).second && x->second < m_GoodThreshold)
                {
                // Eliminate courses which are a subset of a "good" course (ignoring order)
                if (courseIsSubset(y->first, x->first))
                    eliminate.push_back(x->first);
                else
                // Eliminate courses with only one or two punches different to a more popular courses
                    {
                    int diffs = ControlsDifferent(y->first,x->first);
                    if (diffs < 3 && (*y).first.size() > 7)
                        {
                        eliminate.push_back(x->first);
                        }
                    }
                }
            }
        }
    for (unsigned int i = 0; i < eliminate.size(); i++)
        a_Sequences.erase(eliminate[i]);
 }

void CEvent::guessCourses()
{
    // Tidy up existing courses if required
    if (!ContinueCourseLoad())
        return;

    std::map<std::list<long>,int > sequences;
    CollectPunchSequences(sequences);
    //EliminateMispunchSequences(sequences);
    AddGuessedCourses(sequences);

    emit coursesGuessed();
}
