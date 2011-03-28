#include "ciofresultxmlhandler.h"
#include "CCourse.h"
#include "CEvent.h"
#include "QDebug"

CIofResultXmlHandler::CIofResultXmlHandler()
    {
    m_States["ClassShortName"] = inClassName;
    m_States["Family"] = inName;
    m_States["Given"] = inName;
    m_States["ShortName"] = inClub;
    m_States["CCardId"] = inCardId;
    m_States["Time"] = inTime;
    m_States["ControlCode"] = inControlCode;
    }


bool CIofResultXmlHandler::endElement( const QString&, const QString&, const QString &name )
{
    if (name == "PersonResult")
        {
       /* if (m_LengthType == "m")
            m_Length = QString("%1").arg(m_Length.toFloat()/1000,0,'g',3);
        else if (m_LengthType == "ft")
            m_Length = QString("%1").arg(m_Length.toFloat()/3280.8399,0,'g',3);
        CCourse* course = new CCourse(m_Name, m_Length, m_Climb, m_Controls);
        CEvent::Event()->addNewCourse(course);*/
        if (m_Controls.size() > m_CourseControls.size())
            m_CourseControls = m_Controls;
        }
    if (name == "ClassResult")
        {

        }
    m_State = inOther;
    return true;
}

bool CIofResultXmlHandler::characters(const QString &ch)
    {
    switch (m_State)
        {
        case inClassName: m_CourseName = ch; break;
        case inName: m_Name = m_Name + (m_Name.isEmpty() ? "" : " ") + ch; break;
        case inCardId: m_SINumber = ch; break;
        case inTime: m_Time = ch; break;
        case inClub: m_Club = ch; break;
        case inControlCode: m_Controls.append(ch); break;
        case inSplitTime: m_Splits.append(ch); break;
        case inOther: break;
        }
    return true;
    }

bool CIofResultXmlHandler::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
{
    if (m_State == inControlCode && name == "Time")
        m_State = inSplitTime;
    else if (m_States.find(name) != m_States.end())
        m_State = m_States[name];
    else
        m_State = inOther;

    if (name == "PersonResult")
        {
        m_Name.clear();
        m_SINumber.clear();
        m_Club.clear();
        m_Time.clear();
        m_Status.clear();
        m_Controls.clear();
        m_Splits.clear();
        }

    if (name == "ClassResult")
        {
        m_CourseName.clear();
        m_CourseControls.clear();
        }

    if (name == "CompetitorStatus")
        {
        if (attrs.index(QString("value")) >= 0)
            m_Status = attrs.value(attrs.index(QString("value")));
        }
    return true;
}
