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

#ifndef CRESULT_H
#define CRESULT_H
#include <QString>
#include <QObject>
#include "CPunch.h"
#include <list>
#include <map>
#include "CXmlWriter.h"
class CCourse;
class CLegStat;

// Objects of this class represent a single result - the start, finish,
// SI number, splits and any data that is provided by user-interaction
class CResult  : public QObject
{
    Q_OBJECT
    public:
        // class constructor
        CResult(QString& a_RawData);
        // class destructor
                ~CResult();

        static bool ValidData(QString& a_Data);
        CPunch GetClear() {return m_Clear;}
        CPunch GetCheck() {return m_Check;}
        CPunch GetStart() {return m_Start;}
        CPunch GetFinish() {return m_Finish;}
        QString GetRawData() {return m_RawData;}
        long GetSINumber() {return m_SINumber;}
        long GetRawIndex() {return m_RawIndex;}
        QString GetDisplayName();
        QString GetName() {return m_Name;}
        QString GetClub() {return m_Club;}

        bool GetFinished();
        void SetFinished(bool a_Finished) {m_Finished = a_Finished;}
        void SetFinishedOverride(bool a_Finished);
        void PunchedControls(std::list<CPunch>& a_Controls, bool a_IncludeStartFinish);
        QString RawDataDisplayStr();
        QString TextResultStr();
        QString TextElapsedStr();
        QString TextLegStr();
        QString TextLegBehindStr();
        long LegTime(long a_Leg);
        void SetPos(long a_Pos) {m_Pos = a_Pos;}
        long GetPos() {return m_Pos;}
        bool GetInvalid() {return m_Invalid;}
        void ClearValidPunches();
        void AddLegStat(CLegStat* a_LegStat);
        void ClearLegStats();
        CLegStat* GetLegStat(int a_Leg);
        CCourse* GetCourse() {return m_Course;}
        long TimeTaken();
        void AddXML(CXmlWriter& a_Writer);
        void SetCourse(CCourse* a_Course) {m_Course = a_Course;}
        QString DebugStr();
        bool Altered() {return m_Altered;}
        int GetStartCN() {return m_Start.GetCN();}
        int GetFinishCN() {return m_Finish.GetCN();}

    public slots:
        void SetName(QString a_Name);
        void SetCourse(QString a_Course);
        void SetClub(QString a_Club);

    private:
        void DoTimeSanityCheck();
        void ProcessResult();
        CResult(const CResult& a_Copy);  // hide copy constructor
        CResult(); // hide default constructor
        QString PersonID();
        QString ClubID();

        QString m_RawData;
        bool m_ProcessedResult;
        bool m_Invalid;
        bool m_Finished;
        long m_Pos;
        bool m_FinishedOverride;
        bool m_FinishedOverrideSet;
        CCourse* m_Course;
        bool m_Altered;


        long m_SINumber;
        long m_RawIndex;
        QString m_Name;
        QString m_Club;

        CPunch m_Clear;
        CPunch m_Check;
        CPunch m_Start;
        CPunch m_Finish;

        std::list<CPunch*> m_Punches;
        std::map<int, CLegStat*> m_LegStats;

};

#endif // CRESULT_H
