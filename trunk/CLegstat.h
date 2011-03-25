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

#ifndef CLEGSTAT_H
#define CLEGSTAT_H
#include <QDateTime>
#include "CPunch.h"
#include <QDataStream>

// lightweight class holding statistical information for a single leg for a single runner
class CLegStat
{
    public:
        // class constructor
        CLegStat(int a_Index, CPunch a_Punch);
        // class destructor
        ~CLegStat();

       long m_Index; // index of leg within course
       long m_ElapsedTime; // elapsed time in seconds
       long m_ElapsedPos;
       long m_ElapsedBehind;

       long m_LegTime;
       long m_LegBehind;
       CPunch m_Punch;
       long m_LegPos;
};

QDataStream &operator<<(QDataStream &out, const CLegStat &a_LegStat);
QDataStream &operator>>(QDataStream &in, CLegStat &a_LegStat);


#endif // CLEGSTAT_H
