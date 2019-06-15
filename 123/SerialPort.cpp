//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, §átickTick Group  
/// All rights reserved.
///
/// @file    SerialPort.cpp
/// @brief   
///
/// 
///
/// @version 1.0
/// @author  
/// @E-maillujun.hust@gmail.com
/// @date    2010/03/19
///
///
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SerialPort.h"
#include "mainwindow.h"
#include<QDebug>
#include <process.h>
#include <iostream>


/**  */
bool CSerialPort::s_bExit = false;
/** ,sleep¦Â,¦Ë: */
const UINT SLEEP_TIME_INTERVAL = 5;

CSerialPort::CSerialPort(void)
    : m_hListenThread(INVALID_HANDLE_VALUE)
{
    m_hComm = INVALID_HANDLE_VALUE;
    m_hListenThread = INVALID_HANDLE_VALUE;

    InitializeCriticalSection(&m_csCommunicationSync);
    over = 0;
    da = "";

}

CSerialPort::~CSerialPort(void)
{
    CloseListenTread();
    ClosePort();
    DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
    UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

    /** ,,DCB */
    char szDCBparam[50];
    sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

    /** ,¨², */
    if (!openPort(portNo))
    {
        return false;
    }

    /**  */
    EnterCriticalSection(&m_csCommunicationSync);

    /** §Õ */
    BOOL bIsSuccess = TRUE;

    /** §³,,.
    *  §³,§»,
    */
    /*if (bIsSuccess )
    {
    bIsSuccess = SetupComm(m_hComm,10,10);
    }*/

    /** ,0,¨® */
    COMMTIMEOUTS  CommTimeouts;
    CommTimeouts.ReadIntervalTimeout = 0;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 0;
    if (bIsSuccess)
    {
        bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
    }

    DCB  dcb;
    if (bIsSuccess)
    {
        // ANSIUNICODE
        DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
        wchar_t *pwText = new wchar_t[dwNum];
        if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
        {
            bIsSuccess = TRUE;
        }

        /** ¨°,DCB */
        bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
        /** RTS flow */
        dcb.fRtsControl = RTS_CONTROL_ENABLE;

        /**  */
        delete[] pwText;
    }

    if (bIsSuccess)
    {
        /** DCB */
        bIsSuccess = SetCommState(m_hComm, &dcb);
    }

    /**   */
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    /**  */
    LeaveCriticalSection(&m_csCommunicationSync);

    return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
    /** ,¨², */
    if (!openPort(portNo))
    {
        return false;
    }

    /**  */
    EnterCriticalSection(&m_csCommunicationSync);

    /**  */
    if (!SetCommState(m_hComm, plDCB))
    {
        return false;
    }

    /**   */
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    /**  */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

void CSerialPort::ClosePort()
{
    /** §Õ */
    if (m_hComm != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
    }
}

bool CSerialPort::openPort(UINT portNo)
{
    /**  */
    EnterCriticalSection(&m_csCommunicationSync);

    /** õô */
    char szPort[50];
    sprintf_s(szPort, "COM%d", portNo);

    /**  */
    m_hComm = CreateFileA(szPort,  /** õô,COM1,COM2 */
        GENERIC_READ | GENERIC_WRITE, /** ,§Õ */
        0,                            /** ,0 */
        NULL,                         /** ,NULL */
        OPEN_EXISTING,                /** ¨°õô, */
        0,
        0);

    /**  */
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        LeaveCriticalSection(&m_csCommunicationSync);
        return false;
    }

    /**  */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

bool CSerialPort::OpenListenThread()
{
    /**  */
    if (m_hListenThread != INVALID_HANDLE_VALUE)
    {
        /**  */
        return false;
    }

    s_bExit = false;
    /** ID */
    UINT threadId;
    /**  */
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
    if (!m_hListenThread)
    {
        return false;
    }
    /** , */
    if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
    {
        return false;
    }

    return true;
}

bool CSerialPort::CloseListenTread()
{
    if (m_hListenThread != INVALID_HANDLE_VALUE)
    {
        /**  */
        s_bExit = true;

        /**  */
        Sleep(10);

        /** §¹ */
        CloseHandle(m_hListenThread);
        m_hListenThread = INVALID_HANDLE_VALUE;
    }
    return true;
}

UINT CSerialPort::GetBytesInCOM()
{
    DWORD dwError = 0;  /**  */
    COMSTAT  comstat;   /** COMSTAT,õô */
    memset(&comstat, 0, sizeof(COMSTAT));

    UINT BytesInQue = 0;
    /** ReadFileWriteFile, */
    if (ClearCommError(m_hComm, &dwError, &comstat))
    {
        BytesInQue = comstat.cbInQue; /** §Ö */
    }

    return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
    /**  */
    CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

    // ,
    while (!pSerialPort->s_bExit)
    {
        UINT BytesInQue = pSerialPort->GetBytesInCOM();
        /** , */
        if (BytesInQue == 0)
        {
            Sleep(SLEEP_TIME_INTERVAL);
            continue;
        }

        /** §Ö */
        char cRecved = 0x00;
        do
        {
            cRecved = 0x00;
            if (pSerialPort->ReadChar(cRecved) == true)
            {
                //qDebug()<<cRecved;

                if(cRecved != '\n' && !pSerialPort->over )
                    pSerialPort->da +=cRecved;
                else
                    pSerialPort->over = 1;

                if(pSerialPort->over == 1)
                {
                    that->getAngle(pSerialPort->da);
                    //qDebug()<<pSerialPort->da<<"\n";
                    pSerialPort->da = "";
                    pSerialPort->over = 0;

                }

                std::cout << cRecved;
                continue;
            }
        } while (--BytesInQue);
    }

    return 0;
}

bool CSerialPort::ReadChar(char &cRecved)
{
    BOOL  bResult = TRUE;
    DWORD BytesRead = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    /**  */
    EnterCriticalSection(&m_csCommunicationSync);

    /**  */
    bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
    if ((!bResult))
    {
        /** , */
        DWORD dwError = GetLastError();

        /**  */
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
        LeaveCriticalSection(&m_csCommunicationSync);

        return false;
    }

    /**  */
    LeaveCriticalSection(&m_csCommunicationSync);

    return (BytesRead == 1);

}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
    BOOL   bResult = TRUE;
    DWORD  BytesToSend = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    /**  */
    EnterCriticalSection(&m_csCommunicationSync);

    /** §Õ */
    bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
    if (!bResult)
    {
        DWORD dwError = GetLastError();
        /**  */
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
        LeaveCriticalSection(&m_csCommunicationSync);

        return false;
    }

    /**  */
    LeaveCriticalSection(&m_csCommunicationSync);

    return true;
}

bool CSerialPort::BuffSend(unsigned char Num)
{
    unsigned char pData[3];

        //pData[0] = 0XF1;
        pData[0] =Num;     //
        pData[1] = '\r';
        pData[2] = '\n';
        /** 0XF1,¦Â0XAA */
        qDebug()<<"ini";
        if (!WriteData(pData, sizeof(pData)))
        {
            return false;
        }

    return true;
}
