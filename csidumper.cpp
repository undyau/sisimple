#include "csidumper.h"
#include <QSerialPort>
#include <QMessageBox>
#include <QtDebug>
#include "sidumprecord.h"
#include "Utils.h"
#include <QTimerEvent>

#define STX 2
#define ETX 3
#define ACK 6
#define NAK 0x15
#define DLE 0x10

#define TIMEOUT 1000

#define SICmdSetMSMode 0xF0
#define SICmdAltSetMSMode 0x70
#define SICmdGetSystemValue 0x83
#define SICmdGetBackupDataValue 0x81
#define SIParamDirectComm 0x4D
#define SIParamProtocolConf 0x74


CSIDumper::CSIDumper(QObject *parent) :
     QObject(parent), m_Port(NULL)
{
}

CSIDumper::~CSIDumper()
{
    ClosePort();
}

void CSIDumper::SetSerialPort(QString a_SerialPort)
{
    ClosePort();
    m_SerialPort = a_SerialPort;
}

bool CSIDumper::IncompleteResponse(const QByteArray& a_Data)
{
if (a_Data.length() == 0)
    return true;
if (a_Data[a_Data.length() -1] == NAK || a_Data[a_Data.length() -1] == ETX)
    return false;
return true;
}

void CSIDumper::CreatePort()
{
    m_Port = new QSerialPort(m_SerialPort);
    m_Port->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    m_Port->setParity(QSerialPort::NoParity);
    m_Port->setDataBits(QSerialPort::Data8);
    m_Port->setStopBits(QSerialPort::StopBits::OneStop);
}

