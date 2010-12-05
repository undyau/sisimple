#include "ciofresultxmlhandler.h"
#include "CCourse.h"
#include "CEvent.h"

CIofResultXmlHandler::CIofResultXmlHandler()
{
    m_States["CourseName"] = inName;
    m_States["CourseLength"] = inLength;
    m_States["CourseClimb"] = inClimb;
    m_States["ControlCode"] = inControls;
}

bool CIofResultXmlHandler::endElement( const QString&, const QString&, const QString &name )
{
    if (name == "Course")
        {
        CCourse* course = new CCourse(m_Name, m_Length, m_Climb, m_Controls);
        CEvent::Event()->addNewCourse(course);
        }
    return true;
}

bool CIofResultXmlHandler::characters(const QString &ch)
    {
    switch (m_State)
        {
        case inName: m_Name = ch; break;
        case inLength: m_Length = ch; break;
        case inClimb: m_Climb = ch; break;
        case inControls: m_Controls.append(ch); break;
        case inOther: break;
        }
    return true;
    }

bool CIofResultXmlHandler::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
{
    if (m_States.find(name) != m_States.end())
        m_State = m_States[name];
    else
        m_State = inOther;

    if (name == "Course")
        {
        m_Length.clear();
        m_Name.clear();
        m_Controls.clear();
        return true;
        }

    return true;
}
