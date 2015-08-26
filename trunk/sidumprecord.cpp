#include "sidumprecord.h"

SIDumpRecord::SIDumpRecord() :
     m_No(0)
{
}

void SIDumpRecord::setControl(QString a_CN, QString a_DOW, QString a_Time)
{
    m_CNArray.append(a_CN);
    m_TimeArray.append(a_Time);
    m_DOWArray.append(a_DOW);
}

QString SIDumpRecord::NumPunches()
{
    QString result = QString("%1").arg(m_CNArray.count());
    return result;
}

QString SIDumpRecord::getAsCsv()
{
QString result;

//read at,SI-Card,St no,cat.,First name,name,club,country,sex,year-op,Email,mobile,city,street,zip,CLR_CN,CLR_DOW,clear time,CHK_CN,CHK_DOW,check time,ST_CN,ST_DOW,start time,FI_CN,FI_DOW,Finish time,No. of punches,1.CN,1.DOW,1.Time,2.CN,2.DOW,2.Time,3.CN,3.DOW,3.Time,4.CN,4.DOW,4.Time,5.CN,5.DOW,5.Time,6.CN,6.DOW,6.Time,7.CN,7.DOW,7.Time,8.CN,8.DOW,8.Time,9.CN,9.DOW,9.Time,10.CN,10.DOW,10.Time,11.CN,11.DOW,11.Time,12.CN,12.DOW,12.Time,13.CN,13.DOW,13.Time,14.CN,14.DOW,14.Time,15.CN,15.DOW,15.Time,
result = QString("%1,").arg(m_No); // no.
result += "00:00:00,";          // read time
result += m_SICard + ',';
result += ',';                  // St no
result += ',';                  // cat
result += m_FirstName + ',';
result += m_Name + ',';
result += m_Club + ',';
result += ',';                   //country
result += ',';                   //sex
result += ',';                   //year-op
result += ',';                   //email
result += ',';                   //mobile
result += ',';                   //city
result += ',';                   //street
result += ',';                   //zip
result += m_ClearCN + ',';
result += m_ClearDOW + ',';
result += m_ClearTime + ',';
result += m_CheckCN + ',';
result += m_CheckDOW + ',';
result += m_CheckTime + ',';
result += m_StartCN + ',';
result += m_StartDOW + ',';
result += m_StartTime + ',';
result += m_FinishCN + ',';
result += m_FinishDOW + ',';
result += m_FinishTime + ',';


result += NumPunches() + ',';

if (m_CNArray.count() > 0)
    {
    for (int i = 0; i < m_CNArray.count(); i++)
        {
        result += m_CNArray[i] + ',';
        result += m_DOWArray[i] + ',';
        result += m_TimeArray[i] + ',';
        }
    }

return result;
}