bool CSIDumper::OpenPort()
{
    if (m_Port->open(QIODevice::ReadWrite) == true) {
        connect(m_Port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(m_Port, SIGNAL(dataTerminalReadyChanged(bool)), this, SLOT(onDataTerminalReadyChanged(bool)));
        return true;
    }
    else {
        QString msg = QString(tr("Device failed to open: %1")).arg(m_Port->errorString());
        emit ErrorOcurred(msg);
        return false;
    }
}

void CSIDumper::tryDownload()
{
    m_SI5 = m_SI6 = m_SI8 = m_SI9 = m_SI10 = m_SI11 = m_SIAC1 = 0;
    if (m_SerialPort.isEmpty() || m_Port)
        return;
    CreatePort();
    m_Port->setBaudRate(QSerialPort::Baud4800);

    if (!OpenPort())
        return;

// If things didn't work, retry at another baud rate
    if (!SetMSMode())
        {
        ClosePort();
        return;
        }
}

void CSIDumper::RestartWith38400()
{
    CreatePort();
    m_Port->setBaudRate(QSerialPort::Baud38400);
    if (!OpenPort())
        return;

    if (!SetMSMode())
        {
        ClosePort();
        return;
        }
}

bool CSIDumper::CompleteMessage(QByteArray &a_Msg, bool& a_Error)
{
if (a_Msg.isEmpty())
    return false;
if ((unsigned char)a_Msg[0] == NAK ||
    (unsigned char)a_Msg[0] != STX)
    {
    a_Error = true;
    return true;
    }

if (a_Msg.length() < 4)
    return false;
if ((unsigned char)a_Msg[1] >= 0x80)    // new command
    {
    int len = (unsigned char) a_Msg[2];
    if (a_Msg.length() == len + 6)
        {
        if ((unsigned char)a_Msg[a_Msg.length() -1] == ETX)
            {
            unsigned int readcrc = 256 * (unsigned char)a_Msg[a_Msg.length()-3];
            readcrc += (unsigned char)a_Msg[a_Msg.length()-2];
            unsigned char* data = new unsigned char[a_Msg.size()-4];
            for (int i = 1; i < a_Msg.length()-3; i++)
                data[i-1] = a_Msg[i];
            unsigned int mycrc = crc(a_Msg.size()-4, data);
            if (mycrc == readcrc)
                a_Error = false;
            return !a_Error;
            }
        return false;
        }
    else if (a_Msg.length() < len + 6)
        a_Error = false;  // not enough data yet
    return false;
    }
else
    return false;  // TODO - correct this
}

void CSIDumper::onReadyRead()
{
    killTimer(m_Timer);
    QByteArray bytes;
    int a = m_Port->bytesAvailable();
    bytes.resize(a);
    m_Port->read(bytes.data(), bytes.size());

    m_ReadBuf.append(bytes);
    bool error(false);
    if (CompleteMessage(m_ReadBuf, error))
    {
    //if (m_State != STATE_SETMSMODE && m_State != STATE_GETBUFPTR)
    //DumpMessage("in onReadyRead", m_ReadBuf);
    switch (m_State)
        {
        case STATE_SETMSMODE: HandleSetMSModeResponse(m_ReadBuf); return;
        case STATE_GETBUFPTR: HandleGetEndMemoryAddrResponse(m_ReadBuf); return;
        case STATE_DUMPING: HandleDumping(m_ReadBuf); return;
        case STATE_READING_CARD_89: HandleReadingCard89(m_ReadBuf); return;
        case STATE_READING_CARD_1011: HandleReadingCard1011(m_ReadBuf); return;
        case STATE_READING_CARD_6: HandleReadingCard6(m_ReadBuf); return;
        case STATE_READING_CARD_SIAC1: HandleReadingCardSiac1(m_ReadBuf); return;
        }
    }
    else
    {
        if (error)
            HandleBadData(m_ReadBuf);
        else
        {}// wait
            /*qDebug() << "inbound message incomplete, waiting for more";*/
    }
}

void CSIDumper::onDataTerminalReadyChanged(bool status)
{
    if (!status)
        {
        emit ErrorOcurred(tr("Device was turned off"));
        ClosePort();
        }
}

void CSIDumper::ClosePort()
{
    qDebug() << "Closing port" << m_SerialPort;
    if (m_Port)
        {
        m_Port->close();
        m_Port = NULL;
        }
    m_State = STATE_SETMSMODE;
}

void CSIDumper::DumpMessage2(QString a_Prefix, QByteArray& a_Data)
{
    char lookup[] = {"0123456789ABCDEF"};

    QString temp = "\nMessage Dump v.2 (" + a_Prefix + "):\n";
    for (int i = 0; i < a_Data.length() && i < 5000; i+=4)
        {
        temp += "0x";
        temp += QString((char)lookup[((i/4)/16)]);
        temp += QString((char)lookup[((i/4) & 0xF)]);
        temp += ":  ";
        for (int j = 0; j < 4 && j+i < a_Data.length(); j++)
            {
            int val = (unsigned char) a_Data[i+j];
            temp += QString((char)lookup[(val/16)]);
            temp += QString((char)lookup[(val & 0xF)]);
            temp += "  ";
            }

        temp += "  ";
        for (int j = 0; j < 4 && j + i < a_Data.length(); j++)
            {
            int val = (unsigned char) a_Data[i+j];
            if (val >=32 && val <= 127)
                temp += val;
            else
                temp += ".";
            }
        qDebug() << temp;
        temp.clear();
        }
}


void CSIDumper::DumpMessage(QString a_Prefix, QByteArray& a_Data)
{
    char lookup[] = {"0123456789ABCDEF"};

    QString temp = "\nMessage Dump (" + a_Prefix + "):\n";
    for (int i = 0; i < a_Data.length() && i < 5000; i++)
        {
        int val = (unsigned char) a_Data[i];
        switch (val)
            {
            case 2: temp += "STX "; break;
            case 3: temp += "ETX "; break;
            case 6: temp += "ACK "; break;
            case 16: temp += "DLE "; break;
            case 21: temp += "NAK "; break;
            default:
                    temp += QString((char)lookup[(val/16)]);
                    temp += QString((char)lookup[(val & 0xF)]);
                    temp += "  ";
            }
        }
    qDebug() << temp;
    qDebug() << "\nASCII Dump\n";
    temp.clear();
    for (int i = 0; i < a_Data.length() && i < 5000; i++)
        {
        int val = (unsigned char) a_Data[i];
        if (val >=32 && val <= 127)
            temp += val;
        else
            temp += ".";
        }
    qDebug() << temp;
}

bool CSIDumper::SetMSMode()
{
    emit StatusUpdate(tr("Sending SICmdSetMSMode"));
    m_State = STATE_SETMSMODE;
    if (!SendSmallCmd(SICmdSetMSMode, SIParamDirectComm))
        {
        ClosePort();
        return false;
        }

    return true;
}

void CSIDumper::HandleSetMSModeResponse(QByteArray& a_Data)
{
    if (a_Data.length() > 0 && a_Data[0] == (char)NAK)
        {
        if (SendSmallCmd(SICmdAltSetMSMode, SIParamDirectComm))
            return;
        ClosePort();
        return;
        }

    if (GetEndMemoryAddr())
        return;

    ClosePort();
    return;
}

bool CSIDumper::GetEndMemoryAddr()
{
    emit StatusUpdate(tr("Sending SICmdGetSystemValue to get mem pointer"));
    m_State = STATE_GETBUFPTR;
    QByteArray params;
    params.append((char)0x1C);  // from SI manual
    params.append(7);
    if (!SendCmd(SICmdGetSystemValue, params))
        {
        ClosePort();
        return false;
        }

    return true;
}

void CSIDumper::HandleGetEndMemoryAddrResponse(QByteArray& a_Data)
{
    m_EndAddress = (unsigned char)a_Data[6] * 256;
    m_EndAddress += (unsigned char)a_Data[7];
    m_EndAddress *= 256;
    m_EndAddress += (unsigned char)a_Data[11];
    m_EndAddress *= 256;
    m_EndAddress += (unsigned char)a_Data[12];

    if (m_EndAddress > 0x200000)
        m_Address = m_Address - 0x200000;
    else
        m_Address = 0x100;

    m_Increment = 0x80;
    if (DumpData())
        return;

    ClosePort();
    return;
}

void CSIDumper::HandleBadData(QByteArray& a_Data)
{
// Dump the bad data and keep reading if in a dumping state, else quit
    if (m_State == STATE_DUMPING ||
        m_State == STATE_READING_CARD_89 ||
        m_State == STATE_READING_CARD_1011 ||
        m_State == STATE_READING_CARD_6)
        {
        DumpMessage("Bad data encountered - ignored", a_Data);
        GetNextBlock();
        }
    else
        {
        DumpMessage("Bad data encountered - terminating", a_Data);
        ClosePort();
        return;
        }
}

bool CSIDumper::GetBackupData()
{
    QByteArray params;
    long a1,a2,a3;
    a1 = (m_Address & 0xFF0000)/(256*256);
    a2 = (m_Address & 0xFF00)/256;
    a3 = m_Address & 0xFF;

    params.append((unsigned char) a1);
    params.append((unsigned char) a2);
    params.append((unsigned char) a3);
    params.append((unsigned char) m_Increment);

//    qDebug() << "Attempting to read from" << m_Address << "for" << m_Increment << "bytes";
    if (!SendCmd(SICmdGetBackupDataValue, params))
        return false;

    return true;
}

bool CSIDumper::DumpData()
{
    emit StatusUpdate(tr("Starting dump of backup memory"));
    m_State = STATE_DUMPING;
    return GetBackupData();
}

void CSIDumper::GetNextBlock()
{
    m_Address += m_Increment;

    if (m_Address >= m_EndAddress)
        {
        ClosePort();
        QString summary = QString(tr("Cards: %1 SI-5, %2 SI-6, %3 SI-8, %4 SI-9, %5 SI-10, %6 SI-11, %7 SIAC1"))
        .arg(m_SI5).arg(m_SI6).arg(m_SI8).arg(m_SI9).arg(m_SI10).arg(m_SI11).arg(m_SIAC1);
        emit StatusUpdate(summary);
        emit Finished(m_AllCards.size(), summary);
        qDebug() << "Read" << m_AllCards.size() << summary;
        return;
        }

    if (!DumpData())
        ClosePort();
}

void CSIDumper::HandleDumping(QByteArray& a_Data)
{
    if(ProcessResp(a_Data)) // response consumed, read on
        GetNextBlock();
}

bool CSIDumper::SendSmallCmd(unsigned char a_Cmd, unsigned char a_ParamData)
{
QByteArray data;
data.append(a_ParamData);
return SendCmd(a_Cmd, data);
}

void CSIDumper::TrimDataResp(QByteArray& a_Resp)
{
    a_Resp.remove(a_Resp.length()-3, 3);
    a_Resp.remove(0, 8);
}

CardType CSIDumper::GuessCardType(QByteArray& a_Rec)
{
    if ((unsigned char) a_Rec[30] == 0x00 && (unsigned char)a_Rec[31] == 0x07)
        return CARD_SI5;

    // Other cards need more data checking
    bool isSI891011(true), isSI6(true);
    for (int i = 4; i <= 7; i++)
        {
        //DumpMessage("Suspect SI card", a_Rec);
        if ((unsigned char)a_Rec[i] != 0xEA)
            isSI891011 = false;
        if ((unsigned char)a_Rec[i] != 0xED)
            isSI6 = false;
        }

    if (isSI891011)
        {
        // check the number range
        if ((unsigned char)a_Rec[24] == 0x0F)
            {
            long SINumber(0);
            for (int i = 25; i < 28; i++)
                {
                SINumber *= 256;
                SINumber += (unsigned char)a_Rec[i];
                }
            if (SINumber > 7000000 && SINumber < 8000000)
                return CARD_SI10;
            if (SINumber > 9000000 && SINumber < 10000000)
                return CARD_SI11;

            return CARD_SIAC1;
            }
        return CARD_SI89;
        }

    if (isSI6)
        return CARD_SI6;

    return CARD_UNKNOWN;
}

/*
Processing card data - return true if response forms a complete card download, otherwise false for more
*/
bool CSIDumper::ProcessResp(QByteArray& a_Rec)
{
    TrimDataResp(a_Rec);

    switch (GuessCardType(a_Rec))
    {
        case CARD_SI5:
            ProcessSICard5(a_Rec);
            return true;
        case CARD_SI6:
            ProcessSICard6(a_Rec);
            return false;
        case CARD_SI89:
            ProcessSICard8Or9(a_Rec);
            return false;
        case CARD_SI10:
        case CARD_SI11:
            ProcessSICard10Or11(a_Rec);
            return false;
        case CARD_SIAC1:
            ProcessSICardSIAC1(a_Rec);
            return false;
        case CARD_UNKNOWN:
        default:
            qDebug() << "Unknown card type encountered";
            DumpMessage("Unknown card type encountered", a_Rec);
            return true;
    }
}


void CSIDumper::ProcessSICard10Or11(QByteArray& a_Rec)
{
    // Need more data
    m_State = STATE_READING_CARD_1011;
    m_CardData = a_Rec;
    m_Increment = 0x80;
    m_Address += m_Increment;

    GetBackupData();
}

void CSIDumper::ProcessSICardSIAC1(QByteArray& a_Rec)
{
    // Need more data
    m_State = STATE_READING_CARD_SIAC1;
    m_CardData = a_Rec;
    m_Increment = 0x80;
    m_Address += m_Increment;

    GetBackupData();
}

void CSIDumper::HandleReadingCard1011(QByteArray& a_Rec)
    {
    TrimDataResp(a_Rec);
    SIDumpRecord rec;

    if (GuessCardType(a_Rec) != CARD_UNKNOWN)
        {
        // This block belongs to the next card
        m_Address -= m_Increment;
 //       qDebug() << "Ooops, read too much" << m_CardData.size() + a_Rec.size() << cType;
        }
    else
        {
        m_CardData.append(a_Rec);
 //       qDebug() << "m_CardData.size()" << m_CardData.size() << "a_Rec.size()" << a_Rec.size();
        if (m_CardData.length () < 0x80 * 8)
            {
            // Get next load of data !
            m_State = STATE_READING_CARD_1011;
            m_Increment = 0x80;
            m_Address += m_Increment;
            GetBackupData();
            return;
            }
        }
//DumpMessage2("Got data to process inside HandleReadingCard1011", m_CardData);
    long SINumber(0);
    for (int i = 25; i < 28; i++)
        {
        SINumber *= 256;
        SINumber += (unsigned char)m_CardData[i];
//        qDebug() <<"Found SI10/11 card" << SINumber;
        }
    rec.setSICard(QString::number(SINumber));
    bool isSI10 = SINumber > 7000000 && SINumber < 8000000;
    bool isSI11 = SINumber > 9000000 && SINumber < 10000000;

    QStringList names = QString(m_CardData.mid(32,24)).split(";");
    rec.setFirstName(names.at(0));
    rec.setOtherName(names.at(1));


    QString cn, when, dow;
    Read4ByteControlData(m_CardData, 8, cn, dow, when);
    rec.setCheck(cn, dow, when);
    rec.setClear(cn, dow, when);
    Read4ByteControlData(m_CardData, 12, cn, dow, when);
    rec.setStart(cn, dow, when);
    Read4ByteControlData(m_CardData, 16, cn, dow, when);
    rec.setFinish(cn, dow, when);

    int offset = 128;
    int max_punches = 128;


    for (int i = 0; i < max_punches && offset < m_CardData.size ()-4; i++)
        {
        if ((unsigned char)m_CardData[offset] != 0xEE ||
            (unsigned char)m_CardData[offset + 1] != 0xEE ||
            (unsigned char)m_CardData[offset + 2] != 0xEE ||
            (unsigned char)m_CardData[offset + 3] != 0xEE)
            {
            Read4ByteControlData(m_CardData, offset, cn, dow, when);
            rec.setControl(cn, dow, when);
            }
        else
            break;
        offset += 4;
        }

    AddNewCard(rec);
    if (isSI10)
        m_SI10++;
    else if (isSI11)
        m_SI11++;

    GetNextBlock();
}

void CSIDumper::HandleReadingCardSiac1(QByteArray& a_Rec)
    {
    TrimDataResp(a_Rec);
    SIDumpRecord rec;

    if (GuessCardType(a_Rec) != CARD_UNKNOWN)
        {
        // This block belongs to the next card
        m_Address -= m_Increment;
 //       qDebug() << "Ooops, read too much" << m_CardData.size() + a_Rec.size() << cType;
        }
    else
        {
        m_CardData.append(a_Rec);
 //       qDebug() << "m_CardData.size()" << m_CardData.size() << "a_Rec.size()" << a_Rec.size();
        if (m_CardData.length () < 0x80 * 8)
            {
            // Get next load of data !
            m_State = STATE_READING_CARD_SIAC1;
            m_Increment = 0x80;
            m_Address += m_Increment;
            GetBackupData();
            return;
            }
        }
//DumpMessage2("Got data to process inside HandleReadingCardSIAC1", m_CardData);
    long SINumber(0);
    for (int i = 25; i < 28; i++)
        {
        SINumber *= 256;
        SINumber += (unsigned char)m_CardData[i];
        qDebug() <<"Found SIAC card" << SINumber;
        }
    rec.setSICard(QString::number(SINumber));

    QStringList names = QString(m_CardData.mid(32,24)).split(";");
    rec.setFirstName(names.at(0));
    rec.setOtherName(names.at(1));


    QString cn, when, dow;
    Read4ByteControlData(m_CardData, 8, cn, dow, when);
    rec.setCheck(cn, dow, when);
    rec.setClear(cn, dow, when);
    Read4ByteControlData(m_CardData, 12, cn, dow, when);
    rec.setStart(cn, dow, when);
    Read4ByteControlData(m_CardData, 16, cn, dow, when);
    rec.setFinish(cn, dow, when);

    int offset = 128;
    int max_punches = 128;


    for (int i = 0; i < max_punches && offset < m_CardData.size ()-4; i++)
        {
        if ((unsigned char)m_CardData[offset] != 0xEE ||
            (unsigned char)m_CardData[offset + 1] != 0xEE ||
            (unsigned char)m_CardData[offset + 2] != 0xEE ||
            (unsigned char)m_CardData[offset + 3] != 0xEE)
            {
            Read4ByteControlData(m_CardData, offset, cn, dow, when);
            rec.setControl(cn, dow, when);
            }
        else
            break;
        offset += 4;
        }

    AddNewCard(rec);
    m_SIAC1++;

    GetNextBlock();
}



void CSIDumper::ProcessSICard8Or9(QByteArray& a_Rec)
{
    // Need more data
    m_State = STATE_READING_CARD_89;
    m_CardData = a_Rec;
    m_Increment = 0x80;
    m_Address += m_Increment;

    GetBackupData();
}

void CSIDumper::HandleReadingCard89(QByteArray& a_Rec)
    {
    TrimDataResp(a_Rec);
    SIDumpRecord rec;

    if (GuessCardType(a_Rec) != CARD_UNKNOWN)
        {
        // This block belongs to the next card
        m_Address -= m_Increment;
        }
    else
        m_CardData.append(a_Rec);

    long SINumber(0);
    for (int i = 25; i < 28; i++)
        {
        SINumber *= 256;
        SINumber += (unsigned char)m_CardData[i];
        }
    rec.setSICard(QString::number(SINumber));


    //DumpMessage2("Processing SI Card 8/9 " + QString::number(SINumber), m_CardData);
    bool isSI9 = SINumber < 2000000;

    QStringList names = QString(m_CardData.mid(32,24)).split(";");
    rec.setFirstName(names.at(0));
    rec.setOtherName(names.at(1));

    QString cn, when, dow;
    Read4ByteControlData(m_CardData, 8, cn, dow, when);
    rec.setCheck(cn, dow, when);
    rec.setClear(cn, dow, when);
    Read4ByteControlData(m_CardData, 12, cn, dow, when);
    rec.setStart(cn, dow, when);
    Read4ByteControlData(m_CardData, 16, cn, dow, when);
    rec.setFinish(cn, dow, when);

    int offset = isSI9 ? 56 : 136;
    int max_punches = isSI9 ? 50 : 30;

    for (int i = 0; i < max_punches && offset < m_CardData.size(); i++)
        {
        if ((unsigned char)m_CardData[offset] != 0xEE ||
            (unsigned char)m_CardData[offset + 1] != 0xEE ||
            (unsigned char)m_CardData[offset + 2] != 0xEE ||
            (unsigned char)m_CardData[offset + 3] != 0xEE)
            {
            Read4ByteControlData(m_CardData, offset, cn, dow, when);
            rec.setControl(cn, dow, when);
            }
        else
            break;
        offset += 4;
        }

    AddNewCard(rec);
    if (isSI9)
        m_SI9++;
    else
        m_SI8++;

    GetNextBlock();
}


void CSIDumper::HandleReadingCard6(QByteArray& a_Data)
    {
    TrimDataResp(a_Data);
    SIDumpRecord rec;
    bool hitEnd(false);

    if (GuessCardType(a_Data) != CARD_UNKNOWN)
        {
        // This block belongs to the next card
        m_Address -= m_Increment;
        hitEnd = true;
        }
    else
        m_CardData.append(a_Data);

    long sinumber(0);
    for (int i = 11; i < 14; i++)
        {
        sinumber *= 256;
        sinumber += (unsigned char)m_CardData[i];
        }

    if (!hitEnd &&
        (m_CardData.size() < 3*128 ||
        (sinumber >= 0xFF0000 && m_CardData.size() < 8*128)))  // Its a 6-star card !
        {
        m_Increment = 0x80;
        m_Address += m_Increment;
        GetBackupData();
        return;
        }
//    DumpMessage2("Processing SI Card 6 " + QString::number(sinumber), m_CardData);

    rec.setSICard(QString::number(sinumber));
    rec.setFirstName(m_CardData.mid(68,20).trimmed());
    rec.setOtherName(m_CardData.mid(48,20).trimmed());

    QString cn, when, dow;
    Read4ByteControlData(m_CardData, 28, cn, dow, when);
    rec.setCheck(cn, dow, when);
    Read4ByteControlData(m_CardData, 32, cn, dow, when);
    rec.setClear(cn, dow, when);
    Read4ByteControlData(m_CardData, 24, cn, dow, when);
    rec.setStart(cn, dow, when);
    Read4ByteControlData(m_CardData, 20, cn, dow, when);
    rec.setFinish(cn, dow, when);
    rec.setClub(m_CardData.mid(96,32).trimmed());

   // if (!rec.getBadRead())
   //     {
        int offset;
        if (sinumber >= 0xFF0000)
            offset =  7 *  // block number
                      16 * // page size
                      8;  // number of pages
        else
            offset = 1 * 16 * 8;

        for (int i = 0; i < 64; i++)  // only support up to 64 punches at the moment
            {
            if ((unsigned char)m_CardData[offset] != 0xEE ||
                (unsigned char)m_CardData[offset + 1] != 0xEE ||
                (unsigned char)m_CardData[offset + 2] != 0xEE ||
                (unsigned char)m_CardData[offset + 3] != 0xEE)
                {
                Read4ByteControlData(m_CardData, offset, cn, dow, when);
                rec.setControl(cn, dow, when);
                }
            else
                break;
            offset += 4;
            }
      //  }

    AddNewCard(rec);
    m_SI6++;
    GetNextBlock();
}

void CSIDumper::ProcessSICard6(QByteArray& a_Rec)
{
    // Need more data
    m_State = STATE_READING_CARD_6;
    m_CardData = a_Rec;
    m_Increment = 0x80;
    m_Address += m_Increment;
    GetBackupData();
}

void CSIDumper::ProcessSICard5(QByteArray& a_Rec)
{
    SIDumpRecord rec;

    long temp = (unsigned char)a_Rec[4] * 256 + (unsigned char)a_Rec[5];
    switch((int)a_Rec[6])
        {
        case 0:
        case 1:
            break;
        case 2:
        case 3:
        case 4:
            temp += (100000 * (int)a_Rec[6]);
            break;
        }
    rec.setSICard(QString::number(temp));
    QString cn, when;
    Read2ByteControlData(a_Rec, 25, when);
    rec.setCheck("", "", when);
    Read2ByteControlData(a_Rec, 19, when);
    rec.setStart("", "", when);
    Read2ByteControlData(a_Rec, 21, when);
    rec.setFinish("", "", when);

    #define START_PUNCH_DATA 32
    int offset(0);
    for (int i = 0; i < 6; i++)
        {
        offset += 1;  // skip emergency punch at start of block
        for (int j = 0; j < 5; j++)
            {
            if (a_Rec[offset + START_PUNCH_DATA] != (char)0)
                {
                Read3ByteControlData(a_Rec, START_PUNCH_DATA + offset, cn, when);
                rec.setControl(cn, "", when);
                }
            offset += 3;
            }
        }
    #undef START_PUNCH_DATA
    AddNewCard(rec);
    m_SI5++;
}




bool CSIDumper::SendCmd(unsigned char a_Cmd, QByteArray& a_ParamData)
{
    QByteArray msg;
    QByteArray data;
    for (int i = 0; i < a_ParamData.length(); ++i)
        data.append((unsigned char)a_ParamData[i]);
    BuildMsg(a_Cmd, data, msg);

    qint64 retval(0);

    m_ReadBuf.clear();

    if ( (retval = m_Port->write(msg)) == -1)
        {
        QMessageBox msgbox;
        QString txt = QString("Can't write %1 bytes to %2 - only wrote %3 bytes").arg(msg.length()).arg(m_SerialPort).arg(retval);
        msgbox.setInformativeText(txt);
        msgbox.exec();
        return false;
        }
    m_Timer = startTimer(TIMEOUT);
    return true;
}

void CSIDumper::BuildMsg(unsigned char a_Cmd, const QByteArray& a_ParamData, QByteArray& a_Msg)
{
a_Msg.append(a_Cmd);
if (a_Cmd >= 0x80)
    a_Msg.append(a_ParamData.size());
for (int i = 0;i < a_ParamData.size(); i++)
    a_Msg.append(a_ParamData[i]);


if (a_Cmd >= 0x80)
    {
    unsigned char* data = new unsigned char[a_Msg.size()];
    for (int i = 0;i < a_Msg.size(); i++)
        data[i] = a_Msg[i];

    unsigned int mycrc = crc(a_Msg.size(), data);

    delete [] data;
    a_Msg.append(mycrc/256);
    a_Msg.append(mycrc & 0x00FF);
    }
a_Msg.append(ETX);
a_Msg.insert(0,STX);
a_Msg.insert(0,0xFF);

}

void CSIDumper::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    bool running4800 = (m_Port->baudRate() == QSerialPort::Baud4800);
    ClosePort();

    if (running4800)
        RestartWith38400();
}


