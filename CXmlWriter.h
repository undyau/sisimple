// Class automatically generated by Dev-C++ New Class wizard

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