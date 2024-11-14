#pragma once
class SerialRDWR
{
public:
	SerialRDWR();
	~SerialRDWR();
	bool open(int ComId, unsigned long baudRate, unsigned int dataBits, unsigned int parity, unsigned int stopBits);
	bool close();
	bool SendData(unsigned char *Data, unsigned char length);
	int RecvData(unsigned char *Command);
	HANDLE m_hSerial = {};
};

