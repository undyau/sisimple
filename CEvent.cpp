// Class automatically generated by Dev-C++ New Class wizard

#include "CEvent.h" // class's header file
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/datetime.h>
#include "utils.h"
#include "Notifications.h"
#include "CSiDetails.h"
#include "CResult.h"
#include "CCourse.h"
#include "CUniquePunch.h"
#include "CLegStat.h"
#include <functional>
#include "CXmlWriter.h"




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

bool CEvent::SetDirectory(wxString a_Dir)
{
    if (a_Dir != m_Dir && m_Changed)
    	if (!CanClose())
    		return false;
    	
    m_Dir = a_Dir;
    m_Changed = false;
    m_CourseFile = FindCourseFile(a_Dir);
    m_RawDataFile = FindRawDataFile(a_Dir);
    m_SINamesFile = FindSINamesFile(a_Dir);
    wxFileName dir(a_Dir, "");
    dir.RemoveLastDir();
    m_SINamesGlobalFile = FindSINamesGlobalFile(dir.GetFullPath());
    Notify(NFY_ALLFILES);
    
    LoadCourseData();
    LoadSIData();	
    LoadRawData();
    DisplayRawData();  

    RecalcResults();      
    
    return true;	
}

void CEvent::RecalcResults()
{
    if (!m_SavingResults)
        Notify(NFY_CLEARDISPLAY);
    CalcResults();      
    if (m_ShowHTML)
        {
        WriteResultLine ("<html>");
        WriteResultLine("<head>");
        WriteResultLine( "<title>" + m_EventName + "</title>");
        WriteResultLine( "</head>");
        WriteResultLine( "<body>");
        WriteResultLine( "<h1 align=\"center\">" + m_EventName + "</h1>");
        WriteResultLine( "<pre>");        
        }
    DisplayTextResults();
    if (m_ShowSplits)
        DisplayTextSplits(); 
    
    if (m_ShowHTML)
        { 
        WriteResultLine ("");                 
        WriteResultLine ("</pre>");        
        WriteResultLine("</body>");
        WriteResultLine("</html>");
        }          
}

void CEvent::LoadCourseData()
{
    // Clear out old data
    for (std::vector<CCourse*>::iterator i = m_Courses.begin(); i != m_Courses.end(); i++)
    	delete (*i);
    m_Courses.clear();
    
    // Load new file
    wxTextFile      tfile;
    tfile.Open(m_CourseFile);
    wxString str;
    
    for ( str = tfile.GetFirstLine(); !tfile.Eof(); str = tfile.GetNextLine() )
        {
    	if (CCourse::ValidData(str))
    		m_Courses.push_back(new CCourse(str));
    	}
}

void CEvent::LoadSIData()
{
    // Clear out old data
    for (std::map<long, CSiDetails*>::iterator i = m_SiDetails.begin(); i != m_SiDetails.end(); i++)
    	delete i->second;
    m_SiDetails.clear();
    
    if (!m_SINamesGlobalFile.IsEmpty())
    	LoadSIFile(m_SINamesGlobalFile);
    if (!m_SINamesFile.IsEmpty())
    	LoadSIFile(m_SINamesFile);
}

