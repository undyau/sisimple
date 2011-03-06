#ifndef SIDUMPRECORD_H
#define SIDUMPRECORD_H

#include <vector>
#include <QStringList>

class SIDumpRecord
{
public:
    SIDumpRecord();

    void setSICard(QString a_SICard) {m_SICard = a_SICard;}
    void setStNo(QString a_StNo) {m_StNo = a_StNo;}
    void setCat(QString a_Cat) {m_Cat = a_Cat;}
    void setFirstName(QString a_FirstName) {m_FirstName = a_FirstName;}
    void setOtherName(QString a_Name) {m_Name = a_Name;}
    void setClub(QString a_Club) {m_Club = a_Club;}
    void setNo(int a_No) {m_No = a_No;}
    void SetSerialPort(QString a_SerialPort);

    void setClear(QString a_ClearCN, QString a_DOW, QString a_ClearTime) {m_ClearCN = a_ClearCN;m_ClearTime = a_ClearTime; m_ClearDOW = a_DOW;}
    void setCheck(QString a_CheckCN, QString a_DOW, QString a_CheckTime) {m_CheckCN = a_CheckCN;m_CheckTime = a_CheckTime; m_CheckDOW = a_DOW;}
    void setStart(QString a_StartCN, QString a_DOW, QString a_StartTime) {m_StartCN = a_StartCN;m_StartTime = a_StartTime; m_StartDOW = a_DOW;}
    void setFinish(QString a_FinishCN, QString a_DOW, QString a_FinishTime) {m_FinishCN = a_FinishCN;m_FinishTime = a_FinishTime; m_FinishDOW = a_DOW;}

    void setControl(int a_Index, QString a_CN, QString a_DOW, QString a_Time);
    void setBadRead(bool a_BadRead) {m_BadRead = a_BadRead;}

    bool getBadRead() {return m_BadRead;}
    QString getAsCsv();

signals:

public slots:

private:
    int m_No;
    QString m_ReadAt;
    QString m_SICard;
    QString m_StNo;
    QString m_Cat;
    QString m_FirstName;
    QString m_Name;
    QString m_Club;
    QString m_Country;
    QString m_Sex;
    QString m_YearOp;
    QString m_Email;
    QString m_Mobile;
    QString m_City;
    QString m_Street;
    QString m_Zip;
    QString m_ClearCN;
    QString m_ClearTime;
    QString m_ClearDOW;
    QString m_CheckCN;
    QString m_CheckTime;
    QString m_CheckDOW;
    QString m_StartCN;
    QString m_StartTime;
    QString m_StartDOW;
    QString m_FinishCN;
    QString m_FinishTime;
    QString m_FinishDOW;
    QString NumPunches();

    QStringList m_CNArray;
    QStringList m_TimeArray;
    QStringList m_DOWArray;
    bool m_BadRead;

};

#endif // SIDUMPRECORD_H
