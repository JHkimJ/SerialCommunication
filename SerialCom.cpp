//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <iostream>
#include <iomanip>
#include <sstream>
#include "SerialCom.h"
#include <windows.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

void __fastcall TempThread::UpdateLabel()
{
    lblTemp->Caption = message;
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
    tempThread->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	OpenSerialPort();
    SerialSet(); // DCB,Timeout Parameter
    tempThread = new TempThread(false, hComm, lblTemp);  // RealTime PV Recv
}
//---------------------------------------------------------------------------
void __fastcall TempThread::Execute()
{
	AnsiString txData;
    txData.sprintf("%c01010WRR01D0003%c%c", STX, ETX, CR);

    if (hComm == INVALID_HANDLE_VALUE)
    {
        message = _T("포트에 쓰는 중 오류가 발생했습니다.");
        return;
    }

    char rxData[14] = {0}; // rxData 버퍼
    DWORD bytesWritten, bytesRead;

    while (!Terminated)
    {
        // COM1에 Write 쓰기 (Transmit)
        if (WriteFile(hComm, txData.c_str(), txData.Length(), &bytesWritten, nullptr) == false)
        {
            // Error
            message = _T("포트에 쓰는 중 오류가 발생했습니다.");
            Synchronize(ShowMessage);
            return;
        }

        // COM1에 Read 하기 (Receive)
        if (ReadFile(hComm, rxData, sizeof(rxData) , &bytesRead, nullptr))
        {

            if (bytesRead > 0)
            {
//                rxData[bytesRead] = '\0'; // 추가된 코드
                std::string data(rxData);

                // substr 할 끝문자 위치 PV value
                size_t splitPos = data.find("OK");

                // 유효한 위치 확인
                if (splitPos != std::string::npos) // "OK" 가 없으면
                {
                    std::string secondPart = data.substr(splitPos + 2);
                    size_t endPos = secondPart.find("");
                    if (endPos != std::string::npos)
                    {
                    	// 값 추출 "OK"문자 뒤 HEX 값 갖고옴
                        std::string hexString = secondPart.substr(0, endPos);

                        // 16진수 값을 10진수 값으로 변환하고 10.0으로 나누기
                        unsigned int decimalValueInt;
                        sscanf(hexString.c_str(), "%x", &decimalValueInt);
                        double decimalValue = static_cast<double>(decimalValueInt) / 10.0;

                        // 소수점 1자리까지 표시하는 문자열로 10진수 값을 변환
                        std::ostringstream stream;
                        stream << std::fixed << std::setprecision(1) << decimalValue;
                        std::string decimalString = stream.str();

                        // 10진수 값을 message에 할당
                        message = decimalString.c_str();
                        // lblTemp 업데이트
                        Synchronize(UpdateLabel);
                    }
                }

            }
        }
        else
        {
            // Error reading from the port
            message = _T("포트에서 읽는 중 오류가 발생했습니다.");
            Synchronize(ShowMessage);
            CloseHandle(hComm);
            return;
        }
        Sleep(500); // 0.5초 딜레이
    }

}
//---------------------------------------------------------------------------
void TForm1::SerialSet()
{
	DCB dcbParams = { 0 };
    dcbParams.DCBlength = sizeof(dcbParams);
    COMMTIMEOUTS timeouts = { 0 };

    // Get the current settings
    if (!GetCommState(hComm, &dcbParams))
    {
        ShowMessage(_T("GetCommState 에러."));
        return;
    }

    // Change the settings
    dcbParams.BaudRate = CBR_9600;   // BaudRate = 9600
    dcbParams.ByteSize = 8;          // ByteSize = 8
    dcbParams.StopBits = ONESTOPBIT; // StopBits = 1
    dcbParams.Parity   = EVENPARITY; // Parity = Even

    // Apply the settings
    if (!SetCommState(hComm, &dcbParams))
    {
		ShowMessage(_T("SetCommState 에러."));
        return;
    }

    // Set the timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hComm, &timeouts))
    {
        ShowMessage(_T("SetCommTimeouts 에러."));
        return;
    }
}
void TForm1::OpenSerialPort()
{
    //  COM PORT Open
	hComm = CreateFile(L"COM3",
					   GENERIC_READ | GENERIC_WRITE,
                       NULL,
                       NULL,
                       OPEN_EXISTING,
                       NULL,
                       NULL);
//    if (hComm != INVALID_HANDLE_VALUE)
//    {
    HANDLE hNewComm = ReOpenFile(hComm,
    							 GENERIC_READ | GENERIC_WRITE,
                                 NULL,
                                 NULL);

        if (hNewComm != INVALID_HANDLE_VALUE)
        {
            ShowMessage(_T("포트를 여는중 오류 발생!."));
            CloseHandle(hComm);   // 이전 Handle 종료
            hComm = hNewComm;     // 새로운 Handle 사용
        }
//    }
}
void TForm1::CloseSerialPort()
{
    // Close the COM Port
    CloseHandle(hComm);
}
//---------------------------------------------------------------------------

