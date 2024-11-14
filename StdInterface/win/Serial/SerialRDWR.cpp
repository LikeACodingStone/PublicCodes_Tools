#include "pch.h"
#include "SerialRDWR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SerialRDWR::SerialRDWR()
{};

SerialRDWR::~SerialRDWR()
{};

bool SerialRDWR::open(int ComId, unsigned long baudRate, unsigned int dataBits, unsigned int parity, unsigned int stopBits)
{
    // シリアルポートをオープンする。
    TCHAR ComName[64];

    _stprintf_s(ComName, _T("COM%d"), ComId);

    m_hSerial = CreateFile(
        ComName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    // シリアルポートがオープンできない場合
    if (m_hSerial == INVALID_HANDLE_VALUE)
    {
        // 終了します。
        return FALSE;
    }

    // シリアルポートの通信条件を取得します。
    DCB dcb;
    ZeroMemory(&dcb, sizeof(dcb));
    if (!GetCommState(m_hSerial, &dcb))
    {
        // シリアルポートの通信条件が取得できない場合、終了します。
        CloseHandle(m_hSerial);
        m_hSerial = NULL;
        return FALSE;
    }

    // シリアル通信の入出力バッファをクリアします。
    PurgeComm(m_hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    // シリアルポートの通信条件を設定します。
    dcb.fBinary = TRUE;
    //dcb.BaudRate = 9600;
    dcb.BaudRate = baudRate;
    dcb.fParity = TRUE;
    //dcb.Parity = NOPARITY;
    //dcb.ByteSize = 8;
    //dcb.StopBits = ONESTOPBIT;
    if (parity == NOPARITY) {
        dcb.fParity = FALSE;
    }
    dcb.Parity = parity;
    dcb.ByteSize = dataBits;
    dcb.StopBits = stopBits;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fNull = FALSE;

    // シリアル通信の設定を行います。
    if (!SetCommState(m_hSerial, &dcb))
    {
        // シリアル通信の設定でエラーの場合、終了します。
        CloseHandle(m_hSerial);
        m_hSerial = NULL;
        return FALSE;
    }

    // シリアル通信のタイムアウトパラメータを取得します。
    COMMTIMEOUTS timeouts;
    ZeroMemory(&timeouts, sizeof(timeouts));
    if (!GetCommTimeouts(m_hSerial, &timeouts))
    {
        // シリアル通信のタイムアウトパラメータの取得でエラーの場合、終了します。
        CloseHandle(m_hSerial);
        m_hSerial = NULL;
        return FALSE;
    }

    // シリアル通信のタイムアウトパラメータを再設定します。
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 10;
    if (!SetCommTimeouts(m_hSerial, &timeouts))
    {
        // シリアル通信のタイムアウトパラメータの再設定でエラーの場合、終了します。
        CloseHandle(m_hSerial);
        m_hSerial = NULL;
        return FALSE;
    }

    // シリアル通信の拡張機能を設定します。
    EscapeCommFunction(m_hSerial, SETRTS);

    return TRUE;
}

bool SerialRDWR::close()
{
    CloseHandle(m_hSerial);
    m_hSerial = NULL;
	return true;
};

bool SerialRDWR::SendData(unsigned char* Data, unsigned char length)
{
    int writeBytes = 0;
    BOOL bRet = WriteFile(m_hSerial, Data, length, (LPDWORD)&writeBytes, NULL);
    return bRet;
}

int SerialRDWR::RecvData(unsigned char *Data)
{
    char achar = 0;
    DWORD dwRead = 0;
    int ReadDataNum = 0;
    while (TRUE)
    {
        BOOL bRet = ReadFile(m_hSerial, (LPVOID)&Data[ReadDataNum], 1, &dwRead, NULL);
        if (dwRead < 1)
            break;
        ReadDataNum += dwRead;
    }
    return ReadDataNum;
};