void CEvent::LoadSIFile(wxString& a_File)
{
    // Load new file
    wxTextFile tfile;
    tfile.Open(a_File);
    wxString str;
    
    for ( str = tfile.GetFirstLine(); !tfile.Eof(); str = tfile.GetNextLine() )
        {
    	wxString temp = str.BeforeFirst(' ');
        long number;
    
        if (temp.ToLong(&number))
    		{
    		wxString name = str.AfterFirst(' ').BeforeFirst(',');
    		wxString club = str.AfterLast(',');
    		if (str.Find(',') == wxNOT_FOUND)
    		  club.Empty();
            if (m_SiDetails.find(number) != m_SiDetails.end())
                {
                if (m_SiDetails[number]->GetName() != name)
                    {
                    wxString msg;
                    msg.Printf("Duplicate entry in %s for %ld, using %s instead of %s", SimplifyPath(a_File).c_str(), number, name.c_str(), m_SiDetails[number]->GetName().c_str());
                    LogMsg(msg);
                    m_SiDetails[number]->SetName(name);
                    m_SiDetails[number]->SetClub(club);
                    }
                else if (m_SiDetails[number]->GetClub() != club && !club.IsEmpty() && !m_SiDetails[number]->GetClub().IsEmpty()) 
                    {
                    wxString msg;
                    msg.Printf("Duplicate entry in %s for %ld, using club %s instead of %s", SimplifyPath(a_File).c_str(), number, club.c_str(), m_SiDetails[number]->GetClub().c_str());
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

void CEvent::LoadRawData()
{
    // Clear out old data	
    for (std::vector<CResult*>::iterator i = m_Results.begin(); i != m_Results.end(); i++)
    	delete (*i);
    m_Results.clear();
    
    // Load new file, marking duplcates
    wxTextFile      tfile;
    tfile.Open(m_RawDataFile);
    wxString str;
    
    std::map<CUniquePunch, CResult*>uniqueList;
    std::map<long, int> usedSIs;
    for ( str = tfile.GetFirstLine(); !tfile.Eof(); str = tfile.GetNextLine() )
        {    
    	if (CResult::ValidData(str))
    	    {
            CResult* result = new CResult(str);  
            if (result->GetName().IsEmpty())
                LogResultProblem(result, "No name supplied to match SI number - please fix");
            SetCourse(result);              		
    		if (uniqueList.find(CUniquePunch(result)) != uniqueList.end())
                {
                if (uniqueList[CUniquePunch(result)]->GetRawData().Length() < result->GetRawData().Length())
                    {
                    // want to replace entry in m_Results with new one
                    LogResultProblem(result, "Duplicate download found - ignored"); 
                    std::replace(m_Results.begin(),m_Results.end(), uniqueList[CUniquePunch(result)], result);
                    uniqueList[CUniquePunch(result)] = result;    
                    }
                else // ignore new one, its a copy of something we have
                    {
                    LogResultProblem(result, "Duplicate download found - ignored"); 
                    delete result;
                    }
                }
            else
                {
                uniqueList[CUniquePunch(result)] = result;
                m_Results.push_back(result);  
                if (usedSIs.find(result->GetSINumber()) != usedSIs.end())
                    {
                    wxString s;
                    s.Printf("SI number %ld has already been used by %d %s", 
                                result->GetSINumber(), 
                                usedSIs[result->GetSINumber()],
                                usedSIs[result->GetSINumber()] == 1 ? "runner" : "runners");
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
    return SIMessageBox("OK to close current event " + m_Dir + "?","Close current event", wxYES_NO|wxCENTRE ) == wxID_YES; 	
}

wxString CEvent::FindCourseFile(wxString a_Dir) const
{
    wxArrayString files;
    files.Add("courses.txt");
    files.Add("Courses.txt");
    files.Add("course");
    files.Add("Course");
    files.Add("course.txt");
    files.Add("Course.txt");
    
    return FindDataFile(files,a_Dir);
}

wxString CEvent::FindRawDataFile(wxString a_Dir) const
{
    wxArrayString files;
    files.Add("RawData.csv");
    files.Add("rawdata.csv");
    
    return FindDataFile(files,a_Dir);
}

wxString CEvent::FindSINamesFile(wxString a_Dir) const
{
    wxArrayString files;
    files.Add("SINames");
    files.Add("SINames.txt");
    files.Add("SINamesEvent");
    files.Add("SINamesEvent.txt");
    
    return FindDataFile(files,a_Dir);
}

wxString CEvent::FindSINamesGlobalFile(wxString a_Dir) const
{
    wxArrayString files;
    files.Add("SINames");
    files.Add("SINames.txt");
    files.Add("SINamesGlobal");
    files.Add("SINamesGlobal.txt");
    
    return FindDataFile(files,a_Dir);
}

wxString CEvent::FindDataFile(wxArrayString& a_Candidates, wxString a_Dir) const
{
    for (unsigned int i = 0; i < a_Candidates.GetCount(); i++)
        {
        wxFileName fname(a_Dir, a_Candidates[i]);
        if (fname.FileExists() && fname.IsFileReadable())
            return fname.GetFullPath();
        }
    return "";	
}

void CEvent::LogMsg(wxString a_Msg)
{
    m_LastLogMsg = a_Msg;
    Notify(NFY_LOGMSG);
}	


void CEvent::LogResultProblem(CResult* a_Result, wxString a_Msg)
{
    wxString s;
    s.Printf("(%ld) %ld %s - %s", a_Result->GetRawIndex(), a_Result->GetSINumber(),a_Result->GetName().c_str(), a_Msg.c_str());
    LogMsg(s);
}

void CEvent::DisplayMsg(wxString a_Msg)
{
    m_LastDisplayMsg = a_Msg;
    Notify(NFY_DISPLAYMSG);
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


bool legTimeLessThan(wxTimeSpan a_Lhs, wxTimeSpan a_Rhs)
{
    // the default unset value is 0, need to treat that as longest value.
    if (a_Lhs == a_Rhs)
        return false;
        
    if (a_Lhs.GetSeconds() == 0)
        return false;
        
    if (a_Rhs.GetSeconds() == 0)
        return true;
        
    return a_Lhs < a_Rhs;        
}

void CEvent::SetCourseLegStats(CCourse* a_Course, std::vector<CResult*>& a_SortedResults, std::vector<CResult*>::iterator& a_Start, std::vector<CResult*>::iterator& a_End)
{    
    // for each leg on course, get all leg times in a sorted array. Use as a look-up 
    std::vector<wxTimeSpan> legTimes;
    for (int i = 0; i < a_Course->GetLegCount(); i++)
        {
        for (std::vector<CResult*>::iterator j = a_Start; j != a_End; j++)
            if ((*j)->GetLegStat(i) != NULL)
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
            if ((*j)->GetLegStat(i) != NULL)
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
    a_Result->SetCourse(bestCourse);

    if (m_Courses.size() == 1 && bestCourse == NULL)
        a_Result->SetCourse(m_Courses[0]);
    else
        if (bestCourse == NULL)
            LogResultProblem(a_Result, "Couldn't guess course");

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
                    statistic->m_ElapsedTime = (*punchIter).GetWhen() - a_Result->GetStart().GetWhen();
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

void CEvent::DisplayTextResults()
{
// Display header text
    WriteResultLine("");
    if (m_ShowHTML)
        WriteResultLine("<font size=\"4\"><b>Results</b></font>");
    else
        WriteResultLine("Results");
    WriteResultLine("");
   
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
                WriteResultLine("");                    
                WriteResultLine("Unknown course. Cannot guess course for the following runners:");
                WriteResultLine("");
                gotUnknown = true;
                }
            WriteResultLine(results[i]->GetDisplayName());                
            }
        else
            {
            if (results[i]->GetCourse() != lastCourse)
                {
                lastCourse = results[i]->GetCourse();
                WriteResultLine("");   
                if (m_ShowHTML)
                    WriteResultLine("<b>" + results[i]->GetCourse()->TextDescStr() + "</b>");
                else
                    WriteResultLine(results[i]->GetCourse()->TextDescStr());                
                }
            WriteResultLine(results[i]->TextResultStr());
            }
        }
}

void CEvent::DisplayTextSplits()
{
// Display header text
    WriteResultLine("");
    if (m_ShowHTML)
        WriteResultLine("<font size=\"4\"><b>Splits</b></font>");
    else    
        WriteResultLine("Splits");
    WriteResultLine("");
    WriteResultLine("");
   
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
                WriteResultLine("");  
                if (m_ShowHTML) 
                    WriteResultLine("<b>" + results[i]->GetCourse()->TextDescStr() + "</b>");
                else
                    WriteResultLine(results[i]->GetCourse()->TextDescStr());                
                WriteResultLine("");                   
                WriteResultLine(results[i]->GetCourse()->TextSplitHdrStr());
                }
            WriteResultLine(results[i]->TextElapsedStr());
            WriteResultLine(results[i]->TextLegStr());            
            WriteResultLine(results[i]->TextLegBehindStr());              
            }
        }

}    

void CEvent::ExportXML(wxString a_File)
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

void CEvent::SaveResults(wxString a_File)
{   
    wxTextFile tfile(a_File);
    if (tfile.Exists())
        {
        tfile.Clear();
        tfile.Write();
        if (!tfile.Open(a_File))
            {
            SIMessageBox("Unable to open file " + a_File + " to write results.");    
            return;
            }
        }
    else
        if (!tfile.Create(a_File))
            {
            SIMessageBox("Unable to create file " + a_File + " to write results.");    
            return;
            }
            
    m_SavingResults = true;
    m_SaveFile = &tfile;
    RecalcResults(); // only if results may have changed ?
    tfile.Write();
    m_SavingResults = false;    
}


void CEvent::GetCourseNames(wxArrayString& a_Names)
{
    a_Names.Clear();
    for (unsigned int i = 0; i < m_Courses.size(); i++)
        a_Names.Add(m_Courses[i]->GetName()); 
}

void CEvent::SetResultCourse(CResult* a_Result, wxString a_CourseName)
{
    for (unsigned int i = 0; i < m_Courses.size(); i++)
        if (m_Courses[i]->GetName() == a_CourseName)
            {
            a_Result->SetCourse(m_Courses[i]); 
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

wxString CEvent::GetEventName()
{
    return m_EventName;
}

void CEvent::SetEventName(wxString a_Name)
{
    m_EventName = a_Name;
}

void CEvent::WriteResultLine(wxString a_Text)
{
    if (m_SavingResults)
        m_SaveFile->AddLine(a_Text);
    else
        DisplayMsg(a_Text);
}