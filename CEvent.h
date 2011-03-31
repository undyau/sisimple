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
//
// This event is a singleton

#ifndef CEVENT_H
#define CEVENT_H

#include <QString>
#include <QObject>
#include <vector>
#include <map>
#include <QStringList>
class CSiDetails;
class CResult;
class CCourse;
class QFile;

// Class encapsulating an event
class CEvent : public QObject
{
    Q_OBJECT

    private:
        // class constructor
        CEvent();
        CEvent(CEvent const&);         // Don't Implement
        void operator=(CEvent const&); // Don't implement

signals:
    void filesChanged(const QString& a_RawData, const QString &a_Courses, const QString& a_SiNames, const QString& a_GlobalSiNames);
    void logMsg(const QString& a_LogMsg);
    void displayMsg(const QString& a_LogMsg);
    void updatedResultsOutput(std::vector<QString>& a_Lines);
    void newCourseExists(QString);
    void deletedCourse(QString);
    void coursesGuessed();
    void findImportFile();
    void resetLog();
    void loadedSIArchive(QString);
    void exportIOF();

    public:
        // class destructor
        ~CEvent();
        // Singleton access/creation
        static CEvent* Event();
        QString Directory();
        bool SetResultsInputFile(QString a_File);
        QString LastLogMsg() {return m_LastLogMsg;};
        QString LastDisplayMsg() {return m_LastDisplayMsg;};
        void GetCourseNames(QStringList& a_Names);
        void addNewCourse(CCourse* a_Course);  // Add a new course to event
        void ProcessSIDData(QStringList& a_Records, bool a_Append);
        void ProcessSISimpleData(QStringList& a_Records, bool a_Append);
        void RecalcResults();
        QString GetRentalNames();
        void SetRentalNames(QString& a_Names);
        void AddRecoveredResult(CResult* a_Result);

        CSiDetails* GetSIData(long a_SINumber);
        void LogMsg(QString a_Msg);
        void LogResultProblem(CResult* a_Result, QString a_Msg);
        void DisplayMsg(QString a_Msg);
        int  CompareCourseIndex(CCourse* a_Lhs, CCourse* a_Rhs);
        CResult* GetResult(long a_RawIndex);
        unsigned int GetCourseCount() {return m_Courses.size();}
        void SetResultCourse(CResult* a_Result, QString a_CourseName);

        void ExportXML(QString a_File);
        void SaveResults(QString a_File);

        bool GetShowSplits();
        bool GetShowHTML();
        void SetShowSplits(bool a_Show);
        void SetShowHTML(bool a_Show);
        QString GetEventName();
        void SetEventName(QString a_Name);
        bool GetResultFinished(long a_Index);
        bool GetResultInvalid(long a_Index);
        bool ResultExists(long a_Index);
        CCourse* CourseFromName(QString a_Name);
        long LookupResult(QString a_Name, QString a_Result);
        bool CanClose();

    private slots:
        void dnfResult(long a_Index); // DNF someone
        void deleteDownload(long a_Index); // Disappear someone
        void reinstateResult(long a_Index); // Reinstate someone
        void newCourse(CCourse* a_Course);  // Add a new course to event
        void deleteCourse(CCourse* a_Course);  // Remove course from event
        void guessCourses();  // Guess what courses exist from punching records
        void importCourses(QString a_FileName); // import courses in IOF XML format
        void newSIData(QString); // new SI data has been loaded
        void NewDownloadData(QStringList a_NewData); // new download data has been read

    private:
        QString FindDataFile(QStringList& a_Candidates, QString a_Dir) const;
        void LoadRawData();
        void LoadCourseData(bool& a_Guess);
        bool LoadEventFromXML(QString a_FileName);
        bool LoadEventFromDump();

        void SetCourse(CResult* a_Result);
        void DisplayRawData();
        void CalcResults();
        void SetPositions(std::vector<CResult*>& a_SortedResults);
        void SetLegStats(std::vector<CResult*>& a_SortedResults);
        void SetCourseLegStats(CCourse* a_Course, std::vector<CResult*>::iterator& a_Start, std::vector<CResult*>::iterator& a_End);
        void SetPunchTimes(CResult* a_Result);
        void DisplayTextResults(std::vector<QString>& a_Lines);
        void DisplayTextSplits(std::vector<QString>& a_Lines);
        void DisplayHTMLSplits(std::vector<QString>& a_Lines);
        void WriteResults(std::vector<QString>& a_Lines);
        void CollectPunchSequences(std::map<std::list<long>, int >& a_Sequences); // get punch data to guess courses
        void AddGuessedCourses(std::map<std::list<long>, int >& a_Sequences); // Add the courses that have been guessed
        bool ContinueCourseLoad();
        void EliminateMispunchSequences(std::map<std::list<long>, int >& a_Sequences);
        int ControlsMissing(const std::list<long>& a_Good, const std::list<long>& a_Candidate);
        void LoadCoursesFromXML(QString a_FileName);
        bool IsRental(QString a_Name);
        void SaveChangedSIDetails();
        void SICardsAsList(QStringList& a_List);
        void ProcessRawData();

        bool m_ChangedSinceSave;
        bool m_ShowSplits;
        bool m_ShowHTML;
        QString m_EventName;
        QString m_Dir;
        QString m_ResultsInputFile;
        QString m_LastLogMsg;
        QString m_LastDisplayMsg;
        bool m_SavingResults;
        QFile* m_SaveFile;
        int m_GoodThreshold;
        QString m_RentalNames;
        QStringList m_OriginalResultsData;

        static CEvent* _instance;
        std::vector<CCourse*> m_Courses;
        std::map<long, CSiDetails*> m_SiDetails;
        std::vector<CResult*> m_Results;
};

#endif // CEVENT_H
