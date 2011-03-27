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

#ifndef CCOURSE_H
#define CCOURSE_H

#include <vector>
#include <QString>
#include <list>
#include "CLeg.h"

// Objects in this class represent a course for the event.
class CCourse
{
    public:
        // class constructor
        CCourse(QString& a_Data);
        CCourse(QString& a_Name, QString& a_Length, QString& a_Climb, QStringList& a_Controls);
        // class destructor
        ~CCourse();

        static bool ValidData(QString& a_Data);
        void SetUntimedLeg(int a_Index);
        void SetOptionalLeg(int a_Index);
        bool GetUntimedLeg(int a_Index);
        bool GetOptionalLeg(int a_Index);
        QString GetLength() {return m_Length;};

        void CompulsoryControls(std::list<long>& a_Controls);
        void GetControls(QStringList& a_Controls) const;
        QString GetName() {return m_Name;};
        QString GetClimb() {return m_Climb;};
        QString SetName(QString a_Name) {return (m_Name = a_Name);};
        CLeg GetLeg(int a_Index);
        int GetLegCount() {return m_Controls.size();};
        QString TextDescStr();
        QString TextSplitHdrStr();

        void Alter(QString& a_Name, QString& a_Length, QString& a_Climb, QStringList a_Controls);

    private:
        QString m_Name;
        QString m_Length;
        QString m_Climb;

        std::vector<CLeg*> m_Controls;
};

#endif // CCOURSE_H
