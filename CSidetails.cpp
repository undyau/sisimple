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

#include "CSidetails.h" // class's header file

// class constructor
CSiDetails::CSiDetails(long int a_Number): m_Number(a_Number), m_Rental(false)
{
    // insert your code here
}

CSiDetails::CSiDetails(long int a_Number, QString a_Name): m_Number(a_Number), m_Name(a_Name),m_Rental(false)
{
    // insert your code here
}

CSiDetails::CSiDetails(long int a_Number, QString a_Name, QString a_Club): m_Number(a_Number), m_Name(a_Name), m_Club(a_Club),m_Rental(false)
{
    // insert your code here
}



// class destructor
CSiDetails::~CSiDetails()
{
    // insert your code here
}

QString CSiDetails::ExportText()
{
    QString result;
    if (!m_Club.isEmpty())
        {
        result = "%1,\"%2\",\"%3\"";
        result.arg(m_Number).arg(m_Name, m_Club);
        }
    return result;
}

