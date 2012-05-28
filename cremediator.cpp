#include "cremediator.h"
#include <map>
#include "Utils.h"
#include <QInputDialog>

CRemediator::CRemediator(QStringList& a_RawData, QObject *parent) :
    QObject(parent), m_OriginalResultsData(a_RawData)
{
}

void CRemediator::CheckForBadFinishUnit()
{
    std::map<long, long> candidates;
    long lastControl = -1;

    for (int i = 0; i < m_OriginalResultsData.size(); i++)
        {
        QString str = m_OriginalResultsData.at(i);
        QStringList array = str.split(',');
        if (array[26].isEmpty() && array[27].isEmpty()) // No Finish !!
            if (ToLong(array[28]) > 0)
                {
                lastControl = ToLong(array[29 + (3*(ToLong(array[28]) -1))]);
                if (candidates.find(lastControl) == candidates.end())
                    candidates[lastControl] = 1;
                else
                    candidates[lastControl] = 1 + candidates[lastControl];
                }
        }
    if (lastControl == -1) // Nothing to do
        return;

    for (std::map<long, long>::iterator x = candidates.begin(); x != candidates.end(); x++)
        if (x->second > candidates[lastControl])
            lastControl = x->first;

    if (candidates.find(lastControl) != candidates.end() && candidates[lastControl] > 3)
        {
        // DANGER - looks as though someone didn't program a finish unit !!
        QString msg = QString(tr("It looks as though unit %1 was used as a finish unit, but not programmed.\nThis affects %2 runners.\n\nWas unit %1 used as a finish unit ?"))
        .arg(lastControl).arg(candidates[lastControl]);
        int result = SIMessageBox(msg, QMessageBox::Critical, QMessageBox::Yes|QMessageBox::No);
        if (result == QMessageBox::Yes)
            {
            bool ok;
            int offset = QInputDialog::getInt(NULL, tr("Unprogrammed finish control adjustment"),
                              tr("If the clock time was wrong on this unit, enter the number of seconds to correct it by: "),
                               0, -7200, 7200, 1, &ok);
            if (ok)
                FixupRawData(lastControl, offset);
            }
        }
}

QString CRemediator::FixupSingleResult(QString a_ResultData, long a_ControlId, int a_Adjustment, int a_Offset)
{
    QStringList array = a_ResultData.split(',');
    QStringList ftime = array[a_Offset + 2].split(':');
    long ss = ToLong(ftime[2]) + (60 * ToLong(ftime[1])) + (3600 * ToLong(ftime[0])) + a_Adjustment;
    long hh = ss /3600;
    ss -= hh *3600;
    long mm = ss /60;
    ss -= mm *60;
    QString adjustedFTime = QString("%1:%2:%3").arg(hh,2,10,QChar('0')).arg(mm, 2,10,QChar('0')).arg(ss,2,10,QChar('0'));

    array[25] = a_ControlId;
    array[26] = array[a_Offset + 1];
    array[27] = adjustedFTime;
    array[28] = QString("%1").arg(ToLong(array[28])-1);
    array[a_Offset + 1].clear();
    array[a_Offset + 2].clear();
    return array.join(",");
}

void CRemediator::FixupRawData(long a_ControlId, int a_Adjustment)
{
    for (int i = 0; i < m_OriginalResultsData.size(); i++)
        {
        QString str = m_OriginalResultsData.at(i);
        QStringList array = str.split(',');
        if (array[26].isEmpty() && array[27].isEmpty()) // No Finish !!
            if (ToLong(array[28]) > 0)
                {
                if (ToLong(array[29 + (3*(ToLong(array[28]) -1))]) == a_ControlId)
                    {
                    m_OriginalResultsData[i] = FixupSingleResult(str, a_ControlId, a_Adjustment, 29 + (3*(ToLong(array[28]) -1)));
                    }
                }
        }
}
