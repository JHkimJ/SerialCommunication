//---------------------------------------------------------------------------

#ifndef SerialComH
#define SerialComH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
const char STX = 0x02;
const char ETX = 0X03;
const char CR = 0X0D;

class TempThread : public TThread
{
private:
    HANDLE hComm;
    TLabel *lblTemp;
    AnsiString message;

    void __fastcall ShowMessage()
    {
    }
protected:
    void __fastcall Execute();
public:
	__fastcall TempThread(bool CreateSuspended, HANDLE hComm, TLabel *lblTemp);
	 void __fastcall UpdateLabel();
};

	__fastcall TempThread::TempThread(bool CreateSuspended, HANDLE hComm, TLabel *lblTemp)
: TThread(CreateSuspended), hComm(hComm), lblTemp(lblTemp)
{
};

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TLabel *lblTemp;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
    HANDLE hComm;
    TempThread *tempThread;
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
    __fastcall ~TForm1();
    void OpenSerialPort();
	void SerialSet();
    void CloseSerialPort();
    void Execute();


};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
