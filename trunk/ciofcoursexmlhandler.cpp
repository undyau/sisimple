#include "ciofcoursexmlhandler.h"
#include "CCourse.h"
#include "CEvent.h"
#include "QDebug"

CIofCourseXmlHandler::CIofCourseXmlHandler()
    {
    m_States["CourseName"] = inName;
    m_States["CourseLength"] = inLength;
    m_States["CourseClimb"] = inClimb;
    m_States["ControlCode"] = inControls;
    }


bool CIofCourseXmlHandler::endElement( const QString&, const QString&, const QString &name )
{
    if (name == "Course")
        {
        if (m_LengthType == "m")
            m_Length = QString("%1").arg(m_Length.toFloat()/1000,0,'g',3);
        else if (m_LengthType == "ft")
            m_Length = QString("%1").arg(m_Length.toFloat()/3280.8399,0,'g',3);
        CCourse* course = new CCourse(m_Name, m_Length, m_Climb, m_Controls);
        CEvent::Event()->addNewCourse(course);
        }
    m_State = inOther;
    return true;
}

bool CIofCourseXmlHandler::characters(const QString &ch)
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

bool CIofCourseXmlHandler::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
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

    if (name == "CourseLength")
        {
        if (attrs.index(QString("unit")) >= 0)
            m_LengthType = attrs.value(attrs.index(QString("unit")));
        else
            m_LengthType = "m";
        }

    return true;
}
