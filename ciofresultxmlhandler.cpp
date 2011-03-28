#include "ciofresultxmlhandler.h"
#include "CCourse.h"
#include "CEvent.h"
#include "QDebug"
#include "CResult.h"

int CIofResultXmlHandler::m_ResultCount = 0;

CIofResultXmlHandler::CIofResultXmlHandler()
    {
    m_States["ClassShortName"] = inClassName;
    m_States["Family"] = inFamilyName;
    m_States["Given"] = inGivenName;
    m_States["ShortName"] = inClub;
    m_States["CCardId"] = inCardId;
    m_States["Time"] = inTime;
    m_States["ControlCode"] = inControlCode;
    }


bool CIofResultXmlHandler::endElement( const QString&, const QString&, const QString &name )
{
    if (name == "PersonResult")
        {
        CResult* result = new CResult(CIofResultXmlHandler::m_ResultCount++,
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
        QString len, climb;
        CCourse* course = new CCourse(m_CourseName, len, climb, m_CourseControls);
        CEvent::Event()->addNewCourse(course);
        for (unsigned int i = 0; i < m_CourseResults.size(); i++)
            m_CourseResults.at(i)->SetCourse(course);
        }
    m_State = inOther;
    return true;
}

bool CIofResultXmlHandler::characters(const QString &ch)
    {
    switch (m_State)
        {
        case inClassName: m_CourseName = ch; break;
        case inFamilyName: m_FamilyName = m_FamilyName + (m_FamilyName.isEmpty() ? "" : " ") + ch; break;
        case inGivenName: m_GivenName = m_GivenName + (m_GivenName.isEmpty() ? "" : " ") + ch; break;
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
        m_GivenName.clear();
        m_FamilyName.clear();
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
        m_CourseResults.empty();
        }

    if (name == "CompetitorStatus")
        {
        if (attrs.index(QString("value")) >= 0)
            m_Status = attrs.value(attrs.index(QString("value")));
        }
    return true;
}
