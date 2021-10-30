#ifndef CSIDUMPER_H
#define CSIDUMPER_H

#include <QObject>
#include "sidumprecord.h"
#include "QStringList"
class QSerialPort;

enum StateType
{
    STATE_SETMSMODE,
    STATE_GETBUFPTR,
    STATE_READING_CARD_89,
    STATE_READING_CARD_1011,
    STATE_READING_CARD_SIAC1,
    STATE_READING_CARD_6,
    STATE_DUMPING
};

enum CardType
{
    CARD_SI5,
    CARD_SI6,
    CARD_SI89,
    CARD_SI10,
    CARD_SI11,
    CARD_SIAC1,
    CARD_UNKNOWN
};

class CSIDumper : public QObject
{
    Q_OBJECT
public:
    explicit CSIDumper(QObject *parent = 0);
    ~CSIDumper();
    void SetSerialPort(QString a_SerialPort);
    QStringList GetAllDataCsv();
    void Clear() {m_AllCards.clear();}

signals:
    void Finished(int a_Count, QString a_Summary);
    void CardCsv(QString a_CardCsv);
    void StatusUpdate(QString a_Msg);
    void ErrorOcurred(QString a_Msg);

public slots:
    void tryDownload();

private slots:
    void onReadyRead();
    void onDataTerminalReadyChanged(bool status);

private:
    StateType m_State;
    QSerialPort *m_Port;
    QString m_SerialPort;
    unsigned long m_Address;
    unsigned long m_EndAddress;
    int m_Increment;
    QByteArray m_ReadBuf;
    QByteArray m_CardData;
    int m_Timer;
    int m_SI5,m_SI6,m_SI8, m_SI9, m_SI10, m_SI11, m_SIAC1;
    std::vector<SIDumpRecord> m_AllCards;

    bool SendSmallCmd(unsigned char a_Cmd, unsigned char a_ParamData);
    bool SendCmd(unsigned char a_Cmd, QByteArray& a_ParamData);
    void BuildMsg(unsigned char a_Cmd, const QByteArray& a_ParamData, QByteArray& a_Msg);
    void DumpMessage(QString a_Prefix, QByteArray& a_Data);
    void DumpMessage2(QString a_Prefix, QByteArray& a_Data);
    bool IncompleteResponse(const QByteArray& a_Data);
    bool SetMSMode();
    void HandleSetMSModeResponse(QByteArray& a_Data);
    void HandleGetEndMemoryAddrResponse(QByteArray& a_Data);
    void HandleReadingCard89(QByteArray& a_Data);
    void HandleReadingCard1011(QByteArray& a_Data);
    void HandleReadingCardSiac1(QByteArray& a_Data);
    void HandleReadingCard6(QByteArray& a_Data);
    void HandleBadData(QByteArray& a_Data);
    void HandleDumping(QByteArray& a_Data);

    bool GetBackupData();
    bool DumpData();
    bool GetEndMemoryAddr();
    void ClosePort();
    bool OpenPort();
    void CreatePort();
    void GetNextBlock();
    bool CompleteMessage(QByteArray& a_Msg, bool& a_Error);
    CardType GuessCardType(QByteArray& a_Rec);
    void RestartWith38400();
    void timerEvent(QTimerEvent *event);

    bool ProcessResp(QByteArray& a_Rec);
    void ProcessSICard5(QByteArray& a_Rec);
    void ProcessSICard6(QByteArray& a_Rec);
    void ProcessSICard8Or9(QByteArray& a_Rec);
    void ProcessSICard10Or11(QByteArray& a_Rec);
    void ProcessSICardSIAC1(QByteArray& a_Rec);
    void TrimDataResp(QByteArray& a_Resp);
    void Read4ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_Cn, QString& a_DOW, QString& a_When);
    void Read2ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_When);
    void Read3ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_Cn, QString& a_When);
    QString LookupDay(int a_Day);

    void AddNewCard(SIDumpRecord& a_Card);
};

#endif // CSIDUMPER_H
