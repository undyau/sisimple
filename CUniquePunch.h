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

#ifndef CUNIQUEPUNCH_H
#define CUNIQUEPUNCH_H
#include "CResult.h"

// Combination of a CPunch and an SI number. USed for unique sort when detecting duplicates
class CUniquePunch
{
    public:
        // class constructor
        CUniquePunch(CResult* a_Result);
        // class destructor
        ~CUniquePunch();
        bool operator<(const CUniquePunch& a_Other) const;

        // Default copy constructor is OK

        CPunch m_Punch;
        long m_SINumber;
};

#endif // CUNIQUEPUNCH_H
