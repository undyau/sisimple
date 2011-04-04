#ifndef CIOFRESULTXMLHANDLER_H
#define CIOFRESULTXMLHANDLER_H

#include <QXmlDefaultHandler>
#include <map>
#include <vector>

class CCourse;
class CResult;

class CIofResultXmlHandler : public QXmlDefaultHandler
{

enum e_State{inClassName, inFamilyName, inGivenName, inClub, inCardId,
             inTime, inControlCode, inSplitTime, inEventId, inCourseLen, inCourseClimb,
             inOther};
public:
    CIofResultXmlHandler();

    virtual bool endElement( const QString&, const QString&, const QString &name );
    virtual bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );
    virtual bool characters(const QString &ch);

private:
    bool m_Valid;
    e_State m_State;
    e_State m_PrevState;
    QString m_EventId;
    QString m_CourseName;
    QString m_CourseLen;   // Not in IOF XML, just used by SI Simple internally
    QString m_CourseClimb; // Not in IOF XML, just used by SI Simple internally
    QString m_FamilyName;
    QString m_GivenName;
    QString m_SINumber;
    QString m_Club;
    QString m_Time;
    QString m_Status;
    QStringList m_Controls;
    QStringList m_Splits;
    QStringList m_CourseControls;
    static int m_ResultCount;

    std::map<QString, e_State> m_States;
    std::vector<CResult*> m_CourseResults;
};

#endif // CIOFRESULTXMLHANDLER_H