void CSIDumper::Read4ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_Cn, QString& a_DOW, QString& a_When)
{
// record structure: PTD - CN - PTH - PTL

    unsigned char byte[4];
    for (int i = 0; i < 4; i++)
        byte[i] = a_Rec[a_Offset + i];

    unsigned int cn = ((byte[0] & 0xC0) >> 6) << 8;
    cn += byte[1];
    a_Cn = QString("%1").arg(cn);

    // Calculate DOW
    int dow = (byte[0] & 0x0E) >> 1;
    a_DOW = LookupDay(dow);

    // Calculate time - tricky ?
    int time12h = byte[3] + (256*byte[2]);
    if (byte[0] & 0x01)
        time12h += (12 * 60 *60);

    int hr, min, sec;
    hr = time12h / (60*60);
    min = (time12h - (hr * 60 * 60))/60;
    sec = time12h - (hr * 60 * 60) - (min * 60);

    a_When = QString("%1:%2:%3").arg(hr, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));

}

void CSIDumper::Read3ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_Cn, QString& a_When)
{
// record structure: CN - PTH - PTL

    a_Cn = QString("%1").arg((unsigned char)a_Rec[a_Offset]);
    Read2ByteControlData(a_Rec, a_Offset + 1, a_When);
}

void CSIDumper::Read2ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_When)
{
// record structure: PTH - PTL
   unsigned char byte[2];
    for (int i = 0; i < 2; i++)
        byte[i] = a_Rec[a_Offset + i];

    int time12h = byte[1] + (256*byte[0]);

    int hr, min, sec;
    hr = time12h / (60*60);
    min = (time12h - (hr * 60 * 60))/60;
    sec = time12h - (hr * 60 * 60) - (min * 60);

    a_When = QString("%1:%2:%3").arg(hr, 2, 10, QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
}

void CSIDumper::AddNewCard(SIDumpRecord& a_Card)
{
    static SIDumpRecord lastCard;
    if (m_AllCards.size() > 0 &&
        a_Card.getSICard() == lastCard.getSICard() &&
        lastCard.getPunchCount() < a_Card.getPunchCount())
        {
        m_AllCards.pop_back();
        }
    a_Card.setNo(m_AllCards.size() + 1);
    m_AllCards.push_back(a_Card);
    emit CardCsv(a_Card.getAsCsv());
    QString msg = QString("Reading card %1").arg(a_Card.getSICard());
    emit StatusUpdate(msg);
    lastCard = a_Card;
}

QString CSIDumper::LookupDay(int a_Day)
{
    switch (a_Day)
    {
    case 0: return "Su";
    case 1: return "Mo";
    case 2: return "Tu";
    case 3: return "We";
    case 4: return "Th";
    case 5: return "Fr";
    case 6: return "Sa";
    default: return "";
    }
}

QStringList CSIDumper::GetAllDataCsv()
{
    QStringList result;
    for (unsigned int i = 0; i < m_AllCards.size(); i++)
        result += m_AllCards.at(i).getAsCsv();
    return result;
}
