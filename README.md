# SerialCommunication UP55A PV읽어오기
장치통신 .
## UP55A, Win32 Api 사용

### CreateFile(); 핸들 에 COM# ,#:1~9 COM 포트 번호 지정
![image](https://github.com/JHkimJ/SerialCommunication/assets/138735492/4c2b9b9c-87f0-4343-aa3e-d32fa585bd84)
### WriteFile();  UP55A에 있는 Configuration of Commands Protocol에 따른 Transe Data 하기위한 설명
01010WRR01D0003
0x02 01 01 0 WRR 01 D0003 0X03 0X0D
0x02  = STX,
01  = Adr No.01~99,
01  = CPU No.(Fixed 01),
0   = Response waiting time 0~F(HEX),
WRR = Command , //Word-basis, Random Read
01  = 등록된 Register Size 지정,
D0003 = PV읽기위한 Register,
0X03  = ETX
0X0D  = CR

(1) STX (Start of Text)
This indicates the start of the command. ASCII code is 02 in hexadecimal.
(2) Address No. (01 to 99)
This No. is for enabling the host computer to identify the UTAdvanced at the 
communication destination. (These are the ID Nos. of each UTAdvanced.)
(3) CPU No.
Fixed at “01”
(4) Response waiting time
A waiting time (time delay) from transmission of a command up to reception of a 
response can be set.
Set the response waiting time within 0 to 600 ms using characters 0 to F
(5) Command (For a list of commands, see “3.2.6 Commands and Responses.”)
The instruction (command) from the host computer is specified.
(6) Data corresponding to command
Internal register (D register / I relay), number of data points, etc. are specified
(7) Checksum
The ASCII codes of text from the character following STX up to before the checksum 
are added one byte at a time. The lowermost byte of the addition result is removed, 
and an ASCII text string of two bytes expressed in Hex is taken to be the checksum.
This column is required only when performing PC link communication with a 
checksum.
When performing PC link communication without a checksum, this ASCII code 2-byte 
area is not required.
(8) ETX (End of Text)
This indicates the end of the command. ASCII code is 03 in hexadecimal.
(9) CR (Carriage Return)
This indicates the end of the command line. ASCII code is 0D in hexadecimal
### ReadFile(); RXD (Receive Data) 읽어오기
COM# 에 Transe(WriteFile) 해준걸 Read 해준다

### ReOpenFile();
기존핸들이 (장치)연결 해제 될때 Port 재연결시 기존핸들을 다시 잡을때 사용


