#ifndef CIOFRESULTXMLHANDLER_H
#define CIOFRESULTXMLHANDLER_H

#include <QXmlDefaultHandler>
#include <map>

class CCourse;

class CIofResultXmlHandler : public QXmlDefaultHandler
{

enum e_State{inClassName, inName, inClub, inCardId,
             inTime, inControlCode, inSplitTime,
             inOther};
public:
    CIofResultXmlHandler();

    virtual bool endElement( const QString&, const QString&, const QString &name );
    virtual bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );
    virtual bool characters(const QString &ch);

private:
    e_State m_State;
    QString m_CourseName;
    QString m_Name;
    QString m_SINumber;
    QString m_Club;
    QString m_Time;
    QString m_Status;
    QStringList m_Controls;    
    QStringList m_Splits;
    QStringList m_CourseControls;

    std::map<QString, e_State> m_States;
};

#endif // CIOFRESULTXMLHANDLER_H
