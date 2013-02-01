#include "ciofresultxmlhandler.h"
#include "CCourse.h"
#include "CEvent.h"
#include "QDebug"
#include "CResult.h"
#include "Utils.h"

int CIofResultXmlHandler::m_ResultCount = 0;

CIofResultXmlHandler::CIofResultXmlHandler() : m_Valid(false)
    {

    }


bool CIofResultXmlHandler::endElement( const QString&, const QString&, const QString &name )
{
    if (m_Tags.back() != name)
        qDebug() << "Tag found out of order: " << name;
    else
        m_Tags.pop_back();

    if (name == "Event")
        CEvent::Event()->SetEventName(m_EventName);

    if (name == "PersonResult")
        {
        CResult* result = new CResult(CIofResultXmlHandler::m_ResultCount++,
                            m_StartTime,
                            m_FinishTime,
                            m_SINumber.toLong(),
                            QString(m_GivenName + ' ' + m_FamilyName),
                            m_Club,
                            m_Time,
                            m_Status,
                            m_Controls,
                            m_Splits);
        CEvent::Event()->AddRecoveredResult(result);
        if (m_Controls.size() > m_CourseControls.size())
            m_CourseControls = m_Controls;
        m_CourseResults.push_back(result);
        }

    if (name == "ClassResult")
        {
        CCourse* course = new CCourse(m_CourseName, m_CourseLen, m_CourseClimb, m_CourseControls);
        CEvent::Event()->addNewCourse(course);
        for (unsigned int i = 0; i < m_CourseResults.size(); i++)
            m_CourseResults.at(i)->SetCourse(course);
        }

    return true;
}

bool CIofResultXmlHandler::characters(const QString &ch)
    {
    if (Parent() == "Class" && m_Tags.back() == "Name") {m_CourseName = ch; return true;}
    if (Parent() == "Name" && m_Tags.back() =="Family") {m_FamilyName = ch; return true;}
    if (Parent() == "Name" && m_Tags.back() =="Given") {m_GivenName = ch; return true;}
    if (m_Tags.back() =="ControlCard") {m_SINumber = ch; return true;}
    if (Parent() == "SplitTime" && m_Tags.back() =="ControlCode") {m_Controls.append(ch); return true;}
    if (Parent() == "Course" && m_Tags.back() =="Length") {m_CourseLen = ch; return true;}
    if (Parent() == "Course" && m_Tags.back() =="Climb") {m_CourseClimb = ch; return true;}
    if (Parent() == "Event" && m_Tags.back() =="Name") {m_EventName = ch; return true;}
    if (Parent() == "Organisation" && m_Tags.back() =="Name") {m_Club = ch; return true;}
    if (Parent() == "Result" && m_Tags.back() =="StartTime") {m_StartTime = ch; return true;}
    if (Parent() == "Result" && m_Tags.back() =="FinishTime") {m_FinishTime = ch; return true;}
    if (Parent() == "Result" && m_Tags.back() =="Status") {m_Status = ch; return true;}
    if (Parent() == "Result" && m_Tags.back() =="Time")
        {
        m_Time = TimeTakenTo0BasedTime(FormatTimeTaken(ch.toLong()));
        return true;
        }
    if (Parent() == "SplitTime" && m_Tags.back() =="Time")
        {
        m_Splits.append(ch);
        return true;
        }

    return true;
    }

bool CIofResultXmlHandler::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
{
    if (!m_Valid)
        m_Valid = name == "ResultList";

    if (!m_Valid)
        {
        CEvent::Event()->LogMsg("Invalid results XML file");
        return false;
        }

    m_Tags.push_back(name);

    if (name == "SplitTime")
        if (attrs.index(QString("status")) >= 0 &&
            attrs.value(attrs.index(QString("status"))) == "Missing")
            m_Splits.append("Missing");

    if (name == "PersonResult")
        {
        m_GivenName.clear();
        m_FamilyName.clear();
        m_SINumber.clear();
        m_Club.clear();
        m_Time.clear();
        m_Status.clear();
        m_Controls.clear();
        m_Splits.clear();
        m_FinishTime.clear();
        m_StartTime.clear();
        }

    if (name == "ClassResult")
        {
        m_CourseName.clear();
        m_CourseLen.clear();
        m_CourseClimb.clear();
        m_CourseControls.clear();
        m_CourseResults.empty();
        }

    return true;
}

QString CIofResultXmlHandler::Parent()
{
    if (m_Tags.count() < 2)
        return "";

    return (m_Tags[m_Tags.count()-2]);
}
