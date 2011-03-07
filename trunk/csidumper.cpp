#include "csidumper.h"
#include "qextserialport.h"
#include <QMessageBox>
#include <Qtdebug>
#include "sidumprecord.h"
#include "utils.h"
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
    m_Port = new QextSerialPort(m_SerialPort, QextSerialPort::EventDriven);
    m_Port->setFlowControl(FLOW_OFF);
    m_Port->setParity(PAR_NONE);
    m_Port->setDataBits(DATA_8);
    m_Port->setStopBits(STOP_1);
}

bool CSIDumper::OpenPort()
{
    if (m_Port->open(QIODevice::ReadWrite) == true) {
        connect(m_Port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(m_Port, SIGNAL(dsrChanged(bool)), this, SLOT(onDsrChanged(bool)));
        return true;
    }
    else {
        QString msg = QString(tr("Device failed to open: %1")).arg(m_Port->errorString());
        emit StatusUpdate(msg);
        return false;
    }
}

void CSIDumper::tryDownload()
{
    m_SI5 = m_SI6 = m_SI89 = 0;
    if (m_SerialPort.isEmpty() || m_Port)
        return;
    CreatePort();
    m_Port->setBaudRate(BAUD4800);

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
    m_Port->setBaudRate(BAUD38400);
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
a_Error = true;
if (a_Msg.isEmpty())
    return false;
if ((unsigned char)a_Msg[0] == NAK)
    return true;
if ((unsigned char)a_Msg[0] != STX)
    return true;
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
    switch (m_State)
        {
        case STATE_SETMSMODE: HandleSetMSModeResponse(m_ReadBuf); return;
        case STATE_GETBUFPTR: HandleGetEndMemoryAddrResponse(m_ReadBuf); return;
        case STATE_DUMPING: HandleDumping(m_ReadBuf); return;
        case STATE_READING_CARD89: HandleReadingCard89(m_ReadBuf); return;
        case STATE_READING_CARD6: HandleReadingCard6(m_ReadBuf); return;
        }
    }
    else if (error)
        {
        HandleBadData(m_ReadBuf); return;
        }
}

void CSIDumper::onDsrChanged(bool status)
{
    if (!status)
        {
        emit StatusUpdate(tr("Device was turned off"));
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
                    temp += " ";
                    temp += QString((char)lookup[(val/16)]);
                    temp += QString((char)lookup[(val & 0xF)]);
                    temp += " ";
            }
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
        m_State == STATE_READING_CARD89 ||
        m_State == STATE_READING_CARD6)
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

    if (!SendCmd(SICmdGetBackupDataValue, params))
        return false;

    return true;
}

bool CSIDumper::DumpData()
{
    m_State = STATE_DUMPING;
    return GetBackupData();
}

void CSIDumper::GetNextBlock()
{
    m_Address += m_Increment;

    if (m_Address >= m_EndAddress)
        {
        ClosePort();
        QString summary = QString(tr("%1 SI 5 cards, %2 SI 6 cards, %3 SI 8/9 cards"))
        .arg(m_SI5).arg(m_SI6).arg(m_SI89);
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
    bool isSI89(true), isSI6(true);
    for (int i = 4; i <= 7; i++)
        {
        //DumpMessage("Suspect SI card", a_Rec);
        if ((unsigned char)a_Rec[i] != 0xEA)
            isSI89 = false;
        if ((unsigned char)a_Rec[i] != 0xED)
            isSI6 = false;
        }

    if (isSI89)
        return CARD_SI89;

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

        case CARD_UNKNOWN:
        default:
            DumpMessage("Unknown card type encountered", a_Rec);
            exit(1);
            return true;
    }

}



