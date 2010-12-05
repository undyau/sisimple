#ifndef CIOFRESULTXMLHANDLER_H
#define CIOFRESULTXMLHANDLER_H

#include <QXmlDefaultHandler>
#include <map>

class CCourse;

class CIofResultXmlHandler : public QXmlDefaultHandler
{

enum e_State{inName, inLength, inClimb, inControls, inOther};
public:
    CIofResultXmlHandler();

    virtual bool endElement( const QString&, const QString&, const QString &name );
    virtual bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );
    virtual bool characters(const QString &ch);

private:
    e_State m_State;
    QString m_Name;
    QString m_Length;
    QString m_Climb;
    QString m_LengthType;
    QStringList m_Controls;
    std::map<QString, e_State> m_States;
};

#endif // CIOFRESULTXMLHANDLER_H
