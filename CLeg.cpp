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

#include "CLeg.h" // class's header file
#include <QDataStream>

// class constructor
CLeg::CLeg(long a_StartCN, long a_EndCN): m_StartCN(a_StartCN), m_EndCN(a_EndCN),
    m_Untimed(false), m_Optional(false)
{
    // insert your code here
}

// class destructor
CLeg::~CLeg()
{
    // insert your code here
}

QString CLeg::Label()
{
    if (m_EndCN < 0)
        return "F";

    QString s("%1");
    s.arg(m_EndCN);
    return s;
}


QDataStream &operator<<(QDataStream &out, const CLeg &a_Leg)
{
    out << (qint32) a_Leg.GetStartCN() << (qint32) a_Leg.GetEndCN()
        << a_Leg.GetUntimed() << a_Leg.GetOptional();
    return out;
}

QDataStream &operator>>(QDataStream &in, CLeg &a_Leg)
{
    qint32 StartCN;
    qint32 EndCN;
    bool untimed, optional;

    in >> StartCN >> EndCN >> untimed >> optional;

    a_Leg = CLeg(StartCN, EndCN);
    a_Leg.SetOptional(optional);
    a_Leg.SetUntimed(untimed);
    return in;
}
