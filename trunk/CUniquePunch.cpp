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

#include "CUniquePunch.h" // class's header file

// class constructor
CUniquePunch::CUniquePunch(CResult* a_Result) : m_Punch(a_Result->GetCheck()), m_SINumber(a_Result->GetSINumber())
{
    // insert your code here
}

// class destructor
CUniquePunch::~CUniquePunch()
{
    // insert your code here
}

bool CUniquePunch::operator<(const CUniquePunch& a_Other) const
{
    if (m_Punch != a_Other.m_Punch)
        return m_Punch < a_Other.m_Punch;
    return m_SINumber < a_Other.m_SINumber;
}
