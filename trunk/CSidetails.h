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

#ifndef CSIDETAILS_H
#define CSIDETAILS_H
#include <QString>

// Objects in this class represent the known details relating to an SI stick
class CSiDetails
{
    public:
        // class constructor
        CSiDetails(long int a_Number);
        CSiDetails(long int a_Number, QString a_Name);
        CSiDetails(long int a_Number, QString a_Name, QString a_Club);

        QString GetName() {return m_Name;};
        QString GetClub() {return m_Club;};
        bool GetRental() {return m_Rental;};

        QString SetName(QString a_Name) {return (m_Name = a_Name);};
        QString SetClub(QString a_Club) {return (m_Club = a_Club);};
        bool SetRental(bool a_Rental) {return (m_Rental = a_Rental);};

        QString ExportText();

        // class destructor
        ~CSiDetails();

    private:
        long m_Number;
        QString m_Name;
        QString m_Club;
        bool m_Rental;
};

#endif // CSIDETAILS_H
