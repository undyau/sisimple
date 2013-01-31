#ifndef CIOFRESULTXMLHANDLER_H
#define CIOFRESULTXMLHANDLER_H

#include <QXmlDefaultHandler>
#include <map>
#include <vector>

class CCourse;
class CResult;

class CIofResultXmlHandler : public QXmlDefaultHandler
{
public:
    CIofResultXmlHandler();

    virtual bool endElement( const QString&, const QString&, const QString &name );
    virtual bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );
    virtual bool characters(const QString &ch);

private:
    bool m_Valid;
    QString m_EventName;
    QString m_CourseName;
    QString m_CourseLen;
    QString m_CourseClimb;
    QString m_FamilyName;
    QString m_GivenName;
    QString m_SINumber;
    QString m_Club;
    QString m_Time;
    QString m_Status;
    QStringList m_Controls;
    QStringList m_Splits;
    QStringList m_CourseControls;
    QString m_StartTime;
    QString m_FinishTime;
    static int m_ResultCount;

    std::vector<CResult*> m_CourseResults;
    QStringList m_Tags;

    QString Parent();
};

#endif // CIOFRESULTXMLHANDLER_H
