#ifndef CIOFRESULTXMLHANDLER_H
#define CIOFRESULTXMLHANDLER_H

#include <QXmlDefaultHandler>
#include <map>

class CIofResultXmlHandler : public QXmlDefaultHandler
{
Q_OBJECT

enum e_State{inName, inLength, inClimb, inControls, inOther};
public:
    CIofResultXmlHandler();
    virtual bool startDocument();
    virtual bool endElement( const QString&, const QString&, const QString &name );
    virtual bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );
    virtual bool characters(const QString &ch);

signals:
    void newCourse(CCourse*);

private:
    e_State m_State;
    QString m_Name;
    QString m_Length;
    QString m_Climb;
    QStringList m_Controls;
    std::map<QString, e_State> m_States;
};

#endif // CIOFRESULTXMLHANDLER_H
