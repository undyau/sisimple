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
#include <set>
#include <QXmlSimpleReader>
#include "ciofcoursexmlhandler.h"
#include "ciofresultxmlhandler.h"
#include <QFileDialog>
#include <QSettings>
#include "chtmloptions.h"



// class constructor
CEvent::CEvent() : m_ChangedSinceSave(false), m_ShowSplits(true), m_SavingResults(false),
  m_GoodThreshold(3)
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("General");
    m_Dir = settings.value("lastDir","").toString();
    m_RentalNames = settings.value("rentalNames","").toString();

    int size = settings.beginReadArray("SiArchive");
    for (int i = 0; i < size; ++i)
        {
        settings.setArrayIndex(i);
        QString name,club;
        long number;
        number = settings.value("number").toLongLong();
        name = settings.value("name").toString();
        club = settings.value("club").toString();
        m_SiDetails[number] = new CSiDetails(number, name, club);
        }
    settings.endArray();
    settings.endGroup();
}

void CEvent::LoadLastEvent()
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    QDir dir(QFileInfo(settings.fileName()).absolutePath() + "/");
    QString file = dir.filePath("SISimpleLastEvent.xml");
    QFileInfo fi(file);
    if (fi.exists())
        LoadEventFromXML(file);
}

// class destructor
CEvent::~CEvent()
{
    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("General");
    settings.setValue("lastDir", m_Dir);
    settings.setValue("rentalNames", m_RentalNames);
    settings.endGroup();

    // Save event details (if any) to last event file
    if (m_Results.size() > 0)
        {
        QDir dir(QFileInfo(settings.fileName()).absolutePath() + "/");
        QString file = dir.filePath("SISimpleLastEvent.xml");
        ExportXML(file, true);
        }

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

void CEvent::Reset()
{
    for (std::vector<CCourse*>::iterator i = m_Courses.begin(); i != m_Courses.end(); i++)
        delete (*i);
    for (std::vector<CResult*>::iterator j = m_Results.begin(); j != m_Results.end(); j++)
        delete (*j);

    SetEventName(QString(""));
    m_Courses.clear();
    m_Results.clear();
    emit (resetLog());
    std::vector<QString> lines;
    emit (updatedResultsOutput(lines));
}

QString CEvent::Directory()
{
    return m_Dir;
};

QString CEvent::GetRentalNames()
{
    return m_RentalNames;
}

void CEvent::SetRentalNames(QString& a_Names)
{
    m_RentalNames = a_Names.trimmed();
}

void CEvent::NewDownloadData(QStringList a_NewData)
{
    m_OriginalResultsData.empty();
    m_OriginalResultsData.append(a_NewData);

    ProcessRawData();
}

bool CEvent::SetResultsInputFile(QString a_File)
{
    if (m_ResultsInputFile != m_ResultsInputFile)
        if (!CanClose())
            return false;

    m_ResultsInputFile = a_File;
    QFileInfo fileInfo(a_File);
    m_Dir = fileInfo.dir().canonicalPath ();

    m_OriginalResultsData.empty();

    // Sample the file
    QFile tfile(m_ResultsInputFile);
    if (!tfile.open(QIODevice::ReadOnly))
        return LoadEventFromDump();
    QTextStream stream( &tfile );
    QString line = stream.readLine();
    tfile.close();
    if (line.contains("?xml"))
        return LoadEventFromXML(a_File);
    else
        return LoadEventFromDump();
}

bool CEvent::LoadEventFromDump()
{
    QFile tfile(m_ResultsInputFile);
    if (!tfile.open(QIODevice::ReadOnly))
        {
        QMessageBox msg;
        msg.setText("Unable to open raw data file " + m_ResultsInputFile);
        msg.exec();
        return false;
        }

    QTextStream stream( &tfile );
    while ( !stream.atEnd() )
        m_OriginalResultsData += stream.readLine();
    tfile.close();

    ProcessRawData();

    return true;
}

void CEvent::ProcessRawData()
{
    bool guess(false);
    LoadCourseData(guess);
    LoadRawData();
    if (guess)
        {
        guessCourses();    // Should be a decision made by user here
        LoadRawData();     // Reload now courses are known
        }
    DisplayRawData();

    RecalcResults();
}

void CEvent::RecalcResults()
{
    m_ChangedSinceSave = true;

    std::vector<QString> lines;
    CalcResults();

    CHtmlOptions options;
    bool m_ResultsAsHTMLPage = options.getWholePage();

    if (m_ShowHTML)
        {
        if (m_ResultsAsHTMLPage && m_SavingResults )
            {
            lines.push_back("<html>");
            lines.push_back("<head>");
            lines.push_back( "<title>" + GetEventName() + "</title>");
            lines.push_back( "</head>");
            lines.push_back( "<body>");
            }
        lines.push_back( "<h1 align=\"center\">" + GetEventName() + "</h1>");
        lines.push_back( "<pre>");
        }
    DisplayTextResults(lines);
    if (m_ShowSplits)
        DisplayTextSplits(lines);

    if (m_ShowHTML)
        {
        lines.push_back ("");
        lines.push_back ("</pre>");
        if (m_ResultsAsHTMLPage && m_SavingResults)
            {
            lines.push_back("</body>");
            lines.push_back("</html>");
            }
        }

    WriteResults(lines);
}

void CEvent::LoadCourseData(bool& a_Guess)
{
    if (m_Courses.size() > 0)
        return;

    // Guess, import or set manually ?
    QMessageBox msgBox;
    QPushButton *importButton = msgBox.addButton(tr("Import"), QMessageBox::AcceptRole);
    QPushButton *guessButton = msgBox.addButton(tr("Guess"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Later"), QMessageBox::AcceptRole);

    msgBox.setText(tr("<p>If you have a file containing the course data in XML format from Purple Pen or OCAD, you can load that data now.</p><p>Alternatively SI Simple can guess the courses based on the controls visited or you can define the courses later.</p><p>Whichever approach you choose now, you can always alter the courses later.</p>"));
    msgBox.exec();

    a_Guess = false;

     if (msgBox.clickedButton() == (QAbstractButton*)importButton)
        {
        QString file = QFileDialog::getOpenFileName(NULL,tr("Select course file"), CEvent::Event()->Directory(), QString("*.xml"));

        if (!file.isEmpty())
            importCourses(file);
        }
     else if (msgBox.clickedButton() == (QAbstractButton*)guessButton)
         a_Guess = true;

}

void CEvent::AddRecoveredResult(CResult *a_Result)
{
    m_Results.push_back(a_Result);
}

void CEvent::LoadRawData()
{
    // Clear out old data
    for (std::vector<CResult*>::iterator i = m_Results.begin(); i != m_Results.end(); i++)
        delete (*i);
    m_Results.clear();
    emit(resetLog());

    // Load new data, marking duplicates

    QString str;
    std::map<CUniquePunch, CResult*>uniqueList;
    std::map<long, int> usedSIs;
    for (int i = 0; i < m_OriginalResultsData.size(); i++)
        {
        str = m_OriginalResultsData.at(i);

        if (CResult::ValidData(str))
            {
            CResult* result = new CResult(str);
            SetCourse(result);
            if (uniqueList.find(CUniquePunch(result)) != uniqueList.end())
                {
                if (uniqueList[CUniquePunch(result)]->GetRawData().length() < result->GetRawData().length())
                    {
                    // want to replace entry in m_Results with new one
                    //LogResultProblem(result, tr("Duplicate download found - replacing old data"));
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
                if (result->GetName().isEmpty())
                    LogResultProblem(result, tr("No name supplied to match SI number - please fix"));
                else if (IsRental(result->GetName()))
                    LogResultProblem(result, tr("Looks like rental stick - please fix"));

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

void CEvent::SaveChangedSIDetails()
{
// open file
    QString filter;
    filter = "Text files (*.csv)";
    QString file = QFileDialog::getSaveFileName(NULL, "Save new SI details to", Directory(), filter);
    if (!file.isEmpty())
        {
        QFile tfile(file);
        if (tfile.exists())
            {
            tfile.remove(file);
            }

        if (!tfile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
            SIMessageBox(tr("Unable to open file ") + file + tr(" to save changed SI card details."));
            return;
            }
        QTextStream out(&tfile);
        for (std::vector<CResult*>::iterator j = m_Results.begin(); j != m_Results.end(); j++)
            {
            if ((*j)->Altered())
                {
                QString line = QString("%1,\"%2\",\"%3\"")
                           .arg((*j)->GetSINumber())
                           .arg((*j)->GetName())
                           .arg((*j)->GetClub());
                out << line << endl;
                }
            }

        tfile.close();
        }
}

bool CEvent::CanClose()
{
    int count(0);
    for (std::vector<CResult*>::iterator j = m_Results.begin(); j != m_Results.end(); j++)
        {
        if ((*j)->Altered())
            ++count;
        }

    if (count > 0)
        {
        QString msg = QString(tr("You have altered %1 SI %2 - do you want to save the list of cards ?"))
                    .arg(count).arg(count > 1 ? tr("cards") : tr("card") );
        int result = SIMessageBox(msg, QMessageBox::Question, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (result == QMessageBox::Yes)
            {
            SaveChangedSIDetails();
            }
        return result != QMessageBox::Cancel;
        }
    return true;
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

    if (a_Lhs->GetDisqualified() != a_Rhs->GetDisqualified())
        return a_Rhs->GetDisqualified();

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
            SetCourseLegStats(lastCourse, startCourse, i);
            startCourse = i;
            }
        lastCourse = (*i)->GetCourse();
        }
    if (lastCourse)
        {
        i = a_SortedResults.end();
        SetCourseLegStats(lastCourse, startCourse, i);
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

void CEvent::SetCourseLegStats(CCourse* a_Course, std::vector<CResult*>::iterator& a_Start, std::vector<CResult*>::iterator& a_End)
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
                            {
                            stat->m_LegPos = k + 1;
                            break;
                            }
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
                            {
                            stat->m_ElapsedPos = k + 1;
                            break;
                            }
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
                    CLegStat* statistic = new CLegStat(i, *punchIter); // been here
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
    CHtmlOptions options;

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
                QString pre,post;
                if (m_ShowHTML && m_SavingResults)
                    {
                    pre = "<span " + CssAsHtmlAttr(options.getHeaderCss()) + ">";
                    post = "</span>";
                    }
                a_Lines.push_back(pre + results[i]->GetCourse()->TextSplitHdrStr() + post);
                }
            QString spanStartElapsed, spanStartLeg, spanStartBehind, spanEnd;
            if (m_ShowHTML && m_SavingResults)
                {
                spanStartElapsed =  "<span ";
                if (options.getShowNameTooltip())
                    spanStartElapsed += "title=\"" + results[i]->GetDisplayName() + "\" ";
                spanStartLeg = spanStartBehind = spanStartElapsed;
                spanStartElapsed += CssAsHtmlAttr(options.getElapsedCss()) + ">";
                spanStartLeg += CssAsHtmlAttr(options.getLegCss()) + ">";
                spanStartBehind += CssAsHtmlAttr(options.getBehindCss()) + ">";
                }
            a_Lines.push_back(spanStartElapsed + results[i]->TextElapsedStr() + spanEnd);
            a_Lines.push_back(spanStartLeg + results[i]->TextLegStr() + spanEnd);
            a_Lines.push_back(spanStartBehind + results[i]->TextLegBehindStr() + spanEnd);
            }
        }

}

void CEvent::ExportXML(QString a_File, bool a_ExtendedFormat)
{
    CXmlWriter xml("ResultList","IOFdata.dtd");
    xml.StartElement("ResultList","status=\"complete\"");

    xml.StartElement("EventId", "type=\"loc\" idManager=\"anyone\"");
    xml.AddValue(GetEventName());
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
                if (a_ExtendedFormat)
                    {
                    xml.StartElement("CourseLength");
                    xml.AddValue(lastCourse->GetLength());
                    xml.EndElement();
                    xml.StartElement("CourseClimb");
                    xml.AddValue(lastCourse->GetLength());
                    xml.EndElement();
                    }
                }
            results[i]->AddXML(xml);
            }
        }
    if (lastCourse != NULL)
        xml.EndElement();
    xml.EndElement();

    xml.WriteToFile(a_File);
    m_ChangedSinceSave = false;
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
    if (m_EventName != a_Name)
        {
        m_EventName = a_Name;
        emit eventNameSet(a_Name);
        }
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

void CEvent::dsqResult(long a_Index)
{
    CResult* res = GetResult(a_Index);
    if (res)
        res->SetDisqualified(true);
}

void CEvent::deleteDownload(long a_Index)
{
    CResult* res = GetResult(a_Index);
    for (unsigned long i = 0; i < m_Results.size(); i++)
        if (m_Results.at(i) == res)
            {
            m_Results.erase(m_Results.begin() + i);
            delete res;
            break;
            }
}

void CEvent::reinstateResult(long a_Index) // Reinstate someone
{
    CResult* res = GetResult(a_Index);
    if (res)
        {
        res->SetFinishedOverride(true);
        res->SetDisqualified(false);
        }
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
    for (unsigned int i = 0; i < m_Results.size(); i++)
        if (m_Results[i]->GetCourse() == a_Course)
            m_Results[i]->SetCourse(NULL);

    for (unsigned int i = 0; i < m_Courses.size(); i++)
        if (m_Courses[i] == a_Course)
            {
            m_Courses.erase(m_Courses.begin() + i);
            emit deletedCourse(a_Course->GetName());
            delete a_Course;
            return;
            }
}

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
    for (std::map<std::list<long>,int >::iterator x = a_Sequences.begin(); x != a_Sequences.end(); x++)
        {
        QStringList sl;
        for (std::list<long>::const_iterator y = x->first.begin(); y != x->first.end(); y++)
            {
            QString t = QString("%1").arg(*y);
            sl.append(t);
            }

        if (std::find(existingSequences.begin(), existingSequences.end(), sl) == existingSequences.end())
            {
            QString name;
            do
                name = QString(tr("Course_%1 (%3 people, %2 ctls, first CN: %4)")).arg(i++)
                       .arg(sl.size()).arg(x->second).arg(sl.size() > 0 ? sl.first() : "n.a.");
            while (std::find(existingNames.begin(), existingNames.end(), name) != existingNames.end());

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

int CEvent::ControlsMissing(const std::list<long>& a_Good, const std::list<long>& a_Candidate)
    {
    int missing(0);
    std::list<long>::iterator goodIter, candidateIter;

    for (std::list<long>::const_iterator goodIter = a_Good.begin(); goodIter != a_Good.end(); goodIter++)
        {
        if (std::find(a_Candidate.begin(),a_Candidate.end(), *goodIter) == a_Candidate.end())
            missing++;
        }
    return missing;
    }

void CEvent::EliminateMispunchSequences(std::map<std::list<long>, int>& a_Sequences)
{
    std::set<std::list<long> > eliminate;
    for (std::map<std::list<long>,int >::iterator x = a_Sequences.begin(); x != a_Sequences.end(); x++)
        {
        for (std::map<std::list<long>,int >::iterator y = a_Sequences.begin(); y != a_Sequences.end(); y++)
            {
            if (y->first != x->first && y->second > x->second && x->second < m_GoodThreshold)
                {
                // Eliminate courses which are only 1 punch out from being a "good" course (ignoring order)
                int missing, extra;
                if ((missing = ControlsMissing(y->first, x->first)) < 3 && missing > 0)
                    {
                    qDebug() << "Eliminating a course (1)";
                    eliminate.insert(x->first);
                    break;
                    }
                // find extra controls
                if ((extra = ControlsMissing(x->first, y->first)) < 3 && extra > 0)
                    {
                    qDebug() << "Eliminating a course (2)";
                    eliminate.insert(x->first);
                    break;
                    }
                }
            }
        }

    for (std::set<std::list<long> >::iterator i = eliminate.begin(); i != eliminate.end(); i++)
        a_Sequences.erase(*i);
 }

void CEvent::guessCourses()
{
    // Tidy up existing courses if required
    if (!ContinueCourseLoad())
        return;

    std::map<std::list<long>,int > sequences;
    CollectPunchSequences(sequences);
    EliminateMispunchSequences(sequences);
    AddGuessedCourses(sequences);

    emit coursesGuessed();
}

void CEvent::LoadCoursesFromXML(QString a_FileName)
{
  QFile file(a_FileName);
  QXmlInputSource source( &file );
  CIofCourseXmlHandler handler;

  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  reader.parse( source );
}

void CEvent::importCourses(QString a_FileName)
{
    // Tidy up existing courses if required
    if (!ContinueCourseLoad())
        return;

    LoadCoursesFromXML(a_FileName);

    emit coursesGuessed();
}

void CEvent::addNewCourse(CCourse* a_Course)
{
    QString name = a_Course->GetName();
    int i(2);
    bool unique(false);

    while (!unique)
        {
        unique = true;
        for (std::vector<CCourse*>::iterator x = m_Courses.begin(); x != m_Courses.end(); x++)
            if ((*x)->GetName() == name)
                unique = false;
        if (!unique)
            name = QString("%1(%2)").arg(a_Course->GetName()).arg(i++);
        }

    a_Course->SetName(name);
    this->newCourse(a_Course);
}

void CEvent::newSIData(QString input)
{
// split on line endings
    QStringList records = input.split(QRegExp("\n|\r|\r\n"));
    QRegExp sidRegex("[0-9]{2,10} [A-Za-z][^,]*");
    QRegExp sisimpleRegex("[0-9]{2,10} [A-Za-z][^,]*,[A-Za-z0-9]*");
    int sidCount(0), sisimpleCount(0);

    for (int i = 0; i < records.count(); i++)
        {
        if (sidRegex.exactMatch(records.at(i)))
            ++sidCount;
        if (sisimpleRegex.exactMatch(records.at(i)))
            ++sisimpleCount;
        }

    if (sidCount > sisimpleCount)
        ProcessSIDData(records, false);
    else if (sisimpleCount > sidCount)
        ProcessSISimpleData(records, false);
    else
        SIMessageBox("Unknown data format, no records loaded");

    QSettings settings(QSettings::IniFormat,  QSettings::UserScope, "undy","SI Simple");
    settings.beginGroup("General");
    settings.beginWriteArray("SiArchive");
    int index(0);
    for (std::map<long, CSiDetails*>::iterator j = m_SiDetails.begin();
         j != m_SiDetails.end(); ++j)
        {
        settings.setArrayIndex(index++);
        settings.setValue("number", (long long)j->first);
        settings.setValue("name", (j->second)->GetName());
        settings.setValue("club", (j->second)->GetClub());
        }
    settings.endArray();
    settings.endGroup();

    QString msg = QString(tr("%1 cards in SI archive")).arg(records.count());
    emit(loadedSIArchive(msg));
}

void CEvent::ProcessSIDData(QStringList& a_Records, bool a_Append)
{
    ProcessSISimpleData(a_Records, a_Append);  // schema is a subset
}

void CEvent::ProcessSISimpleData(QStringList& a_Records, bool a_Append)
{
    if (!a_Append)
        {
        for (std::map<long, CSiDetails*>::iterator i = m_SiDetails.begin(); i != m_SiDetails.end(); i++)
            delete i->second;
        m_SiDetails.clear();
        }

    for (int i = 0; i < a_Records.count(); i++)
        {
        QString str = a_Records.at(i);

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
                    QString msg("Duplicate entry for %1, using %2 instead of %3");
                    msg = msg.arg(number).arg(name).arg(m_SiDetails[number]->GetName());
                    LogMsg(msg);
                    m_SiDetails[number]->SetName(name);
                    m_SiDetails[number]->SetClub(club);
                    }
                else if (m_SiDetails[number]->GetClub() != club && !club.isEmpty() && !m_SiDetails[number]->GetClub().isEmpty())
                    {
                    QString msg("Duplicate entry in %1 for %2, using club%3 instead of %4");
                    msg=msg.arg(number).arg(club).arg(m_SiDetails[number]->GetClub());
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

}

bool CEvent::IsRental(QString a_Name)
{
    QStringList parts = m_RentalNames.split(",");
    for (int i = 0; i < parts.count(); i++)
        if (a_Name.contains(parts.at(i)) && !parts.at(i).isEmpty())
            return true;
    return false;
}

long CEvent::LookupResult(QString a_Name, QString a_Result)
{
    long index(-1);
    int count(0);
    for (unsigned int i = 0; i < m_Results.size(); i++)
        {
        if( m_Results[i]->GetName() == a_Name || a_Name.isEmpty())
            {
            if (a_Result == FormatTimeTaken(m_Results[i]->TimeTaken()) ||
                (a_Result == "DNF" && !m_Results[i]->GetFinished()) ||
                (a_Result == "DSQ" && !m_Results[i]->GetDisqualified()))
                {
                index = m_Results[i]->GetRawIndex();
                ++count;
                }
            }
        }
    if (count == 1)
        return index;
    return -1;
}

bool CEvent::LoadEventFromXML(QString a_FileName)
{
  QFile file(a_FileName);
  QXmlInputSource source( &file );
  CIofResultXmlHandler handler;

  QXmlSimpleReader reader;
  reader.setContentHandler( &handler );
  if (!reader.parse( source ))
    return false;

  RecalcResults();
  return true;
}

void CEvent::elevateCourse(CCourse* a_Course)
{
    for (unsigned int i = 1; i < m_Courses.size(); i++)
        if (m_Courses[i] == a_Course)
            std::swap(m_Courses[i], m_Courses[i-1]);
}

void CEvent::demoteCourse(CCourse* a_Course)
{
    for (unsigned int i = 10; i < m_Courses.size() - 1; i++)
        if (m_Courses[i] == a_Course)
            std::swap(m_Courses[i], m_Courses[i+1]);
}
