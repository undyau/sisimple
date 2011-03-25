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

#ifndef CPUNCH_H
#define CPUNCH_H
#include <QDateTime>
#include <QString>

// Represents a punch at a control
class CPunch
{
    public:
        // class constructor
        CPunch(int a_CN, QString a_DOW, QDateTime a_When, bool a_Ignore = false);
        CPunch();
        CPunch(const CPunch& a_Copy);
        CPunch& operator=(const CPunch& a_Other);
        bool operator<(const CPunch& a_Other) const;
        bool operator==(const CPunch& a_Other) const;
        bool operator!=(const CPunch& a_Other) const {return !operator==(a_Other);};

                void SetData(long a_CN, QString a_DOW, QDateTime a_When);

        // class destructor
        ~CPunch();
        void SetCN(long x); // sets the value of m_CN
        long GetCN() const; // returns the value of m_CN
        void SetDOW(QString x); // sets the value of m_DOW
        QString GetDOW() const; // returns the value of m_DOW
        void SetWhen(QDateTime x); // sets the value of m_When
        QDateTime GetWhen() const; // returns the value of m_When
        void SetIgnore(bool a_Ignore) {m_Ignore = a_Ignore;};
        bool GetIgnore() const {return m_Ignore;};

    private:
        // Control Number
        long m_CN;
        // Day of week
        QString m_DOW;
        // Tme that runner punched control
        QDateTime m_When;
        bool m_Ignore;
};

QDataStream &operator<<(QDataStream &out, const CPunch &a_Punch);
QDataStream &operator>>(QDataStream &in, CPunch &a_Punch);

#endif // CPUNCH_H
