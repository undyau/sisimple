#ifndef CREMEDIATOR_H
#define CREMEDIATOR_H

#include <QObject>
#include <QStringList>

class CRemediator : public QObject
{
    Q_OBJECT
public:
    explicit CRemediator(QStringList& a_RawData, QObject *parent = 0);
    void CheckForBadFinishUnit();

signals:

public slots:

private:
    QStringList& m_OriginalResultsData;
    void FixupRawData(long a_ControlId, int a_Adjustment);
    QString FixupSingleResult(QString a_ResultData, long a_ControlId, int a_Adjustment, int a_Offset);
};

#endif // CREMEDIATOR_H