void CSIDumper::ProcessSICard8Or9(QByteArray& a_Rec)
{
    // Need more data
    m_State = STATE_READING_CARD89;
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
        rec.setBadRead(true);
        }
    else
        m_CardData.append(a_Rec);

    long temp(0);
    for (int i = 25; i < 28; i++)
        {
        temp *= 256;
        temp += (unsigned char)m_CardData[i];
        }
    rec.setSICard(QString::number(temp));

    QStringList names = QString(m_CardData.mid(32,24)).split(";");
    rec.setFirstName(names.at(0));
    rec.setOtherName(names.at(1));

    QString cn, when, dow;
    Read4ByteControlData(m_CardData, 8, cn, dow, when, false);
    rec.setCheck(cn, dow, when);
    Read4ByteControlData(m_CardData, 12, cn, dow, when, true);
    rec.setStart(cn, dow, when);
    Read4ByteControlData(m_CardData, 16, cn, dow, when, true);
    rec.setStart(cn, dow, when);

    qDebug() << "SI 89 card from" << m_CardData.size() << "bytes";
    emit StatusUpdate("SI 8 or 9 card found in documented format - freaky");
    AddNewCard(rec);
    m_SI89++;

    GetNextBlock();
}


void CSIDumper::HandleReadingCard6(QByteArray& a_Rec)
    {
    TrimDataResp(a_Rec);
    SIDumpRecord rec;

    if (GuessCardType(a_Rec) != CARD_UNKNOWN)
        {
        // This block belongs to the next card
        m_Address -= m_Increment;
        rec.setBadRead(true);
        }
    else
        m_CardData.append(a_Rec);

    long sinumber(0);
    for (int i = 11; i < 14; i++)
        {
        sinumber *= 256;
        sinumber += (unsigned char)m_CardData[i];
        }

    if (!rec.getBadRead())
        {
        if (m_CardData.size() < 3*128 ||
           (sinumber >= 0xFF0000 && m_CardData.size() < 8*128))  // Its a 6-star card !
            {
            m_Increment = 0x80;
            m_Address += m_Increment;
            GetBackupData();
            return;
            }
        }

    rec.setSICard(QString::number(sinumber));
    rec.setFirstName(m_CardData.mid(68,20).trimmed());
    rec.setOtherName(m_CardData.mid(48,20).trimmed());

    QString cn, when, dow;
    Read4ByteControlData(m_CardData, 28, cn, dow, when, false);
    rec.setCheck(cn, dow, when);
    Read4ByteControlData(m_CardData, 32, cn, dow, when, false);
    rec.setClear(cn, dow, when);
    Read4ByteControlData(m_CardData, 24, cn, dow, when, true);
    rec.setStart(cn, dow, when);
    Read4ByteControlData(m_CardData, 20, cn, dow, when, true);
    rec.setFinish(cn, dow, when);
    rec.setClub(m_CardData.mid(96,32).trimmed());

    if (!rec.getBadRead())
        {
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
                Read4ByteControlData(m_CardData, offset, cn, dow, when, false);
                rec.setControl(i, cn, dow, when);
                }
            offset += 4;
            }
        }

    AddNewCard(rec);
    m_SI6++;
    GetNextBlock();
}

void CSIDumper::ProcessSICard6(QByteArray& a_Rec)
{
    // Need more data
    m_State = STATE_READING_CARD6;
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
                rec.setControl(i, cn, "", when);
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
    bool running4800 = (m_Port->baudRate() == BAUD4800);
    ClosePort();

    if (running4800)
        RestartWith38400();
}


void CSIDumper::Read4ByteControlData(QByteArray& a_Rec, int a_Offset, QString& a_Cn, QString& a_DOW, QString& a_When, bool a_Subsecond)
{
// record structure: PTD - CN - PTH - PTL

    unsigned char byte[4];
    for (int i = 0; i < 4; i++)
        byte[i] = a_Rec[a_Offset + i];

    // See if we have subsecond data in control number
    a_Subsecond = a_Subsecond && (byte[0] & 1);

    int cn = byte[1];

    a_Cn = QString("%1").arg(cn);

    // Calculate DOW
    int dow = (byte[0] & 0x70) >> 4;
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
    a_Card.setNo(m_AllCards.size() + 1);
    m_AllCards.push_back(a_Card);
    emit CardCsv(a_Card.getAsCsv());
    QString msg = QString("Reading card %1").arg(a_Card.getSICard());
    emit StatusUpdate(msg);
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
