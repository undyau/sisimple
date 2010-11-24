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

#ifndef CLEG_H
#define CLEG_H
#include <QString>

// An object of this class represents a leg on a course
class CLeg
{
    public:
        // class constructor
        CLeg(long start_CN, long end_CN);
        // class destructor
        ~CLeg();

        long GetStartCN() {return m_StartCN;};
        long GetEndCN() {return m_EndCN;};
        bool GetUntimed() {return m_Untimed;};
        bool GetOptional() {return m_Optional;};
        bool IsFinishLeg() {return m_EndCN < 0;};

        void SetOptional(bool a_Optional) {m_Optional = a_Optional;};
        void SetUntimed(bool a_Untimed) {m_Untimed = a_Untimed;};
        QString Label();

    private:
        long m_StartCN;
        long m_EndCN;
        bool m_Untimed;
        bool m_Optional;
};

#endif // CLEG_H
