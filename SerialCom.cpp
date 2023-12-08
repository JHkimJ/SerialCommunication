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
        message = _T("��Ʈ�� ���� �� ������ �߻��߽��ϴ�.");
        return;
    }

    char rxData[14] = {0}; // rxData ����
    DWORD bytesWritten, bytesRead;

    while (!Terminated)
    {
        // COM1�� Write ���� (Transmit)
        if (WriteFile(hComm, txData.c_str(), txData.Length(), &bytesWritten, nullptr) == false)
        {
            // Error
            message = _T("��Ʈ�� ���� �� ������ �߻��߽��ϴ�.");
            Synchronize(ShowMessage);
            return;
        }

        // COM1�� Read �ϱ� (Receive)
        if (ReadFile(hComm, rxData, sizeof(rxData) , &bytesRead, nullptr))
        {

            if (bytesRead > 0)
            {
//                rxData[bytesRead] = '\0'; // �߰��� �ڵ�
                std::string data(rxData);

                // substr �� ������ ��ġ PV value
                size_t splitPos = data.find("OK");

                // ��ȿ�� ��ġ Ȯ��
                if (splitPos != std::string::npos) // "OK" �� ������
                {
                    std::string secondPart = data.substr(splitPos + 2);
                    size_t endPos = secondPart.find("");
                    if (endPos != std::string::npos)
                    {
                    	// �� ���� "OK"���� �� HEX �� �����
                        std::string hexString = secondPart.substr(0, endPos);

                        // 16���� ���� 10���� ������ ��ȯ�ϰ� 10.0���� ������
                        unsigned int decimalValueInt;
                        sscanf(hexString.c_str(), "%x", &decimalValueInt);
                        double decimalValue = static_cast<double>(decimalValueInt) / 10.0;

                        // �Ҽ��� 1�ڸ����� ǥ���ϴ� ���ڿ��� 10���� ���� ��ȯ
                        std::ostringstream stream;
                        stream << std::fixed << std::setprecision(1) << decimalValue;
                        std::string decimalString = stream.str();

                        // 10���� ���� message�� �Ҵ�
                        message = decimalString.c_str();
                        // lblTemp ������Ʈ
                        Synchronize(UpdateLabel);
                    }
                }

            }
        }
        else
        {
            // Error reading from the port
            message = _T("��Ʈ���� �д� �� ������ �߻��߽��ϴ�.");
            Synchronize(ShowMessage);
            CloseHandle(hComm);
            return;
        }
        Sleep(500); // 0.5�� ������
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
        ShowMessage(_T("GetCommState ����."));
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
		ShowMessage(_T("SetCommState ����."));
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
        ShowMessage(_T("SetCommTimeouts ����."));
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
            ShowMessage(_T("��Ʈ�� ������ ���� �߻�!."));
            CloseHandle(hComm);   // ���� Handle ����
            hComm = hNewComm;     // ���ο� Handle ���
        }
//    }
}
void TForm1::CloseSerialPort()
{
    // Close the COM Port
    CloseHandle(hComm);
}
//---------------------------------------------------------------------------

