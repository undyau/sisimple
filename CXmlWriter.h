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

#ifndef CXMLWRITER_H
#define CXMLWRITER_H
#include <QString>
#include <stack>

// Objects in this class can be used to create then write out an XML tree
class CXmlWriter
{
    public:
        // class constructor
        CXmlWriter(QString a_DocType, QString a_DtdName);
        // class destructor
        ~CXmlWriter();
        void StartElement(QString a_ElementName, QString a_Attributes = "");
        void EndElement();
        void AddValue(QString a_Value);
        void AddValue(long a_Value);
        bool WriteToFile(QString a_File);

    private:
        QString m_Output;
        std::stack<QString> m_Elements;
        bool m_GotValue;
        QString m_LastElement;
};

#endif // CXMLWRITER_H
