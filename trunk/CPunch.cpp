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

#include "CPunch.h" // class's header file
#include "Utils.h"

// class constructor
CPunch::CPunch(int a_CN, QString a_DOW, QDateTime a_When) : m_CN(a_CN), m_DOW(a_DOW), m_When(a_When),m_Ignore(false)
{
    // insert your code here
}

CPunch::CPunch(const CPunch& a_Copy) : m_CN(a_Copy.m_CN), m_DOW(a_Copy.m_DOW), m_When(a_Copy.m_When),m_Ignore(false)
{
}

CPunch::CPunch()
{
}

CPunch& CPunch::operator=(const CPunch& a_Other)
{
    m_CN = a_Other.m_CN;
    m_DOW = a_Other.m_DOW;
    m_When = a_Other.m_When;
    return *this;
}

bool CPunch::operator<(const CPunch& a_Other) const
{
    if (m_CN != a_Other.m_CN)
        return m_CN < a_Other.m_CN;
    if (m_DOW != a_Other.m_DOW)
        return DOWToInt(m_DOW) < DOWToInt(a_Other.m_DOW);

    return m_When < a_Other.m_When;
}

bool CPunch::operator==(const CPunch& a_Other) const
{
    return m_CN == a_Other.m_CN && m_DOW == a_Other.m_DOW && m_When == a_Other.m_When;
}

void CPunch::SetData(long a_CN, QString a_DOW, QDateTime a_When)
{
    m_CN = a_CN;
    m_DOW = a_DOW;
    m_When = a_When;
}

// class destructor
CPunch::~CPunch()
{
    // insert your code here
}


// returns the value of m_CN
long CPunch::GetCN()
{
    return m_CN;
}


// sets the value of m_CN
void CPunch::SetCN(long x)
{
    m_CN = x;
}


// returns the value of m_DOW
QString CPunch::GetDOW()
{
    return m_DOW;
}


// sets the value of m_DOW
void CPunch::SetDOW(QString x)
{
    m_DOW = x;
}


// returns the value of m_When
QDateTime CPunch::GetWhen()
{
    return m_When;
}


// sets the value of m_When
void CPunch::SetWhen(QDateTime x)
{
    m_When = x;
}
